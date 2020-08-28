//
//  include/ADT/Mach-O/ObjcUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 6/13/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/BasicContiguousList.h"
#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"
#include "Utils/PointerUtils.h"

#include "BindUtil.h"
#include "DeVirtualizer.h"
#include "ObjcUtil.h"

namespace MachO {
    template <PointerKind Kind>
    using ClassTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           ObjcClass64,
                           ObjcClass>;

    template <PointerKind Kind>
    using ClassRoTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           ObjcClassRo64,
                           ObjcClassRo>;

    template <PointerKind Kind>
    using ClassCategoryTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           ObjcClassCategory64,
                           ObjcClassCategory>;

    ObjcClassInfo *
    CreateClassForList(
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        ObjcClassInfo &&Class,
        uint64_t ClassAddr = uint64_t()) noexcept
    {
        const auto Ptr =
            List.insert({
                ClassAddr,
                std::make_unique<ObjcClassInfo>(Class)
            }).first->second.get();

        return Ptr;
    }

    void
    SetSuperClassForClassInfo(ObjcClassInfo *Super,
                              ObjcClassInfo *Info) noexcept
    {
        if (Super != Info) {
            Super->AddChild(*Info);
        } else {
            Info->setSuper(nullptr);
        }
    }

    static inline std::string_view
    GetNameFromBindActionSymbol(const std::string &Symbol) noexcept {
        constexpr auto Prefix = std::string_view("_OBJC_CLASS_$_");
        if (Symbol.compare(0, Prefix.size(), Prefix) == 0) {
            return std::string_view(Symbol.data() + Prefix.length());
        }

        return Symbol;
    }

    void
    SetSuperWithBindAction(
        ObjcClassInfo *Info,
        const BindActionCollection::Info &Action,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList) noexcept
    {
        const auto ActionSymbol =
            GetNameFromBindActionSymbol(Action.getSymbol());

        const auto Pred = [&](const auto &Lhs) noexcept {
            if (Lhs->getDylibOrdinal() != Action.getDylibOrdinal()) {
                return false;
            }

            return (Lhs->getName() == ActionSymbol);
        };

        const auto Begin = ExternalAndRootClassList.cbegin();
        const auto End = ExternalAndRootClassList.cend();
        const auto Iter = std::find_if(Begin, End, Pred);

        if (Iter != End) {
            SetSuperClassForClassInfo(*Iter, Info);
            return;
        }

        auto SuperInfo = ObjcClassInfo(ActionSymbol);

        SuperInfo.setIsExternal();
        SuperInfo.setBindAddr(Action.getAddress());
        SuperInfo.setDylibOrdinal(Action.getDylibOrdinal());

        const auto Ptr =
            CreateClassForList(List, std::move(SuperInfo), Action.getAddress());

        SetSuperClassForClassInfo(Ptr, Info);
        ExternalAndRootClassList.emplace_back(Ptr);
    }

    template <PointerKind Kind>
    static ObjcParseError
    ParseObjcClass(uint64_t Addr,
                   const ConstDeVirtualizer &DeVirtualizer,
                   const BindActionCollection &BindCollection,
                   ObjcClassInfo &Info,
                   bool IsBigEndian) noexcept
    {
        using ClassType = ClassTypeCalculator<Kind>;
        using ClassRoType = ClassRoTypeCalculator<Kind>;

        const auto Class =
            DeVirtualizer.GetDataAtAddressIgnoreSections<ClassType>(Addr);

        if (Class == nullptr) {
            Info.setAddr(Addr);
            Info.setIsNull();

            return ObjcParseError::None;
        }

        const auto SuperAddr = Class->getSuperClassAddress(IsBigEndian);
        const auto RoAddr = Class->getData(IsBigEndian);
        const auto ClassRo =
            DeVirtualizer.GetDataAtAddressIgnoreSections<ClassRoType>(RoAddr);

        if (ClassRo == nullptr) {
            Info.setAddr(Addr);
            Info.setIsNull();

            return ObjcParseError::None;
        }

        const auto NameAddr = ClassRo->getNameAddress(IsBigEndian);
        if (auto String = DeVirtualizer.GetStringAtAddress(NameAddr)) {
            Info.setName(std::string(String.value()));
        }

        Info.setAddr(Addr);
        Info.setFlags(ClassRo->getFlags(IsBigEndian));
        Info.setIsSwift(Class->IsSwift(IsBigEndian));
        Info.setSuper(reinterpret_cast<ObjcClassInfo *>(SuperAddr | 1));

        return ObjcParseError::None;
    }

