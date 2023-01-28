/*
 * MachO/ObjcInfo.cpp
 * © suhas pai
 */

#include "MachO/ObjcInfo.h"
#include "MachO/DeVirtualizer.h"

namespace MachO {
    void
    ObjcClassInfoList::AdjustExternalAndRootClassList(
        std::vector<Info *> &List) noexcept
    {
        if (List.empty()) {
            return;
        }

        if (List.size() != 1) {
            setRoot(
                ObjcParse::ClassCollectionTypeAddClass(this->List, Info(), 0));

            Root->setIsNull();
            for (const auto &Node : List) {
                Root->addChild(*Node);
            }
        } else {
            setRoot(List.front());
        }
    }

    auto
    ObjcClassInfoList::Parse(const DeVirtualizer &DeVirtualizer,
                             const BindActionList::UnorderedMap &BindList,
                             const bool IsBigEndian,
                             const bool Is64Bit) noexcept
        -> ObjcParse::Error
    {
        auto ExternalAndRootClassList = std::vector<Info *>();
        auto Error = ObjcParse::Error::None;

        const auto &SegmentList = DeVirtualizer.segmentList();
        const auto ObjcClassRefsSection =
            SegmentList.findSectionWithName(
                ObjcParse::ObjcClassListSegmentSectionNamePairList);

        if (ObjcClassRefsSection != nullptr) {
            if (Is64Bit) {
                Error =
                    ObjcParse::ParseObjcClassRefsSection<true>(
                        *ObjcClassRefsSection,
                        DeVirtualizer,
                        BindList,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ObjcParse::ParseObjcClassRefsSection<false>(
                        *ObjcClassRefsSection,
                        DeVirtualizer,
                        BindList,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            }

            if (Error != ObjcParse::Error::None) {
                return Error;
            }

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
            return Error;
        }

        const auto ObjcClassListSection =
            SegmentList.findSectionWithName({
                { "__OBJC2",      { "__class_list"     } },
                { "__DATA_CONST", { "__objc_classlist" } },
                { "__DATA",       { "__objc_classlist" } },
                { "__DATA_DIRTY", { "__objc_classlist" } },
            });

        if (ObjcClassListSection != nullptr) {
            if (Is64Bit) {
                Error =
                    ObjcParse::ParseObjcClassListSection<true>(
                        *ObjcClassListSection,
                        DeVirtualizer,
                        BindList,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ObjcParse::ParseObjcClassListSection<false>(
                        *ObjcClassListSection,
                        DeVirtualizer,
                        BindList,
                        List,
                         ExternalAndRootClassList,
                        IsBigEndian);
            }

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
            return Error;
        }

        return ObjcParse::Error::NoObjcData;
    }

    auto
    ObjcClassInfoList::getAsList() const noexcept -> std::vector<Info *> {
        auto Vector = std::vector<Info *>();
        Vector.reserve(List.size());

        for (const auto &Info : List) {
            Vector.emplace_back(Info.second.get());
        }

        return Vector;
    }

    auto
    ObjcClassInfoList::addNullClass(const uint64_t BindAddress) noexcept
        -> ObjcClassInfo *
    {
        auto NewInfo = Info();

        NewInfo.setAddr(BindAddress);
        NewInfo.setIsNull();

        return ObjcParse::ClassCollectionTypeAddClass(List,
                                                      std::move(NewInfo),
                                                      BindAddress);
    }

    auto
    ObjcClassInfoList::addExternalClass(
        const std::string_view Name,
        const uint64_t DylibOrdinal,
        const uint64_t BindAddress) noexcept -> ObjcClassInfo *
    {
        auto NewInfo =
            ObjcParse::CreateExternalClass(Name, DylibOrdinal, BindAddress);

        return ObjcParse::ClassCollectionTypeAddClass(List,
                                                      std::move(NewInfo),
                                                      BindAddress);
    }

    [[nodiscard]]
    auto
    ObjcClassInfoList::getInfoForAddress(const uint64_t Address) const noexcept
        -> ObjcClassInfo *
    {
        if (const auto It = List.find(Address); It != List.end()) {
            return It->second.get();
        }

        return nullptr;
    }

    [[nodiscard]] auto
    ObjcClassInfoList::getInfoForClassName(
        const std::string_view Name) const noexcept -> ObjcClassInfo *
    {
        for (const auto &Info : List) {
            if (Info.second->name() == Name) {
                return Info.second.get();
            }
        }

        return nullptr;
    }

    template <bool Is64Bit>
    static void ParseObjcClassCategorySection(
        const SectionInfo &SectInfo,
        const DeVirtualizer &DeVirt,
        const BindActionList::UnorderedMap &BindList,
        ObjcClassInfoList *ClassInfoTree,
        std::vector<std::unique_ptr<ObjcClassCategoryInfo>> &CategoryList,
        const bool IsBigEndian) noexcept
    {
        using PtrAddrType = Utils::PointerAddrConstType<Is64Bit>;
        using ObjcCategoryType = ObjcParse::ObjcClassCategoryType<Is64Bit>;

        const auto Map = DeVirt.map();

        auto End = static_cast<PtrAddrType *>(nullptr);
        auto Begin = Map.getFromRange<PtrAddrType>(SectInfo.fileRange(), &End);

        const auto List = ADT::List<PtrAddrType>(Begin, End);
        auto ListAddr = SectInfo.vmRange().begin();

        if (ClassInfoTree != nullptr) {
            const auto BindEnd = BindList.end();
            for (const auto &Addr : List) {
                auto Info = std::make_unique<ObjcClassCategoryInfo>();

                const auto SwitchedAddr =
                    ADT::SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirt.getDataAtAddress<ObjcCategoryType>(
                        SwitchedAddr,
                        /*IgnoreSectionBounds=*/true);

                Info->setAddress(SwitchedAddr);
                if (Category == nullptr) {
                    Info->setIsNull();
                    CategoryList.emplace_back(std::move(Info));

                    continue;
                }

                const auto NameAddr = Category->nameAddress(IsBigEndian);
                if (const auto Name = DeVirt.getStringAtAddress(NameAddr)) {
                    Info->setName(std::string(Name.value()));
                }

                // ClassAddr initially points to the 'Class' field that (may)
                // get bound.

                auto Class = static_cast<ObjcClassInfo *>(nullptr);
                auto ClassAddr =
                    SwitchedAddr +
                    offsetof(ObjcParse::ObjcClassCategoryType<Is64Bit>, Class);

                if (const auto It = BindList.find(ClassAddr); It != BindEnd) {
                    const auto Info = It->second;
                    const auto Name =
                        ObjcParse::GetNameFromBindActionSymbol(Info.SymbolName);

                    Class = ClassInfoTree->getInfoForClassName(Name);
                    if (Class == nullptr) {
                        const auto FullAddress =
                            Info.getFullAddress(DeVirt.segmentList());

                        Class =
                            ClassInfoTree->addExternalClass(
                                Name,
                                static_cast<uint64_t>(Info.DylibOrdinal),
                                FullAddress.has_value() ?
                                    FullAddress.value() :
                                    std::numeric_limits<uint64_t>::max());
                    }
                } else {
                    ClassAddr = Category->classAddress(IsBigEndian);
                    if (ClassAddr != 0) {
                        Class = ClassInfoTree->getInfoForAddress(ClassAddr);
                        if (Class == nullptr) {
                            Class = ClassInfoTree->addNullClass(ClassAddr);
                        }

                        Info->setClass(Class);
                        Class->categoryListRef().emplace_back(Info.get());
                    }
                }

                CategoryList.emplace_back(std::move(Info));
                ListAddr += Utils::PointerSize<Is64Bit>();
            }
        } else {
            for (const auto &Addr : List) {
                auto Info = std::make_unique<ObjcClassCategoryInfo>();

                auto SwitchedAddr = ADT::SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirt.getDataAtAddress<ObjcCategoryType>(
                        SwitchedAddr,
                        /*IgnoreSectionBounds=*/true);

                Info->setAddress(SwitchedAddr);
                if (Category == nullptr) {
                    Info->setIsNull();
                    CategoryList.emplace_back(std::move(Info));

                    continue;
                }

                const auto NameAddr = Category->nameAddress(IsBigEndian);
                if (const auto Name = DeVirt.getStringAtAddress(NameAddr)) {
                    Info->setName(std::string(Name.value()));
                }

                CategoryList.emplace_back(std::move(Info));
                ListAddr += Utils::PointerSize<Is64Bit>();
            }
        }
    }

    auto
    ObjcClassCategoryInfoList::CollectFrom(
        const DeVirtualizer &DeVirtualizer,
        const BindActionList::UnorderedMap &BindCollection,
        ObjcClassInfoList *const ClassInfoTree,
        const bool IsBigEndian,
        const bool Is64Bit) noexcept -> ObjcParse::Error
    {
        const auto ObjcClassCategorySection =
            DeVirtualizer.segmentList().findSectionWithName(
                ObjcParse::ObjcClassCategoryListSegmentSectionNamePairList);

        if (ObjcClassCategorySection == nullptr) {
            return ObjcParse::Error::NoObjcData;
        }

        if (Is64Bit) {
            ParseObjcClassCategorySection<true>(*ObjcClassCategorySection,
                                                DeVirtualizer,
                                                BindCollection,
                                                ClassInfoTree,
                                                List,
                                                IsBigEndian);
        } else {
            ParseObjcClassCategorySection<false>(*ObjcClassCategorySection,
                                                 DeVirtualizer,
                                                 BindCollection,
                                                 ClassInfoTree,
                                                 List,
                                                 IsBigEndian);
        }

        return ObjcParse::Error::None;
    }
}