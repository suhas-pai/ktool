//
//  include/ADT/DscImage/SegmentUtil.h
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "ADT/Mach-O/SegmentUtil.h"

namespace DscImage {
    using SegmentInfo = MachO::SegmentInfo;
    using SectionInfo = MachO::SectionInfo;

    struct SegmentInfoCollection : public MachO::SegmentInfoCollection {
    protected:
        uint64_t ImageAddress;
    public:
        [[nodiscard]] static SegmentInfoCollection
        Open(const MachO::ConstLoadCommandStorage &LoadCmdStorage,
             uint64_t ImageAddress,
             bool Is64Bit,
             Error *ErrorOut) noexcept;

        [[nodiscard]] inline
        uint64_t getFullAddressFromRelative(uint64_t Relative) const noexcept {
            return ImageAddress + Relative;
        }

        [[nodiscard]] const SegmentInfo *
        FindSegmentWithRelativeAddress(
            uint64_t Address) const noexcept override
        {
            const auto FullAddr = ImageAddress + Address;
            return FindSegmentContainingAddress(FullAddr);
        }

        [[nodiscard]] const SectionInfo *
        FindSectionWithRelativeAddress(
            uint64_t Address,
            const SegmentInfo **SegmentOut = nullptr) const noexcept override
        {
            const auto FullAddr = ImageAddress + Address;
            if (const auto Segment = FindSegmentContainingAddress(FullAddr)) {
                if (SegmentOut != nullptr) {
                    *SegmentOut = Segment;
                }
                
                return Segment->FindSectionContainingAddress(FullAddr);
            }

            return nullptr;
        }
    };
}
