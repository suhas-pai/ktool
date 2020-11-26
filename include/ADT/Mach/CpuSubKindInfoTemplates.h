//
//  include/ADT/Mach/CpuSubKind.h
//  ktool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <string_view>

#include "Utils/Macros.h"
#include "Info.h"

namespace Mach {
    template <Mach::CpuSubKind::Any>
    struct CpuSubKindAnyInfo {};

    using namespace std::literals;

    template <>
    struct CpuSubKindAnyInfo<CpuSubKind::Any::Multiple> {
        constexpr static CpuSubKind::Any CpuSubKind = CpuSubKind::Any::Multiple;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = true;

        constexpr static auto Name = "CPU_SUBTYPE_MULTIPLE"sv;
        constexpr static auto FullName = "Multiple-Endian"sv;
        constexpr static auto Description = "Multiple-Endian"sv;
    };

    template <>
    struct CpuSubKindAnyInfo<CpuSubKind::Any::LittleEndian> {
        constexpr static CpuSubKind::Any CpuSubKind =
            CpuSubKind::Any::LittleEndian;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_LITTLE_ENDIAN"sv;
        constexpr static auto FullName = "Little-Endian"sv;
        constexpr static auto Description = "Little-Endian"sv;
    };

    template <>
    struct CpuSubKindAnyInfo<CpuSubKind::Any::BigEndian> {
        constexpr static CpuSubKind::Any CpuSubKind =
            CpuSubKind::Any::LittleEndian;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_BIG_ENDIAN"sv;
        constexpr static auto FullName = "Big-Endian"sv;
        constexpr static auto Description = "Big-Endian"sv;
    };

