//
//  include/ADT/Mach-O/ObjcParse.h
//  ktool
//
//  Created by Suhas Pai on 9/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <algorithm>
#include <unordered_map>

#include "ADT/BasicContiguousList.h"
#include "Utils/PointerUtils.h"

#include "BindUtil.h"
#include "ObjcInfo.h"

namespace MachO::ObjcParse {
    enum class Error {
        None,
        NoObjcData,
        UnalignedSection,
        InvalidAddress
    };

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

    inline ObjcClassInfo *
    AddClassToList(
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        ObjcClassInfo &&Class,
        uint64_t ClassAddr) noexcept
    {
        const auto Ptr =
            List.insert({
                ClassAddr,
                std::make_unique<ObjcClassInfo>(Class)
            }).first->second.get();

        return Ptr;
    }

    inline void
    SetSuperClassForClassInfo(ObjcClassInfo *Super,
                              ObjcClassInfo *Info) noexcept
    {
        if (Super != Info) {
            Super->AddChild(*Info);
        } else {
            Info->setSuper(nullptr);
        }
    }

    inline std::string_view
    GetNameFromBindActionSymbol(std::string_view Symbol) noexcept {
        constexpr auto Prefix = std::string_view("_OBJC_CLASS_$_");
        if (Symbol.compare(0, Prefix.size(), Prefix) == 0) {
            return std::string_view(Symbol.data() + Prefix.length());
        }

        return Symbol;
    }

    inline void
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
            AddClassToList(List, std::move(SuperInfo), Action.getAddress());

