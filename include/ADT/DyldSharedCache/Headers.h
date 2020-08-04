//
//  include/ADT/DyldSharedCacheHeaders.h
//  ktool
//
//  Created by Suhas Pai on 7/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <string_view>

#include "ADT/BasicContiguousList.h"
#include "ADT/LargestIntHelper.h"
#include "ADT/LocationRange.h"

#define DscHeaderHasField(Header, Field) \
    (Header->MappingOffset >= (offsetof(DyldCacheHeader, Field) + \
     SizeOfField(DyldCacheHeader, Field)))

namespace DyldSharedCache {
    struct MappingInfo {
        uint64_t Address;
        uint64_t Size;
        uint64_t FileOffset;
        uint32_t MaxProt;
        uint32_t InitProt;

        [[nodiscard]] inline
        std::optional<LocationRange> getAddressRange() const noexcept {
            return LocationRange::CreateWithSize(Address, Size);
        }

        [[nodiscard]] inline
        std::optional<LocationRange> getFileRange() const noexcept {
            return LocationRange::CreateWithSize(FileOffset, Size);
        }

        [[nodiscard]] inline std::optional<uint64_t>
        getFileOffsetFromAddr(uint64_t Addr,
                              uint64_t *MaxSizeOut = nullptr) const noexcept
        {
            const auto Range = getAddressRange();
            if (!Range || !Range->containsLocation(Addr)) {
                return std::nullopt;
            }

            return getFileOffsetFromAddrUnsafe(Addr, MaxSizeOut);
        }

        [[nodiscard]] inline uint64_t
        getFileOffsetFromAddrUnsafe(
            uint64_t Addr,
            uint64_t *MaxSizeOut = nullptr) const noexcept
        {
            const auto Delta = Addr - this->Address;
            if (MaxSizeOut != nullptr) {
                *MaxSizeOut = (this->Address + this->Size) - Addr;
            }

            return FileOffset + Delta;
        }
    };

    struct ImageInfo {
        uint64_t Address;
        uint64_t ModTime;
        uint64_t Inode;
        uint32_t PathFileOffset;
        uint32_t Pad;

        [[nodiscard]]
        inline const char *getPath(const uint8_t *Map) const noexcept {
            return reinterpret_cast<const char *>(Map) + PathFileOffset;
        }

        [[nodiscard]] inline bool IsAlias(const uint8_t *Map) const noexcept;
    };

    struct ImageInfoExtra {
        uint64_t ExportsTrieAddr;
        uint64_t WeakBindingsAddr;
        uint32_t ExportsTrieSize;
        uint32_t WeakBindingsSize;
        uint32_t DependentsStartArrayIndex;
        uint32_t ReExportsStartArrayIndex;
    };

    struct AcceleratorInfo {
        uint32_t Version;
        uint32_t ImageExtrasCount;
        uint32_t ImageExtrasOffset;
        uint32_t BottomUpListOffset;
        uint32_t DylibTrieOffset;
        uint32_t DylibTrieSize;
        uint32_t InitializersOffset;
        uint32_t InitializersCount;
        uint32_t DofSectionsOffset;
        uint32_t DofSectionsCount;
        uint32_t ReExportListOffset;
        uint32_t ReExportCount;
        uint32_t DepListOffset;
        uint32_t DepListCount;
        uint32_t RangeTableOffset;
        uint32_t RangeTableCount;
        uint64_t DyldSectionAddr;

        [[nodiscard]]
        ImageInfoExtra &getImageInfoExtraAtIndex(uint32_t Index) noexcept {
            auto &Result =
                const_cast<ImageInfoExtra &>(
                    getConstImageInfoExtraAtIndex(Index));

            return Result;
        }

        [[nodiscard]]
        const ImageInfoExtra &
        getImageInfoExtraAtIndex(uint32_t Index) const noexcept {
            return getConstImageInfoExtraAtIndex(Index);
        }

        [[nodiscard]] const ImageInfoExtra &
        getConstImageInfoExtraAtIndex(uint32_t Index) const noexcept {
            assert(!IndexOutOfBounds(Index, ImageExtrasCount));

            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto ImageInfoExtraTable =
                reinterpret_cast<const ImageInfoExtra *>(
                    Map + ImageExtrasOffset);

            return ImageInfoExtraTable[Index];
        }
    };

    using ImageList = BasicContiguousList<ImageInfo>;
    using ConstImageList = BasicContiguousList<const ImageInfo>;

    using MappingList = BasicContiguousList<MappingInfo>;
    using ConstMappingList = BasicContiguousList<const MappingInfo>;

    // Apple doesn't provide versions for their dyld_shared_caches, so we have
    // to make up our own.

    // This is the original header from dyld v95.3

    struct HeaderV0 {
        char Magic[16];

        uint32_t MappingOffset;
        uint32_t MappingCount;

        uint32_t ImagesOffset;
        uint32_t ImagesCount;

        uint64_t DyldBaseAddress;

        [[nodiscard]] inline ImageList getImageInfoList() noexcept {
            const auto Ptr = reinterpret_cast<uint8_t *>(this) + ImagesOffset;
            return BasicContiguousList<ImageInfo>(Ptr, ImagesCount);
        }

        [[nodiscard]] inline ConstImageList getImageInfoList() const noexcept {
            return getConstImageInfoList();
        }

        [[nodiscard]]
        inline ConstImageList getConstImageInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + ImagesOffset;

            return ConstImageList(Ptr, ImagesCount);
        }

        [[nodiscard]] inline MappingList getMappingInfoList() noexcept {
            const auto Ptr = reinterpret_cast<uint8_t *>(this) + MappingOffset;
            return MappingList(Ptr, MappingCount);
        }

