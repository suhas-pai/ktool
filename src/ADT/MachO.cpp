//
//  src/ADT/MachO.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <vector>

#include "Utils/DoesOverflow.h"
#include "Utils/MiscTemplates.h"
#include "Utils/SwitchEndian.h"
#include "Utils/Uleb128.h"

#include "Mach.h"
#include "MachO.h"

namespace MachO {
    bool Header::HasValidMagic() const noexcept { return MagicIsValid(Magic); }
    bool Header::HasValidFileType() const noexcept {
        const auto Value = SwitchEndianIf(FileType, this->IsBigEndian());
        return FileTypeIsValid(static_cast<enum FileType>(Value));
    }

    bool Header::IsBigEndian() const noexcept {
        switch (this->Magic) {
            case Magic::BigEndian:
            case Magic::BigEndian64:
                return true;
            case Magic::Default:
            case Magic::Default64:
                return false;
        }

        return false;
    }

    bool Header::Is64Bit() const noexcept {
        switch (this->Magic) {
            case Magic::Default64:
            case Magic::BigEndian64:
                return true;
            case Magic::Default:
            case Magic::BigEndian:
                return false;
        }

        return false;
    }

    Mach::CpuType Header::GetCpuType() const noexcept {
        return Mach::CpuType(SwitchEndianIf(CpuType, this->IsBigEndian()));
    }

    enum Header::FileType Header::GetFileType() const noexcept {
        const auto Value = SwitchEndianIf(FileType, this->IsBigEndian());
        return static_cast<enum FileType>(Value);
    }

    BasicMasksHandler<Header::FlagsEnum> Header::GetFlags() const noexcept {
        const auto Flags = this->Flags.GetIntegerValue(this->IsBigEndian());
        return Flags;
    }

    uint8_t *Header::GetLoadCmdBuffer() noexcept {
        const auto LoadCmdBuffer =
            reinterpret_cast<uint8_t *>(this) + GetSize();

        return LoadCmdBuffer;
    }

    const uint8_t *Header::GetConstLoadCmdBuffer() const noexcept {
        const auto LoadCmdBuffer =
            reinterpret_cast<const uint8_t *>(this) + GetSize();

        return LoadCmdBuffer;
    }

    uint64_t Header::GetSize() const noexcept {
        if (this->Is64Bit()) {
            return (sizeof(*this) + sizeof(uint32_t));
        }

        return sizeof(*this);
    }

    FatHeader::Arch32List FatHeader::GetArch32List(bool IsBigEndian) noexcept {
        const auto Entries = reinterpret_cast<Arch32 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return Arch32List(Entries, Count);
    }

    FatHeader::Arch64List FatHeader::GetArch64List(bool IsBigEndian) noexcept {
        const auto Entries = reinterpret_cast<Arch64 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return Arch64List(Entries, Count);
    }

    FatHeader::ConstArch32List
    FatHeader::GetConstArch32List(bool IsBigEndian) const noexcept {
        const auto Entries = reinterpret_cast<const Arch32 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return ConstArch32List(Entries, Count);
    }

    FatHeader::ConstArch64List
    FatHeader::GetConstArch64List(bool IsBigEndian) const noexcept {
        const auto Entries = reinterpret_cast<const Arch64 *>(this + 1);
        const auto Count = SwitchEndianIf(NFatArch, IsBigEndian);

        return ConstArch64List(Entries, Count);
    }

    bool FatHeader::HasValidMagic() const noexcept {
        return MagicIsValid(Magic);
    }

    bool FatHeader::IsBigEndian() const noexcept {
        switch (this->Magic) {
            case Magic::BigEndian:
            case Magic::BigEndian64:
                return true;
            case Magic::Default:
            case Magic::Default64:
                return false;
        }

        return false;
    }

    bool FatHeader::Is64Bit() const noexcept {
        switch (this->Magic) {
            case Magic::Default64:
            case Magic::BigEndian64:
                return true;
            case Magic::Default:
            case Magic::BigEndian:
                return false;
        }

        return false;
    }
}
