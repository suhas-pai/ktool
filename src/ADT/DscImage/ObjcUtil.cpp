//
//  src/ADT/DscImage/ObjcUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/19/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/BasicContiguousList.h"
#include "ObjcUtil.h"

namespace DscImage {
    template <PointerKind Kind>
    using ClassTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           const MachO::ObjcClass64,
                           const MachO::ObjcClass>;

    template <PointerKind Kind>
    using ClassRoTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           const MachO::ObjcClassRo64,
                           const MachO::ObjcClassRo>;

    template <PointerKind Kind>
    using ClassCategoryTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           const MachO::ObjcClassCategory64,
                           const MachO::ObjcClassCategory>;

    using ObjcParseError = MachO::ObjcParseError;
    using ObjcClassInfo = MachO::ObjcClassInfo;

    ObjcClassInfo *
    CreateClassForList(
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        ObjcClassInfo &&Class) noexcept
    {
        const auto Ptr =
            List.insert({
                Class.Addr,
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
        const MachO::BindActionCollection::Info &Action,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
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

        auto SuperInfo = ObjcClassInfo(ActionSymbol);

        SuperInfo.BindAddr = Action.Address;
        SuperInfo.DylibOrdinal = Action.DylibOrdinal;
        SuperInfo.IsExternal = true;

        SetSuperClassForClassInfo(&SuperInfo, Info);

        const auto Ptr = CreateClassForList(List, std::move(SuperInfo));
        ExternalAndRootClassList.emplace_back(Ptr);
    }

    template <PointerKind Kind>
    static ObjcParseError
    ParseObjcClass(uint64_t Addr,
                   const ConstDeVirtualizer &DeVirtualizer,
                   const MachO::BindActionCollection &BindCollection,
                   ObjcClassInfo &Info,
                   bool IsBigEndian) noexcept
    {
        using ClassType = ClassTypeCalculator<Kind>;
        using ClassRoType = ClassRoTypeCalculator<Kind>;

        const auto Class = DeVirtualizer.GetDataAtVmAddr<ClassType>(Addr);
        if (Class == nullptr) {
            Info.Addr = Addr;
            Info.IsNull = true;

            return ObjcParseError::None;
        }

        const auto SuperAddr = SwitchEndianIf(Class->SuperClass, IsBigEndian);
        const auto RoAddr = SwitchEndianIf(Class->Data, IsBigEndian);
        const auto ClassRo = DeVirtualizer.GetDataAtVmAddr<ClassRoType>(RoAddr);

        if (ClassRo == nullptr) {
            Info.Addr = Addr;
            Info.IsNull = true;

            return ObjcParseError::None;
        }

        const auto NameAddr = SwitchEndianIf(ClassRo->Name, IsBigEndian);
        if (auto String = DeVirtualizer.GetStringAtAddress(NameAddr)) {
            Info.Name = String.value();
        }

        Info.Addr = Addr;
        Info.Flags = ClassRo->getFlags(IsBigEndian);
        Info.IsSwift = Class->IsSwift(IsBigEndian);
        Info.setSuper(reinterpret_cast<ObjcClassInfo *>(SuperAddr));

        return ObjcParseError::None;
    }

    static bool
    SetSuperIfExists(
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        ObjcClassInfo *Info,
        bool IsBigEndian) noexcept
    {
        const auto Addr = reinterpret_cast<uint64_t>(Info->getParent());
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
        const MachO::BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
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

        auto SuperInfo = ObjcClassInfo();

        const auto Addr = reinterpret_cast<uint64_t>(Info->getSuper());
        const auto Error =
            ParseObjcClass<PointerKind::s64Bit>(Addr,
                                                DeVirtualizer,
                                                BindCollection,
                                                SuperInfo,
                                                IsBigEndian);

        if (Error != ObjcParseError::None) {
            return Error;
        }

        SetSuperClassForClassInfo(&SuperInfo, Info);

        const auto Ptr = CreateClassForList(List, std::move(SuperInfo));
        ExternalAndRootClassList.emplace_back(Ptr);

        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    HandleAddrForObjcClass(
        uint64_t Addr,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
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

        CreateClassForList(List, std::move(Info));
        return ObjcParseError::None;
    }

    template <PointerKind Kind>
    static ObjcParseError
    FixSuperClassForClassList(
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        for (auto &Iter : List) {
            const auto &Info = Iter.second.get();
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
        const MachO::BindActionCollection &BindCollection,
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

    ObjcClassInfo
    CreateExternalClass(const std::string_view &Name,
                        uint64_t DylibOrdinal,
                        uint64_t BindAddr) noexcept
    {
        auto NewInfo = ObjcClassInfo();

        NewInfo.Name = Name;
        NewInfo.DylibOrdinal = DylibOrdinal;
        NewInfo.BindAddr = BindAddr;
        NewInfo.IsExternal = true;

        return NewInfo;
    }

    template <PointerKind Kind>
    static ObjcParseError
    ParseObjcClassRefsSection(
        const uint8_t *Map,
        const MachO::SectionInfo *SectionInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
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
        auto ListAddr = SectionInfo->Memory.getBegin();

        for (const auto &Addr : List) {
            if (const auto *It = BindCollection.GetInfoForAddress(ListAddr)) {
                const auto Name = GetNameFromBindActionSymbol(*It->Symbol);
                auto NewInfo =
                    CreateExternalClass(Name, It->DylibOrdinal, It->Address);
                const auto Ptr =
                    CreateClassForList(ClassList, std::move(NewInfo));

                ExternalAndRootClassList.emplace_back(Ptr);
            } else {
                const auto Error =
                    HandleAddrForObjcClass<Kind>(Addr,
                                                 DeVirtualizer,
                                                 BindCollection,
                                                 ClassList,
                                                 IsBigEndian);

                if (Error != ObjcClassInfoCollection::Error::None) {
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
        const MachO::SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
        bool Is64Bit,
        bool IsBigEndian,
        Error *ErrorOut) noexcept
    {
        auto Error = MachO::ObjcParseError::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

        const auto AdjustExternalAndRootClassList = [&]() noexcept {
            assert(!ExternalAndRootClassList.empty() &&
                   "Objc Root-Class list is empty");

            if (ExternalAndRootClassList.size() != 1) {
                Root = CreateClassForList(List, Info());

                getRoot()->IsNull = true;
                getRoot()->SetChildrenFromList(
                    reinterpret_cast<std::vector<BasicTreeNode *> &> (
                        ExternalAndRootClassList));
            } else {
                getRoot()->AddChild(*ExternalAndRootClassList.front());
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

        return *this;
    }

    template <PointerKind Kind>
    ObjcParseError ParseObjcClassCategorySection(
        const uint8_t *Map,
        const MachO::SectionInfo *SectInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
        ObjcClassInfoCollection *ClassInfoTree,
        std::vector<MachO::ObjcClassCategoryInfo *> &CategoryList,
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
                auto Info = new MachO::ObjcClassCategoryInfo();

                const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtVmAddr<ObjcCategoryType>(
                        SwitchedAddr);

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
                auto Info = new MachO::ObjcClassCategoryInfo();

                auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtVmAddr<ObjcCategoryType>(
                        SwitchedAddr);

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

                ListAddr += PointerSize<Kind>();
                CategoryList.emplace_back(Info);
            }
        }

        return ObjcParseError::None;
    }


    ObjcClassCategoryCollection &
    ObjcClassCategoryCollection::Parse(
        const uint8_t *Map,
        const MachO::SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
        ObjcClassInfoCollection *ClassInfoTree,
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