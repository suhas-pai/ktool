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
        uint64_t MaxVmSize;
        uint64_t FirstAddr;

        constexpr
        explicit DyldSharedSingleCacheInfo(const uint64_t VmOffset,
                                           const uint64_t MaxVmSize) noexcept
        : VmOffset(VmOffset), MaxVmSize(MaxVmSize) {}

        constexpr
        DyldSharedSingleCacheInfo(const ADT::MemoryMap &Map,
                                  const uint64_t VmOffset,
                                  const uint64_t MaxVmSize) noexcept
        : Map(Map), VmOffset(VmOffset), MaxVmSize(MaxVmSize) {}

        [[nodiscard]] constexpr auto maxVmRange() const noexcept {
            return ADT::Range::FromSize(VmOffset, MaxVmSize);
        }
    public:
        [[nodiscard]] constexpr auto map() const noexcept {
            return Map;
        }

        [[nodiscard]] constexpr auto range() const noexcept {
            return ADT::Range::FromSize(0, this->map().size());
        }

        [[nodiscard]] inline auto &header() const noexcept {
            return *this->map().base<::DyldSharedCache::HeaderV9, false>();
        }

        [[nodiscard]] inline auto isAtleastV1() const noexcept {
            return this->header().isAtleastV1();
        }

        [[nodiscard]] inline auto isAtleastV2() const noexcept {
            return this->header().isAtleastV2();
        }

        [[nodiscard]] inline auto isAtleastV3() const noexcept {
            return this->header().isAtleastV3();
        }

        [[nodiscard]] inline auto isAtleastV4() const noexcept {
            return this->header().isAtleastV4();
        }

        [[nodiscard]] inline auto isAtleastV5() const noexcept {
            return this->header().isAtleastV5();
        }

        [[nodiscard]] inline auto isAtleastV6() const noexcept {
            return this->header().isAtleastV6();
        }

        [[nodiscard]] inline auto isAtleastV7() const noexcept {
            return this->header().isAtleastV7();
        }

        [[nodiscard]] inline auto isAtleastV8() const noexcept {
            return this->header().isAtleastV8();
        }

        [[nodiscard]] inline auto isAtleastV9() const noexcept {
            return this->header().isAtleastV9();
        }

        [[nodiscard]] inline auto getVersion() const noexcept {
            return this->header().getVersion();
        }

        [[nodiscard]]
        inline auto headerV0() const noexcept -> ::DyldSharedCache::HeaderV0 & {
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV1() const noexcept -> ::DyldSharedCache::HeaderV1 & {
            assert(this->isAtleastV1());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV2() const noexcept -> ::DyldSharedCache::HeaderV2 & {
            assert(this->isAtleastV2());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV3() const noexcept -> ::DyldSharedCache::HeaderV3 & {
            assert(this->isAtleastV3());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV4() const noexcept -> ::DyldSharedCache::HeaderV4 & {
            assert(this->isAtleastV4());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV5() const noexcept -> ::DyldSharedCache::HeaderV5 & {
            assert(this->isAtleastV5());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV6() const noexcept -> ::DyldSharedCache::HeaderV6 & {
            assert(this->isAtleastV6());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV7() const noexcept -> ::DyldSharedCache::HeaderV7 & {
            assert(this->isAtleastV7());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV8() const noexcept -> ::DyldSharedCache::HeaderV8 & {
            assert(this->isAtleastV8());
            return this->header();
        }

        [[nodiscard]]
        inline auto headerV9() const noexcept -> ::DyldSharedCache::HeaderV9 & {
            assert(this->isAtleastV9());
            return this->header();
        }

        [[nodiscard]] inline auto mappingCount() const noexcept {
            return this->header().MappingCount;
        }

        [[nodiscard]] inline auto imageCount() const noexcept {
            return this->header().imageCount();
        }

        [[nodiscard]] inline auto mappingInfoList() const noexcept {
            const auto Ptr =
                this->map().get<const ::DyldSharedCache::MappingInfo>(
                    this->header().MappingOffset);
            return std::span<const ::DyldSharedCache::MappingInfo>(
                Ptr, this->header().MappingCount);
        }

        [[nodiscard]] inline auto imageInfoList() const noexcept {
            const auto Ptr =
                this->map().get<const ::DyldSharedCache::ImageInfo>(
                    this->header().imageOffset());
            return std::span<const ::DyldSharedCache::ImageInfo>(
                Ptr, this->header().imageCount());
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
                for (const auto &Mapping : this->mappingInfoList()) {
                    if (const auto Offset =
                            Mapping.getFileOffsetFromAddr(Addr, MaxSizeOut))
                    {
                        return Offset.value();
                    }
                }
            } else {
                const auto FirstMappingAddress =
                    this->mappingInfoList().front().Address;

                if (Addr < FirstMappingAddress) {
                    return std::nullopt;
                }

                const auto Offset = Addr - FirstMappingAddress;
                const auto DscSize = this->map().range().size();

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
            const uint64_t BaseAddr = AddrRange.front();
            if (AddrRange.empty() || BaseAddr == 0) {
                return std::nullopt;
            }

            if (InsideMappings) {
                for (const auto &Mapping : this->mappingInfoList()) {
                    uint64_t MaxSize = uint64_t();
                    if (const auto Offset =
                            Mapping.getFileOffsetFromAddr(BaseAddr, &MaxSize))
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
                    this->mappingInfoList().front().Address;

                if (BaseAddr < FirstMappingAddress) {
                    return std::nullopt;
                }

                const auto Offset = BaseAddr - FirstMappingAddress;
                const auto DscSize = this->map().range().size();

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
            uint64_t *const FileOffsetOut = nullptr) const noexcept -> T *
        {
            auto AvailSize = uint64_t();
            if (const auto Offset =
                    this->getFileOffsetForAddress(Address,
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

                return this->map().get<T>(Offset.value());
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
                this->getFileRangeForAddrRange(AddrRange, InsideMappings);

            if (!FileRangeOpt.has_value()) {
                return std::nullopt;
            }

            const auto Map = ADT::MemoryMap(this->map(), FileRangeOpt.value());
            return std::make_pair(*this, Map);
        }
    };
}