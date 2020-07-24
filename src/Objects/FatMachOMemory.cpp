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
        return ConstFatMachOMemoryObject::Error::SizeTooSmall;
    }

    if (!Header->hasValidMagic()) {
        return ConstFatMachOMemoryObject::Error::WrongFormat;
    }

    const auto IsBigEndian = Header->IsBigEndian();
    const auto ArchCount = SwitchEndianIf(Header->NFatArch, IsBigEndian);

    if (ArchCount == 0) {
        return ConstFatMachOMemoryObject::Error::ZeroArchitectures;
    }

    const auto Is64Bit = Header->Is64Bit();
    const auto SingleArchSize =
        (Is64Bit) ?
            sizeof(MachO::FatHeader::Arch64) :
            sizeof(MachO::FatHeader::Arch32);

    auto TotalHeaderSize = uint64_t();
    if (Is64Bit) {
        if (DoesMultiplyAndAddOverflow(SingleArchSize, ArchCount,
                                       sizeof(*Header), &TotalHeaderSize))
        {
            return ConstFatMachOMemoryObject::Error::TooManyArchitectures;
        }
    } else {
        if (DoesMultiplyAndAddOverflow<uint32_t>(SingleArchSize, ArchCount,
                                                 sizeof(*Header),
                                                 &TotalHeaderSize))
        {
            return ConstFatMachOMemoryObject::Error::TooManyArchitectures;
        }
    }

    if (TotalHeaderSize > MapSize) {
        return ConstFatMachOMemoryObject::Error::SizeTooSmall;
    }

    if (Is64Bit) {
        const auto Begin =
            reinterpret_cast<const MachO::FatHeader::Arch64 *>(Header + 1);

        for (const auto &Arch : BasicContiguousList(Begin, ArchCount)) {
            const auto ArchOffset = SwitchEndianIf(Arch.Offset, IsBigEndian);
            const auto ArchSize = SwitchEndianIf(Arch.Size, IsBigEndian);
            const auto ArchRange =
                LocationRange::CreateWithSize(ArchOffset, ArchSize);

            if (!ArchRange || ArchRange->goesPastEnd(MapSize)) {
                return ConstFatMachOMemoryObject::Error::ArchOutOfBounds;
            }

            for (const auto &Inner : BasicContiguousList(Begin, &Arch)) {
                const auto InnerOffset =
                    SwitchEndianIf(Inner.Offset, IsBigEndian);

                const auto InnerSize = SwitchEndianIf(Inner.Size, IsBigEndian);
                const auto InnerRange =
                    LocationRange::CreateWithSize(InnerOffset, InnerSize);

                if (ArchRange->overlaps(InnerRange.value())) {
                    return ConstFatMachOMemoryObject::Error::ArchOverlapsArch;
                }
            }
        }
    } else {
        const auto Begin =
            reinterpret_cast<const MachO::FatHeader::Arch32 *>(Header + 1);

        for (const auto &Arch : BasicContiguousList(Begin, ArchCount)) {
            const auto ArchOffset = SwitchEndianIf(Arch.Offset, IsBigEndian);
            const auto ArchSize = SwitchEndianIf(Arch.Size, IsBigEndian);
            const auto ArchRange =
                LocationRange::CreateWithSize(ArchOffset, ArchSize);

            if (!ArchRange || ArchRange->goesPastEnd(MapSize)) {
                return ConstFatMachOMemoryObject::Error::ArchOutOfBounds;
            }

            for (const auto &Inner : BasicContiguousList(Begin, &Arch)) {
                const auto InnerOffset =
                    SwitchEndianIf(Inner.Offset, IsBigEndian);

                const auto InnerSize = SwitchEndianIf(Inner.Size, IsBigEndian);
                const auto InnerRange =
                    LocationRange::CreateWithSize(InnerOffset, InnerSize);

                if (ArchRange->overlaps(InnerRange.value())) {
                    return ConstFatMachOMemoryObject::Error::ArchOverlapsArch;
                }
            }
        }
    }

    return ConstFatMachOMemoryObject::Error::None;
}

FatMachOMemoryObject
FatMachOMemoryObject::Open(const MemoryMap &Map) noexcept {
    const auto Error = ValidateMap(Map);
    if (Error != ConstFatMachOMemoryObject::Error::None) {
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

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(
    const ConstMemoryMap &Map) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()) {}

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(Error Error) noexcept
: MemoryObject(ObjKind), ErrorStorage(Error) {}

FatMachOMemoryObject::FatMachOMemoryObject(const MemoryMap &Map) noexcept
: ConstFatMachOMemoryObject(Map) {}

FatMachOMemoryObject::FatMachOMemoryObject(Error Error) noexcept
: ConstFatMachOMemoryObject(Error) {}

static bool MatchesFormat(ConstFatMachOMemoryObject::Error Error) noexcept {
    switch (Error) {
        case ConstFatMachOMemoryObject::Error::None:
            return true;
        case ConstFatMachOMemoryObject::Error::WrongFormat:
        case ConstFatMachOMemoryObject::Error::SizeTooSmall:
            return false;
        case ConstFatMachOMemoryObject::Error::ZeroArchitectures:
        case ConstFatMachOMemoryObject::Error::TooManyArchitectures:
        case ConstFatMachOMemoryObject::Error::ArchOutOfBounds:
        case ConstFatMachOMemoryObject::Error::ArchOverlapsArch:
            return true;
    }

    return false;
}

bool ConstFatMachOMemoryObject::DidMatchFormat() const noexcept {
    return MatchesFormat(getError());
}

MemoryObject *ConstFatMachOMemoryObject::ToPtr() const noexcept {
    return new ConstFatMachOMemoryObject(ConstMemoryMap(Map, End));
}

enum class ArchKind {
    None,
    MachO
};

template <typename ListType>
static ConstFatMachOMemoryObject::ArchInfo
GetArchInfoAtIndexImpl(const ListType &List,
                       uint32_t Index,
                       bool IsBigEndian) noexcept
{
    auto Info = ConstFatMachOMemoryObject::ArchInfo();
    const auto &Arch = List.at(Index);

    Info.CpuKind = Arch.getCpuKind(IsBigEndian);
    Info.CpuSubKind = SwitchEndianIf(Arch.CpuSubKind, IsBigEndian);

    Info.Offset = SwitchEndianIf(Arch.Offset, IsBigEndian);
    Info.Size = SwitchEndianIf(Arch.Size, IsBigEndian);
    Info.Align = SwitchEndianIf(Arch.Align, IsBigEndian);

    return Info;
}

ConstFatMachOMemoryObject::ArchInfo
ConstFatMachOMemoryObject::GetArchInfoAtIndex(uint32_t Index) const noexcept {
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
    using WarningEnum = ConstFatMachOMemoryObject::GetObjectResult::WarningEnum;
    using GetObjectResult = ConstFatMachOMemoryObject::GetObjectResult;

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

ConstFatMachOMemoryObject::GetObjectResult
ConstFatMachOMemoryObject::GetArchObjectFromInfo(
    const ArchInfo &Info) const noexcept
{
    const auto ArchMapBegin = Map + Info.Offset;
    const auto ArchMap = ConstMemoryMap(ArchMapBegin, ArchMapBegin + Info.Size);

    auto Kind = ArchKind::None;
    switch (Kind) {
        case ArchKind::None:
        case ArchKind::MachO:
            const auto Object = ConstMachOMemoryObject::Open(ArchMap);
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
