/*
 * Objects/DyldSharedCache.cpp
 * © suhas pai
 */

#include "DyldSharedCache/Headers.h"
#include "Mach/Machine.h"

#include "MachO/Magic.h"
#include "Objects/DscImage.h"
#include "Objects/DyldSharedCache.h"
#include "Objects/Base.h"
#include "Utils/Misc.h"

namespace Objects {
    constexpr auto GetCpuKind(const std::string_view MagicCpuKindStr) noexcept {
        enum DyldSharedCache::CpuKind CpuKind = DyldSharedCache::CpuKind::i386;
        switch (CpuKind) {
            case DyldSharedCache::CpuKind::i386:
                if (MagicCpuKindStr == "    i386") {
                    CpuKind = DyldSharedCache::CpuKind::i386;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::x86_64:
                if (MagicCpuKindStr == "  x86_64") {
                    CpuKind = DyldSharedCache::CpuKind::x86_64;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::x86_64h:
                if (MagicCpuKindStr == " x86_64h") {
                    CpuKind = DyldSharedCache::CpuKind::x86_64h;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::armv5:
                if (MagicCpuKindStr == "   armv5") {
                    CpuKind = DyldSharedCache::CpuKind::armv5;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::armv6:
                if (MagicCpuKindStr == "   armv6") {
                    CpuKind = DyldSharedCache::CpuKind::armv6;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::armv7:
                if (MagicCpuKindStr == "   armv7") {
                    CpuKind = DyldSharedCache::CpuKind::armv7;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::armv7f:
                if (MagicCpuKindStr == "  armv7f") {
                    CpuKind = DyldSharedCache::CpuKind::armv7f;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::armv7k:
                if (MagicCpuKindStr == "  armv7k") {
                    CpuKind = DyldSharedCache::CpuKind::armv7k;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::armv7s:
                if (MagicCpuKindStr == "  armv7s") {
                    CpuKind = DyldSharedCache::CpuKind::armv7s;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::arm64:
                if (MagicCpuKindStr == "  arm64") {
                    CpuKind = DyldSharedCache::CpuKind::arm64;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::arm64e:
                if (MagicCpuKindStr == "  arm64e") {
                    CpuKind = DyldSharedCache::CpuKind::arm64e;
                    break;
                }

                [[fallthrough]];
            case DyldSharedCache::CpuKind::arm64_32:
                if (MagicCpuKindStr == "arm64_32") {
                    CpuKind = DyldSharedCache::CpuKind::arm64_32;
                    break;
                }

                return std::pair(CpuKind, false);
        }

        return std::pair(CpuKind, true);
    }
    auto DyldSharedCache::Open(const ADT::MemoryMap &Map) noexcept ->
        ADT::PointerOrError<DyldSharedCache, OpenError>
    {
        const auto HeaderV0 = Map.base<::DyldSharedCache::HeaderV0>();
        if (HeaderV0 == nullptr) {
            const auto MagicPtr =
                Map.base<const char,
                         /*Verify=*/true,
                         sizeof(::DyldSharedCache::HeaderV0::Magic)>();

            if (MagicPtr == nullptr) {
                return OpenError::SizeTooSmall;
            }

            const auto MagicLength =
                strnlen(MagicPtr, sizeof(::DyldSharedCache::HeaderV0::Magic));

            const auto Magic = std::string_view(MagicPtr, MagicLength);
            constexpr auto MagicStart = std::string_view("dyld_v1");

            if (!Magic.starts_with(MagicStart)) {
                return OpenError::WrongFormat;
            }

            const auto MagicCpuKindStr = Magic.substr(MagicStart.length());
            const auto [CpuKind, IsValid] = GetCpuKind(MagicCpuKindStr);

            if (!IsValid) {
                return OpenError::UnrecognizedCpuKind;
            }

            return OpenError::WrongFormat;
        }

        const auto Magic = HeaderV0->magic();
        constexpr auto MagicStart = std::string_view("dyld_v1");

        if (!Magic.starts_with(MagicStart)) {
            return OpenError::WrongFormat;
        }

        const auto MagicCpuKindStr = Magic.substr(MagicStart.length());
        const auto [CpuKind, IsValid] = GetCpuKind(MagicCpuKindStr);

        if (!IsValid) {
            return OpenError::UnrecognizedCpuKind;
        }

        const auto FirstMapping = HeaderV0->mappingInfoList().front();
        if (FirstMapping.FileOffset != 0) {
            return OpenError::FirstMappingFileOffNotZero;
        }

        return new DyldSharedCache(Map, CpuKind);
    }

    auto DyldSharedCache::subCacheEntryV1InfoList() const noexcept
        -> std::optional<ADT::List<::DyldSharedCache::SubCacheEntryV1>>
    {
        if (!header().hasSubCacheV1List()) {
            return std::nullopt;
        }

        const auto &Header = headerV8();
        const auto Array =
            map().get<::DyldSharedCache::SubCacheEntryV1>(
                Header.SubCacheArrayOffset, Header.SubCacheArrayCount);

        if (Array == nullptr) {
            return std::nullopt;
        }

        return ADT::List(Array, Header.SubCacheArrayCount);
    }

    auto DyldSharedCache::subCacheEntryInfoList() const noexcept
        -> std::optional<ADT::List<::DyldSharedCache::SubCacheEntry>>
    {
        if (!header().hasSubCacheList()) {
            return std::nullopt;
        }

        const auto &Header = headerV9();
        const auto Array =
            map().get<::DyldSharedCache::SubCacheEntry>(
                Header.SubCacheArrayOffset, Header.SubCacheArrayCount);

        if (Array == nullptr) {
            return std::nullopt;
        }

        return ADT::List(Array, Header.SubCacheArrayCount);
    }

    auto
    DyldSharedCache::getImageObjectAtIndex(const uint32_t Index) const noexcept
        -> Objects::OpenResult
    {
        assert(!Utils::IndexOutOfBounds(Index, imageCount()));

        const auto &ImageInfo = imageInfoList()[Index];
        const auto ObjectOrErr = DscImage::Open(*this, ImageInfo);

        if (const auto Ptr = ObjectOrErr.ptr()) {
            return Ptr;
        }

        return Objects::OpenError(Kind::DscImage,
                                  static_cast<uint32_t>(ObjectOrErr.error()));
    }
}