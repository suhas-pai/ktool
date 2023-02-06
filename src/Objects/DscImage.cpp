/*
 * Objects/DscImage.cpp
 * © suhas pai
 */

#include "DyldSharedCache/Headers.h"
#include "Objects/DyldSharedCache.h"

namespace Objects {
    auto
    DscImage::Open(const DyldSharedCache &Dsc,
                   const ::DyldSharedCache::ImageInfo &ImageInfo) noexcept
        -> ADT::PointerOrError<DscImage, OpenError>
    {
        auto MaxPossibleSize = uint64_t();
        auto FileOffset = uint64_t();
        auto Map = ADT::MemoryMap();

        const auto HeaderPair =
            Dsc.getPtrForAddress<::MachO::Header>(ImageInfo.Address,
                                                  Map,
                                                  /*InsideMappings=*/true,
                                                  &MaxPossibleSize,
                                                  &FileOffset);

        if (!HeaderPair.has_value()) {
            return OpenError::SizeTooSmall;
        }

        const auto Header = HeaderPair->second;
        if (MaxPossibleSize < sizeof(::MachO::Header)) {
            if (MaxPossibleSize < sizeof(::MachO::Magic)) {
                return OpenError::InvalidAddress;
            }

            if (::MachO::MagicIsThin(Header->Magic)) {
                return OpenError::SizeTooSmall;
            }

            return OpenError::WrongFormat;
        }

        if (!::MachO::MagicIsThin(Header->Magic)) {
            return OpenError::WrongFormat;
        }

        if (!Dsc.cpuInfoMatches(Header->cpuKind(), Header->cpuSubKind())) {
            return OpenError::WrongCpuInfo;
        }

        if (!Header->flags().dylibInCache()) {
            return OpenError::NotMarkedAsImage;
        }

        if (Header->fileKind() != ::MachO::FileKind::DynamicLibrary &&
            Header->fileKind() != ::MachO::FileKind::DynamicLinker)
        {
            return OpenError::NotADylib;
        }

        const auto LoadCommandsSize = Header->sizeOfCmds();
        const auto MinLoadCommandSize =
            sizeof(::MachO::LoadCommand) * Header->ncmds();

        if (LoadCommandsSize < MinLoadCommandSize) {
            return OpenError::TooManyLoadCommands;
        }

        if (MaxPossibleSize < Header->size() + LoadCommandsSize) {
            return OpenError::TooManyLoadCommands;
        }

        const auto IsBigEndian = Header->isBigEndian();
        const auto LoadCommandsRange =
            ADT::Range::FromSize(FileOffset + Header->size(), LoadCommandsSize);
        const auto LoadCommandsMap =
            ::MachO::LoadCommandsMap(
                ADT::MemoryMap(Map, LoadCommandsRange), IsBigEndian);

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
                        return OpenError::OutOfBoundsSegment;
                    }

                    const auto NewFileSize =
                        Utils::AddAndCheckOverflow(
                            Segment->fileSize(IsBigEndian), FileSize);

                    if (!NewFileSize.has_value()) {
                        return OpenError::OutOfBoundsSegment;
                    }

                    FileSize = NewFileSize.value();
                }
            }
        } else {
            for (auto Iter = LoadCommandsMap.begin();
                 Iter != LoadCommandsMap.end();
                 Iter++)
            {
                if (const auto Segment =
                        Iter.dyn_cast<::MachO::SegmentCommand>())
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
                        return OpenError::OutOfBoundsSegment;
                    }

                    FileSize += Segment->fileSize(IsBigEndian);
                    if (FileSize > std::numeric_limits<uint32_t>::max()) {
                        return OpenError::OutOfBoundsSegment;
                    }
                }
            }
        }

        FileSize = std::min(FileSize, MaxPossibleSize);

        const auto ImageRange = ADT::Range::FromSize(FileOffset, FileSize);
        const auto ImageMap = ADT::MemoryMap(Map, ImageRange);

        return new DscImage(Map, ImageInfo, ImageMap);
    }

    auto DscImage::getMapForFileOffsets() const noexcept -> ADT::MemoryMap {
        return dscMap();
    }
}