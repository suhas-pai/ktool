//
//  src/Objects/MachOMemory.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/DoesOverflow.h"
#include "MachOMemory.h"

MachOMemoryObject::MachOMemoryObject(Error Error) noexcept
: MemoryObject(ObjectKind::MachO), ErrorStorage(Error) {}

MachOMemoryObject::MachOMemoryObject(const MemoryMap &Map) noexcept
: MemoryObject(ObjectKind::MachO), Map(Map.GetBegin()), End(Map.GetEnd()) {}

ConstMachOMemoryObject::ConstMachOMemoryObject(Error Error) noexcept
: ConstMemoryObject(ObjectKind::MachO), ErrorStorage(Error) {}

ConstMachOMemoryObject::ConstMachOMemoryObject(const ConstMemoryMap &Map)
noexcept : ConstMemoryObject(ObjectKind::MachO), Map(Map.GetBegin()),
           End(Map.GetEnd()) {}

static
MachOMemoryObject::Error ValidateMap(const ConstMemoryMap &Map) noexcept {
    const auto Header = reinterpret_cast<const MachO::Header *>(Map.GetBegin());
    const auto MapSize = Map.GetSize();

    if (MapSize < sizeof(*Header)) {
        return MachOMemoryObject::Error::SizeTooSmall;
    }

    if (!Header->HasValidMagic()) {
        return MachOMemoryObject::Error::WrongFormat;
    }

    auto HeaderAndLCSize = uint64_t();

    const auto IsBigEndian = Header->IsBigEndian();
    const auto Is64Bit = Header->Is64Bit();
    const auto SizeOfCmds = SwitchEndianIf(Header->SizeOfCmds, IsBigEndian);

    if (Is64Bit) {
        if (DoesAddOverflow(Header->GetSize(), SizeOfCmds, &HeaderAndLCSize)) {
            return MachOMemoryObject::Error::TooManyLoadCommands;
        }
    } else {
        if (DoesAddOverflow<uint32_t>(Header->GetSize(), SizeOfCmds,
                                      &HeaderAndLCSize))
        {
            return MachOMemoryObject::Error::TooManyLoadCommands;
        }
    }

    if (HeaderAndLCSize > MapSize) {
        return MachOMemoryObject::Error::TooManyLoadCommands;
    }

    // Basic check with Ncmds.
    auto MinSizeOfCmds = uint32_t();

    const auto Ncmds = SwitchEndianIf(Header->Ncmds, IsBigEndian);
    const auto LCSize = sizeof(MachO::LoadCommand);

    if (DoesMultiplyOverflow(LCSize, Ncmds, &MinSizeOfCmds)) {
        return MachOMemoryObject::Error::TooManyLoadCommands;
    }

    if (SizeOfCmds < MinSizeOfCmds) {
        return MachOMemoryObject::Error::TooManyLoadCommands;
    }

    return MachOMemoryObject::Error::None;
}

MachOMemoryObject MachOMemoryObject::Open(const MemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return MachOMemoryObject(Map);
}

ConstMachOMemoryObject
ConstMachOMemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return ConstMachOMemoryObject(Map);
}

static bool MatchesFormat(MachOMemoryObject::Error Error) noexcept {
    switch (Error) {
        case MachOMemoryObject::Error::None:
            return true;
        case MachOMemoryObject::Error::WrongFormat:
        case MachOMemoryObject::Error::SizeTooSmall:
            return false;
        case MachOMemoryObject::Error::TooManyLoadCommands:
            return true;
    }

    return false;
}

bool MachOMemoryObject::DidMatchFormat() const noexcept {
    return MatchesFormat(GetError());
}

bool ConstMachOMemoryObject::DidMatchFormat() const noexcept {
    return MatchesFormat(GetError());
}

MemoryObject *MachOMemoryObject::ToPtr() const noexcept {
    return new MachOMemoryObject(MemoryMap(Map, End));
}

ConstMemoryObject *ConstMachOMemoryObject::ToPtr() const noexcept {
    return new ConstMachOMemoryObject(ConstMemoryMap(Map, End));
}

MachO::LoadCommandStorage
MachOMemoryObject::GetLoadCommands(bool Verify) noexcept {
    assert(!HasError());

    const auto IsBigEndian = Header->IsBigEndian();
    const auto Is64Bit = Header->Is64Bit();

    const auto Begin = Header->GetLoadCmdBuffer();
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

MachO::ConstLoadCommandStorage
GetConstLoadCommandStorage(const MachO::Header *Header, bool Verify) noexcept {
    const auto IsBigEndian = Header->IsBigEndian();
    const auto Is64Bit = Header->Is64Bit();

    const auto Begin = Header->GetConstLoadCmdBuffer();
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

MachO::ConstLoadCommandStorage
MachOMemoryObject::GetConstLoadCommands(bool Verify) const noexcept {
    assert(!HasError());
    return GetConstLoadCommandStorage(Header, Verify);
}

MachO::ConstLoadCommandStorage
ConstMachOMemoryObject::GetLoadCommands(bool Verify) const noexcept {
    assert(!HasError());
    return GetConstLoadCommandStorage(Header, Verify);
}
