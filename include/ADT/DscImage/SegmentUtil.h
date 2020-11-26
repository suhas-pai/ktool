//
//  include/ADT/DscImage/SegmentUtil.h
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/DyldSharedCache/Headers.h"
#include "ADT/Mach-O/SegmentUtil.h"

namespace DscImage {
    using SegmentInfo = MachO::SegmentInfo;
    using SectionInfo = MachO::SectionInfo;

    struct SegmentInfoCollection : public MachO::SegmentInfoCollection {
    protected:
        uint64_t ImageAddress;
    public:
        [[nodiscard]] static SegmentInfoCollection
        Open(uint64_t ImageAddress,
             const MachO::ConstLoadCommandStorage &LoadCmdStorage,
             bool Is64Bit,
             Error *ErrorOut) noexcept;

        [[nodiscard]] inline
        uint64_t getFullAddressFromRelative(uint64_t Relative) const noexcept {
            return ImageAddress + Relative;
        }

        [[nodiscard]] inline const SegmentInfo *
        FindSegmentWithImageRelativeAddress(uint64_t Address) const noexcept {
            const auto FullAddr = getFullAddressFromRelative(Address);
            return FindSegmentContainingAddress(FullAddr);
        }

        [[nodiscard]] inline const SectionInfo *
        FindSectionWithImageRelativeAddress(
            uint64_t Address,
            const SegmentInfo **SegmentOut = nullptr) const noexcept
        {
            const auto FullAddr = getFullAddressFromRelative(Address);
            if (const auto Segment = FindSegmentContainingAddress(FullAddr)) {
                *SegmentOut = Segment;
                return Segment->FindSectionContainingAddress(FullAddr);
            }

            return nullptr;
        }
    };
}
