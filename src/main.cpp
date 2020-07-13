//
//  src/main.cpp
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ADT/Allocation.h"
#include "ADT/ArgvArray.h"
#include "ADT/FileDescriptor.h"
#include "ADT/MappedFile.h"
#include "ADT/TypedAllocation.h"

#include "Objects/Kind.h"
#include "Objects/MachOMemory.h"
#include "Objects/FatMachOMemory.h"

#include "Operations/Operation.h"

#include "Utils/Casting.h"
#include "Utils/DoesOverflow.h"
#include "Utils/MiscTemplates.h"
#include "Utils/Path.h"
#include "Utils/PrintUtils.h"
#include "Utils/StringUtils.h"

[[nodiscard]]
static bool MatchesOption(OperationKind Kind, const char *Arg) noexcept {
    if (Arg[0] == '-' && Arg[1] != '-') {
        const auto ShortName = Operation::GetOptionShortName(Kind);
        if (!ShortName.empty() && ShortName == (Arg + 1)) {
            return true;
        }
    } else if (Operation::GetOptionName(Kind) == (Arg + 2)) {
        return true;
    }

    return false;
}

[[nodiscard]] static inline bool IsHelpOption(const char *Arg) noexcept {
    if (memcmp(Arg, "--", 2) != 0) {
        return false;
    }

    const auto Result =
        (Operation::HelpOption == (Arg + 2)) ||
        (Operation::UsageOption == (Arg + 2));

    return Result;
}

constexpr static auto UsageString =
    "Usage: ktool [Main-Operation] [Operation-Options] [Path] [Path-Options]\n";

