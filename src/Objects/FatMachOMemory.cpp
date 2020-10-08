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

template <PointerKind Kind>
[[nodiscard]] static ConstFatMachOMemoryObject::Error
ValidateArchList(const ConstMemoryMap &Map,
                 const MachO::FatHeader &Header) noexcept
{
    using ArchType =
        typename std::conditional_t<PointerKindIs64Bit(Kind),
                                    MachO::FatHeader::Arch64,
                                    MachO::FatHeader::Arch32>;

    using AddrType = PointerAddrTypeFromKind<Kind>;

    if (Header.hasZeroArchs()) {
        return ConstFatMachOMemoryObject::Error::ZeroArchitectures;
    }

    const auto ArchCount = Header.getArchCount();
    auto TotalHeaderSize = uint64_t();

    if (DoesMultiplyAndAddOverflow<AddrType>(sizeof(ArchType), ArchCount,
                                             sizeof(Header), &TotalHeaderSize))
    {
        return ConstFatMachOMemoryObject::Error::TooManyArchitectures;
    }

    if (!Map.isLargeEnoughForSize(TotalHeaderSize)) {
        return ConstFatMachOMemoryObject::Error::SizeTooSmall;
    }

    const auto IsBigEndian = Header.isBigEndian();
    const auto ArchListBegin = reinterpret_cast<const ArchType *>(&Header + 1);
    const auto ArchListEnd = ArchListBegin + ArchCount;

    for (const auto &Arch : BasicContiguousList(ArchListBegin, ArchListEnd)) {
        const auto ArchRangeOpt = Arch.getFileRange(IsBigEndian);
        if (!ArchRangeOpt.has_value()) {
            return ConstFatMachOMemoryObject::Error::ArchOutOfBounds;
        }

        const auto &ArchRange = ArchRangeOpt.value();
        if (ArchRange.goesPastEnd(Map.size())) {
            return ConstFatMachOMemoryObject::Error::ArchOutOfBounds;
        }

        for (const auto &Inner : BasicContiguousList(ArchListBegin, &Arch)) {
            const auto InnerRange = Inner.getFileRange(IsBigEndian).value();
            if (ArchRange.overlaps(InnerRange)) {
                return ConstFatMachOMemoryObject::Error::ArchOverlapsArch;
            }
        }
    }

    return ConstFatMachOMemoryObject::Error::None;
}

[[nodiscard]] static ConstFatMachOMemoryObject::Error
ValidateMap(const ConstMemoryMap &Map) noexcept {
    if (!Map.isLargeEnoughForType<MachO::FatHeader>()) {
        return ConstFatMachOMemoryObject::Error::SizeTooSmall;
    }

    const auto &Header = *Map.getBeginAs<MachO::FatHeader>();
    if (!Header.hasValidMagic()) {
        return ConstFatMachOMemoryObject::Error::WrongFormat;
    }

    auto Error = ConstFatMachOMemoryObject::Error::None;
    if (Header.is64Bit()) {
        Error = ValidateArchList<PointerKind::s64Bit>(Map, Header);
    } else {
        Error = ValidateArchList<PointerKind::s32Bit>(Map, Header);
    }

    if (Error != ConstFatMachOMemoryObject::Error::None) {
        return Error;
    }

    return ConstFatMachOMemoryObject::Error::None;
}

auto
ConstFatMachOMemoryObject::Open(const ConstMemoryMap &Map) noexcept ->
    PointerOrError<ConstFatMachOMemoryObject, Error>
{
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return new ConstFatMachOMemoryObject(Map);
}

ConstFatMachOMemoryObject::ConstFatMachOMemoryObject(
    const ConstMemoryMap &Map) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()) {}

FatMachOMemoryObject::FatMachOMemoryObject(const MemoryMap &Map) noexcept
: ConstFatMachOMemoryObject(Map) {}

