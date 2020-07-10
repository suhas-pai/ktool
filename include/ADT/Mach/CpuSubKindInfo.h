//
//  include/ADT/Mach/CpuSubKindInfo.h
//  stool
//
//  Created by Suhas Pai on 4/19/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "../DefaultStructVariable.h"

#include "CpuKindInfoTemplates.h"
#include "CpuSubKindInfoTemplates.h"
#include "CpuKindInfoTemplates.h"

#include "Info.h"
#include <string_view>

namespace Mach {
    struct CpuSubKindInfo {
        DefaultStructVariable<int32_t> SubType;

        DefaultStructVariable<std::string_view> Name;
        DefaultStructVariable<std::string_view> FullName;
        DefaultStructVariable<std::string_view> Description;

        DefaultStructVariable<bool> IsEitherEndian;
        DefaultStructVariable<bool> IsBigEndian;
    };

    struct CpuKindInfo {
        DefaultStructVariable<Mach::CpuKind> CpuKind;

        DefaultStructVariable<std::string_view> Name;
        DefaultStructVariable<std::string_view> BrandName;
        DefaultStructVariable<std::string_view> Description;

        DefaultStructVariable<const CpuSubKindInfo *> SubTypeTable;
    };

    using namespace CpuSubKind;

    constexpr struct CpuSubKindInfo AnyCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Any::Multiple),
            .Name = CpuSubKindAnyInfo<Any::Multiple>::Name,
            .FullName = CpuSubKindAnyInfo<Any::Multiple>::FullName,
            .Description = CpuSubKindAnyInfo<Any::Multiple>::Description,
            .IsEitherEndian = true,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Any::LittleEndian),
            .Name = CpuSubKindAnyInfo<Any::LittleEndian>::Name,
            .FullName = CpuSubKindAnyInfo<Any::LittleEndian>::FullName,
            .Description = CpuSubKindAnyInfo<Any::LittleEndian>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Any::BigEndian),
            .Name = CpuSubKindAnyInfo<Any::BigEndian>::Name,
            .FullName = CpuSubKindAnyInfo<Any::BigEndian>::FullName,
            .Description = CpuSubKindAnyInfo<Any::BigEndian>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true,
        }
    };

    constexpr struct CpuSubKindInfo VaxCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Vax::All),
            .Name = CpuSubKindVaxInfo<Vax::All>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::All>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n780),
            .Name = CpuSubKindVaxInfo<Vax::n780>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n780>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n780>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n785),
            .Name = CpuSubKindVaxInfo<Vax::n785>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n785>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n785>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n750),
            .Name = CpuSubKindVaxInfo<Vax::n750>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n750>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n750>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n730),
            .Name = CpuSubKindVaxInfo<Vax::n730>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n730>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n730>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::UvaXI),
            .Name = CpuSubKindVaxInfo<Vax::UvaXI>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::UvaXI>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::UvaXI>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8200),
            .Name = CpuSubKindVaxInfo<Vax::n8200>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n8200>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n8200>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8500),
            .Name = CpuSubKindVaxInfo<Vax::n8500>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n8500>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n8500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8650),
            .Name = CpuSubKindVaxInfo<Vax::n8650>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n8650>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n8650>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8800),
            .Name = CpuSubKindVaxInfo<Vax::n8500>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::n8500>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::n8500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::UvaXII),
            .Name = CpuSubKindVaxInfo<Vax::UvaXII>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::UvaXII>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::UvaXII>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::UvaXIII),
            .Name = CpuSubKindVaxInfo<Vax::UvaXIII>::Name,
            .FullName = CpuSubKindVaxInfo<Vax::UvaXIII>::FullName,
            .Description = CpuSubKindVaxInfo<Vax::UvaXIII>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        }
    };

    constexpr struct CpuSubKindInfo RompCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Romp::All),
            .Name = CpuSubKindRompInfo<Romp::All>::Name,
            .FullName = CpuSubKindRompInfo<Romp::All>::FullName,
            .Description = CpuSubKindRompInfo<Romp::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Romp::Pc),
            .Name = CpuSubKindRompInfo<Romp::Pc>::Name,
            .FullName = CpuSubKindRompInfo<Romp::Pc>::FullName,
            .Description = CpuSubKindRompInfo<Romp::Pc>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Romp::Apc),
            .Name = CpuSubKindRompInfo<Romp::Apc>::Name,
            .FullName = CpuSubKindRompInfo<Romp::Apc>::FullName,
            .Description = CpuSubKindRompInfo<Romp::Apc>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Romp::n135),
            .Name = CpuSubKindRompInfo<Romp::All>::Name,
            .FullName = CpuSubKindRompInfo<Romp::All>::FullName,
            .Description = CpuSubKindRompInfo<Romp::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo MmaxCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Mmax::All),
            .Name = CpuSubKindMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubKindMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubKindMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::Dpc),
            .Name = CpuSubKindMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubKindMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubKindMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::Sqt),
            .Name = CpuSubKindMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubKindMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubKindMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::ApcFPU),
            .Name = CpuSubKindMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubKindMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubKindMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::ApcFPA),
            .Name = CpuSubKindMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubKindMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubKindMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::Xpc),
            .Name = CpuSubKindMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubKindMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubKindMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo i386CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(i386::All),
            .Name = CpuSubKindi386Info<i386::All>::Name,
            .FullName = CpuSubKindi386Info<i386::All>::FullName,
            .Description = CpuSubKindi386Info<i386::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::n486),
            .Name = CpuSubKindi386Info<i386::n486>::Name,
            .FullName = CpuSubKindi386Info<i386::n486>::FullName,
            .Description = CpuSubKindi386Info<i386::n486>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::n486sx),
            .Name = CpuSubKindi386Info<i386::n486sx>::Name,
            .FullName = CpuSubKindi386Info<i386::n486sx>::FullName,
            .Description = CpuSubKindi386Info<i386::n486sx>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::Pentium),
            .Name = CpuSubKindi386Info<i386::Pentium>::Name,
            .FullName = CpuSubKindi386Info<i386::Pentium>::FullName,
            .Description = CpuSubKindi386Info<i386::Pentium>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::PentiumPro),
            .Name = CpuSubKindi386Info<i386::PentiumPro>::Name,
            .FullName = CpuSubKindi386Info<i386::PentiumPro>::FullName,
            .Description = CpuSubKindi386Info<i386::PentiumPro>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::PentiumIIM3),
            .Name = CpuSubKindi386Info<i386::PentiumIIM3>::Name,
            .FullName = CpuSubKindi386Info<i386::PentiumIIM3>::FullName,
            .Description = CpuSubKindi386Info<i386::PentiumIIM3>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::PentiumIIM5),
            .Name = CpuSubKindi386Info<i386::PentiumIIM5>::Name,
            .FullName = CpuSubKindi386Info<i386::PentiumIIM5>::FullName,
            .Description = CpuSubKindi386Info<i386::PentiumIIM5>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::Pentium4),
            .Name = CpuSubKindi386Info<i386::Pentium4>::Name,
            .FullName = CpuSubKindi386Info<i386::Pentium4>::FullName,
            .Description = CpuSubKindi386Info<i386::Pentium4>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo x86_64CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(x86_64::All),
            .Name = CpuSubKindX86_64Info<x86_64::All>::Name,
            .FullName = CpuSubKindX86_64Info<x86_64::All>::FullName,
            .Description = CpuSubKindX86_64Info<x86_64::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(x86_64::Haswell),
            .Name = CpuSubKindX86_64Info<x86_64::Haswell>::Name,
            .FullName = CpuSubKindX86_64Info<x86_64::Haswell>::FullName,
            .Description = CpuSubKindX86_64Info<x86_64::Haswell>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo MipsCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Mips::All),
            .Name = CpuSubKindMipsInfo<Mips::All>::Name,
            .FullName = CpuSubKindMipsInfo<Mips::All>::FullName,
            .Description = CpuSubKindMipsInfo<Mips::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2300),
            .Name = CpuSubKindMipsInfo<Mips::r2300>::Name,
            .FullName = CpuSubKindMipsInfo<Mips::r2300>::FullName,
            .Description = CpuSubKindMipsInfo<Mips::r2300>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2600),
            .Name = CpuSubKindMipsInfo<Mips::r2600>::Name,
            .FullName = CpuSubKindMipsInfo<Mips::r2600>::FullName,
            .Description = CpuSubKindMipsInfo<Mips::r2600>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2800),
            .Name = CpuSubKindMipsInfo<Mips::r2800>::Name,
            .FullName = CpuSubKindMipsInfo<Mips::r2800>::FullName,
            .Description = CpuSubKindMipsInfo<Mips::r2800>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2000a),
            .Name = CpuSubKindMipsInfo<Mips::r2000a>::Name,
            .FullName = CpuSubKindMipsInfo<Mips::r2000a>::FullName,
            .Description = CpuSubKindMipsInfo<Mips::r2000a>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo Mc680x0CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Mc680x0::All),
            .Name = CpuSubKindMc680x0Info<Mc680x0::All>::Name,
            .FullName = CpuSubKindMc680x0Info<Mc680x0::All>::FullName,
            .Description = CpuSubKindMc680x0Info<Mc680x0::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc680x0::n30),
            .Name = CpuSubKindMc680x0Info<Mc680x0::n30>::Name,
            .FullName = CpuSubKindMc680x0Info<Mc680x0::n30>::FullName,
            .Description = CpuSubKindMc680x0Info<Mc680x0::n30>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc680x0::n40),
            .Name = CpuSubKindMc680x0Info<Mc680x0::n40>::Name,
            .FullName = CpuSubKindMc680x0Info<Mc680x0::n40>::FullName,
            .Description = CpuSubKindMc680x0Info<Mc680x0::n40>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc680x0::n30Only),
            .Name = CpuSubKindMc680x0Info<Mc680x0::n30Only>::Name,
            .FullName = CpuSubKindMc680x0Info<Mc680x0::n30Only>::FullName,
            .Description = CpuSubKindMc680x0Info<Mc680x0::n30Only>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubKindInfo HppaCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Hppa::All),
            .Name = CpuSubKindHppaInfo<Hppa::All>::Name,
            .FullName = CpuSubKindHppaInfo<Hppa::All>::FullName,
            .Description = CpuSubKindHppaInfo<Hppa::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Hppa::n7100LC),
            .Name = CpuSubKindHppaInfo<Hppa::n7100LC>::Name,
            .FullName = CpuSubKindHppaInfo<Hppa::n7100LC>::FullName,
            .Description = CpuSubKindHppaInfo<Hppa::n7100LC>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo ArmCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Arm::All),
            .Name = CpuSubKindArmInfo<Arm::All>::Name,
            .FullName = CpuSubKindArmInfo<Arm::All>::FullName,
            .Description = CpuSubKindArmInfo<Arm::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::A500Arch),
            .Name = CpuSubKindArmInfo<Arm::A500Arch>::Name,
            .FullName = CpuSubKindArmInfo<Arm::A500Arch>::FullName,
            .Description = CpuSubKindArmInfo<Arm::A500Arch>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::A500),
            .Name = CpuSubKindArmInfo<Arm::A500>::Name,
            .FullName = CpuSubKindArmInfo<Arm::A500>::FullName,
            .Description = CpuSubKindArmInfo<Arm::A500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::A440),
            .Name = CpuSubKindArmInfo<Arm::A440>::Name,
            .FullName = CpuSubKindArmInfo<Arm::A440>::FullName,
            .Description = CpuSubKindArmInfo<Arm::A440>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::m4),
            .Name = CpuSubKindArmInfo<Arm::m4>::Name,
            .FullName = CpuSubKindArmInfo<Arm::m4>::FullName,
            .Description = CpuSubKindArmInfo<Arm::m4>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v4T),
            .Name = CpuSubKindArmInfo<Arm::v4T>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v4T>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v4T>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v6),
            .Name = CpuSubKindArmInfo<Arm::v6>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v6>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v6>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v5tej),
            .Name = CpuSubKindArmInfo<Arm::v5tej>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v5tej>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v5tej>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::Xscale),
            .Name = CpuSubKindArmInfo<Arm::Xscale>::Name,
            .FullName = CpuSubKindArmInfo<Arm::Xscale>::FullName,
            .Description = CpuSubKindArmInfo<Arm::Xscale>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7),
            .Name = CpuSubKindArmInfo<Arm::v7>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v7>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v7>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7f),
            .Name = CpuSubKindArmInfo<Arm::v7f>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v7f>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v7f>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7s),
            .Name = CpuSubKindArmInfo<Arm::v7s>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v7s>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v7s>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7k),
            .Name = CpuSubKindArmInfo<Arm::v7k>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v7k>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v7k>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v6m),
            .Name = CpuSubKindArmInfo<Arm::v6m>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v6m>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v6m>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7m),
            .Name = CpuSubKindArmInfo<Arm::v7m>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v7m>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v7m>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7em),
            .Name = CpuSubKindArmInfo<Arm::v7em>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v7em>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v7em>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v8),
            .Name = CpuSubKindArmInfo<Arm::v8>::Name,
            .FullName = CpuSubKindArmInfo<Arm::v8>::FullName,
            .Description = CpuSubKindArmInfo<Arm::v8>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
    };

    constexpr struct CpuSubKindInfo Arm64CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Arm64::All),
            .Name = CpuSubKindArm64Info<Arm64::All>::Name,
            .FullName = CpuSubKindArm64Info<Arm64::All>::FullName,
            .Description = CpuSubKindArm64Info<Arm64::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm64::v8),
            .Name = CpuSubKindArm64Info<Arm64::v8>::Name,
            .FullName = CpuSubKindArm64Info<Arm64::v8>::FullName,
            .Description = CpuSubKindArm64Info<Arm64::v8>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm64::E),
            .Name = CpuSubKindArm64Info<Arm64::E>::Name,
            .FullName = CpuSubKindArm64Info<Arm64::E>::FullName,
            .Description = CpuSubKindArm64Info<Arm64::E>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo Arm64_32CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Arm64_32::v8),
            .Name = CpuSubKindArm64_32Info<Arm64_32::v8>::Name,
            .FullName = CpuSubKindArm64_32Info<Arm64_32::v8>::FullName,
            .Description = CpuSubKindArm64_32Info<Arm64_32 ::v8>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo Mc88000CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Mc88000::All),
            .Name = CpuSubKindMc88000Info<Mc88000::All>::Name,
            .FullName = CpuSubKindMc88000Info<Mc88000::All>::FullName,
            .Description = CpuSubKindMc88000Info<Mc88000::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc88000::n100),
            .Name = CpuSubKindMc88000Info<Mc88000::n100>::Name,
            .FullName = CpuSubKindMc88000Info<Mc88000::n100>::FullName,
            .Description = CpuSubKindMc88000Info<Mc88000::n100>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc88000::n110),
            .Name = CpuSubKindMc88000Info<Mc88000::n110>::Name,
            .FullName = CpuSubKindMc88000Info<Mc88000::n110>::FullName,
            .Description = CpuSubKindMc88000Info<Mc88000::n110>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubKindInfo Mc98000CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Mc98000::All),
            .Name = CpuSubKindMc98000Info<Mc98000::All>::Name,
            .FullName = CpuSubKindMc98000Info<Mc98000::All>::FullName,
            .Description = CpuSubKindMc98000Info<Mc98000::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc98000::n601),
            .Name = CpuSubKindMc98000Info<Mc98000::n601>::Name,
            .FullName = CpuSubKindMc98000Info<Mc98000::n601>::FullName,
            .Description = CpuSubKindMc98000Info<Mc98000::n601>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubKindInfo i860CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(i860::All),
            .Name = CpuSubKindI860Info<i860::All>::Name,
            .FullName = CpuSubKindI860Info<i860::All>::FullName,
            .Description = CpuSubKindI860Info<i860::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(i860::n860),
            .Name = CpuSubKindI860Info<i860::n860>::Name,
            .FullName = CpuSubKindI860Info<i860::n860>::FullName,
            .Description = CpuSubKindI860Info<i860::n860>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubKindInfo i860LittleEndianCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(i860Little::All),
            .Name = CpuSubKindI860LittleInfo<i860Little::All>::Name,
            .FullName = CpuSubKindI860LittleInfo<i860Little::All>::FullName,
            .Description = CpuSubKindI860LittleInfo<i860Little::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i860Little::Default),
            .Name = CpuSubKindI860LittleInfo<i860Little::Default>::Name,
            .FullName = CpuSubKindI860LittleInfo<i860Little::Default>::FullName,
            .Description =
                CpuSubKindI860LittleInfo<i860Little::Default>::Description,

            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo RS9600CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Rs9600::All),
            .Name = CpuSubKindRs9600Info<Rs9600::All>::Name,
            .FullName = CpuSubKindRs9600Info<Rs9600::All>::FullName,
            .Description = CpuSubKindRs9600Info<Rs9600::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Rs9600::Default),
            .Name = CpuSubKindRs9600Info<Rs9600::Default>::Name,
            .FullName = CpuSubKindRs9600Info<Rs9600::Default>::FullName,
            .Description = CpuSubKindRs9600Info<Rs9600::Default>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo SparcCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Sparc::All),
            .Name = CpuSubKindSparcInfo<Sparc::All>::Name,
            .FullName = CpuSubKindSparcInfo<Sparc::All>::FullName,
            .Description = CpuSubKindSparcInfo<Sparc::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubKindInfo PowerPCCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(PowerPC::All),
            .Name = CpuSubKindPowerPCInfo<PowerPC::All>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::All>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n601),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n601>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n601>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n601>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n602),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n602>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n602>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n602>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n603),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n603>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n603>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n603>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n603e),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n603e>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n603e>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n603e>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n603ev),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n603ev>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n603ev>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n603ev>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n604),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n604>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n604>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n604>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n604e),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n604e>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n604e>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n604e>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n620),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n620>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n620>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n620>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n750),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n750>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n750>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n750>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n7400),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n7400>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n7400>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n7400>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n7500),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n7500>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n7500>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n7500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n970),
            .Name = CpuSubKindPowerPCInfo<PowerPC::n970>::Name,
            .FullName = CpuSubKindPowerPCInfo<PowerPC::n970>::FullName,
            .Description = CpuSubKindPowerPCInfo<PowerPC::n970>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubKindInfo PowerPC64CpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(PowerPC64::All),
            .Name = CpuSubKindPowerPC64Info<PowerPC64::All>::Name,
            .FullName = CpuSubKindPowerPC64Info<PowerPC64::All>::FullName,
            .Description = CpuSubKindPowerPC64Info<PowerPC64::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC64::n970),
            .Name = CpuSubKindPowerPC64Info<PowerPC64::n970>::Name,
            .FullName = CpuSubKindPowerPC64Info<PowerPC64::n970>::FullName,
            .Description =
                CpuSubKindPowerPC64Info<PowerPC64::n970>::Description,

            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubKindInfo VeoCpuSubKindTable[] = {
        {
            .SubType = static_cast<int32_t>(Veo::n1),
            .Name = CpuSubKindVeoInfo<Veo::n1>::Name,
            .FullName = CpuSubKindVeoInfo<Veo::n1>::FullName,
            .Description = CpuSubKindVeoInfo<Veo::n1>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Veo::n2),
            .Name = CpuSubKindVeoInfo<Veo::n2>::Name,
            .FullName = CpuSubKindVeoInfo<Veo::n2>::FullName,
            .Description = CpuSubKindVeoInfo<Veo::n2>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Veo::n3),
            .Name = CpuSubKindVeoInfo<Veo::n3>::Name,
            .FullName = CpuSubKindVeoInfo<Veo::n3>::FullName,
            .Description = CpuSubKindVeoInfo<Veo::n3>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Veo::n4),
            .Name = CpuSubKindVeoInfo<Veo::n4>::Name,
            .FullName = CpuSubKindVeoInfo<Veo::n4>::FullName,
            .Description = CpuSubKindVeoInfo<Veo::n4>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuKindInfo CpuKindInfoTable[] = {
        {
            .CpuKind = CpuKind::Any,
            .Name = CpuKindTemplateInfo<CpuKind::Any>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Any>::Name,
            .Description = CpuKindTemplateInfo<CpuKind::Any>::Name,
            .SubTypeTable = AnyCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Vax,
            .Name = CpuKindTemplateInfo<CpuKind::Vax>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Vax>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Vax>::Description,
            .SubTypeTable = VaxCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Romp,
            .Name = CpuKindTemplateInfo<CpuKind::Romp>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Romp>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Romp>::Description,
            .SubTypeTable = RompCpuSubKindTable,
        },
        {
            .CpuKind = CpuKind::NS32032,
            .Name = CpuKindTemplateInfo<CpuKind::NS32032>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::NS32032>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::NS32032>::Description,
            .SubTypeTable = MmaxCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::NS32332,
            .Name = CpuKindTemplateInfo<CpuKind::NS32332>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::NS32332>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::NS32332>::Description,
            .SubTypeTable = MmaxCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Mc680x0,
            .Name = CpuKindTemplateInfo<CpuKind::Mc680x0>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Mc680x0>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Mc680x0>::Description,
            .SubTypeTable = Mc680x0CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::i386,
            .Name = CpuKindTemplateInfo<CpuKind::i386>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::i386>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::i386>::Description,
            .SubTypeTable = i386CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::x86_64,
            .Name = CpuKindTemplateInfo<CpuKind::x86_64>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::x86_64>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::x86_64>::Description,
            .SubTypeTable = x86_64CpuSubKindTable,
        },
        {
            .CpuKind = CpuKind::Mips,
            .Name = CpuKindTemplateInfo<CpuKind::Mips>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Mips>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Mips>::Description,
            .SubTypeTable = MipsCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::NS32532,
            .Name = CpuKindTemplateInfo<CpuKind::NS32532>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::NS32532>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::NS32532>::Description,
            .SubTypeTable = MmaxCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Hppa,
            .Name = CpuKindTemplateInfo<CpuKind::Hppa>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Hppa>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Hppa>::Description,
            .SubTypeTable = HppaCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Arm,
            .Name = CpuKindTemplateInfo<CpuKind::Arm>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Arm>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Arm>::Description,
            .SubTypeTable = ArmCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Mc88000,
            .Name = CpuKindTemplateInfo<CpuKind::Mc88000>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Mc88000>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Mc88000>::Description,
            .SubTypeTable = Mc88000CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Sparc,
            .Name = CpuKindTemplateInfo<CpuKind::Sparc>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Sparc>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Sparc>::Description,
            .SubTypeTable = SparcCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::i860,
            .Name = CpuKindTemplateInfo<CpuKind::i860>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::i860>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::i860>::Description,
            .SubTypeTable = i860CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::i860Little,
            .Name = CpuKindTemplateInfo<CpuKind::i860Little>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::i860Little>::BrandName,
            .Description =
                CpuKindTemplateInfo<CpuKind::i860Little>::Description,
            .SubTypeTable = i860LittleEndianCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Rs9600,
            .Name = CpuKindTemplateInfo<CpuKind::Rs9600>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Rs9600>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Rs9600>::Description,
            .SubTypeTable = RS9600CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Mc98000,
            .Name = CpuKindTemplateInfo<CpuKind::Mc98000>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Mc98000>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Mc98000>::Description,
            .SubTypeTable = Mc98000CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::PowerPC,
            .Name = CpuKindTemplateInfo<CpuKind::PowerPC>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::PowerPC>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::PowerPC>::Description,
            .SubTypeTable = PowerPCCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::PowerPC64,
            .Name = CpuKindTemplateInfo<CpuKind::PowerPC64>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::PowerPC64>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::PowerPC64>::Description,
            .SubTypeTable = PowerPC64CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Veo,
            .Name = CpuKindTemplateInfo<CpuKind::Veo>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Veo>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Veo>::Description,
            .SubTypeTable = VeoCpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Arm64,
            .Name = CpuKindTemplateInfo<CpuKind::Arm64>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Arm64>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Arm64>::Description,
            .SubTypeTable = Arm64CpuSubKindTable
        },
        {
            .CpuKind = CpuKind::Arm64_32,
            .Name = CpuKindTemplateInfo<CpuKind::Arm64_32>::Name,
            .BrandName = CpuKindTemplateInfo<CpuKind::Arm64_32>::BrandName,
            .Description = CpuKindTemplateInfo<CpuKind::Arm64_32>::Description,
            .SubTypeTable = Arm64_32CpuSubKindTable
        }
    };

    [[nodiscard]]
    constexpr const CpuKindInfo *GetInfoForCpuKind(CpuKind CpuKind) noexcept {
        switch (CpuKind) {
            case CpuKind::Any:
                return (CpuKindInfoTable + 0);
            case CpuKind::Vax:
                return (CpuKindInfoTable + 1);
            case CpuKind::Romp:
                return (CpuKindInfoTable + 2);
            case CpuKind::NS32032:
                return (CpuKindInfoTable + 3);
            case CpuKind::NS32332:
                return (CpuKindInfoTable + 4);
            case CpuKind::Mc680x0:
                return (CpuKindInfoTable + 5);
            case CpuKind::i386:
                return (CpuKindInfoTable + 6);
            case CpuKind::x86_64:
                return (CpuKindInfoTable + 7);
            case CpuKind::Mips:
                return (CpuKindInfoTable + 8);
            case CpuKind::NS32532:
                return (CpuKindInfoTable + 9);
            case CpuKind::Hppa:
                return (CpuKindInfoTable + 10);
            case CpuKind::Arm:
                return (CpuKindInfoTable + 11);
            case CpuKind::Mc88000:
                return (CpuKindInfoTable + 12);
            case CpuKind::Sparc:
                return (CpuKindInfoTable + 13);
            case CpuKind::i860:
                return (CpuKindInfoTable + 14);
            case CpuKind::i860Little:
                return (CpuKindInfoTable + 15);
            case CpuKind::Rs9600:
                return (CpuKindInfoTable + 16);
            case CpuKind::Mc98000:
                return (CpuKindInfoTable + 17);
            case CpuKind::PowerPC:
                return (CpuKindInfoTable + 18);
            case CpuKind::PowerPC64:
                return (CpuKindInfoTable + 19);
            case CpuKind::Veo:
                return (CpuKindInfoTable + 20);
            case CpuKind::Arm64:
                return (CpuKindInfoTable + 21);
            case CpuKind::Arm64_32:
                return (CpuKindInfoTable + 22);
        }

        return nullptr;
    }

    [[nodiscard]] constexpr const CpuSubKindInfo *
    GetInfoForCpuSubKind(CpuKind CpuKind, int32_t SubType) noexcept {
        switch (CpuKind) {
            case CpuKind::Any:
                switch (Any(SubType)) {
                    case Any::Multiple:
                        return (AnyCpuSubKindTable + 0);

                    case Any::LittleEndian:
                        return (AnyCpuSubKindTable + 1);

                    case Any::BigEndian:
                        return (AnyCpuSubKindTable + 2);
                }

                break;

            case CpuKind::Vax:
                switch (Vax(SubType)) {
                    case Vax::All:
                      return (VaxCpuSubKindTable + 0);
                    case Vax::n780:
                      return (VaxCpuSubKindTable + 1);
                    case Vax::n785:
                      return (VaxCpuSubKindTable + 2);
                    case Vax::n750:
                      return (VaxCpuSubKindTable + 3);
                    case Vax::n730:
                      return (VaxCpuSubKindTable + 4);
                    case Vax::UvaXI:
                      return (VaxCpuSubKindTable + 5);
                    case Vax::UvaXII:
                      return (VaxCpuSubKindTable + 6);
                    case Vax::n8200:
                      return (VaxCpuSubKindTable + 7);
                    case Vax::n8500:
                      return (VaxCpuSubKindTable + 8);
                    case Vax::n8600:
                      return (VaxCpuSubKindTable + 9);
                    case Vax::n8650:
                      return (VaxCpuSubKindTable + 10);
                    case Vax::n8800:
                      return (VaxCpuSubKindTable + 11);
                    case Vax::UvaXIII:
                      return (VaxCpuSubKindTable + 12);
                }

                break;

            case CpuKind::Romp:
                switch (Romp(SubType)) {
                    case Romp::All:
                        return (RompCpuSubKindTable + 0);
                    case Romp::Pc:
                        return (RompCpuSubKindTable + 1);
                    case Romp::Apc:
                        return (RompCpuSubKindTable + 2);
                    case Romp::n135:
                        return (RompCpuSubKindTable + 3);
                }

                break;

            case CpuKind::NS32032:
            case CpuKind::NS32332:
            case CpuKind::NS32532:
                switch (Mmax(SubType)) {
                    case Mmax::All:
                        return (MmaxCpuSubKindTable + 0);
                    case Mmax::Dpc:
                        return (MmaxCpuSubKindTable + 0);
                    case Mmax::Sqt:
                        return (MmaxCpuSubKindTable + 0);
                    case Mmax::ApcFPU:
                        return (MmaxCpuSubKindTable + 0);
                    case Mmax::ApcFPA:
                        return (MmaxCpuSubKindTable + 0);
                    case Mmax::Xpc:
                        return (MmaxCpuSubKindTable + 0);
                }

                break;

            case CpuKind::Mc680x0: {
                switch (Mc680x0(SubType)) {
                    case Mc680x0::All:
                        return (Mc680x0CpuSubKindTable + 0);
                    case Mc680x0::n40:
                        return (Mc680x0CpuSubKindTable + 1);
                    case Mc680x0::n30Only:
                        return (Mc680x0CpuSubKindTable + 2);
                }

                break;
            }

            case CpuKind::i386:
                switch (i386(SubType)) {
                    case i386::All:
                        return (i386CpuSubKindTable + 0);
                    case i386::n486:
                        return (i386CpuSubKindTable + 1);
                    case i386::n486sx:
                        return (i386CpuSubKindTable + 2);
                    case i386::Pentium:
                        return (i386CpuSubKindTable + 3);
                    case i386::PentiumPro:
                        return (i386CpuSubKindTable + 4);
                    case i386::PentiumIIM3:
                        return (i386CpuSubKindTable + 5);
                    case i386::PentiumIIM5:
                        return (i386CpuSubKindTable + 6);
                    case i386::Pentium4:
                        return (i386CpuSubKindTable + 7);
                }

                break;

            case CpuKind::x86_64:
                switch (x86_64(SubType)) {
                    case x86_64::All:
                        return (x86_64CpuSubKindTable + 0);

                    case x86_64::Haswell:
                        return (x86_64CpuSubKindTable + 1);
                }

                break;

            case CpuKind::Mips:
                switch (Mips(SubType)) {
                    case Mips::All:
                        return (MipsCpuSubKindTable + 0);
                    case Mips::r2300:
                        return (MipsCpuSubKindTable + 1);
                    case Mips::r2600:
                        return (MipsCpuSubKindTable + 1);
                    case Mips::r2800:
                        return (MipsCpuSubKindTable + 1);
                    case Mips::r2000a:
                        return (MipsCpuSubKindTable + 1);
                }

                break;

            case CpuKind::Hppa:
                switch (Hppa(SubType)) {
                    case Hppa::All:
                        return (HppaCpuSubKindTable + 0);
                    case Hppa::n7100LC:
                        return (HppaCpuSubKindTable + 1);
                }

                break;

            case CpuKind::Arm:
                switch (Arm(SubType)) {
                    case Arm::All:
                        return (ArmCpuSubKindTable + 0);
                    case Arm::A500Arch:
                        return (ArmCpuSubKindTable + 1);
                    case Arm::A500:
                        return (ArmCpuSubKindTable + 2);
                    case Arm::A440:
                        return (ArmCpuSubKindTable + 3);
                    case Arm::m4:
                        return (ArmCpuSubKindTable + 4);
                    case Arm::v4T:
                        return (ArmCpuSubKindTable + 5);
                    case Arm::v6:
                        return (ArmCpuSubKindTable + 6);
                    case Arm::v5tej:
                        return (ArmCpuSubKindTable + 7);
                    case Arm::Xscale:
                        return (ArmCpuSubKindTable + 8);
                    case Arm::v7:
                        return (ArmCpuSubKindTable + 9);
                    case Arm::v7f:
                        return (ArmCpuSubKindTable + 10);
                    case Arm::v7s:
                        return (ArmCpuSubKindTable + 11);
                    case Arm::v7k:
                        return (ArmCpuSubKindTable + 12);
                    case Arm::v6m:
                        return (ArmCpuSubKindTable + 13);
                    case Arm::v7m:
                        return (ArmCpuSubKindTable + 14);
                    case Arm::v7em:
                        return (ArmCpuSubKindTable + 15);
                    case Arm::v8:
                        return (ArmCpuSubKindTable + 16);
                }

                break;

            case CpuKind::Mc88000:
                switch (Mc88000(SubType)) {
                    case Mc88000::All:
                        return (Mc88000CpuSubKindTable + 0);
                    case Mc88000::n100:
                        return (Mc88000CpuSubKindTable + 1);
                    case Mc88000::n110:
                        return (Mc88000CpuSubKindTable + 2);
                }

                break;

            case CpuKind::Sparc:
                switch (Sparc(SubType)) {
                    case Sparc::All:
                        return (SparcCpuSubKindTable + 0);
                }

                break;

            case CpuKind::i860:
                switch (i860(SubType)) {
                    case i860::All:
                        return (i860CpuSubKindTable + 0);
                    case i860::n860:
                        return (i860CpuSubKindTable + 1);
                }

                break;

            case CpuKind::i860Little:
                switch (i860Little(SubType)) {
                    case i860Little::All:
                        return (i860LittleEndianCpuSubKindTable + 0);
                    case i860Little::Default:
                        return (i860LittleEndianCpuSubKindTable + 1);
                }

                break;

            case CpuKind::Rs9600:
                switch (Rs9600(SubType)) {
                    case Rs9600::All:
                        return (RS9600CpuSubKindTable + 0);
                    case Rs9600::Default:
                        return (RS9600CpuSubKindTable + 1);
                }

                break;

            case CpuKind::Mc98000:
                switch (Mc98000(SubType)) {
                    case Mc98000::All:
                        return (Mc98000CpuSubKindTable + 0);
                    case Mc98000::n601:
                        return (Mc98000CpuSubKindTable + 1);
                }

                break;

            case CpuKind::PowerPC:
                switch (PowerPC(SubType)) {
                    case PowerPC::All:
                        return (PowerPCCpuSubKindTable + 0);
                    case PowerPC::n601:
                        return (PowerPCCpuSubKindTable + 1);
                    case PowerPC::n602:
                        return (PowerPCCpuSubKindTable + 2);
                    case PowerPC::n603:
                        return (PowerPCCpuSubKindTable + 3);
                    case PowerPC::n603e:
                        return (PowerPCCpuSubKindTable + 4);
                    case PowerPC::n603ev:
                        return (PowerPCCpuSubKindTable + 5);
                    case PowerPC::n604:
                        return (PowerPCCpuSubKindTable + 6);
                    case PowerPC::n604e:
                        return (PowerPCCpuSubKindTable + 7);
                    case PowerPC::n620:
                        return (PowerPCCpuSubKindTable + 8);
                    case PowerPC::n750:
                        return (PowerPCCpuSubKindTable + 9);
                    case PowerPC::n7400:
                        return (PowerPCCpuSubKindTable + 10);
                    case PowerPC::n7500:
                        return (PowerPCCpuSubKindTable + 11);
                    case PowerPC::n970:
                        return (PowerPCCpuSubKindTable + 12);
                }

                break;

            case CpuKind::PowerPC64:
                switch (PowerPC64(SubType)) {
                    case PowerPC64::All:
                        return (PowerPC64CpuSubKindTable + 0);
                    case PowerPC64::n970:
                        return (PowerPC64CpuSubKindTable + 1);
                }

                break;

            case CpuKind::Veo:
                switch (Veo(SubType)) {
                    case Veo::n1:
                        return (VeoCpuSubKindTable + 0);
                    case Veo::n2:
                        return (VeoCpuSubKindTable + 1);
                    case Veo::n3:
                        return (VeoCpuSubKindTable + 2);
                    case Veo::n4:
                        return (VeoCpuSubKindTable + 3);
                }

            case CpuKind::Arm64:
                switch (Arm64(SubType)) {
                    case Arm64::All:
                        return (Arm64CpuSubKindTable + 0);
                    case Arm64::v8:
                        return (Arm64CpuSubKindTable + 1);
                    case Arm64::E:
                        return (Arm64CpuSubKindTable + 2);
                }

                break;

            case CpuKind::Arm64_32:
                switch (Arm64_32(SubType)) {
                    case Arm64_32::v8:
                        return (Arm64_32CpuSubKindTable + 0);
                }

                break;
        }

        return nullptr;
    }

    namespace CpuSubKind {
        constexpr inline std::string_view EmptyStringView = std::string_view();

        [[nodiscard]] constexpr const std::string_view &
        GetName(CpuKind CpuKind, int32_t CpuSubKind) noexcept {
            if (auto *Info = GetInfoForCpuSubKind(CpuKind, CpuSubKind)) {
                return Info->Name;
            }

            return EmptyStringView;
        }

        [[nodiscard]] constexpr const std::string_view &
        GetFullName(CpuKind CpuKind, int32_t CpuSubKind) noexcept {
            if (auto *Info = GetInfoForCpuSubKind(CpuKind, CpuSubKind)) {
                return Info->FullName;
            }

            return EmptyStringView;
        }

        [[nodiscard]] constexpr const std::string_view &
        GetDescription(CpuKind CpuKind, int32_t CpuSubKind) noexcept {
            if (auto *Info = GetInfoForCpuSubKind(CpuKind, CpuSubKind)) {
                return Info->Description;
            }

            return EmptyStringView;
        }
    }
}
