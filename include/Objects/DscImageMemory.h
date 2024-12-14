//
//  Objects/DscImageMemory.h
//  ktool
//
//  Created by Suhas Pai on 7/12/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/DyldSharedCache/Headers.h"
#include "MachOMemory.h"

struct DscImageMemoryObject : public MachOMemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::DscImage;
    friend struct DscMemoryObject;
protected:
    MemoryMap DscMap;
    const DyldSharedCache::ImageInfo &ImageInfo;

    DscImageMemoryObject(const MemoryMap &DscMap,
                         const DyldSharedCache::ImageInfo &ImageInfo,
                         uint8_t *Begin,
                         uint8_t *End) noexcept;
public:
    [[nodiscard]]
    static inline auto IsOfKind(const MemoryObject &Obj) noexcept {
        return Obj.getKind() == ObjKind;
    }

    [[nodiscard]] static inline auto classof(const MemoryObject *Obj) noexcept {
        return IsOfKind(*Obj);
    }

    virtual ~DscImageMemoryObject() noexcept = default;

    [[nodiscard]] inline auto &getDscMap() const noexcept {
        return DscMap;
    }

    [[nodiscard]] inline auto getDscRange() const noexcept {
        return this->getDscMap().getRange();
    }

    [[nodiscard]] inline auto &getDscHeader() const noexcept {
        return *this->getDscMap().getBeginAs<const DyldSharedCache::Header>();
    }

    [[nodiscard]] inline auto &getDscHeader() noexcept {
        return *this->getDscMap().getBeginAs<DyldSharedCache::Header>();
    }

    [[nodiscard]] inline auto getDscHeaderV0() const noexcept
        -> const DyldSharedCache::HeaderV0 &
    {
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV1() const noexcept
        -> const DyldSharedCache::HeaderV1 &
    {
        assert(this->getDscHeader().isV1());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV2() const noexcept
        -> const DyldSharedCache::HeaderV2 &
    {
        assert(this->getDscHeader().isV2());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV3() const noexcept
        -> const DyldSharedCache::HeaderV3 &
    {
        assert(this->getDscHeader().isV3());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV4() const noexcept
        -> const DyldSharedCache::HeaderV4 &
    {
        assert(this->getDscHeader().isV4());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV5() const noexcept
        -> const DyldSharedCache::HeaderV5 &
    {
        assert(this->getDscHeader().isV5());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV6() const noexcept
        -> const DyldSharedCache::HeaderV6 &
    {
        assert(this->getDscHeader().isV6());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV7() const noexcept
        -> const DyldSharedCache::HeaderV7 &
    {
        assert(this->getDscHeader().isV7());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV8() const noexcept
        -> const DyldSharedCache::HeaderV8 &
    {
        assert(this->getDscHeader().isV8());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV0() noexcept
        -> DyldSharedCache::HeaderV0 &
    {
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV1() noexcept
        -> DyldSharedCache::HeaderV1 &
    {
        assert(this->getDscHeader().isV1());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV2() noexcept
        -> DyldSharedCache::HeaderV2 &
    {
        assert(this->getDscHeader().isV2());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV3() noexcept
        -> DyldSharedCache::HeaderV3 &
    {
        assert(this->getDscHeader().isV3());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV4() noexcept
        -> DyldSharedCache::HeaderV4 &
    {
        assert(this->getDscHeader().isV4());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV5() noexcept
        -> DyldSharedCache::HeaderV5 &
    {
        assert(this->getDscHeader().isV5());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV6() noexcept
        -> DyldSharedCache::HeaderV6 &
    {
        assert(this->getDscHeader().isV6());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV7() noexcept
        -> DyldSharedCache::HeaderV7 &
    {
        assert(this->getDscHeader().isV7());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto getDscHeaderV8() noexcept
        -> DyldSharedCache::HeaderV8 &
    {
        assert(this->getDscHeader().isV8());
        return this->getDscHeader();
    }

    [[nodiscard]] inline auto &getImageInfo() const noexcept {
        return ImageInfo;
    }

    [[nodiscard]] inline uint32_t getImageIndex() const noexcept {
        const auto ImagesOffset =
            this->getDscHeaderV0().isV8() ?
                this->getDscHeaderV8().ImagesOffset :
                this->getDscHeaderV0().ImagesOffsetOld;

        const auto List =
            reinterpret_cast<const DyldSharedCache::ImageInfo *>(
                this->getDscMap().getBegin() + ImagesOffset);

        return static_cast<uint32_t>(&this->getImageInfo() - List);
    }

    [[nodiscard]] inline auto getFileOffset() const noexcept {
        return this->getMap().getBegin() - this->getDscMap().getBegin();
    }

    [[nodiscard]] inline auto getAddress() const noexcept {
        return this->getImageInfo().Address;
    }

    [[nodiscard]] inline auto
    GetFileOffsetForAddress(const uint64_t Addr,
                            uint64_t *const MaxSizeOut) const noexcept
        -> std::optional<uint64_t>
    {
        return this->getDscHeaderV0().GetFileOffsetForAddress(Addr, MaxSizeOut);
    }

    [[nodiscard]] inline auto getPath() const noexcept {
        return this->getImageInfo().getPath(getDscMap().getBegin());
    }
};
