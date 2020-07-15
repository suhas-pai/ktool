//
//  src/Objects/FatMachOMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/LocationRange.h"
#include "ADT/Mach.h"

#include "Utils/DoesOverflow.h"
#include "Utils/MiscTemplates.h"
#include "Utils/SwitchEndian.h"

#include "FatMachOMemory.h"
#include "MachOMemory.h"

FatMachOMemoryObject::Error ValidateMap(const ConstMemoryMap &Map) {
    const auto MapSize = Map.size();
    const auto Header =
        reinterpret_cast<const MachO::FatHeader *>(Map.getBegin());

    if (MapSize < sizeof(*Header)) {
        return FatMachOMemoryObject::Error::SizeTooSmall;
    }

    if (!Header->hasValidMagic()) {
        return FatMachOMemoryObject::Error::WrongFormat;
    }

    const auto IsBigEndian = Header->IsBigEndian();
    const auto NFatArch = SwitchEndianIf(Header->NFatArch, IsBigEndian);

    if (NFatArch == 0) {
        return FatMachOMemoryObject::Error::ZeroArchitectures;
    }

    const auto Is64Bit = Header->Is64Bit();
    const auto SingleArchSize =
        (Is64Bit) ?
            sizeof(MachO::FatHeader::Arch64) :
            sizeof(MachO::FatHeader::Arch32);

    auto TotalHeaderSize = uint64_t();
    if (Is64Bit) {
        if (DoesMultiplyAndAddOverflow(SingleArchSize, NFatArch,
                                       sizeof(*Header), &TotalHeaderSize))
        {
            return FatMachOMemoryObject::Error::TooManyArchitectures;
        }
    } else {
        if (DoesMultiplyAndAddOverflow<uint32_t>(SingleArchSize, NFatArch,
                                                 sizeof(*Header),
                                                 &TotalHeaderSize))
        {
            return FatMachOMemoryObject::Error::TooManyArchitectures;
        }
    }

    if (TotalHeaderSize > MapSize) {
        return FatMachOMemoryObject::Error::SizeTooSmall;
    }

    if (Is64Bit) {
        const auto Begin =
            reinterpret_cast<const MachO::FatHeader::Arch64 *>(Header + 1);

        for (const auto &Arch : BasicContiguousList(Begin, NFatArch)) {
            const auto ArchOffset = SwitchEndianIf(Arch.Offset, IsBigEndian);
            const auto ArchSize = SwitchEndianIf(Arch.Size, IsBigEndian);
            const auto ArchRange =
                LocationRange::CreateWithSize(ArchOffset, ArchSize);

            if (!ArchRange || ArchRange->goesPastEnd(MapSize)) {
                return FatMachOMemoryObject::Error::ArchOutOfBounds;
            }

            for (const auto &Inner : BasicContiguousList(Begin, &Arch)) {
                const auto InnerOffset =
                    SwitchEndianIf(Inner.Offset, IsBigEndian);

                const auto InnerSize = SwitchEndianIf(Inner.Size, IsBigEndian);
                const auto InnerRange =
                    LocationRange::CreateWithSize(InnerOffset, InnerSize);

                if (ArchRange->overlaps(InnerRange.value())) {
                    return FatMachOMemoryObject::Error::ArchOverlapsArch;
                }
            }
        }
    } else {
        const auto Begin =
            reinterpret_cast<const MachO::FatHeader::Arch32 *>(Header + 1);

        for (const auto &Arch : BasicContiguousList(Begin, NFatArch)) {
            const auto ArchOffset = SwitchEndianIf(Arch.Offset, IsBigEndian);
            const auto ArchSize = SwitchEndianIf(Arch.Size, IsBigEndian);
            const auto ArchRange =
                LocationRange::CreateWithSize(ArchOffset, ArchSize);

            if (!ArchRange || ArchRange->goesPastEnd(MapSize)) {
                return FatMachOMemoryObject::Error::ArchOutOfBounds;
            }

            for (const auto &Inner : BasicContiguousList(Begin, &Arch)) {
                const auto InnerOffset =
                    SwitchEndianIf(Inner.Offset, IsBigEndian);

                const auto InnerSize = SwitchEndianIf(Inner.Size, IsBigEndian);
                const auto InnerRange =
                    LocationRange::CreateWithSize(InnerOffset, InnerSize);

                if (ArchRange->overlaps(InnerRange.value())) {
                    return FatMachOMemoryObject::Error::ArchOverlapsArch;
                }
            }
        }
    }

    return FatMachOMemoryObject::Error::None;
}

FatMachOMemoryObject
FatMachOMemoryObject::Open(const MemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != FatMachOMemoryObject::Error::None) {
        return Error;
    }

    return FatMachOMemoryObject(Map);
}