    static bool
    SetSuperIfExists(
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        ObjcClassInfo *Info,
        bool IsBigEndian) noexcept
    {
        const auto Addr = reinterpret_cast<uint64_t>(Info->getSuper()) & ~1;
        const auto Iter = List.find(Addr);

        if (Iter != List.cend()) {
            SetSuperClassForClassInfo(Iter->second.get(), Info);
            return true;
        }

        return false;
    }

    template <PointerKind Kind>
    static ObjcParseError
    FixSuperForClassInfo(
        ObjcClassInfo *Info,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        // BindAddr points to the `SuperClass` field inside ObjcClass[64]

        const auto BindAddr =
            Info->getAddress() +
            offsetof(ClassTypeCalculator<Kind>, SuperClass);

        if (const auto *Action = BindCollection.GetInfoForAddress(BindAddr)) {
            SetSuperWithBindAction(Info,
                                   *Action,
                                   List,
                                   ExternalAndRootClassList);

            return ObjcParseError::None;
        }

        const auto SuperAddr =
            reinterpret_cast<uint64_t>(Info->getSuper()) & ~1;

        if (SuperAddr == 0) {
            Info->setSuper(nullptr);
            ExternalAndRootClassList.emplace_back(Info);

            return ObjcParseError::None;
        }

        const auto DidFindSuper =
            SetSuperIfExists(DeVirtualizer,
                             BindCollection,
                             List,
                             Info,
                             IsBigEndian);

        if (DidFindSuper) {
            return ObjcParseError::None;
        }

        auto SuperInfo = ObjcClassInfo();
        const auto Error =
            ParseObjcClass<Kind>(SuperAddr,
                                 DeVirtualizer,
                                 BindCollection,
                                 SuperInfo,
                                 IsBigEndian);

        if (Error != ObjcParseError::None) {
            return Error;
        }

        const auto Ptr =
            CreateClassForList(List, std::move(SuperInfo), SuperAddr);

        SetSuperClassForClassInfo(Ptr, Info);
        const auto FixSuperError =
            FixSuperForClassInfo<Kind>(Ptr,
                                       DeVirtualizer,
                                       BindCollection,
                                       List,
                                       ExternalAndRootClassList,
                                       IsBigEndian);

        if (FixSuperError != ObjcParseError::None) {
            return FixSuperError;
        }

        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    HandleAddrForObjcClass(
        uint64_t Addr,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        bool IsBigEndian) noexcept
    {
        if (Addr == 0) {
            return ObjcParseError::None;
        }

        auto Info = ObjcClassInfo();

        const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
        const auto Error =
            ParseObjcClass<Kind>(SwitchedAddr,
                                 DeVirtualizer,
                                 BindCollection,
                                 Info,
                                 IsBigEndian);

        if (Error != ObjcParseError::None) {
            return Error;
        }

        CreateClassForList(List, std::move(Info), SwitchedAddr);
        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    FixSuperClassForClassList(
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        for (auto &Iter : List) {
            const auto &Info = Iter.second.get();
            const auto SuperAddr = reinterpret_cast<uint64_t>(Info->getSuper());

            if ((SuperAddr & 1) == 0) {
                continue;
            }

            if (Info->IsExternal()) {
                continue;
            }

            const auto Error =
                FixSuperForClassInfo<Kind>(Info,
                                           DeVirtualizer,
                                           BindCollection,
                                           List,
                                           ExternalAndRootClassList,
                                           IsBigEndian);

            if (Error != ObjcParseError::None) {
                return Error;
            }
        }

        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    ParseObjcClassListSection(
        const uint8_t *Begin,
        const uint8_t *End,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &ClassList,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        using PtrAddrType = PointerAddrConstTypeFromKind<Kind>;
        if (!IntegerIsPointerAligned<Kind>(End - Begin)) {
            return ObjcParseError::UnalignedSection;
        }

        const auto List = BasicContiguousList<PtrAddrType>(Begin, End);
        for (const auto &Addr : List) {
            const auto Error =
                HandleAddrForObjcClass<Kind>(Addr,
                                             DeVirtualizer,
                                             BindCollection,
                                             ClassList,
                                             IsBigEndian);

            if (Error != ObjcParseError::None) {
                return Error;
            }
        }

        FixSuperClassForClassList<Kind>(DeVirtualizer,
                                        BindCollection,
                                        ClassList,
                                        ExternalAndRootClassList,
                                        IsBigEndian);

        return ObjcParseError::None;
    }

    [[nodiscard]] static ObjcClassInfo
    CreateExternalClass(const std::string_view &Name,
                        uint64_t DylibOrdinal,
                        uint64_t BindAddr) noexcept
    {
        auto NewInfo = ObjcClassInfo();

        NewInfo.setName(std::string(Name));
        NewInfo.setDylibOrdinal(DylibOrdinal);

        NewInfo.setIsExternal();
        NewInfo.setBindAddr(BindAddr);

        return NewInfo;
    }

    template <PointerKind Kind>
    static ObjcParseError
    ParseObjcClassRefsSection(
        const uint8_t *Map,
        const SectionInfo *SectionInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &ClassList,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        using PointerAddrType = PointerAddrConstTypeFromKind<Kind>;

        const auto Begin = SectionInfo->getData(Map);
        const auto End = SectionInfo->getDataEnd(Map);

        if (!IntegerIsPointerAligned<Kind>(End - Begin)) {
            return ObjcParseError::UnalignedSection;
        }

        const auto List = BasicContiguousList<PointerAddrType>(Begin, End);
        auto ListAddr = SectionInfo->getMemoryRange().getBegin();

        for (const auto &Addr : List) {
            if (const auto *It = BindCollection.GetInfoForAddress(ListAddr)) {
                const auto Name = GetNameFromBindActionSymbol(It->getSymbol());
                auto NewInfo =
                    CreateExternalClass(Name, It->getDylibOrdinal(), ListAddr);
                const auto Ptr =
                    CreateClassForList(ClassList, std::move(NewInfo), ListAddr);

                ExternalAndRootClassList.emplace_back(Ptr);
            } else {
                const auto Error =
                    HandleAddrForObjcClass<Kind>(Addr,
                                                 DeVirtualizer,
                                                 BindCollection,
                                                 ClassList,
                                                 IsBigEndian);

                if (Error != ObjcParseError::None) {
                    return Error;
                }
            }

            ListAddr += PointerSize<Kind>();
        }

        FixSuperClassForClassList<Kind>(DeVirtualizer,
                                        BindCollection,
                                        ClassList,
                                        ExternalAndRootClassList,
                                        IsBigEndian);

        return ObjcParseError::None;
    }

    ObjcClassInfoCollection &
    ObjcClassInfoCollection::Parse(
        const uint8_t *Map,
        const SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        bool Is64Bit,
        bool IsBigEndian,
        Error *ErrorOut) noexcept
    {
        auto Error = ObjcParseError::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

        const auto AdjustExternalAndRootClassList = [&]() noexcept {
            assert(!ExternalAndRootClassList.empty() &&
                   "Objc Root-Class list is empty");

            if (ExternalAndRootClassList.size() != 1) {
                Root = CreateClassForList(List, Info());

                getRoot()->setIsNull();
                getRoot()->SetChildrenFromList(
                    reinterpret_cast<std::vector<BasicTreeNode *> &> (
                        ExternalAndRootClassList));
            } else {
                Root = ExternalAndRootClassList.front();
            }

            if (Error != Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error;
                }
            }
        };

        const auto ObjcClassRefsSection =
            SegmentCollection.FindSectionWithName({
                { "__OBJC2", { "__class_refs"     } },
                { "__DATA",  { "__objc_classrefs" } },
            });

        if (ObjcClassRefsSection != nullptr) {
            if (Is64Bit) {
                Error =
                    ParseObjcClassRefsSection<PointerKind::s64Bit>(
                        Map,
                        ObjcClassRefsSection,
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ParseObjcClassRefsSection<PointerKind::s32Bit>(
                        Map,
                        ObjcClassRefsSection,
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            }

            if (Error == Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error::None;
                }
            }

            AdjustExternalAndRootClassList();
            return *this;
        }

        const auto ObjcClassListSection =
            SegmentCollection.FindSectionWithName({
                { "__OBJC2",      { "__class_list"     } },
                { "__DATA_CONST", { "__objc_classlist" } },
                { "__DATA",       { "__objc_classlist" } },
                { "__DATA_DIRTY", { "__objc_classlist" } },
            });

        if (ObjcClassListSection != nullptr) {
            if (Is64Bit) {
                Error =
                    ParseObjcClassListSection<PointerKind::s64Bit>(
                        ObjcClassListSection->getData(Map),
                        ObjcClassListSection->getDataEnd(Map),
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ParseObjcClassListSection<PointerKind::s32Bit>(
                        ObjcClassListSection->getData(Map),
                        ObjcClassListSection->getDataEnd(Map),
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            }

            if (Error == Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error::None;
                }
            }

            AdjustExternalAndRootClassList();
            return *this;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error::NoObjcData;
        }

        return *this;
    }

    [[nodiscard]] static int
    GetBindCollection(const SegmentInfoCollection &SegmentCollection,
                      const BindActionList *BindList,
                      const LazyBindActionList *LazyBindList,
                      const WeakBindActionList *WeakBindList,
                      const LocationRange &Range,
                      BindActionCollection &CollectionOut,
                      BindOpcodeParseError *ParseErrorOut,
                      BindActionCollection::Error *CollectionErrorOut) noexcept
    {
        auto ParseError = BindOpcodeParseError::None;
        auto CollectionError = BindActionCollection::Error::None;

        CollectionOut =
            BindActionCollection::Open(SegmentCollection,
                                       BindList,
                                       LazyBindList,
                                       WeakBindList,
                                       Range,
                                       &ParseError,
                                       &CollectionError);

        if (ParseError != BindOpcodeParseError::None) {
            if (ParseErrorOut != nullptr) {
                *ParseErrorOut = ParseError;
            }

            return 1;
        }

        if (CollectionError != BindActionCollection::Error::None) {
            if (CollectionErrorOut != nullptr) {
                *CollectionErrorOut = CollectionError;
            }

            return 1;
        }

        return 0;
    }

    ObjcClassInfoCollection &
    ObjcClassInfoCollection::Parse(
        const ConstMemoryMap &Map,
        const SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionList *BindList,
        const LazyBindActionList *LazyBindList,
        const WeakBindActionList *WeakBindList,
        bool IsBigEndian,
        bool Is64Bit,
        Error *ErrorOut,
        BindOpcodeParseError *ParseErrorOut,
        BindActionCollection::Error *CollectionErrorOut) noexcept
    {
        auto Error = ObjcParseError::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

        const auto AdjustExternalAndRootClassList = [&]() noexcept {
            assert(!ExternalAndRootClassList.empty() &&
                   "Objc Root-Class list is empty");

            if (ExternalAndRootClassList.size() != 1) {
                Root = CreateClassForList(List, Info());

                getRoot()->setIsNull();
                getRoot()->SetChildrenFromList(
                    reinterpret_cast<std::vector<BasicTreeNode *> &> (
                        ExternalAndRootClassList));
            } else {
                Root = ExternalAndRootClassList.front();
            }

            if (Error != Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error;
                }
            }
        };

        const auto ObjcClassRefsSection =
            SegmentCollection.FindSectionWithName({
                { "__OBJC2", { "__class_refs"     } },
                { "__DATA",  { "__objc_classrefs" } },
            });

        if (ObjcClassRefsSection != nullptr) {
            const auto BindCollectionRange =
                ObjcClassRefsSection->getMemoryRange();

            auto BindCollection = BindActionCollection();
            const auto GetBindActionCollectionResult =
                GetBindCollection(SegmentCollection,
                                  BindList,
                                  LazyBindList,
                                  WeakBindList,
                                  BindCollectionRange,
                                  BindCollection,
                                  ParseErrorOut,
                                  CollectionErrorOut);

            if (GetBindActionCollectionResult != 0) {
                return *this;
            }

            if (Is64Bit) {
                Error =
                    ParseObjcClassRefsSection<PointerKind::s64Bit>(
                        Map.getBegin(),
                        ObjcClassRefsSection,
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ParseObjcClassRefsSection<PointerKind::s32Bit>(
                        Map.getBegin(),
                        ObjcClassRefsSection,
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            }

            if (Error == Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error::None;
                }
            }

            AdjustExternalAndRootClassList();
            return *this;
        }

        const auto ObjcClassListSection =
            SegmentCollection.FindSectionWithName({
                { "__OBJC2",      { "__class_list"     } },
                { "__DATA_CONST", { "__objc_classlist" } },
                { "__DATA",       { "__objc_classlist" } },
                { "__DATA_DIRTY", { "__objc_classlist" } },
            });

        if (ObjcClassListSection != nullptr) {
            const auto BindCollectionRange =
                ObjcClassListSection->getMemoryRange();

            auto BindCollection = BindActionCollection();
            const auto GetBindActionCollectionResult =
                GetBindCollection(SegmentCollection,
                                  BindList,
                                  LazyBindList,
                                  WeakBindList,
                                  BindCollectionRange,
                                  BindCollection,
                                  ParseErrorOut,
                                  CollectionErrorOut);

            if (GetBindActionCollectionResult != 0) {
                return *this;
            }

            if (Is64Bit) {
                Error =
                    ParseObjcClassListSection<PointerKind::s64Bit>(
                        ObjcClassListSection->getData(Map.getBegin()),
                        ObjcClassListSection->getDataEnd(Map.getBegin()),
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ParseObjcClassListSection<PointerKind::s32Bit>(
                        ObjcClassListSection->getData(Map.getBegin()),
                        ObjcClassListSection->getDataEnd(Map.getBegin()),
                        DeVirtualizer,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            }

            if (Error == Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error::None;
                }
            }

            AdjustExternalAndRootClassList();
            return *this;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error::NoObjcData;
        }

        return *this;
    }

    std::vector<ObjcClassInfo *>
    ObjcClassInfoCollection::GetAsList() const noexcept {
        auto Result = std::vector<Info *>();
        Result.reserve(List.size());

        for (const auto &Info : List) {
            Result.emplace_back(Info.second.get());
        }

        return Result;
    }

    ObjcClassInfo *
    ObjcClassInfoCollection::AddNullClass(uint64_t Address) noexcept {
        auto NewInfo = Info();

        NewInfo.setAddr(Address);
        NewInfo.setIsNull();

        return CreateClassForList(List, std::move(NewInfo), Address);
    }

    ObjcClassInfo *
    ObjcClassInfoCollection::AddExternalClass(const std::string_view &Name,
                                              uint64_t DylibOrdinal,
                                              uint64_t BindAddr) noexcept
    {
        auto NewInfo = CreateExternalClass(Name, DylibOrdinal, BindAddr);
        return CreateClassForList(List, std::move(NewInfo), BindAddr);
    }

    ObjcClassInfo *
    ObjcClassInfoCollection::GetInfoForAddress(uint64_t Address) const noexcept
    {
        const auto Iter = List.find(Address);
        if (Iter != List.end()) {
            return Iter->second.get();
        }

        return nullptr;
    }

    ObjcClassInfo *
    ObjcClassInfoCollection::GetInfoForClassName(
        const std::string_view &Name) const noexcept
    {
        for (const auto &Iter : List) {
            const auto &Info = Iter.second;
            if (Info->getName() == Name) {
                return Info.get();
            }
        }

        return nullptr;
    }

    ObjcClassInfoCollection::Iterator
    ObjcClassInfoCollection::begin() const noexcept {
        return Iterator(getRoot());
    }

    ObjcClassInfoCollection::Iterator
    ObjcClassInfoCollection::end() const noexcept {
        return Iterator(nullptr);
    }

    ObjcClassInfoCollection::ConstIterator
    ObjcClassInfoCollection::cbegin() const noexcept {
        return ConstIterator(getRoot());
    }

    ObjcClassInfoCollection::ConstIterator
    ObjcClassInfoCollection::cend() const noexcept {
        return ConstIterator(nullptr);
    }

    template <PointerKind Kind>
    ObjcParseError ParseObjcClassCategorySection(
        const uint8_t *Map,
        const SectionInfo *SectInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        ObjcClassInfoCollection *ClassInfoTree,
        std::vector<std::unique_ptr<ObjcClassCategoryInfo>> &CategoryList,
        bool IsBigEndian) noexcept
    {
        using PtrAddrType = PointerAddrConstTypeFromKind<Kind>;
        using ObjcCategoryType = ClassCategoryTypeCalculator<Kind>;

        auto Begin = SectInfo->getData(Map);
        auto End = SectInfo->getDataEnd(Map);

        const auto List = BasicContiguousList<PtrAddrType>(Begin, End);
        auto ListAddr = SectInfo->getMemoryRange().getBegin();

        if (ClassInfoTree != nullptr) {
            for (const auto &Addr : List) {
                auto Info = std::make_unique<ObjcClassCategoryInfo>();

                const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtAddressIgnoreSections<
                        ObjcCategoryType>(SwitchedAddr);

                if (Category == nullptr) {
                    Info->setAddress(SwitchedAddr);
                    Info->setIsNull();

                    CategoryList.emplace_back(std::move(Info));
                    continue;
                }

                const auto NameAddr = Category->getNameAddress(IsBigEndian);
                const auto Name = DeVirtualizer.GetStringAtAddress(NameAddr);

                if (Name.has_value()) {
                    Info->setName(std::string(Name.value()));
                }

                // ClassAddr initially points to the 'Class' field that (may)
                // get binded.

                auto Class = static_cast<ObjcClassInfo *>(nullptr);
                auto ClassAddr =
                    SwitchedAddr +
                    offsetof(ClassCategoryTypeCalculator<Kind>, Class);

                if (auto *It = BindCollection.GetInfoForAddress(ClassAddr)) {
                    const auto Name =
                        GetNameFromBindActionSymbol(It->getSymbol());

                    Class = ClassInfoTree->GetInfoForClassName(Name);
                    if (Class == nullptr) {
                        Class =
                            ClassInfoTree->AddExternalClass(
                                Name,
                                It->getDylibOrdinal(),
                                It->getAddress());
                    }
                } else {
                    ClassAddr = Category->getClassAddress(IsBigEndian);
                    if (ClassAddr != 0) {
                        Class = ClassInfoTree->GetInfoForAddress(ClassAddr);
                        if (Class == nullptr) {
                            Class = ClassInfoTree->AddNullClass(ClassAddr);
                        }

                        Class->getCategoryList().emplace_back(Info.get());
                    }
                }

                Info->setAddress(SwitchedAddr);
                Info->setClass(Class);

                CategoryList.emplace_back(std::move(Info));
                ListAddr += PointerSize<Kind>();
            }
        } else {
            for (const auto &Addr : List) {
                auto Info = std::make_unique<ObjcClassCategoryInfo>();

                auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtAddressIgnoreSections<
                        ObjcCategoryType>(SwitchedAddr);

                if (Category == nullptr) {
                    Info->setAddress(SwitchedAddr);
                    Info->setIsNull();

                    CategoryList.emplace_back(std::move(Info));
                    continue;
                }

                const auto NameAddr = Category->getNameAddress(IsBigEndian);
                const auto Name = DeVirtualizer.GetStringAtAddress(NameAddr);

                if (Name.has_value()) {
                    Info->setName(std::string(Name.value()));
                }

                // ClassAddr initially points to the 'Class' field that (may)
                // get binded.

                auto ClassAddr =
                    SwitchedAddr +
                    offsetof(ClassCategoryTypeCalculator<Kind>, Class);

                if (auto *It = BindCollection.GetInfoForAddress(ClassAddr)) {
                    SwitchedAddr = static_cast<PtrAddrType>(It->getAddress());
                } else {
                    ClassAddr = Category->getClassAddress(IsBigEndian);
                }

                Info->setAddress(SwitchedAddr);
                CategoryList.emplace_back(std::move(Info));

                ListAddr += PointerSize<Kind>();
            }
        }

        return ObjcParseError::None;
    }

    ObjcClassCategoryCollection &
    ObjcClassCategoryCollection::CollectFrom(
        const uint8_t *Map,
        const SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        ObjcClassInfoCollection *ClassInfoTree,
        bool IsBigEndian,
        bool Is64Bit,
        Error *ErrorOut) noexcept
    {
        const auto ObjcClassCategorySection =
            SegmentCollection.FindSectionWithName({
                { "__DATA_CONST",  { "__objc_catlist" } },
                { "__DATA",        { "__objc_catlist" } },
            });

        if (ObjcClassCategorySection == nullptr) {
            if (ErrorOut != nullptr) {
                *ErrorOut = Error::NoObjcData;
            }

            return *this;
        }

        if (Is64Bit) {
            ParseObjcClassCategorySection<PointerKind::s64Bit>(
                Map,
                ObjcClassCategorySection,
                DeVirtualizer,
                BindCollection,
                ClassInfoTree,
                List,
                IsBigEndian);
        } else {
            ParseObjcClassCategorySection<PointerKind::s32Bit>(
                Map,
                ObjcClassCategorySection,
                DeVirtualizer,
                BindCollection,
                ClassInfoTree,
                List,
                IsBigEndian);
        }

        return *this;
    }

    ObjcClassCategoryCollection &
    ObjcClassCategoryCollection::CollectFrom(
        const ConstMemoryMap &Map,
        const SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        ObjcClassInfoCollection *ClassInfoTree,
        const BindActionList *BindList,
        const LazyBindActionList *LazyBindList,
        const WeakBindActionList *WeakBindList,
        bool IsBigEndian,
        bool Is64Bit,
        Error *ErrorOut,
        BindOpcodeParseError *ParseErrorOut,
        BindActionCollection::Error *CollectionErrorOut) noexcept
    {
        const auto ObjcClassCategorySection =
            SegmentCollection.FindSectionWithName({
                { "__DATA_CONST",  { "__objc_catlist" } },
                { "__DATA",        { "__objc_catlist" } },
            });

        if (ObjcClassCategorySection == nullptr) {
            if (ErrorOut != nullptr) {
                *ErrorOut = Error::NoObjcData;
            }

            return *this;
        }

        const auto BindCollectionRange =
            ObjcClassCategorySection->getMemoryRange();

        auto BindCollection = BindActionCollection();
        const auto GetBindActionCollectionResult =
            GetBindCollection(SegmentCollection,
                              BindList,
                              LazyBindList,
                              WeakBindList,
                              BindCollectionRange,
                              BindCollection,
                              ParseErrorOut,
                              CollectionErrorOut);

        if (GetBindActionCollectionResult != 0) {
            return *this;
        }

        if (Is64Bit) {
            ParseObjcClassCategorySection<PointerKind::s64Bit>(
                Map.getBegin(),
                ObjcClassCategorySection,
                DeVirtualizer,
                BindCollection,
                ClassInfoTree,
                List,
                IsBigEndian);
        } else {
            ParseObjcClassCategorySection<PointerKind::s32Bit>(
                Map.getBegin(),
                ObjcClassCategorySection,
                DeVirtualizer,
                BindCollection,
                ClassInfoTree,
                List,
                IsBigEndian);
        }

        return *this;
    }
}

