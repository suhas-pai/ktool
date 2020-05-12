//
//  include/ADT/Mach/CpuSubType.h
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
    template <Mach::CpuSubType::Any>
    struct CpuSubTypeAnyInfo {};

    template <>
    struct CpuSubTypeAnyInfo<CpuSubType::Any::Multiple> {
        constexpr static CpuSubType::Any CpuSubType = CpuSubType::Any::Multiple;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = true;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MULTIPLE";
        constexpr static std::string_view FullName = "Multiple-Endian";
        constexpr static std::string_view Description = "Multiple-Endian";
    };

    template <>
    struct CpuSubTypeAnyInfo<CpuSubType::Any::LittleEndian> {
        constexpr static CpuSubType::Any CpuSubType =
            CpuSubType::Any::LittleEndian;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_LITTLE_ENDIAN";
        constexpr static std::string_view FullName = "Little-Endian";
        constexpr static std::string_view Description = "Little-Endian";
    };

    template <>
    struct CpuSubTypeAnyInfo<CpuSubType::Any::BigEndian> {
        constexpr static CpuSubType::Any CpuSubType =
            CpuSubType::Any::LittleEndian;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_BIG_ENDIAN";
        constexpr static std::string_view FullName = "Big-Endian";
        constexpr static std::string_view Description = "Big-Endian";
    };

    template <Mach::CpuSubType::Vax>
    struct CpuSubTypeVaxInfo {};

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::All> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_ALL";
        constexpr static std::string_view FullName = "Vax All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n780> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n780;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_780";
        constexpr static std::string_view FullName = "Vax 780";
        constexpr static std::string_view Description = "780";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n785> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n785;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_785";
        constexpr static std::string_view FullName = "Vax 785";
        constexpr static std::string_view Description = "785";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n750> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n750;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_750";
        constexpr static std::string_view FullName = "Vax 750";
        constexpr static std::string_view Description = "750";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n730> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n730;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_730";
        constexpr static std::string_view FullName = "Vax 730";
        constexpr static std::string_view Description = "730";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::UvaXI> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::UvaXI;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_UVAXI";
        constexpr static std::string_view FullName = "Vax UVAXI";
        constexpr static std::string_view Description = "UVAXI";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::UvaXII> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::UvaXII;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_UVAXII";
        constexpr static std::string_view FullName = "Vax UVAXII";
        constexpr static std::string_view Description = "UVAXII";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n8200> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n8200;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_8200";
        constexpr static std::string_view FullName = "Vax 8200";
        constexpr static std::string_view Description = "8200";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n8500> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n8500;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_8500";
        constexpr static std::string_view FullName = "Vax 8500";
        constexpr static std::string_view Description = "8500";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n8600> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n8600;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_8600";
        constexpr static std::string_view FullName = "Vax 8600";
        constexpr static std::string_view Description = "8600";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n8650> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n8650;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_8650";
        constexpr static std::string_view FullName = "Vax 8650";
        constexpr static std::string_view Description = "8650";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::n8800> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::n8800;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_8880";
        constexpr static std::string_view FullName = "Vax 8800";
        constexpr static std::string_view Description = "8880";
    };

    template <>
    struct CpuSubTypeVaxInfo<CpuSubType::Vax::UvaXIII> {
        constexpr static CpuSubType::Vax CpuSubType = CpuSubType::Vax::UvaXIII;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VAX_UVAXIII";
        constexpr static std::string_view FullName = "Vax UVAXIII";
        constexpr static std::string_view Description = "UVAXIII";
    };

    template <Mach::CpuSubType::Romp>
    struct CpuSubTypeRompInfo {};

    template <>
    struct CpuSubTypeRompInfo<CpuSubType::Romp::All> {
        constexpr static CpuSubType::Romp CpuSubType = CpuSubType::Romp::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ROMP_ALL";
        constexpr static std::string_view FullName = "Romp All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeRompInfo<CpuSubType::Romp::Pc> {
        constexpr static CpuSubType::Romp CpuSubType = CpuSubType::Romp::Pc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ROMP_PC";
        constexpr static std::string_view FullName = "Romp PC";
        constexpr static std::string_view Description = "PC";
    };

    template <>
    struct CpuSubTypeRompInfo<CpuSubType::Romp::Apc> {
        constexpr static CpuSubType::Romp CpuSubType = CpuSubType::Romp::Apc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ROMP_APC";
        constexpr static std::string_view FullName = "Romp APC";
        constexpr static std::string_view Description = "APC";
    };

    template <>
    struct CpuSubTypeRompInfo<CpuSubType::Romp::n135> {
        constexpr static CpuSubType::Romp CpuSubType = CpuSubType::Romp::Apc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ROMP_135";
        constexpr static std::string_view FullName = "Romp 135";
        constexpr static std::string_view Description = "135";
    };

    template <Mach::CpuSubType::Mmax>
    struct CpuSubTypeMmaxInfo {};

    template <>
    struct CpuSubTypeMmaxInfo<CpuSubType::Mmax::All> {
        constexpr static CpuSubType::Mmax CpuSubType = CpuSubType::Mmax::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MMAX_ALL";
        constexpr static std::string_view FullName = "Mmax All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeMmaxInfo<CpuSubType::Mmax::Dpc> {
        constexpr static CpuSubType::Mmax CpuSubType = CpuSubType::Mmax::Dpc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MMAX_DPC";
        constexpr static std::string_view FullName = "Mmax Dpc";
        constexpr static std::string_view Description = "Dpc";
    };

    template <>
    struct CpuSubTypeMmaxInfo<CpuSubType::Mmax::Sqt> {
        constexpr static CpuSubType::Mmax CpuSubType = CpuSubType::Mmax::Sqt;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MMAX_SQT";
        constexpr static std::string_view FullName = "Mmax Sqt";
        constexpr static std::string_view Description = "Sqt";
    };

    template <>
    struct CpuSubTypeMmaxInfo<CpuSubType::Mmax::ApcFPU> {
        constexpr static CpuSubType::Mmax CpuSubType = CpuSubType::Mmax::ApcFPU;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MMAX_APC_FPU";
        constexpr static std::string_view FullName = "Mmax Apc FPU";
        constexpr static std::string_view Description = "Apc FPU";
    };

    template <>
    struct CpuSubTypeMmaxInfo<CpuSubType::Mmax::ApcFPA> {
        constexpr static CpuSubType::Mmax CpuSubType = CpuSubType::Mmax::ApcFPA;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MMAX_APC_FPA";
        constexpr static std::string_view FullName = "Mmax Apc FPA";
        constexpr static std::string_view Description = "Apc FPA";
    };

    template <>
    struct CpuSubTypeMmaxInfo<CpuSubType::Mmax::Xpc> {
        constexpr static CpuSubType::Mmax CpuSubType = CpuSubType::Mmax::Xpc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MMAX_XPC";
        constexpr static std::string_view FullName = "Mmax Xpc";
        constexpr static std::string_view Description = "Xpc";
    };

    template <Mach::CpuSubType::i386>
    struct CpuSubTypei386Info {};

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::All> {
        constexpr static CpuSubType::i386 CpuSubType = CpuSubType::i386::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_I386_ALL";
        constexpr static std::string_view FullName = "Intel i386 All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::n486> {
        constexpr static CpuSubType::i386 CpuSubType = CpuSubType::i386::n486;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_486";
        constexpr static std::string_view FullName = "Intel Xpc";
        constexpr static std::string_view Description = "486";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::n486sx> {
        constexpr static CpuSubType::i386 CpuSubType = CpuSubType::i386::n486sx;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_486SX";
        constexpr static std::string_view FullName = "Intel 486 SX";
        constexpr static std::string_view Description = "486 SX";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::Pentium> {
        constexpr static CpuSubType::i386 CpuSubType =
            CpuSubType::i386::Pentium;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_PENTIUM";
        constexpr static std::string_view FullName = "Intel Pentium";
        constexpr static std::string_view Description = "Pentium";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::PentiumPro> {
        constexpr static CpuSubType::i386 CpuSubType =
            CpuSubType::i386::PentiumPro;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_PENTPRO";
        constexpr static std::string_view FullName = "Intel Pentium Pro";
        constexpr static std::string_view Description = "Pentium Pro";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::PentiumIIM3> {
        constexpr static CpuSubType::i386 CpuSubType =
            CpuSubType::i386::PentiumIIM3;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_PENTII_M3";
        constexpr static std::string_view FullName = "Intel Pentium IIM3";
        constexpr static std::string_view Description = "Pentium IIM3";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::PentiumIIM5> {
        constexpr static CpuSubType::i386 CpuSubType =
            CpuSubType::i386::PentiumIIM5;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_PENTII_M5";
        constexpr static std::string_view FullName = "Intel Pentium IIM5";
        constexpr static std::string_view Description = "Pentium IIM5";
    };

    template <>
    struct CpuSubTypei386Info<CpuSubType::i386::Pentium4> {
        constexpr static CpuSubType::i386 CpuSubType =
            CpuSubType::i386::Pentium4;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_PENTIUM_4";
        constexpr static std::string_view FullName = "Intel Pentium 4";
        constexpr static std::string_view Description = "Pentium 4";
    };

    template <Mach::CpuSubType::x86_64>
    struct CpuSubTypeX86_64Info {};

    template <>
    struct CpuSubTypeX86_64Info<CpuSubType::x86_64::All> {
        constexpr static CpuSubType::x86_64 CpuSubType =
            CpuSubType::x86_64::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_X86_64_ALL";
        constexpr static std::string_view FullName = "Intel x86_64 All";
        constexpr static std::string_view Description = "x86_64 All";
    };

    template <>
    struct CpuSubTypeX86_64Info<CpuSubType::x86_64::Haswell> {
        constexpr static CpuSubType::x86_64 CpuSubType =
            CpuSubType::x86_64::Haswell;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_X86_64_ALL";
        constexpr static std::string_view FullName = "Intel x86_64 Haswell";
        constexpr static std::string_view Description = "Haswell";
    };

    template <Mach::CpuSubType::Mips>
    struct CpuSubTypeMipsInfo {};

    template <>
    struct CpuSubTypeMipsInfo<CpuSubType::Mips::All> {
        constexpr static CpuSubType::Mips CpuSubType = CpuSubType::Mips::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MIPS_ALL";
        constexpr static std::string_view FullName = "Mips All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeMipsInfo<CpuSubType::Mips::r2300> {
        constexpr static CpuSubType::Mips CpuSubType = CpuSubType::Mips::r2300;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MIPS_R2300";
        constexpr static std::string_view FullName = "Mips R2300";
        constexpr static std::string_view Description = "R2300";
    };

    template <>
    struct CpuSubTypeMipsInfo<CpuSubType::Mips::r2600> {
        constexpr static CpuSubType::Mips CpuSubType = CpuSubType::Mips::r2600;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MIPS_R2600";
        constexpr static std::string_view FullName = "Mips R2600";
        constexpr static std::string_view Description = "R2600";
    };

    template <>
    struct CpuSubTypeMipsInfo<CpuSubType::Mips::r2800> {
        constexpr static CpuSubType::Mips CpuSubType = CpuSubType::Mips::r2800;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MIPS_R2800";
        constexpr static std::string_view FullName = "Mips R2800";
        constexpr static std::string_view Description = "R2800";
    };

    template <>
    struct CpuSubTypeMipsInfo<CpuSubType::Mips::r2000a> {
        constexpr static CpuSubType::Mips CpuSubType = CpuSubType::Mips::r2000a;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MIPS_R2000A";
        constexpr static std::string_view FullName = "Mips R2000a";
        constexpr static std::string_view Description = "R2000a";
    };

    template <Mach::CpuSubType::Mc680x0>
    struct CpuSubTypeMc680x0Info {};

    template <>
    struct CpuSubTypeMc680x0Info<CpuSubType::Mc680x0::All> {
        constexpr static CpuSubType::Mc680x0 CpuSubType =
            CpuSubType::Mc680x0::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC680X0_All";
        constexpr static std::string_view FullName = "Motorola m68k All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeMc680x0Info<CpuSubType::Mc680x0::n40> {
        constexpr static CpuSubType::Mc680x0 CpuSubType =
            CpuSubType::Mc680x0::n40;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC68040";
        constexpr static std::string_view FullName = "Motorola 68040";
        constexpr static std::string_view Description = "68040";
    };

    template <>
    struct CpuSubTypeMc680x0Info<CpuSubType::Mc680x0::n30Only> {
        constexpr static CpuSubType::Mc680x0 CpuSubType =
            CpuSubType::Mc680x0::n30Only;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC68030_ONLY";
        constexpr static std::string_view FullName = "Motorola 68030 Only";
        constexpr static std::string_view Description = "68030 Only";
    };

    template <Mach::CpuSubType::Hppa>
    struct CpuSubTypeHppaInfo {};

    template <>
    struct CpuSubTypeHppaInfo<CpuSubType::Hppa::All> {
        constexpr static CpuSubType::Hppa CpuSubType = CpuSubType::Hppa::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_HPPA_ONLY";
        constexpr static std::string_view FullName = "Hppa All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeHppaInfo<CpuSubType::Hppa::n7100LC> {
        constexpr static CpuSubType::Hppa CpuSubType =
            CpuSubType::Hppa::n7100LC;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_HPPA_7100LC";
        constexpr static std::string_view FullName = "Hppa 7100LC";
        constexpr static std::string_view Description = "7100LC";
    };

    template <Mach::CpuSubType::Arm>
    struct CpuSubTypeArmInfo {};

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::All> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_ALL";
        constexpr static std::string_view FullName = "Arm All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::A500Arch> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::A500Arch;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_A500_ARCH";
        constexpr static std::string_view FullName = "Arm A500 Arch";
        constexpr static std::string_view Description = "A500 Arch";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::A500> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::A500;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_A500";
        constexpr static std::string_view FullName = "Arm A500";
        constexpr static std::string_view Description = "A500";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::A440> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::A440;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_A440";
        constexpr static std::string_view FullName = "Arm A440";
        constexpr static std::string_view Description = "A440";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::m4> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::m4;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_M4";
        constexpr static std::string_view FullName = "Arm M4";
        constexpr static std::string_view Description = "M4";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v4T> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v4T;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V4";
        constexpr static std::string_view FullName = "Armv4t";
        constexpr static std::string_view Description = "V4T";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v6> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v6;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V6";
        constexpr static std::string_view FullName = "Armv6";
        constexpr static std::string_view Description = "V6";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v5tej> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v5tej;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V5TEJ";
        constexpr static std::string_view FullName = "Armv5";
        constexpr static std::string_view Description = "V5";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::Xscale> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::Xscale;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_XSCALE";
        constexpr static std::string_view FullName = "Arm Xscale";
        constexpr static std::string_view Description = "Xscale";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v7> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v7;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V7";
        constexpr static std::string_view FullName = "Armv7";
        constexpr static std::string_view Description = "V7";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v7f> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v7f;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V7";
        constexpr static std::string_view FullName = "Armv7f";
        constexpr static std::string_view Description = "V7F";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v7s> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v7s;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V7S";
        constexpr static std::string_view FullName = "Armv7s";
        constexpr static std::string_view Description = "V7S";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v7k> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v7s;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V7S";
        constexpr static std::string_view FullName = "Armv7s";
        constexpr static std::string_view Description = "V7S";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v6m> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v6m;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V6M";
        constexpr static std::string_view FullName = "Armv6m";
        constexpr static std::string_view Description = "V6M";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v7m> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v7m;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V7M";
        constexpr static std::string_view FullName = "Armv7m";
        constexpr static std::string_view Description = "V7M";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v7em> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v7em;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V7EM";
        constexpr static std::string_view FullName = "Armv7em";
        constexpr static std::string_view Description = "V7EM";
    };

    template <>
    struct CpuSubTypeArmInfo<CpuSubType::Arm::v8> {
        constexpr static CpuSubType::Arm CpuSubType = CpuSubType::Arm::v8;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V8";
        constexpr static std::string_view FullName = "Armv8";
        constexpr static std::string_view Description = "V8";
    };

    template <Mach::CpuSubType::Arm64>
    struct CpuSubTypeArm64Info {};

    template <>
    struct CpuSubTypeArm64Info<CpuSubType::Arm64::All> {
        constexpr static CpuSubType::Arm64 CpuSubType = CpuSubType::Arm64::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM_V8";
        constexpr static std::string_view FullName = "Arm64 All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeArm64Info<CpuSubType::Arm64::v8> {
        constexpr static CpuSubType::Arm64 CpuSubType = CpuSubType::Arm64::v8;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM64_V8";
        constexpr static std::string_view FullName = "Arm64v8";
        constexpr static std::string_view Description = "V8";
    };

    template <>
    struct CpuSubTypeArm64Info<CpuSubType::Arm64::E> {
        constexpr static CpuSubType::Arm64 CpuSubType = CpuSubType::Arm64::E;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM64E";
        constexpr static std::string_view FullName = "Arm64E";
        constexpr static std::string_view Description = "E";
    };

    template <Mach::CpuSubType::Arm64_32>
    struct CpuSubTypeArm64_32Info {};

    template <>
    struct CpuSubTypeArm64_32Info<CpuSubType::Arm64_32::v8> {
        constexpr static CpuSubType::Arm64_32 CpuSubType =
            CpuSubType::Arm64_32::v8;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_ARM64_32_V8";
        constexpr static std::string_view FullName = "Arm64_32 v8";
        constexpr static std::string_view Description = "V8";
    };

    template <Mach::CpuSubType::Mc88000>
    struct CpuSubTypeMc88000Info {};

    template <>
    struct CpuSubTypeMc88000Info<CpuSubType::Mc88000::All> {
        constexpr static CpuSubType::Mc88000 CpuSubType =
            CpuSubType::Mc88000::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC88000_ALL";
        constexpr static std::string_view FullName = "Motorola m88k All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeMc88000Info<CpuSubType::Mc88000::n100> {
        constexpr static CpuSubType::Mc88000 CpuSubType =
            CpuSubType::Mc88000::n100;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC88000_100";
        constexpr static std::string_view FullName = "Motorola 88100";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeMc88000Info<CpuSubType::Mc88000::n110> {
        constexpr static CpuSubType::Mc88000 CpuSubType =
            CpuSubType::Mc88000::n110;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC88000_110";
        constexpr static std::string_view FullName = "Motorola 88110";
        constexpr static std::string_view Description = "All";
    };

    template <Mach::CpuSubType::Sparc>
    struct CpuSubTypeSparcInfo {};

    template <>
    struct CpuSubTypeSparcInfo<CpuSubType::Sparc::All> {
        constexpr static CpuSubType::Sparc CpuSubType = CpuSubType::Sparc::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_SPARC_ALL";
        constexpr static std::string_view FullName = "Sparc All";
        constexpr static std::string_view Description = "All";
    };

    template <Mach::CpuSubType::Mc98000>
    struct CpuSubTypeMc98000Info {};

    template <>
    struct CpuSubTypeMc98000Info<CpuSubType::Mc98000::All> {
        constexpr static CpuSubType::Mc98000 CpuSubType =
            CpuSubType::Mc98000::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC98000_ALL";
        constexpr static std::string_view FullName = "Motorola m98k All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeMc98000Info<CpuSubType::Mc98000::n601> {
        constexpr static CpuSubType::Mc98000 CpuSubType =
            CpuSubType::Mc98000::n601;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_MC98000_601";
        constexpr static std::string_view FullName = "Motorola mk98k 601";
        constexpr static std::string_view Description = "All";
    };

    template <Mach::CpuSubType::i860>
    struct CpuSubTypeI860Info {};

    template <>
    struct CpuSubTypeI860Info<CpuSubType::i860::All> {
        constexpr static CpuSubType::i860 CpuSubType = CpuSubType::i860::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_I860_ALL";
        constexpr static std::string_view FullName = "Intel i860 All";
        constexpr static std::string_view Description = "i860";
    };

    template <>
    struct CpuSubTypeI860Info<CpuSubType::i860::n860> {
        constexpr static CpuSubType::i860 CpuSubType = CpuSubType::i860::n860;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_I860_601";
        constexpr static std::string_view FullName = "Intel i860 601";
        constexpr static std::string_view Description = "601";
    };

    template <Mach::CpuSubType::i860Little>
    struct CpuSubTypeI860LittleInfo {};

    template <>
    struct CpuSubTypeI860LittleInfo<CpuSubType::i860Little::All> {
        constexpr static CpuSubType::i860Little CpuSubType =
            CpuSubType::i860Little::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_I860_LITTLE_ALL";
        constexpr static std::string_view FullName = "Intel i860 (Little-Endian)";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeI860LittleInfo<CpuSubType::i860Little::Default> {
        constexpr static CpuSubType::i860Little CpuSubType =
            CpuSubType::i860Little::Default;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_I860_LITTLE";
        constexpr static std::string_view FullName = "Intel i860 (Little-Endian)";
        constexpr static std::string_view Description = "All";
    };

    template <Mach::CpuSubType::Rs9600>
    struct CpuSubTypeRs9600Info {};

    template <>
    struct CpuSubTypeRs9600Info<CpuSubType::Rs9600::All> {
        constexpr static CpuSubType::Rs9600 CpuSubType =
            CpuSubType::Rs9600::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_RS9600_LITTLE_ALL";
        constexpr static std::string_view FullName = "Rs9600";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypeRs9600Info<CpuSubType::Rs9600::Default> {
        constexpr static CpuSubType::i860Little CpuSubType =
            CpuSubType::i860Little::Default;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_RS9600_LITTLE";
        constexpr static std::string_view FullName = "Rs9600";
        constexpr static std::string_view Description = "All";
    };

    template <Mach::CpuSubType::PowerPC>
    struct CpuSubTypePowerPCInfo {};

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::All> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::All;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_ALL";
        constexpr static std::string_view FullName = "PowerPC All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n601> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n601;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_601";
        constexpr static std::string_view FullName = "PowerPC 601";
        constexpr static std::string_view Description = "601";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n602> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n602;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_602";
        constexpr static std::string_view FullName = "PowerPC 602";
        constexpr static std::string_view Description = "602";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n603> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n603;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_603";
        constexpr static std::string_view FullName = "PowerPC 603";
        constexpr static std::string_view Description = "603";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n603e> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n603e;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_603E";
        constexpr static std::string_view FullName = "PowerPC 603e";
        constexpr static std::string_view Description = "603e";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n603ev> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n603ev;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_603EV";
        constexpr static std::string_view FullName = "PowerPC 603ev";
        constexpr static std::string_view Description = "603ev";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n604> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n604;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_604";
        constexpr static std::string_view FullName = "PowerPC 604";
        constexpr static std::string_view Description = "604";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n604e> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n604e;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_604E";
        constexpr static std::string_view FullName = "PowerPC 604e";
        constexpr static std::string_view Description = "604e";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n620> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n620;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_620";
        constexpr static std::string_view FullName = "PowerPC 620";
        constexpr static std::string_view Description = "620";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n750> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n750;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_750";
        constexpr static std::string_view FullName = "PowerPC 750";
        constexpr static std::string_view Description = "750";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n7400> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n7400;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_7400";
        constexpr static std::string_view FullName = "PowerPC 7400";
        constexpr static std::string_view Description = "7400";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n7500> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n7500;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_7500";
        constexpr static std::string_view FullName = "PowerPC 7500";
        constexpr static std::string_view Description = "7500";
    };

    template <>
    struct CpuSubTypePowerPCInfo<CpuSubType::PowerPC::n970> {
        constexpr static CpuSubType::PowerPC CpuSubType =
            CpuSubType::PowerPC::n970;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_970";
        constexpr static std::string_view FullName = "PowerPC 970";
        constexpr static std::string_view Description = "970";
    };

    template <Mach::CpuSubType::PowerPC64>
    struct CpuSubTypePowerPC64Info {};

    template <>
    struct CpuSubTypePowerPC64Info<CpuSubType::PowerPC64::All> {
        constexpr static CpuSubType::PowerPC64 CpuSubType =
            CpuSubType::PowerPC64::All;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_ALL";
        constexpr static std::string_view FullName = "PowerPC (64-Bit) All";
        constexpr static std::string_view Description = "All";
    };

    template <>
    struct CpuSubTypePowerPC64Info<CpuSubType::PowerPC64::n970> {
        constexpr static CpuSubType::PowerPC64 CpuSubType =
            CpuSubType::PowerPC64::n970;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_POWERPC_970";
        constexpr static std::string_view FullName = "PowerPC (64-Bit) 970";
        constexpr static std::string_view Description = "All";
    };

    template <Mach::CpuSubType::Veo>
    struct CpuSubTypeVeoInfo {};

    template <>
    struct CpuSubTypeVeoInfo<CpuSubType::Veo::n1> {
        constexpr static CpuSubType::Veo CpuSubType = CpuSubType::Veo::n1;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VEO_1";
        constexpr static std::string_view FullName = "Veo 1";
        constexpr static std::string_view Description = "1";
    };

    template <>
    struct CpuSubTypeVeoInfo<CpuSubType::Veo::n2> {
        constexpr static CpuSubType::Veo CpuSubType = CpuSubType::Veo::n2;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VEO_2";
        constexpr static std::string_view FullName = "Veo 2";
        constexpr static std::string_view Description = "2";
    };

    template <>
    struct CpuSubTypeVeoInfo<CpuSubType::Veo::n3> {
        constexpr static CpuSubType::Veo CpuSubType = CpuSubType::Veo::n3;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VEO_3";
        constexpr static std::string_view FullName = "Veo 3";
        constexpr static std::string_view Description = "3";
    };

    template <>
    struct CpuSubTypeVeoInfo<CpuSubType::Veo::n4> {
        constexpr static CpuSubType::Veo CpuSubType = CpuSubType::Veo::n4;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static std::string_view Name = "CPU_SUBTYPE_VEO_4";
        constexpr static std::string_view FullName = "Veo 4";
        constexpr static std::string_view Description = "4";
    };
}
