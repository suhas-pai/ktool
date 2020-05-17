//
//  src/Operations/PrintObjcClassList.cpp
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "ADT/MachO.h"

#include "Utils/CharUtils.h"
#include "Utils/MachOPrinter.h"
#include "Utils/MiscTemplates.h"
#include "Utils/PointerUtils.h"
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintObjcClassList.h"

PrintObjcClassListOperation::PrintObjcClassListOperation() noexcept
: PrintOperation(OpKind) {}
PrintObjcClassListOperation::PrintObjcClassListOperation(
    const struct Options &Options) noexcept
: PrintOperation(OpKind), Options(Options) {}

int PrintObjcClassListOperation::run(const ConstMemoryObject &Object) noexcept {
    return run(Object, Options);
}

[[nodiscard]]
static inline bool ShouldSkipClass(const std::string &Name) noexcept {
    for (const auto &Ch : Name) {
        if (IsControl(Ch) || IsSpace(Ch)) {
            return true;
        }
    }

    return false;
}

static inline const char *
HandleNameString(const char *It,
                 const char *End,
                 std::vector<std::string> &NameList) noexcept
{
    const auto Name = std::string(It, strnlen(It, End - It));
    if (Name.empty()) {
        return (It += 1);
    }

    It += (Name.length() + 1);
    if (ShouldSkipClass(Name)) {
        return It;
    }

    NameList.emplace_back(std::move(Name));
    return It;
}

template <bool Is64Bit>
static inline std::vector<std::string>
ParseObjcClassNameList(const MachO::SegmentInfoCollection &Collection,
                       const uint8_t *Data,
                       const uint64_t Size) noexcept
{
    auto NameList = std::vector<std::string>();

    const auto Begin = reinterpret_cast<const char *>(Data);
    const auto End = Begin + Size;

    for (auto It = Begin; It != End;) {
        It = HandleNameString(It, End, NameList);
    }

    return NameList;
}


template <bool Is64Bit>
static inline std::vector<std::string>
ParseObjcClassList(const MachO::SegmentInfoCollection &Collection,
                   const uint8_t *Map,
                   const uint8_t *AddrListPtr,
                   const uint64_t Size,
                   bool IsBigEndian) noexcept
{
    using AddrType = PointerAddrConstType<Is64Bit>;
    using ObjcClassType =
        std::conditional_t<Is64Bit, MachO::ObjcClass64, MachO::ObjcClass>;
    using ObjcClassRoType =
        std::conditional_t<Is64Bit, MachO::ObjcClassRo64, MachO::ObjcClassRo>;

    const auto AddrList = BasicContiguousList<AddrType>(AddrListPtr, Size);
    auto NameList = std::vector<std::string>();

    for (const auto &Addr : AddrList) {
        const auto ObjcClass =
            MachO::GetPtrForVirtualAddr<ObjcClassType>(Collection, Map, Addr);

        if (ObjcClass == nullptr) {
            continue;
        }

        const auto ObjcClassRoAddr =
            SwitchEndianIf(ObjcClass->Data, IsBigEndian) & ~7;
        const auto ObjcClassRo =
            MachO::GetPtrForVirtualAddr<ObjcClassRoType>(Collection,
                                                         Map,
                                                         ObjcClassRoAddr);

        if (ObjcClassRo == nullptr) {
            continue;
        }

        auto NameMaxEnd = static_cast<const char *>(nullptr);

        const auto NameAddr = SwitchEndianIf(ObjcClassRo->Name, IsBigEndian);
        const auto Name =
            MachO::GetPtrForVirtualAddr<const char>(Collection,
                                                    Map,
                                                    NameAddr,
                                                    &NameMaxEnd);
        if (Name == nullptr) {
            continue;
        }

        HandleNameString(Name, NameMaxEnd, NameList);
    }

    return NameList;
}