        [[nodiscard]]
        inline ConstMappingList getMappingInfoList() const noexcept {
            return getConstMappingInfoList();
        }

        [[nodiscard]]
        inline ConstMappingList getConstMappingInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + MappingOffset;

            return ConstMappingList(Ptr, MappingCount);
        }

        [[nodiscard]] std::optional<uint64_t>
        GetFileOffsetForAddress(uint64_t Addr,
                                uint64_t *MaxSizeOut = nullptr) const noexcept
        {
            if (Addr == 0) {
                return std::nullopt;
            }

            for (const auto &Mapping : getConstMappingInfoList()) {
                const auto Offset =
                    Mapping.getFileOffsetFromAddr(Addr, MaxSizeOut);

                if (Offset.has_value()) {
                    return Offset;
                }
            }

            return std::nullopt;
        }

        template <typename T = uint8_t>
        [[nodiscard]] T *GetPtrForAddress(uint64_t Address) noexcept {
            auto Size = uint64_t();
            if (const auto Offset = GetFileOffsetForAddress(Address, &Size)) {
                if (Size < sizeof(T)) {
                    return nullptr;
                }

                const auto Map = reinterpret_cast<uint8_t *>(this);
                return reinterpret_cast<T *>(Map + Offset.value());
            }

            return nullptr;
        }

        template <typename T = uint8_t>
        [[nodiscard]]
        const T *GetPtrForAddress(uint64_t Address) const noexcept {
            auto Size = uint64_t();
            if (const auto Offset = GetFileOffsetForAddress(Address, &Size)) {
                if (Size < sizeof(T)) {
                    return nullptr;
                }

                const auto Map = reinterpret_cast<const uint8_t *>(this);
                return reinterpret_cast<const T *>(Map + Offset.value());
            }

            return nullptr;
        }

        [[nodiscard]] inline LocationRange getMappingsRange() const noexcept {
            auto End = LargestIntHelper();

            const auto List = getConstMappingInfoList();
            const auto Begin = List.front().Address;

            for (const auto &Mapping : List) {
                End = Mapping.Address + Mapping.Size;
            }

            return LocationRange::CreateWithEnd(Begin, End);
        }
    };

    inline bool ImageInfo::IsAlias(const uint8_t *Map) const noexcept {
        const auto Header = reinterpret_cast<const HeaderV0 *>(Map);
        const auto MappingList = Header->getConstMappingInfoList();
        const auto FirstMappingFileOff = MappingList.front().FileOffset;

        return (PathFileOffset < FirstMappingFileOff);
    }

    // From dyld v195.5

    struct HeaderV1 : public HeaderV0 {
        uint64_t CodeSignatureOffset;
        uint64_t CodeSignatureSize;

        uint64_t SlideInfoOffset;
        uint64_t SlideInfoSize;
    };

    // From dyld v239.3

    struct HeaderV2 : public HeaderV1 {
        uint64_t LocalSymbolsOffset;
        uint64_t LocalSymbolsSize;

        uint8_t Uuid[16];
    };

    // From dyld v360.14

    enum class CacheKind : uint64_t {
        Development,
        Production
    };

    struct HeaderV3 : public HeaderV2 {
        CacheKind Kind;
    };

    /*
     * From dyld v421.1
     */

    struct HeaderV4 : public HeaderV3 {
        uint32_t BranchPoolsOffset;
        uint32_t BranchPoolsCount;

        uint64_t AccelerateInfoAddr;
        uint64_t AccelerateInfoSize;

        uint64_t ImagesTextOffset;
        uint64_t ImagesTextCount;

        [[nodiscard]] AcceleratorInfo *GetAcceleratorInfo() noexcept {
            if (AccelerateInfoAddr == 0 || AccelerateInfoSize == 0) {
                return nullptr;
            }

            return GetPtrForAddress<AcceleratorInfo>(AccelerateInfoAddr);
        }

        [[nodiscard]]
        const AcceleratorInfo *GetAcceleratorInfo() const noexcept {
            if (AccelerateInfoAddr == 0 || AccelerateInfoSize == 0) {
                return nullptr;
            }

            return GetPtrForAddress<AcceleratorInfo>(AccelerateInfoAddr);
        }
    };

    // From dyld v519.2.1

    struct HeaderV5 : public HeaderV4 {
        uint64_t DylibsImageGroupAddr;
        uint64_t DylibsImageGroupSize;

        uint64_t OtherImageGroupAddr;
        uint64_t OtherImageGroupSize;

        uint64_t ProgClosuresAddr;
        uint64_t ProgClosuresSize;

        uint64_t ProgClosuresTrieAddr;
        uint64_t ProgClosuresTrieSize;

        uint32_t Platform;
        uint32_t FormatVersion          : 8,
                 DylibsExpectedOnDisk   : 1,
                 Simulator              : 1,
                 LocallyBuiltCache      : 1, // Only in HeaderV6+
                 BuiltFromChainedFixups : 1, // Only in dyld v732.8 +
                 Padding                : 20;

        uint64_t SharedRegionStart;
        uint64_t SharedRegionSize;

        uint64_t MaxSlide;
    };

    // From dyld v625.13

    struct HeaderV6 : public HeaderV5 {
        uint64_t DylibsImageArrayAddr;
        uint64_t DylibsImageArraySize;

        uint64_t DylibsTrieAddr;
        uint64_t DylibsTrieSize;

        uint64_t OtherImageArrayAddr;
        uint64_t OtherImageArraySize;

        uint64_t OtherTrieAddr;
        uint64_t OtherTrieSize;
    };

    using Header = HeaderV6;
}
