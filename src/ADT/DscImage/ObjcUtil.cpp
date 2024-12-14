//
//  ADT/DscImage/ObjcUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 9/5/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/DscImage/ObjcUtil.h"

namespace DscImage {
    auto
    ObjcClassInfoCollection::Parse(
        const uint8_t *const Map,
        const MachO::SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection &BindCollection,
        const bool IsBigEndian,
        const bool Is64Bit,
        Error *const ErrorOut) noexcept
            -> decltype(*this)
    {
        auto Error = MachO::ObjcParse::Error::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

        const auto DeVirtualizeAddrFunc = [&](uint64_t Addr) noexcept {
            return DeVirtualizer.GetDataAtVmAddr<uint8_t>(Addr);
        };

        const auto DeVirtualizeStringFunc = [&](uint64_t Addr) noexcept {
            return DeVirtualizer.GetStringAtAddress(Addr);
        };

        const auto ObjcClassRefsSection =
            SegmentCollection.FindSectionWithName({
                { "__OBJC2", { "__class_refs"     } },
                { "__DATA",  { "__objc_classrefs" } },
            });

        if (ObjcClassRefsSection != nullptr) {
            if (Is64Bit) {
                Error =
                    MachO::ObjcParse::ParseObjcClassRefsSection<
                        PointerKind::s64Bit>(
                            Map,
                            ObjcClassRefsSection,
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            } else {
                Error =
                    MachO::ObjcParse::ParseObjcClassRefsSection<
                        PointerKind::s32Bit>(
                            Map,
                            ObjcClassRefsSection,
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            }

            if (Error != Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error;
                }
            }

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
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
                    MachO::ObjcParse::ParseObjcClassListSection<
                        PointerKind::s64Bit>(
                            ObjcClassListSection->getData(Map),
                            ObjcClassListSection->getDataEnd(Map),
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            } else {
                Error =
                    MachO::ObjcParse::ParseObjcClassListSection<
                        PointerKind::s32Bit>(
                            ObjcClassListSection->getData(Map),
                            ObjcClassListSection->getDataEnd(Map),
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            }

            if (Error != Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error;
                }
            }

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
            return *this;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error::NoObjcData;
        }

        return *this;
    }

    auto
    ObjcClassInfoCollection::Parse(
        const ConstMemoryMap &DscMap,
        const ConstMemoryMap &ImageMap,
        const MachO::SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionList *const BindList,
        const MachO::LazyBindActionList *const LazyBindList,
        const MachO::WeakBindActionList *const WeakBindList,
        const bool IsBigEndian,
        const bool Is64Bit,
        Error *const ErrorOut,
        MachO::BindOpcodeParseError *ParseErrorOut,
        MachO::BindActionCollection::Error *const CollectionErrorOut) noexcept
            -> decltype(*this)
    {
        (void)ImageMap;

        auto Error = MachO::ObjcParse::Error::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

//      const auto ImageBase =
//            SegmentCollection.front().getMemoryRange().getBegin();
        const auto DeVirtualizeAddrFunc = [&](uint64_t Addr) noexcept {
            return DeVirtualizer.GetDataAtVmAddr<uint8_t>(Addr);
        };

        const auto DeVirtualizeStringFunc = [&](uint64_t Addr) noexcept {
            return DeVirtualizer.GetStringAtAddress(Addr);
        };

        const auto ObjcClassRefsSection =
            SegmentCollection.FindSectionWithName({
                { "__OBJC2", { "__class_refs"     } },
                { "__DATA",  { "__objc_classrefs" } },
            });

        if (ObjcClassRefsSection != nullptr) {
            const auto BindCollectionRange =
                ObjcClassRefsSection->getMemoryRange();

            auto BindCollection = MachO::BindActionCollection();
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
                    MachO::ObjcParse::ParseObjcClassRefsSection<
                        PointerKind::s64Bit>(
                            DscMap.getBegin(),
                            ObjcClassRefsSection,
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            } else {
                Error =
                    MachO::ObjcParse::ParseObjcClassRefsSection<
                        PointerKind::s32Bit>(
                            DscMap.getBegin(),
                            ObjcClassRefsSection,
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            }

            if (Error != Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error;
                }
            }

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
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

            auto BindCollection = MachO::BindActionCollection();
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
                    MachO::ObjcParse::ParseObjcClassListSection<
                        PointerKind::s64Bit>(
                            ObjcClassListSection->getData(DscMap.getBegin()),
                            ObjcClassListSection->getDataEnd(DscMap.getBegin()),
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            } else {
                Error =
                    MachO::ObjcParse::ParseObjcClassListSection<
                        PointerKind::s32Bit>(
                            ObjcClassListSection->getData(DscMap.getBegin()),
                            ObjcClassListSection->getDataEnd(DscMap.getBegin()),
                            DeVirtualizeAddrFunc,
                            DeVirtualizeStringFunc,
                            BindCollection,
                            List,
                            ExternalAndRootClassList,
                            IsBigEndian);
            }

            if (Error != Error::None) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Error;
                }

                return *this;
            }

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
            return *this;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error::NoObjcData;
        }

