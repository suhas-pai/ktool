//
//  Mach/Machine.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once
#include <string_view>

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
        Haswell
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

    constexpr auto CpuKindIs64Bit(const CpuKind CpuKind) noexcept {
        return static_cast<uint32_t>(CpuKind) & CpuABI64;
    }

    constexpr auto CpuKindIs64_32Bit(const CpuKind CpuKind) noexcept {
        return static_cast<uint32_t>(CpuKind) & CpuABI64_32;
    }

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

    constexpr auto
    CpuKindAndSubKindIsValid(const CpuKind CpuKind,
                             const uint32_t SubKind) noexcept
    {
        switch (CpuKind) {
            case CpuKind::Any:
                return (SubKind == -1);
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
                return (SubKind == 0);
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

    constexpr auto
    CpuKindAndSubKindGetString(const CpuKind CpuKind,
                               const uint32_t SubKind) noexcept
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
                       "CpuKindGetString() called with unknown Vax CpuSubKind");
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
                       "CpuKindGetString() called with unknown Motorola 68000 "
                       "CpuSubKind");
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
                       "CpuKindGetString() called with unknown x86 CpuSubKind");
            case CpuKind::IntelX86_64:
                switch (Intelx86_64(SubKind)) {
                    case Intelx86_64::All:
                        return "CPU_SUBTYPE_X86_64_ALL";
                    case Intelx86_64::Haswell:
                        return "CPU_SUBTYPE_X86_64_H";
                }

                assert(false &&
                       "CpuKindGetString() called with unknown x86_64 "
                       "CpuSubKind");
            case CpuKind::Motorola98000:
                switch (Motorola98000(SubKind)) {
                    case Motorola98000::All:
                        return "CPU_SUBTYPE_MC98000_ALL";
                    case Motorola98000::v98601:
                        return "CPU_SUBTYPE_MC98601";
                }

                assert(false &&
                       "CpuKindGetString() called with unknown Motorola 98000 "
                       "CpuSubKind");
            case CpuKind::Hppa:
                switch (Hppa(SubKind)) {
                    case Hppa::v7100:
                        return "CPU_SUBTYPE_HPPA_7100";
                    case Hppa::v7100LC:
                        return "CPU_SUBTYPE_HPPA_7100LC";
                }

                assert(false &&
                       "CpuKindGetString() called with unknown Motorola 98000 "
                       "CpuSubKind");
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
                       "CpuKindGetString() called with unknown Arm CpuSubKind");
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
                       "CpuKindGetString() called with unknown arm64 "
                       "CpuSubKind");
            case CpuKind::Arm64_32:
                switch (Arm64_32(SubKind)) {
                    case Arm64_32::All:
                        return "CPU_SUBTYPE_ARM64_32_ALL";
                    case Arm64_32::v8:
                        return "CPU_SUBTYPE_ARM64_32_V8";
                }

                assert(false &&
                       "CpuKindGetString() called with unknown arm64_32 "
                       "CpuSubKind");
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
                       "CpuKindGetString() called with unknown Motorola 98000 "
                       "CpuSubKind");
            case CpuKind::Sparc:
                if (SubKind == 0) {
                    return "CPU_TYPE_SPARC_ALL";
                }

                assert(false &&
                       "CpuKindGetString() called with unknown Sparc "
                       "CpuSubKind");
            case CpuKind::IntelI860:
                switch (IntelI860(SubKind)) {
                    case IntelI860::All:
                        return "CPU_SUBTYPE_I860_ALL";
                    case IntelI860::v860:
                        return "CPU_SUBTYPE_I860_860";
                }

                assert(false &&
                       "CpuKindGetString() called with unknown I860 "
                       "CpuSubKind");
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
                       "CpuKindGetString() called with unknown PowerPC[64] "
                       "CpuSubKind");
        }

        assert(false && "CpuKindGetString() called with unknown CpuKind");
    }
}
