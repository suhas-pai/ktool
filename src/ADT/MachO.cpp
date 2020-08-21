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
    FatHeader::Arch32List FatHeader::getArch32List() noexcept {
        assert(!this->Is64Bit());

        const auto Entries = reinterpret_cast<Arch32 *>(this + 1);
        return Arch32List(Entries, this->getArchCount());
    }

    FatHeader::Arch64List FatHeader::getArch64List() noexcept {
        assert(this->Is64Bit());

        const auto Entries = reinterpret_cast<Arch64 *>(this + 1);
        return Arch64List(Entries, this->getArchCount());
    }

    FatHeader::ConstArch32List FatHeader::getConstArch32List() const noexcept {
        assert(!this->Is64Bit());

        const auto Entries = reinterpret_cast<const Arch32 *>(this + 1);
        return ConstArch32List(Entries, this->getArchCount());
    }

    FatHeader::ConstArch64List FatHeader::getConstArch64List() const noexcept {
        assert(this->Is64Bit());

        const auto Entries = reinterpret_cast<const Arch64 *>(this + 1);
        return ConstArch64List(Entries, this->getArchCount());
    }
}
