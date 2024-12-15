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
            FailedToOpenDscSubCache,

            WrongFormat,
            WrongCpuInfo,

            NotMarkedAsImage,
            NotADylib,

            SizeTooSmall,
            TooManyLoadCommands,

            OutOfBoundsSegment,
        };

        struct Error {
            OpenError Kind;

            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wnested-anon-types"

            union {
                struct {
                    uint64_t Address;
                } InvalidAddress;
            };

            #pragma clang diagnostic pop
            explicit Error(const OpenError Kind) noexcept : Kind(Kind) {}

            static inline auto invalidAddress(const uint64_t Address) noexcept {
                auto Result = Error(OpenError::InvalidAddress);
                Result.InvalidAddress.Address = Address;

                return Result;
            }
        };

        ~DscImage() noexcept override {}

        static auto
        Open(const DyldSharedCache &Dsc,
             const ::DyldSharedCache::ImageInfo &ImageInfo) noexcept
                -> std::expected<DscImage *, Error>;

        [[nodiscard]] constexpr auto &dsc() const noexcept {
            return this->Dsc;
        }

        [[nodiscard]] constexpr auto &info() const noexcept {
            return this->ImageInfo;
        }

        [[nodiscard]] constexpr auto address() const noexcept {
            return this->info().Address;
        }

        [[nodiscard]] inline auto path() const noexcept {
            return this->Map.string(ImageInfo.PathFileOffset);
        }

        [[nodiscard]] constexpr auto dscMap() const noexcept {
            return this->DscInfo.map();
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
            return this->dsc().getMapForAddrRange<T, Size>(AddrRange,
                                                           InsideMappings);
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getMapForFileRange(const ADT::Range &FileRange,
                           const bool InsideMappings = true) const noexcept
            -> T *
        {
            return this->dsc().getForFileRange<T, Size>(FileRange,
                                                        InsideMappings);
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
                this->dsc().getPtrForAddress<T, Size>(Address,
                                                      InsideMappings,
                                                      TotalAvailSize,
                                                      FileOffsetOut);
        }
    };
}