/*
 * MachO/DeVirtualizer.h
 * © suhas pai
 */

#pragma once

#include "ADT/DeVirtualizer.h"
#include "ADT/MemoryMap.h"

#include "MachO/SegmentList.h"

namespace MachO {
    struct DeVirtualizer : public ADT::DeVirtualizer {
    protected:
        ADT::MemoryMap Map;
        MachO::SegmentList SegmentList;
    public:
        explicit
        DeVirtualizer(const ADT::MemoryMap Map,
                      MachO::SegmentList &SegmentList) noexcept
        : Map(Map), SegmentList(SegmentList) {}

        [[nodiscard]] constexpr auto map() const noexcept {
            return this->Map;
        }

        [[nodiscard]] constexpr auto &segmentList() const noexcept {
            return this->SegmentList;
        }

        [[nodiscard]] uint64_t getBaseAddress() const noexcept override;

        [[nodiscard]] void *
        getPtrForAddress(uint64_t Address,
                         bool IgnoreProtBounds = false,
                         void **EndOut = nullptr) const noexcept override;

        [[nodiscard]] std::optional<ADT::MemoryMap>
        getMapForVmRange(const ADT::Range Range,
                         bool IgnoreProtBounds = false) const noexcept override;
    };
}