int main(int Argc, const char *Argv[]) {
    if (Argc < 2) {
        fprintf(stdout,
                "Run --%s or --%s to see a list of options\n",
                Operation::HelpOption.data(),
                Operation::UsageOption.data());

        return 0;
    }

    // Get the Operation-Kind.

    auto Ops = TypedAllocation<Operation>();
    auto PathIndex = int();

    if (Argv[1][0] != '-') {
        fprintf(stderr, "Expected Option, Got: \"%s\"\n", Argv[1]);
        return 1;
    }

    if (strcmp(Argv[1], "-") == 0 || strcmp(Argv[1], "--") == 0) {
        fputs("Please provide a non-empty option\n", stderr);
        return 1;
    }

    auto ArgvArr = ArgvArray(Argc, Argv);
    auto OpsKind = OperationKind::None;

    switch (OpsKind) {
        case OperationKind::None:
            if (IsHelpOption(Argv[1])) {
                fputs(UsageString, stdout);
                fputs("Options:\n", stdout);

                Operation::PrintHelpMenu(stdout);
                return 0;
            }
        case OperationKind::PrintHeader:
            if (MatchesOption(OperationKind::PrintHeader, Argv[1])) {
                Ops = new PrintHeaderOperation();
                break;
            }
        case OperationKind::PrintLoadCommands:
            if (MatchesOption(OperationKind::PrintLoadCommands, Argv[1])) {
                Ops = new PrintLoadCommandsOperation();
                break;
            }
        case OperationKind::PrintSharedLibraries:
            if (MatchesOption(OperationKind::PrintSharedLibraries, Argv[1])) {
                Ops = new PrintSharedLibrariesOperation();
                break;
            }
        case OperationKind::PrintId:
            if (MatchesOption(OperationKind::PrintId, Argv[1])) {
                Ops = new PrintIdOperation();
                break;
            }
        case OperationKind::PrintArchList:
            if (MatchesOption(OperationKind::PrintArchList, Argv[1])) {
                Ops = new PrintArchListOperation();
                break;
            }
        case OperationKind::PrintFlags:
            if (MatchesOption(OperationKind::PrintFlags, Argv[1])) {
                Ops = new PrintFlagsOperation();
                break;
            }
        case OperationKind::PrintExportTrie:
            if (MatchesOption(OperationKind::PrintExportTrie, Argv[1])) {
                Ops = new PrintExportTrieOperation();
                break;
            }
        case OperationKind::PrintObjcClassList:
            if (MatchesOption(OperationKind::PrintObjcClassList, Argv[1])) {
                Ops = new PrintObjcClassListOperation();
                break;
            }
        case OperationKind::PrintBindActionList:
            if (MatchesOption(OperationKind::PrintBindActionList, Argv[1])) {
                Ops = new PrintBindActionListOperation();
                break;
            }
        case OperationKind::PrintBindOpcodeList:
            if (MatchesOption(OperationKind::PrintBindOpcodeList, Argv[1])) {
                Ops = new PrintBindOpcodeListOperation();
                break;
            }
        case OperationKind::PrintBindSymbolList:
            if (MatchesOption(OperationKind::PrintBindSymbolList, Argv[1])) {
                Ops = new PrintBindSymbolListOperation();
                break;
            }
        case OperationKind::PrintRebaseActionList:
            if (MatchesOption(OperationKind::PrintRebaseActionList, Argv[1])) {
                Ops = new PrintRebaseActionListOperation();
                break;
            }
        case OperationKind::PrintRebaseOpcodeList:
            if (MatchesOption(OperationKind::PrintRebaseOpcodeList, Argv[1])) {
                Ops = new PrintRebaseOpcodeListOperation();
                break;
            }
        case OperationKind::PrintCStringSection:
            if (MatchesOption(OperationKind::PrintCStringSection, Argv[1])) {
                Ops = new PrintCStringSectionOperation();
                break;
            }
        case OperationKind::PrintSymbolPtrSection:
            if (MatchesOption(OperationKind::PrintSymbolPtrSection, Argv[1])) {
                Ops = new PrintSymbolPtrSectionOperation();
                break;
            }
        case OperationKind::PrintImageList:
            if (MatchesOption(OperationKind::PrintImageList, Argv[1])) {
                Ops = new PrintImageListOperation();
                break;
            }
    }

    if (Ops == nullptr) {
        fprintf(stderr,
                "Unrecognized Option: \"%s\".\n"
                "Use Option --%s or --%s to see a list of options\n",
                Argv[1],
                Operation::HelpOption.data(),
                Operation::UsageOption.data());
        return 1;
    }

    const auto OpsArgv = ArgvArr.fromIndex(2);
    if (!OpsArgv.empty()) {
        const auto &Front = OpsArgv.front();
        if (strcmp(Front, "--help") == 0) {
            if (OpsArgv.count() != 1) {
                fputs("Option --help should be run separately\n", stderr);
                return 1;
            }
            
            const auto OpsKind = Ops->getKind();
            fprintf(stdout,
                    "Usage: --%s [Options] [Path] [Path-Options]\n"
                    "Options:\n",
                    Operation::GetOptionName(OpsKind).data());

            Operation::PrintOptionHelpMenu(OpsKind, stdout, "\t");
            return 0;
        }
    }

    Ops->ParseOptions(OpsArgv, &PathIndex);

    // Since we gave Operation::Options a [2, Argc] Argv, we have to add two
    // here to get the full index.

    PathIndex += 2;
    if (PathIndex == Argc) {
        fprintf(stderr,
                "Please provide a file for operation %s\n",
                OperationKindGetName(Ops->getKind()).data());
        return 1;
    }

    const auto Path = PathUtil::Absolutify(Argv[PathIndex]);
    const auto Fd =
        FileDescriptor::Open(Path.data(), FileDescriptor::OpenType::Read);

    if (Fd.hasError()) {
        fprintf(stderr,
                "Could not open the provided file (at path: %s), error: %s\n",
                Path.data(),
                strerror(errno));
        return 1;
    }

    auto FileMapProt = MappedFile::Protections();

    FileMapProt.add(MappedFile::ProtectionsEnum::Read);
    FileMapProt.add(MappedFile::ProtectionsEnum::Write);

    const auto FileMap =
        MappedFile::Open(Fd, FileMapProt, MappedFile::Type::Private);

    switch (FileMap.getError()) {
        case MappedFile::OpenError::None:
            break;
        case MappedFile::OpenError::FailedToGetSize:
            fprintf(stderr,
                    "Failed to get file-size of provided file. Error: %s\n",
                    strerror(errno));
            return 1;
        case MappedFile::OpenError::EmptyFile:
            fputs("Provided file is empty\n", stderr);
            return 1;
        case MappedFile::OpenError::MmapCallFailed:
            fprintf(stderr,
                    "Failed to map file to memory. Error: %s\n",
                    strerror(errno));
            return 1;
    }

    auto Object = TypedAllocation(MemoryObject::Open(FileMap));
    if (Object == nullptr) {
        fputs("Provided file has an unsupported file-format\n", stderr);
        return 1;
    }

    if (Object->hasError()) {
        switch (Object->getKind()) {
            case ObjectKind::None:
                assert(0 && "MemoryObject::Open() returned ObjectKind::None");
            case ObjectKind::MachO: {
                const auto RealObject = cast<ObjectKind::MachO>(Object);
                switch (RealObject->getError()) {
                    case MachOMemoryObject::Error::None:
                    case MachOMemoryObject::Error::WrongFormat:
                    case MachOMemoryObject::Error::SizeTooSmall:
                        assert(0 && "Got Unhandled errors in main");
                    case MachOMemoryObject::Error::TooManyLoadCommands:
                        fputs("Provided File has an invalid LoadCommands "
                              "buffer\n", stderr);
                        return 1;
                }

                break;
            }

            case ObjectKind::FatMachO: {
                const auto RealObject = cast<ObjectKind::FatMachO>(Object);
                switch (RealObject->getError()) {
                    case FatMachOMemoryObject::Error::None:
                    case FatMachOMemoryObject::Error::WrongFormat:
                    case FatMachOMemoryObject::Error::SizeTooSmall:
                        assert(0 && "Got Unhandled errors in main");
                    case FatMachOMemoryObject::Error::ZeroArchitectures:
                        fputs("Provided File has zero architectures\n",
                              stderr);
                        return 1;
                    case FatMachOMemoryObject::Error::TooManyArchitectures:
                        fputs("Provided has too many architectures for its "
                              "size", stderr);
                        return 1;
                    case FatMachOMemoryObject::Error::ArchOutOfBounds:
                        fputs("Provided file has architecture(s) out of "
                              "bounds\n", stderr);
                        return 1;
                    case FatMachOMemoryObject::Error::ArchOverlapsArch:
                        fputs("Provided file has overlapping architectures\n",
                              stderr);
                        return 1;
                }

                break;
            }

            case ObjectKind::DyldSharedCache: {
                const auto RealObject =
                    cast<ObjectKind::DyldSharedCache>(Object);

                switch (RealObject->getError()) {
                    case DscMemoryObject::Error::None:
                    case DscMemoryObject::Error::WrongFormat:
                    case DscMemoryObject::Error::SizeTooSmall:
                        assert(0 && "Got Unhandled errors in main");
                    case DscMemoryObject::Error::UnknownCpuKind:
                        fputs("Provided file is a dyld-shared-cache file with "
                              "an unknown cpu-kind\n",
                              stderr);
                        return 1;
                    case DscMemoryObject::Error::InvalidMappingRange:
                        fputs("Provided file is a dyld-shared-cache file with "
                              "an invalid mapping-range\n",
                              stderr);
                        return 1;
                    case DscMemoryObject::Error::InvalidImageRange:
                        fputs("Provided file is a dyld-shared-cache file with "
                              "an unknown image-range\n",
                              stderr);
                        return 1;
                    case DscMemoryObject::Error::OverlappingImageMappingRange:
                        fputs("Provided file is a dyld_shared_cache file with "
                              "Overlapping Image-Info and Mapping-Info "
                              "Ranges\n",
                              stderr);
                        return 1;
                }

                break;
            }
        }
    }

    // Parse any arguments for the path.

    for (auto &Argument : ArgvArray(Argv, PathIndex + 1, Argc)) {
        if (strcmp(Argument, "-arch") == 0) {
            const auto FatObject = dyn_cast<ObjectKind::FatMachO>(Object);
            if (FatObject == nullptr) {
                fputs("-arch option not allowed: Provided file is not a Fat "
                      "Mach-O File\n",
                      stderr);
                return 1;
            }

            if (!Argument.hasNext()) {
                fputs("Please provide an arch-index.\nUse the --list-archs "
                      "option to see a list of archs\n",
                      stderr);
                return 1;
            }

            Argument.advance();

            const auto ArchNumber = ParseNumber<uint32_t>(Argument.getString());
            const auto ArchCount = FatObject->getArchCount();

            if (ArchNumber == 0) {
                fputs("An ArchNumber of 0 is invalid. For the first arch, use "
                      "-arch 1\n",
                      stderr);
                return 1;
            }

            const auto ArchIndex = ArchNumber - 1;
            if (IndexOutOfBounds(ArchIndex, ArchCount)) {
                fprintf(stderr,
                        "Provided file only has %" PRIu32 " archs\n",
                        ArchCount);
                return 1;
            }

            const auto ArchInfo = FatObject->GetArchInfoAtIndex(ArchIndex);
            const auto ArchObject = FatObject->GetArchObjectFromInfo(ArchInfo);

            Object = ArchObject.getObject();
            if (Object == nullptr) {
                fputs("Provided file's selected arch is of an unrecognized "
                      "kind\n", stderr);
                exit(1);
            }

            using ArchWarningEnum =
                FatMachOMemoryObject::GetObjectResult::WarningEnum;

            switch (ArchObject.getWarning()) {
                case ArchWarningEnum::None:
                    break;
                case ArchWarningEnum::MachOCpuKindMismatch:
                    fputs("Warning: Arch's Cputype differs from expected\n",
                          stderr);
                    break;
            }
        } else {
            fprintf(stdout,
                    "Unrecognized Argument for provided path: %s\n",
                    Argument.getString());
            return 1;
        }
    }

    const auto Result = Ops->Run(*Object);
    if (Result == Operation::InvalidObjectKind) {
        Operation::PrintObjectKindNotSupportedError(Ops->getKind(), *Object);
    }

    return 0;
}
