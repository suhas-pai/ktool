//
//  src/Operations/Common.cpp
//  ktool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <unistd.h>
#include <string_view>

#include "Utils/PrintUtils.h"
#include "Common.h"

static void
HandleLoadCommandStorageError(FILE *ErrFile,
                              MachO::LoadCommandStorage::Error Error) noexcept
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
        case MachO::LoadCommandStorage::Error::SizeOfCmdsTooSmall:
            fprintf(ErrFile, "LoadCommands Buffer is too small\n");
            break;
    }
}

MachO::LoadCommandStorage
OperationCommon::GetLoadCommandStorage(MachOMemoryObject &Object,
                                       FILE *ErrFile) noexcept
{
    const auto LoadCommandStorage = Object.GetLoadCommands();
    HandleLoadCommandStorageError(ErrFile, LoadCommandStorage.getError());

    return LoadCommandStorage;
}

MachO::ConstLoadCommandStorage
OperationCommon::GetConstLoadCommandStorage(
    const ConstMachOMemoryObject &Object,
    FILE *ErrFile) noexcept
{
    const auto LoadCommandStorage = Object.GetLoadCommands();
    HandleLoadCommandStorageError(ErrFile, LoadCommandStorage.getError());

    return LoadCommandStorage;
}

const std::string_view &
OperationCommon::GetLoadCommandStringValue(
    const MachO::LoadCommandString::GetValueResult &Result) noexcept
{
    using namespace std::literals;
    if (Result.hasError()) {
        constexpr static const auto InvalidString =
            "(Invalid LoadCommand-String)"sv;
        return InvalidString;
    }

    return Result.getString();
}

int
OperationCommon::HandleSegmentCollectionError(
    FILE *ErrFile,
    MachO::SegmentInfoCollection::Error Error) noexcept
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
    FILE *ErrFile,
    MachO::SharedLibraryInfoCollection::Error Error) noexcept
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
OperationCommon::PrintSpecialDylibOrdinal(FILE *OutFile,
                                          int64_t DylibOrdinal) noexcept
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
    FILE *ErrFile,
    MachO::BindOpcodeParseError Error) noexcept
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
            fputs("Invalid Bind-Info: Bind-Type not set\n", ErrFile);
            return 1;
    }

    assert(0 && "Unrecognized Bind-Opcode Parse Error");
}

int
OperationCommon::HandleBindCollectionError(
    FILE *ErrFile,
    MachO::BindActionCollection::Error Error) noexcept
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
    FILE *ErrFile,
    MachO::RebaseOpcodeParseError Error) noexcept
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
    FILE *OutFile,
    const MachO::SharedLibraryInfoCollection &Collection,
    int64_t DylibOrdinal) noexcept
{
    if (DylibOrdinal <= 0) {
        return;
    }

    const auto DylibIndex = DylibOrdinal - 1;
    if (IndexOutOfBounds(DylibIndex, Collection.size())) {
        fputs("<out-of-bounds>", OutFile);
        return;
    }

    const auto &Path = Collection.at(DylibIndex).Path;
    fprintf(OutFile, "\"%s\"", Path.data());
}

void
OperationCommon::PrintDylibOrdinalInfo(
    FILE *OutFile,
    const MachO::SharedLibraryInfoCollection &Collection,
    int64_t DylibOrdinal,
    bool Verbose) noexcept
{
    const auto DylibIndex = DylibOrdinal - 1;
    if (DylibOrdinal > 0) {
        if (IndexOutOfBounds(DylibIndex, Collection.size())) {
            fprintf(OutFile,
                    "Dylib-Ordinal %02" PRId64 " (Out Of Bounds!)",
                    DylibOrdinal);
        } else {
            if (Verbose) {
                const auto &Path = Collection.at(DylibIndex).Path;
                fprintf(OutFile,
                        "Dylib-Ordinal %02" PRId64 " - \"%s\"",
                        DylibOrdinal,
                        Path.data());
            } else {
                fprintf(OutFile, "Dylib-Ordinal %02" PRId64, DylibOrdinal);
            }
        }

        return;
    }

    OperationCommon::PrintSpecialDylibOrdinal(OutFile, DylibOrdinal);
}

using namespace std::literals;
constexpr static auto SegmentSectionPairFormat =
    "<segment-name>,<section-name>"sv;

void
OperationCommon::ParseSegmentSectionPair(
    FILE *ErrFile,
    const std::string_view &Pair,
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
    FILE *ErrFile,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const MachO::DyldInfoCommand *&DyldInfoCommandOut) noexcept
{
    auto FoundDyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);
    const auto IsBigEndian = LoadCmdStorage.IsBigEndian();

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto *DyldInfo =
            dyn_cast<MachO::DyldInfoCommand>(LoadCmd, IsBigEndian);

        if (DyldInfo == nullptr) {
            continue;
        }

        if (FoundDyldInfo != nullptr) {
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
OperationCommon::GetBindActionCollection(
    FILE *ErrFile,
    const ConstMemoryMap &Map,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const MachO::SegmentInfoCollection &SegmentCollection,
    MachO::BindActionCollection &BindCollection,
    bool Is64Bit) noexcept
{
    const auto IsBigEndian = LoadCmdStorage.IsBigEndian();

    auto FoundDyldInfo = false;
    auto BindActionList = static_cast<MachO::BindActionList *>(nullptr);
    auto LazyBindActionList = static_cast<MachO::LazyBindActionList *>(nullptr);
    auto WeakBindActionList = static_cast<MachO::WeakBindActionList *>(nullptr);

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto *DyldInfo =
            dyn_cast<MachO::DyldInfoCommand>(LoadCmd, IsBigEndian);

        if (DyldInfo == nullptr) {
            continue;
        }

        auto BindActionOpt =
            DyldInfo->GetBindActionList(Map,
                                        SegmentCollection,
                                        IsBigEndian,
                                        Is64Bit);

        if (BindActionOpt.hasPtr()) {
            BindActionList = BindActionOpt.getPtr();
        }

        auto LazyBindActionOpt =
            DyldInfo->GetLazyBindActionList(Map,
                                            SegmentCollection,
                                            IsBigEndian,
                                            Is64Bit);

        if (LazyBindActionOpt.hasPtr()) {
            LazyBindActionList = LazyBindActionOpt.getPtr();
        }

        auto WeakBindActionOpt =
            DyldInfo->GetWeakBindActionList(Map,
                                            SegmentCollection,
                                            IsBigEndian,
                                            Is64Bit);

        if (WeakBindActionOpt.hasPtr()) {
            WeakBindActionList = WeakBindActionOpt.getPtr();
        }

        BindActionOpt.clear();
        LazyBindActionOpt.clear();
        WeakBindActionOpt.clear();

        FoundDyldInfo = true;
        break;
    }

    auto BindCollectionError = MachO::BindActionCollection::Error::None;
    auto BindCollectionParseError =
        MachO::BindActionCollection::ParseError::None;

    BindCollection =
        MachO::BindActionCollection::Open(SegmentCollection,
                                          BindActionList,
                                          LazyBindActionList,
                                          WeakBindActionList,
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
