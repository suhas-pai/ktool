/*
 * Objects/DyldSharedCache.h
 * © suhas pai
 */

#pragma once

#include <unordered_map>
#include "DyldSharedCache/Headers.h"

#include "Objects/Base.h"
#include "Objects/DscImage.h"

namespace Objects {
    struct DyldSharedCache : public Base {
    public:
        enum class CpuKind {
            i386,
            x86_64,
            x86_64h,
            armv5,
            armv6,
            armv7,
            armv7f,
            armv7k,
            armv7s,
            arm64,
            arm64e,
            arm64_32
        };

        enum class OpenError {
            None,

            WrongFormat,
            UnrecognizedCpuKind,
            SizeTooSmall,

            NoMappings,
            MappingsOutOfBounds,

            FirstMappingFileOffNotZero,
            FailedToOpenSubCaches,

            SubCacheHasDiffCpuKind,
            SubCacheHasDiffVersion,

            RecursiveSubCache,
        };

        struct CacheInfo {
            friend struct DyldSharedCache;
        protected:
            ADT::MemoryMap Map;

            constexpr CacheInfo() noexcept = default;
            constexpr CacheInfo(const ADT::MemoryMap &Map) noexcept
            : Map(Map) {}
        public:
            [[nodiscard]] constexpr auto map() const noexcept {
                return Map;
            }

            [[nodiscard]] constexpr auto range() const noexcept {
                return ADT::Range::FromSize(0, map().size());
            }

            [[nodiscard]] inline auto &header() const noexcept {
                return *Map.base<::DyldSharedCache::HeaderV9, false>();
            }

            [[nodiscard]] inline auto isV1() const noexcept {
                return header().isV1();
            }

            [[nodiscard]] inline auto isV2() const noexcept {
                return header().isV2();
            }

            [[nodiscard]] inline auto isV3() const noexcept {
                return header().isV3();
            }

            [[nodiscard]] inline auto isV4() const noexcept {
                return header().isV4();
            }

            [[nodiscard]] inline auto isV5() const noexcept {
                return header().isV5();
            }

            [[nodiscard]] inline auto isV6() const noexcept {
                return header().isV6();
            }

            [[nodiscard]] inline auto isV7() const noexcept {
                return header().isV7();
            }

            [[nodiscard]] inline auto isV8() const noexcept {
                return header().isV8();
            }

            [[nodiscard]] inline auto isV9() const noexcept {
                return header().isV9();
            }

            [[nodiscard]] inline auto getVersion() const noexcept {
                return header().getVersion();
            }

            [[nodiscard]]
            inline auto headerV0() const noexcept
                -> ::DyldSharedCache::HeaderV0 &
            {
                return header();
            }

            [[nodiscard]]
            inline auto headerV1() const noexcept
                -> ::DyldSharedCache::HeaderV1 &
            {
                assert(isV1());
                return header();
            }

            [[nodiscard]]
            inline auto headerV2() const noexcept
                -> ::DyldSharedCache::HeaderV2 &
            {
                assert(isV2());
                return header();
            }

            [[nodiscard]]
            inline auto headerV3() const noexcept
                -> ::DyldSharedCache::HeaderV3 &
            {
                assert(isV3());
                return header();
            }

            [[nodiscard]]
            inline auto headerV4() const noexcept
                -> ::DyldSharedCache::HeaderV4 &
            {
                assert(isV4());
                return header();
            }

            [[nodiscard]]
            inline auto headerV5() const noexcept
                -> ::DyldSharedCache::HeaderV5 &
            {
                assert(isV5());
                return header();
            }

            [[nodiscard]]
            inline auto headerV6() const noexcept
                -> ::DyldSharedCache::HeaderV6 &
            {
                assert(isV6());
                return header();
            }

            [[nodiscard]]
            inline auto headerV7() const noexcept
                -> ::DyldSharedCache::HeaderV7 &
            {
                assert(isV7());
                return header();
            }

            [[nodiscard]]
            inline auto headerV8() const noexcept
                -> ::DyldSharedCache::HeaderV8 &
            {
                assert(isV8());
                return header();
            }