    template <Mach::CpuSubKind::Vax>
    struct CpuSubKindVaxInfo {};

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::All> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_ALL"sv;
        constexpr static auto FullName = "Vax All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n780> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n780;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_780"sv;
        constexpr static auto FullName = "Vax 780"sv;
        constexpr static auto Description = "780"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n785> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n785;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_785"sv;
        constexpr static auto FullName = "Vax 785"sv;
        constexpr static auto Description = "785"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n750> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n750;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_750"sv;
        constexpr static auto FullName = "Vax 750"sv;
        constexpr static auto Description = "750"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n730> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n730;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_730"sv;
        constexpr static auto FullName = "Vax 730"sv;
        constexpr static auto Description = "730"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::UvaXI> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::UvaXI;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_UVAXI"sv;
        constexpr static auto FullName = "Vax UVAXI"sv;
        constexpr static auto Description = "UVAXI"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::UvaXII> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::UvaXII;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_UVAXII"sv;
        constexpr static auto FullName = "Vax UVAXII"sv;
        constexpr static auto Description = "UVAXII"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n8200> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n8200;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_8200"sv;
        constexpr static auto FullName = "Vax 8200"sv;
        constexpr static auto Description = "8200"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n8500> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n8500;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_8500"sv;
        constexpr static auto FullName = "Vax 8500"sv;
        constexpr static auto Description = "8500"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n8600> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n8600;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_8600"sv;
        constexpr static auto FullName = "Vax 8600"sv;
        constexpr static auto Description = "8600"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n8650> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n8650;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_8650"sv;
        constexpr static auto FullName = "Vax 8650"sv;
        constexpr static auto Description = "8650"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::n8800> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::n8800;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_8880"sv;
        constexpr static auto FullName = "Vax 8800"sv;
        constexpr static auto Description = "8880"sv;
    };

    template <>
    struct CpuSubKindVaxInfo<CpuSubKind::Vax::UvaXIII> {
        constexpr static CpuSubKind::Vax CpuSubKind = CpuSubKind::Vax::UvaXIII;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VAX_UVAXIII"sv;
        constexpr static auto FullName = "Vax UVAXIII"sv;
        constexpr static auto Description = "UVAXIII"sv;
    };

    template <Mach::CpuSubKind::Romp>
    struct CpuSubKindRompInfo {};

    template <>
    struct CpuSubKindRompInfo<CpuSubKind::Romp::All> {
        constexpr static CpuSubKind::Romp CpuSubKind = CpuSubKind::Romp::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ROMP_ALL"sv;
        constexpr static auto FullName = "Romp All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindRompInfo<CpuSubKind::Romp::Pc> {
        constexpr static CpuSubKind::Romp CpuSubKind = CpuSubKind::Romp::Pc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ROMP_PC"sv;
        constexpr static auto FullName = "Romp PC"sv;
        constexpr static auto Description = "PC"sv;
    };

    template <>
    struct CpuSubKindRompInfo<CpuSubKind::Romp::Apc> {
        constexpr static CpuSubKind::Romp CpuSubKind = CpuSubKind::Romp::Apc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ROMP_APC"sv;
        constexpr static auto FullName = "Romp APC"sv;
        constexpr static auto Description = "APC"sv;
    };

    template <>
    struct CpuSubKindRompInfo<CpuSubKind::Romp::n135> {
        constexpr static CpuSubKind::Romp CpuSubKind = CpuSubKind::Romp::Apc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ROMP_135"sv;
        constexpr static auto FullName = "Romp 135"sv;
        constexpr static auto Description = "135"sv;
    };

    template <Mach::CpuSubKind::Mmax>
    struct CpuSubKindMmaxInfo {};

    template <>
    struct CpuSubKindMmaxInfo<CpuSubKind::Mmax::All> {
        constexpr static CpuSubKind::Mmax CpuSubKind = CpuSubKind::Mmax::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MMAX_ALL"sv;
        constexpr static auto FullName = "Mmax All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindMmaxInfo<CpuSubKind::Mmax::Dpc> {
        constexpr static CpuSubKind::Mmax CpuSubKind = CpuSubKind::Mmax::Dpc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MMAX_DPC"sv;
        constexpr static auto FullName = "Mmax Dpc"sv;
        constexpr static auto Description = "Dpc"sv;
    };

    template <>
    struct CpuSubKindMmaxInfo<CpuSubKind::Mmax::Sqt> {
        constexpr static CpuSubKind::Mmax CpuSubKind = CpuSubKind::Mmax::Sqt;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MMAX_SQT"sv;
        constexpr static auto FullName = "Mmax Sqt"sv;
        constexpr static auto Description = "Sqt"sv;
    };

    template <>
    struct CpuSubKindMmaxInfo<CpuSubKind::Mmax::ApcFPU> {
        constexpr static CpuSubKind::Mmax CpuSubKind = CpuSubKind::Mmax::ApcFPU;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MMAX_APC_FPU"sv;
        constexpr static auto FullName = "Mmax Apc FPU"sv;
        constexpr static auto Description = "Apc FPU"sv;
    };

    template <>
    struct CpuSubKindMmaxInfo<CpuSubKind::Mmax::ApcFPA> {
        constexpr static CpuSubKind::Mmax CpuSubKind = CpuSubKind::Mmax::ApcFPA;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MMAX_APC_FPA"sv;
        constexpr static auto FullName = "Mmax Apc FPA"sv;
        constexpr static auto Description = "Apc FPA"sv;
    };

    template <>
    struct CpuSubKindMmaxInfo<CpuSubKind::Mmax::Xpc> {
        constexpr static CpuSubKind::Mmax CpuSubKind = CpuSubKind::Mmax::Xpc;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MMAX_XPC"sv;
        constexpr static auto FullName = "Mmax Xpc"sv;
        constexpr static auto Description = "Xpc"sv;
    };

    template <Mach::CpuSubKind::i386>
    struct CpuSubKindi386Info {};

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::All> {
        constexpr static CpuSubKind::i386 CpuSubKind = CpuSubKind::i386::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_I386_ALL"sv;
        constexpr static auto FullName = "Intel i386 All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::n486> {
        constexpr static CpuSubKind::i386 CpuSubKind = CpuSubKind::i386::n486;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_486"sv;
        constexpr static auto FullName = "Intel Xpc"sv;
        constexpr static auto Description = "486"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::n486sx> {
        constexpr static CpuSubKind::i386 CpuSubKind = CpuSubKind::i386::n486sx;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_486SX"sv;
        constexpr static auto FullName = "Intel 486 SX"sv;
        constexpr static auto Description = "486 SX"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::Pentium> {
        constexpr static CpuSubKind::i386 CpuSubKind =
            CpuSubKind::i386::Pentium;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_PENTIUM"sv;
        constexpr static auto FullName = "Intel Pentium"sv;
        constexpr static auto Description = "Pentium"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::PentiumPro> {
        constexpr static CpuSubKind::i386 CpuSubKind =
            CpuSubKind::i386::PentiumPro;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_PENTPRO"sv;
        constexpr static auto FullName = "Intel Pentium Pro"sv;
        constexpr static auto Description = "Pentium Pro"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::PentiumIIM3> {
        constexpr static CpuSubKind::i386 CpuSubKind =
            CpuSubKind::i386::PentiumIIM3;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_PENTII_M3"sv;
        constexpr static auto FullName = "Intel Pentium IIM3"sv;
        constexpr static auto Description = "Pentium IIM3"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::PentiumIIM5> {
        constexpr static CpuSubKind::i386 CpuSubKind =
            CpuSubKind::i386::PentiumIIM5;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_PENTII_M5"sv;
        constexpr static auto FullName = "Intel Pentium IIM5"sv;
        constexpr static auto Description = "Pentium IIM5"sv;
    };

    template <>
    struct CpuSubKindi386Info<CpuSubKind::i386::Pentium4> {
        constexpr static CpuSubKind::i386 CpuSubKind =
            CpuSubKind::i386::Pentium4;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_PENTIUM_4"sv;
        constexpr static auto FullName = "Intel Pentium 4"sv;
        constexpr static auto Description = "Pentium 4"sv;
    };

    template <Mach::CpuSubKind::x86_64>
    struct CpuSubKindX86_64Info {};

    template <>
    struct CpuSubKindX86_64Info<CpuSubKind::x86_64::All> {
        constexpr static CpuSubKind::x86_64 CpuSubKind =
            CpuSubKind::x86_64::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_X86_64_ALL"sv;
        constexpr static auto FullName = "Intel x86_64 All"sv;
        constexpr static auto Description = "x86_64 All"sv;
    };

    template <>
    struct CpuSubKindX86_64Info<CpuSubKind::x86_64::Haswell> {
        constexpr static CpuSubKind::x86_64 CpuSubKind =
            CpuSubKind::x86_64::Haswell;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_X86_64_ALL"sv;
        constexpr static auto FullName = "Intel x86_64 Haswell"sv;
        constexpr static auto Description = "Haswell"sv;
    };

    template <Mach::CpuSubKind::Mips>
    struct CpuSubKindMipsInfo {};

    template <>
    struct CpuSubKindMipsInfo<CpuSubKind::Mips::All> {
        constexpr static CpuSubKind::Mips CpuSubKind = CpuSubKind::Mips::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MIPS_ALL"sv;
        constexpr static auto FullName = "Mips All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindMipsInfo<CpuSubKind::Mips::r2300> {
        constexpr static CpuSubKind::Mips CpuSubKind = CpuSubKind::Mips::r2300;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MIPS_R2300"sv;
        constexpr static auto FullName = "Mips R2300"sv;
        constexpr static auto Description = "R2300"sv;
    };

    template <>
    struct CpuSubKindMipsInfo<CpuSubKind::Mips::r2600> {
        constexpr static CpuSubKind::Mips CpuSubKind = CpuSubKind::Mips::r2600;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MIPS_R2600"sv;
        constexpr static auto FullName = "Mips R2600"sv;
        constexpr static auto Description = "R2600"sv;
    };

    template <>
    struct CpuSubKindMipsInfo<CpuSubKind::Mips::r2800> {
        constexpr static CpuSubKind::Mips CpuSubKind = CpuSubKind::Mips::r2800;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MIPS_R2800"sv;
        constexpr static auto FullName = "Mips R2800"sv;
        constexpr static auto Description = "R2800"sv;
    };

    template <>
    struct CpuSubKindMipsInfo<CpuSubKind::Mips::r2000a> {
        constexpr static CpuSubKind::Mips CpuSubKind = CpuSubKind::Mips::r2000a;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MIPS_R2000A"sv;
        constexpr static auto FullName = "Mips R2000a"sv;
        constexpr static auto Description = "R2000a"sv;
    };

    template <Mach::CpuSubKind::Mc680x0>
    struct CpuSubKindMc680x0Info {};

    template <>
    struct CpuSubKindMc680x0Info<CpuSubKind::Mc680x0::All> {
        constexpr static CpuSubKind::Mc680x0 CpuSubKind =
            CpuSubKind::Mc680x0::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC680X0_All"sv;
        constexpr static auto FullName = "Motorola m68k All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindMc680x0Info<CpuSubKind::Mc680x0::n40> {
        constexpr static CpuSubKind::Mc680x0 CpuSubKind =
            CpuSubKind::Mc680x0::n40;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC68040"sv;
        constexpr static auto FullName = "Motorola 68040"sv;
        constexpr static auto Description = "68040"sv;
    };

    template <>
    struct CpuSubKindMc680x0Info<CpuSubKind::Mc680x0::n30Only> {
        constexpr static CpuSubKind::Mc680x0 CpuSubKind =
            CpuSubKind::Mc680x0::n30Only;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC68030_ONLY"sv;
        constexpr static auto FullName = "Motorola 68030 Only"sv;
        constexpr static auto Description = "68030 Only"sv;
    };

    template <Mach::CpuSubKind::Hppa>
    struct CpuSubKindHppaInfo {};

    template <>
    struct CpuSubKindHppaInfo<CpuSubKind::Hppa::All> {
        constexpr static CpuSubKind::Hppa CpuSubKind = CpuSubKind::Hppa::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_HPPA_ONLY"sv;
        constexpr static auto FullName = "Hppa All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindHppaInfo<CpuSubKind::Hppa::n7100LC> {
        constexpr static CpuSubKind::Hppa CpuSubKind =
            CpuSubKind::Hppa::n7100LC;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_HPPA_7100LC"sv;
        constexpr static auto FullName = "Hppa 7100LC"sv;
        constexpr static auto Description = "7100LC"sv;
    };

    template <Mach::CpuSubKind::Arm>
    struct CpuSubKindArmInfo {};

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::All> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_ALL"sv;
        constexpr static auto FullName = "Arm All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::A500Arch> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::A500Arch;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_A500_ARCH"sv;
        constexpr static auto FullName = "Arm A500 Arch"sv;
        constexpr static auto Description = "A500 Arch"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::A500> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::A500;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_A500"sv;
        constexpr static auto FullName = "Arm A500"sv;
        constexpr static auto Description = "A500"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::A440> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::A440;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_A440"sv;
        constexpr static auto FullName = "Arm A440"sv;
        constexpr static auto Description = "A440"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::m4> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::m4;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_M4"sv;
        constexpr static auto FullName = "Arm M4"sv;
        constexpr static auto Description = "M4"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v4T> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v4T;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V4"sv;
        constexpr static auto FullName = "Armv4t"sv;
        constexpr static auto Description = "V4T"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v6> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v6;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V6"sv;
        constexpr static auto FullName = "Armv6"sv;
        constexpr static auto Description = "V6"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v5tej> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v5tej;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V5TEJ"sv;
        constexpr static auto FullName = "Armv5"sv;
        constexpr static auto Description = "V5"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::Xscale> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::Xscale;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_XSCALE"sv;
        constexpr static auto FullName = "Arm Xscale"sv;
        constexpr static auto Description = "Xscale"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v7> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v7;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V7"sv;
        constexpr static auto FullName = "Armv7"sv;
        constexpr static auto Description = "V7"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v7f> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v7f;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V7"sv;
        constexpr static auto FullName = "Armv7f"sv;
        constexpr static auto Description = "V7F"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v7s> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v7s;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V7S"sv;
        constexpr static auto FullName = "Armv7s"sv;
        constexpr static auto Description = "V7S"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v7k> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v7s;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V7S"sv;
        constexpr static auto FullName = "Armv7s"sv;
        constexpr static auto Description = "V7S"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v6m> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v6m;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V6M"sv;
        constexpr static auto FullName = "Armv6m"sv;
        constexpr static auto Description = "V6M"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v7m> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v7m;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V7M"sv;
        constexpr static auto FullName = "Armv7m"sv;
        constexpr static auto Description = "V7M"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v7em> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v7em;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V7EM"sv;
        constexpr static auto FullName = "Armv7em"sv;
        constexpr static auto Description = "V7EM"sv;
    };

    template <>
    struct CpuSubKindArmInfo<CpuSubKind::Arm::v8> {
        constexpr static CpuSubKind::Arm CpuSubKind = CpuSubKind::Arm::v8;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V8"sv;
        constexpr static auto FullName = "Armv8"sv;
        constexpr static auto Description = "V8"sv;
    };

    template <Mach::CpuSubKind::Arm64>
    struct CpuSubKindArm64Info {};

    template <>
    struct CpuSubKindArm64Info<CpuSubKind::Arm64::All> {
        constexpr static CpuSubKind::Arm64 CpuSubKind = CpuSubKind::Arm64::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM_V8"sv;
        constexpr static auto FullName = "Arm64 All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindArm64Info<CpuSubKind::Arm64::v8> {
        constexpr static CpuSubKind::Arm64 CpuSubKind = CpuSubKind::Arm64::v8;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM64_V8"sv;
        constexpr static auto FullName = "Arm64v8"sv;
        constexpr static auto Description = "V8"sv;
    };

    template <>
    struct CpuSubKindArm64Info<CpuSubKind::Arm64::E> {
        constexpr static CpuSubKind::Arm64 CpuSubKind = CpuSubKind::Arm64::E;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM64E"sv;
        constexpr static auto FullName = "Arm64E"sv;
        constexpr static auto Description = "E"sv;
    };

    template <Mach::CpuSubKind::Arm64_32>
    struct CpuSubKindArm64_32Info {};

    template <>
    struct CpuSubKindArm64_32Info<CpuSubKind::Arm64_32::v8> {
        constexpr static CpuSubKind::Arm64_32 CpuSubKind =
            CpuSubKind::Arm64_32::v8;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_ARM64_32_V8"sv;
        constexpr static auto FullName = "Arm64_32 v8"sv;
        constexpr static auto Description = "V8"sv;
    };

    template <Mach::CpuSubKind::Mc88000>
    struct CpuSubKindMc88000Info {};

    template <>
    struct CpuSubKindMc88000Info<CpuSubKind::Mc88000::All> {
        constexpr static CpuSubKind::Mc88000 CpuSubKind =
            CpuSubKind::Mc88000::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC88000_ALL"sv;
        constexpr static auto FullName = "Motorola m88k All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindMc88000Info<CpuSubKind::Mc88000::n100> {
        constexpr static CpuSubKind::Mc88000 CpuSubKind =
            CpuSubKind::Mc88000::n100;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC88000_100"sv;
        constexpr static auto FullName = "Motorola 88100"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindMc88000Info<CpuSubKind::Mc88000::n110> {
        constexpr static CpuSubKind::Mc88000 CpuSubKind =
            CpuSubKind::Mc88000::n110;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC88000_110"sv;
        constexpr static auto FullName = "Motorola 88110"sv;
        constexpr static auto Description = "All"sv;
    };

    template <Mach::CpuSubKind::Sparc>
    struct CpuSubKindSparcInfo {};

    template <>
    struct CpuSubKindSparcInfo<CpuSubKind::Sparc::All> {
        constexpr static CpuSubKind::Sparc CpuSubKind = CpuSubKind::Sparc::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_SPARC_ALL"sv;
        constexpr static auto FullName = "Sparc All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <Mach::CpuSubKind::Mc98000>
    struct CpuSubKindMc98000Info {};

    template <>
    struct CpuSubKindMc98000Info<CpuSubKind::Mc98000::All> {
        constexpr static CpuSubKind::Mc98000 CpuSubKind =
            CpuSubKind::Mc98000::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC98000_ALL"sv;
        constexpr static auto FullName = "Motorola m98k All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindMc98000Info<CpuSubKind::Mc98000::n601> {
        constexpr static CpuSubKind::Mc98000 CpuSubKind =
            CpuSubKind::Mc98000::n601;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_MC98000_601"sv;
        constexpr static auto FullName = "Motorola mk98k 601"sv;
        constexpr static auto Description = "All"sv;
    };

    template <Mach::CpuSubKind::i860>
    struct CpuSubKindI860Info {};

    template <>
    struct CpuSubKindI860Info<CpuSubKind::i860::All> {
        constexpr static CpuSubKind::i860 CpuSubKind = CpuSubKind::i860::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_I860_ALL"sv;
        constexpr static auto FullName = "Intel i860 All"sv;
        constexpr static auto Description = "i860"sv;
    };

    template <>
    struct CpuSubKindI860Info<CpuSubKind::i860::n860> {
        constexpr static CpuSubKind::i860 CpuSubKind = CpuSubKind::i860::n860;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_I860_601"sv;
        constexpr static auto FullName = "Intel i860 601"sv;
        constexpr static auto Description = "601"sv;
    };

    template <Mach::CpuSubKind::i860Little>
    struct CpuSubKindI860LittleInfo {};

    template <>
    struct CpuSubKindI860LittleInfo<CpuSubKind::i860Little::All> {
        constexpr static CpuSubKind::i860Little CpuSubKind =
            CpuSubKind::i860Little::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_I860_LITTLE_ALL"sv;
        constexpr static auto FullName = "Intel i860 (Little-Endian)"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindI860LittleInfo<CpuSubKind::i860Little::Default> {
        constexpr static CpuSubKind::i860Little CpuSubKind =
            CpuSubKind::i860Little::Default;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_I860_LITTLE"sv;
        constexpr static auto FullName = "Intel i860 (Little-Endian)"sv;
        constexpr static auto Description = "All"sv;
    };

    template <Mach::CpuSubKind::Rs9600>
    struct CpuSubKindRs9600Info {};

    template <>
    struct CpuSubKindRs9600Info<CpuSubKind::Rs9600::All> {
        constexpr static CpuSubKind::Rs9600 CpuSubKind =
            CpuSubKind::Rs9600::All;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_RS9600_LITTLE_ALL"sv;
        constexpr static auto FullName = "Rs9600"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindRs9600Info<CpuSubKind::Rs9600::Default> {
        constexpr static CpuSubKind::i860Little CpuSubKind =
            CpuSubKind::i860Little::Default;

        constexpr static bool IsBigEndian = false;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_RS9600_LITTLE"sv;
        constexpr static auto FullName = "Rs9600"sv;
        constexpr static auto Description = "All"sv;
    };

    template <Mach::CpuSubKind::PowerPC>
    struct CpuSubKindPowerPCInfo {};

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::All> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::All;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_ALL"sv;
        constexpr static auto FullName = "PowerPC All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n601> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n601;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_601"sv;
        constexpr static auto FullName = "PowerPC 601"sv;
        constexpr static auto Description = "601"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n602> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n602;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_602"sv;
        constexpr static auto FullName = "PowerPC 602"sv;
        constexpr static auto Description = "602"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n603> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n603;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_603"sv;
        constexpr static auto FullName = "PowerPC 603"sv;
        constexpr static auto Description = "603"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n603e> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n603e;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_603E"sv;
        constexpr static auto FullName = "PowerPC 603e"sv;
        constexpr static auto Description = "603e"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n603ev> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n603ev;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_603EV"sv;
        constexpr static auto FullName = "PowerPC 603ev"sv;
        constexpr static auto Description = "603ev"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n604> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n604;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_604"sv;
        constexpr static auto FullName = "PowerPC 604"sv;
        constexpr static auto Description = "604"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n604e> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n604e;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_604E"sv;
        constexpr static auto FullName = "PowerPC 604e"sv;
        constexpr static auto Description = "604e"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n620> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n620;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_620"sv;
        constexpr static auto FullName = "PowerPC 620"sv;
        constexpr static auto Description = "620"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n750> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n750;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_750"sv;
        constexpr static auto FullName = "PowerPC 750"sv;
        constexpr static auto Description = "750"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n7400> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n7400;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_7400"sv;
        constexpr static auto FullName = "PowerPC 7400"sv;
        constexpr static auto Description = "7400"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n7500> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n7500;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_7500"sv;
        constexpr static auto FullName = "PowerPC 7500"sv;
        constexpr static auto Description = "7500"sv;
    };

    template <>
    struct CpuSubKindPowerPCInfo<CpuSubKind::PowerPC::n970> {
        constexpr static CpuSubKind::PowerPC CpuSubKind =
            CpuSubKind::PowerPC::n970;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_970"sv;
        constexpr static auto FullName = "PowerPC 970"sv;
        constexpr static auto Description = "970"sv;
    };

    template <Mach::CpuSubKind::PowerPC64>
    struct CpuSubKindPowerPC64Info {};

    template <>
    struct CpuSubKindPowerPC64Info<CpuSubKind::PowerPC64::All> {
        constexpr static CpuSubKind::PowerPC64 CpuSubKind =
            CpuSubKind::PowerPC64::All;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_ALL"sv;
        constexpr static auto FullName = "PowerPC (64-Bit) All"sv;
        constexpr static auto Description = "All"sv;
    };

    template <>
    struct CpuSubKindPowerPC64Info<CpuSubKind::PowerPC64::n970> {
        constexpr static CpuSubKind::PowerPC64 CpuSubKind =
            CpuSubKind::PowerPC64::n970;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_POWERPC_970"sv;
        constexpr static auto FullName = "PowerPC (64-Bit) 970"sv;
        constexpr static auto Description = "All"sv;
    };

    template <Mach::CpuSubKind::Veo>
    struct CpuSubKindVeoInfo {};

    template <>
    struct CpuSubKindVeoInfo<CpuSubKind::Veo::n1> {
        constexpr static CpuSubKind::Veo CpuSubKind = CpuSubKind::Veo::n1;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VEO_1"sv;
        constexpr static auto FullName = "Veo 1"sv;
        constexpr static auto Description = "1"sv;
    };

    template <>
    struct CpuSubKindVeoInfo<CpuSubKind::Veo::n2> {
        constexpr static CpuSubKind::Veo CpuSubKind = CpuSubKind::Veo::n2;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VEO_2"sv;
        constexpr static auto FullName = "Veo 2"sv;
        constexpr static auto Description = "2"sv;
    };

    template <>
    struct CpuSubKindVeoInfo<CpuSubKind::Veo::n3> {
        constexpr static CpuSubKind::Veo CpuSubKind = CpuSubKind::Veo::n3;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VEO_3"sv;
        constexpr static auto FullName = "Veo 3"sv;
        constexpr static auto Description = "3"sv;
    };

    template <>
    struct CpuSubKindVeoInfo<CpuSubKind::Veo::n4> {
        constexpr static CpuSubKind::Veo CpuSubKind = CpuSubKind::Veo::n4;

        constexpr static bool IsBigEndian = true;
        constexpr static bool IsEitherEndian = false;

        constexpr static auto Name = "CPU_SUBTYPE_VEO_4"sv;
        constexpr static auto FullName = "Veo 4"sv;
        constexpr static auto Description = "4"sv;
    };
}
