//
//  include/ADT/Mach/CpuKindInfoTemplates.h
//  stool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <string_view>

#include "Utils/Macros.h"
#include "Info.h"

namespace Mach {
    template <CpuKind Kind>
    struct CpuKindTemplateInfo {};

    using namespace std::literals;

    template <>
    struct CpuKindTemplateInfo<CpuKind::Any> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Any;

        constexpr static auto Name = "CPU_TYPE_ANY"sv;
        constexpr static auto BrandName = "Any"sv;
        constexpr static auto Description = "Any"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Vax> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Vax;

        constexpr static auto Name = "CPU_TYPE_VAX"sv;
        constexpr static auto BrandName = "Vax"sv;
        constexpr static auto Description = "Vax"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Romp> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Romp;

        constexpr static auto Name = "CPU_TYPE_ROMP"sv;
        constexpr static auto BrandName = "Romp"sv;
        constexpr static auto Description = "Romp"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::NS32032> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::NS32032;

        constexpr static auto Name = "CPU_TYPE_NS32032"sv;
        constexpr static auto BrandName = "NS32032"sv;
        constexpr static auto Description = "NS32032"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::NS32332> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::NS32332;

        constexpr static auto Name = "CPU_TYPE_NS32332"sv;
        constexpr static auto BrandName = "NS32332"sv;
        constexpr static auto Description = "NS32332"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Mc680x0> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Mc680x0;

        constexpr static auto Name = "CPU_TYPE_Mc680x0"sv;
        constexpr static auto BrandName = "Motorola"sv;
        constexpr static auto Description = "Motorola m68k"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::i386> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::i386;

        constexpr static auto Name = "CPU_TYPE_i386"sv;
        constexpr static auto BrandName = "Intel"sv;
        constexpr static auto Description = "i386"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::x86_64> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::x86_64;

        constexpr static auto Name = "CPU_TYPE_X86_64"sv;
        constexpr static auto BrandName = "Intel"sv;
        constexpr static auto Description = "x86_64"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Mips> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Mips;

        constexpr static auto Name = "CPU_TYPE_MIPS"sv;
        constexpr static auto BrandName = "Mips"sv;
        constexpr static auto Description = "Mips"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::NS32532> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::NS32532;

        constexpr static auto Name = "CPU_TYPE_NS32532"sv;
        constexpr static auto BrandName = "NS32532"sv;
        constexpr static auto Description = "NS32532"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Hppa> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Hppa;

        constexpr static auto Name = "CPU_TYPE_HPPA"sv;
        constexpr static auto BrandName = "Hppa"sv;
        constexpr static auto Description = "Hppa"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Arm> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Arm;

        constexpr static auto Name = "CPU_TYPE_ARM"sv;
        constexpr static auto BrandName = "Arm"sv;
        constexpr static auto Description = "Arm"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Mc88000> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Mc88000;

        constexpr static auto Name = "CPU_TYPE_ARM"sv;
        constexpr static auto BrandName = "Motorola"sv;
        constexpr static auto Description = "Motorola m88K"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Sparc> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Sparc;

        constexpr static auto Name = "CPU_TYPE_SPARC"sv;
        constexpr static auto BrandName = "Sparc"sv;
        constexpr static auto Description = "Sparc"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::i860> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::i860;

        constexpr static auto Name = "CPU_TYPE_I860"sv;
        constexpr static auto BrandName = "Intel"sv;
        constexpr static auto Description = "Intel i860"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::i860Little> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::i860Little;

        constexpr static auto Name = "CPU_TYPE_I860_LITTLE"sv;
        constexpr static auto BrandName = "Intel"sv;
        constexpr static auto Description =
            "Intel i860 (Little-Endian)"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Rs9600> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Rs9600;

        constexpr static auto Name = "CPU_TYPE_RS9600"sv;
        constexpr static auto BrandName = "Rs9600"sv;
        constexpr static auto Description = "Rs9600"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Mc98000> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Mc98000;

        constexpr static auto Name = "CPU_TYPE_RS9600"sv;
        constexpr static auto BrandName = "Motorola"sv;
        constexpr static auto Description = "Motorola m98k"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::PowerPC> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::PowerPC;

        constexpr static auto Name = "CPU_TYPE_POWERPC"sv;
        constexpr static auto BrandName = "PowerPC"sv;
        constexpr static auto Description = "PowerPC"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::PowerPC64> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::PowerPC64;

        constexpr static auto Name = "CPU_TYPE_POWERPC64"sv;
        constexpr static auto BrandName = "PowerPC64"sv;
        constexpr static auto Description = "PowerPC (64-Bit)"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Veo> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Veo;