            [[nodiscard]]
            inline auto headerV9() const noexcept
                -> ::DyldSharedCache::HeaderV9 &
            {
                assert(isV9());
                return header();
            }

            [[nodiscard]] inline auto mappingCount() const noexcept {
                return header().MappingCount;
            }

            [[nodiscard]] inline auto imageCount() const noexcept {
                return header().imageCount();
            }

            [[nodiscard]] inline auto mappingInfoList() const noexcept {
                const auto Ptr =
                    map().get<const ::DyldSharedCache::MappingInfo>(
                        header().MappingOffset);
                return ADT::List<const ::DyldSharedCache::MappingInfo>(
                    Ptr, header().MappingCount);
            }

            [[nodiscard]] inline auto imageInfoList() const noexcept {
                const auto Ptr =
                    map().get<const ::DyldSharedCache::ImageInfo>(
                        header().imageOffset());

                return ADT::List<const ::DyldSharedCache::ImageInfo>(
                    Ptr, header().imageCount());
            }


            [[nodiscard]] inline auto
            getFileOffsetForAddress(
                const uint64_t Addr,
                const bool InsideMappings = true,
                uint64_t *const MaxSizeOut = nullptr) const noexcept
                    -> std::optional<uint64_t>
            {
                if (Addr == 0) {
                    return std::nullopt;
                }

                if (InsideMappings) {
                    for (const auto &Mapping : mappingInfoList()) {
                        if (const auto Offset =
                                Mapping.getFileOffsetFromAddr(Addr, MaxSizeOut))
                        {
                            return Offset.value();
                        }
                    }
                } else {
                    const auto FirstMappingAddress =
                        mappingInfoList().front().Address;

                    if (Addr < FirstMappingAddress) {
                        return std::nullopt;
                    }

                    const auto Offset = Addr - FirstMappingAddress;
                    const auto DscSize = map().range().size();

                    if (Offset >= DscSize) {
                        return std::nullopt;
                    }

                    if (MaxSizeOut != nullptr) {
                        *MaxSizeOut = DscSize - Offset;
                    }

                    return Offset;
                }

                return std::nullopt;
            }

            [[nodiscard]] inline auto
            getFileRangeForAddrRange(
                const ADT::Range &AddrRange,
                const bool InsideMappings = true) const noexcept
                    -> std::optional<ADT::Range>
            {
                const uint64_t BaseAddr = AddrRange.begin();
                if (AddrRange.empty() || BaseAddr == 0) {
                    return std::nullopt;
                }

                if (InsideMappings) {
                    for (const auto &Mapping : mappingInfoList()) {
                        uint64_t MaxSize = uint64_t();
                        if (const auto Offset =
                                Mapping.getFileOffsetFromAddr(BaseAddr,
                                                              &MaxSize))
                        {
                            const auto AddrRangeSize = AddrRange.size();
                            if (MaxSize < AddrRangeSize) {
                                return std::nullopt;
                            }

                            return ADT::Range::FromSize(Offset.value(),
                                                        AddrRangeSize);
                        }
                    }
                } else {
                    const auto FirstMappingAddress =
                        mappingInfoList().front().Address;

                    if (BaseAddr < FirstMappingAddress) {
                        return std::nullopt;
                    }

                    const auto Offset = BaseAddr - FirstMappingAddress;
                    const auto DscSize = map().range().size();

                    if (Offset >= DscSize) {
                        return std::nullopt;
                    }

                    const auto MaxSize = DscSize - Offset;
                    const auto AddrRangeSize = AddrRange.size();

                    if (AddrRangeSize > MaxSize) {
                        return std::nullopt;
                    }

                    return ADT::Range::FromSize(Offset, AddrRangeSize);
                }

                return std::nullopt;
            }