bool ConstFatMachOMemoryObject::errorDidMatchFormat(Error Error) noexcept {
    switch (Error) {
        case Error::None:
            return true;
        case Error::WrongFormat:
        case Error::SizeTooSmall:
            return false;
        case Error::ZeroArchitectures:
        case Error::TooManyArchitectures:
        case Error::ArchOutOfBounds:
        case Error::ArchOverlapsArch:
            return true;
    }

    return false;
}

enum class ArchKind {
    None,
    MachO
};

template <typename ListType>
[[nodiscard]] static ConstFatMachOMemoryObject::ArchInfo
GetArchInfoAtIndexImpl(const ListType &List,
                       uint32_t Index,
                       bool IsBigEndian) noexcept
{
    auto Info = ConstFatMachOMemoryObject::ArchInfo();
    const auto &Arch = List.at(Index);

    Info.CpuKind = Arch.getCpuKind(IsBigEndian);
    Info.CpuSubKind = Arch.getCpuSubKind(IsBigEndian);

    Info.Offset = Arch.getFileOffset(IsBigEndian);
    Info.Size = Arch.getFileSize(IsBigEndian);
    Info.Align = Arch.getAlign(IsBigEndian);

    return Info;
}

auto
ConstFatMachOMemoryObject::GetArchInfoAtIndex(uint32_t Index) const noexcept ->
    ArchInfo
{
    assert(!IndexOutOfBounds(Index, this->getArchCount()));

    auto Info = ArchInfo();
    const auto IsBigEndian = this->isBigEndian();

    if (this->is64Bit()) {
        Info = GetArchInfoAtIndexImpl(Header->getConstArch64List(),
                                      Index,
                                      IsBigEndian);
    } else {
        Info = GetArchInfoAtIndexImpl(Header->getConstArch32List(),
                                      Index,
                                      IsBigEndian);
    }

    return Info;
}

[[nodiscard]] static FatMachOMemoryObject::GetArchObjectResult
GetMachOObjectResult(MemoryObject *ArchObject,
                     Mach::CpuKind CpuKind,
                     int32_t CpuSubKind) noexcept
{
    using WarningEnum = ConstFatMachOMemoryObject::GetArchObjectWarning;
    using GetObjectResult = ConstFatMachOMemoryObject::GetArchObjectResult;

    const auto MachOObject = cast<ObjectKind::MachO>(ArchObject);
    const auto ArchCpuKind = MachOObject->getCpuKind();
    const auto ArchCpuSubKind = MachOObject->getCpuSubKind();

    if ((ArchCpuKind != CpuKind) || (ArchCpuSubKind != CpuSubKind)) {
        return GetObjectResult(MachOObject, WarningEnum::MachOCpuKindMismatch);
    }

    return MachOObject;
}

auto
ConstFatMachOMemoryObject::GetArchObjectFromInfo(
    const ArchInfo &Info) const noexcept -> GetArchObjectResult
{
    const auto ArchRangeOpt =
        LocationRange::CreateWithSize(Info.Offset, Info.Size);

    if (!ArchRangeOpt.has_value()) {
        return GetArchObjectError::InvalidArchRange;
    }

    const auto &ArchRange = ArchRangeOpt.value();
    if (!getRange().containsLocRange(ArchRange)) {
        return GetArchObjectError::InvalidArchRange;
    }

    const auto ArchMap = getMap().mapFromLocRange(ArchRange);
    auto Kind = ArchKind::None;

    switch (Kind) {
        case ArchKind::None:
        case ArchKind::MachO:
            const auto ObjectOrError = ConstMachOMemoryObject::Open(ArchMap);
            const auto Error = ObjectOrError.getError();

            if (ConstMachOMemoryObject::errorDidMatchFormat(Error)) {
                const auto Result =
                    GetMachOObjectResult(ObjectOrError.getPtr(),
                                         Info.CpuKind,
                                         Info.CpuSubKind);

                return Result;
            }
    }

    return GetArchObjectError::UnsupportedObjectKind;
}
