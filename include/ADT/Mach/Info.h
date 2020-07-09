//
//  include/ADT/Info.h
//  stool
//
//  Created by Suhas Pai on 4/5/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <string_view>

namespace Mach {
    static inline const auto CpuKindIs64BitMask =
        static_cast<uint32_t>(0x1000000);

    static inline const auto CpuKindIs64Bit32Mask =
        static_cast<uint32_t>(0x2000000);

    enum class CpuKind : int32_t {
        Any = -1,
        Vax = 1,
        Romp,
        NS32032 = 4,
        NS32332,
        Mc680x0,
        i386,
        x86_64 = (i386 | CpuKindIs64BitMask),
        Mips = 8,
        NS32532,
        Hppa = 11,
        Arm,
        Mc88000,
        Sparc,
        i860,
        i860Little,
        Rs9600,
        Mc98000,
        PowerPC,
        PowerPC64 = (PowerPC | CpuKindIs64BitMask),
        Veo = 255,
        Arm64 = (Arm | CpuKindIs64BitMask),
        Arm64_32 = (Arm | CpuKindIs64Bit32Mask)
    };

    [[nodiscard]] inline bool CpuKindIs64Bit(CpuKind CpuKind) noexcept {
        return (static_cast<int32_t>(CpuKind) & CpuKindIs64BitMask);
    }

    [[nodiscard]] inline bool CpuKindIs64Bit32(CpuKind CpuKind) noexcept {
        return (static_cast<int32_t>(CpuKind) & CpuKindIs64Bit32Mask);
    }

    namespace CpuSubKind {
        enum class Any {
            Multiple,
            LittleEndian,
            BigEndian,
        };

        enum class Vax : int32_t {
            All = 0,
            n780,
            n785,
            n750,
            n730,
            UvaXI,
            UvaXII,
            n8200,
            n8500,
            n8600,
            n8650,
            n8800,
            UvaXIII,
        };

        enum class Romp : int32_t {
            All,
            Pc,
            Apc,
            n135
        };

        enum class Mmax : int32_t {
            All,
            Dpc,
            Sqt,
            ApcFPU,
            ApcFPA,
            Xpc
        };

        enum class i386 : int32_t {
            All = 3,
            n386 = 3,
            n486,
            n486sx = 132,
            n586 = 5,
            Pentium = 5,
            PentiumPro = 24,
            PentiumIIM3 = 54,
            PentiumIIM5 = 86,
            Pentium4 = 10,
        };

        enum class x86_64 : int32_t {
            All = 3,
            Haswell = 8
        };

        enum class Mips : int32_t {
            All,
            r2300,
            r2600,
            r2800,
            r2000a
        };

        enum class Mc680x0 : int32_t {
            All = 1,
            n30 = 1,
            n40,
            n30Only
        };

        enum class Hppa : int32_t {
            All,
            n7100 = 0,
            n7100LC
        };

        enum class Arm : int32_t {
            All,
            A500Arch,
            A500,
            A440,
            m4,
            v4T,
            v6,
            v5tej,
            Xscale,
            v7,
            v7f,
            v7s,
            v7k,
            v6m,
            v7m,
            v7em,
            v8
        };

        enum class Arm64 : int32_t {
            All,
            v8,
            E
        };

        enum class Arm64_32 : int32_t {
            v8,
        };

        enum class Mc88000 : int32_t {
            All,
            n100,
            n110
        };

        enum class Mc98000 : int32_t {
            All,
            n601
        };

        enum class i860 : int32_t {
            All,
            n860
        };

        enum class i860Little : int32_t {
            All,
            Default
        };

        enum class Rs9600 : int32_t {
            All,
            Default,
        };

        enum class Sparc : int32_t {
            All
        };

        enum class PowerPC : int32_t {
            All,
            n601,
            n602,
            n603,
            n603e,
            n603ev,
            n604,
            n604e,
            n620,
            n750,
            n7400,
            n7500,
            n970 = 100
        };

        enum class PowerPC64 : int32_t {
            All,
            n970
        };

        enum class Veo : int32_t {
            n1,
            n2,
            n3,
            n4,
            All = static_cast<int>(n2)
        };
    }
}
