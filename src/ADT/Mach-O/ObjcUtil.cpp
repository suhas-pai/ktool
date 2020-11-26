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
    void
    ObjcClassInfoCollection::AdjustExternalAndRootClassList(
        std::vector<Info *> &List) noexcept
    {
        if (List.empty()) {
            return;
        }

        if (List.size() != 1) {
            Root = ObjcParse::AddClassToList(this->List, Info(), 0);
            getRoot()->setIsNull();

            for (const auto &Node : List) {
                getRoot()->AddChild(*Node);
            }
        } else {
            Root = List.front();
        }
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
        auto Error = ObjcParse::Error::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

        const auto DeVirtualizeAddrFunc = [&](uint64_t Addr) noexcept {
            return DeVirtualizer.GetDataAtAddressIgnoreSections<uint8_t>(Addr);
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
                    ObjcParse::ParseObjcClassRefsSection<PointerKind::s64Bit>(
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
                    ObjcParse::ParseObjcClassRefsSection<PointerKind::s32Bit>(
                        Map,
                        ObjcClassRefsSection,
                        DeVirtualizeAddrFunc,
                        DeVirtualizeStringFunc,
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
                    ObjcParse::ParseObjcClassListSection<PointerKind::s64Bit>(
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
                    ObjcParse::ParseObjcClassListSection<PointerKind::s32Bit>(
                        ObjcClassListSection->getData(Map),
                        ObjcClassListSection->getDataEnd(Map),
                        DeVirtualizeAddrFunc,
                        DeVirtualizeStringFunc,
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

            AdjustExternalAndRootClassList(ExternalAndRootClassList);
            return *this;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error::NoObjcData;
        }

        return *this;
    }

    int
    ObjcClassInfoCollection::GetBindCollection(
        const SegmentInfoCollection &SegmentCollection,
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
        auto Error = ObjcParse::Error::None;
        auto ExternalAndRootClassList = std::vector<Info *>();

        const auto DeVirtualizeAddrFunc = [&](uint64_t Addr) noexcept {
            return DeVirtualizer.GetDataAtAddressIgnoreSections<uint8_t>(Addr);
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
                    ObjcParse::ParseObjcClassRefsSection<PointerKind::s64Bit>(
                        Map.getBegin(),
                        ObjcClassRefsSection,
                        DeVirtualizeAddrFunc,
                        DeVirtualizeStringFunc,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ObjcParse::ParseObjcClassRefsSection<PointerKind::s32Bit>(
                        Map.getBegin(),
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
                    ObjcParse::ParseObjcClassListSection<PointerKind::s64Bit>(
                        ObjcClassListSection->getData(Map.getBegin()),
                        ObjcClassListSection->getDataEnd(Map.getBegin()),
                        DeVirtualizeAddrFunc,
                        DeVirtualizeStringFunc,
                        BindCollection,
                        List,
                        ExternalAndRootClassList,
                        IsBigEndian);
            } else {
                Error =
                    ObjcParse::ParseObjcClassListSection<PointerKind::s32Bit>(
                        ObjcClassListSection->getData(Map.getBegin()),
                        ObjcClassListSection->getDataEnd(Map.getBegin()),
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

        return ObjcParse::AddClassToList(List, std::move(NewInfo), Address);
    }

    ObjcClassInfo *
    ObjcClassInfoCollection::AddExternalClass(std::string_view Name,
                                              uint64_t DylibOrdinal,
                                              uint64_t BindAddr) noexcept
    {
        auto NewInfo =
            ObjcParse::CreateExternalClass(Name, DylibOrdinal, BindAddr);

        return ObjcParse::AddClassToList(List, std::move(NewInfo), BindAddr);
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
        std::string_view Name) const noexcept
    {
        for (const auto &Iter : List) {
            const auto &Info = Iter.second;
            if (Info->getName() == Name) {
                return Info.get();
            }
        }

        return nullptr;
    }

    template <PointerKind Kind>
    static void ParseObjcClassCategorySection(
        const uint8_t *Map,
        const SectionInfo *SectInfo,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection *BindCollection,
        ObjcClassInfoCollection *ClassInfoTree,
        std::vector<std::unique_ptr<ObjcClassCategoryInfo>> &CategoryList,
        bool IsBigEndian) noexcept
    {
        using PtrAddrType = PointerAddrConstTypeFromKind<Kind>;
        using ObjcCategoryType = ObjcParse::ClassCategoryTypeCalculator<Kind>;

        auto Begin = SectInfo->getData(Map);
        auto End = SectInfo->getDataEnd(Map);

        const auto List = BasicContiguousList<PtrAddrType>(Begin, End);
        auto ListAddr = SectInfo->getMemoryRange().getBegin();

        if (ClassInfoTree != nullptr) {
            assert(BindCollection != nullptr);
            for (const auto &Addr : List) {
                auto Info = std::make_unique<ObjcClassCategoryInfo>();

                const auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtAddressIgnoreSections<
                        ObjcCategoryType>(SwitchedAddr);

                Info->setAddress(SwitchedAddr);
                if (Category == nullptr) {
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
                    offsetof(ObjcParse::ClassCategoryTypeCalculator<Kind>,
                             Class);

                if (auto *It = BindCollection->GetInfoForAddress(ClassAddr)) {
                    const auto Name =
                        ObjcParse::GetNameFromBindActionSymbol(It->getSymbol());

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
                auto Info = std::make_unique<ObjcClassCategoryInfo>();

                auto SwitchedAddr = SwitchEndianIf(Addr, IsBigEndian);
                const auto Category =
                    DeVirtualizer.GetDataAtAddressIgnoreSections<
                        ObjcCategoryType>(SwitchedAddr);

                Info->setAddress(SwitchedAddr);
                if (Category == nullptr) {
                    Info->setIsNull();
                    CategoryList.emplace_back(std::move(Info));

                    continue;
                }

                const auto NameAddr = Category->getNameAddress(IsBigEndian);
                const auto Name = DeVirtualizer.GetStringAtAddress(NameAddr);

                if (Name.has_value()) {
                    Info->setName(std::string(Name.value()));
                }

                CategoryList.emplace_back(std::move(Info));
                ListAddr += PointerSize<Kind>();
            }
        }
    }

    ObjcClassCategoryCollection &
    ObjcClassCategoryCollection::CollectFrom(
        const uint8_t *Map,
        const SegmentInfoCollection &SegmentCollection,
        const ConstDeVirtualizer &DeVirtualizer,
        const BindActionCollection *BindCollection,
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

