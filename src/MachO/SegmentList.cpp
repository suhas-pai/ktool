/*
 * MachO/SegmentList.cpp
 * © suhas pai
 */

#include "MachO/SegmentList.h"

namespace MachO {
    SegmentList::SegmentList(const MachO::LoadCommandsMap &Map,
                             const bool Is64Bit) noexcept
    {
        const auto IsBigEndian = Map.isBigEndian();
        if (Is64Bit) {
            for (const auto &LC : Map) {
                if (const auto Segment =
                        dyn_cast<MachO::SegmentCommand64>(&LC, IsBigEndian))
                {
                    addSegment(*Segment, IsBigEndian);
                }
            }
        } else {
            for (const auto &LC : Map) {
                if (const auto Segment =
                        dyn_cast<MachO::SegmentCommand>(&LC, IsBigEndian))
                {
                    addSegment(*Segment, IsBigEndian);
                }
            }
        }
    }
}