//
//  Objects/FatMachOMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Range.h"

#include "Objects/FatMachOMemory.h"
#include "Objects/MachOMemory.h"

#include "Utils/DoesOverflow.h"
#include "Utils/MiscTemplates.h"

template <PointerKind Kind>
[[nodiscard]] static auto
ValidateArchList(const ConstMemoryMap &Map,
                 const MachO::FatHeader &Header) noexcept
    -> FatMachOMemoryObject::Error
{
    using AddrType = PointerAddrTypeFromKind<Kind>;
    using ArchType =
        typename std::conditional_t<PointerKindIs64Bit(Kind),
                                    MachO::FatHeader::Arch64,
                                    MachO::FatHeader::Arch32>;

    if (Header.hasZeroArchs()) {
        return FatMachOMemoryObject::Error::ZeroArchitectures;
    }

    const auto ArchCount = Header.getArchCount();
    auto TotalHeaderSize = uint64_t();

    if (DoesMultiplyAndAddOverflow<AddrType>(sizeof(ArchType), ArchCount,
                                             sizeof(Header), &TotalHeaderSize))
    {
        return FatMachOMemoryObject::Error::TooManyArchitectures;
    }

    if (!Map.isLargeEnoughForSize(TotalHeaderSize)) {
        return FatMachOMemoryObject::Error::SizeTooSmall;
    }

    const auto IsBigEndian = Header.isBigEndian();
    const auto ArchListBegin = reinterpret_cast<const ArchType *>(&Header + 1);
    const auto ArchListEnd = ArchListBegin + ArchCount;

    for (const auto &Arch : BasicContiguousList(ArchListBegin, ArchListEnd)) {
        const auto ArchRangeOpt = Arch.getFileRange(IsBigEndian);
        if (!ArchRangeOpt.has_value()) {
            return FatMachOMemoryObject::Error::ArchOutOfBounds;
        }

        const auto &ArchRange = ArchRangeOpt.value();
        if (ArchRange.goesPastEnd(Map.size())) {
            return FatMachOMemoryObject::Error::ArchOutOfBounds;
        }

        for (const auto &Inner : BasicContiguousList(ArchListBegin, &Arch)) {
            const auto InnerRange = Inner.getFileRange(IsBigEndian).value();
            if (ArchRange.overlaps(InnerRange)) {
                return FatMachOMemoryObject::Error::ArchOverlapsArch;
            }
        }
    }

    return FatMachOMemoryObject::Error::None;
}

[[nodiscard]] static auto ValidateMap(const ConstMemoryMap &Map) noexcept
    -> FatMachOMemoryObject::Error
{
    if (!Map.isLargeEnoughForType<MachO::FatHeader>()) {
        return FatMachOMemoryObject::Error::SizeTooSmall;
    }

    const auto &Header = *Map.getBeginAs<MachO::FatHeader>();
    if (!Header.hasValidMagic()) {
        return FatMachOMemoryObject::Error::WrongFormat;
    }

    auto Error = FatMachOMemoryObject::Error::None;
    if (Header.is64Bit()) {
        Error = ValidateArchList<PointerKind::s64Bit>(Map, Header);
    } else {
        Error = ValidateArchList<PointerKind::s32Bit>(Map, Header);
    }

    if (Error != FatMachOMemoryObject::Error::None) {
        return Error;
    }

    return FatMachOMemoryObject::Error::None;
}

auto FatMachOMemoryObject::Open(const MemoryMap &Map) noexcept
    -> ExpectedPointer<FatMachOMemoryObject, Error>
{
    const auto Error = ValidateMap(Map);
    if (Error != Error::None) {
        return Error;
    }

    return new FatMachOMemoryObject(Map);
}

FatMachOMemoryObject::FatMachOMemoryObject(
    const MemoryMap &Map) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()) {}

bool FatMachOMemoryObject::errorDidMatchFormat(const Error Error) noexcept {
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
[[nodiscard]] static auto
GetArchInfoAtIndexImpl(const ListType &List,
                       const uint32_t Index,
                       const bool IsBigEndian) noexcept
    -> FatMachOMemoryObject::ArchInfo
{
    auto Info = FatMachOMemoryObject::ArchInfo();
    const auto &Arch = List.at(Index);

    Info.CpuKind = Arch.getCpuKind(IsBigEndian);
    Info.CpuSubKind = Arch.getCpuSubKind(IsBigEndian);

    Info.Offset = Arch.getFileOffset(IsBigEndian);
    Info.Size = Arch.getFileSize(IsBigEndian);
    Info.Align = Arch.getAlign(IsBigEndian);

    return Info;
}

auto
FatMachOMemoryObject::GetArchInfoAtIndex(const uint32_t Index) const noexcept
    -> ArchInfo
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
GetMachOObjectResult(MemoryObject *const ArchObject,
                     const Mach::CpuKind CpuKind,
                     const int32_t CpuSubKind) noexcept
{
    using WarningEnum = FatMachOMemoryObject::GetArchObjectWarning;
    using GetObjectResult = FatMachOMemoryObject::GetArchObjectResult;

    const auto MachOObject = cast<ObjectKind::MachO>(ArchObject);
    const auto ArchCpuKind = MachOObject->getCpuKind();
    const auto ArchCpuSubKind = MachOObject->getCpuSubKind();

    if ((ArchCpuKind != CpuKind) || (ArchCpuSubKind != CpuSubKind)) {
        return GetObjectResult(MachOObject, WarningEnum::MachOCpuKindMismatch);
    }

    return MachOObject;
}

auto
FatMachOMemoryObject::GetArchObjectFromInfo(const ArchInfo &Info) const noexcept
    -> GetArchObjectResult
{
    const auto ArchRange = Range::CreateWithSize(Info.Offset, Info.Size);
    if (!this->getRange().contains(ArchRange)) {
        return GetArchObjectError::InvalidArchRange;
    }

    const auto ArchMap = this->getMap().mapFromLocRange(ArchRange);
    auto Kind = ArchKind::None;

    switch (Kind) {
        case ArchKind::None:
        case ArchKind::MachO:
            const auto ObjectOrError = MachOMemoryObject::Open(ArchMap);
            const auto Error = ObjectOrError.getError();

            if (MachOMemoryObject::errorDidMatchFormat(Error)) {
                const auto Result =
                    GetMachOObjectResult(ObjectOrError.value(),
                                         Info.CpuKind,
                                         Info.CpuSubKind);

                return Result;
            }
    }

    return GetArchObjectError::UnsupportedObjectKind;
}
