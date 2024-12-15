//
//  Mach/Machine.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once

#include <string_view>
#include "Utils/Assert.h"

namespace Mach {
    constexpr static auto CpuABI64 = 0x01000000;
    constexpr static auto CpuABI64_32 = 0x02000000;

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

    enum class Vax : int32_t {
        All,
        v780,
        v785,
        v750,
        v730,
        UvaxI,
        UvaxII,
        v8200,
        v8500,
        v8600,
        v8650,
        v8800,
        UvaxIII
    };

    enum class Motorola68000 : int32_t {
        All,
        v68040,
        v68030_Only
    };

    enum class Intelx86 : int32_t {
#define CPU_SUBTYPE_INTEL(f, m) ((int32_t)(f) + ((m) << 4))
        x86 = 3,
        x86Arch1 = 4,
        v486SX =        CPU_SUBTYPE_INTEL(4, 8),
        Pentium =       CPU_SUBTYPE_INTEL(5, 0),
        PentiumPro =    CPU_SUBTYPE_INTEL(6, 1),
        PentiumM3 =     CPU_SUBTYPE_INTEL(6, 3),
        PentiumM5 =     CPU_SUBTYPE_INTEL(6, 5),
        Celeron =       CPU_SUBTYPE_INTEL(7, 6),
        CeleronMobile = CPU_SUBTYPE_INTEL(7, 7),
        Pentium3 =      CPU_SUBTYPE_INTEL(8, 0),
        Pentium3m =     CPU_SUBTYPE_INTEL(8, 1),
        Pentium3Xeon =  CPU_SUBTYPE_INTEL(8, 2),
        PentiumM =      CPU_SUBTYPE_INTEL(9, 0),
        Pentium4 =      CPU_SUBTYPE_INTEL(10, 0),
        Pentium4m =     CPU_SUBTYPE_INTEL(10, 1),
        Itanium =       CPU_SUBTYPE_INTEL(11, 0),
        Itanium2 =      CPU_SUBTYPE_INTEL(11, 1),
        Xeon =          CPU_SUBTYPE_INTEL(12, 0),
        XeonMP =        CPU_SUBTYPE_INTEL(12, 1),
#undef CPU_SUBTYPE_INTEL
    };

    enum class Intelx86_64 : int32_t {
        All = 3,
        Haswell = 8
    };

    enum class Motorola98000 : int32_t {
        All,
        v98601
    };

    enum class Hppa : int32_t {
        v7100,
        v7100LC
    };

    enum class Motorola88000 : int32_t {
        All,
        v88100,
        v88110
    };

    enum class Sparc : int32_t {
        All
    };

    enum class IntelI860 : int32_t {
        All,
        v860
    };

    enum class PowerPC : int32_t {
        All,
        v601,
        v602,
        v603,
        v603e,
        v603ev,
        v604,
        v604e,
        v620,
        v750,
        v7400,
        v7450,
        v970 = 100
    };

    enum class Arm : int32_t {
        All,
        v4t = 5,
        v6,
        v5tej,
        Xscale,
        v7,
        v7f,
        v7s,
        v7k,
        v8,
        v6m,
        v7m,
        v7em,
        v8m
    };

    enum class Arm64 : int32_t {
        All,
        v8,
        arm64e
    };

    enum class Arm64_32 : int32_t {
        All,
        v8
    };