        constexpr static auto Name = "CPU_TYPE_VEO"sv;
        constexpr static auto BrandName = "Veo"sv;
        constexpr static auto Description = "Veo"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Arm64> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Arm64;

        constexpr static auto Name = "CPU_TYPE_ARM64"sv;
        constexpr static auto BrandName = "Arm64"sv;
        constexpr static auto Description = "Arm64"sv;
    };

    template <>
    struct CpuKindTemplateInfo<CpuKind::Arm64_32> {
        constexpr static Mach::CpuKind CpuKind = Mach::CpuKind::Arm64_32;

        constexpr static auto Name = "CPU_TYPE_ARM64_32"sv;
        constexpr static auto BrandName = "Arm64_32"sv;
        constexpr static auto Description = "Arm64_32"sv;
    };

    constexpr std::string_view CpuKindGetName(CpuKind CpuKind) noexcept {
        switch (CpuKind) {
            case CpuKind::Any:
                return CpuKindTemplateInfo<CpuKind::Any>::Name;
            case CpuKind::Vax:
                return CpuKindTemplateInfo<CpuKind::Vax>::Name;
            case CpuKind::Romp:
                return CpuKindTemplateInfo<CpuKind::Romp>::Name;
            case CpuKind::NS32032:
                return CpuKindTemplateInfo<CpuKind::NS32032>::Name;
            case CpuKind::NS32332:
                return CpuKindTemplateInfo<CpuKind::NS32332>::Name;
            case CpuKind::Mc680x0:
                return CpuKindTemplateInfo<CpuKind::Mc680x0>::Name;
            case CpuKind::i386:
                return CpuKindTemplateInfo<CpuKind::i386>::Name;
            case CpuKind::x86_64:
                return CpuKindTemplateInfo<CpuKind::x86_64>::Name;
            case CpuKind::Mips:
                return CpuKindTemplateInfo<CpuKind::Mips>::Name;
            case CpuKind::NS32532:
                return CpuKindTemplateInfo<CpuKind::NS32532>::Name;
            case CpuKind::Hppa:
                return CpuKindTemplateInfo<CpuKind::Hppa>::Name;
            case CpuKind::Arm:
                return CpuKindTemplateInfo<CpuKind::Arm>::Name;
            case CpuKind::Mc88000:
                return CpuKindTemplateInfo<CpuKind::Mc88000>::Name;
            case CpuKind::Sparc:
                return CpuKindTemplateInfo<CpuKind::Sparc>::Name;
            case CpuKind::i860:
                return CpuKindTemplateInfo<CpuKind::i860>::Name;
            case CpuKind::i860Little:
                return CpuKindTemplateInfo<CpuKind::i860Little>::Name;
            case CpuKind::Rs9600:
                return CpuKindTemplateInfo<CpuKind::Rs9600>::Name;
            case CpuKind::Mc98000:
                return CpuKindTemplateInfo<CpuKind::Mc98000>::Name;
            case CpuKind::PowerPC:
                return CpuKindTemplateInfo<CpuKind::PowerPC>::Name;
            case CpuKind::PowerPC64:
                return CpuKindTemplateInfo<CpuKind::PowerPC64>::Name;
            case CpuKind::Veo:
                return CpuKindTemplateInfo<CpuKind::Veo>::Name;
            case CpuKind::Arm64:
                return CpuKindTemplateInfo<CpuKind::Arm64>::Name;
            case CpuKind::Arm64_32:
                return CpuKindTemplateInfo<CpuKind::Arm64_32>::Name;
        }

        return std::string_view();
    }

    constexpr
    std::string_view CpuKindGetBrandName(enum CpuKind CpuKind) noexcept {
        switch (CpuKind) {
            case CpuKind::Any:
                return CpuKindTemplateInfo<CpuKind::Any>::BrandName;
            case CpuKind::Vax:
                return CpuKindTemplateInfo<CpuKind::Vax>::BrandName;
            case CpuKind::Romp:
                return CpuKindTemplateInfo<CpuKind::Romp>::BrandName;
            case CpuKind::NS32032:
                return CpuKindTemplateInfo<CpuKind::NS32032>::BrandName;
            case CpuKind::NS32332:
                return CpuKindTemplateInfo<CpuKind::NS32332>::BrandName;
            case CpuKind::Mc680x0:
                return CpuKindTemplateInfo<CpuKind::Mc680x0>::BrandName;
            case CpuKind::i386:
                return CpuKindTemplateInfo<CpuKind::i386>::BrandName;
            case CpuKind::x86_64:
                return CpuKindTemplateInfo<CpuKind::x86_64>::BrandName;
            case CpuKind::Mips:
                return CpuKindTemplateInfo<CpuKind::Mips>::BrandName;
            case CpuKind::NS32532:
                return CpuKindTemplateInfo<CpuKind::NS32532>::BrandName;
            case CpuKind::Hppa:
                return CpuKindTemplateInfo<CpuKind::Hppa>::BrandName;
            case CpuKind::Arm:
                return CpuKindTemplateInfo<CpuKind::Arm>::BrandName;
            case CpuKind::Mc88000:
                return CpuKindTemplateInfo<CpuKind::Mc88000>::BrandName;
            case CpuKind::Sparc:
                return CpuKindTemplateInfo<CpuKind::Sparc>::BrandName;
            case CpuKind::i860:
                return CpuKindTemplateInfo<CpuKind::i860>::BrandName;
            case CpuKind::i860Little:
                return CpuKindTemplateInfo<CpuKind::i860Little>::BrandName;
            case CpuKind::Rs9600:
                return CpuKindTemplateInfo<CpuKind::Rs9600>::BrandName;
            case CpuKind::Mc98000:
                return CpuKindTemplateInfo<CpuKind::Mc98000>::BrandName;
            case CpuKind::PowerPC:
                return CpuKindTemplateInfo<CpuKind::PowerPC>::BrandName;
            case CpuKind::PowerPC64:
                return CpuKindTemplateInfo<CpuKind::PowerPC64>::BrandName;
            case CpuKind::Veo:
                return CpuKindTemplateInfo<CpuKind::Veo>::BrandName;
            case CpuKind::Arm64:
                return CpuKindTemplateInfo<CpuKind::Arm64>::BrandName;
            case CpuKind::Arm64_32:
                return CpuKindTemplateInfo<CpuKind::Arm64_32>::BrandName;
        }

        return std::string_view();
    }

    constexpr
    std::string_view CpuKindGetDescripton(enum CpuKind CpuKind) noexcept {
        switch (CpuKind) {
            case CpuKind::Any:
                return CpuKindTemplateInfo<CpuKind::Any>::Description;
            case CpuKind::Vax:
                return CpuKindTemplateInfo<CpuKind::Vax>::Description;
            case CpuKind::Romp:
                return CpuKindTemplateInfo<CpuKind::Romp>::Description;
            case CpuKind::NS32032:
                return CpuKindTemplateInfo<CpuKind::NS32032>::Description;
            case CpuKind::NS32332:
                return CpuKindTemplateInfo<CpuKind::NS32332>::Description;
            case CpuKind::Mc680x0:
                return CpuKindTemplateInfo<CpuKind::Mc680x0>::Description;
            case CpuKind::i386:
                return CpuKindTemplateInfo<CpuKind::i386>::Description;
            case CpuKind::x86_64:
                return CpuKindTemplateInfo<CpuKind::x86_64>::Description;
            case CpuKind::Mips:
                return CpuKindTemplateInfo<CpuKind::Mips>::Description;
            case CpuKind::NS32532:
                return CpuKindTemplateInfo<CpuKind::NS32532>::Description;
            case CpuKind::Hppa:
                return CpuKindTemplateInfo<CpuKind::Hppa>::Description;
            case CpuKind::Arm:
                return CpuKindTemplateInfo<CpuKind::Arm>::Description;
            case CpuKind::Mc88000:
                return CpuKindTemplateInfo<CpuKind::Mc88000>::Description;
            case CpuKind::Sparc:
                return CpuKindTemplateInfo<CpuKind::Sparc>::Description;
            case CpuKind::i860:
                return CpuKindTemplateInfo<CpuKind::i860>::Description;
            case CpuKind::i860Little:
                return CpuKindTemplateInfo<CpuKind::i860Little>::Description;
            case CpuKind::Rs9600:
                return CpuKindTemplateInfo<CpuKind::Rs9600>::Description;
            case CpuKind::Mc98000:
                return CpuKindTemplateInfo<CpuKind::Mc98000>::Description;
            case CpuKind::PowerPC:
                return CpuKindTemplateInfo<CpuKind::PowerPC>::Description;
            case CpuKind::PowerPC64:
                return CpuKindTemplateInfo<CpuKind::PowerPC64>::Description;
            case CpuKind::Veo:
                return CpuKindTemplateInfo<CpuKind::Veo>::Description;
            case CpuKind::Arm64:
                return CpuKindTemplateInfo<CpuKind::Arm64>::Description;
            case CpuKind::Arm64_32:
                return CpuKindTemplateInfo<CpuKind::Arm64_32>::Description;
        }

        return std::string_view();
    }
}