        SetSuperClassForClassInfo(Ptr, Info);
        ExternalAndRootClassList.emplace_back(Ptr);
    }

    template <PointerKind Kind, typename S, typename T>
    static void
    ParseObjcClass(uint64_t Addr,
                   const S &DeVirtualizeAddr,
                   const T &DeVirtualizeString,
                   const BindActionCollection &BindCollection,
                   ObjcClassInfo &Info,
                   bool IsBigEndian) noexcept
    {
        using ClassType = ClassTypeCalculator<Kind>;
        using ClassRoType = ClassRoTypeCalculator<Kind>;

        const auto Class =
            reinterpret_cast<const ClassType *>(DeVirtualizeAddr(Addr));

        if (Class == nullptr) {
            Info.setAddr(Addr);
            Info.setIsNull();

            return;
        }

        const auto SuperAddr = Class->getSuperClassAddress(IsBigEndian);
        const auto RoAddr = Class->getData(IsBigEndian);
        const auto ClassRo =
            reinterpret_cast<const ClassRoType *>(DeVirtualizeAddr(RoAddr));

        if (ClassRo == nullptr) {
            Info.setAddr(Addr);
            Info.setIsNull();

            return;
        }

        const auto NameAddr = ClassRo->getNameAddress(IsBigEndian);
        if (auto String = DeVirtualizeString(NameAddr)) {
            Info.setName(std::string(String.value()));
        }

        Info.setAddr(Addr);
        Info.setFlags(ClassRo->getFlags(IsBigEndian));
        Info.setIsSwift(Class->isSwift(IsBigEndian));
        Info.setSuper(reinterpret_cast<ObjcClassInfo *>(SuperAddr | 1));
    }

    inline bool
    SetSuperIfExists(
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

    template <PointerKind Kind, typename S, typename T>
    static void
    FixSuperForClassInfo(
        ObjcClassInfo *Info,
        const S &DeVirtualizeAddr,
        const T &DeVirtualizeString,
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
            return;
        }

        const auto SuperAddr =
            reinterpret_cast<uint64_t>(Info->getSuper()) & ~1;

        if (SuperAddr == 0) {
            Info->setSuper(nullptr);
            ExternalAndRootClassList.emplace_back(Info);

            return;
        }

        if (SetSuperIfExists(BindCollection, List, Info, IsBigEndian)) {
            return;
        }

        auto SuperInfo = ObjcClassInfo();
        ParseObjcClass<Kind>(SuperAddr,
                             DeVirtualizeAddr,
                             DeVirtualizeString,
                             BindCollection,
                             SuperInfo,
                             IsBigEndian);

        const auto Ptr = AddClassToList(List, std::move(SuperInfo), SuperAddr);

        SetSuperClassForClassInfo(Ptr, Info);
        FixSuperForClassInfo<Kind>(Ptr,
                                   DeVirtualizeAddr,
                                   DeVirtualizeString,
                                   BindCollection,
                                   List,
                                   ExternalAndRootClassList,
                                   IsBigEndian);

        return;
    }

    template <PointerKind Kind, typename S, typename T>
    static void
    HandleAddrForObjcClass(
        uint64_t Addr,
        const S &DeVirtualizeAddr,
        const T &DeVirtualizeString,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &List,
        bool IsBigEndian) noexcept
    {
        if (Addr == 0) {
            return;
        }

        auto Info = ObjcClassInfo();
        const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);

        ParseObjcClass<Kind>(SwitchedAddr,
                             DeVirtualizeAddr,
                             DeVirtualizeString,
                             BindCollection,
                             Info,
                             IsBigEndian);

        AddClassToList(List, std::move(Info), SwitchedAddr);
    }

    template <PointerKind Kind, typename S, typename T>
    static void
    FixSuperClassForClassList(
        const S &DeVirtualizeAddr,
        const T &DeVirtualizeString,
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

            if (Info->isExternal()) {
                continue;
            }

            FixSuperForClassInfo<Kind>(Info,
                                       DeVirtualizeAddr,
                                       DeVirtualizeString,
                                       BindCollection,
                                       List,
                                       ExternalAndRootClassList,
                                       IsBigEndian);
        }
    }

    template <PointerKind Kind, typename S, typename T>
    static Error
    ParseObjcClassListSection(
        const uint8_t *Begin,
        const uint8_t *End,
        const S &DeVirtualizeAddr,
        const T &DeVirtualizeString,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &ClassList,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        using PtrAddrType = PointerAddrConstTypeFromKind<Kind>;
        if (!IntegerIsPointerAligned<Kind>(End - Begin)) {
            return Error::UnalignedSection;
        }

        const auto List = BasicContiguousList<PtrAddrType>(Begin, End);
        for (const auto &Addr : List) {
            HandleAddrForObjcClass<Kind>(Addr,
                                         DeVirtualizeAddr,
                                         DeVirtualizeString,
                                         BindCollection,
                                         ClassList,
                                         IsBigEndian);
        }

        FixSuperClassForClassList<Kind>(DeVirtualizeAddr,
                                        DeVirtualizeString,
                                        BindCollection,
                                        ClassList,
                                        ExternalAndRootClassList,
                                        IsBigEndian);

        return Error::None;
    }

    [[nodiscard]] inline ObjcClassInfo
    CreateExternalClass(std::string_view Name,
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

    template <PointerKind Kind, typename S, typename T>
    static Error
    ParseObjcClassRefsSection(
        const uint8_t *Map,
        const SectionInfo *SectionInfo,
        const S &DeVirtualizeAddr,
        const T &DeVirtualizeString,
        const BindActionCollection &BindCollection,
        std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>> &ClassList,
        std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
        bool IsBigEndian) noexcept
    {
        using PointerAddrType = PointerAddrConstTypeFromKind<Kind>;

        const auto Begin = SectionInfo->getData(Map);
        const auto End = SectionInfo->getDataEnd(Map);

        if (!IntegerIsPointerAligned<Kind>(End - Begin)) {
            return Error::UnalignedSection;
        }

        const auto List = BasicContiguousList<PointerAddrType>(Begin, End);
        auto ListAddr = SectionInfo->getMemoryRange().getBegin();

        for (const auto &Addr : List) {
            if (const auto *It = BindCollection.GetInfoForAddress(ListAddr)) {
                const auto Name = GetNameFromBindActionSymbol(It->getSymbol());
                auto NewInfo =
                    CreateExternalClass(Name, It->getDylibOrdinal(), ListAddr);
                const auto Ptr =
                    AddClassToList(ClassList, std::move(NewInfo), ListAddr);

                ExternalAndRootClassList.emplace_back(Ptr);
            } else {
                HandleAddrForObjcClass<Kind>(Addr,
                                             DeVirtualizeAddr,
                                             DeVirtualizeString,
                                             BindCollection,
                                             ClassList,
                                             IsBigEndian);
            }

            ListAddr += PointerSize<Kind>();
        }

        FixSuperClassForClassList<Kind>(DeVirtualizeAddr,
                                        DeVirtualizeString,
                                        BindCollection,
                                        ClassList,
                                        ExternalAndRootClassList,
                                        IsBigEndian);

        return Error::None;
    }
}
