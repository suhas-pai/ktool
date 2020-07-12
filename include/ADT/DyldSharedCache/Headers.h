//
//  include/ADT/DyldSharedCacheHeaders.h
//  ktool
//
//  Created by Suhas Pai on 7/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string_view>
#include <cstdint>

#include "ADT/BasicContiguousList.h"

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
    };

    struct ImageInfo {
        uint64_t Address;
        uint64_t ModTime;
        uint64_t Inode;
        uint32_t PathFileOffset;
        uint32_t Pad;
    };

    struct ImageInfoExtra {
        uint64_t ExportsTrieAddr;
        uint64_t WeakBindingsAddr;
        uint32_t ExportsTrieSize;
        uint32_t WeakBindingsSize;
        uint32_t DependentsStartArrayIndex;
        uint32_t ReExportsStartArrayIndex;
    };

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

        [[nodiscard]] inline
        BasicContiguousList<ImageInfo> getImageInfoList() noexcept {
            const auto Ptr = reinterpret_cast<uint8_t *>(this) + ImagesOffset;
            return BasicContiguousList<ImageInfo>(Ptr, ImagesCount);
        }

        [[nodiscard]] inline
        BasicContiguousList<const ImageInfo> getImageInfoList() const noexcept {
            const auto Ptr =
                reinterpret_cast<const uint8_t *>(this) + ImagesOffset;

            return BasicContiguousList<const ImageInfo>(Ptr, ImagesCount);
        }

        [[nodiscard]]
        inline BasicContiguousList<MappingInfo> getMappingInfoList() noexcept {
            const auto Ptr = reinterpret_cast<uint8_t *>(this) + MappingOffset;
            return BasicContiguousList<MappingInfo>(Ptr, MappingCount);
        }

        [[nodiscard]] inline BasicContiguousList<const MappingInfo>
        getMappingInfoList() const noexcept {
            const auto Ptr =
                reinterpret_cast<const uint8_t *>(this) + MappingOffset;

            return BasicContiguousList<const MappingInfo>(Ptr, MappingCount);
        }
    };

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