            template <typename T = uint8_t, uint64_t Size = sizeof(T)>
            [[nodiscard]] inline auto
            getPtrForAddress(
                const uint64_t Address,
                ADT::MemoryMap &MapOut,
                const bool InsideMappings = true,
                uint64_t *const TotalAvailSize = nullptr,
                uint64_t *const FileOffsetOut = nullptr) const noexcept
                    -> T *
            {
                auto AvailSize = uint64_t();
                if (const auto Offset =
                        getFileOffsetForAddress(Address,
                                                InsideMappings,
                                                &AvailSize))
                {
                    if (AvailSize < Size) {
                        return nullptr;
                    }

                    if (TotalAvailSize != nullptr) {
                        *TotalAvailSize = AvailSize;
                    }

                    if (FileOffsetOut != nullptr) {
                        *FileOffsetOut = Offset.value();
                    }

                    MapOut = map();
                    return map().get<T>(Offset.value());
                }

                return nullptr;
            }

            template <typename T = uint8_t, uint64_t Size = sizeof(T)>
            [[nodiscard]] inline auto
            getMapForAddrRange(const ADT::Range &AddrRange,
                               const bool InsideMappings = true) const noexcept
                    -> std::optional<std::pair<CacheInfo, ADT::MemoryMap>>
            {
                const auto FileRangeOpt =
                    getFileRangeForAddrRange(AddrRange, InsideMappings);

                if (!FileRangeOpt.has_value()) {
                    return std::nullopt;
                }

                const auto Map = ADT::MemoryMap(map(), FileRangeOpt.value());
                return std::make_pair(*this, Map);
            }
        };
    protected:
        CacheInfo Info;
        CpuKind CpuKind;

        struct SubCacheInfo {
            ADT::FileMap *Map;
            CacheInfo Info;

            explicit
            SubCacheInfo(ADT::FileMap *const Map,
                         const CacheInfo &Info) noexcept
            : Map(Map), Info(Info) {}
        };

        std::string Path;
        std::unordered_map<std::string_view, SubCacheInfo> SubCacheList;

        auto CollectSubCaches(ADT::FileMap::Prot Prot) noexcept -> OpenError;
        auto VerifySubCacheMap(const ADT::MemoryMap &Map) noexcept -> OpenError;

        explicit
        DyldSharedCache(const ADT::MemoryMap &Map,
                        const enum CpuKind CpuKind,
                        const std::string_view Path) noexcept
        : Base(Kind::DyldSharedCache), Info(Map), CpuKind(CpuKind),
          Path(Path) {}
    public:
        static auto
        Open(const ADT::MemoryMap &Map,
             std::string_view Path,
             ADT::FileMap::Prot SubCacheProt) noexcept
                -> ADT::PointerOrError<DyldSharedCache, OpenError>;

        ~DyldSharedCache() noexcept override {}

        [[nodiscard]] constexpr auto map() const noexcept {
            return Info.map();
        }

        [[nodiscard]] constexpr auto range() const noexcept {
            return Info.range();
        }

        [[nodiscard]] constexpr auto cpuKind() const noexcept {
            return CpuKind;
        }

