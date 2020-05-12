//
//  src/Objects/FatMachOMap.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/LocationRange.h"
#include "ADT/Mach.h"

#include "Utils/DoesOverflow.h"
#include "Utils/SwitchEndian.h"
#include "Utils/MiscTemplates.h"

#include "FatMachOMemory.h"
#include "MachOMemory.h"

FatMachOMemoryObject::Error ValidateMap(const ConstMemoryMap &Map) {
    const auto MapSize = Map.GetSize();
    const auto Header =
        reinterpret_cast<const MachO::FatHeader *>(Map.GetBegin());

    if (MapSize < sizeof(*Header)) {
        return FatMachOMemoryObject::Error::SizeTooSmall;
    }

    if (!Header->HasValidMagic()) {
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

            if (!ArchRange || ArchRange->GoesPastSize(MapSize)) {
                return FatMachOMemoryObject::Error::ArchOutOfBounds;
            }

            for (const auto &Inner : BasicContiguousList(Begin, &Arch)) {
                const auto InnerOffset =
                    SwitchEndianIf(Inner.Offset, IsBigEndian);

                const auto InnerSize = SwitchEndianIf(Inner.Size, IsBigEndian);
                const auto InnerRange =
                    LocationRange::CreateWithSize(InnerOffset, InnerSize);

                if (ArchRange->Overlaps(InnerRange.value())) {
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

            if (!ArchRange || ArchRange->GoesPastSize(MapSize)) {
                return FatMachOMemoryObject::Error::ArchOutOfBounds;
            }

            for (const auto &Inner : BasicContiguousList(Begin, &Arch)) {
                const auto InnerOffset =
                    SwitchEndianIf(Inner.Offset, IsBigEndian);

                const auto InnerSize = SwitchEndianIf(Inner.Size, IsBigEndian);
                const auto InnerRange =
                    LocationRange::CreateWithSize(InnerOffset, InnerSize);

                if (ArchRange->Overlaps(InnerRange.value())) {
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
: MemoryObject(ObjectKind::FatMachO), Map(Map.GetBegin()), End(Map.GetEnd()) {}

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(const ConstMemoryMap &Map)
noexcept : ConstMemoryObject(ObjectKind::FatMachO), Map(Map.GetBegin()),
           End(Map.GetEnd()) {}

FatMachOMemoryObject::FatMachOMemoryObject(Error Error) noexcept
: MemoryObject(ObjectKind::FatMachO), ErrorStorage(Error) {}

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(Error Error) noexcept
: ConstMemoryObject(ObjectKind::FatMachO), ErrorStorage(Error) {}

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
    return MatchesFormat(GetError());
}

bool ConstFatMachOMemoryObject::DidMatchFormat() const noexcept {
    return MatchesFormat(GetError());
}

MemoryObject *FatMachOMemoryObject::ToPtr() const noexcept {
    return new FatMachOMemoryObject(MemoryMap(Map, End));
}

ConstMemoryObject *ConstFatMachOMemoryObject::ToPtr() const noexcept {
    return new ConstFatMachOMemoryObject(ConstMemoryMap(Map, End));
}

enum class ArchKind {
    None,
    MachO
};

template <typename ListType>
static FatMachOMemoryObject::ArchInfo
GetArchInfoAtIndexImpl(const ListType &List, uint32_t Index, bool IsBigEndian)
noexcept {
    auto Info = FatMachOMemoryObject::ArchInfo();
    const auto &Arch = List.at(Index);

    Info.CpuType = Arch->GetCpuType(IsBigEndian);
    Info.CpuSubType = SwitchEndianIf(Arch->CpuSubType, IsBigEndian);

    Info.Offset = SwitchEndianIf(Arch->Offset, IsBigEndian);
    Info.Size = SwitchEndianIf(Arch->Size, IsBigEndian);
    Info.Align = SwitchEndianIf(Arch->Align, IsBigEndian);

    return Info;
}

FatMachOMemoryObject::ArchInfo
FatMachOMemoryObject::GetArchInfoAtIndex(uint32_t Index) const noexcept {
    assert(!IndexOutOfBounds(Index, this->GetArchCount()) &&
           "Index is past bounds of ArchList");

    auto Info = ArchInfo();
    const auto IsBigEndian = this->IsBigEndian();

    if (this->Is64Bit()) {
        Info = GetArchInfoAtIndexImpl(Header->GetConstArch64List(IsBigEndian),
                                      Index, IsBigEndian);
    } else {
        Info = GetArchInfoAtIndexImpl(Header->GetConstArch32List(IsBigEndian),
                                      Index, IsBigEndian);
    }

    return Info;
}

ConstFatMachOMemoryObject::ArchInfo
ConstFatMachOMemoryObject::GetArchInfoAtIndex(uint32_t Index) const noexcept {
    assert(!IndexOutOfBounds(Index, this->GetArchCount()) &&
           "Index is past bounds of ArchList");

    auto Info = ArchInfo();
    const auto IsBigEndian = this->IsBigEndian();

    if (this->Is64Bit()) {
        Info = GetArchInfoAtIndexImpl(Header->GetConstArch64List(IsBigEndian),
                                      Index, IsBigEndian);
    } else {
        Info = GetArchInfoAtIndexImpl(Header->GetConstArch32List(IsBigEndian),
                                      Index, IsBigEndian);
    }

    return Info;
}

template <typename MachOObjectType, typename MemoryObjectType>
static FatMachOMemoryObject::GetObjectResultTemplate<MemoryObjectType>
GetMachOObjectResult(MemoryObjectType *ArchObject,
                     Mach::CpuType CpuType,
                     int32_t CpuSubType)
{
    using WarningEnum = FatMachOMemoryObject::GetObjectResult::WarningEnum;
    using GetObjectResult =
        FatMachOMemoryObject::GetObjectResultTemplate<MemoryObjectType>;

    const auto MachOObject = cast<MachOObjectType *>(ArchObject);
    const auto Header = MachOObject->GetConstHeader();
    const auto IsBigEndian = MachOObject->IsBigEndian();

    const auto ArchCpuType = MachOObject->GetCpuType();
    const auto ArchCpuSubType = SwitchEndianIf(Header.CpuSubType, IsBigEndian);

    if ((ArchCpuType != CpuType) || (ArchCpuSubType != CpuSubType)) {
        const auto Warning = WarningEnum::MachOCpuTypeMismatch;
        return GetObjectResult(MachOObject, Warning);
    }

    return MachOObject;
}

FatMachOMemoryObject::GetObjectResult
FatMachOMemoryObject::GetArchObjectFromInfo(const ArchInfo &Info)
const noexcept {
    const auto ArchMapBegin = Map + Info.Offset;
    const auto ArchMap = MemoryMap(ArchMapBegin, ArchMapBegin + Info.Size);

    auto Kind = ArchKind::None;
    switch (Kind) {
        case ArchKind::None:
        case ArchKind::MachO:
            const auto Object = MachOMemoryObject::Open(ArchMap);
            if (Object.DidMatchFormat()) {
                return GetMachOObjectResult<MachOMemoryObject>(
                    Object.ToPtr(), Info.CpuType, Info.CpuSubType);
            }
    }

    return nullptr;
}

ConstFatMachOMemoryObject::GetObjectResult
ConstFatMachOMemoryObject::GetArchObjectFromInfo(const ArchInfo &Info)
const noexcept {
    const auto ArchMapBegin = Map + Info.Offset;
    const auto ArchMap = ConstMemoryMap(ArchMapBegin, ArchMapBegin + Info.Size);

    auto Kind = ArchKind::None;
    switch (Kind) {
        case ArchKind::None:
        case ArchKind::MachO:
            const auto Object = ConstMachOMemoryObject::Open(ArchMap);
            if (Object.DidMatchFormat()) {
                return GetMachOObjectResult<ConstMachOMemoryObject>(
                    Object.ToPtr(), Info.CpuType, Info.CpuSubType);
            }
    }

    return nullptr;
}
