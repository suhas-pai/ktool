//
//  src/Objects/MachOMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/DoesOverflow.h"
#include "MachOMemory.h"

ConstMachOMemoryObject::ConstMachOMemoryObject(Error Error) noexcept
: MemoryObject(ObjKind), ErrorStorage(Error) {}

ConstMachOMemoryObject::ConstMachOMemoryObject(
    const ConstMemoryMap &Map) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()) {}

ConstMachOMemoryObject::ConstMachOMemoryObject(
    ObjectKind Kind,
    const ConstMemoryMap &Map) noexcept
: MemoryObject(Kind), Map(Map.getBegin()), End(Map.getEnd()) {}

MachOMemoryObject::MachOMemoryObject(Error Error) noexcept
: ConstMachOMemoryObject(Error) {}

MachOMemoryObject::MachOMemoryObject(const MemoryMap &Map) noexcept
: ConstMachOMemoryObject(Map) {}

ConstMachOMemoryObject::Error
ConstMachOMemoryObject::ValidateMap(const ConstMemoryMap &Map) noexcept {
    if (!Map.IsLargeEnoughForType<MachO::Header>()) {
        return ConstMachOMemoryObject::Error::SizeTooSmall;
    }

    const auto &Header = *Map.getBeginAs<MachO::Header>();
    if (!Header.hasValidMagic()) {
        return ConstMachOMemoryObject::Error::WrongFormat;
    }

    auto HeaderAndLCSize = uint64_t();

    const auto Is64Bit = Header.Is64Bit();
    const auto SizeOfCmds = Header.getLoadCommandsSize();

    if (Is64Bit) {
        if (DoesAddOverflow(Header.size(), SizeOfCmds, &HeaderAndLCSize)) {
            return ConstMachOMemoryObject::Error::TooManyLoadCommands;
        }
    } else {
        if (DoesAddOverflow<uint32_t>(Header.size(), SizeOfCmds,
                                      &HeaderAndLCSize))
        {
            return ConstMachOMemoryObject::Error::TooManyLoadCommands;
        }
    }

    if (!Map.IsLargeEnoughForSize(HeaderAndLCSize)) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    // Basic check with Ncmds.
    auto MinSizeOfCmds = uint32_t();

    const auto Ncmds = Header.getLoadCommandsCount();
    const auto LCSize = sizeof(MachO::LoadCommand);

    if (DoesMultiplyOverflow(LCSize, Ncmds, &MinSizeOfCmds)) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    if (SizeOfCmds < MinSizeOfCmds) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    return ConstMachOMemoryObject::Error::None;
}

ConstMachOMemoryObject
ConstMachOMemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return ConstMachOMemoryObject(Map);
}

[[nodiscard]]
static bool MatchesFormat(ConstMachOMemoryObject::Error Error) noexcept {
    switch (Error) {
        case ConstMachOMemoryObject::Error::None:
            return true;
        case ConstMachOMemoryObject::Error::WrongFormat:
        case ConstMachOMemoryObject::Error::SizeTooSmall:
            return false;
        case ConstMachOMemoryObject::Error::TooManyLoadCommands:
            return true;
    }

    return false;
}

bool ConstMachOMemoryObject::DidMatchFormat() const noexcept {
    return MatchesFormat(getError());
}

MemoryObject *ConstMachOMemoryObject::ToPtr() const noexcept {
    return new ConstMachOMemoryObject(ConstMemoryMap(Map, End));
}
