/*
 * Objects/DscImage.cpp
 * © suhas pai
 */

#include <ranges>

#include "DyldSharedCache/Headers.h"
#include "Objects/DscImage.h"

namespace Objects {
    auto
    DscImage::Open(const DyldSharedCache &Dsc,
                   const uint32_t ImageIndex) noexcept
        -> std::expected<DscImage *, Error>
    {
        const auto ImageList = Dsc.imageInfoList();
        const auto &ImageInfo = ImageList[ImageIndex];

        auto MaxPossibleSize = uint64_t();
        auto FileOffset = uint64_t();

        const auto HeaderPairOpt =
            Dsc.getPtrForAddress<::MachO::Header>(ImageInfo.Address,
                                                  /*InsideMappings=*/true,
                                                  &MaxPossibleSize,
                                                  &FileOffset);

        if (!HeaderPairOpt.has_value()) {
            switch (HeaderPairOpt.error().Kind) {
                case DyldSharedCache::OpenError::None:
                    return std::unexpected(
                        Error::invalidAddress(ImageInfo.Address));
                case DyldSharedCache::OpenError::WrongFormat:
                case DyldSharedCache::OpenError::UnrecognizedCpuKind:
                case DyldSharedCache::OpenError::SizeTooSmall:
                case DyldSharedCache::OpenError::NoMappings:
                case DyldSharedCache::OpenError::MappingsOutOfBounds:
                case DyldSharedCache::OpenError::FirstMappingFileOffNotZero:
                case DyldSharedCache::OpenError::FailedToOpenSubCaches:
                case DyldSharedCache::OpenError::SubCacheListIsInvalid:
                case DyldSharedCache::OpenError::SubCacheHasDiffCpuKind:
                case DyldSharedCache::OpenError::SubCacheHasDiffVersion:
                case DyldSharedCache::OpenError::RecursiveSubCache:
                case DyldSharedCache::OpenError::SubCacheFileDoesNotExist:
                    return std::unexpected(
                        Error(OpenError::FailedToOpenDscSubCache));
            }

            assert(0 && "Expected an error");
        }

        const auto &[SingleCache, Header] = HeaderPairOpt.value();
        if (MaxPossibleSize < sizeof(::MachO::Header)) {
            if (MaxPossibleSize < sizeof(::MachO::Magic)) {
                return std::unexpected(
                    Error::invalidAddress(ImageInfo.Address));
            }

            if (::MachO::MagicIsThin(Header->Magic)) {
                return std::unexpected(OpenError::SizeTooSmall);
            }

            return std::unexpected(OpenError::WrongFormat);
        }

        if (!::MachO::MagicIsThin(Header->Magic)) {
            return std::unexpected(OpenError::WrongFormat);
        }

        if (!Dsc.cpuInfoMatches(Header->cpuKind(), Header->cpuSubKind())) {
            return std::unexpected(OpenError::WrongCpuInfo);
        }

        if (!Header->flags().dylibInCache()) {
            return std::unexpected(OpenError::NotMarkedAsImage);
        }

        if (Header->fileKind() != ::MachO::FileKind::DynamicLibrary &&
            Header->fileKind() != ::MachO::FileKind::DynamicLinker)
        {
            return std::unexpected(OpenError::NotADylib);
        }

        const auto LoadCommandsSize = Header->sizeOfCmds();
        const auto MinLoadCommandSizeOpt =
            Utils::MulAndCheckOverflow(sizeof(::MachO::LoadCommand),
                                       Header->ncmds());

        if (!MinLoadCommandSizeOpt.has_value()) {
            return std::unexpected(OpenError::TooManyLoadCommands);
        }

        const auto MinLoadCommandSize = MinLoadCommandSizeOpt.value();
        if (LoadCommandsSize < MinLoadCommandSize) {
            return std::unexpected(OpenError::TooManyLoadCommands);
        }

        if (MaxPossibleSize < Header->size() + LoadCommandsSize) {
            return std::unexpected(OpenError::TooManyLoadCommands);
        }

        const auto Map = SingleCache.map();
        const auto IsBigEndian = Header->isBigEndian();

        const auto LoadCommandsRange =
            ADT::Range::FromSize(FileOffset + Header->size(), LoadCommandsSize);
        const auto LoadCommandsMap =
            ::MachO::LoadCommandsMap(ADT::MemoryMap(Map, LoadCommandsRange),
                                     IsBigEndian);

        auto FileSize = uint64_t();
        if (Header->is64Bit()) {
            for (auto Iter = LoadCommandsMap.begin();
                 Iter != LoadCommandsMap.end();
                 Iter++)
            {
                if (const auto Segment =
                        Iter.dyn_cast<::MachO::SegmentCommand64>())
                {
                    auto FoundMapping = true;
                    for (const auto &Mapping : Dsc.mappingInfoList()) {
                        const auto FileRange = Segment->fileRange(IsBigEndian);
                        if (Mapping.fileRange().contains(FileRange)) {
                            FoundMapping = true;
                            break;
                        }
                    }

                    if (!FoundMapping) {
                        return std::unexpected(OpenError::OutOfBoundsSegment);
                    }

                    const auto NewFileSize =
                        Utils::AddAndCheckOverflow(
                            Segment->fileSize(IsBigEndian), FileSize);

                    if (!NewFileSize.has_value()) {
                        return std::unexpected(OpenError::OutOfBoundsSegment);
                    }

                    FileSize = NewFileSize.value();
                }
            }
        } else {
            for (const auto &Segment :
                    LoadCommandsMap |
                    ::MachO::LCMapFilterType<
                        ::MachO::SegmentCommand>(IsBigEndian))
            {
                const auto FoundMapping =
                    std::ranges::find_if(
                        Dsc.mappingInfoList(),
                        [&](const auto &Mapping) {
                            const auto FileRange =
                                Segment->fileRange(IsBigEndian);
                            return Mapping.fileRange().contains(FileRange);
                        });

                if (FoundMapping != Dsc.mappingInfoList().end()) {
                    return std::unexpected(OpenError::OutOfBoundsSegment);
                }

                FileSize += Segment->fileSize(IsBigEndian);
                if (FileSize > std::numeric_limits<uint32_t>::max()) {
                    return std::unexpected(OpenError::OutOfBoundsSegment);
                }
            }
        }

        FileSize = std::min(FileSize, MaxPossibleSize);

        const auto ImageRange = ADT::Range::FromSize(FileOffset, FileSize);
        const auto ImageMap = ADT::MemoryMap(Map, ImageRange);

        return new DscImage(Dsc, SingleCache, ImageInfo, ImageMap, ImageIndex);
    }

