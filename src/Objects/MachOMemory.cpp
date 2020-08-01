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

[[nodiscard]] static
ConstMachOMemoryObject::Error ValidateMap(const ConstMemoryMap &Map) noexcept {
    const auto Header = reinterpret_cast<const MachO::Header *>(Map.getBegin());
    const auto MapSize = Map.size();

    if (MapSize < sizeof(*Header)) {
        return ConstMachOMemoryObject::Error::SizeTooSmall;
    }

    if (!Header->hasValidMagic()) {
        return ConstMachOMemoryObject::Error::WrongFormat;
    }

    auto HeaderAndLCSize = uint64_t();

    const auto IsBigEndian = Header->IsBigEndian();
    const auto Is64Bit = Header->Is64Bit();
    const auto SizeOfCmds = SwitchEndianIf(Header->SizeOfCmds, IsBigEndian);

    if (Is64Bit) {
        if (DoesAddOverflow(Header->size(), SizeOfCmds, &HeaderAndLCSize)) {
            return ConstMachOMemoryObject::Error::TooManyLoadCommands;
        }
    } else {
        if (DoesAddOverflow<uint32_t>(Header->size(), SizeOfCmds,
                                      &HeaderAndLCSize))
        {
            return ConstMachOMemoryObject::Error::TooManyLoadCommands;
        }
    }

    if (HeaderAndLCSize > MapSize) {
        return ConstMachOMemoryObject::Error::TooManyLoadCommands;
    }

    // Basic check with Ncmds.
    auto MinSizeOfCmds = uint32_t();

    const auto Ncmds = SwitchEndianIf(Header->Ncmds, IsBigEndian);
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

MachOMemoryObject MachOMemoryObject::Open(const MemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return MachOMemoryObject(Map);
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

MachO::ConstLoadCommandStorage
ConstMachOMemoryObject::GetLoadCommands(bool Verify) const noexcept {
    assert(!hasError());

    const auto IsBigEndian = Header->IsBigEndian();
    const auto Is64Bit = Header->Is64Bit();

    const auto Begin = Header->getConstLoadCmdBuffer();
    const auto Ncmds = SwitchEndianIf(Header->Ncmds, IsBigEndian);
    const auto SizeOfCmds = SwitchEndianIf(Header->SizeOfCmds, IsBigEndian);

    const auto Obj =
        MachO::ConstLoadCommandStorage::Open(Begin,
                                             Ncmds,
                                             SizeOfCmds,
                                             IsBigEndian,
                                             Is64Bit,
                                             Verify);
    return Obj;
}

MachO::LoadCommandStorage
MachOMemoryObject::GetLoadCommands(bool Verify) noexcept {
    assert(!hasError());

    const auto IsBigEndian = Header->IsBigEndian();
    const auto Is64Bit = Header->Is64Bit();

    const auto Begin = getHeader().getLoadCmdBuffer();
    const auto Ncmds = SwitchEndianIf(Header->Ncmds, IsBigEndian);
    const auto SizeOfCmds = SwitchEndianIf(Header->SizeOfCmds, IsBigEndian);

    const auto Obj =
        MachO::LoadCommandStorage::Open(Begin,
                                        Ncmds,
                                        SizeOfCmds,
                                        IsBigEndian,
                                        Is64Bit,
                                        Verify);
    return Obj;
}
