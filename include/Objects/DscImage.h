/*
 * Objects/DscImage.h
 * © suhas pai
 */

#pragma once

#include "ADT/MemoryMap.h"
#include "DyldSharedCache/Headers.h"
#include "Objects/MachO.h"

namespace Objects {
    struct DyldSharedCache;
    struct DscImage : public MachO {
    protected:
        ADT::MemoryMap DscMap;
        const ::DyldSharedCache::ImageInfo &ImageInfo;

        explicit
        DscImage(const ADT::MemoryMap &DscMap,
                 const ::DyldSharedCache::ImageInfo &ImageInfo,
                 const ADT::MemoryMap &Map) noexcept
        : MachO(Map), DscMap(DscMap), ImageInfo(ImageInfo) {}
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

            SizeTooLarge,
        };

        ~DscImage() noexcept override {}

        static auto
        Open(const DyldSharedCache &Dsc,
             const ::DyldSharedCache::ImageInfo &ImageInfo) noexcept
             -> ADT::PointerOrError<DscImage, OpenError>;

        [[nodiscard]] constexpr auto dscMap() const noexcept {
            return DscMap;
        }

        [[nodiscard]] constexpr auto info() const noexcept {
            return ImageInfo;
        }

        [[nodiscard]] inline auto path() const noexcept {
            return std::string_view(ImageInfo.pathPtr(dscMap().base()));
        }
    };
}