        [[nodiscard]] constexpr auto getMachCpuKindAndSubKind() const noexcept {
            switch (CpuKind) {
                case CpuKind::i386:
                    return std::pair(
                        Mach::CpuKind::IntelX86,
                        static_cast<int32_t>(Mach::Intelx86::x86));
                case CpuKind::x86_64:
                    return std::pair(
                        Mach::CpuKind::IntelX86_64,
                        static_cast<int32_t>(Mach::Intelx86_64::All));
                case CpuKind::x86_64h:
                    return std::pair(
                        Mach::CpuKind::IntelX86_64,
                        static_cast<int32_t>(Mach::Intelx86_64::Haswell));
                case CpuKind::armv5:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v5tej));
                case CpuKind::armv6:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v6));
                case CpuKind::armv7:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7));
                case CpuKind::armv7f:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7f));
                case CpuKind::armv7k:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7k));
                case CpuKind::armv7s:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7s));
                case CpuKind::arm64:
                    return std::pair(Mach::CpuKind::Arm64,
                                     static_cast<int32_t>(Mach::Arm64::All));
                case CpuKind::arm64e:
                    return std::pair(
                        Mach::CpuKind::Arm64,
                        static_cast<int32_t>(Mach::Arm64::arm64e));
                case CpuKind::arm64_32:
                    return std::pair(
                        Mach::CpuKind::Arm64_32,
                        static_cast<int32_t>(Mach::Arm64_32::All));
            }

            assert(false &&
                   "Objects::getMachCpuKindAndSubKind() couldn't recognize "
                   "internally set CpuKind");
        }

        [[nodiscard]] constexpr auto
        cpuInfoMatches(const Mach::CpuKind CpuKind,
                       const int32_t SubKind) const noexcept
        {
            switch (this->CpuKind) {
                case CpuKind::i386:
                    return CpuKind == Mach::CpuKind::IntelX86 &&
                           SubKind == static_cast<int32_t>(Mach::Intelx86::x86);
                case CpuKind::x86_64:
                    return CpuKind == Mach::CpuKind::IntelX86_64 &&
                           SubKind ==
                                static_cast<int32_t>(Mach::Intelx86_64::All);
                case CpuKind::x86_64h:
                    if (CpuKind == Mach::CpuKind::IntelX86_64 &&
                        SubKind ==
                            static_cast<int32_t>(Mach::Intelx86_64::Haswell))
                    {
                        return true;
                    }

                    if (CpuKind == Mach::CpuKind::IntelX86_64 &&
                        SubKind == static_cast<int32_t>(Mach::Intelx86_64::All))
                    {
                        return true;
                    }

                    return false;
                case CpuKind::armv5:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v5tej);
                case CpuKind::armv6:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v6);
                case CpuKind::armv7:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7);
                case CpuKind::armv7f:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7f);
                case CpuKind::armv7k:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7k);
                case CpuKind::armv7s:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7s);
                case CpuKind::arm64:
                    return CpuKind == Mach::CpuKind::Arm64 &&
                           SubKind == static_cast<int32_t>(Mach::Arm64::All);
                case CpuKind::arm64e:
                    if (CpuKind != Mach::CpuKind::Arm64) {
                        return false;
                    }

                    return
                        SubKind == static_cast<int32_t>(Mach::Arm64::arm64e) ||
                        SubKind == -2147483646;
                case CpuKind::arm64_32:
                    return CpuKind == Mach::CpuKind::Arm64_32 &&
                           SubKind == static_cast<int32_t>(Mach::Arm64_32::All);
            }

            assert(false &&
                   "Objects::getMachCpuKindAndSubKind() couldn't recognize "
                   "internally set CpuKind");
        }

        [[nodiscard]] inline auto &header() const noexcept {
            return Info.header();
        }

        [[nodiscard]] inline auto isV1() const noexcept {
            return Info.isV1();
        }

        [[nodiscard]] inline auto isV2() const noexcept {
            return Info.isV2();
        }

        [[nodiscard]] inline auto isV3() const noexcept {
            return Info.isV3();
        }

        [[nodiscard]] inline auto isV4() const noexcept {
            return Info.isV4();
        }

        [[nodiscard]] inline auto isV5() const noexcept {
            return Info.isV5();
        }

        [[nodiscard]] inline auto isV6() const noexcept {
            return Info.isV6();
        }

        [[nodiscard]] inline auto isV7() const noexcept {
            return Info.isV7();
        }

        [[nodiscard]] inline auto isV8() const noexcept {
            return Info.isV8();
        }

        [[nodiscard]] inline auto isV9() const noexcept {
            return Info.isV9();
        }

        [[nodiscard]] inline auto getVersion() const noexcept {
            return Info.getVersion();
        }

        [[nodiscard]]
        inline auto headerV0() const noexcept -> ::DyldSharedCache::HeaderV0 & {
            return Info.headerV0();
        }

        [[nodiscard]]
        inline auto headerV1() const noexcept -> ::DyldSharedCache::HeaderV1 & {
            return Info.headerV1();
        }

        [[nodiscard]]
        inline auto headerV2() const noexcept -> ::DyldSharedCache::HeaderV2 & {
            return Info.headerV2();
        }

        [[nodiscard]]
        inline auto headerV3() const noexcept -> ::DyldSharedCache::HeaderV3 & {
            return Info.headerV3();
        }

        [[nodiscard]]
        inline auto headerV4() const noexcept -> ::DyldSharedCache::HeaderV4 & {
            return Info.headerV4();
        }

        [[nodiscard]]
        inline auto headerV5() const noexcept -> ::DyldSharedCache::HeaderV5 & {
            return Info.headerV5();
        }

        [[nodiscard]]
        inline auto headerV6() const noexcept -> ::DyldSharedCache::HeaderV6 & {
            return Info.headerV6();
        }

        [[nodiscard]]
        inline auto headerV7() const noexcept -> ::DyldSharedCache::HeaderV7 & {
            return Info.headerV7();
        }

        [[nodiscard]]
        inline auto headerV8() const noexcept -> ::DyldSharedCache::HeaderV8 & {
            return Info.headerV8();
        }

        [[nodiscard]]
        inline auto headerV9() const noexcept -> ::DyldSharedCache::HeaderV9 & {
            return Info.headerV9();
        }

        [[nodiscard]] inline auto mappingCount() const noexcept {
            return Info.mappingCount();
        }

        [[nodiscard]] inline auto imageCount() const noexcept {
            return Info.imageCount();
        }

        [[nodiscard]] inline auto mappingInfoList() const noexcept {
            return Info.mappingInfoList();
        }

        [[nodiscard]] inline auto imageInfoList() const noexcept {
            return Info.imageInfoList();
        }

        [[nodiscard]] inline auto
        getFileOffsetForAddress(
            const uint64_t Addr,
            const bool InsideMappings = true,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
                -> std::optional<uint64_t>
        {
            if (Addr == 0) {
                return std::nullopt;
            }

            if (const auto Result =
                    Info.getFileOffsetForAddress(Addr,
                                                 InsideMappings,
                                                 MaxSizeOut))
            {
                return Result;
            }

            for (const auto &SubCachePair : SubCacheList) {
                const auto &SubCache = SubCachePair.second.Info;
                if (const auto Result =
                        SubCache.getFileOffsetForAddress(Addr,
                                                         InsideMappings,
                                                         MaxSizeOut))
                {
                    return Result;
                }
            }

            return std::nullopt;
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getMapForAddrRange(const ADT::Range &AddrRange,
                           const bool InsideMappings = true) const noexcept
                -> std::optional<std::pair<CacheInfo, ADT::MemoryMap>>
        {
            const uint64_t BaseAddr = AddrRange.begin();
            if (AddrRange.empty() || BaseAddr == 0) {
                return std::nullopt;
            }

            if (const auto Result =
                    Info.getMapForAddrRange(AddrRange, InsideMappings))
            {
                return Result;
            }

            for (const auto &SubCachePair : SubCacheList) {
                const auto &SubCache = SubCachePair.second.Info;
                if (const auto Result =
                        SubCache.getMapForAddrRange(AddrRange, InsideMappings))
                {
                    return Result;
                }
            }

            return std::nullopt;
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getPtrForAddress(const uint64_t Address,
                         ADT::MemoryMap &MapOut,
                         const bool InsideMappings = true,
                         uint64_t *const TotalAvailSize = nullptr,
                         uint64_t *const FileOffsetOut = nullptr) const noexcept
            -> std::optional<std::pair<const CacheInfo &, T *>>
        {
            if (const auto Result =
                    Info.getPtrForAddress<T, Size>(Address,
                                                   MapOut,
                                                   InsideMappings,
                                                   TotalAvailSize,
                                                   FileOffsetOut))
            {
                return std::make_pair(Info, Result);
            }

            for (const auto &SubCachePair : SubCacheList) {
                const auto &SubCache = SubCachePair.second.Info;
                if (const auto Result =
                        SubCache.getPtrForAddress<T, Size>(Address,
                                                           MapOut,
                                                           InsideMappings,
                                                           TotalAvailSize,
                                                           FileOffsetOut))
                {
                    return std::make_pair(SubCache, Result);
                }
            }

            return std::nullopt;
        }

        [[nodiscard]] auto subCacheEntryV1InfoList() const noexcept
            -> std::optional<ADT::List<::DyldSharedCache::SubCacheEntryV1>>;

        [[nodiscard]] auto subCacheEntryInfoList() const noexcept
            -> std::optional<ADT::List<::DyldSharedCache::SubCacheEntry>>;

        [[nodiscard]]
        auto getImageObjectAtIndex(uint32_t Index) const noexcept
            -> Objects::OpenResult;
    };
}