//
//  src/main.cc
//  stool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ADT/Allocation.h"
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

static inline bool IsAtArgcEnd(int Index, int Argc) {
    return (Index == (Argc - 1));
}

int HandleHelp(int Argc, const char *Argv[]) {
    if (!IsAtArgcEnd(1, Argc)) {
        fprintf(stderr, "Unrecognized Argument: %s\n", Argv[2]);
        return 1;
    }

    fputs("Usage: stool <operation> <operation-opts> <path> <path-options>\n",
          stdout);
    return 0;
}

int main(int Argc, const char *Argv[]) {
    if (Argc < 2) {
        fputs("Run --help or --usage to see a list of options\n", stdout);
        return 1;
    }

    // Get the Operation-Kind.

    auto Ops = OperationKind::None;
    auto OpsOptions = TypedAllocation<Operation::Options>();
    auto PathIndex = int();

    switch (Ops) {
        case OperationKind::None:
            if (strcmp(Argv[1], "--help") == 0 ||
                strcmp(Argv[1], "--usage") == 0)
            {
                return HandleHelp(Argc, Argv);
            }

        case OperationKind::PrintHeader:
            if (strcmp(Argv[1], "-h") == 0 ||
                strcmp(Argv[1], "--header") == 0)
            {
                Ops = OperationKind::PrintHeader;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintHeader>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }

        case OperationKind::PrintLoadCommands:
            if (strcmp(Argv[1], "-l") == 0 || strcmp(Argv[1], "-lc") == 0) {
                Ops = OperationKind::PrintLoadCommands;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintLoadCommands>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }

        case OperationKind::PrintSharedLibraries:
            if (strcmp(Argv[1], "-L") == 0) {
                Ops = OperationKind::PrintSharedLibraries;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintSharedLibraries>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }

        case OperationKind::PrintId:
            if (strcmp(Argv[1], "-Id") == 0) {
                Ops = OperationKind::PrintId;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintId>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }

        case OperationKind::PrintArchList:
            if (strcmp(Argv[1], "--list-archs") == 0) {
                Ops = OperationKind::PrintArchList;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintArchList>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }

        case OperationKind::PrintFlags:
            if (strcmp(Argv[1], "--list-flags") == 0) {
                Ops = OperationKind::PrintFlags;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintFlags>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }

        case OperationKind::PrintExportTrie:
            if (strcmp(Argv[1], "--list-export-trie") == 0) {
                Ops = OperationKind::PrintExportTrie;
                OpsOptions =
                    OperationTypeFromKind<OperationKind::PrintExportTrie>::
                        ParseOptions(Argc - 2, Argv + 2, &PathIndex);
                break;
            }
    }

    if (Ops == OperationKind::None) {
        fprintf(stderr, "Unrecognized Operation: %s\n", Argv[1]);
        return 1;
    }

    // Since we gave Operation::Options a [2, Argc] Argv, we have to add two
    // here to get the full index.

    PathIndex += 2;
    if (PathIndex == Argc) {
        fputs("Please provide a file for the operation\n", stderr);
        return 1;
    }

    const auto Path = PathUtil::Absolutify(Argv[PathIndex]);
    const auto Fd =
        FileDescriptor::Open(Path.data(), FileDescriptor::OpenType::Read);

    if (Fd.HasError()) {
        fprintf(stderr,
                "Could not open the provided file (at path: %s), error: %s\n",
                Path.data(),
                strerror(errno));
        return 1;
    }

    auto FileMapProt = MappedFile::Protections();

    FileMapProt.Add(MappedFile::ProtectionsEnum::Read);
    FileMapProt.Add(MappedFile::ProtectionsEnum::Write);

    const auto FileMap =
        MappedFile::Open(Fd, FileMapProt, MappedFile::Type::Private);

    switch (FileMap.GetError()) {
        case MappedFile::OpenError::None:
            break;
        case MappedFile::OpenError::FailedToGetSize:
            fprintf(stderr,
                    "Failed to get file-size of provided file. Error=%s\n",
                    strerror(errno));
            return 1;
        case MappedFile::OpenError::EmptyFile:
            fputs("Provided file is empty\n", stderr);
            return 1;
        case MappedFile::OpenError::MmapCallFailed:
            fprintf(stderr,
                    "Failed to map file to memory. Error=%s\n",
                    strerror(errno));
            return 1;
    }

    auto Object = TypedAllocation(MemoryObject::Open(FileMap));
    if (Object == nullptr) {
        fputs("Provided file has an unrecognized file-format\n", stderr);
        return 1;
    }

    if (Object->HasError()) {
        switch (Object->GetKind()) {
            case ObjectKind::None:
                assert(0 && "MemoryObject::Open() returned ObjectKind::None");
            case ObjectKind::MachO: {
                const auto RealObject = cast<ObjectKind::MachO>(Object);
                switch (RealObject->GetError()) {
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

            case ObjectKind::FatMachO:
                const auto RealObject = cast<ObjectKind::FatMachO>(Object);
                switch (RealObject->GetError()) {
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
    }

    // Parse any arguments for the path.

    for (auto Index = PathIndex + 1; Index != Argc; Index++) {
        if (strcmp(Argv[Index], "-arch") == 0) {
            const auto FatObject = dyn_cast<ObjectKind::FatMachO>(Object);
            if (!FatObject) {
                fputs("Provided file is not a Fat Mach-O File\n", stderr);
                return 1;
            }

            if (IsAtArgcEnd(Index, Argc)) {
                fputs("Please provide an arch-index.\nUse the --list-archs "
                      "option to see a list of archs\n",
                      stderr);
                return 1;
            }

            const auto ArchNumber = ParseNumber<uint32_t>(Argv[++Index]);
            const auto ArchCount = FatObject->GetArchCount();

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

            Object = ArchObject.GetObject();
            if (Object == nullptr) {
                fputs("Provided file's selected arch is of an unrecognized "
                      "kind\n", stderr);
                exit(1);
            }

            using ArchWarningEnum =
                FatMachOMemoryObject::GetObjectResult::WarningEnum;

            switch (ArchObject.GetWarning()) {
                case ArchWarningEnum::None:
                    break;
                case ArchWarningEnum::MachOCpuTypeMismatch:
                    fputs("Warning: Arch's Cputype differs from expected\n",
                          stderr);
                    break;
            }
        } else {
            fprintf(stdout,
                    "Unrecognized Argument for provided path: %s\n",
                    Argv[Index]);
            return 1;
        }
    }

    Operation::Run(Ops, *Object, *OpsOptions);
    return 0;
}
