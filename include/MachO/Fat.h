//
//  MachO/Fat.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once

#include "ADT/Range.h"
#include "ADT/SwitchEndian.h"

#include "Mach/Machine.h"
#include "Magic.h"

namespace MachO {
    struct FatHeader {
        Magic Magic;
        uint32_t ArchCount;

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
            return MagicIsBigEndian(Magic);
        }

        [[nodiscard]] constexpr auto is64Bit() const noexcept {
            return MagicIs64Bit(Magic);
        }

        [[nodiscard]] constexpr auto archCount() const noexcept {
            return ADT::SwitchEndianIf(ArchCount, isBigEndian());
        }
    };

    struct FatArch {
        Mach::CpuKind CpuKind;
        int32_t CpuSubKind;
        uint32_t Offset;
        uint32_t Size;
        uint32_t Align;

        [[nodiscard]] constexpr auto range() const noexcept {
            return ADT::Range::FromSize(Offset, Size);
        }
    };

    struct FatArch64 {
        Mach::CpuKind CpuKind;
        int32_t CpuSubKind;
        uint64_t Offset;
        uint64_t Size;
        uint32_t Align;

        [[nodiscard]] constexpr auto range() const noexcept {
            return ADT::Range::FromSize(Offset, Size);
        }
    };
}
