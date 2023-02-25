/*
 * Objects/DyldSharedCache/CacheInfo.h
 * © suhas pai
 */

#pragma once

#include "ADT/MemoryMap.h"
#include "DyldSharedCache/Headers.h"

namespace Objects {
    struct DyldSharedSingleCacheInfo {
        friend struct DyldSharedCache;
    protected:
        ADT::MemoryMap Map;
        uint64_t VmOffset;

        constexpr DyldSharedSingleCacheInfo() noexcept = default;
        constexpr
        DyldSharedSingleCacheInfo(const ADT::MemoryMap &Map,
                                  uint64_t VmOffset) noexcept
        : Map(Map), VmOffset(VmOffset) {}
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
        inline auto headerV0() const noexcept -> ::DyldSharedCache::HeaderV0 & {
            return header();
        }

        [[nodiscard]]
        inline auto headerV1() const noexcept -> ::DyldSharedCache::HeaderV1 & {
            assert(isV1());
            return header();
        }

        [[nodiscard]]
        inline auto headerV2() const noexcept -> ::DyldSharedCache::HeaderV2 & {
            assert(isV2());
            return header();
        }

        [[nodiscard]]
        inline auto headerV3() const noexcept -> ::DyldSharedCache::HeaderV3 & {
            assert(isV3());
            return header();
        }

        [[nodiscard]]
        inline auto headerV4() const noexcept -> ::DyldSharedCache::HeaderV4 & {
            assert(isV4());
            return header();
        }

        [[nodiscard]]
        inline auto headerV5() const noexcept -> ::DyldSharedCache::HeaderV5 & {
            assert(isV5());
            return header();
        }

        [[nodiscard]]
        inline auto headerV6() const noexcept -> ::DyldSharedCache::HeaderV6 & {
            assert(isV6());
            return header();
        }

        [[nodiscard]]
        inline auto headerV7() const noexcept -> ::DyldSharedCache::HeaderV7 & {
            assert(isV7());
            return header();
        }

        [[nodiscard]]
        inline auto headerV8() const noexcept -> ::DyldSharedCache::HeaderV8 & {
            assert(isV8());
            return header();
        }

        [[nodiscard]]
        inline auto headerV9() const noexcept -> ::DyldSharedCache::HeaderV9 & {
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

                return map().get<T>(Offset.value());
            }

            return nullptr;
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getMapForAddrRange(const ADT::Range &AddrRange,
                           const bool InsideMappings = true) const noexcept
            -> std::optional<
                    std::pair<DyldSharedSingleCacheInfo, ADT::MemoryMap>>
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
}