//
//  include/Objects/DscMemory.h
//  ktool
//
//  Created by Suhas Pai on 7/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/PointerErrorStorage.h"
#include "ADT/DyldSharedCache.h"
#include "ADT/Mach.h"

#include "MemoryBase.h"

struct ConstDscMemoryObject;
struct DscMemoryObject : public MemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::DyldSharedCache;

    enum class CpuKind {
        i386,
        x86_64,
        x86_64h,
        Armv5,
        Armv6,
        Armv7,
        Armv7k,
        Armv7f,
        Armv7s,
        Arm64,
        Arm64e,
        Arm64_32,
    };

    enum class Version {
        v0,
        v1,
        v2,
        v3,
        v4,
        v5,
        v6,
        v7
    };

    enum class Error : uintptr_t {
        None,

        WrongFormat,
        SizeTooSmall,

        UnknownCpuKind,

        InvalidMappingRange,
        InvalidImageRange,
        OverlappingImageMappingRange,
    };
protected:
    union {
        uint8_t *Map;

        DyldSharedCache::Header *Header;
        DyldSharedCache::HeaderV0 *HeaderV0;
        DyldSharedCache::HeaderV1 *HeaderV1;
        DyldSharedCache::HeaderV2 *HeaderV2;
        DyldSharedCache::HeaderV3 *HeaderV3;
        DyldSharedCache::HeaderV4 *HeaderV4;
        DyldSharedCache::HeaderV5 *HeaderV5;
        DyldSharedCache::HeaderV6 *HeaderV6;

        PointerErrorStorage<Error> ErrorStorage;
    };

    uint8_t *End;
    CpuKind sCpuKind;

    DscMemoryObject(Error Error) noexcept;
    explicit DscMemoryObject(const MemoryMap &Map, CpuKind CpuKind) noexcept;
public:
    [[nodiscard]] static DscMemoryObject Open(const MemoryMap &Map) noexcept;
    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjectKind::DyldSharedCache);
    }

    [[nodiscard]] bool DidMatchFormat() const noexcept override;
    [[nodiscard]] MemoryObject *ToPtr() const noexcept override;

    [[nodiscard]] inline bool hasError() const noexcept override {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] Error getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] inline const ConstDscMemoryObject &toConst() const noexcept {
        return reinterpret_cast<const ConstDscMemoryObject &>(*this);
    }

    [[nodiscard]] inline ConstDscMemoryObject &toConst() noexcept {
        return reinterpret_cast<ConstDscMemoryObject &>(*this);
    }

    [[nodiscard]] inline operator ConstDscMemoryObject &() noexcept {
        return toConst();
    }

    [[nodiscard]]
    inline operator const ConstDscMemoryObject &() const noexcept {
        return toConst();
    }

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        return MemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        return RelativeRange(End - Map);
    }

    [[nodiscard]] DyldSharedCache::Header &getHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] DyldSharedCache::HeaderV0 &getHeaderV0() const noexcept {
        return *HeaderV0;
    }

    [[nodiscard]] DyldSharedCache::HeaderV1 &getHeaderV1() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV1));
        return *HeaderV1;
    }

    [[nodiscard]] DyldSharedCache::HeaderV2 &getHeaderV2() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV2));
        return *HeaderV2;
    }

    [[nodiscard]] DyldSharedCache::HeaderV3 &getHeaderV3() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV3));
        return *HeaderV3;
    }

    [[nodiscard]] DyldSharedCache::HeaderV4 &getHeaderV4() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV4));
        return *HeaderV4;
    }

    [[nodiscard]] DyldSharedCache::HeaderV5 &getHeaderV5() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV5));
        return *HeaderV5;
    }

    [[nodiscard]] DyldSharedCache::HeaderV6 &getHeaderV6() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV6));
        return *HeaderV6;
    }

    [[nodiscard]] Version getVersion() const noexcept;

    const DscMemoryObject &
    getCpuKind(Mach::CpuKind &CpuKind, int32_t &CpuSubKind) const noexcept;
};

struct ConstDscMemoryObject : public DscMemoryObject {
protected:
    ConstDscMemoryObject(Error Error) noexcept;

    explicit
    ConstDscMemoryObject(const ConstMemoryMap &Map, CpuKind CpuKind) noexcept;
public:
    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] const DyldSharedCache::Header &getHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV0 &getHeaderV0() const noexcept {
        return *HeaderV0;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV1 &getHeaderV1() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV1));
        return *HeaderV1;
    }

    [[nodiscard]] DyldSharedCache::HeaderV2 &getHeaderV2() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV2));
        return *HeaderV2;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV3 &getHeaderV3() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV3));
        return *HeaderV3;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV4 &getHeaderV4() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV4));
        return *HeaderV4;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV5 &getHeaderV5() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV5));
        return *HeaderV5;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV6 &getHeaderV6() const noexcept {
        assert(HeaderV0->MappingOffset >= sizeof(DyldSharedCache::HeaderV6));
        return *HeaderV6;
    }
};
