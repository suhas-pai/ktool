//
//  Operations/Run.cpp
//  ktool
//
//  Created by suhaspai on 12/14/24.
//

#include "Objects/Open.h"
#include "Operations/Run.h"

namespace Operations {
    auto RunResult::isUnsupportedError() const noexcept -> bool {
        switch (Kind) {
            case Kind::PrintHeader:
                return false;
            case Kind::PrintId:
                return PrintIdResult.Error ==
                    PrintId::RunResult::Error::Unsupported;
            case Kind::PrintLoadCommands:
                return PrintLoadCommandsResult.Error ==
                    PrintLoadCommands::RunResult::Error::Unsupported;
            case Kind::PrintLibraries:
                return PrintLibrariesResult.Error ==
                    PrintLibraries::RunResult::Error::Unsupported;
            case Kind::PrintArchs:
                return PrintArchsResult.Error ==
                    PrintArchs::RunResult::Error::Unsupported;
            case Kind::PrintCStringSection:
                return PrintCStringSectionResult.Error ==
                    PrintCStringSection::RunResult::Error::Unsupported;
            case Kind::PrintSymbolPtrSection:
                return PrintSymbolPtrSectionResult.Error ==
                    PrintSymbolPtrSection::RunResult::Error::Unsupported;
            case Kind::PrintExportTrie:
                return PrintExportTrieResult.Error ==
                    PrintExportTrie::RunResult::Error::Unsupported;
            case Kind::PrintBindOpcodeList:
                return PrintBindOpcodeListResult.Error ==
                    PrintBindOpcodeList::RunResult::Error::Unsupported;
            case Kind::PrintBindActionList:
                return PrintBindActionListResult.Error ==
                    PrintBindActionList::RunResult::Error::Unsupported;
            case Kind::PrintBindSymbolList:
                return PrintBindSymbolListResult.Error ==
                    PrintBindSymbolList::RunResult::Error::Unsupported;
            case Kind::PrintRebaseOpcodeList:
                return PrintRebaseOpcodeListResult.Error ==
                    PrintRebaseOpcodeList::RunResult::Error::Unsupported;
            case Kind::PrintRebaseActionList:
                return PrintRebaseActionListResult.Error ==
                    PrintRebaseActionList::RunResult::Error::Unsupported;
            case Kind::PrintObjcClassList:
                return PrintObjcClassListResult.Error ==
                    PrintObjcClassList::RunResult::Error::Unsupported;
            case Kind::PrintProgramTrie:
                return PrintProgramTrieResult.Error ==
                    PrintProgramTrie::RunResult::Error::Unsupported;
            case Kind::PrintImageList:
                return PrintImageListResult.Error ==
                    PrintImageList::RunResult::Error::Unsupported;
        }

        assert(0 && "Got unrecognized RunResult::Kind");
    }

