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

ConstDscMemoryObject::ConstDscMemoryObject(const ConstMemoryMap &Map,
                                           const CpuKind CpuKind) noexcept
: MemoryObject(ObjKind), Map(Map.getBegin()), End(Map.getEnd()),
  sCpuKind(CpuKind) {}

DscMemoryObject::DscMemoryObject(const MemoryMap &Map,
                                 const CpuKind CpuKind) noexcept
: ConstDscMemoryObject(Map, CpuKind) {}

[[nodiscard]] static ConstDscMemoryObject::Error
ValidateMap(const ConstMemoryMap &Map,
            ConstDscMemoryObject::CpuKind &CpuKind) noexcept
{
    if (!Map.isLargeEnoughForType<DyldSharedCache::HeaderV0>()) {
        return ConstDscMemoryObject::Error::SizeTooSmall;
    }

    constexpr auto MagicStart = std::string_view("dyld_v1");
    const auto Magic = Map.getBeginAs<const char>();

    if (memcmp(Magic, MagicStart.data(), MagicStart.length()) != 0) {
        return ConstDscMemoryObject::Error::WrongFormat;
    }

    const auto CpuKindStr = Magic + MagicStart.length();
    constexpr auto CpuKindMaxLength =
        sizeof(DyldSharedCache::HeaderV0::Magic) - MagicStart.length();

    if (strncmp(CpuKindStr, "    i386", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::i386;
    } else if (strncmp(CpuKindStr, "  x86_64", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::x86_64;
    } else if (strncmp(CpuKindStr, " x86_64h", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::x86_64h;
    } else if (strncmp(CpuKindStr, "   armv5", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Armv5;
    } else if (strncmp(CpuKindStr, "   armv6", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Armv6;
    } else if (strncmp(CpuKindStr, "   armv7", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Armv7;
    } else if (strncmp(CpuKindStr, "  armv7f", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Armv7f;
    } else if (strncmp(CpuKindStr, "  armv7k", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Armv7k;
    } else if (strncmp(CpuKindStr, "  armv7s", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Armv7s;
    } else if (strncmp(CpuKindStr, "  arm64", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Arm64;
    } else if (strncmp(CpuKindStr, "  arm64e", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Arm64e;
    } else if (memcmp(CpuKindStr, "arm64_32", CpuKindMaxLength) == 0) {
        CpuKind = ConstDscMemoryObject::CpuKind::Arm64_32;
    } else {
        return ConstDscMemoryObject::Error::UnknownCpuKind;
    }

    auto MappingEnd = uint64_t();

    const auto &Header = *Map.getBeginAs<DyldSharedCache::HeaderV8>();
    const auto MappingOffset = Header.MappingOffset;
    const auto AllowedRange =
        LocationRange::CreateWithEnd(MappingOffset, Map.size());

    if (DoesMultiplyAndAddOverflow(sizeof(DyldSharedCache::MappingInfo),
                                   Header.MappingCount, MappingOffset,
                                   &MappingEnd))
    {
        return ConstDscMemoryObject::Error::InvalidMappingInfoListRange;
    }

    const auto MappingRange =
        LocationRange::CreateWithEnd(MappingOffset, MappingEnd);

    if (!AllowedRange.contains(MappingRange)) {
        return ConstDscMemoryObject::Error::InvalidMappingInfoListRange;
    }

    const auto ImagesOffset =
        Header.isV8() ? Header.ImagesOffset : Header.ImagesOffsetOld;
    const auto ImageCount =
        Header.isV8() ? Header.ImagesCount : Header.ImagesCountOld;

    auto ImageEnd = uint64_t();
    if (DoesAddAndMultiplyOverflow(sizeof(DyldSharedCache::ImageInfo),
                                   ImageCount, ImagesOffset,
                                   &ImageEnd))
    {
        return ConstDscMemoryObject::Error::InvalidImageInfoListRange;
    }

    const auto ImageRange =
        LocationRange::CreateWithEnd(ImagesOffset, ImageEnd);

    if (!AllowedRange.contains(ImageRange)) {
        return ConstDscMemoryObject::Error::InvalidImageInfoListRange;
    }

    if (MappingRange.overlaps(ImageRange)) {
        using Error = ConstDscMemoryObject::Error;
        return Error::OverlappingImageMappingInfoListRange;
    }

    return ConstDscMemoryObject::Error::None;
}

auto
ConstDscMemoryObject::Open(const ConstMemoryMap &Map) noexcept ->
    PointerOrError<ConstDscMemoryObject, Error>
{
    auto CpuKind = ConstDscMemoryObject::CpuKind::i386;
    const auto Error = ValidateMap(Map, CpuKind);

    if (Error != Error::None) {
        return Error;
    }

    return new ConstDscMemoryObject(Map, CpuKind);
}

bool ConstDscMemoryObject::errorDidMatchFormat(const Error Error) noexcept {
    switch (Error) {
        case Error::None:
            return true;
        case Error::WrongFormat:
        case Error::SizeTooSmall:
            return false;
        case Error::UnknownCpuKind:
        case Error::InvalidMappingInfoListRange:
        case Error::InvalidImageInfoListRange:
        case Error::OverlappingImageMappingInfoListRange:
            return true;
    }
}

const ConstDscMemoryObject &
ConstDscMemoryObject::getCpuKind(Mach::CpuKind &CpuKind,
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

auto
ConstDscMemoryObject::ValidateImageMapAndGetEnd(
    const ConstMemoryMap &Map) noexcept ->
        PointerOrError<const uint8_t, DscImageOpenError>
{
    const auto ValidateError = ConstMachOMemoryObject::ValidateMap(Map);
    switch (ValidateError) {
        case ConstMachOMemoryObject::Error::None:
            break;
        case ConstMachOMemoryObject::Error::WrongFormat:
            return DscImageOpenError::NotAMachO;
        case ConstMachOMemoryObject::Error::SizeTooSmall:
            return DscImageOpenError::InvalidMachO;
        case ConstMachOMemoryObject::Error::TooManyLoadCommands:
            return DscImageOpenError::InvalidLoadCommands;
    }

    const auto &Header = *Map.getBeginAs<MachO::Header>();
    if (Header.getFileKind() != MachO::Header::FileKind::Dylib) {
        return DscImageOpenError::NotADylib;
    }

    if (!Header.getFlags().hasFlag(MachO::Header::FlagsEnum::DylibInCache)) {
        return DscImageOpenError::NotMarkedAsImage;
    }

    const auto Is64Bit = Header.is64Bit();
    const auto IsBE = Header.isBigEndian();
    const auto LoadCmdStorage = Header.GetConstLoadCmdStorage();

    if (LoadCmdStorage.hasError()) {
        return DscImageOpenError::InvalidLoadCommands;
    }

    auto End = Map.getBegin();
    if (Is64Bit) {
        for (const auto &LC : LoadCmdStorage) {
            if (const auto *Seg = dyn_cast<MachO::SegmentCommand64>(LC, IsBE)) {
                if (DoesAddOverflow(End, Seg->FileSize, &End)) {
                    return DscImageOpenError::SizeTooLarge;
                }
            }
        }
    } else {
        for (const auto &LC : LoadCmdStorage) {
            if (const auto *Seg = dyn_cast<MachO::SegmentCommand>(LC, IsBE)) {
                End += Seg->FileSize;
            }
        }
    }

    if (!Map.containsEndPtr(End)) {
        return DscImageOpenError::SizeTooLarge;
    }

    return End;
}

const DyldSharedCache::ImageInfo *
ConstDscMemoryObject::GetImageInfoWithPath(
    const std::string_view Path) const noexcept
{
    const auto Map = getMap().getBegin();
    for (const auto &ImageInfo : getConstImageInfoList()) {
        if (Path == ImageInfo.getPath(Map)) {
            return &ImageInfo;
        }
    }

    return nullptr;
}

auto
ConstDscMemoryObject::GetImageWithInfo(
    const DyldSharedCache::ImageInfo &ImageInfo) const noexcept ->
        PointerOrError<ConstDscImageMemoryObject, DscImageOpenError>
{
    if (const auto Ptr = GetPtrForAddress(ImageInfo.Address)) {
        const auto Map = getMap();
        const auto EndOrError = ValidateImageMapAndGetEnd(Map.mapFromPtr(Ptr));

        if (EndOrError.hasPtr()) {
            const auto End = EndOrError.getPtr();
            return new ConstDscImageMemoryObject(Map, ImageInfo, Ptr, End);
        }
    }

    return DscImageOpenError::InvalidAddress;
}
