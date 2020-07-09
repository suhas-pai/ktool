//
//  include/Operations/Common.h
//  stool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <string_view>

#include "ADT/MachO.h"
#include "Objects/MachOMemory.h"

struct OperationCommon {
    static MachO::LoadCommandStorage
    GetLoadCommandStorage(MachOMemoryObject &Object, FILE *ErrFile) noexcept;

    static MachO::ConstLoadCommandStorage
    GetConstLoadCommandStorage(const ConstMachOMemoryObject &Object,
                               FILE *ErrFile) noexcept;

    static const std::string_view &
    GetLoadCommandStringValue(
        const MachO::LoadCommandString::GetValueResult &) noexcept;

    static int
    HandleSegmentCollectionError(
        FILE *ErrFile,
        MachO::SegmentInfoCollection::Error Error) noexcept;

    static int
    HandleSharedLibraryCollectionError(
        FILE *ErrFile,
        MachO::SharedLibraryInfoCollection::Error Error) noexcept;

    static void
    PrintSpecialDylibOrdinal(FILE *OutFile, int64_t DylibOrdinal) noexcept;

    static int
    HandleBindOpcodeParseError(FILE *ErrFile,
                               MachO::BindOpcodeParseError Error) noexcept;

    static int
    HandleBindCollectionError(
        FILE *ErrFile,
        MachO::BindActionCollection::Error Error) noexcept;

    static int
    HandleRebaseOpcodeParseError(FILE *ErrFile,
                                 MachO::RebaseOpcodeParseError Error) noexcept;

    // 32 for Max Segment and Section Names, 4 for the apostraphes, and 1 for
    // the comma.

    static constexpr auto SegmentSectionPairMaxLength = 32 + 4 + 1;

    static void
    PrintDylibOrdinalPath(FILE *OutFile,
                          const MachO::SharedLibraryInfoCollection &Collection,
                          int64_t DylibOrdinal) noexcept;

    static void
    PrintDylibOrdinalInfo(FILE *OutFile,
                          const MachO::SharedLibraryInfoCollection &Collection,
                          int64_t DylibOrdinal,
                          bool Verbose) noexcept;

    static void
    ParseSegmentSectionPair(FILE *ErrFile,
                            const std::string_view &Pair,
                            std::string_view &SegmentNameOut,
                            std::string_view &SectionNameOut) noexcept;

    static int
    GetDyldInfoCommand(
        FILE *ErrFile,
        const MachO::ConstLoadCommandStorage &LoadCmdStorage,
        const MachO::DyldInfoCommand *&DyldInfoCommandOut) noexcept;

    static int
    GetBindActionCollection(
        FILE *ErrFile,
        const ConstMemoryMap &Map,
        const MachO::ConstLoadCommandStorage &LoadCmdStorage,
        const MachO::SegmentInfoCollection &SegmentCollection,
        MachO::BindActionCollection &Collection,
        bool Is64Bit) noexcept;
};