    auto
    RunAndHandleFile(const Operations::Base &Op,
                     const HandleFileOptions &Options) noexcept -> RunResult
    {
        const auto Prot = ADT::FileMap::Prot::Read;
        const auto FileMapOrError =
            ADT::FileMap::Open(Options.Path.data(), Prot);

        switch (FileMapOrError.error()) {
            case ADT::FileMap::OpenError::None:
                break;
            case ADT::FileMap::OpenError::FailedToOpen:
                fprintf(stderr,
                        "Failed to open file (at path %s), error=%s\n",
                        Options.Path.data(),
                        strerror(errno));
                exit(1);
            case ADT::FileMap::OpenError::FailedToStat:
                fprintf(stderr,
                        "Failed to get info on file (at path %s), error=%s\n",
                        Options.Path.data(),
                        strerror(errno));
                exit(1);
            case ADT::FileMap::OpenError::FailedToMemMap:
                fprintf(stderr,
                        "Failed to open memory-map of file (at path %s), "
                        "error=%s\n",
                        Options.Path.data(),
                        strerror(errno));
                exit(1);
        }

        const auto FileMap = FileMapOrError.value();
        const auto ObjectOrError =
            Objects::Open(FileMap->map(), Options.Path, Prot);

        if (!ObjectOrError.has_value()) {
            if (ObjectOrError.error().isUnrecognizedFormat()) {
                fprintf(stderr,
                        "File (at path %s) is of an unrecognized format\n",
                        Options.Path.data());
                exit(1);
            }

            switch (ObjectOrError.error().Kind) {
                case Objects::Kind::None:
                    assert(false &&
                           "Got Object-Kind None when handling error in "
                           "Operations::runAndHandleFile()");
                case Objects::Kind::MachO: {
                    using OpenError = Objects::MachO::OpenError;
                    switch (ObjectOrError.error().MachOError.Kind) {
                        case OpenError::None:
                            assert(false &&
                                   "Got Error::None when handling "
                                   "Objects::MachO::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::WrongFormat:
                            assert(false &&
                                   "Got Error::WrongFormat when handling "
                                   "Objects::MachO::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::SizeTooSmall:
                            fputs("File is too small to be a valid mach-o "
                                  "file\n", stderr);
                            exit(1);
                        case OpenError::TooManyLoadCommands:
                            fputs("Mach-O file has too many load-commands\n",
                                  stderr);
                            exit(1);
                    }

                    break;
                }
                case Objects::Kind::FatMachO: {
                    using OpenError = Objects::FatMachO::OpenError;
                    switch (ObjectOrError.error().FatMachOError.Kind) {
                        case OpenError::None:
                            assert(false &&
                                   "Got Error::None when handling "
                                   "Objects::FatMachO::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::WrongFormat:
                            assert(false &&
                                   "Got Error::WrongFormat when handling "
                                   "Objects::FatMachO::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::SizeTooSmall:
                            fputs("File is too small to be a valid mach-o "
                                  "file\n", stderr);
                            exit(1);
                        case OpenError::TooManyArchitectures:
                            fputs("Fat Mach-O file has too many "
                                  "architectures\n",
                                  stderr);
                            exit(1);
                        case OpenError::ArchOutOfBounds:
                            fputs("Fat Mach-O File has at least 1 arch "
                                  "out-of-bounds of file\n",
                                  stderr);
                            exit(1);
                        case OpenError::OverlappingArchs:
                            fputs("Fat Mach-O File has at least 2 archs "
                                  "overlap one another\n",
                                  stderr);
                            exit(1);
                        case OpenError::ArchsForSameCpu:
                            fputs("Fat Mach-O File has at least 2 archs "
                                  "for the same cpu\n",
                                  stderr);
                            exit(1);
                        }

                    break;
                }
                case Objects::Kind::DyldSharedCache: {
                    using OpenError = Objects::DyldSharedCache::OpenError;
                    switch (ObjectOrError.error().DscError.Kind) {
                        case OpenError::None:
                            assert(false &&
                                   "Got Error::None when handling "
                                   "Objects::DyldSharedCache::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::WrongFormat:
                            assert(false &&
                                   "Got Error::WrongFormat when handling "
                                   "Objects::DyldSharedCache::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::UnrecognizedCpuKind:
                            fputs("Dyld-shared-cache has an unrecognized "
                                  "cputype\n",
                                  stderr);
                            exit(1);
                        case OpenError::SizeTooSmall:
                            fputs("File is too small to be a valid "
                                  "dyld-shared-cache file\n",
                                  stderr);
                            exit(1);
                        case OpenError::NoMappings:
                            fputs("Dyld Shared-Cache's has no mappings\n",
                                  stderr);
                            exit(1);
                        case OpenError::MappingsOutOfBounds:
                            fputs("Dyld Shared-Cache's mappings are "
                                  "out-of-bound\n",
                                  stderr);
                            exit(1);
                        case OpenError::FirstMappingFileOffNotZero:
                            fputs("Dyld Shared-Cache's first-mapping's "
                                  "file-offset is not 0x0\n",
                                  stderr);
                            exit(1);
                        case OpenError::FailedToOpenSubCaches:
                            fputs("Failed to open sub-caches of shared-cache\n",
                                  stderr);
                            exit(1);
                        case OpenError::SubCacheHasDiffCpuKind:
                            fputs("At least 1 Sub-cache has a different "
                                  "cpu-kind\n",
                                  stderr);
                            exit(1);
                        case OpenError::SubCacheHasDiffVersion:
                            fputs("At least 1 Sub-cache has a different "
                                  "header-size\n",
                                  stderr);
                            exit(1);
                        case OpenError::RecursiveSubCache:
                            fputs("At least 1 Sub-cache has its own "
                                  "sub-caches\n",
                                  stderr);
                            exit(1);
                        case OpenError::SubCacheListIsInvalid:
                            fputs("List of Sub-Caches is invalid\n", stderr);
                            exit(1);
                        case OpenError::SubCacheFileDoesNotExist:
                            fputs("Sub-cache file does not exist\n", stderr);
                            exit(1);
                    }

                    break;
                }
                case Objects::Kind::DscImage: {
                    using OpenError = Objects::DscImage::OpenError;
                    switch (ObjectOrError.error().DscImageError.Kind) {
                        case OpenError::None:
                            assert(false &&
                                   "Got Error::None when handling "
                                   "Objects::DscImage::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::WrongFormat:
                            assert(false &&
                                   "Got Error::WrongFormat when handling "
                                   "Objects::DscImage::OpenError in "
                                   "Operations::runAndHandleFile()");
                        case OpenError::InvalidAddress:
                            fputs("Dsc-image has an invalid address inside "
                                  "shared-cache\n",
                                  stderr);
                            exit(1);
                        case OpenError::WrongCpuInfo:
                            fputs("Dsc-image has a different cputype than its "
                                  "shared-cache\n",
                                  stderr);
                            exit(1);
                        case OpenError::NotMarkedAsImage:
                            fputs("Dsc-image is not marked as a mach-o image\n",
                                  stderr);
                            exit(1);
                        case OpenError::NotADylib:
                            fputs("Dsc-image is neither a dylib or dylinker\n",
                                  stderr);
                            exit(1);
                        case OpenError::SizeTooSmall:
                            fputs("Dsc-image is too small to be a valid "
                                  "mach-o\n",
                                  stderr);
                            exit(1);
                        case OpenError::TooManyLoadCommands:
                            fputs("Dsc-image has too many load-commands\n",
                                  stderr);
                            exit(1);
                        case OpenError::OutOfBoundsSegment:
                            fputs("Dsc-image has a segment that is "
                                  "out-of-bounds of its shared-cache\n",
                                  stderr);
                            exit(1);
                        case OpenError::FailedToOpenDscSubCache:
                            fputs("Dsc-image at  exists in a separate "
                                  "shared-cache file, a sub-cache\n",
                                  stderr);
                            exit(1);
                    }

                    break;
                }
            }
        }

        const auto Object = ObjectOrError.value();
        const auto Result = RunAndHandleFile(Op, *Object, Options);

        delete FileMap;
        delete Object;

        return Result;
    }

    auto PrintUnsupportedError(const std::string_view Path) noexcept {
        fprintf(stderr,
                "Operation doesn't support file at path %s\n",
                Path.data());
        exit(1);
    }

    [[nodiscard]] static
    auto RunOperation(const Operations::Base &Op, const Objects::Base &Object)
        -> RunResult
    {
        switch (Op.kind()) {
            case Kind::PrintHeader:
                return RunResult(
                    static_cast<const PrintHeader &>(Op).run(Object));
            case Kind::PrintId:
                return RunResult(
                    static_cast<const PrintId &>(Op).run(Object));
            case Kind::PrintLoadCommands:
                return RunResult(
                    static_cast<const PrintLoadCommands &>(Op).run(Object));
            case Kind::PrintLibraries:
                return RunResult(
                    static_cast<const PrintHeader &>(Op).run(Object));
            case Kind::PrintArchs:
                return RunResult(
                    static_cast<const PrintArchs &>(Op).run(Object));
            case Kind::PrintCStringSection:
                return RunResult(
                    static_cast<const PrintHeader &>(Op).run(Object));
            case Kind::PrintSymbolPtrSection:
                return RunResult(
                    static_cast<const PrintHeader &>(Op).run(Object));
            case Kind::PrintExportTrie:
                return RunResult(
                    static_cast<const PrintExportTrie &>(Op).run(Object));
            case Kind::PrintBindOpcodeList:
                return RunResult(
                    static_cast<const PrintBindOpcodeList &>(Op).run(Object));
            case Kind::PrintBindActionList:
                return RunResult(
                    static_cast<const PrintBindActionList &>(Op).run(Object));
            case Kind::PrintBindSymbolList:
                return RunResult(
                    static_cast<const PrintBindSymbolList &>(Op).run(Object));
            case Kind::PrintRebaseOpcodeList:
                return RunResult(
                    static_cast<const PrintRebaseOpcodeList &>(Op)
                        .run(Object));
            case Kind::PrintRebaseActionList:
                return RunResult(
                    static_cast<const PrintRebaseActionList &>(Op).run(Object));
            case Kind::PrintObjcClassList:
                return RunResult(
                    static_cast<const PrintObjcClassList &>(Op).run(Object));
            case Kind::PrintProgramTrie:
                return RunResult(
                    static_cast<const PrintProgramTrie &>(Op).run(Object));
            case Kind::PrintImageList:
                return RunResult(
                    static_cast<const PrintImageList &>(Op).run(Object));
        }
    }

    auto
    RunAndHandleFile(const Operations::Base &Op,
                     const Objects::Base &Object,
                     const HandleFileOptions &Options) noexcept -> RunResult
    {
        switch (Object.kind()) {
            case Objects::Kind::None:
                assert(false && "Got Object-Kind None in runAndHandleFile()");
            case Objects::Kind::MachO:
                if (!Op.supportsObjectKind(Objects::Kind::MachO)) {
                    PrintUnsupportedError(Options.Path);
                }

                return RunOperation(Op, Object);
            case Objects::Kind::FatMachO: {
                const auto SupportsFatMachO =
                    Op.supportsObjectKind(Objects::Kind::FatMachO);

                const auto Fat = static_cast<const Objects::FatMachO &>(Object);
                if (SupportsFatMachO) {
                    if (Options.ArchIndex == -1) {
                        return RunOperation(Op, Object);
                    }

                    if (!Op.supportsObjectKind(Objects::Kind::MachO)) {
                        fputs("Operation doesn't support Mach-O Files, but "
                              "does support Fat Mach-O Files.\nDrop the "
                              "-arch option to run on the Fat Mach-O "
                              "file\n",
                              stderr);
                        exit(1);
                    }
                } else if (!Op.supportsObjectKind(Objects::Kind::MachO)) {
                    PrintUnsupportedError(Options.Path);
                }

                if (Options.ArchIndex == -1) {
                    fputs("Operation doesn't support Fat Mach-O Files. Please "
                          "select an arch by its index using option "
                          "-arch\n",
                          stderr);
                    exit(1);
                }

                const auto ArchIndex =
                    static_cast<uint32_t>(Options.ArchIndex);

                const auto ArchCount = Fat.archCount();
                if (Utils::IndexOutOfBounds(ArchIndex, ArchCount)) {
                    fprintf(stderr,
                            "An Arch-Index of %" PRIu32 " is invalid. The "
                            "provided Fat Mach-O file only has %" PRIu32
                            " architectures\n",
                            ArchIndex,
                            ArchCount);
                    exit(1);
                }

                const auto ArchObjectOrError =
                    Objects::OpenArch(Fat, ArchIndex);

                if (!ArchObjectOrError.has_value()) {
                    const auto Error = ArchObjectOrError.error();
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
                            switch (Error.MachOError.Kind) {
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

                return RunOperation(Op, *ArchObjectOrError.value());
            }
            case Objects::Kind::DyldSharedCache: {
                const auto Dsc =
                    static_cast<const Objects::DyldSharedCache &>(Object);

                if (Op.supportsObjectKind(Objects::Kind::DyldSharedCache)) {
                    if (Options.ImageIndex == -1) {
                        return RunOperation(Op, Dsc);
                    }

                    if (!Op.supportsObjectKind(Objects::Kind::DscImage)) {
                        fputs("Operation doesn't support Dyld Shared-Cache "
                              "Images, but does support Dyld Shared-Cache "
                              "Images.\nDrop the -image option to run "
                              "on the Dyld Shared-Cache file\n",
                              stderr);
                        exit(1);
                    }
                } else if (!Op.supportsObjectKind(Objects::Kind::DscImage)) {
                    PrintUnsupportedError(Options.Path);
                }

                if (Options.ImageIndex == -1) {
                    fputs("Operation doesn't support Dyld Shared-Cache. Please "
                          "select an image by its index using option "
                          "-image\n",
                          stderr);
                    exit(1);
                }

                const auto ImageCount = Dsc.imageCount();
                const auto ImageIndex =
                    static_cast<uint32_t>(Options.ImageIndex);

                if (Utils::IndexOutOfBounds(ImageIndex, ImageCount)) {
                    fprintf(stderr,
                            "An Image-Index of %" PRIu32 " is invalid. The "
                            "provided Dyld Shared-Cache file only has %" PRIu32
                            " images\n",
                            ImageIndex,
                            ImageCount);
                    exit(1);
                }

                const auto ImageOrError = Objects::OpenImage(Dsc, ImageIndex);
                if (!ImageOrError.has_value()) {
                    const auto Error = ImageOrError.error();
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
                            assert(false &&
                                   "Image-Object is somehow a MachO (and not a "
                                   "dyld-shared-cache image)");
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
                            switch (Error.DscImageError.Kind) {
                                case ErrorKind::None:
                                    assert(false &&
                                           "Got Error None for MachO "
                                           "OpenError");
                                case ErrorKind::InvalidAddress:
                                    fprintf(stderr,
                                            "Address of image %p is "
                                            "out-of-bounds from shared-cache\n",
                                            (void *)Error.DscImageError
                                                .InvalidAddress.Address);
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
                                case ErrorKind::OutOfBoundsSegment:
                                    fputs("At least one of image's segments "
                                          "isn't fully contained within a "
                                          "single mapping\n",
                                          stderr);
                                    exit(1);
                                case ErrorKind::TooManyLoadCommands:
                                    fprintf(stderr,
                                            "Image at index %" PRIu32 " has "
                                            "too many load-commands for its "
                                            "size\n",
                                            ImageIndex);
                                    exit(1);
                                case ErrorKind::FailedToOpenDscSubCache:
                                    fprintf(stderr,
                                            "Image at index %" PRIu32 " exists "
                                            "in a separate shared-cache file, "
                                            "a sub-cache\n",
                                            ImageIndex);
                                    exit(1);
                            }
                        }
                }

                return RunOperation(Op, *ImageOrError.value());
            }
            case Objects::Kind::DscImage:
                assert(false && "File is somehow a Dsc-image");

        }

        assert(false && "Got unrecognized Object-Kind in runAndHandleFile()");
    }
}