    [[nodiscard]]
    constexpr auto CpuKindIsValid(const CpuKind CpuKind) noexcept {
        switch (CpuKind) {
            case CpuKind::Any:
            case CpuKind::Vax:
            case CpuKind::Motorola680x0:
            case CpuKind::IntelX86:
            case CpuKind::IntelX86_64:
            case CpuKind::Motorola98000:
            case CpuKind::Hppa:
            case CpuKind::Arm:
            case CpuKind::Arm64:
            case CpuKind::Arm64_32:
            case CpuKind::Motorola88000:
            case CpuKind::Sparc:
            case CpuKind::IntelI860:
            case CpuKind::PowerPC:
            case CpuKind::PowerPC64:
                return true;
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto CpuKindIs64Bit(const CpuKind CpuKind) noexcept {
        return static_cast<uint32_t>(CpuKind) & CpuABI64;
    }

    [[nodiscard]]
    constexpr auto CpuKindIs64_32Bit(const CpuKind CpuKind) noexcept {
        return static_cast<uint32_t>(CpuKind) & CpuABI64_32;
    }

    [[nodiscard]]
    constexpr auto CpuKindGetString(const CpuKind CpuKind) noexcept ->
        std::string_view
    {
        switch (CpuKind) {
            case CpuKind::Any:
                return "CPU_TYPE_ANY";
            case CpuKind::Vax:
                return "CPU_TYPE_VAX";
            case CpuKind::Motorola680x0:
                return "CPU_TYPE_MC680x0";
            case CpuKind::IntelX86:
                return "CPU_TYPE_X86";
            case CpuKind::IntelX86_64:
                return "CPU_TYPE_X86_64";
            case CpuKind::Motorola98000:
                return "CPU_TYPE_MC98000";
            case CpuKind::Hppa:
                return "CPU_TYPE_HPPA";
            case CpuKind::Arm:
                return "CPU_TYPE_ARM";
            case CpuKind::Arm64:
                return "CPU_TYPE_ARM64";
            case CpuKind::Arm64_32:
                return "CPU_TYPE_ARM64_32";
            case CpuKind::Motorola88000:
                return "CPU_TYPE_MC88000";
            case CpuKind::Sparc:
                return "CPU_TYPE_SPARC";
            case CpuKind::IntelI860:
                return "CPU_TYPE_I860";
            case CpuKind::PowerPC:
                return "CPU_TYPE_POWERPC";
            case CpuKind::PowerPC64:
                return "CPU_TYPE_POWERPC64";
        }

        assert(false && "CpuKindGetString() called with unknown CpuKind");
    }

    [[nodiscard]]
    constexpr auto CpuKindGetDesc(const CpuKind CpuKind) noexcept ->
        std::string_view
    {
        switch (CpuKind) {
            case CpuKind::Any:
                return "Any";
            case CpuKind::Vax:
                return "Vax";
            case CpuKind::Motorola680x0:
                return "Motorola 68000";
            case CpuKind::IntelX86:
                return "Intel x86";
            case CpuKind::IntelX86_64:
                return "Intel x86_64";
            case CpuKind::Motorola98000:
                return "Motorola 98000";
            case CpuKind::Hppa:
                return "Hppa";
            case CpuKind::Arm:
                return "Arm";
            case CpuKind::Arm64:
                return "Arm64";
            case CpuKind::Arm64_32:
                return "Arm64_32";
            case CpuKind::Motorola88000:
                return "Motorola 88000";
            case CpuKind::Sparc:
                return "Sparc";
            case CpuKind::IntelI860:
                return "Intel I860";
            case CpuKind::PowerPC:
                return "PowerPC";
            case CpuKind::PowerPC64:
                return "PowerPC (64-bit)";
        }

        assert(false && "CpuKindGetDesc() called with unknown CpuKind");
    }

    [[nodiscard]]
    constexpr auto
    CpuKindAndSubKindIsValid(const CpuKind CpuKind,
                             const int32_t SubKind) noexcept
    {
        switch (CpuKind) {
            case CpuKind::Any:
                return SubKind == -1;
            case CpuKind::Vax:
                switch (Vax(SubKind)) {
                    case Vax::All:
                    case Vax::v780:
                    case Vax::v785:
                    case Vax::v750:
                    case Vax::v730:
                    case Vax::UvaxI:
                    case Vax::UvaxII:
                    case Vax::v8200:
                    case Vax::v8500:
                    case Vax::v8600:
                    case Vax::v8650:
                    case Vax::v8800:
                    case Vax::UvaxIII:
                        return true;
                }

                return false;
            case CpuKind::Motorola680x0:
                switch (Motorola68000(SubKind)) {
                    case Motorola68000::All:
                    case Motorola68000::v68040:
                    case Motorola68000::v68030_Only:
                        return true;
                }

                return false;
            case CpuKind::IntelX86:
                switch (Intelx86(SubKind)) {
                    case Intelx86::x86:
                    case Intelx86::x86Arch1:
                    case Intelx86::v486SX:
                    case Intelx86::Pentium:
                    case Intelx86::PentiumPro:
                    case Intelx86::PentiumM3:
                    case Intelx86::PentiumM5:
                    case Intelx86::Celeron:
                    case Intelx86::CeleronMobile:
                    case Intelx86::Pentium3:
                    case Intelx86::Pentium3m:
                    case Intelx86::Pentium3Xeon:
                    case Intelx86::PentiumM:
                    case Intelx86::Pentium4:
                    case Intelx86::Pentium4m:
                    case Intelx86::Itanium:
                    case Intelx86::Itanium2:
                    case Intelx86::Xeon:
                    case Intelx86::XeonMP:
                        return true;
                }

                return false;
            case CpuKind::IntelX86_64:
                switch (Intelx86_64(SubKind)) {
                    case Intelx86_64::All:
                    case Intelx86_64::Haswell:
                        return true;
                }

                return false;
            case CpuKind::Motorola98000:
                switch (Motorola98000(SubKind)) {
                    case Motorola98000::All:
                    case Motorola98000::v98601:
                        return true;
                }

                return false;
            case CpuKind::Hppa:
                switch (Hppa(SubKind)) {
                    case Hppa::v7100:
                    case Hppa::v7100LC:
                        return true;
                }

                return false;
            case CpuKind::Arm:
                switch (Arm(SubKind)) {
                    case Arm::All:
                    case Arm::v4t:
                    case Arm::v6:
                    case Arm::v5tej:
                    case Arm::Xscale:
                    case Arm::v7:
                    case Arm::v7f:
                    case Arm::v7s:
                    case Arm::v7k:
                    case Arm::v8:
                    case Arm::v6m:
                    case Arm::v7m:
                    case Arm::v7em:
                    case Arm::v8m:
                        return true;
                }

                return false;
            case CpuKind::Arm64:
                switch (Arm64(SubKind)) {
                    case Arm64::All:
                    case Arm64::v8:
                    case Arm64::arm64e:
                        return true;
                }

                return false;
            case CpuKind::Arm64_32:
                switch (Arm64_32(SubKind)) {
                    case Arm64_32::All:
                    case Arm64_32::v8:
                        return true;
                }

                return false;
            case CpuKind::Motorola88000:
                switch (Motorola88000(SubKind)) {
                    case Motorola88000::All:
                    case Motorola88000::v88100:
                    case Motorola88000::v88110:
                        return true;
                }

                return false;
            case CpuKind::Sparc:
                return SubKind == 0;
            case CpuKind::IntelI860:
                switch (IntelI860(SubKind)) {
                    case IntelI860::All:
                    case IntelI860::v860:
                        return true;
                }

                return false;
            case CpuKind::PowerPC:
            case CpuKind::PowerPC64:
                switch (PowerPC(SubKind)) {
                    case PowerPC::All:
                    case PowerPC::v601:
                    case PowerPC::v602:
                    case PowerPC::v603:
                    case PowerPC::v603e:
                    case PowerPC::v603ev:
                    case PowerPC::v604:
                    case PowerPC::v604e:
                    case PowerPC::v620:
                    case PowerPC::v750:
                    case PowerPC::v7400:
                    case PowerPC::v7450:
                    case PowerPC::v970:
                        return true;
                }

                return false;
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto
    CpuKindAndSubKindGetString(const CpuKind CpuKind,
                               const int32_t SubKind) noexcept
        -> std::string_view
    {
        switch (CpuKind) {
            case CpuKind::Any:
                if (SubKind == -1) {
                    return "CPU_SUBTYPE_ANY";
                }

                break;
            case CpuKind::Vax:
                switch (Vax(SubKind)) {
                    case Vax::All:
                        return "CPU_SUBTYPE_VAX_ALL";
                    case Vax::v780:
                        return "CPU_SUBTYPE_VAX780";
                    case Vax::v785:
                        return "CPU_SUBTYPE_VAX785";
                    case Vax::v750:
                        return "CPU_SUBTYPE_VAX750";
                    case Vax::v730:
                        return "CPU_SUBTYPE_VAX730";
                    case Vax::UvaxI:
                        return "CPU_SUBTYPE_UVAXI";
                    case Vax::UvaxII:
                        return "CPU_SUBTYPE_UVAXII";
                    case Vax::v8200:
                        return "CPU_SUBTYPE_VAX8200";
                    case Vax::v8500:
                        return "CPU_SUBTYPE_VAX8500";
                    case Vax::v8600:
                        return "CPU_SUBTYPE_VAX8600";
                    case Vax::v8650:
                        return "CPU_SUBTYPE_VAX8650";
                    case Vax::v8800:
                        return "CPU_SUBTYPE_VAX8800";
                    case Vax::UvaxIII:
                        return "CPU_SUBTYPE_UVAXIII";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Vax "
                       "Mach::CpuSubKind");
            case CpuKind::Motorola680x0:
                switch (Motorola68000(SubKind)) {
                    case Motorola68000::All:
                        return "CPU_SUBTYPE_MC680x0_ALL";
                    case Motorola68000::v68040:
                        return "CPU_SUBTYPE_MC68040";
                    case Motorola68000::v68030_Only:
                        return "CPU_SUBTYPE_MC68030_ONLY";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Motorola "
                       "68000 Mach::CpuSubKind");
            case CpuKind::IntelX86:
                switch (Intelx86(SubKind)) {
                    case Intelx86::x86:
                        return "CPU_SUBTYPE_X86";
                    case Intelx86::x86Arch1:
                        return "CPU_SUBTYPE_X86_ARCH1";
                    case Intelx86::v486SX:
                        return "CPU_SUBTYPE_486SX";
                    case Intelx86::Pentium:
                        return "CPU_SUBTYPE_PENT";
                    case Intelx86::PentiumPro:
                        return "CPU_SUBTYPE_PENTPRO";
                    case Intelx86::PentiumM3:
                        return "CPU_SUBTYPE_PENTII_M3";
                    case Intelx86::PentiumM5:
                        return "CPU_SUBTYPE_PENTII_M5";
                    case Intelx86::Celeron:
                        return "CPU_SUBTYPE_CELERON";
                    case Intelx86::CeleronMobile:
                        return "CPU_SUBTYPE_CELERON_MOBILE";
                    case Intelx86::Pentium3:
                        return "CPU_SUBTYPE_PENTIUM_3";
                    case Intelx86::Pentium3m:
                        return "CPU_SUBTYPE_PENTIUM_3_M";
                    case Intelx86::Pentium3Xeon:
                        return "CPU_SUBTYPE_PENTIUM_3_XEON";
                    case Intelx86::PentiumM:
                        return "CPU_SUBTYPE_PENTIUM_M";
                    case Intelx86::Pentium4:
                        return "CPU_SUBTYPE_PENTIUM_4";
                    case Intelx86::Pentium4m:
                        return "CPU_SUBTYPE_PENTIUM_4_M";
                    case Intelx86::Itanium:
                        return "CPU_SUBTYPE_ITANIUM";
                    case Intelx86::Itanium2:
                        return "CPU_SUBTYPE_ITANIUM_2";
                    case Intelx86::Xeon:
                        return "CPU_SUBTYPE_XEON";
                    case Intelx86::XeonMP:
                        return "CPU_SUBTYPE_XEON_MP";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown x86 "
                       "Mach::CpuSubKind");
            case CpuKind::IntelX86_64:
                switch (Intelx86_64(SubKind)) {
                    case Intelx86_64::All:
                        return "CPU_SUBTYPE_X86_64_ALL";
                    case Intelx86_64::Haswell:
                        return "CPU_SUBTYPE_X86_64_H";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown x86_64 "
                       "Mach::CpuSubKind");
            case CpuKind::Motorola98000:
                switch (Motorola98000(SubKind)) {
                    case Motorola98000::All:
                        return "CPU_SUBTYPE_MC98000_ALL";
                    case Motorola98000::v98601:
                        return "CPU_SUBTYPE_MC98601";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Motorola "
                       "98000 Mach::CpuSubKind");
            case CpuKind::Hppa:
                switch (Hppa(SubKind)) {
                    case Hppa::v7100:
                        return "CPU_SUBTYPE_HPPA_7100";
                    case Hppa::v7100LC:
                        return "CPU_SUBTYPE_HPPA_7100LC";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Hppa"
                       "Mach::CpuSubKind");
            case CpuKind::Arm:
                switch (Arm(SubKind)) {
                    case Arm::All:
                        return "CPU_SUBTYPE_ARM_ALL";
                    case Arm::v4t:
                        return "CPU_SUBTYPE_ARM_V4T";
                    case Arm::v6:
                        return "CPU_SUBTYPE_ARM_V6";
                    case Arm::v5tej:
                        return "CPU_SUBTYPE_ARM_V5TEJ";
                    case Arm::Xscale:
                        return "CPU_SUBTYPE_ARM_XSCALE";
                    case Arm::v7:
                        return "CPU_SUBTYPE_ARM_V7";
                    case Arm::v7f:
                        return "CPU_SUBTYPE_ARM_V7F";
                    case Arm::v7s:
                        return "CPU_SUBTYPE_ARM_V7S";
                    case Arm::v7k:
                        return "CPU_SUBTYPE_ARM_V7K";
                    case Arm::v8:
                        return "CPU_SUBTYPE_ARM_V8";
                    case Arm::v6m:
                        return "CPU_SUBTYPE_ARM_V6M";
                    case Arm::v7m:
                        return "CPU_SUBTYPE_ARM_V7M";
                    case Arm::v7em:
                        return "CPU_SUBTYPE_ARM_V7EM";
                    case Arm::v8m:
                        return "CPU_SUBTYPE_ARM_V8M";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Arm "
                       "CpuSubKind");
            case CpuKind::Arm64:
                switch (Arm64(SubKind)) {
                    case Arm64::All:
                        return "CPU_SUBTYPE_ARM64_ALL";
                    case Arm64::v8:
                        return "CPU_SUBTYPE_ARM64_V8";
                    case Arm64::arm64e:
                        return "CPU_SUBTYPE_ARM64E";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown arm64 "
                       "Mach::CpuSubKind");
            case CpuKind::Arm64_32:
                switch (Arm64_32(SubKind)) {
                    case Arm64_32::All:
                        return "CPU_SUBTYPE_ARM64_32_ALL";
                    case Arm64_32::v8:
                        return "CPU_SUBTYPE_ARM64_32_V8";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown arm64_32 "
                       "Mach::CpuSubKind");
            case CpuKind::Motorola88000:
                switch (Motorola88000(SubKind)) {
                    case Motorola88000::All:
                        return "CPU_SUBTYPE_MC88000_ALL";
                    case Motorola88000::v88100:
                        return "CPU_SUBTYPE_MC88100";
                    case Motorola88000::v88110:
                        return "CPU_SUBTYPE_MC88110";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Motorola "
                       "98000 Mach::CpuSubKind");
            case CpuKind::Sparc:
                if (SubKind == 0) {
                    return "CPU_TYPE_SPARC_ALL";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown Sparc "
                       "Mach::CpuSubKind");
            case CpuKind::IntelI860:
                switch (IntelI860(SubKind)) {
                    case IntelI860::All:
                        return "CPU_SUBTYPE_I860_ALL";
                    case IntelI860::v860:
                        return "CPU_SUBTYPE_I860_860";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown I860 "
                       "Mach::CpuSubKind");
            case CpuKind::PowerPC:
            case CpuKind::PowerPC64:
                switch (PowerPC(SubKind)) {
                    case PowerPC::All:
                        return "CPU_SUBTYPE_POWERPC_ALL";
                    case PowerPC::v601:
                        return "CPU_SUBTYPE_POWERPC_601";
                    case PowerPC::v602:
                        return "CPU_SUBTYPE_POWERPC_602";
                    case PowerPC::v603:
                        return "CPU_SUBTYPE_POWERPC_603";
                    case PowerPC::v603e:
                        return "CPU_SUBTYPE_POWERPC_603e";
                    case PowerPC::v603ev:
                        return "CPU_SUBTYPE_POWERPC_603ev";
                    case PowerPC::v604:
                        return "CPU_SUBTYPE_POWERPC_603";
                    case PowerPC::v604e:
                        return "CPU_SUBTYPE_POWERPC_604e";
                    case PowerPC::v620:
                        return "CPU_SUBTYPE_POWERPC_620";
                    case PowerPC::v750:
                        return "CPU_SUBTYPE_POWERPC_750";
                    case PowerPC::v7400:
                        return "CPU_SUBTYPE_POWERPC_7400";
                    case PowerPC::v7450:
                        return "CPU_SUBTYPE_POWERPC_7450";
                    case PowerPC::v970:
                        return "CPU_SUBTYPE_POWERPC_970";
                }

                assert(false &&
                       "Mach::CpuKindGetString() called with unknown "
                       "PowerPC[64] Mach::CpuSubKind");
        }

        assert(false && "Mach::CpuKindGetString() called with unknown CpuKind");
    }

    [[nodiscard]]
    constexpr auto
    CpuKindAndSubKindGetDesc(const CpuKind CpuKind,
                             const int32_t SubKind) noexcept
        -> std::string_view
    {
        switch (CpuKind) {
            case CpuKind::Any:
                if (SubKind == -1) {
                    return "Any";
                }

                break;
            case CpuKind::Vax:
                switch (Vax(SubKind)) {
                    case Vax::All:
                        return "Vax All";
                    case Vax::v780:
                        return "Vax 780";
                    case Vax::v785:
                        return "Vax 785";
                    case Vax::v750:
                        return "Vax 750";
                    case Vax::v730:
                        return "Vax 730";
                    case Vax::UvaxI:
                        return "Vax UvaxI";
                    case Vax::UvaxII:
                        return "Vax UvaxII";
                    case Vax::v8200:
                        return "Vax 8200";
                    case Vax::v8500:
                        return "Vax 8500";
                    case Vax::v8600:
                        return "Vax 8600";
                    case Vax::v8650:
                        return "Vax 8650";
                    case Vax::v8800:
                        return "Vax 8800";
                    case Vax::UvaxIII:
                        return "Vax UvaxIII";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Vax "
                       "Mach::CpuSubKind");
            case CpuKind::Motorola680x0:
                switch (Motorola68000(SubKind)) {
                    case Motorola68000::All:
                        return "Motorola 68000 All";
                    case Motorola68000::v68040:
                        return "Motorola 68040";
                    case Motorola68000::v68030_Only:
                        return "Motorola 68030";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Motorola "
                       "68000 Mach::CpuSubKind");
            case CpuKind::IntelX86:
                switch (Intelx86(SubKind)) {
                    case Intelx86::x86:
                        return "Intel x86 All";
                    case Intelx86::x86Arch1:
                        return "Intel x86 Arch1";
                    case Intelx86::v486SX:
                        return "Intel 486SX";
                    case Intelx86::Pentium:
                        return "Intel Pentium";
                    case Intelx86::PentiumPro:
                        return "Intel Pentium Pro";
                    case Intelx86::PentiumM3:
                        return "Intel Pentium M3";
                    case Intelx86::PentiumM5:
                        return "Intel Pentium M5";
                    case Intelx86::Celeron:
                        return "Intel Celeron";
                    case Intelx86::CeleronMobile:
                        return "Intel Celeron Mobile";
                    case Intelx86::Pentium3:
                        return "Intel Pentium 3";
                    case Intelx86::Pentium3m:
                        return "Intel Pentium 3M";
                    case Intelx86::Pentium3Xeon:
                        return "Intel Pentium 3 Xeon";
                    case Intelx86::PentiumM:
                        return "Intel Pentium M";
                    case Intelx86::Pentium4:
                        return "Intel Pentium 4";
                    case Intelx86::Pentium4m:
                        return "Intel Pentium 4M";
                    case Intelx86::Itanium:
                        return "Intel Itanium";
                    case Intelx86::Itanium2:
                        return "Intel Itanium 2";
                    case Intelx86::Xeon:
                        return "Intel Xeon";
                    case Intelx86::XeonMP:
                        return "Intel Xeon MP";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown x86 "
                       "Mach::CpuSubKind");
            case CpuKind::IntelX86_64:
                switch (Intelx86_64(SubKind)) {
                    case Intelx86_64::All:
                        return "Intel x86_64 All";
                    case Intelx86_64::Haswell:
                        return "Intel x86_64 Haswell";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown x86_64 "
                       "Mach::CpuSubKind");
            case CpuKind::Motorola98000:
                switch (Motorola98000(SubKind)) {
                    case Motorola98000::All:
                        return "Motorola 98000 All";
                    case Motorola98000::v98601:
                        return "Motorola 98601";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Motorola "
                       "98000 Mach::CpuSubKind");
            case CpuKind::Hppa:
                switch (Hppa(SubKind)) {
                    case Hppa::v7100:
                        return "Hppa 7100";
                    case Hppa::v7100LC:
                        return "Hppa 7100LC";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Hppa "
                       "Mach::CpuSubKind");
            case CpuKind::Arm:
                switch (Arm(SubKind)) {
                    case Arm::All:
                        return "Arm all";
                    case Arm::v4t:
                        return "Armv4t";
                    case Arm::v6:
                        return "Armv6";
                    case Arm::v5tej:
                        return "Armv5-tej";
                    case Arm::Xscale:
                        return "Arm Xscale";
                    case Arm::v7:
                        return "Armv7";
                    case Arm::v7f:
                        return "Armv7f";
                    case Arm::v7s:
                        return "Armv7s";
                    case Arm::v7k:
                        return "Armv7k";
                    case Arm::v8:
                        return "Armv8";
                    case Arm::v6m:
                        return "Armv6m";
                    case Arm::v7m:
                        return "Armv7m";
                    case Arm::v7em:
                        return "Armv7em";
                    case Arm::v8m:
                        return "Armv8m";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Arm "
                       "Mach::CpuSubKind");
            case CpuKind::Arm64:
                switch (Arm64(SubKind)) {
                    case Arm64::All:
                        return "Arm64 All";
                    case Arm64::v8:
                        return "Arm64 v8";
                    case Arm64::arm64e:
                        return "Arm64e";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown arm64 "
                       "Mach::CpuSubKind");
            case CpuKind::Arm64_32:
                switch (Arm64_32(SubKind)) {
                    case Arm64_32::All:
                        return "Arm64_32 All";
                    case Arm64_32::v8:
                        return "Arm64_32 v8";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown arm64_32 "
                       "Mach::CpuSubKind");
            case CpuKind::Motorola88000:
                switch (Motorola88000(SubKind)) {
                    case Motorola88000::All:
                        return "Motorola All";
                    case Motorola88000::v88100:
                        return "Motorola 88100";
                    case Motorola88000::v88110:
                        return "Motorola 88110";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Motorola "
                       "98000 Mach::CpuSubKind");
            case CpuKind::Sparc:
                if (SubKind == 0) {
                    return "Sparc All";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown Sparc "
                       "Mach::CpuSubKind");
            case CpuKind::IntelI860:
                switch (IntelI860(SubKind)) {
                    case IntelI860::All:
                        return "Intel I860 All";
                    case IntelI860::v860:
                        return "Intel I860 860";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown I860 "
                       "Mach::CpuSubKind");
            case CpuKind::PowerPC:
            case CpuKind::PowerPC64:
                switch (PowerPC(SubKind)) {
                    case PowerPC::All:
                        return "PowerPC All";
                    case PowerPC::v601:
                        return "PowerPC 601";
                    case PowerPC::v602:
                        return "PowerPC 602";
                    case PowerPC::v603:
                        return "PowerPC 603";
                    case PowerPC::v603e:
                        return "PowerPC 604";
                    case PowerPC::v603ev:
                        return "PowerPC 603ev";
                    case PowerPC::v604:
                        return "PowerPC 604";
                    case PowerPC::v604e:
                        return "PowerPC 604e";
                    case PowerPC::v620:
                        return "PowerPC 620";
                    case PowerPC::v750:
                        return "PowerPC 750";
                    case PowerPC::v7400:
                        return "PowerPC 7400";
                    case PowerPC::v7450:
                        return "PowerPC 7450";
                    case PowerPC::v970:
                        return "PowerPC 970";
                }

                assert(false &&
                       "Mach::CpuKindGetDesc() called with unknown PowerPC[64] "
                       "Mach::CpuSubKind");
        }

        assert(false && "Mach::CpuKindGetDesc() called with unknown CpuKind");
    }
}
