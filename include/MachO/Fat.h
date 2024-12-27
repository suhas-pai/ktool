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
        MachO::Magic Magic;
        uint32_t ArchCount;

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
            return MagicIsBigEndian(this->Magic);
        }

        [[nodiscard]] constexpr auto is64Bit() const noexcept {
            return MagicIs64Bit(this->Magic);
        }

        [[nodiscard]] constexpr auto archCount() const noexcept {
            return ADT::SwitchEndianIf(this->ArchCount, this->isBigEndian());
        }
    };

    struct FatArch {
        int32_t CpuKind;
        int32_t CpuSubKind;
        uint32_t Offset;
        uint32_t Size;
        uint32_t Align;

        [[nodiscard]]
        constexpr auto cpuKind(const bool IsBigEndian) const noexcept {
            return Mach::CpuKind(ADT::SwitchEndianIf(this->CpuKind, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto cpuSubKind(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->CpuSubKind, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto offset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto size(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Size, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto align(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Align, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto range(const bool IsBigEndian) const noexcept {
            return ADT::Range::FromSize(this->offset(IsBigEndian),
                                        this->size(IsBigEndian));
        }
    };

    struct FatArch64 {
        int32_t CpuKind;
        int32_t CpuSubKind;
        uint64_t Offset;
        uint64_t Size;
        uint32_t Align;

        [[nodiscard]]
        constexpr auto cpuKind(const bool IsBigEndian) const noexcept {
            const auto Value = ADT::SwitchEndianIf(this->CpuKind, IsBigEndian);
            return Mach::CpuKind(Value);
        }

        [[nodiscard]]
        constexpr auto cpuSubKind(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->CpuSubKind, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto offset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto size(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Size, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto align(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Align, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto range(const bool IsBigEndian) const noexcept {
            return ADT::Range::FromSize(this->offset(IsBigEndian),
                                        this->size(IsBigEndian));
        }
    };
}
