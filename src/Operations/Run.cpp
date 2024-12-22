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
                std::print(stderr,
                           "Failed to open file (at path {}), error={}\n",
                           Options.Path,
                           strerror(errno));
                exit(1);
            case ADT::FileMap::OpenError::FailedToStat:
                std::print(stderr,
                           "Failed to get info on file (at path {}), "
                           "error={}\n",
                           Options.Path,
                           strerror(errno));
                exit(1);
            case ADT::FileMap::OpenError::FailedToMemMap:
                std::print(stderr,
                           "Failed to open memory-map of file (at path {}), "
                           "error={}\n",
                           Options.Path,
                           strerror(errno));
                exit(1);
        }

        const auto FileMap = FileMapOrError.value();
        const auto ObjectOrError =
            Objects::Open(FileMap->map(), Options.Path, Prot);

        if (!ObjectOrError.has_value()) {
            if (ObjectOrError.error().isUnrecognizedFormat()) {
                std::print(stderr,
                           "File (at path {}) is of an unrecognized format\n",
                           Options.Path);
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
                            std::print(stderr,
                                       "File is too small to be a valid "
                                       "mach-o ");
                            exit(1);
                        case OpenError::TooManyLoadCommands:
                            std::print(stderr,
                                       "Mach-O file has too many "
                                       "load-commands\n");
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
                            std::print(stderr,
                                       "File is too small to be a valid mach-o "
                                       "file\n");
                            exit(1);
                        case OpenError::TooManyArchitectures:
                            std::print(stderr,
                                       "Fat Mach-O file has too many "
                                       "architectures\n");
                            exit(1);
                        case OpenError::ArchOutOfBounds:
                            std::print(stderr,
                                       "Fat Mach-O File has at least 1 arch "
                                       "out-of-bounds of file\n");
                            exit(1);
                        case OpenError::OverlappingArchs:
                            std::print(stderr,
                                       "Fat Mach-O File has at least 2 archs "
                                       "overlap one another\n");
                            exit(1);
                        case OpenError::ArchsForSameCpu:
                            std::print(stderr,
                                       "Fat Mach-O File has at least 2 archs "
                                       "for the same cpu\n");
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
                            std::print(stderr,
                                       "Dyld-shared-cache has an unrecognized "
                                       "cputype\n");
                            exit(1);
                        case OpenError::SizeTooSmall:
                            std::print(stderr,
                                       "File is too small to be a valid "
                                       "dyld-shared-cache file\n");
                            exit(1);
                        case OpenError::NoMappings:
                            std::print(stderr,
                                       "Dyld Shared-Cache's has no mappings\n");
                            exit(1);
                        case OpenError::MappingsOutOfBounds:
                            std::print(stderr,
                                       "Dyld Shared-Cache's mappings are "
                                       "out-of-bound\n");
                            exit(1);
                        case OpenError::FirstMappingFileOffNotZero:
                            std::print(stderr,
                                       "Dyld Shared-Cache's first-mapping's "
                                       "file-offset is not 0x0\n");
                            exit(1);
                        case OpenError::FailedToOpenSubCaches:
                            std::print(stderr,
                                       "Failed to open sub-caches of "
                                       "dyld-shared-cache\n");
                            exit(1);
                        case OpenError::SubCacheHasDiffCpuKind:
                            std::print(stderr,
                                       "At least 1 Sub-cache has a different "
                                       "cpu-kind\n");
                            exit(1);
                        case OpenError::SubCacheHasDiffVersion:
                            std::print(stderr,
                                       "At least 1 Sub-cache has a different "
                                       "header-size\n");
                            exit(1);
                        case OpenError::RecursiveSubCache:
                            std::print(stderr,
                                       "At least 1 Sub-cache has its own "
                                       "sub-caches\n");
                            exit(1);
                        case OpenError::SubCacheListIsInvalid:
                            std::print(stderr,
                                       "List of Sub-Caches is invalid\n");
                            exit(1);
                        case OpenError::SubCacheFileDoesNotExist:
                            std::print(stderr,
                                       "Sub-cache file does not exist\n");
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
                            std::print(stderr,
                                       "Dsc-image has an invalid address "
                                       "inside shared-cache\n");
                            exit(1);
                        case OpenError::WrongCpuInfo:
                            std::print(stderr,
                                       "Dsc-image has a different cputype than "
                                       "its shared-cache\n");
                            exit(1);
                        case OpenError::NotMarkedAsImage:
                            std::print(stderr,
                                       "Dsc-image is not marked as a mach-o "
                                       "image\n");
                            exit(1);
                        case OpenError::NotADylib:
                            std::print(stderr,
                                       "Dsc-image is neither a dylib or "
                                       "dylinker\n");
                            exit(1);
                        case OpenError::SizeTooSmall:
                            std::print(stderr,
                                       "Dsc-image is too small to be a valid "
                                       "mach-o\n");
                            exit(1);
                        case OpenError::TooManyLoadCommands:
                            std::print(stderr,
                                       "Dsc-image has too many "
                                       "load-commands\n");
                            exit(1);
                        case OpenError::OutOfBoundsSegment:
                            std::print(stderr,
                                       "Dsc-image has a segment that is "
                                       "out-of-bounds of its shared-cache\n");
                            exit(1);
                        case OpenError::FailedToOpenDscSubCache:
                            std::print(stderr,
                                       "Dsc-image at  exists in a separate "
                                       "shared-cache file, a sub-cache\n");
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
        std::print(stderr,
                   "Operation doesn't support file at path {}\n",
                   Path);
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
                return RunResult(static_cast<const PrintId &>(Op).run(Object));
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
                    static_cast<const PrintRebaseOpcodeList &>(Op).run(Object));
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
                        std::print(stderr,
                                   "Operation doesn't support Mach-O Files, "
                                   "but does support Fat Mach-O Files.\nDrop "
                                   "the -arch option to run on the Fat Mach-O "
                                   "file\n");
                        exit(1);
                    }
                } else if (!Op.supportsObjectKind(Objects::Kind::MachO)) {
                    PrintUnsupportedError(Options.Path);
                }

                if (Options.ArchIndex == -1) {
                    std::print(stderr,
                               "Operation doesn't support Fat Mach-O Files. "
                               "Please select an arch by its index using "
                               "option -arch\n");
                    exit(1);
                }

                const auto ArchIndex =
                    static_cast<uint32_t>(Options.ArchIndex);

                const auto ArchCount = Fat.archCount();
                if (Utils::IndexOutOfBounds(ArchIndex, ArchCount)) {
                    std::print(stderr,
                               "An Arch-Index of {} is invalid. The provided "
                               "Fat Mach-O file only has {} architectures\n",
                               ArchIndex,
                               ArchCount);
                    exit(1);
                }

                const auto ArchObjectOrError =
                    Objects::OpenArch(Fat, ArchIndex);

                if (!ArchObjectOrError.has_value()) {
                    const auto Error = ArchObjectOrError.error();
                    if (Error.isUnrecognizedFormat()) {
                        std::print(stderr,
                                   "Architecture at index {} is of an "
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
                                    std::print(stderr,
                                               "Arch at index {} is too small "
                                               "to be a valid mach-o\n",
                                               ArchIndex);
                                    exit(1);
                                case ErrorKind::TooManyLoadCommands:
                                    std::print(stderr,
                                               "Arch at index {} has too many "
                                               "load-commands for its size\n",
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
                    if (Options.ImageOrdinal == -1) {
                        return RunOperation(Op, Dsc);
                    }

                    if (!Op.supportsObjectKind(Objects::Kind::DscImage)) {
                        std::print(stderr,
                                   "Operation doesn't support "
                                   "Dyld Shared-Cache Images, but does support "
                                   "the Dyld Shared-Cache itself.\nDrop the "
                                   "-image option to run on the "
                                   "Dyld Shared-Cache file\n");
                        exit(1);
                    }
                } else if (!Op.supportsObjectKind(Objects::Kind::DscImage)) {
                    PrintUnsupportedError(Options.Path);
                }

                if (Options.ImageOrdinal == -1) {
                    std::print(stderr,
                               "Operation doesn't support the "
                               "Dyld Shared-Cache. Please select an image "
                               "using option -image\n");
                    exit(1);
                }

                const auto ImageCount = Dsc.imageCount();
                const auto ImageOrdinal =
                    static_cast<uint32_t>(Options.ImageOrdinal);

                if (Utils::OrdinalOutOfBounds(ImageOrdinal, ImageCount)) {
                    std::print(stderr,
                               "An Image-Number of {} is invalid. The provided "
                               "Dyld Shared-Cache file only has {} images\n",
                               ImageOrdinal,
                               ImageCount);
                    exit(1);
                }

                const auto ImageOrError =
                    Objects::OpenImage(Dsc, ImageOrdinal - 1);

                if (!ImageOrError.has_value()) {
                    const auto Error = ImageOrError.error();
                    if (Error.isUnrecognizedFormat()) {
                        std::print(stderr,
                                   "Image at ordinal {} is of an unrecognized "
                                   "format\n",
                                   ImageOrdinal);
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
                                    std::print(stderr,
                                               "Address of image {} is "
                                               "out-of-bounds from "
                                               "dyld-shared-cache\n",
                                               (void *)Error.DscImageError
                                                .InvalidAddress.Address);
                                    exit(1);
                                case ErrorKind::WrongFormat:
                                    assert(false &&
                                           "Got Error WrongFormat for MachO "
                                           "OpenError");
                                case ErrorKind::SizeTooSmall:
                                    std::print(stderr,
                                               "Image at ordinal {} is too "
                                               "small to be a valid mach-o\n",
                                               ImageOrdinal);
                                    exit(1);
                                case ErrorKind::WrongCpuInfo:
                                    std::print(stderr,
                                               "Image has different cpu-info "
                                               "than the shared-cache\n");
                                    exit(1);
                                case ErrorKind::NotMarkedAsImage:
                                    std::print(stderr,
                                               "Image's mach_header is not "
                                               "marked as a shared-cache "
                                               "image\n");
                                    exit(1);
                                case ErrorKind::NotADylib:
                                    std::print(stderr,
                                               "Image is not a "
                                               "dynamic-library\n");
                                    exit(1);
                                case ErrorKind::OutOfBoundsSegment:
                                    std::print(stderr,
                                               "At least one of image's "
                                               "segments isn't fully contained "
                                               "within a single mapping\n");
                                    exit(1);
                                case ErrorKind::TooManyLoadCommands:
                                    std::print(stderr,
                                               "Image at ordinal {} has too "
                                               "many load-commands for its "
                                               "size\n",
                                               ImageOrdinal);
                                    exit(1);
                                case ErrorKind::FailedToOpenDscSubCache:
                                    std::print(stderr,
                                               "Image at ordinal {} exists in "
                                               "a separate dyld-shared-cache "
                                               "file, a sub-cache\n",
                                               ImageOrdinal);
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