//
//  src/Objects/DscMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 7/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "ADT/DyldSharedCache/Headers.h"
#include "ADT/LocationRange.h"

#include "Utils/Macros.h"
#include "DscMemory.h"

DscMemoryObject::DscMemoryObject(Error Error) noexcept
: MemoryObject(ObjKind), ErrorStorage(Error) {}

DscMemoryObject::DscMemoryObject(const MemoryMap &Map, CpuKind CpuKind) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()),
  sCpuKind(CpuKind) {}

ConstDscMemoryObject::ConstDscMemoryObject(Error Error) noexcept
: DscMemoryObject(Error) {}

ConstDscMemoryObject::ConstDscMemoryObject(const ConstMemoryMap &Map,
                                           CpuKind CpuKind) noexcept
: DscMemoryObject(reinterpret_cast<const MemoryMap &>(Map), CpuKind) {}

static DscMemoryObject::Error
ValidateMap(const ConstMemoryMap &Map,
            DscMemoryObject::CpuKind &CpuKind) noexcept
{
    if (Map.size() < sizeof(DyldSharedCache::HeaderV0)) {
        return DscMemoryObject::Error::SizeTooSmall;
    }

    const auto Begin = Map.getBeginAs<const char>();
    if (memcmp(Begin, "dyld_v1", LENGTH_OF("dyld_v1")) != 0) {
        return DscMemoryObject::Error::WrongFormat;
    }

    const auto CpuKindStr = Begin + 7;
    constexpr auto CpuKindMaxLength =
        sizeof(DyldSharedCache::HeaderV0::Magic) - 7;

    if (strncmp(CpuKindStr, "    i386", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::i386;
    } else if (strncmp(CpuKindStr, "  x86_64", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::x86_64;
    } else if (strncmp(CpuKindStr, " x86_64h", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::x86_64h;
    } else if (strncmp(CpuKindStr, "   armv5", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Armv5;
    } else if (strncmp(CpuKindStr, "   armv6", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Armv6;
    } else if (strncmp(CpuKindStr, "   armv7", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Armv7;
    } else if (strncmp(CpuKindStr, "  armv7f", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Armv7f;
    } else if (strncmp(CpuKindStr, "  armv7k", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Armv7k;
    } else if (strncmp(CpuKindStr, "  armv7s", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Armv7s;
    } else if (strncmp(CpuKindStr, "  arm64", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Arm64;
    } else if (strncmp(CpuKindStr, "  arm64e", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Arm64e;
    } else if (memcmp(CpuKindStr, "arm64_32", CpuKindMaxLength) == 0) {
        CpuKind = DscMemoryObject::CpuKind::Arm64e;
    } else {
        return DscMemoryObject::Error::UnknownCpuKind;
    }

    const auto Header = Map.getBeginAs<DyldSharedCache::HeaderV0>();
    const auto MappingOffset = Header->MappingOffset;
    const auto AllowedRange =
        LocationRange::CreateWithEnd(MappingOffset, Map.size());

    auto MappingEnd = uint64_t();
    if (DoesMultiplyAndAddOverflow(sizeof(DyldSharedCache::MappingInfo),
                                   Header->MappingCount, MappingOffset,
                                   &MappingEnd))
    {
        return DscMemoryObject::Error::InvalidImageRange;
    }

    const auto MappingRange =
        LocationRange::CreateWithEnd(MappingOffset, MappingEnd);

    if (!AllowedRange.contains(MappingRange)) {
        return DscMemoryObject::Error::InvalidImageRange;
    }

    const auto ImageOffset = Header->ImagesOffset;
    auto ImageEnd = uint64_t();

    if (DoesAddAndMultiplyOverflow(sizeof(DyldSharedCache::ImageInfo),
                                   Header->ImagesCount, Header->ImagesOffset,
                                   &ImageEnd))
    {
        return DscMemoryObject::Error::InvalidImageRange;
    }

    const auto ImageRange = LocationRange::CreateWithEnd(ImageOffset, ImageEnd);
    if (!AllowedRange.contains(ImageRange)) {
        return DscMemoryObject::Error::InvalidImageRange;
    }

    if (MappingRange.overlaps(ImageRange)) {
        return DscMemoryObject::Error::OverlappingImageMappingRange;
    }

    return DscMemoryObject::Error::None;
}

DscMemoryObject DscMemoryObject::Open(const MemoryMap &Map) noexcept {
    auto CpuKind = DscMemoryObject::CpuKind::i386;
    const auto Error = ValidateMap(Map, CpuKind);

    if (Error != DscMemoryObject::Error::None) {
        return Error;
    }

    return DscMemoryObject(Map, CpuKind);
}

bool DscMemoryObject::DidMatchFormat() const noexcept {
    switch (ErrorStorage.getValue()) {
        case Error::None:
            return true;
        case Error::WrongFormat:
        case Error::SizeTooSmall:
            return false;
        case Error::UnknownCpuKind:
        case Error::InvalidMappingRange:
        case Error::InvalidImageRange:
        case Error::OverlappingImageMappingRange:
            return true;
    }
}

MemoryObject *DscMemoryObject::ToPtr() const noexcept {
    return new DscMemoryObject(MemoryMap(Map, End), sCpuKind);
}

DscMemoryObject::Version DscMemoryObject::getVersion() const noexcept {
    auto Version = Version::v0;
    if (HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV6)) {
        Version = Version::v6;
    } else if (HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV5)) {
        Version = Version::v5;
    } else if (HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV4)) {
        Version = Version::v4;
    } else if (HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV3)) {
        Version = Version::v3;
    } else if (HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV2)) {
        Version = Version::v2;
    } else if (HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV1)) {
        Version = Version::v1;
    }

    return Version;
}

const DscMemoryObject &
DscMemoryObject::getCpuKind(Mach::CpuKind &CpuKind,
                            int32_t &CpuSubKind) const noexcept
{
    switch (sCpuKind) {
        case CpuKind::i386:
            CpuKind = Mach::CpuKind::i386;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::i386::All);

            return *this;
        case CpuKind::x86_64:
            CpuKind = Mach::CpuKind::x86_64;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::x86_64::All);

            return *this;
        case CpuKind::x86_64h:
            CpuKind = Mach::CpuKind::x86_64;
            CpuSubKind =
                static_cast<int32_t>(Mach::CpuSubKind::x86_64::Haswell);

            return *this;
        case CpuKind::Armv5:
            CpuKind = Mach::CpuKind::Arm;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm::v5tej);

            return *this;
        case CpuKind::Armv6:
            CpuKind = Mach::CpuKind::Arm;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm::v6);

            return *this;
        case CpuKind::Armv7:
            CpuKind = Mach::CpuKind::Arm;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm::v7);

            return *this;
        case CpuKind::Armv7k:
            CpuKind = Mach::CpuKind::Arm;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm::v7k);

            return *this;
        case CpuKind::Armv7f:
            CpuKind = Mach::CpuKind::Arm;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm::v7f);

            return *this;
        case CpuKind::Armv7s:
            CpuKind = Mach::CpuKind::Arm;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm::v7s);

            return *this;
        case CpuKind::Arm64:
            CpuKind = Mach::CpuKind::Arm64;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm64::All);

            return *this;
        case CpuKind::Arm64e:
            CpuKind = Mach::CpuKind::Arm64;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm64::E);

            return *this;
        case CpuKind::Arm64_32:
            CpuKind = Mach::CpuKind::Arm64_32;
            CpuSubKind = static_cast<int>(Mach::CpuSubKind::Arm64_32::v8);

            return *this;
    }

    assert(0 && "Unrecognized Cpu-Kind");
}
