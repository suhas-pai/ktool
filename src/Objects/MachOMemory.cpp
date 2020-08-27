//
//  src/Objects/MachOMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/DoesOverflow.h"
#include "MachOMemory.h"

ConstMachOMemoryObject::ConstMachOMemoryObject(
    const ConstMemoryMap &Map) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()) {}

ConstMachOMemoryObject::ConstMachOMemoryObject(
    ObjectKind Kind,
    const ConstMemoryMap &Map) noexcept
: MemoryObject(Kind), Map(Map.getBegin()), End(Map.getEnd()) {}

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
    const auto LCSize = Header.getLoadCommandsSize();

    if (Is64Bit) {
        if (DoesAddOverflow(Header.size(), LCSize, &HeaderAndLCSize)) {
            return ConstMachOMemoryObject::Error::TooManyLoadCommands;
        }
    } else {
        if (DoesAddOverflow<uint32_t>(Header.size(), LCSize, &HeaderAndLCSize))
        {
            return ConstMachOMemoryObject::Error::TooManyLoadCommands;
        }
    }

    if (!Map.IsLargeEnoughForSize(HeaderAndLCSize)) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    // Basic check with Load-Command Count.
    auto MinLCSize = uint32_t();

    const auto LCCount = Header.getLoadCommandsCount();
    const auto SingleLCSize = sizeof(MachO::LoadCommand);

    if (DoesMultiplyOverflow(SingleLCSize, LCCount, &MinLCSize)) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    if (LCSize < MinLCSize) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    return ConstMachOMemoryObject::Error::None;
}

PointerOrError<ConstMachOMemoryObject, ConstMachOMemoryObject::Error>
ConstMachOMemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return new ConstMachOMemoryObject(Map);
}

bool ConstMachOMemoryObject::errorDidMatchFormat(Error Error) noexcept {
    switch (Error) {
        case ConstMachOMemoryObject::Error::None:
            return true;
        case ConstMachOMemoryObject::Error::WrongFormat:
        case ConstMachOMemoryObject::Error::SizeTooSmall:
            return false;
        case ConstMachOMemoryObject::Error::TooManyLoadCommands:
            return true;
    }
}
