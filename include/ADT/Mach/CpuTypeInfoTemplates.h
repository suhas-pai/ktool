//
//  include/ADT/Mach/CpuType.h
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
    template <CpuType Type>
    struct CpuTypeTemplateInfo {};

    template <>
    struct CpuTypeTemplateInfo<CpuType::Any> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Any;

        constexpr static std::string_view Name = "CPU_TYPE_ANY";
        constexpr static std::string_view BrandName = "Any";
        constexpr static std::string_view Description = "Any";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Vax> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Vax;

        constexpr static std::string_view Name = "CPU_TYPE_VAX";
        constexpr static std::string_view BrandName = "Vax";
        constexpr static std::string_view Description = "Vax";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Romp> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Romp;

        constexpr static std::string_view Name = "CPU_TYPE_ROMP";
        constexpr static std::string_view BrandName = "Romp";
        constexpr static std::string_view Description = "Romp";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::NS32032> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::NS32032;

        constexpr static std::string_view Name = "CPU_TYPE_NS32032";
        constexpr static std::string_view BrandName = "NS32032";
        constexpr static std::string_view Description = "NS32032";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::NS32332> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::NS32332;

        constexpr static std::string_view Name = "CPU_TYPE_NS32332";
        constexpr static std::string_view BrandName = "NS32332";
        constexpr static std::string_view Description = "NS32332";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Mc680x0> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Mc680x0;

        constexpr static std::string_view Name = "CPU_TYPE_Mc680x0";
        constexpr static std::string_view BrandName = "Motorola";
        constexpr static std::string_view Description = "Motorola m68k";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::i386> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::i386;

        constexpr static std::string_view Name = "CPU_TYPE_i386";
        constexpr static std::string_view BrandName = "Intel";
        constexpr static std::string_view Description = "i386";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::x86_64> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::x86_64;

        constexpr static std::string_view Name = "CPU_TYPE_X86_64";
        constexpr static std::string_view BrandName = "Intel";
        constexpr static std::string_view Description = "x86_64";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Mips> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Mips;

        constexpr static std::string_view Name = "CPU_TYPE_MIPS";
        constexpr static std::string_view BrandName = "Mips";
        constexpr static std::string_view Description = "Mips";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::NS32532> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::NS32532;

        constexpr static std::string_view Name = "CPU_TYPE_NS32532";
        constexpr static std::string_view BrandName = "NS32532";
        constexpr static std::string_view Description = "NS32532";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Hppa> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Hppa;

        constexpr static std::string_view Name = "CPU_TYPE_HPPA";
        constexpr static std::string_view BrandName = "Hppa";
        constexpr static std::string_view Description = "Hppa";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Arm> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Arm;

        constexpr static std::string_view Name = "CPU_TYPE_ARM";
        constexpr static std::string_view BrandName = "Arm";
        constexpr static std::string_view Description = "Arm";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Mc88000> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Mc88000;

        constexpr static std::string_view Name = "CPU_TYPE_ARM";
        constexpr static std::string_view BrandName = "Motorola";
        constexpr static std::string_view Description = "Motorola m88K";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Sparc> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Sparc;

        constexpr static std::string_view Name = "CPU_TYPE_SPARC";
        constexpr static std::string_view BrandName = "Sparc";
        constexpr static std::string_view Description = "Sparc";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::i860> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::i860;

        constexpr static std::string_view Name = "CPU_TYPE_I860";
        constexpr static std::string_view BrandName = "Intel";
        constexpr static std::string_view Description = "Intel i860";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::i860Little> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::i860Little;

        constexpr static std::string_view Name = "CPU_TYPE_I860_LITTLE";
        constexpr static std::string_view BrandName = "Intel";
        constexpr static std::string_view Description =
            "Intel i860 (Little-Endian)";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Rs9600> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Rs9600;

        constexpr static std::string_view Name = "CPU_TYPE_RS9600";
        constexpr static std::string_view BrandName = "Rs9600";
        constexpr static std::string_view Description = "Rs9600";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Mc98000> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Mc98000;

        constexpr static std::string_view Name = "CPU_TYPE_RS9600";
        constexpr static std::string_view BrandName = "Motorola";
        constexpr static std::string_view Description = "Motorola m98k";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::PowerPC> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::PowerPC;

        constexpr static std::string_view Name = "CPU_TYPE_POWERPC";
        constexpr static std::string_view BrandName = "PowerPC";
        constexpr static std::string_view Description = "PowerPC";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::PowerPC64> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::PowerPC64;

        constexpr static std::string_view Name = "CPU_TYPE_POWERPC64";
        constexpr static std::string_view BrandName = "PowerPC64";
        constexpr static std::string_view Description = "PowerPC (64-Bit)";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Veo> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Veo;

        constexpr static std::string_view Name = "CPU_TYPE_VEO";
        constexpr static std::string_view BrandName = "Veo";
        constexpr static std::string_view Description = "Veo";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Arm64> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Arm64;

        constexpr static std::string_view Name = "CPU_TYPE_ARM64";
        constexpr static std::string_view BrandName = "Arm64";
        constexpr static std::string_view Description = "Arm64";
    };

    template <>
    struct CpuTypeTemplateInfo<CpuType::Arm64_32> {
        constexpr static Mach::CpuType CpuType = Mach::CpuType::Arm64_32;

        constexpr static std::string_view Name = "CPU_TYPE_ARM64_32";
        constexpr static std::string_view BrandName = "Arm64_32";
        constexpr static std::string_view Description = "Arm64_32";
    };

    constexpr std::string_view CpuTypeGetName(CpuType CpuType) noexcept {
        switch (CpuType) {
            case CpuType::Any:
                return CpuTypeTemplateInfo<CpuType::Any>::Name;
            case CpuType::Vax:
                return CpuTypeTemplateInfo<CpuType::Vax>::Name;
            case CpuType::Romp:
                return CpuTypeTemplateInfo<CpuType::Romp>::Name;
            case CpuType::NS32032:
                return CpuTypeTemplateInfo<CpuType::NS32032>::Name;
            case CpuType::NS32332:
                return CpuTypeTemplateInfo<CpuType::NS32332>::Name;
            case CpuType::Mc680x0:
                return CpuTypeTemplateInfo<CpuType::Mc680x0>::Name;
            case CpuType::i386:
                return CpuTypeTemplateInfo<CpuType::i386>::Name;
            case CpuType::x86_64:
                return CpuTypeTemplateInfo<CpuType::x86_64>::Name;
            case CpuType::Mips:
                return CpuTypeTemplateInfo<CpuType::Mips>::Name;
            case CpuType::NS32532:
                return CpuTypeTemplateInfo<CpuType::NS32532>::Name;
            case CpuType::Hppa:
                return CpuTypeTemplateInfo<CpuType::Hppa>::Name;
            case CpuType::Arm:
                return CpuTypeTemplateInfo<CpuType::Arm>::Name;
            case CpuType::Mc88000:
                return CpuTypeTemplateInfo<CpuType::Mc88000>::Name;
            case CpuType::Sparc:
                return CpuTypeTemplateInfo<CpuType::Sparc>::Name;
            case CpuType::i860:
                return CpuTypeTemplateInfo<CpuType::i860>::Name;
            case CpuType::i860Little:
                return CpuTypeTemplateInfo<CpuType::i860Little>::Name;
            case CpuType::Rs9600:
                return CpuTypeTemplateInfo<CpuType::Rs9600>::Name;
            case CpuType::Mc98000:
                return CpuTypeTemplateInfo<CpuType::Mc98000>::Name;
            case CpuType::PowerPC:
                return CpuTypeTemplateInfo<CpuType::PowerPC>::Name;
            case CpuType::PowerPC64:
                return CpuTypeTemplateInfo<CpuType::PowerPC64>::Name;
            case CpuType::Veo:
                return CpuTypeTemplateInfo<CpuType::Veo>::Name;
            case CpuType::Arm64:
                return CpuTypeTemplateInfo<CpuType::Arm64>::Name;
            case CpuType::Arm64_32:
                return CpuTypeTemplateInfo<CpuType::Arm64_32>::Name;
        }

        return std::string_view();
    }

    constexpr
    std::string_view CpuTypeGetBrandName(enum CpuType CpuType) noexcept {
        switch (CpuType) {
            case CpuType::Any:
                return CpuTypeTemplateInfo<CpuType::Any>::BrandName;
            case CpuType::Vax:
                return CpuTypeTemplateInfo<CpuType::Vax>::BrandName;
            case CpuType::Romp:
                return CpuTypeTemplateInfo<CpuType::Romp>::BrandName;
            case CpuType::NS32032:
                return CpuTypeTemplateInfo<CpuType::NS32032>::BrandName;
            case CpuType::NS32332:
                return CpuTypeTemplateInfo<CpuType::NS32332>::BrandName;
            case CpuType::Mc680x0:
                return CpuTypeTemplateInfo<CpuType::Mc680x0>::BrandName;
            case CpuType::i386:
                return CpuTypeTemplateInfo<CpuType::i386>::BrandName;
            case CpuType::x86_64:
                return CpuTypeTemplateInfo<CpuType::x86_64>::BrandName;
            case CpuType::Mips:
                return CpuTypeTemplateInfo<CpuType::Mips>::BrandName;
            case CpuType::NS32532:
                return CpuTypeTemplateInfo<CpuType::NS32532>::BrandName;
            case CpuType::Hppa:
                return CpuTypeTemplateInfo<CpuType::Hppa>::BrandName;
            case CpuType::Arm:
                return CpuTypeTemplateInfo<CpuType::Arm>::BrandName;
            case CpuType::Mc88000:
                return CpuTypeTemplateInfo<CpuType::Mc88000>::BrandName;
            case CpuType::Sparc:
                return CpuTypeTemplateInfo<CpuType::Sparc>::BrandName;
            case CpuType::i860:
                return CpuTypeTemplateInfo<CpuType::i860>::BrandName;
            case CpuType::i860Little:
                return CpuTypeTemplateInfo<CpuType::i860Little>::BrandName;
            case CpuType::Rs9600:
                return CpuTypeTemplateInfo<CpuType::Rs9600>::BrandName;
            case CpuType::Mc98000:
                return CpuTypeTemplateInfo<CpuType::Mc98000>::BrandName;
            case CpuType::PowerPC:
                return CpuTypeTemplateInfo<CpuType::PowerPC>::BrandName;
            case CpuType::PowerPC64:
                return CpuTypeTemplateInfo<CpuType::PowerPC64>::BrandName;
            case CpuType::Veo:
                return CpuTypeTemplateInfo<CpuType::Veo>::BrandName;
            case CpuType::Arm64:
                return CpuTypeTemplateInfo<CpuType::Arm64>::BrandName;
            case CpuType::Arm64_32:
                return CpuTypeTemplateInfo<CpuType::Arm64_32>::BrandName;
        }

        return std::string_view();
    }

    constexpr
    std::string_view CpuTypeGetDescripton(enum CpuType CpuType) noexcept {
        switch (CpuType) {
            case CpuType::Any:
                return CpuTypeTemplateInfo<CpuType::Any>::Description;
            case CpuType::Vax:
                return CpuTypeTemplateInfo<CpuType::Vax>::Description;
            case CpuType::Romp:
                return CpuTypeTemplateInfo<CpuType::Romp>::Description;
            case CpuType::NS32032:
                return CpuTypeTemplateInfo<CpuType::NS32032>::Description;
            case CpuType::NS32332:
                return CpuTypeTemplateInfo<CpuType::NS32332>::Description;
            case CpuType::Mc680x0:
                return CpuTypeTemplateInfo<CpuType::Mc680x0>::Description;
            case CpuType::i386:
                return CpuTypeTemplateInfo<CpuType::i386>::Description;
            case CpuType::x86_64:
                return CpuTypeTemplateInfo<CpuType::x86_64>::Description;
            case CpuType::Mips:
                return CpuTypeTemplateInfo<CpuType::Mips>::Description;
            case CpuType::NS32532:
                return CpuTypeTemplateInfo<CpuType::NS32532>::Description;
            case CpuType::Hppa:
                return CpuTypeTemplateInfo<CpuType::Hppa>::Description;
            case CpuType::Arm:
                return CpuTypeTemplateInfo<CpuType::Arm>::Description;
            case CpuType::Mc88000:
                return CpuTypeTemplateInfo<CpuType::Mc88000>::Description;
            case CpuType::Sparc:
                return CpuTypeTemplateInfo<CpuType::Sparc>::Description;
            case CpuType::i860:
                return CpuTypeTemplateInfo<CpuType::i860>::Description;
            case CpuType::i860Little:
                return CpuTypeTemplateInfo<CpuType::i860Little>::Description;
            case CpuType::Rs9600:
                return CpuTypeTemplateInfo<CpuType::Rs9600>::Description;
            case CpuType::Mc98000:
                return CpuTypeTemplateInfo<CpuType::Mc98000>::Description;
            case CpuType::PowerPC:
                return CpuTypeTemplateInfo<CpuType::PowerPC>::Description;
            case CpuType::PowerPC64:
                return CpuTypeTemplateInfo<CpuType::PowerPC64>::Description;
            case CpuType::Veo:
                return CpuTypeTemplateInfo<CpuType::Veo>::Description;
            case CpuType::Arm64:
                return CpuTypeTemplateInfo<CpuType::Arm64>::Description;
            case CpuType::Arm64_32:
                return CpuTypeTemplateInfo<CpuType::Arm64_32>::Description;
        }

        return std::string_view();
    }
}
