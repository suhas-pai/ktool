//
//  Operations/Common.h
//  ktool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/MachO.h"
#include "Objects/MachOMemory.h"

struct OperationCommon {
    static MachO::LoadCommandStorage
    GetLoadCommandStorage(MachOMemoryObject &Object, FILE *ErrFile) noexcept;

    static MachO::ConstLoadCommandStorage
    GetConstLoadCommandStorage(const MachOMemoryObject &Object,
                               FILE *ErrFile) noexcept;

    constexpr static auto InvalidLCString =
        std::string_view("(Invalid LoadCommand-String)");

    [[nodiscard]] static std::string_view
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
    PrintSpecialDylibOrdinal(FILE *const OutFile, int64_t DylibOrdinal) noexcept;

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

    // 32 for Max Segment and Section Names, 4 for the apostrophes, and 1 for
    // the comma.

    constexpr static auto SegmentSectionPairMaxLength = 32 + 4 + 1;

    static void
    PrintDylibOrdinalPath(FILE *const OutFile,
                          const MachO::SharedLibraryInfoCollection &Collection,
                          int64_t DylibOrdinal) noexcept;

    static void
    PrintDylibOrdinalInfo(FILE *const OutFile,
                          const MachO::SharedLibraryInfoCollection &Collection,
                          int64_t DylibOrdinal,
                          PrintKind Print) noexcept;

    static void
    ParseSegmentSectionPair(FILE *ErrFile,
                            std::string_view Pair,
                            std::string_view &SegmentNameOut,
                            std::string_view &SectionNameOut) noexcept;

    static int
    GetDyldInfoCommand(
        FILE *ErrFile,
        const MachO::ConstLoadCommandStorage &LoadCmdStorage,
        const MachO::DyldInfoCommand *&DyldInfoCommandOut) noexcept;

    static int
    GetBindActionLists(
        FILE *ErrFile,
        const ConstMemoryMap &Map,
        const MachO::SegmentInfoCollection &SegmentCollection,
        const MachO::DyldInfoCommand &DyldInfo,
        bool IsBigEndian,
        bool Is64Bit,
        const MachO::BindActionList *& BindList,
        const MachO::LazyBindActionList *& LazyBindList,
        const MachO::WeakBindActionList *& WeakBindList) noexcept;

    static int
    GetBindActionCollection(
        FILE *ErrFile,
        const ConstMemoryMap &Map,
        const Range &Range,
        const MachO::ConstLoadCommandStorage &LoadCmdStorage,
        const MachO::SegmentInfoCollection &SegmentCollection,
        MachO::BindActionCollection &Collection,
        bool Is64Bit) noexcept;

    struct FlagInfo {
        std::string_view Name;
        std::string_view Description;

        uint32_t Mask;
    };

    [[nodiscard]] static std::vector<FlagInfo>
    GetFlagInfoList(MachO::Header::FlagsType Flags) noexcept;

    static void
    PrintFlagInfoList(FILE *const OutFile,
                      const std::vector<FlagInfo> &FlagInfoList,
                      PrintKind PrintKind,
                      const char *LinePrefix = "",
                      const char *LineSuffix = "") noexcept;

    static int
    HandleExportTrieParseError(FILE *const OutFile,
                               MachO::ExportTrieParseError ParseError) noexcept;
};
