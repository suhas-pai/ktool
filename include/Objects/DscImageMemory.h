//
//  include/Objects/DscImageMemory.h
//  ktool
//
//  Created by Suhas Pai on 7/12/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/DyldSharedCache.h"
#include "MachOMemory.h"

struct ConstDscImageMemoryObject : public ConstMachOMemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::DscImage;
    friend struct ConstDscMemoryObject;
protected:
    ConstMemoryMap DscMap;
    const DyldSharedCache::ImageInfo &ImageInfo;

    ConstDscImageMemoryObject(const ConstMemoryMap &DscMap,
                              const DyldSharedCache::ImageInfo &ImageInfo,
                              const uint8_t *Begin,
                              const uint8_t *End) noexcept;
public:
    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjKind);
    }

    [[nodiscard]] static inline bool classof(const MemoryObject *Obj) noexcept {
        return IsOfKind(*Obj);
    }

    virtual ~ConstDscImageMemoryObject() noexcept = default;

    [[nodiscard]] inline RelativeRange getDscRange() const noexcept {
        return getDscMap().getRange();
    }

    [[nodiscard]] inline const ConstMemoryMap &getDscMap() const noexcept {
        return DscMap;
    }

    [[nodiscard]]
    inline const DyldSharedCache::Header &getDscHeader() const noexcept {
        return *getDscMap().getBeginAs<const DyldSharedCache::Header>();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV0 &getDscHeaderV0() const noexcept {
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV1 &getDscHeaderV1() const noexcept {
        assert(getDscHeader().isV1());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV2 &getDscHeaderV2() const noexcept {
        assert(getDscHeader().isV2());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV3 &getDscHeaderV3() const noexcept {
        assert(getDscHeader().isV3());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV4 &getDscHeaderV4() const noexcept {
        assert(getDscHeader().isV4());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV5 &getDscHeaderV5() const noexcept {
        assert(getDscHeader().isV5());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV6 &getDscHeaderV6() const noexcept {
        assert(getDscHeader().isV6());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV7 &getDscHeaderV7() const noexcept {
        assert(getDscHeader().isV7());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV8 &getDscHeaderV8() const noexcept {
        assert(getDscHeader().isV8());
        return getDscHeader();
    }

    [[nodiscard]]
    inline const DyldSharedCache::ImageInfo &getImageInfo() const noexcept {
        return ImageInfo;
    }

    [[nodiscard]] inline uint32_t getImageIndex() const noexcept {
        const auto ImagesOffset =
            getDscHeaderV0().isV8() ?
                getDscHeaderV8().ImagesOffset :
                getDscHeaderV0().ImagesOffsetOld;

        const auto List =
            reinterpret_cast<const DyldSharedCache::ImageInfo *>(
                getDscMap().getBegin() + ImagesOffset);

        return static_cast<uint32_t>(&getImageInfo() - List);
    }

    [[nodiscard]] inline uint64_t getFileOffset() const noexcept {
        return getMap().getBegin() - getDscMap().getBegin();
    }

    [[nodiscard]] inline uint64_t getAddress() const noexcept {
        return getImageInfo().Address;
    }

    [[nodiscard]] inline std::optional<uint64_t>
    GetFileOffsetForAddress(const uint64_t Addr,
                            uint64_t *const MaxSizeOut) const noexcept
    {
        return getDscHeaderV0().GetFileOffsetForAddress(Addr, MaxSizeOut);
    }

    [[nodiscard]] inline const char *getPath() const noexcept {
        return getImageInfo().getPath(getDscMap().getBegin());
    }
};

struct DscImageMemoryObject : public ConstDscImageMemoryObject {
protected:
    DscImageMemoryObject(const MemoryMap &DscMap,
                         const DyldSharedCache::ImageInfo &ImageInfo,
                         uint8_t *Begin,
                         uint8_t *End) noexcept;
public:
    [[nodiscard]]
    inline DyldSharedCache::Header &getDscHeader() const noexcept {
        const auto &Result = ConstDscImageMemoryObject::getDscHeader();
        return const_cast<DyldSharedCache::Header &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV0 &getDscHeaderV0() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV0();
        return const_cast<DyldSharedCache::HeaderV0 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV1 &getDscHeaderV1() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV1();
        return const_cast<DyldSharedCache::HeaderV1 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV2 &getDscHeaderV2() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV2();
        return const_cast<DyldSharedCache::HeaderV2 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV3 &getDscHeaderV3() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV3();
        return const_cast<DyldSharedCache::HeaderV3 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV4 &getDscHeaderV4() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV4();
        return const_cast<DyldSharedCache::HeaderV4 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV5 &getDscHeaderV5() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV5();
        return const_cast<DyldSharedCache::HeaderV5 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV6 &getDscHeaderV6() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV6();
        return const_cast<DyldSharedCache::HeaderV6 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV7 &getDscHeaderV7() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV7();
        return const_cast<DyldSharedCache::HeaderV7 &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV8 &getDscHeaderV8() const noexcept {
        auto &Result = ConstDscImageMemoryObject::getDscHeaderV8();
        return const_cast<DyldSharedCache::HeaderV8 &>(Result);
    }

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline MachO::Header &getHeader() noexcept {
        return const_cast<MachO::Header &>(getConstHeader());
    }

    [[nodiscard]] inline MachO::LoadCommandStorage
    GetLoadCommands(const bool Verify = true) noexcept {
        auto Result = ConstMachOMemoryObject::GetLoadCommandsStorage(Verify);
        return *reinterpret_cast<const MachO::LoadCommandStorage *>(&Result);
    }
};

