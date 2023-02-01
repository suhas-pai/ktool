//
//  Operations/Base.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <cstdio>
#include <inttypes.h>

#include "ADT/FileMap.h"

#include "Objects/DscImage.h"
#include "Objects/DyldSharedCache.h"
#include "Objects/FatMachO.h"

#include "Operations/Base.h"

namespace Operations {
    auto PrintUnsupportedError(const std::string_view Path) noexcept {
        fprintf(stderr,
                "Operation doesn't support file at path %s\n",
                Path.data());
        exit(1);
    }

    auto
    Base::runAndHandleFile(const HandleFileOptions Options) const noexcept
        -> RunResult
    {
        const auto FileMap =
            ADT::FileMap(Options.Path.data(), ADT::FileMap::Prot::Read);

        const auto ObjectOrError = Objects::Open(FileMap.map());
        if (ObjectOrError.hasError()) {
            fprintf(stderr,
                    "File (at path %s) is of an unrecognized format\n",
                    Options.Path.data());
            exit(1);
        }

        const auto Object = ObjectOrError.ptr();
        const auto Result = runAndHandleFile(*Object, Options);

        delete Object;
        return Result;
    }

    auto
    Base::runAndHandleFile(const Objects::Base &Object,
                           const HandleFileOptions Options) const noexcept
        -> RunResult
    {
        switch (Object.kind()) {
            case Objects::Kind::None:
                assert(false && "Got Object-Kind None in runAndHandleFile()");
            case Objects::Kind::MachO:
                if (!supportsObjectKind(Objects::Kind::MachO)) {
                    PrintUnsupportedError(Options.Path);
                }

                return run(static_cast<const Objects::MachO &>(Object));
            case Objects::Kind::FatMachO: {
                const auto SupportsFatMachO =
                    supportsObjectKind(Objects::Kind::FatMachO);

                const auto Fat = static_cast<const Objects::FatMachO &>(Object);
                if (SupportsFatMachO) {
                    if (Options.ArchIndex == -1) {
                        return run(Fat);
                    }

                    if (!supportsObjectKind(Objects::Kind::MachO)) {
                        fputs("Operation doesn't support Mach-O Files, but "
                              "does support Fat Mach-O Files.\nDrop the "
                              "--arch-index option to run on the Fat Mach-O "
                              "file\n",
                              stderr);
                        exit(1);
                    }
                } else if (!supportsObjectKind(Objects::Kind::MachO)) {
                    PrintUnsupportedError(Options.Path);
                }

                if (Options.ArchIndex == -1) {
                    fputs("Operation doesn't support Fat Mach-O Files. Please "
                          "select an arch by its index using option "
                          "--arch-index\n",
                          stderr);
                    exit(1);
                }

                const auto ArchIndex = static_cast<uint32_t>(Options.ArchIndex);
                const auto ArchCount = Fat.archCount();

                if (Options.ArchIndex >= ArchCount) {
                    fprintf(stderr,
                            "An Arch-Index of %" PRIu32 " is invalid. The "
                            "provided Fat Mach-O file only has %" PRIu32
                            " architectures\n",
                            ArchIndex,
                            ArchCount);
                    exit(1);
                }

                const auto ArchObject = Fat.getArchObjectAtIndex(ArchIndex);
                if (const auto Error = ArchObject.error(); !Error.isNone()) {
                    if (Error.isUnrecognizedFormat()) {
                        fprintf(stderr,
                                "Architecture at index %" PRIu32 " is of an "
                                "unrecognized format\n",
                                ArchIndex);
                        exit(1);
                    }

                    switch (Error.Kind) {
                        case Objects::Kind::None:
                            assert(false &&
                                   "Got Object-Kind None for OpenError\n");
                        case Objects::Kind::MachO:
                            using ErrorKind = Objects::MachO::OpenError;
                            switch (ErrorKind(Error.Error)) {
                                case Objects::MachO::OpenError::None:
                                    assert(false &&
                                           "Got Error None for MachO "
                                           "OpenError");
                                case Objects::MachO::OpenError::WrongFormat:
                                    assert(false &&
                                           "Got Error WrongFormat for MachO "
                                           "OpenError");
                                case ErrorKind::SizeTooSmall:
                                    fprintf(stderr,
                                            "Arch at index %" PRIu32 " is too "
                                            "small to be a valid mach-o\n",
                                            ArchIndex);
                                    exit(1);
                                case ErrorKind::TooManyLoadCommands:
                                    fprintf(stderr,
                                            "Arch at index %" PRIu32 " has too "
                                            "many load-commands for its size\n",
                                            ArchIndex);
                                    exit(1);
                            }

                            break;
                        case Objects::Kind::FatMachO:
                            assert(false &&
                                   "Arch-Object is somehow a Fat-MachO");
                        case Objects::Kind::DyldSharedCache:
                            assert(false &&
                                   "Arch-Object is somehow a Dyld "
                                   "Shared-Cache");
                            break;
                        case Objects::Kind::DscImage:
                            assert(false &&
                                   "Arch-Object is somehow a Dsc-Image");
                        }
                }

                return run(*ArchObject.ptr());
            }
            case Objects::Kind::DyldSharedCache: {
                const auto Dsc =
                    static_cast<const Objects::DyldSharedCache &>(Object);

                if (supportsObjectKind(Objects::Kind::DyldSharedCache)) {
                    if (Options.ImageIndex == -1) {
                        return run(Dsc);
                    }

                    if (!supportsObjectKind(Objects::Kind::DscImage)) {
                        fputs("Operation doesn't support Dyld Shared-Cache "
                              "Images, but does support Dyld Shared-Cache "
                              "Images.\nDrop the arch-index option to run on "
                              "the Dyld Shared-Cache file\n",
                              stderr);
                        exit(1);
                    }
                } else if (Options.ImageIndex == -1) {
                    PrintUnsupportedError(Options.Path);
                }

                const auto ImageCount = Dsc.imageCount();
                const auto ImageIndex =
                    static_cast<uint32_t>(Options.ImageIndex);

                if (ImageIndex >= ImageCount) {
                    fprintf(stderr,
                            "An Image-Index of %" PRIu32 " is invalid. The "
                            "provided Dyld Shared-Cache file only has %" PRIu32
                            " images\n",
                            ImageIndex,
                            ImageCount);
                    exit(1);
                }

                const auto ImageObject = Dsc.getImageObjectAtIndex(ImageIndex);
                if (const auto Error = ImageObject.error(); !Error.isNone()) {
                    if (Error.isUnrecognizedFormat()) {
                        fprintf(stderr,
                                "Image at index %" PRIu32 " is of an "
                                "unrecognized format\n",
                                ImageIndex);
                        exit(1);
                    }

                    switch (Error.Kind) {
                        case Objects::Kind::None:
                            assert(false &&
                                   "Got Object-Kind None for OpenError\n");
                        case Objects::Kind::MachO:

                            break;
                        case Objects::Kind::FatMachO:
                            assert(false &&
                                   "Image-Object is somehow a Fat-MachO");
                        case Objects::Kind::DyldSharedCache:
                            assert(false &&
                                   "Image-Object is somehow a Dyld "
                                   "Shared-Cache");
                            break;
                        case Objects::Kind::DscImage:
                            using ErrorKind = Objects::DscImage::OpenError;
                            switch (ErrorKind(Error.Error)) {
                                case ErrorKind::None:
                                    assert(false &&
                                           "Got Error None for MachO "
                                           "OpenError");
                                case ErrorKind::InvalidAddress:
                                    fputs("Image Address is invalid\n", stderr);
                                    exit(1);
                                case ErrorKind::WrongFormat:
                                    assert(false &&
                                           "Got Error WrongFormat for MachO "
                                           "OpenError");
                                case ErrorKind::SizeTooSmall:
                                    fprintf(stderr,
                                            "Image at index %" PRIu32 " is too "
                                            "small to be a valid mach-o\n",
                                            ImageIndex);
                                    exit(1);
                                case ErrorKind::WrongCpuInfo:
                                    fputs("Image has different cpu-info than "
                                          "the shared-cache\n",
                                          stderr);
                                    exit(1);
                                case ErrorKind::NotMarkedAsImage:
                                    fputs("Image's mach_header is not marked "
                                          "as a shared-cache image\n",
                                          stderr);
                                    exit(1);
                                case ErrorKind::NotADylib:
                                    fputs("Image is not a dynamic-library\n",
                                          stderr);
                                    exit(1);
                                case ErrorKind::SizeTooLarge:
                                    fputs("Image extends beyond of file\n",
                                          stderr);
                                    exit(1);
                                case ErrorKind::TooManyLoadCommands:
                                    fprintf(stderr,
                                            "Image at index %" PRIu32 " has "
                                            "too many load-commands for its "
                                            "size\n",
                                            ImageIndex);
                                    exit(1);
                                }
                        }
                }

                return run(
                    static_cast<Objects::DscImage &>(*ImageObject.ptr()));
            }
            case Objects::Kind::DscImage:
                assert(false && "File is somehow a Dsc-image");

        }

        assert(false && "Got unrecognized Object-Kind in runAndHandleFile()");
    }
}
