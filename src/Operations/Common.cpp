//
//  Operations/Common.cpp
//  ktool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <string_view>

#include "ADT/MemoryMap.h"
#include "Operations/Common.h"
#include "Utils/PrintUtils.h"

using namespace std::literals;

static void
HandleLoadCommandStorageError(
    FILE *const ErrFile,
    const MachO::LoadCommandStorage::Error Error) noexcept
{
    switch (Error) {
        case MachO::LoadCommandStorage::Error::None:
            break;
        case MachO::LoadCommandStorage::Error::NoLoadCommands:
            fprintf(ErrFile, "Provided file has no load-commands\n");
            break;
        case MachO::LoadCommandStorage::Error::CmdSizeTooSmall:
            fprintf(ErrFile,
                    "LoadCommands Buffer has an invalid LoadCommand (CmdSize "
                    "Too Small)\n");
            break;
        case MachO::LoadCommandStorage::Error::CmdSizeNotAligned:
            fprintf(ErrFile,
                    "LoadCommands Buffer has an invalid LoadCommand (CmdSize "
                    "Not Aligned)\n");
            break;
        case MachO::LoadCommandStorage::Error::StorageSizeTooSmall:
            fprintf(ErrFile, "LoadCommands Buffer is too small\n");
            break;
    }
}

auto
OperationCommon::GetLoadCommandStorage(MachOMemoryObject &Object,
                                       FILE *const ErrFile) noexcept
    -> MachO::LoadCommandStorage
{
    const auto LoadCommandStorage = Object.GetLoadCommandsStorage();
    const auto Error = LoadCommandStorage.getError();

    if (Error != MachO::ConstLoadCommandStorage::Error::None) {
        HandleLoadCommandStorageError(ErrFile, Error);
        exit(1);
    }

    return LoadCommandStorage;
}

auto
OperationCommon::GetConstLoadCommandStorage(
    const MachOMemoryObject &Object,
    FILE *const ErrFile) noexcept
        -> MachO::ConstLoadCommandStorage
{
    const auto LoadCommandStorage = Object.GetLoadCommandsStorage();
    const auto Error = LoadCommandStorage.getError();

    if (Error != MachO::ConstLoadCommandStorage::Error::None) {
        HandleLoadCommandStorageError(ErrFile, Error);
        exit(1);
    }

    return LoadCommandStorage;
}

auto
OperationCommon::GetLoadCommandStringValue(
    const MachO::LoadCommandString::GetValueResult &Result) noexcept
        -> std::string_view
{
    using namespace std::literals;
    if (Result.hasError()) {
        return InvalidLCString;
    }

    return Result.getString();
}

int
OperationCommon::HandleSegmentCollectionError(
    FILE *const ErrFile,
    const MachO::SegmentInfoCollection::Error Error) noexcept
{
    switch (Error) {
        case MachO::SegmentInfoCollection::Error::None:
            return 0;
        case MachO::SegmentInfoCollection::Error::InvalidSegment:
            fputs("Provided File has an Invalid Segment\n", ErrFile);
            return 1;
        case MachO::SegmentInfoCollection::Error::OverlappingSegments:
            fputs("Provided File has Overlapping Segments\n", ErrFile);
            return 1;
        case MachO::SegmentInfoCollection::Error::InvalidSectionList:
            fputs("Provided File has a Segment with an invalid Section-List\n",
                  ErrFile);
            return 1;
        case MachO::SegmentInfoCollection::Error::InvalidSection:
            fputs("Provided File has a Segment with an invalid Section\n",
                  ErrFile);
            return 1;
        case MachO::SegmentInfoCollection::Error::OverlappingSections:
            fputs("Provided file has a Segment with Overlapping Sections\n",
                  ErrFile);
            return 1;
    }

    assert(0 && "Unrecognized Segment-Collection Error");
}

int
OperationCommon::HandleSharedLibraryCollectionError(
    FILE *const ErrFile,
    const MachO::SharedLibraryInfoCollection::Error Error) noexcept
{
    switch (Error) {
        case MachO::SharedLibraryInfoCollection::Error::None:
            return 0;
        case MachO::SharedLibraryInfoCollection::Error::InvalidPath:
            fputs("Provided File has a invalid path for a shared-library\n",
                  ErrFile);
            return 1;
    }

    assert(0 && "Unrecognized Shared-Library Collection Error");
}

