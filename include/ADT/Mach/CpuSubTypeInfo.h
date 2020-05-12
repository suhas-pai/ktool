//
//  include/ADT/Mach/CpuSubTypeInfo.h
//  stool
//
//  Created by Suhas Pai on 4/19/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "../DefaultStructVariable.h"

#include "CpuTypeInfoTemplates.h"
#include "CpuSubTypeInfoTemplates.h"
#include "CpuTypeInfoTemplates.h"

#include "Info.h"
#include <string_view>

namespace Mach {
    struct CpuSubTypeInfo {
        DefaultStructVariable<int32_t> SubType;

        DefaultStructVariable<std::string_view> Name;
        DefaultStructVariable<std::string_view> FullName;
        DefaultStructVariable<std::string_view> Description;

        DefaultStructVariable<bool> IsEitherEndian;
        DefaultStructVariable<bool> IsBigEndian;
    };

    struct CpuTypeInfo {
        DefaultStructVariable<Mach::CpuType> CpuType;

        DefaultStructVariable<std::string_view> Name;
        DefaultStructVariable<std::string_view> BrandName;
        DefaultStructVariable<std::string_view> Description;

        DefaultStructVariable<const CpuSubTypeInfo *> SubTypeTable;
    };

    using namespace CpuSubType;

    constexpr struct CpuSubTypeInfo AnyCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Any::Multiple),
            .Name = CpuSubTypeAnyInfo<Any::Multiple>::Name,
            .FullName = CpuSubTypeAnyInfo<Any::Multiple>::FullName,
            .Description = CpuSubTypeAnyInfo<Any::Multiple>::Description,
            .IsEitherEndian = true,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Any::LittleEndian),
            .Name = CpuSubTypeAnyInfo<Any::LittleEndian>::Name,
            .FullName = CpuSubTypeAnyInfo<Any::LittleEndian>::FullName,
            .Description = CpuSubTypeAnyInfo<Any::LittleEndian>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Any::BigEndian),
            .Name = CpuSubTypeAnyInfo<Any::BigEndian>::Name,
            .FullName = CpuSubTypeAnyInfo<Any::BigEndian>::FullName,
            .Description = CpuSubTypeAnyInfo<Any::BigEndian>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true,
        }
    };

    constexpr struct CpuSubTypeInfo VaxCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Vax::All),
            .Name = CpuSubTypeVaxInfo<Vax::All>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::All>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n780),
            .Name = CpuSubTypeVaxInfo<Vax::n780>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n780>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n780>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n785),
            .Name = CpuSubTypeVaxInfo<Vax::n785>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n785>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n785>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n750),
            .Name = CpuSubTypeVaxInfo<Vax::n750>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n750>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n750>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n730),
            .Name = CpuSubTypeVaxInfo<Vax::n730>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n730>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n730>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::UvaXI),
            .Name = CpuSubTypeVaxInfo<Vax::UvaXI>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::UvaXI>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::UvaXI>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8200),
            .Name = CpuSubTypeVaxInfo<Vax::n8200>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n8200>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n8200>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8500),
            .Name = CpuSubTypeVaxInfo<Vax::n8500>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n8500>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n8500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8650),
            .Name = CpuSubTypeVaxInfo<Vax::n8650>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n8650>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n8650>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::n8800),
            .Name = CpuSubTypeVaxInfo<Vax::n8500>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::n8500>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::n8500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::UvaXII),
            .Name = CpuSubTypeVaxInfo<Vax::UvaXII>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::UvaXII>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::UvaXII>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        },
        {
            .SubType = static_cast<int32_t>(Vax::UvaXIII),
            .Name = CpuSubTypeVaxInfo<Vax::UvaXIII>::Name,
            .FullName = CpuSubTypeVaxInfo<Vax::UvaXIII>::FullName,
            .Description = CpuSubTypeVaxInfo<Vax::UvaXIII>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false,
        }
    };

    constexpr struct CpuSubTypeInfo RompCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Romp::All),
            .Name = CpuSubTypeRompInfo<Romp::All>::Name,
            .FullName = CpuSubTypeRompInfo<Romp::All>::FullName,
            .Description = CpuSubTypeRompInfo<Romp::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Romp::Pc),
            .Name = CpuSubTypeRompInfo<Romp::Pc>::Name,
            .FullName = CpuSubTypeRompInfo<Romp::Pc>::FullName,
            .Description = CpuSubTypeRompInfo<Romp::Pc>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Romp::Apc),
            .Name = CpuSubTypeRompInfo<Romp::Apc>::Name,
            .FullName = CpuSubTypeRompInfo<Romp::Apc>::FullName,
            .Description = CpuSubTypeRompInfo<Romp::Apc>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Romp::n135),
            .Name = CpuSubTypeRompInfo<Romp::All>::Name,
            .FullName = CpuSubTypeRompInfo<Romp::All>::FullName,
            .Description = CpuSubTypeRompInfo<Romp::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo MmaxCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Mmax::All),
            .Name = CpuSubTypeMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubTypeMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubTypeMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::Dpc),
            .Name = CpuSubTypeMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubTypeMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubTypeMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::Sqt),
            .Name = CpuSubTypeMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubTypeMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubTypeMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::ApcFPU),
            .Name = CpuSubTypeMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubTypeMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubTypeMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::ApcFPA),
            .Name = CpuSubTypeMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubTypeMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubTypeMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mmax::Xpc),
            .Name = CpuSubTypeMmaxInfo<Mmax::All>::Name,
            .FullName = CpuSubTypeMmaxInfo<Mmax::All>::FullName,
            .Description = CpuSubTypeMmaxInfo<Mmax::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo i386CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(i386::All),
            .Name = CpuSubTypei386Info<i386::All>::Name,
            .FullName = CpuSubTypei386Info<i386::All>::FullName,
            .Description = CpuSubTypei386Info<i386::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::n486),
            .Name = CpuSubTypei386Info<i386::n486>::Name,
            .FullName = CpuSubTypei386Info<i386::n486>::FullName,
            .Description = CpuSubTypei386Info<i386::n486>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::n486sx),
            .Name = CpuSubTypei386Info<i386::n486sx>::Name,
            .FullName = CpuSubTypei386Info<i386::n486sx>::FullName,
            .Description = CpuSubTypei386Info<i386::n486sx>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::Pentium),
            .Name = CpuSubTypei386Info<i386::Pentium>::Name,
            .FullName = CpuSubTypei386Info<i386::Pentium>::FullName,
            .Description = CpuSubTypei386Info<i386::Pentium>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::PentiumPro),
            .Name = CpuSubTypei386Info<i386::PentiumPro>::Name,
            .FullName = CpuSubTypei386Info<i386::PentiumPro>::FullName,
            .Description = CpuSubTypei386Info<i386::PentiumPro>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::PentiumIIM3),
            .Name = CpuSubTypei386Info<i386::PentiumIIM3>::Name,
            .FullName = CpuSubTypei386Info<i386::PentiumIIM3>::FullName,
            .Description = CpuSubTypei386Info<i386::PentiumIIM3>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::PentiumIIM5),
            .Name = CpuSubTypei386Info<i386::PentiumIIM5>::Name,
            .FullName = CpuSubTypei386Info<i386::PentiumIIM5>::FullName,
            .Description = CpuSubTypei386Info<i386::PentiumIIM5>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i386::Pentium4),
            .Name = CpuSubTypei386Info<i386::Pentium4>::Name,
            .FullName = CpuSubTypei386Info<i386::Pentium4>::FullName,
            .Description = CpuSubTypei386Info<i386::Pentium4>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo x86_64CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(x86_64::All),
            .Name = CpuSubTypeX86_64Info<x86_64::All>::Name,
            .FullName = CpuSubTypeX86_64Info<x86_64::All>::FullName,
            .Description = CpuSubTypeX86_64Info<x86_64::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(x86_64::Haswell),
            .Name = CpuSubTypeX86_64Info<x86_64::Haswell>::Name,
            .FullName = CpuSubTypeX86_64Info<x86_64::Haswell>::FullName,
            .Description = CpuSubTypeX86_64Info<x86_64::Haswell>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo MipsCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Mips::All),
            .Name = CpuSubTypeMipsInfo<Mips::All>::Name,
            .FullName = CpuSubTypeMipsInfo<Mips::All>::FullName,
            .Description = CpuSubTypeMipsInfo<Mips::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2300),
            .Name = CpuSubTypeMipsInfo<Mips::r2300>::Name,
            .FullName = CpuSubTypeMipsInfo<Mips::r2300>::FullName,
            .Description = CpuSubTypeMipsInfo<Mips::r2300>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2600),
            .Name = CpuSubTypeMipsInfo<Mips::r2600>::Name,
            .FullName = CpuSubTypeMipsInfo<Mips::r2600>::FullName,
            .Description = CpuSubTypeMipsInfo<Mips::r2600>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2800),
            .Name = CpuSubTypeMipsInfo<Mips::r2800>::Name,
            .FullName = CpuSubTypeMipsInfo<Mips::r2800>::FullName,
            .Description = CpuSubTypeMipsInfo<Mips::r2800>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Mips::r2000a),
            .Name = CpuSubTypeMipsInfo<Mips::r2000a>::Name,
            .FullName = CpuSubTypeMipsInfo<Mips::r2000a>::FullName,
            .Description = CpuSubTypeMipsInfo<Mips::r2000a>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo Mc680x0CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Mc680x0::All),
            .Name = CpuSubTypeMc680x0Info<Mc680x0::All>::Name,
            .FullName = CpuSubTypeMc680x0Info<Mc680x0::All>::FullName,
            .Description = CpuSubTypeMc680x0Info<Mc680x0::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc680x0::n30),
            .Name = CpuSubTypeMc680x0Info<Mc680x0::n30>::Name,
            .FullName = CpuSubTypeMc680x0Info<Mc680x0::n30>::FullName,
            .Description = CpuSubTypeMc680x0Info<Mc680x0::n30>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc680x0::n40),
            .Name = CpuSubTypeMc680x0Info<Mc680x0::n40>::Name,
            .FullName = CpuSubTypeMc680x0Info<Mc680x0::n40>::FullName,
            .Description = CpuSubTypeMc680x0Info<Mc680x0::n40>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc680x0::n30Only),
            .Name = CpuSubTypeMc680x0Info<Mc680x0::n30Only>::Name,
            .FullName = CpuSubTypeMc680x0Info<Mc680x0::n30Only>::FullName,
            .Description = CpuSubTypeMc680x0Info<Mc680x0::n30Only>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubTypeInfo HppaCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Hppa::All),
            .Name = CpuSubTypeHppaInfo<Hppa::All>::Name,
            .FullName = CpuSubTypeHppaInfo<Hppa::All>::FullName,
            .Description = CpuSubTypeHppaInfo<Hppa::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Hppa::n7100LC),
            .Name = CpuSubTypeHppaInfo<Hppa::n7100LC>::Name,
            .FullName = CpuSubTypeHppaInfo<Hppa::n7100LC>::FullName,
            .Description = CpuSubTypeHppaInfo<Hppa::n7100LC>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo ArmCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Arm::All),
            .Name = CpuSubTypeArmInfo<Arm::All>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::All>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::A500Arch),
            .Name = CpuSubTypeArmInfo<Arm::A500Arch>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::A500Arch>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::A500Arch>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::A500),
            .Name = CpuSubTypeArmInfo<Arm::A500>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::A500>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::A500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::A440),
            .Name = CpuSubTypeArmInfo<Arm::A440>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::A440>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::A440>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::m4),
            .Name = CpuSubTypeArmInfo<Arm::m4>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::m4>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::m4>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v4T),
            .Name = CpuSubTypeArmInfo<Arm::v4T>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v4T>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v4T>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v6),
            .Name = CpuSubTypeArmInfo<Arm::v6>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v6>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v6>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v5tej),
            .Name = CpuSubTypeArmInfo<Arm::v5tej>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v5tej>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v5tej>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::Xscale),
            .Name = CpuSubTypeArmInfo<Arm::Xscale>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::Xscale>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::Xscale>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7),
            .Name = CpuSubTypeArmInfo<Arm::v7>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v7>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v7>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7f),
            .Name = CpuSubTypeArmInfo<Arm::v7f>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v7f>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v7f>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7s),
            .Name = CpuSubTypeArmInfo<Arm::v7s>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v7s>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v7s>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7k),
            .Name = CpuSubTypeArmInfo<Arm::v7k>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v7k>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v7k>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v6m),
            .Name = CpuSubTypeArmInfo<Arm::v6m>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v6m>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v6m>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7m),
            .Name = CpuSubTypeArmInfo<Arm::v7m>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v7m>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v7m>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v7em),
            .Name = CpuSubTypeArmInfo<Arm::v7em>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v7em>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v7em>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm::v8),
            .Name = CpuSubTypeArmInfo<Arm::v8>::Name,
            .FullName = CpuSubTypeArmInfo<Arm::v8>::FullName,
            .Description = CpuSubTypeArmInfo<Arm::v8>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
    };

    constexpr struct CpuSubTypeInfo Arm64CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Arm64::All),
            .Name = CpuSubTypeArm64Info<Arm64::All>::Name,
            .FullName = CpuSubTypeArm64Info<Arm64::All>::FullName,
            .Description = CpuSubTypeArm64Info<Arm64::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm64::v8),
            .Name = CpuSubTypeArm64Info<Arm64::v8>::Name,
            .FullName = CpuSubTypeArm64Info<Arm64::v8>::FullName,
            .Description = CpuSubTypeArm64Info<Arm64::v8>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Arm64::E),
            .Name = CpuSubTypeArm64Info<Arm64::E>::Name,
            .FullName = CpuSubTypeArm64Info<Arm64::E>::FullName,
            .Description = CpuSubTypeArm64Info<Arm64::E>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo Arm64_32CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Arm64_32::v8),
            .Name = CpuSubTypeArm64_32Info<Arm64_32::v8>::Name,
            .FullName = CpuSubTypeArm64_32Info<Arm64_32::v8>::FullName,
            .Description = CpuSubTypeArm64_32Info<Arm64_32 ::v8>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo Mc88000CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Mc88000::All),
            .Name = CpuSubTypeMc88000Info<Mc88000::All>::Name,
            .FullName = CpuSubTypeMc88000Info<Mc88000::All>::FullName,
            .Description = CpuSubTypeMc88000Info<Mc88000::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc88000::n100),
            .Name = CpuSubTypeMc88000Info<Mc88000::n100>::Name,
            .FullName = CpuSubTypeMc88000Info<Mc88000::n100>::FullName,
            .Description = CpuSubTypeMc88000Info<Mc88000::n100>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc88000::n110),
            .Name = CpuSubTypeMc88000Info<Mc88000::n110>::Name,
            .FullName = CpuSubTypeMc88000Info<Mc88000::n110>::FullName,
            .Description = CpuSubTypeMc88000Info<Mc88000::n110>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubTypeInfo Mc98000CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Mc98000::All),
            .Name = CpuSubTypeMc98000Info<Mc98000::All>::Name,
            .FullName = CpuSubTypeMc98000Info<Mc98000::All>::FullName,
            .Description = CpuSubTypeMc98000Info<Mc98000::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Mc98000::n601),
            .Name = CpuSubTypeMc98000Info<Mc98000::n601>::Name,
            .FullName = CpuSubTypeMc98000Info<Mc98000::n601>::FullName,
            .Description = CpuSubTypeMc98000Info<Mc98000::n601>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubTypeInfo i860CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(i860::All),
            .Name = CpuSubTypeI860Info<i860::All>::Name,
            .FullName = CpuSubTypeI860Info<i860::All>::FullName,
            .Description = CpuSubTypeI860Info<i860::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(i860::n860),
            .Name = CpuSubTypeI860Info<i860::n860>::Name,
            .FullName = CpuSubTypeI860Info<i860::n860>::FullName,
            .Description = CpuSubTypeI860Info<i860::n860>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubTypeInfo i860LittleEndianCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(i860Little::All),
            .Name = CpuSubTypeI860LittleInfo<i860Little::All>::Name,
            .FullName = CpuSubTypeI860LittleInfo<i860Little::All>::FullName,
            .Description = CpuSubTypeI860LittleInfo<i860Little::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(i860Little::Default),
            .Name = CpuSubTypeI860LittleInfo<i860Little::Default>::Name,
            .FullName = CpuSubTypeI860LittleInfo<i860Little::Default>::FullName,
            .Description =
                CpuSubTypeI860LittleInfo<i860Little::Default>::Description,

            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo RS9600CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Rs9600::All),
            .Name = CpuSubTypeRs9600Info<Rs9600::All>::Name,
            .FullName = CpuSubTypeRs9600Info<Rs9600::All>::FullName,
            .Description = CpuSubTypeRs9600Info<Rs9600::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        },
        {
            .SubType = static_cast<int32_t>(Rs9600::Default),
            .Name = CpuSubTypeRs9600Info<Rs9600::Default>::Name,
            .FullName = CpuSubTypeRs9600Info<Rs9600::Default>::FullName,
            .Description = CpuSubTypeRs9600Info<Rs9600::Default>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo SparcCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Sparc::All),
            .Name = CpuSubTypeSparcInfo<Sparc::All>::Name,
            .FullName = CpuSubTypeSparcInfo<Sparc::All>::FullName,
            .Description = CpuSubTypeSparcInfo<Sparc::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = false
        }
    };

    constexpr struct CpuSubTypeInfo PowerPCCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(PowerPC::All),
            .Name = CpuSubTypePowerPCInfo<PowerPC::All>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::All>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n601),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n601>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n601>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n601>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n602),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n602>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n602>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n602>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n603),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n603>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n603>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n603>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n603e),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n603e>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n603e>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n603e>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n603ev),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n603ev>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n603ev>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n603ev>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n604),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n604>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n604>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n604>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n604e),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n604e>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n604e>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n604e>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n620),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n620>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n620>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n620>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n750),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n750>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n750>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n750>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n7400),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n7400>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n7400>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n7400>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n7500),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n7500>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n7500>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n7500>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC::n970),
            .Name = CpuSubTypePowerPCInfo<PowerPC::n970>::Name,
            .FullName = CpuSubTypePowerPCInfo<PowerPC::n970>::FullName,
            .Description = CpuSubTypePowerPCInfo<PowerPC::n970>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubTypeInfo PowerPC64CpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(PowerPC64::All),
            .Name = CpuSubTypePowerPC64Info<PowerPC64::All>::Name,
            .FullName = CpuSubTypePowerPC64Info<PowerPC64::All>::FullName,
            .Description = CpuSubTypePowerPC64Info<PowerPC64::All>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(PowerPC64::n970),
            .Name = CpuSubTypePowerPC64Info<PowerPC64::n970>::Name,
            .FullName = CpuSubTypePowerPC64Info<PowerPC64::n970>::FullName,
            .Description =
                CpuSubTypePowerPC64Info<PowerPC64::n970>::Description,

            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuSubTypeInfo VeoCpuSubTypeTable[] = {
        {
            .SubType = static_cast<int32_t>(Veo::n1),
            .Name = CpuSubTypeVeoInfo<Veo::n1>::Name,
            .FullName = CpuSubTypeVeoInfo<Veo::n1>::FullName,
            .Description = CpuSubTypeVeoInfo<Veo::n1>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Veo::n2),
            .Name = CpuSubTypeVeoInfo<Veo::n2>::Name,
            .FullName = CpuSubTypeVeoInfo<Veo::n2>::FullName,
            .Description = CpuSubTypeVeoInfo<Veo::n2>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Veo::n3),
            .Name = CpuSubTypeVeoInfo<Veo::n3>::Name,
            .FullName = CpuSubTypeVeoInfo<Veo::n3>::FullName,
            .Description = CpuSubTypeVeoInfo<Veo::n3>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        },
        {
            .SubType = static_cast<int32_t>(Veo::n4),
            .Name = CpuSubTypeVeoInfo<Veo::n4>::Name,
            .FullName = CpuSubTypeVeoInfo<Veo::n4>::FullName,
            .Description = CpuSubTypeVeoInfo<Veo::n4>::Description,
            .IsEitherEndian = false,
            .IsBigEndian = true
        }
    };

    constexpr struct CpuTypeInfo CpuTypeInfoTable[] = {
        {
            .CpuType = CpuType::Any,
            .Name = CpuTypeTemplateInfo<CpuType::Any>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Any>::Name,
            .Description = CpuTypeTemplateInfo<CpuType::Any>::Name,
            .SubTypeTable = AnyCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Vax,
            .Name = CpuTypeTemplateInfo<CpuType::Vax>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Vax>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Vax>::Description,
            .SubTypeTable = VaxCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Romp,
            .Name = CpuTypeTemplateInfo<CpuType::Romp>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Romp>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Romp>::Description,
            .SubTypeTable = RompCpuSubTypeTable,
        },
        {
            .CpuType = CpuType::NS32032,
            .Name = CpuTypeTemplateInfo<CpuType::NS32032>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::NS32032>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::NS32032>::Description,
            .SubTypeTable = MmaxCpuSubTypeTable
        },
        {
            .CpuType = CpuType::NS32332,
            .Name = CpuTypeTemplateInfo<CpuType::NS32332>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::NS32332>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::NS32332>::Description,
            .SubTypeTable = MmaxCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Mc680x0,
            .Name = CpuTypeTemplateInfo<CpuType::Mc680x0>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Mc680x0>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Mc680x0>::Description,
            .SubTypeTable = Mc680x0CpuSubTypeTable
        },
        {
            .CpuType = CpuType::i386,
            .Name = CpuTypeTemplateInfo<CpuType::i386>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::i386>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::i386>::Description,
            .SubTypeTable = i386CpuSubTypeTable
        },
        {
            .CpuType = CpuType::x86_64,
            .Name = CpuTypeTemplateInfo<CpuType::x86_64>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::x86_64>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::x86_64>::Description,
            .SubTypeTable = x86_64CpuSubTypeTable,
        },
        {
            .CpuType = CpuType::Mips,
            .Name = CpuTypeTemplateInfo<CpuType::Mips>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Mips>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Mips>::Description,
            .SubTypeTable = MipsCpuSubTypeTable
        },
        {
            .CpuType = CpuType::NS32532,
            .Name = CpuTypeTemplateInfo<CpuType::NS32532>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::NS32532>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::NS32532>::Description,
            .SubTypeTable = MmaxCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Hppa,
            .Name = CpuTypeTemplateInfo<CpuType::Hppa>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Hppa>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Hppa>::Description,
            .SubTypeTable = HppaCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Arm,
            .Name = CpuTypeTemplateInfo<CpuType::Arm>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Arm>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Arm>::Description,
            .SubTypeTable = ArmCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Mc88000,
            .Name = CpuTypeTemplateInfo<CpuType::Mc88000>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Mc88000>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Mc88000>::Description,
            .SubTypeTable = Mc88000CpuSubTypeTable
        },
        {
            .CpuType = CpuType::Sparc,
            .Name = CpuTypeTemplateInfo<CpuType::Sparc>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Sparc>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Sparc>::Description,
            .SubTypeTable = SparcCpuSubTypeTable
        },
        {
            .CpuType = CpuType::i860,
            .Name = CpuTypeTemplateInfo<CpuType::i860>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::i860>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::i860>::Description,
            .SubTypeTable = i860CpuSubTypeTable
        },
        {
            .CpuType = CpuType::i860Little,
            .Name = CpuTypeTemplateInfo<CpuType::i860Little>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::i860Little>::BrandName,
            .Description =
                CpuTypeTemplateInfo<CpuType::i860Little>::Description,
            .SubTypeTable = i860LittleEndianCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Rs9600,
            .Name = CpuTypeTemplateInfo<CpuType::Rs9600>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Rs9600>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Rs9600>::Description,
            .SubTypeTable = RS9600CpuSubTypeTable
        },
        {
            .CpuType = CpuType::Mc98000,
            .Name = CpuTypeTemplateInfo<CpuType::Mc98000>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Mc98000>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Mc98000>::Description,
            .SubTypeTable = Mc98000CpuSubTypeTable
        },
        {
            .CpuType = CpuType::PowerPC,
            .Name = CpuTypeTemplateInfo<CpuType::PowerPC>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::PowerPC>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::PowerPC>::Description,
            .SubTypeTable = PowerPCCpuSubTypeTable
        },
        {
            .CpuType = CpuType::PowerPC64,
            .Name = CpuTypeTemplateInfo<CpuType::PowerPC64>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::PowerPC64>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::PowerPC64>::Description,
            .SubTypeTable = PowerPC64CpuSubTypeTable
        },
        {
            .CpuType = CpuType::Veo,
            .Name = CpuTypeTemplateInfo<CpuType::Veo>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Veo>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Veo>::Description,
            .SubTypeTable = VeoCpuSubTypeTable
        },
        {
            .CpuType = CpuType::Arm64,
            .Name = CpuTypeTemplateInfo<CpuType::Arm64>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Arm64>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Arm64>::Description,
            .SubTypeTable = Arm64CpuSubTypeTable
        },
        {
            .CpuType = CpuType::Arm64_32,
            .Name = CpuTypeTemplateInfo<CpuType::Arm64_32>::Name,
            .BrandName = CpuTypeTemplateInfo<CpuType::Arm64_32>::BrandName,
            .Description = CpuTypeTemplateInfo<CpuType::Arm64_32>::Description,
            .SubTypeTable = Arm64_32CpuSubTypeTable
        }
    };

    constexpr const CpuTypeInfo *GetInfoForCpuType(CpuType CpuType) noexcept {
        switch (CpuType) {
            case CpuType::Any:
                return (CpuTypeInfoTable + 0);
            case CpuType::Vax:
                return (CpuTypeInfoTable + 1);
            case CpuType::Romp:
                return (CpuTypeInfoTable + 2);
            case CpuType::NS32032:
                return (CpuTypeInfoTable + 3);
            case CpuType::NS32332:
                return (CpuTypeInfoTable + 4);
            case CpuType::Mc680x0:
                return (CpuTypeInfoTable + 5);
            case CpuType::i386:
                return (CpuTypeInfoTable + 6);
            case CpuType::x86_64:
                return (CpuTypeInfoTable + 7);
            case CpuType::Mips:
                return (CpuTypeInfoTable + 8);
            case CpuType::NS32532:
                return (CpuTypeInfoTable + 9);
            case CpuType::Hppa:
                return (CpuTypeInfoTable + 10);
            case CpuType::Arm:
                return (CpuTypeInfoTable + 11);
            case CpuType::Mc88000:
                return (CpuTypeInfoTable + 12);
            case CpuType::Sparc:
                return (CpuTypeInfoTable + 13);
            case CpuType::i860:
                return (CpuTypeInfoTable + 14);
            case CpuType::i860Little:
                return (CpuTypeInfoTable + 15);
            case CpuType::Rs9600:
                return (CpuTypeInfoTable + 16);
            case CpuType::Mc98000:
                return (CpuTypeInfoTable + 17);
            case CpuType::PowerPC:
                return (CpuTypeInfoTable + 18);
            case CpuType::PowerPC64:
                return (CpuTypeInfoTable + 19);
            case CpuType::Veo:
                return (CpuTypeInfoTable + 20);
            case CpuType::Arm64:
                return (CpuTypeInfoTable + 21);
            case CpuType::Arm64_32:
                return (CpuTypeInfoTable + 22);
        }

        return nullptr;
    }

    constexpr const CpuSubTypeInfo *
    GetInfoForCpuSubType(CpuType CpuType, int32_t SubType) noexcept {
        switch (CpuType) {
            case CpuType::Any:
                switch (Any(SubType)) {
                    case Any::Multiple:
                        return (AnyCpuSubTypeTable + 0);

                    case Any::LittleEndian:
                        return (AnyCpuSubTypeTable + 1);

                    case Any::BigEndian:
                        return (AnyCpuSubTypeTable + 2);
                }

                break;

            case CpuType::Vax:
                switch (Vax(SubType)) {
                    case Vax::All:
                      return (VaxCpuSubTypeTable + 0);
                    case Vax::n780:
                      return (VaxCpuSubTypeTable + 1);
                    case Vax::n785:
                      return (VaxCpuSubTypeTable + 2);
                    case Vax::n750:
                      return (VaxCpuSubTypeTable + 3);
                    case Vax::n730:
                      return (VaxCpuSubTypeTable + 4);
                    case Vax::UvaXI:
                      return (VaxCpuSubTypeTable + 5);
                    case Vax::UvaXII:
                      return (VaxCpuSubTypeTable + 6);
                    case Vax::n8200:
                      return (VaxCpuSubTypeTable + 7);
                    case Vax::n8500:
                      return (VaxCpuSubTypeTable + 8);
                    case Vax::n8600:
                      return (VaxCpuSubTypeTable + 9);
                    case Vax::n8650:
                      return (VaxCpuSubTypeTable + 10);
                    case Vax::n8800:
                      return (VaxCpuSubTypeTable + 11);
                    case Vax::UvaXIII:
                      return (VaxCpuSubTypeTable + 12);
                }

                break;

            case CpuType::Romp:
                switch (Romp(SubType)) {
                    case Romp::All:
                        return (RompCpuSubTypeTable + 0);
                    case Romp::Pc:
                        return (RompCpuSubTypeTable + 1);
                    case Romp::Apc:
                        return (RompCpuSubTypeTable + 2);
                    case Romp::n135:
                        return (RompCpuSubTypeTable + 3);
                }

                break;

            case CpuType::NS32032:
            case CpuType::NS32332:
            case CpuType::NS32532:
                switch (Mmax(SubType)) {
                    case Mmax::All:
                        return (MmaxCpuSubTypeTable + 0);
                    case Mmax::Dpc:
                        return (MmaxCpuSubTypeTable + 0);
                    case Mmax::Sqt:
                        return (MmaxCpuSubTypeTable + 0);
                    case Mmax::ApcFPU:
                        return (MmaxCpuSubTypeTable + 0);
                    case Mmax::ApcFPA:
                        return (MmaxCpuSubTypeTable + 0);
                    case Mmax::Xpc:
                        return (MmaxCpuSubTypeTable + 0);
                }

                break;

            case CpuType::Mc680x0: {
                switch (Mc680x0(SubType)) {
                    case Mc680x0::All:
                        return (Mc680x0CpuSubTypeTable + 0);
                    case Mc680x0::n40:
                        return (Mc680x0CpuSubTypeTable + 1);
                    case Mc680x0::n30Only:
                        return (Mc680x0CpuSubTypeTable + 2);
                }

                break;
            }

            case CpuType::i386:
                switch (i386(SubType)) {
                    case i386::All:
                        return (i386CpuSubTypeTable + 0);
                    case i386::n486:
                        return (i386CpuSubTypeTable + 1);
                    case i386::n486sx:
                        return (i386CpuSubTypeTable + 2);
                    case i386::Pentium:
                        return (i386CpuSubTypeTable + 3);
                    case i386::PentiumPro:
                        return (i386CpuSubTypeTable + 4);
                    case i386::PentiumIIM3:
                        return (i386CpuSubTypeTable + 5);
                    case i386::PentiumIIM5:
                        return (i386CpuSubTypeTable + 6);
                    case i386::Pentium4:
                        return (i386CpuSubTypeTable + 7);
                }

                break;

            case CpuType::x86_64:
                switch (x86_64(SubType)) {
                    case x86_64::All:
                        return (x86_64CpuSubTypeTable + 0);

                    case x86_64::Haswell:
                        return (x86_64CpuSubTypeTable + 1);
                }

                break;

            case CpuType::Mips:
                switch (Mips(SubType)) {
                    case Mips::All:
                        return (MipsCpuSubTypeTable + 0);
                    case Mips::r2300:
                        return (MipsCpuSubTypeTable + 1);
                    case Mips::r2600:
                        return (MipsCpuSubTypeTable + 1);
                    case Mips::r2800:
                        return (MipsCpuSubTypeTable + 1);
                    case Mips::r2000a:
                        return (MipsCpuSubTypeTable + 1);
                }

                break;

            case CpuType::Hppa:
                switch (Hppa(SubType)) {
                    case Hppa::All:
                        return (HppaCpuSubTypeTable + 0);
                    case Hppa::n7100LC:
                        return (HppaCpuSubTypeTable + 1);
                }

                break;

            case CpuType::Arm:
                switch (Arm(SubType)) {
                    case Arm::All:
                        return (ArmCpuSubTypeTable + 0);
                    case Arm::A500Arch:
                        return (ArmCpuSubTypeTable + 1);
                    case Arm::A500:
                        return (ArmCpuSubTypeTable + 2);
                    case Arm::A440:
                        return (ArmCpuSubTypeTable + 3);
                    case Arm::m4:
                        return (ArmCpuSubTypeTable + 4);
                    case Arm::v4T:
                        return (ArmCpuSubTypeTable + 5);
                    case Arm::v6:
                        return (ArmCpuSubTypeTable + 6);
                    case Arm::v5tej:
                        return (ArmCpuSubTypeTable + 7);
                    case Arm::Xscale:
                        return (ArmCpuSubTypeTable + 8);
                    case Arm::v7:
                        return (ArmCpuSubTypeTable + 9);
                    case Arm::v7f:
                        return (ArmCpuSubTypeTable + 10);
                    case Arm::v7s:
                        return (ArmCpuSubTypeTable + 11);
                    case Arm::v7k:
                        return (ArmCpuSubTypeTable + 12);
                    case Arm::v6m:
                        return (ArmCpuSubTypeTable + 13);
                    case Arm::v7m:
                        return (ArmCpuSubTypeTable + 14);
                    case Arm::v7em:
                        return (ArmCpuSubTypeTable + 15);
                    case Arm::v8:
                        return (ArmCpuSubTypeTable + 16);
                }

                break;

            case CpuType::Mc88000:
                switch (Mc88000(SubType)) {
                    case Mc88000::All:
                        return (Mc88000CpuSubTypeTable + 0);
                    case Mc88000::n100:
                        return (Mc88000CpuSubTypeTable + 1);
                    case Mc88000::n110:
                        return (Mc88000CpuSubTypeTable + 2);
                }

                break;

            case CpuType::Sparc:
                switch (Sparc(SubType)) {
                    case Sparc::All:
                        return (SparcCpuSubTypeTable + 0);
                }

                break;

            case CpuType::i860:
                switch (i860(SubType)) {
                    case i860::All:
                        return (i860CpuSubTypeTable + 0);
                    case i860::n860:
                        return (i860CpuSubTypeTable + 1);
                }

                break;

            case CpuType::i860Little:
                switch (i860Little(SubType)) {
                    case i860Little::All:
                        return (i860LittleEndianCpuSubTypeTable + 0);
                    case i860Little::Default:
                        return (i860LittleEndianCpuSubTypeTable + 1);
                }

                break;

            case CpuType::Rs9600:
                switch (Rs9600(SubType)) {
                    case Rs9600::All:
                        return (RS9600CpuSubTypeTable + 0);
                    case Rs9600::Default:
                        return (RS9600CpuSubTypeTable + 1);
                }

                break;

            case CpuType::Mc98000:
                switch (Mc98000(SubType)) {
                    case Mc98000::All:
                        return (Mc98000CpuSubTypeTable + 0);
                    case Mc98000::n601:
                        return (Mc98000CpuSubTypeTable + 1);
                }

                break;

            case CpuType::PowerPC:
                switch (PowerPC(SubType)) {
                    case PowerPC::All:
                        return (PowerPCCpuSubTypeTable + 0);
                    case PowerPC::n601:
                        return (PowerPCCpuSubTypeTable + 1);
                    case PowerPC::n602:
                        return (PowerPCCpuSubTypeTable + 2);
                    case PowerPC::n603:
                        return (PowerPCCpuSubTypeTable + 3);
                    case PowerPC::n603e:
                        return (PowerPCCpuSubTypeTable + 4);
                    case PowerPC::n603ev:
                        return (PowerPCCpuSubTypeTable + 5);
                    case PowerPC::n604:
                        return (PowerPCCpuSubTypeTable + 6);
                    case PowerPC::n604e:
                        return (PowerPCCpuSubTypeTable + 7);
                    case PowerPC::n620:
                        return (PowerPCCpuSubTypeTable + 8);
                    case PowerPC::n750:
                        return (PowerPCCpuSubTypeTable + 9);
                    case PowerPC::n7400:
                        return (PowerPCCpuSubTypeTable + 10);
                    case PowerPC::n7500:
                        return (PowerPCCpuSubTypeTable + 11);
                    case PowerPC::n970:
                        return (PowerPCCpuSubTypeTable + 12);
                }

                break;

            case CpuType::PowerPC64:
                switch (PowerPC64(SubType)) {
                    case PowerPC64::All:
                        return (PowerPC64CpuSubTypeTable + 0);
                    case PowerPC64::n970:
                        return (PowerPC64CpuSubTypeTable + 1);
                }

                break;

            case CpuType::Veo:
                switch (Veo(SubType)) {
                    case Veo::n1:
                        return (VeoCpuSubTypeTable + 0);
                    case Veo::n2:
                        return (VeoCpuSubTypeTable + 1);
                    case Veo::n3:
                        return (VeoCpuSubTypeTable + 2);
                    case Veo::n4:
                        return (VeoCpuSubTypeTable + 3);
                }

            case CpuType::Arm64:
                switch (Arm64(SubType)) {
                    case Arm64::All:
                        return (Arm64CpuSubTypeTable + 0);
                    case Arm64::v8:
                        return (Arm64CpuSubTypeTable + 1);
                    case Arm64::E:
                        return (Arm64CpuSubTypeTable + 2);
                }

                break;

            case CpuType::Arm64_32:
                switch (Arm64_32(SubType)) {
                    case Arm64_32::v8:
                        return (Arm64_32CpuSubTypeTable + 0);
                }

                break;
        }

        return nullptr;
    }

    namespace CpuSubType {
        constexpr static std::string_view EmptyStringView = std::string_view();

        constexpr const std::string_view &
        GetName(CpuType CpuType, int32_t CpuSubType) noexcept {
            if (auto *Info = GetInfoForCpuSubType(CpuType, CpuSubType)) {
                return Info->Name;
            }

            return EmptyStringView;
        }

        constexpr const std::string_view &
        GetFullName(CpuType CpuType, int32_t CpuSubType) noexcept {
            if (auto *Info = GetInfoForCpuSubType(CpuType, CpuSubType)) {
                return Info->FullName;
            }

            return EmptyStringView;
        }

        constexpr const std::string_view &
        GetDescription(CpuType CpuType, int32_t CpuSubType) noexcept {
            if (auto *Info = GetInfoForCpuSubType(CpuType, CpuSubType)) {
                return Info->Description;
            }

            return EmptyStringView;
        }
    }
}