        return *this;
    }

    template <PointerKind Kind>
    static void
    ParseObjcClassCategorySection(
        const uint8_t *const Map,
        const MachO::SectionInfo *const SectInfo,
        const ConstDeVirtualizer &DeVirt,
        const MachO::BindActionCollection *const BindCollection,
        ObjcClassInfoCollection *const ClassInfoTree,
        std::vector<
            std::unique_ptr<MachO::ObjcClassCategoryInfo>> &CategoryList,
        const bool IsBigEndian) noexcept
    {
        using PtrAddrType = PointerAddrConstTypeFromKind<Kind>;
        using ObjcCategoryType =
            MachO::ObjcParse::ClassCategoryTypeCalculator<Kind>;

        auto Begin = SectInfo->getData(Map);
        auto End = SectInfo->getDataEnd(Map);

        const auto List = BasicContiguousList<PtrAddrType>(Begin, End);
        auto ListAddr = SectInfo->getMemoryRange().getBegin();

        if (ClassInfoTree != nullptr) {
            assert(BindCollection != nullptr);
            for (const auto &Addr : List) {
                auto Info = std::make_unique<MachO::ObjcClassCategoryInfo>();

                const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirt.GetDataAtVmAddr<ObjcCategoryType>(SwitchedAddr);

                Info->setAddress(SwitchedAddr);
                if (Category == nullptr) {
                    Info->setIsNull();
                    CategoryList.emplace_back(std::move(Info));

                    continue;
                }

                const auto NameAddr = Category->getNameAddress(IsBigEndian);
                if (const auto Name = DeVirt.GetStringAtAddress(NameAddr)) {
                    Info->setName(std::string(Name.value()));
                }

                // ClassAddr initially points to the 'Class' field that (may)
                // get bound.

                auto Class = static_cast<MachO::ObjcClassInfo *>(nullptr);
                auto ClassAddr =
                    SwitchedAddr + offsetof(ObjcCategoryType, Class);

                if (auto *It = BindCollection->GetInfoForAddress(ClassAddr)) {
                    const auto Name =
                        MachO::ObjcParse::GetNameFromBindActionSymbol(
                            It->getSymbol());

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

                        Info->setClass(Class);
                        Class->getCategoryListRef().emplace_back(Info.get());
                    }
                }

                CategoryList.emplace_back(std::move(Info));
                ListAddr += PointerSize<Kind>();
            }
        } else {
            for (const auto &Addr : List) {
                auto Info = std::make_unique<MachO::ObjcClassCategoryInfo>();
                auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);

                const auto Category =
                    DeVirt.GetDataAtVmAddr<ObjcCategoryType>(SwitchedAddr);

                Info->setAddress(SwitchedAddr);
                if (Category == nullptr) {
                    Info->setIsNull();
                    CategoryList.emplace_back(std::move(Info));

                    continue;
                }

                const auto NameAddr = Category->getNameAddress(IsBigEndian);
                if (const auto Name = DeVirt.GetStringAtAddress(NameAddr)) {
                    Info->setName(std::string(Name.value()));
                }

                CategoryList.emplace_back(std::move(Info));
                ListAddr += PointerSize<Kind>();
            }
        }
    }

    ObjcClassCategoryCollection &
    ObjcClassCategoryCollection::CollectFrom(
        const uint8_t *const Map,
        const MachO::SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const MachO::BindActionCollection *const BindCollection,
        ObjcClassInfoCollection *const ClassInfoTree,
        const bool IsBigEndian,
        const bool Is64Bit,
        Error *const ErrorOut) noexcept
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

    auto
    ObjcClassCategoryCollection::CollectFrom(
        const ConstMemoryMap &Map,
        const MachO::SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        ObjcClassInfoCollection *const ClassInfoTree,
        const MachO::BindActionList *const BindList,
        const MachO::LazyBindActionList *const LazyBindList,
        const MachO::WeakBindActionList *const WeakBindList,
        const bool IsBigEndian,
        const bool Is64Bit,
        Error *const ErrorOut,
        MachO::BindOpcodeParseError *const ParseErrorOut,
        MachO::BindActionCollection::Error *const CollectionErrorOut) noexcept
            -> decltype(*this)
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

        auto BindCollection = MachO::BindActionCollection();
        const auto GetBindActionCollectionResult =
            ObjcClassInfoCollection::GetBindCollection(SegmentCollection,
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
                &BindCollection,
                ClassInfoTree,
                List,
                IsBigEndian);
        } else {
            ParseObjcClassCategorySection<PointerKind::s32Bit>(
                Map.getBegin(),
                ObjcClassCategorySection,
                DeVirtualizer,
                &BindCollection,
                ClassInfoTree,
                List,
                IsBigEndian);
        }

        return *this;
    }
}