    auto DscImage::getBaseAddress() const noexcept -> std::optional<uint64_t> {
        const auto IsBigEndian = this->isBigEndian();
        if (this->is64Bit()) {
             auto SegmentListRange = this->loadCommandsMap() |
                ::MachO::LCMapFilterType<::MachO::SegmentCommand64>(
                    IsBigEndian) |
                std::views::filter(
                    [IsBigEndian](const auto &Segment) noexcept {
                        return Segment->vmRange(IsBigEndian).size() > 0;
                    }
                ) |
                std::views::take(1) |
                std::views::transform(
                    [IsBigEndian](const auto &Segment) noexcept {
                        return Segment->vmRange(IsBigEndian).front();
                    }
                );

            if (!SegmentListRange.empty()) {
                return SegmentListRange.front();
            }
        } else {
            auto SegmentListRange = this->loadCommandsMap() |
                ::MachO::LCMapFilterType<::MachO::SegmentCommand>(IsBigEndian) |
                std::views::filter(
                    [IsBigEndian](const auto &Segment) noexcept {
                        return Segment->vmRange(IsBigEndian).size() > 0;
                    }
                ) |
                std::views::take(1) |
                std::views::transform(
                    [IsBigEndian](const auto &Segment) noexcept {
                        return Segment->vmRange(IsBigEndian).front();
                    }
                );

            if (!SegmentListRange.empty()) {
                return SegmentListRange.front();
            }
        }

        return std::nullopt;
    }

    auto DscImage::getMapForFileOffsets() const noexcept -> ADT::MemoryMap {
        return this->dscMap();
    }
}