static int
CollectClassNames(const MachO::SegmentInfoCollection &SegCollection,
                  const uint8_t *Map,
                  FILE *ErrFile,
                  bool IsBigEndian,
                  bool Is64Bit,
                  MachO::SegmentInfoCollectionError CollectionError,
                  std::vector<std::string> *ClassesOut) noexcept
{
    const auto ObjCClassNameListSection =
        MachO::FindSectionWithName(SegCollection, {
            { "__TEXT", "__objc_classname" },
        });

    if (ObjCClassNameListSection != nullptr) {
        const auto Size = ObjCClassNameListSection->File.GetSize();
        const auto Data =
            MachO::GetDataForSection(Map, ObjCClassNameListSection);

        if (Is64Bit) {
            *ClassesOut =
                ParseObjcClassNameList<true>(SegCollection, Data, Size);
        } else {
            *ClassesOut =
                ParseObjcClassNameList<false>(SegCollection, Data, Size);
        }

        return 0;
    }

    const auto ObjCClassListSection =
        MachO::FindSectionWithName(SegCollection, {
            { "__DATA",       "__objc_classlist" },
            { "__DATA_CONST", "__objc_classlist" },
            { "__OBJC2",      "__class_list"     }
        });

    if (ObjCClassListSection != nullptr) {
        const auto Size = ObjCClassListSection->File.GetSize();
        if (IntegerIsPointerAligned(Size, Is64Bit)) {
            const auto Data =
                MachO::GetDataForSection(Map, ObjCClassListSection);

            if (Is64Bit) {
                *ClassesOut =
                    ParseObjcClassList<true>(SegCollection,
                                             Map,
                                             Data,
                                             Size,
                                             IsBigEndian);
            } else {
                *ClassesOut =
                    ParseObjcClassList<false>(SegCollection,
                                              Map,
                                              Data,
                                              Size,
                                              IsBigEndian);
            }

            return 0;
        }

        fputs("Objective-C Class-List Section is not aligned properly\n",
              ErrFile);

        return 1;
    }

    // Delay handling Collection's Error as segments and sections are still
    // added to the list.

    switch (CollectionError) {
        case MachO::SegmentInfoCollectionError::None:
            break;
        case MachO::SegmentInfoCollectionError::InvalidSegment:
            fputs("Provided file has an invalid segment\n", ErrFile);
            return 1;
        case MachO::SegmentInfoCollectionError::OverlappingSegments:
            fputs("Provided file has overlapping segments\n", ErrFile);
            return 1;
        case MachO::SegmentInfoCollectionError::InvalidSection:
            fputs("Provided file has an invalid section\n", ErrFile);
            return 1;
        case MachO::SegmentInfoCollectionError::OverlappingSections:
            fputs("Provided file has overlapping sections", ErrFile);
            return 1;
    }

    return 1;
}

int
PrintObjcClassListOperation::run(const ConstMachOMemoryObject &Object,
                                 const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.HasError()) {
        return 1;
    }

    const auto Is64Bit =  Object.Is64Bit();
    const auto CollectionResult =
        MachO::CollectSegmentInfoList(LoadCmdStorage, Is64Bit);

    auto NameList = std::vector<std::string>();

    const auto &SegmentCollection = CollectionResult.Collection;
    const auto CollectClassNamesResult =
        CollectClassNames(SegmentCollection,
                          Object.GetMap().GetBegin(),
                          Options.ErrFile,
                          LoadCmdStorage.IsBigEndian(),
                          Is64Bit,
                          CollectionResult.Error,
                          &NameList);

    if (CollectClassNamesResult != 0) {
        return CollectClassNamesResult;
    }

    if (NameList.empty()) {
        fputs("Provided file has no Objective-C sections\n", Options.OutFile);
        return 0;
    }

    if (Options.Sort) {
        std::sort(NameList.begin(), NameList.end());
    }

    fprintf(Options.OutFile, "%lu Objective-C Classes:\n", NameList.size());

    auto Counter = static_cast<uint64_t>(1);
    for (const auto &Name : NameList) {
        fprintf(Options.OutFile,
                "Objective-C Class %04" PRIu64 ": %s\n",
                Counter,
                Name.data());
        
        Counter++;
    }

    return 0;
}

struct PrintObjcClassListOperation::Options
PrintObjcClassListOperation::ParseOptionsImpl(int Argc,
                                              const char *Argv[],
                                              int *IndexOut) noexcept
{
    struct Options Options;
    for (auto I = int(); I != Argc; I++) {
        const auto Argument = Argv[I];
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort") == 0) {
            Options.Sort = true;
        } else if (Argument[0] != '-') {
            if (IndexOut != nullptr) {
                *IndexOut = I;
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument);
            exit(1);
        }
    }

    return Options;
}

struct PrintObjcClassListOperation::Options *
PrintObjcClassListOperation::ParseOptions(int Argc,
                                          const char *Argv[],
                                          int *IndexOut) noexcept
{
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

int
PrintObjcClassListOperation::run(const ConstMemoryObject &Object,
                                 const struct Options &Options) noexcept
{
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
            PrintObjectKindNotSupportedError(OpKind, Object);
            return 1;
    }

    assert(0 && "Reached end with unrecognizable Object-Kind");
}

int
PrintObjcClassListOperation::run(const ConstMemoryObject &Object,
                                 int Argc,
                                 const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    return run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
