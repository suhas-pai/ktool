//
//  src/ADT/MachO.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <vector>

#include "Utils/DoesOverflow.h"
#include "Utils/Leb128.h"
#include "Utils/MiscTemplates.h"
#include "Utils/SwitchEndian.h"

#include "Mach.h"
#include "MachO.h"

namespace MachO {
    FatHeader::Arch32List FatHeader::getArch32List(bool IsBigEndian) noexcept {
        const auto Entries = reinterpret_cast<Arch32 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return Arch32List(Entries, Count);
    }

    FatHeader::Arch64List FatHeader::getArch64List(bool IsBigEndian) noexcept {
        const auto Entries = reinterpret_cast<Arch64 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return Arch64List(Entries, Count);
    }

    FatHeader::ConstArch32List
    FatHeader::getConstArch32List(bool IsBigEndian) const noexcept {
        const auto Entries = reinterpret_cast<const Arch32 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return ConstArch32List(Entries, Count);
    }

    FatHeader::ConstArch64List
    FatHeader::getConstArch64List(bool IsBigEndian) const noexcept {
        const auto Entries = reinterpret_cast<const Arch64 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return ConstArch64List(Entries, Count);
    }
}
