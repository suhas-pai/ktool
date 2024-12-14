//
//  main.cpp
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>
#include <unistd.h>

#include "ADT/ArgvArray.h"
#include "ADT/FileDescriptor.h"
#include "ADT/MappedFile.h"

#include "Objects/DscMemory.h"
#include "Objects/Kind.h"
#include "Objects/MachOMemory.h"
#include "Objects/FatMachOMemory.h"

#include "Operations/Operation.h"

#include "Utils/MiscTemplates.h"
#include "Utils/Path.h"
#include "Utils/StringUtils.h"

static void PrintRunHelpMessage() noexcept {
    fprintf(stderr,
            "Use Option --%s or --%s to see a list of options\n",
            Operation::HelpOption.data(),
            Operation::UsageOption.data());
}

static void PrintUnrecognizedOptionError(const char *const Option) noexcept {
    fprintf(stderr, "Unrecognized Option: \"%s\"\n", Option);
}

static void
HandleDscImageOpenError(
    const DscMemoryObject::DscImageOpenError Error) noexcept
{
    if (Error == DscMemoryObject::DscImageOpenError::None) {
        return;
    }

    fputs("Could not open image. Error: ", stderr);
    switch (Error) {
        case DscMemoryObject::DscImageOpenError::None:
            break;
        case DscMemoryObject::DscImageOpenError::InvalidAddress:
            fputs("Invalid Image-Address\n", stderr);
            break;
        case DscMemoryObject::DscImageOpenError::NotAMachO:
            fputs("Not a Mach-O\n", stderr);
            break;
        case DscMemoryObject::DscImageOpenError::InvalidMachO:
            fputs("Not a valid Mach-O\n", stderr);
            break;
        case DscMemoryObject::DscImageOpenError::InvalidLoadCommands:
            fputs("Invalid Mach-O Load-Commands\n", stderr);
            break;
        case DscMemoryObject::DscImageOpenError::NotADylib:
            fputs("Not a Mach-O Dynamic Library (Dylib)\n", stderr);
            break;
        case DscMemoryObject::DscImageOpenError::NotMarkedAsImage:
            fputs("Image is not marked as one\n", stderr);
            break;
        case DscMemoryObject::DscImageOpenError::SizeTooLarge:
            fputs("Image-Size too large\n", stderr);
            break;
    }

    exit(1);
}

[[nodiscard]] static const DscImageMemoryObject *
GetImageWithPath(const DscMemoryObject &Object,
                 const std::string_view Path) noexcept
{
    const auto ImageInfo = Object.GetImageInfoWithPath(Path);
    if (ImageInfo == nullptr) {
        fprintf(stderr, "No Image was found for path \"%s\"\n", Path.data());
        exit(0);
    }

    const auto ObjectOrError = Object.GetImageWithInfo(*ImageInfo);
    HandleDscImageOpenError(ObjectOrError.getError());

    return ObjectOrError.value();
}

[[nodiscard]] static auto
MatchesOption(const OperationKind Kind, const ArgvArrayIterator &Arg) noexcept {
    const auto ShortName = OperationKindGetOptionShortName(Kind).value_or("");
    const auto LongName = OperationKindGetOptionName(Kind);

    if (Arg[0] == '-' && Arg[1] != '-') {
        if (!ShortName.empty() && ShortName == (Arg + 1)) {
            return true;
        }

        if (LongName == (Arg + 1)) {
            PrintUnrecognizedOptionError(Arg);

            fprintf(stderr, "Did you mean option \"--%s\"?\n", LongName.data());
            exit(1);
        }
    } else if (LongName == (Arg + 2)) {
        return true;
    }

    if (!ShortName.empty() && ShortName == (Arg + 2)) {
        PrintUnrecognizedOptionError(Arg);

        fprintf(stderr, "Did you mean option \"-%s\"?\n", ShortName.data());
        exit(1);
    }

    return false;
}

constexpr static auto UsageString =
    "Usage: ktool [Operation] [Operation-Options] [Path] [Path-Options]\n";

