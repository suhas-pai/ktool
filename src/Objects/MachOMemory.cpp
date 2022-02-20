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
    const ObjectKind Kind,
    const ConstMemoryMap &Map) noexcept
: MemoryObject(Kind), Map(Map.getBegin()), End(Map.getEnd()) {}

MachOMemoryObject::MachOMemoryObject(const MemoryMap &Map) noexcept
: ConstMachOMemoryObject(Map) {}

auto
ConstMachOMemoryObject::ValidateMap(const ConstMemoryMap &Map) noexcept -> Error
{
    if (!Map.isLargeEnoughForType<MachO::Header>()) {
        return Error::SizeTooSmall;
    }

    const auto &Header = *Map.getBeginAs<MachO::Header>();
    if (!Header.hasValidMagic()) {
        return Error::WrongFormat;
    }

    auto HeaderAndLCSize = uint64_t();

    const auto Is64Bit = Header.is64Bit();
    const auto LCSize = Header.getLoadCommandsSize();

    if (Is64Bit) {
        if (DoesAddOverflow(Header.size(), LCSize, &HeaderAndLCSize)) {
            return Error::TooManyLoadCommands;
        }
    } else {
        if (DoesAddOverflow<uint32_t>(Header.size(), LCSize, &HeaderAndLCSize))
        {
            return Error::TooManyLoadCommands;
        }
    }

    if (!Map.isLargeEnoughForSize(HeaderAndLCSize)) {
        return Error::TooManyLoadCommands;
    }

    // Basic check with Load-Command Count.
    auto MinLCSize = uint32_t();

    const auto LCCount = Header.getLoadCommandsCount();
    const auto SingleLCSize = sizeof(MachO::LoadCommand);

    if (DoesMultiplyOverflow(SingleLCSize, LCCount, &MinLCSize)) {
        return Error::TooManyLoadCommands;
    }

    if (LCSize < MinLCSize) {
        return Error::TooManyLoadCommands;
    }

    return Error::None;
}

auto
ConstMachOMemoryObject::Open(const ConstMemoryMap &Map) noexcept ->
    PointerOrError<ConstMachOMemoryObject, Error>
{
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return new ConstMachOMemoryObject(Map);
}

bool ConstMachOMemoryObject::errorDidMatchFormat(const Error Error) noexcept {
    switch (Error) {
        case Error::None:
            return true;
        case Error::WrongFormat:
        case Error::SizeTooSmall:
            return false;
        case Error::TooManyLoadCommands:
            return true;
    }
}
