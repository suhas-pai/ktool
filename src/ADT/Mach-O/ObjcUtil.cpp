//
//  include/ADT/Mach-O/ObjcUtil.cpp
//  stool
//
//  Created by Suhas Pai on 6/13/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/BasicContiguousList.h"
#include "Utils/PointerUtils.h"

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
        std::unordered_map<uint64_t, ObjcClassInfo *> &List,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList) noexcept
    {
        const auto ActionSymbol = GetNameFromBindActionSymbol(*Action.Symbol);
        const auto Pred = [&](const auto &Lhs) noexcept {
            if (Lhs->DylibOrdinal != Action.DylibOrdinal) {
                return false;
            }

            return (Lhs->Name == ActionSymbol);
        };

        const auto Begin = ExternalAndRootClassList.cbegin();
        const auto End = ExternalAndRootClassList.cend();
        const auto Iter = std::find_if(Begin, End, Pred);

        if (Iter != End) {
            SetSuperClassForClassInfo(*Iter, Info);
            return;
        }

        const auto SuperInfo = new ObjcClassInfo(ActionSymbol);

        SuperInfo->BindAddr = Action.Address;
        SuperInfo->DylibOrdinal = Action.DylibOrdinal;
        SuperInfo->IsExternal = true;

        assert(SuperInfo->DylibOrdinal != 0);

        SetSuperClassForClassInfo(SuperInfo, Info);
        ExternalAndRootClassList.emplace_back(SuperInfo);
    }

    template <PointerKind Kind>
    static ObjcClassInfoTree::Error
    ParseObjcClass(uint64_t Addr,
                   const MachO::ConstDeVirtualizer &DeVirtualizer,
                   const MachO::BindActionCollection &BindCollection,
                   ObjcClassInfo &Info,
                   bool IsBigEndian) noexcept
    {
        using ClassType = ClassTypeCalculator<Kind>;
        using ClassRoType = ClassRoTypeCalculator<Kind>;

        const auto Class =
            DeVirtualizer.GetDataAtAddressIgnoreSections<ClassType>(Addr);

        if (Class == nullptr) {
            Info.Addr = Addr;
            Info.IsNull = true;

            return ObjcClassInfoTree::Error::None;
        }

        const auto SuperAddr = SwitchEndianIf(Class->SuperClass, IsBigEndian);
        const auto RoAddr = SwitchEndianIf(Class->Data, IsBigEndian);
        const auto ClassRo =
            DeVirtualizer.GetDataAtAddressIgnoreSections<ClassRoType>(RoAddr);

        if (ClassRo == nullptr) {
            Info.Addr = Addr;
            Info.IsNull = true;

            return ObjcClassInfoTree::Error::None;
        }

        const auto NameAddr = SwitchEndianIf(ClassRo->Name, IsBigEndian);
        if (auto String = DeVirtualizer.GetStringAtAddress(NameAddr)) {
            Info.Name = String.value();
        }

        const auto Bits = SwitchEndianIf(Class->Data, IsBigEndian);
        const auto Flags = ClassRo->getFlags(IsBigEndian);
        const auto IsSwiftMask =
            (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

        Info.Addr = Addr;
        Info.DylibOrdinal = 0;
        Info.Flags = Flags;
        Info.IsSwift = (Bits & IsSwiftMask);
        Info.setSuper(reinterpret_cast<ObjcClassInfo *>(SuperAddr));

        return ObjcClassInfoTree::Error::None;
    }

    static bool
    SetSuperIfExists(const ConstDeVirtualizer &DeVirtualizer,
                     const BindActionCollection &BindCollection,
                     std::unordered_map<uint64_t, ObjcClassInfo *> &List,
                     ObjcClassInfo *Info,
                     bool IsBigEndian) noexcept
    {
        const auto Addr = reinterpret_cast<uint64_t>(Info->getParent());
        const auto Iter = List.find(Addr);

        if (Iter != List.cend()) {
            SetSuperClassForClassInfo(Iter->second, Info);
            return true;
        }

        return false;
    }

    template <PointerKind Kind>
    static ObjcParseError
    FixSuperForClassInfo(ObjcClassInfo *Info,
                         const ConstDeVirtualizer &DeVirtualizer,
                         const BindActionCollection &BindCollection,
                         std::unordered_map<uint64_t, ObjcClassInfo *> &List,
                         std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
                         bool IsBigEndian) noexcept
    {
        // BindAddr points to the superclass field inside ObjcClass[64]

        const auto BindAddr = Info->Addr + PointerSize<Kind>();
        if (const auto *Action = BindCollection.GetInfoForAddress(BindAddr)) {
            SetSuperWithBindAction(Info,
                                   *Action,
                                   List,
                                   ExternalAndRootClassList);

            return ObjcParseError::None;
        }

        if (Info->getSuper() == nullptr) {
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

        auto SuperInfo = new ObjcClassInfo();

        const auto Addr = reinterpret_cast<uint64_t>(Info->getSuper());
        const auto Error =
            ParseObjcClass<PointerKind::s64Bit>(Addr,
                                                DeVirtualizer,
                                                BindCollection,
                                                *SuperInfo,
                                                IsBigEndian);

        if (Error != ObjcParseError::None) {
            delete SuperInfo;
            return Error;
        }

        SetSuperClassForClassInfo(SuperInfo, Info);
        ExternalAndRootClassList.emplace_back(SuperInfo);

        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    HandleAddrForObjcClass(uint64_t Addr,
                           const ConstDeVirtualizer &DeVirtualizer,
                           const BindActionCollection &BindCollection,
                           std::unordered_map<uint64_t, ObjcClassInfo *> &List,
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

        List.insert({ Info.Addr, new ObjcClassInfo(std::move(Info)) });
        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    FixSuperClassForClassList(
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, ObjcClassInfo *> &List,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        for (auto &Iter : List) {
            const auto &Info = Iter.second;
            if (Info->IsExternal) {
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
        std::unordered_map<uint64_t, ObjcClassInfo *> &ClassList,
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

    ObjcClassInfo *
    CreateExternalClass(const std::string_view &Name,
                        uint64_t DylibOrdinal,
                        uint64_t BindAddr) noexcept
    {
        assert(DylibOrdinal != 0);
        const auto NewInfo = new ObjcClassInfo();

        NewInfo->Name = Name;
        NewInfo->DylibOrdinal = DylibOrdinal;
        NewInfo->BindAddr = BindAddr;
        NewInfo->IsExternal = true;

        return NewInfo;
    }

    template <PointerKind Kind>
    static ObjcParseError
    ParseObjcClassRefsSection(
        const uint8_t *Map,
        const MachO::SectionInfo *SectionInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, ObjcClassInfo *> &ClassList,
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
        auto ListAddr = SectionInfo->Memory.getBegin();

        for (const auto &Addr : List) {
            if (const auto *It = BindCollection.GetInfoForAddress(ListAddr)) {
                const auto Name = GetNameFromBindActionSymbol(*It->Symbol);
                const auto NewInfo =
                    CreateExternalClass(Name, It->DylibOrdinal, It->Address);

                ClassList.insert({ NewInfo->Addr, NewInfo });
                ExternalAndRootClassList.emplace_back(NewInfo);
            } else {
                const auto Error =
                    HandleAddrForObjcClass<Kind>(Addr,
                                                 DeVirtualizer,
                                                 BindCollection,
                                                 ClassList,
                                                 IsBigEndian);

                if (Error != ObjcClassInfoTree::Error::None) {
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

    ObjcClassInfoTree &
    ObjcClassInfoTree::Parse(
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

            for (const auto &Class : ExternalAndRootClassList) {
                List.insert({ Class->BindAddr, Class });
            }

            if (ExternalAndRootClassList.size() != 1) {
                Root = new ObjcClassInfo();

                getRoot()->IsNull = true;
                getRoot()->SetChildrenFromList(
                    reinterpret_cast<std::vector<BasicTreeNode *> &> (
                        ExternalAndRootClassList));
            } else {
                Root = ExternalAndRootClassList.front();
            }

            Root->SetParentOfChildren();
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

    std::vector<ObjcClassInfo *> ObjcClassInfoTree::GetAsList() const noexcept {
        auto Result = std::vector<ObjcClassInfo *>();
        Result.reserve(List.size());

        for (const auto &Info : List) {
            Result.emplace_back(Info.second);
        }

        return Result;
    }

    ObjcClassInfo *ObjcClassInfoTree::AddNullClass(uint64_t Address) noexcept {
        const auto NewInfo = new ObjcClassInfo();

        NewInfo->Addr = Address;
        NewInfo->IsNull = true;

        List.insert({ Address, NewInfo });
        return NewInfo;
    }

    ObjcClassInfo *
    ObjcClassInfoTree::AddExternalClass(const std::string_view &Name,
                                        uint64_t DylibOrdinal,
                                        uint64_t BindAddr) noexcept
    {
        const auto NewInfo = CreateExternalClass(Name, DylibOrdinal, BindAddr);
        List.insert({ NewInfo->BindAddr, NewInfo });

        return NewInfo;
    }

    ObjcClassInfo *
    ObjcClassInfoTree::GetInfoForAddress(uint64_t Address) const noexcept {
        const auto Iter = List.find(Address);
        if (Iter != List.end()) {
            return Iter->second;
        }

        return nullptr;
    }

    ObjcClassInfo *
    ObjcClassInfoTree::GetInfoForClassName(
        const std::string_view &Name) const noexcept
    {
        for (const auto &Iter : List) {
            const auto &Info = Iter.second;
            if (Info->Name == Name) {
                return Info;
            }
        }

        return nullptr;
    }

    ObjcClassInfoTree::Iterator ObjcClassInfoTree::begin() const noexcept {
        return Iterator(getRoot());
    }

    ObjcClassInfoTree::Iterator ObjcClassInfoTree::end() const noexcept {
        return Iterator(nullptr);
    }

    ObjcClassInfoTree::ConstIterator
    ObjcClassInfoTree::cbegin() const noexcept {
        return ConstIterator(getRoot());
    }

    ObjcClassInfoTree::ConstIterator
    ObjcClassInfoTree::cend() const noexcept {
        return ConstIterator(nullptr);
    }

    template <PointerKind Kind>
    ObjcParseError ParseObjcClassCategorySection(
        const uint8_t *Map,
        const MachO::SectionInfo *SectInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection &BindCollection,
        ObjcClassInfoTree *ClassInfoTree,
        std::vector<ObjcClassCategoryInfo *> &CategoryList,
        bool IsBigEndian) noexcept
    {
        using PtrAddrType = PointerAddrConstTypeFromKind<Kind>;
        using ObjcCategoryType = ClassCategoryTypeCalculator<Kind>;

        auto Begin = SectInfo->getData(Map);
        auto End = SectInfo->getDataEnd(Map);

        const auto List = BasicContiguousList<PtrAddrType>(Begin, End);
        auto ListAddr = SectInfo->Memory.getBegin();

        if (ClassInfoTree != nullptr) {
            for (const auto &Addr : List) {
                auto Info = new ObjcClassCategoryInfo();

                const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtAddressIgnoreSections<
                        ObjcCategoryType>(SwitchedAddr);

                const auto NameAddr =
                    SwitchEndianIf(Category->Name, IsBigEndian);

                const auto Name = DeVirtualizer.GetStringAtAddress(NameAddr);
                if (Name.has_value()) {
                    Info->Name = Name.value();
                }

                auto ClassAddr = Addr + PointerSize<Kind>();
                auto Class = static_cast<ObjcClassInfo *>(nullptr);

                if (auto *It = BindCollection.GetInfoForAddress(ClassAddr)) {
                    const auto Name = GetNameFromBindActionSymbol(*It->Symbol);

                    Class = ClassInfoTree->GetInfoForClassName(Name);
                    if (Class == nullptr) {
                        Class =
                            ClassInfoTree->AddExternalClass(Name,
                                                            It->DylibOrdinal,
                                                            It->Address);
                    }
                } else {
                    ClassAddr = SwitchEndianIf(Category->Class, IsBigEndian);
                    Class = ClassInfoTree->GetInfoForAddress(Addr);

                    if (Class == nullptr) {
                        Class = ClassInfoTree->AddNullClass(ClassAddr);
                    }
                }

                Class->CategoryList.emplace_back(Info);

                Info->Address = SwitchedAddr;
                Info->Class = Class;

                ListAddr += PointerSize<Kind>();
                CategoryList.emplace_back(Info);
            }
        } else {
            for (const auto &Addr : List) {
                auto Info = new ObjcClassCategoryInfo();

                auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtAddressIgnoreSections<
                        ObjcCategoryType>(SwitchedAddr);

                if (Category == nullptr) {
                    Info->Address = SwitchedAddr;
                    Info->IsNull = true;

                    CategoryList.emplace_back(Info);
                    continue;
                }

                const auto NameAddr =
                    SwitchEndianIf(Category->Name, IsBigEndian);

                const auto Name = DeVirtualizer.GetStringAtAddress(NameAddr);
                if (Name.has_value()) {
                    Info->Name = Name.value();
                }

                auto ClassAddr = Addr + PointerSize<Kind>();
                if (auto *It = BindCollection.GetInfoForAddress(ClassAddr)) {
                    SwitchedAddr = static_cast<PtrAddrType>(It->Address);
                } else {
                    ClassAddr = SwitchEndianIf(Category->Class, IsBigEndian);
                }

                Info->Address = SwitchedAddr;
                Info->Address = SwitchedAddr;

                ListAddr += PointerSize<Kind>();
                CategoryList.emplace_back(Info);
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
        ObjcClassInfoTree *ClassInfoTree,
        bool Is64Bit,
        bool IsBigEndian,
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
}

