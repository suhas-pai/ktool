//
//  Mach/Machine.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once

namespace Mach {
    static constexpr auto CpuABI64 = 0x01000000;
    static constexpr auto CpuABI64_32 = 0x02000000;

    enum class CpuKind : int32_t {
        Any,
        Vax,

        Motorola680x0 = 6,
        IntelX86,
        IntelX86_64 = static_cast<int32_t>(IntelX86) | CpuABI64,

        Motorola98000 = 10,
        Hppa,

        Arm,
        Arm64 = static_cast<int32_t>(Arm) | CpuABI64,
        Arm64_32 = static_cast<int32_t>(Arm) | CpuABI64_32,

        Motorola88000 = 13,
        Sparc,
        IntelI860,

        PowerPC = 18,
        PowerPC64 = static_cast<int32_t>(PowerPC) | CpuABI64,
    };
}
