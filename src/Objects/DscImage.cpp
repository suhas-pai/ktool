/*
 * Objects/DscImage.cpp
 * © suhas pai
 */

#include "DyldSharedCache/Headers.h"

#include "MachO/Header.h"
#include "Objects/DyldSharedCache.h"

namespace Objects {
    auto
    DscImage::Open(const DyldSharedCache &Dsc,
                   const ::DyldSharedCache::ImageInfo &ImageInfo) noexcept
        -> ADT::PointerOrError<DscImage, OpenError>
    {
        auto MaxPossibleSize = uint64_t();
        auto FileOffset = uint64_t();

        const auto Header =
            Dsc.getPtrForAddress<::MachO::Header>(ImageInfo.Address,
                                                  &MaxPossibleSize,
                                                  &FileOffset);

        if (Header == nullptr) {
            const auto MagicPtr =
                Dsc.getPtrForAddress<::MachO::Magic>(ImageInfo.Address);

            if (MagicPtr == nullptr) {
                return OpenError::WrongFormat;
            }

            if (::MachO::MagicIsThin(*MagicPtr)) {
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
                ADT::MemoryMap(Dsc.map(), LoadCommandsRange), IsBigEndian);

        auto FileSize = uint64_t();
        if (Header->is64Bit()) {
            for (auto Iter = LoadCommandsMap.begin();
                 Iter != LoadCommandsMap.end();
                 Iter++)
            {
                if (const auto Segment =
                        Iter.dyn_cast<::MachO::SegmentCommand64>())
                {
                    const auto NewFileSize =
                        Utils::AddAndCheckOverflow(
                            Segment->fileSize(IsBigEndian), FileSize);

                    if (!NewFileSize.has_value()) {
                        return OpenError::SizeTooLarge;
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
                    const auto NewFileSize =
                        Utils::AddAndCheckOverflow(
                            Segment->fileSize(IsBigEndian), FileSize);

                    if (!NewFileSize.has_value()) {
                        return OpenError::SizeTooLarge;
                    }

                    FileSize = NewFileSize.value();
                }
            }
        }

        if (FileSize > MaxPossibleSize) {
            return OpenError::SizeTooLarge;
        }

        const auto ImageRange = ADT::Range::FromSize(FileOffset, FileSize);
        const auto ImageMap = ADT::MemoryMap(Dsc.map(), ImageRange);

        return new DscImage(Dsc.map(), ImageInfo, ImageMap);
    }

    [[nodiscard]]
    auto DscImage::getMapForFileOffsets() const noexcept -> ADT::MemoryMap {
        return dscMap();
    }
}