void
OperationCommon::PrintSpecialDylibOrdinal(FILE *const OutFile,
                                          const int64_t DylibOrdinal) noexcept
{
    switch (MachO::BindByteDylibSpecialOrdinal(DylibOrdinal)) {
        case MachO::BindByteDylibSpecialOrdinal::DylibSelf:
            fputs("In Self", OutFile);
            return;
        case MachO::BindByteDylibSpecialOrdinal::DylibMainExecutable:
            fputs("In Main Executable", OutFile);
            return;
        case MachO::BindByteDylibSpecialOrdinal::DylibFlatLookup:
            fputs("In Dylib (Flat Lookup)", OutFile);
            return;
        case MachO::BindByteDylibSpecialOrdinal::DylibWeakLookup:
            fputs("In Dylib (Weak Lookup)", OutFile);
            return;
    }

    assert(0 && "Unrecognized Special Dylib-Ordinal");
}

int
OperationCommon::HandleBindOpcodeParseError(
    FILE *const ErrFile,
    const MachO::BindOpcodeParseError Error) noexcept
{
    switch (Error) {
        case MachO::BindOpcodeParseError::None:
            return 0;
        case MachO::BindOpcodeParseError::InvalidLeb128:
            fputs("Invalid Bind-Info: Got Invalid Leb128\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::InvalidSegmentIndex:
            fputs("Invalid Bind-Info: Got Invalid Segment-Index\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::NotEnoughThreadedBinds:
            fputs("Invalid Bind-Info: Too many Threaded-Binds\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::TooManyThreadedBinds:
            fputs("Invalid Bind-Info: Too many Threaded-Binds\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::InvalidThreadOrdinal:
            fputs("Invalid Bind-Info: Got Invalid Thread-Ordinal\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::InvalidString:
            fputs("Invalid Bind-Info: Got Invalid String\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::EmptySymbol:
            fputs("Invalid Bind-Info: Got Empty Symbol\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::IllegalBindOpcode:
            fputs("Invalid Bind-Info: Got an Illegal Bind-Opcode\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::OutOfBoundsSegmentAddr:
            fputs("Invalid Bind-Info: Got Out-Of-Bounds Segment-Address\n",
                  ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::UnrecognizedBindWriteKind:
            fputs("Invalid Bind-Info: Got Invalid Bind Write-Kind\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::UnrecognizedBindOpcode:
            fputs("Invalid Bind-Info: Got unrecognized Bind-Opcode", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::UnrecognizedBindSubOpcode:
            fputs("Invalid Bind-Info: Got unrecognized Bind Sub-Opcode",
                  ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::UnrecognizedSpecialDylibOrdinal:
            fputs("Invalid Bind-Info: Got unrecognized Dylib Special-Ordinal",
                  ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::NoDylibOrdinal:
            fputs("Invalid Bind-Info: Dylib-Ordinal not set\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::NoSegmentIndex:
            fputs("Invalid Bind-Info: Segment-Index not set\n", ErrFile);
            return 1;
        case MachO::BindOpcodeParseError::NoWriteKind:
            fputs("Invalid Bind-Info: Bind Write-Kind not set\n", ErrFile);
            return 1;
    }

    assert(0 && "Unrecognized Bind-Opcode Parse Error");
}

int
OperationCommon::HandleBindCollectionError(
    FILE *const ErrFile,
    const MachO::BindActionCollection::Error Error) noexcept
{
    switch (Error) {
        case MachO::BindActionCollection::Error::None:
            return 0;
        case MachO::BindActionCollection::Error::MultipleBindsForAddress:
            fputs("Invalid Bind-Info: Multiple Bind Actions for a single "
                  "address\n",
                  ErrFile);
            return 1;
    }

    assert(0 && "Unrecognized BindActionCollection-Error");
}

int
OperationCommon::HandleRebaseOpcodeParseError(
    FILE *const ErrFile,
    const MachO::RebaseOpcodeParseError Error) noexcept
{
    switch (Error) {
        case MachO::RebaseOpcodeParseError::None:
            return 0;
        case MachO::RebaseOpcodeParseError::InvalidLeb128:
            fputs("Invalid Rebase-Info: Found Invalid Leb128\n", ErrFile);
            return 1;
        case MachO::RebaseOpcodeParseError::IllegalRebaseOpcode:
            fputs("Invalid Rebase-Info: Found an Illegal Rebase-Opcode\n",
                  ErrFile);
            return 1;
        case MachO::RebaseOpcodeParseError::OutOfBoundsSegmentAddr:
            fputs("Invalid Rebase-Info: Found Out-Of-Bounds Segment-Address\n",
                  ErrFile);
            return 1;
        case MachO::RebaseOpcodeParseError::UnrecognizedRebaseOpcode:
            fputs("Invalid Rebase-Info: Encountered unrecognized Bind-Opcode",
                  ErrFile);
            return 1;
        case MachO::RebaseOpcodeParseError::UnrecognizedRebaseWriteKind:
            fputs("Invalid Bind-Info: Found Invalid Bind-Type\n", ErrFile);
            return 1;
        case MachO::RebaseOpcodeParseError::NoSegmentIndex:
            fputs("Invalid Rebase-Info: Segment-Index not set\n", ErrFile);
            return 1;
        case MachO::RebaseOpcodeParseError::NoWriteKind:
            fputs("Invalid Rebase-Info: Rebase-Type not set\n", ErrFile);
            return 1;
    }

    assert(0 && "Unrecognized Rebase-Opcode Parse Error");
}

void
OperationCommon::PrintDylibOrdinalPath(
    FILE *const OutFile,
    const MachO::SharedLibraryInfoCollection &Collection,
    const int64_t DylibOrdinal) noexcept
{
    if (DylibOrdinal <= 0) {
        fputs("<Invalid Dylib-Ordinal>", OutFile);
        return;
    }

    const auto DylibIndex = DylibOrdinal - 1;
    if (IndexOutOfBounds(DylibIndex, Collection.size())) {
        fputs("<out-of-bounds>", OutFile);
        return;
    }

    const auto &Path = Collection.at(DylibIndex).getPath();
    fprintf(OutFile, "\"%s\"", Path.data());
}

void
OperationCommon::PrintDylibOrdinalInfo(
    FILE *const OutFile,
    const MachO::SharedLibraryInfoCollection &Collection,
    const int64_t DylibOrdinal,
    const PrintKind PrintKind) noexcept
{
    const auto DylibIndex = DylibOrdinal - 1;
    if (DylibOrdinal <= 0) {
        OperationCommon::PrintSpecialDylibOrdinal(OutFile, DylibOrdinal);
        return;
    }

    if (IndexOutOfBounds(DylibIndex, Collection.size())) {
        fprintf(OutFile,
                "Dylib-Ordinal %02" PRId64 " (Out Of Bounds!)",
                DylibOrdinal);
        return;
    }

    if (PrintKindIsVerbose(PrintKind)) {
        const auto &Path = Collection.at(DylibIndex).getPath();
        fprintf(OutFile,
                "Dylib-Ordinal %02" PRId64 " - \"%s\"",
                DylibOrdinal,
                Path.data());
    } else {
        fprintf(OutFile, "Dylib-Ordinal %02" PRId64, DylibOrdinal);
    }
}

constexpr static auto SegmentSectionPairFormat =
    "<segment-name>,<section-name>"sv;

void
OperationCommon::ParseSegmentSectionPair(
    FILE *const ErrFile,
    const std::string_view Pair,
    std::string_view &SegmentNameOut,
    std::string_view &SectionNameOut) noexcept
{
    if (Pair.front() == '/') {
        fprintf(ErrFile,
                "Expected a Segment-Section Pair, Got Path Instead.\n"
                "Format: %s\n",
                SegmentSectionPairFormat.data());
        exit(1);
    }

    const auto Comma = Pair.find(',');
    if (Comma == std::string_view::npos) {
        fprintf(ErrFile,
                "Invalid Segment-Section Pair: %s\nFormat: %s\n",
                Pair.data(),
                SegmentSectionPairFormat.data());
        exit(1);
    }

    if (Comma == 0) {
        fprintf(ErrFile,
                "Invalid Segment-Section Pair: %s (No Segment-Name Provided)\n"
                "Format: %s\n",
                Pair.data(),
                SegmentSectionPairFormat.data());
        exit(1);
    }

    if (Comma == (Pair.length() - 1)) {
        fprintf(ErrFile,
                "Invalid Segment-Section Pair: %s (No Section-Name Provided)\n"
                "Format: %s\n",
                Pair.data(),
                SegmentSectionPairFormat.data());
        exit(1);
    }

    SegmentNameOut = Pair.substr(0, Comma);
    SectionNameOut = Pair.substr(Comma + 1);

    if (SegmentNameOut.length() > 16) {
        fprintf(ErrFile,
                "Invalid Segment-Section Pair: %s (Segment-Name Too Long)\n"
                "Format: %s\n",
                Pair.data(),
                SegmentSectionPairFormat.data());
        exit(1);
    }

    if (SectionNameOut.length() > 16) {
        fprintf(ErrFile,
                "Invalid Segment-Section Pair: %s (Section-Name Too Long)\n"
                "Format: %s\n",
                Pair.data(),
                SegmentSectionPairFormat.data());
        exit(1);
    }
}

int
OperationCommon::GetDyldInfoCommand(
    FILE *const ErrFile,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const MachO::DyldInfoCommand *&DyldInfoCommandOut) noexcept
{
    auto FoundDyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);
    const auto IsBE = LoadCmdStorage.isBigEndian();

    for (const auto &LC : LoadCmdStorage) {
        const auto *DyldInfo = dyn_cast<MachO::DyldInfoCommand>(LC, IsBE);
        if (DyldInfo == nullptr) {
            continue;
        }

        if (FoundDyldInfo != nullptr) {
            // Compare without swapping as they are all the same endian.
            if (DyldInfo->BindOff != FoundDyldInfo->BindOff ||
                DyldInfo->BindSize != FoundDyldInfo->BindSize)
            {
                fputs("Provided file has multiple (conflicting) Bind-List "
                      "information\n",
                      ErrFile);
                return 1;
            }

            if (DyldInfo->LazyBindOff != FoundDyldInfo->LazyBindOff ||
                DyldInfo->LazyBindSize != FoundDyldInfo->LazyBindSize)
            {
                fputs("Provided file has multiple (conflicting) Lazy-Bind "
                      "list information\n",
                      ErrFile);
                return 1;
            }

            if (DyldInfo->WeakBindOff != FoundDyldInfo->WeakBindOff ||
                DyldInfo->WeakBindSize != FoundDyldInfo->WeakBindSize)
            {
                fputs("Provided file has multiple (conflicting) Weak-Bind "
                      "list information\n",
                      ErrFile);
                return 1;
            }
        }

        FoundDyldInfo = DyldInfo;
    }

    if (FoundDyldInfo == nullptr) {
        fputs("Provided file does not have a Dyld-Info Load Command\n",
              ErrFile);
        return 1;
    }

    DyldInfoCommandOut = FoundDyldInfo;
    return 0;
}

int
OperationCommon::GetBindActionLists(
    FILE *const ErrFile,
    const ConstMemoryMap &Map,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const MachO::DyldInfoCommand &DyldInfo,
    const bool IsBigEndian,
    const bool Is64Bit,
    const MachO::BindActionList *& BindList,
    const MachO::LazyBindActionList *& LazyBindList,
    const MachO::WeakBindActionList *& WeakBindList) noexcept
{
    auto BindActionOpt =
        DyldInfo.GetBindActionList(Map,
                                   SegmentCollection,
                                   IsBigEndian,
                                   Is64Bit);

    switch (BindActionOpt.getError()) {
        case MachO::SizeRangeError::None:
        case MachO::SizeRangeError::Empty:
            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Bind-List goes past end of file\n", ErrFile);
            return 1;
    }

    auto LazyBindActionOpt =
        DyldInfo.GetLazyBindActionList(Map,
                                       SegmentCollection,
                                       IsBigEndian,
                                       Is64Bit);

    switch (LazyBindActionOpt.getError()) {
        case MachO::SizeRangeError::None:
        case MachO::SizeRangeError::Empty:
            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Lazy-Bind List goes past end of file\n", ErrFile);
            return 1;
    }

    auto WeakBindActionOpt =
        DyldInfo.GetWeakBindActionList(Map,
                                       SegmentCollection,
                                       IsBigEndian,
                                       Is64Bit);

    switch (WeakBindActionOpt.getError()) {
        case MachO::SizeRangeError::None:
        case MachO::SizeRangeError::Empty:
            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Weak-Bind goes past end of file\n", ErrFile);
            return 1;
    }

    BindList = BindActionOpt.take();
    LazyBindList = LazyBindActionOpt.take();
    WeakBindList = WeakBindActionOpt.take();

    return 0;
}

int
OperationCommon::GetBindActionCollection(
    FILE *const ErrFile,
    const ConstMemoryMap &Map,
    const Range &Range,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const MachO::SegmentInfoCollection &SegmentCollection,
    MachO::BindActionCollection &BindCollection,
    const bool Is64Bit) noexcept
{
    auto DyldInfoCmd = static_cast<const MachO::DyldInfoCommand *>(nullptr);

    const auto IsBigEndian = LoadCmdStorage.isBigEndian();
    const auto GetDyldInfoCmdResult =
        OperationCommon::GetDyldInfoCommand(ErrFile,
                                            LoadCmdStorage,
                                            DyldInfoCmd);

    if (GetDyldInfoCmdResult != 0) {
        return GetDyldInfoCmdResult;
    }

    const auto BindActionOpt =
        DyldInfoCmd->GetBindActionList(Map,
                                       SegmentCollection,
                                       IsBigEndian,
                                       Is64Bit);

    const auto LazyBindActionOpt =
        DyldInfoCmd->GetLazyBindActionList(Map,
                                           SegmentCollection,
                                           IsBigEndian,
                                           Is64Bit);

    const auto WeakBindActionOpt =
        DyldInfoCmd->GetWeakBindActionList(Map,
                                           SegmentCollection,
                                           IsBigEndian,
                                           Is64Bit);

    auto BindCollectionError = MachO::BindActionCollection::Error::None;
    auto BindCollectionParseError =
        MachO::BindActionCollection::ParseError::None;

    BindCollection =
        MachO::BindActionCollection::Open(SegmentCollection,
                                          BindActionOpt.value(),
                                          LazyBindActionOpt.value(),
                                          WeakBindActionOpt.value(),
                                          Range,
                                          &BindCollectionParseError,
                                          &BindCollectionError);

    const auto BindCollectionParseErrorHandleResult =
        OperationCommon::HandleBindOpcodeParseError(ErrFile,
                                                    BindCollectionParseError);

    if (BindCollectionParseErrorHandleResult != 0) {
        return BindCollectionParseErrorHandleResult;
    }

    const auto BindCollectionErrorHandleResult =
        OperationCommon::HandleBindCollectionError(ErrFile,
                                                   BindCollectionError);

    if (BindCollectionErrorHandleResult != 0) {
        return BindCollectionErrorHandleResult;
    }

    return 0;
}

constexpr static int LongestFlagNameLength =
    MachO::Header::FlagsEnumGetName(
        MachO::Header::FlagsEnum::NlistOutOfSyncWithDyldInfo)->length();

auto
OperationCommon::GetFlagInfoList(
    const MachO::Header::FlagsType Flags) noexcept
        -> std::vector<OperationCommon::FlagInfo>
{
    using namespace MachO;

    const auto MachOFlag = Header::FlagsEnum::NoUndefinedReferences;
    auto FlagInfoList = std::vector<FlagInfo>();

    switch (MachOFlag) {
        using Enum = Header::FlagsEnum;
        case Enum::NoUndefinedReferences: {
            constexpr auto Kind = Enum::NoUndefinedReferences;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::IncrementalLink: {
            constexpr auto Kind = Enum::IncrementalLink;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::DyldLink: {
            constexpr auto Kind = Enum::DyldLink;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::BindAtLoad: {
            constexpr auto Kind = Enum::BindAtLoad;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::PreBound: {
            constexpr auto Kind = Enum::PreBound;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::SplitSegments: {
            constexpr auto Kind = Enum::SplitSegments;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::LazyInitialization: {
            constexpr auto Kind = Enum::LazyInitialization;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::TwoLevelNamespaceBindings: {
            constexpr auto Kind = Enum::TwoLevelNamespaceBindings;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::ForceFlatNamespaces: {
            constexpr auto Kind = Enum::ForceFlatNamespaces;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::NoMultipleDefinitions: {
            constexpr auto Kind = Enum::NoMultipleDefinitions;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::NoFixPrebinding: {
            constexpr auto Kind = Enum::NoFixPrebinding;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::Prebindable: {
            constexpr auto Kind = Enum::Prebindable;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::AllModulesBound: {
            constexpr auto Kind = Enum::AllModulesBound;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::SubsectionsViaSymbols: {
            constexpr auto Kind = Enum::SubsectionsViaSymbols;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::Canonical: {
            constexpr auto Kind = Enum::Canonical;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::WeakDefinitions: {
            constexpr auto Kind = Enum::WeakDefinitions;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::BindsToWeak: {
            constexpr auto Kind = Enum::BindsToWeak;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::AllowStackExecution: {
            constexpr auto Kind = Enum::AllowStackExecution;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::RootSafe: {
            constexpr auto Kind = Enum::RootSafe;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::SetuidSafe: {
            constexpr auto Kind = Enum::SetuidSafe;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::NoReexportedDylibs: {
            constexpr auto Kind = Enum::NoReexportedDylibs;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::PositionIndependentExecutable: {
            constexpr auto Kind = Enum::PositionIndependentExecutable;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::DeadStrippableDylib: {
            constexpr auto Kind = Enum::DeadStrippableDylib;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::HasTlvDescriptors: {
            constexpr auto Kind = Enum::HasTlvDescriptors;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::NoHeapExecution: {
            constexpr auto Kind = Enum::NoHeapExecution;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::AppExtensionSafe: {
            constexpr auto Kind = Enum::AppExtensionSafe;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::NlistOutOfSyncWithDyldInfo: {
            constexpr auto Kind = Enum::NlistOutOfSyncWithDyldInfo;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::SimulatorSupport: {
            constexpr auto Kind = Enum::SimulatorSupport;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
        case Header::FlagsEnum::DylibInCache: {
            constexpr auto Kind = Enum::DylibInCache;
            constexpr auto NameOpt = Header::FlagsEnumGetName(Kind);
            constexpr auto DescOpt = Header::FlagsEnumGetDescription(Kind);

            static_assert(NameOpt->length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (Flags.hasFlag(Kind)) {
                FlagInfoList.push_back({
                    .Name = NameOpt.value(),
                    .Description = DescOpt.value(),
                    .Mask = static_cast<uint32_t>(Kind)
                });
            }
        }
    }

    return FlagInfoList;
}

void
OperationCommon::PrintFlagInfoList(FILE *const OutFile,
                                   const std::vector<FlagInfo> &FlagInfoList,
                                   const PrintKind PrintKind,
                                   const char *const LinePrefix,
                                   const char *const LineSuffix) noexcept
{
    auto LongestFlagNameLength = LargestIntHelper();
    for (const auto &Info : FlagInfoList) {
        LongestFlagNameLength = Info.Name.length();
    }

    auto FlagNumber = static_cast<uint32_t>(1);
    for (const auto &Info : FlagInfoList) {
        fprintf(OutFile,
                "%sFlag %02" PRIu32 ": %" PRINTF_RIGHTPAD_FMT "s",
                LinePrefix,
                FlagNumber,
                static_cast<int>(LongestFlagNameLength),
                Info.Name.data());

        if (PrintKindIsVerbose(PrintKind)) {
            fprintf(OutFile,
                    " (%s)%s\n",
                    Info.Description.data(),
                    LineSuffix);
        } else {
            fprintf(OutFile, "%s\n", LineSuffix);
        }

        FlagNumber++;
    }
}

int
OperationCommon::HandleExportTrieParseError(
    FILE *const ErrFile,
    const MachO::ExportTrieParseError ParseError) noexcept
{
    switch (ParseError) {
        case MachO::ExportTrieParseError::None:
            return 0;
        case MachO::ExportTrieParseError::InvalidUleb128:
            fputs("Provided file has an export-trie with an invalid Uleb128\n",
                  ErrFile);
            return 1;
        case MachO::ExportTrieParseError::InvalidFormat:
            fputs("Provided file has an export-trie with an invalid format\n",
                  ErrFile);
            return 1;
        case MachO::ExportTrieParseError::OverlappingRanges:
            fputs("Provided file has an export-trie with overlapping nodes\n",
                  ErrFile);
            return 1;
        case MachO::ExportTrieParseError::EmptyExport:
            fputs("Provided file has an export-trie with an empty export\n",
                  ErrFile);
            return 1;
        case MachO::ExportTrieParseError::TooDeep:
            fputs("Provided file has an export-trie that's too deep\n",
                  ErrFile);
            return 1;
    }
}