ConstFatMachOMemoryObject
ConstFatMachOMemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != ConstFatMachOMemoryObject::Error::None) {
        return Error;
    }

    return ConstFatMachOMemoryObject(Map);
}

FatMachOMemoryObject::FatMachOMemoryObject(const MemoryMap &Map) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()) {}

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(
    const ConstMemoryMap &Map) noexcept
: FatMachOMemoryObject(reinterpret_cast<const MemoryMap &>(Map)) {}

FatMachOMemoryObject::FatMachOMemoryObject(Error Error) noexcept
: MemoryObject(ObjKind), ErrorStorage(Error) {}

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(Error Error) noexcept
: FatMachOMemoryObject(Error) {}

static bool MatchesFormat(FatMachOMemoryObject::Error Error) noexcept {
    switch (Error) {
        case FatMachOMemoryObject::Error::None:
            return true;
        case FatMachOMemoryObject::Error::WrongFormat:
        case FatMachOMemoryObject::Error::SizeTooSmall:
            return false;
        case FatMachOMemoryObject::Error::ZeroArchitectures:
        case FatMachOMemoryObject::Error::TooManyArchitectures:
        case FatMachOMemoryObject::Error::ArchOutOfBounds:
        case FatMachOMemoryObject::Error::ArchOverlapsArch:
            return true;
    }

    return false;
}

bool FatMachOMemoryObject::DidMatchFormat() const noexcept {
    return MatchesFormat(getError());
}

MemoryObject *FatMachOMemoryObject::ToPtr() const noexcept {
    return new FatMachOMemoryObject(MemoryMap(Map, End));
}

enum class ArchKind {
    None,
    MachO
};

template <typename ListType>
static FatMachOMemoryObject::ArchInfo
GetArchInfoAtIndexImpl(const ListType &List,
                       uint32_t Index,
                       bool IsBigEndian) noexcept
{
    auto Info = FatMachOMemoryObject::ArchInfo();
    const auto &Arch = List.at(Index);

    Info.CpuKind = Arch.getCpuKind(IsBigEndian);
    Info.CpuSubKind = SwitchEndianIf(Arch.CpuSubKind, IsBigEndian);

    Info.Offset = SwitchEndianIf(Arch.Offset, IsBigEndian);
    Info.Size = SwitchEndianIf(Arch.Size, IsBigEndian);
    Info.Align = SwitchEndianIf(Arch.Align, IsBigEndian);

    return Info;
}

FatMachOMemoryObject::ArchInfo
FatMachOMemoryObject::GetArchInfoAtIndex(uint32_t Index) const noexcept {
    assert(!IndexOutOfBounds(Index, getArchCount()));

    auto Info = ArchInfo();
    const auto IsBigEndian = this->IsBigEndian();

    if (this->Is64Bit()) {
        Info = GetArchInfoAtIndexImpl(Header->getConstArch64List(IsBigEndian),
                                      Index,
                                      IsBigEndian);
    } else {
        Info = GetArchInfoAtIndexImpl(Header->getConstArch32List(IsBigEndian),
                                      Index,
                                      IsBigEndian);
    }

    return Info;
}

static FatMachOMemoryObject::GetObjectResult
GetMachOObjectResult(MemoryObject *ArchObject,
                     Mach::CpuKind CpuKind,
                     int32_t CpuSubKind) noexcept
{
    using WarningEnum = FatMachOMemoryObject::GetObjectResult::WarningEnum;
    using GetObjectResult = FatMachOMemoryObject::GetObjectResult;

    const auto MachOObject = cast<ObjectKind::MachO>(ArchObject);
    const auto Header = MachOObject->getConstHeader();
    const auto IsBigEndian = MachOObject->IsBigEndian();

    const auto ArchCpuKind = MachOObject->getCpuKind();
    const auto ArchCpuSubKind = SwitchEndianIf(Header.CpuSubKind, IsBigEndian);

    if ((ArchCpuKind != CpuKind) || (ArchCpuSubKind != CpuSubKind)) {
        return GetObjectResult(MachOObject, WarningEnum::MachOCpuKindMismatch);
    }

    return MachOObject;
}

FatMachOMemoryObject::GetObjectResult
FatMachOMemoryObject::GetArchObjectFromInfo(
    const ArchInfo &Info) const noexcept
{
    const auto ArchMapBegin = Map + Info.Offset;
    const auto ArchMap = MemoryMap(ArchMapBegin, ArchMapBegin + Info.Size);

    auto Kind = ArchKind::None;
    switch (Kind) {
        case ArchKind::None:
        case ArchKind::MachO:
            const auto Object = MachOMemoryObject::Open(ArchMap);
            if (Object.DidMatchFormat()) {
                const auto Result =
                    GetMachOObjectResult(Object.ToPtr(),
                                         Info.CpuKind,
                                         Info.CpuSubKind);

                return Result;
            }
    }

    return nullptr;
}
