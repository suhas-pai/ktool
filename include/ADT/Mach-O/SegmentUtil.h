//
//  include/ADT/Mach-O/SegmentUtil.h
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/MemoryMap.h"
#include "LoadCommandStorage.h"
#include "SegmentInfo.h"

namespace MachO {
    enum class SegmentInfoCollectionError {
        None,

        InvalidSegment,
        OverlappingSegments,

        InvalidSection,
        OverlappingSections
    };

    // The list is still collected, but the (last) error encountered is also
    // given.

    typedef std::vector<SegmentInfo> SegmentInfoCollection;
    struct SegmentInfoCollectionResult {
        SegmentInfoCollection Collection;
        SegmentInfoCollectionError Error;
    };

    SegmentInfoCollectionResult
    CollectSegmentInfoList(const MachO::ConstLoadCommandStorage &LoadCmdStorage,
                           bool Is64Bit) noexcept;

    const SegmentInfo *
    FindSegmentWithName(const std::vector<SegmentInfo> &SegInfoList,
                        const std::string_view &Name) noexcept;

    const SectionInfo *
    FindSectionWithName(const std::vector<SegmentInfo> &SegInfoList,
                        const std::string_view &SegmentName,
                        const std::string_view &Name) noexcept;

    struct SectionNamePair {
        std::string_view SegmentName;
        std::string_view SectionName;
    };

    const SectionInfo *
    FindSectionWithName(
        const std::vector<SegmentInfo> &SegInfoList,
        const std::initializer_list<SectionNamePair> &SectNameList) noexcept;

    uint8_t *
    GetDataForSection(uint8_t *Map, const SectionInfo *Info) noexcept;

    const uint8_t *
    GetDataForSection(const uint8_t *Map, const SectionInfo *Info) noexcept;

    uint8_t *
    GetDataForVirtualAddr(const SegmentInfoCollection &Collection,
                          uint8_t *Map,
                          uint64_t Addr,
                          uint8_t **EndOut = nullptr) noexcept;

    const uint8_t *
    GetDataForVirtualAddr(const SegmentInfoCollection &Collection,
                          const uint8_t *Map,
                          uint64_t Addr,
                          const uint8_t **EndOut = nullptr) noexcept;

    template <typename T>
    T *GetPtrForVirtualAddr(const SegmentInfoCollection &Collection,
                            uint8_t *Map,
                            uint64_t Addr,
                            T **EndOut = nullptr) noexcept
    {
        const auto Data =
            GetDataForVirtualAddr(Collection,
                                  Map,
                                  Addr,
                                  reinterpret_cast<uint8_t **>(EndOut));

        return reinterpret_cast<T *>(Data);
    }

    template <typename T>
    const T *GetPtrForVirtualAddr(const SegmentInfoCollection &Collection,
                                  const uint8_t *Map,
                                  uint64_t Addr,
                                  const T **EndOut = nullptr) noexcept
    {
        const auto Data =
            GetDataForVirtualAddr(Collection,
                                  Map,
                                  Addr,
                                  reinterpret_cast<const uint8_t **>(EndOut));

        return reinterpret_cast<const T *>(Data);
    }
}
