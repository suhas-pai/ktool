/*
 * Objects/DyldSharedCache.cpp
 * © suhas pai
 */

#include "DyldSharedCache/Headers.h"

#include "Objects/DyldSharedCache.h"
#include "Objects/DscImage.h"

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
                if (MagicCpuKindStr == "   arm64") {
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

    static auto
    VerifyMap(const ADT::MemoryMap &Map,
              enum DyldSharedCache::CpuKind &CpuKindOut) noexcept
        -> DyldSharedCache::OpenError
    {
        using OpenError = DyldSharedCache::OpenError;

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

        if (HeaderV0->MappingOffset < sizeof(::DyldSharedCache::HeaderV0)) {
            return OpenError::MappingsOutOfBounds;
        }

        if (HeaderV0->MappingCount == 0) {
            return OpenError::NoMappings;
        }

        const auto MappingsRangeOpt = HeaderV0->getMappingInfoListRange();
        if (!MappingsRangeOpt.has_value()) {
            return OpenError::MappingsOutOfBounds;
        }

        if (!Map.range().contains(MappingsRangeOpt.value())) {
            return OpenError::MappingsOutOfBounds;
        }

        const auto FirstMapping =
            Map.get<const ::DyldSharedCache::MappingInfo, false>(
                HeaderV0->MappingOffset);

        if (FirstMapping->FileOffset != 0) {
            return OpenError::FirstMappingFileOffNotZero;
        }

        CpuKindOut = CpuKind;
        return OpenError::None;
    }

    auto DyldSharedCache::VerifySubCacheMap(const ADT::MemoryMap &Map) noexcept
        -> OpenError
    {
        using OpenError = DyldSharedCache::OpenError;

        auto CpuKind = DyldSharedCache::CpuKind::i386;
        if (const auto Error = VerifyMap(Map, CpuKind);
            Error != OpenError::None)
        {
            return OpenError::FailedToOpenSubCaches;
        }

        if (CpuKind != this->CpuKind) {
            return OpenError::SubCacheHasDiffCpuKind;
        }

        const auto Header = Map.base<::DyldSharedCache::HeaderV9, false>();
        if (Header->getVersion() != getVersion()) {
            return OpenError::SubCacheHasDiffVersion;
        }

        if (Header->SubCacheArrayCount != 0) {
            return OpenError::RecursiveSubCache;
        }

        return OpenError::None;
    }

    auto
    DyldSharedCache::CollectSubCaches(const ADT::FileMap::Prot Prot) noexcept
        -> OpenError
    {
        if (const auto ListOpt = subCacheEntryInfoList()) {
            for (const auto &SubCacheEntry : ListOpt.value()) {
                const auto FileSuffix = SubCacheEntry.fileSuffix();
                const auto SubCachePath = std::string(Path).append(FileSuffix);

                const auto FileMapOrErr =
                    ADT::FileMap::Open(SubCachePath.c_str(), Prot);

                if (FileMapOrErr.hasError()) {
                    return OpenError::FailedToOpenSubCaches;
                }

                auto FileMap = FileMapOrErr.ptr();
                const auto Map = FileMap->map();

                if (const auto Error = VerifySubCacheMap(Map);
                    Error != OpenError::None)
                {
                    return Error;
                }

                SubCacheList.emplace(FileSuffix, SubCacheInfo(FileMap, Map));
            }
        } else if (const auto ListOpt = subCacheEntryV1InfoList()) {
            for (auto I = uint32_t(1); I <= ListOpt.value().size(); I++) {
                const auto FileSuffix = std::string(".") + std::to_string(I);
                const auto SubCachePath = std::string(Path).append(FileSuffix);
                const auto FileMapOrErr =
                    ADT::FileMap::Open(SubCachePath.c_str(), Prot);

                if (FileMapOrErr.hasError()) {
                    return OpenError::FailedToOpenSubCaches;
                }

                auto FileMap = FileMapOrErr.ptr();
                const auto Map = FileMap->map();

                if (const auto Error = VerifySubCacheMap(Map);
                    Error != OpenError::None)
                {
                    return Error;
                }

                SubCacheList.emplace(FileSuffix, SubCacheInfo(FileMap, Map));
            }
        }

        return OpenError::None;
    }

    auto
    DyldSharedCache::Open(const ADT::MemoryMap &Map,
                          const std::string_view Path,
                          const ADT::FileMap::Prot SubCacheProt) noexcept ->
        ADT::PointerOrError<DyldSharedCache, OpenError>
    {
        auto CpuKind = CpuKind::i386;
        if (const auto Error = VerifyMap(Map, CpuKind);
            Error != OpenError::None)
        {
            return Error;
        }

        const auto Result = new DyldSharedCache(Map, CpuKind, Path);
        if (const auto Error = Result->CollectSubCaches(SubCacheProt);
            Error != OpenError::None)
        {
            delete Result;
            return Error;
        }

        return Result;
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