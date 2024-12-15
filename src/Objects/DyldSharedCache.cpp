/*
 * Objects/DyldSharedCache.cpp
 * © suhas pai
 */

#include <sys/fcntl.h>
#include <sys/stat.h>

#include <format>
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
    VerifyHeaderMagicAndCpuKind(
        const std::string_view &Magic,
        enum DyldSharedCache::CpuKind &CpuKindOut) noexcept
            -> DyldSharedCache::OpenError
    {
        constexpr auto MagicStart = std::string_view("dyld_v1");
        if (!Magic.starts_with(MagicStart)) {
            return DyldSharedCache::OpenError::WrongFormat;
        }

        const auto MagicCpuKindStr = Magic.substr(MagicStart.length());
        const auto [CpuKind, IsValid] = GetCpuKind(MagicCpuKindStr);

        if (!IsValid) {
            return DyldSharedCache::OpenError::UnrecognizedCpuKind;
        }

        CpuKindOut = CpuKind;
        return DyldSharedCache::OpenError::None;
    }

    static auto
    VerifyHeaderMappingInfo(
        ::DyldSharedCache::HeaderV0 &Header,
        ADT::Range Range) noexcept
            -> DyldSharedCache::OpenError
    {
        if (Header.MappingOffset < sizeof(::DyldSharedCache::HeaderV0)) {
            return DyldSharedCache::OpenError::MappingsOutOfBounds;
        }

        if (Header.MappingCount == 0) {
            return DyldSharedCache::OpenError::NoMappings;
        }

        const auto MappingsRangeOpt = Header.getMappingInfoListRange();
        if (!MappingsRangeOpt.has_value()) {
            return DyldSharedCache::OpenError::MappingsOutOfBounds;
        }

        if (!Range.contains(MappingsRangeOpt.value())) {
            return DyldSharedCache::OpenError::MappingsOutOfBounds;
        }

        return DyldSharedCache::OpenError::None;
    }

    static auto VerifyMappingList(
        const std::span<::DyldSharedCache::MappingInfo> &MappingSpan) noexcept
            -> DyldSharedCache::OpenError
    {
        const auto &FirstMapping = MappingSpan.front();
        if (FirstMapping.FileOffset != 0) {
            return DyldSharedCache::OpenError::FirstMappingFileOffNotZero;
        }

        for (const auto &Mapping : MappingSpan) {
            if (!Mapping.addressRange().end().has_value()) {
                return DyldSharedCache::OpenError::MappingsOutOfBounds;
            }

            if (!Mapping.fileRange().end().has_value()) {
                return DyldSharedCache::OpenError::MappingsOutOfBounds;
            }
        }

        return DyldSharedCache::OpenError::None;
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

            const auto MagicSv =
                std::string_view(
                    MagicPtr,
                    strnlen(MagicPtr,
                            sizeof(::DyldSharedCache::HeaderV0::Magic)));

            if (const auto Error =
                    VerifyHeaderMagicAndCpuKind(MagicSv, CpuKindOut);
                Error != OpenError::None)
            {
                return Error;
            }

            return OpenError::WrongFormat;
        }

        if (const auto Error =
                VerifyHeaderMagicAndCpuKind(CARR_TO_SV(HeaderV0->Magic),
                                            CpuKindOut);
            Error != OpenError::None)
        {
            return Error;
        }

        if (const auto Error = VerifyHeaderMappingInfo(*HeaderV0, Map.range());
            Error != OpenError::None)
        {
            return Error;
        }

        auto MappingList =
            std::span<::DyldSharedCache::MappingInfo>(
                Map.get<::DyldSharedCache::MappingInfo, false>(
                    HeaderV0->MappingOffset),
                HeaderV0->MappingCount);

        if (const auto Error = VerifyMappingList(MappingList);
            Error != OpenError::None)
        {
            return Error;
        }

        return OpenError::None;
    }

    auto
    DyldSharedCache::VerifySubCacheMap(const ADT::MemoryMap &Map) const noexcept
        -> Error
    {
        using OpenError = DyldSharedCache::OpenError;

        auto CpuKind = DyldSharedCache::CpuKind::i386;
        if (const auto Error = VerifyMap(Map, CpuKind);
            Error != OpenError::None)
        {
            return DyldSharedCache::Error(OpenError::FailedToOpenSubCaches);
        }

        if (CpuKind != this->CpuKind) {
            return DyldSharedCache::Error(OpenError::SubCacheHasDiffCpuKind);
        }

        const auto Header = Map.base<::DyldSharedCache::HeaderV9, false>();
        if (Header->getVersion() != this->getVersion()) {
            return DyldSharedCache::Error(OpenError::SubCacheHasDiffVersion);
        }

        if (Header->SubCacheArrayCount != 0) {
            return DyldSharedCache::Error(OpenError::RecursiveSubCache);
        }

        return DyldSharedCache::Error(OpenError::None);
    }

    auto
    DyldSharedCache::OpenSubCacheFileMap(
        const SubCacheInfo &SubCache) const noexcept -> Error
    {
        if (SubCache.FileMap != nullptr) {
            return Error(OpenError::None);
        }

        if (SubCache.Error.Kind != OpenError::None) {
            return SubCache.Error;
        }

        const auto FileMapOrErr =
            ADT::FileMap::Open(SubCache.Path.c_str(), SubCache.Prot);

        if (!FileMapOrErr.has_value()) {
            return Error(OpenError::FailedToOpenSubCaches);
        }

        auto FileMap = FileMapOrErr.value();
        const auto Map = FileMap->map();

        if (const auto Err = VerifySubCacheMap(Map);
            Err.Kind != OpenError::None)
        {
            return Error(Err);
        }

        auto &SingleInfo = (SingleCacheInfo &)SubCache.Info;
        SingleInfo.Map = Map;

        return Error(OpenError::None);
    }

    auto
    DyldSharedCache::FillSubCacheInfo(
        SubCacheInfo &SubCache,
        const std::filesystem::path &Path,
        const ADT::FileMap::Prot Prot) noexcept -> Error
    {
        const int Fd = ::open(Path.c_str(), O_RDONLY);
        if (Fd < 0) {
            if (access(Path.c_str(), F_OK) < 0) {
                SubCache.Error = Error(OpenError::SubCacheFileDoesNotExist);
                return SubCache.Error;
            }

            SubCache.Error = Error(OpenError::FailedToOpenSubCaches);
            return SubCache.Error;
        }

        struct stat Stat;
        if (::fstat(Fd, &Stat) < 0) {
            close(Fd);
            SubCache.Error = Error(OpenError::FailedToOpenSubCaches);

            return SubCache.Error;
        }

        const auto MapRange =
            ADT::Range::FromSize(0, static_cast<size_t>(Stat.st_size));

        auto Header = ::DyldSharedCache::HeaderV0();
        if (const auto HeaderSize = ::read(Fd, &Header, sizeof(Header));
            HeaderSize < static_cast<ssize_t>(sizeof(Header)))
        {
            close(Fd);
            SubCache.Error = Error(OpenError::FailedToOpenSubCaches);

            return SubCache.Error;
        }

        auto CpuKind = DyldSharedCache::CpuKind::i386;
        if (const auto Err =
                VerifyHeaderMagicAndCpuKind(CARR_TO_SV(Header.Magic), CpuKind);
            Err != DyldSharedCache::OpenError::None)
        {
            close(Fd);
            SubCache.Error = Error(Err);

            return SubCache.Error;
        }

        if (const auto Err = VerifyHeaderMappingInfo(Header, MapRange);
            Err != DyldSharedCache::OpenError::None)
        {
            close(Fd);
            SubCache.Error = Error(Err);

            return SubCache.Error;
        }

        const auto MappingListRangeOpt = Header.getMappingInfoListRange();
        if (!MappingListRangeOpt.has_value()) {
            close(Fd);
            SubCache.Error = Error(OpenError::FailedToOpenSubCaches);

            return SubCache.Error;
        }

        const auto MappingList =
            std::unique_ptr<::DyldSharedCache::MappingInfo[]>(
                new ::DyldSharedCache::MappingInfo[Header.MappingCount]);

        if (Header.MappingOffset < sizeof(::DyldSharedCache::HeaderV0)) {
            close(Fd);
            SubCache.Error = Error(OpenError::MappingsOutOfBounds);

            return SubCache.Error;
        }

        if (Header.MappingCount == 0) {
            close(Fd);
            SubCache.Error = Error(OpenError::NoMappings);

            return SubCache.Error;
        }

        if (::lseek(Fd, Header.MappingOffset, SEEK_SET) < 0) {
            close(Fd);
            SubCache.Error = Error(OpenError::MappingsOutOfBounds);

            return SubCache.Error;
        }

        const auto MappingSpan =
            std::span(MappingList.get(), Header.MappingCount);

        const auto ReadSize = MappingSpan.size() * sizeof(MappingSpan.front());
        if (::read(Fd, MappingList.get(), ReadSize) <
                static_cast<ssize_t>(ReadSize))
        {
            close(Fd);
            SubCache.Error = Error(OpenError::MappingsOutOfBounds);

            return SubCache.Error;
        }

        if (const auto Err = VerifyMappingList(MappingSpan);
            Err != DyldSharedCache::OpenError::None)
        {
            close(Fd);
            SubCache.Error = Error(Err);

            return SubCache.Error;
        }

        SubCache.Path = Path;
        if (SubCache.Info.MaxVmSize == 0) {
            const auto MaxVmOffset =
                MappingSpan.back().addressRange().end().value();

            if (MaxVmOffset > SubCache.Info.VmOffset) {
                close(Fd);
                SubCache.Error = Error(OpenError::MappingsOutOfBounds);

                return SubCache.Error;
            }

            SubCache.Info.MaxVmSize = MaxVmOffset - SubCache.Info.VmOffset;
        }

        close(Fd);

        SubCache.Info.FirstAddr = MappingSpan.front().addressRange().front();
        SubCache.Prot = Prot;
        SubCache.Error = Error(OpenError::None);

        return SubCache.Error;
    }

    auto
    DyldSharedCache::CollectSubCaches(
        const ADT::FileMap::Prot Prot,
        const SubCacheProvidedPathMap &SubCacheProvidedPathMap) noexcept
            -> Error
    {
        if (const auto ListOpt = this->subCacheEntryInfoList()) {
            const auto &List = ListOpt.value();
            for (const auto &SubCacheEntry : List) {
                auto MaxVmSize = uint64_t();
                if (&SubCacheEntry != &List.back()) {
                    const auto NextVmOffset = (&SubCacheEntry)[1].CacheVMOffset;
                    if (NextVmOffset <= SubCacheEntry.CacheVMOffset) {
                        return Error(OpenError::SubCacheListIsInvalid);
                    }

                    MaxVmSize = NextVmOffset - SubCacheEntry.CacheVMOffset;
                } else {
                    MaxVmSize =
                        this->mappingInfoList()
                            .back()
                            .addressRange()
                            .end()
                            .value() -
                        SubCacheEntry.CacheVMOffset;
                }

                auto SubCachePath = std::string();
                auto ReturnOnSubCacheError = false;

                if (!SubCacheProvidedPathMap.contains(SubCacheEntry.FileSuffix))
                {
                    SubCachePath =
                        std::format("{}{}",
                                    Path.c_str(),
                                    SubCacheEntry.fileSuffix());
                } else {
                    const auto &Info =
                        SubCacheProvidedPathMap.at(SubCacheEntry.FileSuffix);

                    SubCachePath = Info.Path;
                    ReturnOnSubCacheError = Info.ReturnOnSubCacheError;
                }

                auto Info =
                    SubCacheInfo(
                        SubCachePath,
                        SingleCacheInfo(SubCacheEntry.CacheVMOffset,
                                        MaxVmSize));

                if (const auto Error =
                        FillSubCacheInfo(Info, SubCachePath, Prot);
                    Error.Kind != OpenError::None)
                {
                    if (ReturnOnSubCacheError) {
                        return Error;
                    }
                }

                SubCacheList.emplace(SubCacheEntry.fileSuffix(),
                                     std::move(Info));
            }
        } else if (const auto ListOpt = this->subCacheEntryV1InfoList()) {
            auto I = static_cast<uint32_t>(1);
            const auto &List = ListOpt.value();

            for (const auto &SubCacheEntry : List) {
                auto MaxVmSize = uint64_t();
                if (&SubCacheEntry != &List.back()) {
                    const auto NextVmOffset = (&SubCacheEntry)[1].CacheVMOffset;
                    if (NextVmOffset <= SubCacheEntry.CacheVMOffset) {
                        return Error(OpenError::SubCacheListIsInvalid);
                    }

                    MaxVmSize = NextVmOffset - SubCacheEntry.CacheVMOffset;
                } else {
                    MaxVmSize =
                        this->mappingInfoList()
                            .back()
                            .addressRange()
                            .end()
                            .value() -
                        SubCacheEntry.CacheVMOffset;
                }

                const auto FileSuffix = std::format("{}", I);
                auto SubCachePath = std::string();

                if (!SubCacheProvidedPathMap.contains(FileSuffix)) {
                    SubCachePath =
                        std::filesystem::absolute(
                            std::format("{}{}",
                                        Path.filename().c_str(),
                                        FileSuffix));
                } else {
                    SubCachePath =
                        SubCacheProvidedPathMap.at(FileSuffix)
                            .Path;
                }

                const auto FileMapOrErr =
                    ADT::FileMap::Open(SubCachePath.c_str(), Prot);

                if (!FileMapOrErr.has_value()) {
                    return Error(OpenError::FailedToOpenSubCaches);
                }

                auto FileMap = FileMapOrErr.value();
                const auto Map = FileMap->map();

                if (const auto Error = VerifySubCacheMap(Map);
                    Error.Kind != OpenError::None)
                {
                    return Error;
                }

                auto Info =
                    SingleCacheInfo(Map,
                                    SubCacheEntry.CacheVMOffset,
                                    MaxVmSize);

                auto SubCache =
                    SubCacheInfo(FileMap,
                                 std::move(SubCachePath),
                                 std::move(Info));

                SubCacheList.emplace(FileSuffix, std::move(SubCache));
                I++;
            }
        }

        return Error(OpenError::None);
    }

    auto
    DyldSharedCache::Open(
        const ADT::MemoryMap &Map,
        const std::filesystem::path &Path,
        const ADT::FileMap::Prot SubCacheProt,
        const SubCacheProvidedPathMap &SubCacheProvidedPathMap) noexcept
            -> std::expected<DyldSharedCache *, Error>
    {
        auto CpuKind = CpuKind::i386;
        if (const auto Err = VerifyMap(Map, CpuKind);
            Err != OpenError::None)
        {
            return std::unexpected(Error(Err));
        }

        const auto Result = new DyldSharedCache(Map, CpuKind, Path);
        if (const auto Error =
                Result->CollectSubCaches(SubCacheProt, SubCacheProvidedPathMap);
            Error.Kind != OpenError::None)
        {
            delete Result;
            return std::unexpected(Error);
        }

        return Result;
    }

    auto DyldSharedCache::subCacheEntryV1InfoList() const noexcept
        -> std::optional<std::span<::DyldSharedCache::SubCacheEntryV1>>
    {
        if (!this->header().hasSubCacheV1Array()) {
            return std::nullopt;
        }

        const auto &Header = this->headerV8();
        const auto Array =
            this->map().get<::DyldSharedCache::SubCacheEntryV1>(
                Header.SubCacheArrayOffset, Header.SubCacheArrayCount);

        if (Array == nullptr) {
            return std::nullopt;
        }

        return std::span(Array, Header.SubCacheArrayCount);
    }

    auto DyldSharedCache::subCacheEntryInfoList() const noexcept
        -> std::optional<std::span<::DyldSharedCache::SubCacheEntry>>
    {
        if (!this->header().hasSubCacheArray()) {
            return std::nullopt;
        }

        const auto &Header = this->headerV9();
        const auto Array =
            this->map().get<::DyldSharedCache::SubCacheEntry>(
                Header.SubCacheArrayOffset, Header.SubCacheArrayCount);

        if (Array == nullptr) {
            return std::nullopt;
        }

        return std::span(Array, Header.SubCacheArrayCount);
    }
}