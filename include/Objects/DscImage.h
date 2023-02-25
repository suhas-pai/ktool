/*
 * Objects/DscImage.h
 * © suhas pai
 */

#pragma once

#include "DyldSharedCache.h"
#include "DyldSharedCache/Headers.h"

#include "Objects/DyldSharedCache/CacheInfo.h"
#include "Objects/MachO.h"

namespace Objects {
    struct DyldSharedCache;
    struct DscImage : public MachO {
    protected:
        const DyldSharedCache &Dsc;
        const ::DyldSharedCache::ImageInfo &ImageInfo;

        DyldSharedSingleCacheInfo DscInfo;

        explicit
        DscImage(const DyldSharedCache &Dsc,
                 const DyldSharedSingleCacheInfo &DscInfo,
                 const ::DyldSharedCache::ImageInfo &ImageInfo,
                 const ADT::MemoryMap &Map) noexcept
        : MachO(Map, Kind::DscImage), Dsc(Dsc), ImageInfo(ImageInfo),
          DscInfo(DscInfo) {}
    public:
        enum class OpenError {
            None,

            InvalidAddress,
            WrongFormat,
            WrongCpuInfo,

            NotMarkedAsImage,
            NotADylib,

            SizeTooSmall,
            TooManyLoadCommands,

            OutOfBoundsSegment,
        };

        ~DscImage() noexcept override {}

        static auto
        Open(const DyldSharedCache &Dsc,
             const ::DyldSharedCache::ImageInfo &ImageInfo) noexcept
             -> ADT::PointerOrError<DscImage, OpenError>;

        [[nodiscard]] constexpr auto &dsc() const noexcept {
            return Dsc;
        }

        [[nodiscard]] constexpr auto info() const noexcept {
            return ImageInfo;
        }

        [[nodiscard]] constexpr auto address() const noexcept {
            return info().Address;
        }

        [[nodiscard]] inline auto path() const noexcept {
            return Map.string(ImageInfo.PathFileOffset);
        }

        [[nodiscard]] constexpr auto dscMap() const noexcept {
            return DscInfo.map();
        }

        [[nodiscard]]
        ADT::MemoryMap getMapForFileOffsets() const noexcept override;

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getMapForAddrRange(const ADT::Range &AddrRange,
                           const bool InsideMappings = true) const noexcept
                -> std::optional<
                    std::pair<DyldSharedSingleCacheInfo, ADT::MemoryMap>>
        {
            return dsc().getMapForAddrRange<T, Size>(AddrRange, InsideMappings);
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getMapForFileRange(const ADT::Range &FileRange,
                           const bool InsideMappings = true) const noexcept
            -> T *
        {
            return dsc().getForFileRange<T, Size>(FileRange, InsideMappings);
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getPtrForAddress(const uint64_t Address,
                         const bool InsideMappings = true,
                         uint64_t *const TotalAvailSize = nullptr,
                         uint64_t *const FileOffsetOut = nullptr) const noexcept
            -> std::optional<std::pair<const DyldSharedSingleCacheInfo &, T *>>
        {
            return
                dsc().getPtrForAddress<T, Size>(Address,
                                                InsideMappings,
                                                TotalAvailSize,
                                                FileOffsetOut);
        }
    };
}