//
//  ADT/DscImage/SegmentUtil.h
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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
        [[nodiscard]] static auto
        Open(uint64_t ImageAddress,
             const MachO::ConstLoadCommandStorage &LoadCmdStorage,
             bool Is64Bit,
             Error *ErrorOut) noexcept
            -> SegmentInfoCollection;

        [[nodiscard]] constexpr auto
        getFullAddressFromRelative(const uint64_t Relative) const noexcept {
            return ImageAddress + Relative;
        }

        [[nodiscard]] constexpr auto
        FindSegmentWithImageRelativeAddress(const uint64_t Address)
            const noexcept
        {
            const auto FullAddr = this->getFullAddressFromRelative(Address);
            return FindSegmentContainingAddress(FullAddr);
        }

        [[nodiscard]] constexpr auto
        FindSectionWithImageRelativeAddress(
            const uint64_t Address,
            const SegmentInfo **const SegmentOut = nullptr) const noexcept
                -> const SectionInfo *
        {
            const auto FullAddr = this->getFullAddressFromRelative(Address);
            if (const auto Segment =
                    this->FindSegmentContainingAddress(FullAddr))
            {
                *SegmentOut = Segment;
                return Segment->FindSectionContainingAddress(FullAddr);
            }

            return nullptr;
        }
    };
}