int main(const int Argc, const char *Argv[]) {
    // Skip the command-name at Argv[0]

    const auto ArgvArr = ArgvArray(Argc, Argv).fromIndex(1);
    if (ArgvArr.empty()) {
        PrintRunHelpMessage();
        return 0;
    }

    // Get the Operation-Kind.

    const auto OpsKindArg = ArgvArr.front();
    if (!OpsKindArg.isOption()) {
        fprintf(stderr,
                "Expected Operation-Kind Option, Got: \"%s\"\n",
                OpsKindArg.getString());
        return 1;
    }

    if (OpsKindArg.isEmptyOption()) {
        fputs("Please provide a non-empty option for an operation-kind\n",
              stderr);
        return 1;
    }

    auto OpsKind = OperationKind::None;
    auto OpsOpt = std::optional<std::unique_ptr<Operation>>();

    switch (OpsKind) {
        using Enum = OperationKind;
        case Enum::None:
            if (OpsKindArg.isHelpOption()) {
                fputs(UsageString, stdout);
                fputs("Options:\n", stdout);

                Operation::PrintHelpMenu(stdout);
                return 0;
            }
        case Enum::PrintHeader:
            if (MatchesOption(Enum::PrintHeader, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintHeaderOperation>();
                break;
            }
        case Enum::PrintLoadCommands:
            if (MatchesOption(Enum::PrintLoadCommands, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintLoadCommandsOperation>();
                break;
            }
        case Enum::PrintSharedLibraries:
            if (MatchesOption(Enum::PrintSharedLibraries, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintSharedLibrariesOperation>();
                break;
            }
        case Enum::PrintId:
            if (MatchesOption(Enum::PrintId, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintIdOperation>();
                break;
            }
        case Enum::PrintArchList:
            if (MatchesOption(Enum::PrintArchList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintArchListOperation>();
                break;
            }
        case Enum::PrintExportTrie:
            if (MatchesOption(Enum::PrintExportTrie, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintExportTrieOperation>();
                break;
            }
        case Enum::PrintObjcClassList:
            if (MatchesOption(Enum::PrintObjcClassList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintObjcClassListOperation>();
                break;
            }
        case Enum::PrintBindActionList:
            if (MatchesOption(Enum::PrintBindActionList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintBindActionListOperation>();
                break;
            }
        case Enum::PrintBindOpcodeList:
            if (MatchesOption(Enum::PrintBindOpcodeList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintBindOpcodeListOperation>();
                break;
            }
        case Enum::PrintBindSymbolList:
            if (MatchesOption(Enum::PrintBindSymbolList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintBindSymbolListOperation>();
                break;
            }
        case Enum::PrintRebaseActionList:
            if (MatchesOption(Enum::PrintRebaseActionList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintRebaseActionListOperation>();
                break;
            }
        case Enum::PrintRebaseOpcodeList:
            if (MatchesOption(Enum::PrintRebaseOpcodeList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintRebaseOpcodeListOperation>();
                break;
            }
        case Enum::PrintCStringSection:
            if (MatchesOption(Enum::PrintCStringSection, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintCStringSectionOperation>();
                break;
            }
        case Enum::PrintSymbolPtrSection:
            if (MatchesOption(Enum::PrintSymbolPtrSection, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintSymbolPtrSectionOperation>();
                break;
            }
        case Enum::PrintImageList:
            if (MatchesOption(Enum::PrintImageList, OpsKindArg)) {
                OpsOpt = std::make_unique<PrintImageListOperation>();
                break;
            }
    }

    if (!OpsOpt.has_value()) {
        PrintUnrecognizedOptionError(OpsKindArg);
        PrintRunHelpMessage();

        return 1;
    }

    const auto Ops = OpsOpt->get();
    const auto OpsArgv = ArgvArr.fromIndex(1);

    if (OpsArgv.empty()) {
        fprintf(stderr,
                "Please provide a file for operation %s\n",
                Ops->getName().data());

        PrintRunHelpMessage();
        return 1;
    }

    if (strcmp(OpsArgv.front(), "--help") == 0) {
        if (OpsArgv.count() != 1) {
            fputs("Option --help should be run alone\n", stderr);
            return 1;
        }

        fprintf(stdout,
                "Usage: ktool %s [Options] [Path] [Path-Options]\n",
                OpsKindArg.getString());

        Ops->printEntireOptionUsageMenu(stdout);
        return 0;
    }

    // Since we gave ParseOptions a [2, Argc] Argv, we have to add two here to
    // get the full index.

    const auto PathIndex = Ops->ParseOptions(OpsArgv);
    const auto Path = PathUtil::MakeAbsolute(OpsArgv.at(PathIndex));
    const auto Fd =
        FileDescriptor::Open(Path.data(), FileDescriptor::OpenKind::Read);

    if (Fd.hasError()) {
        fprintf(stderr,
                "Could not open the provided file (at path: %s), error: "
                "\"%s\"\n",
                Path.data(),
                strerror(errno));
        return 1;
    }

    auto FileMapProt = MappedFile::Protections();

    FileMapProt.add(MappedFile::Protections::Flags::Read);
    FileMapProt.add(MappedFile::Protections::Flags::Write);

    const auto FileMap =
        MappedFile::Open(Fd, FileMapProt, MappedFile::MapKind::Private);

    switch (FileMap.getError()) {
        case MappedFile::OpenError::None:
            break;
        case MappedFile::OpenError::FailedToGetInfo:
            fprintf(stderr,
                    "Failed to get file-info of provided file. Error: %s\n",
                    strerror(errno));
            return 1;
        case MappedFile::OpenError::NotAFile:
            fputs("Provided path does not point to a file\n", stderr);
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

    auto ObjectOrError = MemoryObject::Open(FileMap);
    if (const auto ErrorInt = ObjectOrError.getErrorInt()) {
        switch (ObjectOrError.getObjectKind()) {
            case ObjectKind::None:
                assert(0 && "MemoryObject::Open() returned ObjectKind::None");
            case ObjectKind::MachO: {
                switch (MachOMemoryObject::getErrorFromInt(ErrorInt)) {
                    case MachOMemoryObject::Error::None:
                    case MachOMemoryObject::Error::WrongFormat:
                    case MachOMemoryObject::Error::SizeTooSmall:
                        assert(0 && "Got Unhandled errors in main");
                    case MachOMemoryObject::Error::TooManyLoadCommands:
                        fputs("Provided File has an invalid LoadCommands "
                              "buffer\n",
                              stderr);
                        return 1;
                }

                break;
            }
            case ObjectKind::FatMachO: {
                switch (FatMachOMemoryObject::getErrorFromInt(ErrorInt)) {
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
                              "size",
                              stderr);
                        return 1;
                    case FatMachOMemoryObject::Error::ArchOutOfBounds:
                        fputs("Provided file has architecture(s) out of "
                              "bounds\n",
                              stderr);
                        return 1;
                    case FatMachOMemoryObject::Error::ArchOverlapsArch:
                        fputs("Provided file has overlapping architectures\n",
                              stderr);
                        return 1;
                }

                break;
            }
            case ObjectKind::DyldSharedCache: {
                switch (DscMemoryObject::getErrorFromInt(ErrorInt)) {
                    using Error = DscMemoryObject::Error;

                    case Error::None:
                    case Error::WrongFormat:
                    case Error::SizeTooSmall:
                        assert(0 && "Got Unhandled errors in main");
                    case Error::UnknownCpuKind:
                        fputs("Provided file is a dyld-shared-cache file with "
                              "an unknown cpu-kind\n",
                              stderr);
                        return 1;
                    case Error::InvalidMappingInfoListRange:
                        fputs("Provided file is a dyld-shared-cache file with "
                              "an invalid mapping-info list file-range\n",
                              stderr);
                        return 1;
                    case Error::InvalidImageInfoListRange:
                        fputs("Provided file is a dyld-shared-cache file with "
                              "an unknown image-info list file-range\n",
                              stderr);
                        return 1;
                    case Error::OverlappingImageMappingInfoListRange:
                        fputs("Provided file is a dyld_shared_cache file with "
                              "overlapping Image-Info and Mapping-Info "
                              "Ranges\n",
                              stderr);
                        return 1;
                }

                break;
            }
            case ObjectKind::DscImage:
                assert(0 && "Reached Dsc-Image for which no errors exist");
        }
    }

    // Parse any options for the path.

    auto Object = ObjectOrError.value();
    auto SubObject = Object;

    const auto PathArgv = OpsArgv.fromIndex(PathIndex + 1);
    for (auto &Argument : PathArgv) {
        if (!Argument.isOption()) {
            fprintf(stderr,
                    "Expected Path-Option, got \"%s\" instead\n",
                    Argument.getString());
            return 1;
        }

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
            if (ArchNumber == 0) {
                fputs("An Arch-Number of 0 is invalid. For the first arch, use "
                      "-arch 1\n",
                      stderr);
                return 1;
            }

            const auto ArchIndex = ArchNumber - 1;
            const auto ArchCount = FatObject->getArchCount();

            if (IndexOutOfBounds(ArchIndex, ArchCount)) {
                fprintf(stderr,
                        "Provided file only has %" PRIu32 " archs\n",
                        ArchCount);
                return 1;
            }

            const auto ArchInfo = FatObject->GetArchInfoAtIndex(ArchIndex);
            auto ArchObjectOrError = FatObject->GetArchObjectFromInfo(ArchInfo);

            switch (ArchObjectOrError.getError()) {
                using ErrorEnum = FatMachOMemoryObject::GetArchObjectError;
                case ErrorEnum::None:
                    break;

                case ErrorEnum::InvalidArchRange:
                    fprintf(stderr,
                            "Provided file's arch #%" PRIu32 " has an invalid "
                            "file-range\n",
                            ArchNumber);
                    return 1;

                case ErrorEnum::UnsupportedObjectKind:
                    fprintf(stderr,
                            "Provided file's arch #%" PRIu32 " is of an "
                            "unsupported object-kind\n",
                            ArchNumber);
                    return 1;
            }

            switch (ArchObjectOrError.getWarning()) {
                using WarningEnum = FatMachOMemoryObject::GetArchObjectWarning;
                case WarningEnum::None:
                    break;
                case WarningEnum::MachOCpuKindMismatch:
                    fputs("Warning: Arch's Cpu-Kind differs from expected\n",
                          stderr);
                    break;
            }

            SubObject = ArchObjectOrError.getObject();
        } else if (strcmp(Argument, "-image") == 0) {
            const auto DscObj =
                dyn_cast<ObjectKind::DyldSharedCache>(Object);

            if (DscObj == nullptr) {
                fputs("-image option not allowed: Provided file is not a Dyld "
                      "Shared-Cache File\n",
                      stderr);
                return 1;
            }

            if (!Argument.hasNext()) {
                fputs("Please provide an image-index or image-path.\n"
                      "Use the --list-dsc-images option to see a list of "
                      "images\n",
                      stderr);
                return 1;
            }

            Argument.advance();
            if (!Argument.isAbsolutePath()) {
                const auto Number = ParseNumber<uint32_t>(Argument.getString());
                const auto ImageCount = DscObj->getImageCount();

                if (Number == 0) {
                    fputs("An Image-Number of 0 is invalid. For the first "
                          "image, use -image 1\n",
                          stderr);
                    return 1;
                }

                const auto ImageIndex = Number - 1;
                if (IndexOutOfBounds(ImageIndex, ImageCount)) {
                    fprintf(stderr,
                            "Provided file only has %" PRIu32 " images\n",
                            ImageCount);
                    return 1;
                }

                const auto &ImageInfo = DscObj->getImageInfoAtIndex(ImageIndex);
                const auto ImageObjectOrError =
                    DscObj->GetImageWithInfo(ImageInfo);

                HandleDscImageOpenError(ImageObjectOrError.getError());
                SubObject = ImageObjectOrError.value();
            } else {
                SubObject =
                    const_cast<DscImageMemoryObject *>(
                        GetImageWithPath(*DscObj, Argument.GetStringView()));
            }
        } else {
            fprintf(stderr,
                    "Unrecognized Path-Option: \"%s\"\n",
                    Argument.getString());

            return 1;
        }
    }

    const auto Result = Ops->Run(*SubObject);
    if (Result == Operation::InvalidObjectKind) {
        Ops->printObjectKindNotSupportedError(*SubObject);
        return 1;
    }

    return 0;
}
