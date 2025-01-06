/*
 * MachO/DeVirtualizer.cpp
 * © suhas pai
 */

#include "MachO/DeVirtualizer.h"

namespace MachO {
    [[nodiscard]] uint64_t DeVirtualizer::getBaseAddress() const noexcept {
        const auto Segment = this->segmentList().findSegmentWithName("__TEXT");
        if (Segment == nullptr) {
            // FIXME: This is an invalid state
            return 0;
        }

        return Segment->VmRange.front();
    }

    auto
    DeVirtualizer::getPtrForAddress(const uint64_t VmAddr,
                                    const bool IgnoreSectionBounds,
                                    void **const EndOut) const noexcept
        -> void *
    {
        if (const auto Segment =
                this->segmentList().findSegmentWithVmAddr(VmAddr))
        {
            auto FileRange = ADT::Range();
            auto VmRange = ADT::Range();

            if (!IgnoreSectionBounds) {
                const auto Section = Segment->findSectionWithVmAddr(VmAddr);
                if (Section == nullptr) {
                    return nullptr;
                }

                FileRange = Section->fileRange();
                VmRange = Section->vmRange();
            } else {
                FileRange = Segment->FileRange;
                VmRange = Segment->VmRange;
            }

            const auto VmIndex = VmRange.indexForLoc(VmAddr);
            if (!FileRange.hasIndex(VmIndex)) {
                return nullptr;
            }

            if (EndOut != nullptr) {
                *EndOut = this->Map.get(FileRange.end().value());
            }

            return this->Map.get(FileRange.locForIndex(VmIndex));
        }

        return nullptr;
    }

    auto
    DeVirtualizer::getMapForVmRange(
        const ADT::Range Range,
        const bool IgnoreSectionBounds) const noexcept
            -> std::optional<ADT::MemoryMap>
    {
        if (const auto Segment =
                this->segmentList().findSegmentWithVmRange(Range))
        {
            auto FileRange = ADT::Range();
            auto VmRange = ADT::Range();

            if (!IgnoreSectionBounds) {
                const auto Section =
                    Segment->findSectionContainingVmRange(Range);

                if (Section == nullptr) {
                    return std::nullopt;
                }

                FileRange = Section->fileRange();
                VmRange = Section->vmRange();
            } else {
                FileRange = Segment->FileRange;
                VmRange = Segment->VmRange;
            }

            const auto IndexRange = VmRange.indexForLocRange(Range);
            if (!FileRange.containsAsIndex(IndexRange)) {
                return std::nullopt;
            }

            return ADT::MemoryMap(Map, FileRange.locForIndexRange(IndexRange));
        }

        return std::nullopt;
    }
}