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

struct DscMemoryObject;
struct ConstDscImageMemoryObject;

struct DscImageMemoryObject : MachOMemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::DscImage;
    friend struct DscMemoryObject;
protected:
    MemoryMap DscMap;

    const DyldSharedCache::ImageInfo &ImageInfo;
    uint32_t ImageIndex;

    DscImageMemoryObject(const MemoryMap &DscMap,
                         const DyldSharedCache::ImageInfo &ImageInfo,
                         uint32_t ImageIndex,
                         uint8_t *Begin,
                         uint8_t *End) noexcept;
public:
    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjectKind::DscImage);
    }

    virtual ~DscImageMemoryObject() noexcept = default;

    [[nodiscard]] inline const MemoryMap &getDscMap() const noexcept {
        return DscMap;
    }

    [[nodiscard]] inline RelativeRange getDscRange() const noexcept {
        return getDscMap().getRange();
    }

    [[nodiscard]]
    inline const ConstDscImageMemoryObject &toConst() const noexcept {
        return reinterpret_cast<const ConstDscImageMemoryObject &>(*this);
    }

    [[nodiscard]] inline ConstDscImageMemoryObject &toConst() noexcept {
        return reinterpret_cast<ConstDscImageMemoryObject &>(*this);
    }

    [[nodiscard]] inline operator ConstDscImageMemoryObject &() noexcept {
        return toConst();
    }

    [[nodiscard]]
    inline operator const ConstDscImageMemoryObject &() const noexcept {
        return toConst();
    }

    [[nodiscard]] DyldSharedCache::Header &getDscHeader() const noexcept {
        return *getDscMap().getBeginAs<DyldSharedCache::Header>();
    }

    [[nodiscard]] DyldSharedCache::HeaderV0 &getDscHeaderV0() const noexcept {
        return getDscHeader();
    }

    [[nodiscard]] DyldSharedCache::HeaderV1 &getDscHeaderV1() const noexcept {
        auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV1));

        return Header;
    }

    [[nodiscard]] DyldSharedCache::HeaderV2 &getDscHeaderV2() const noexcept {
        auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV2));

        return Header;
    }

    [[nodiscard]] DyldSharedCache::HeaderV3 &getDscHeaderV3() const noexcept {
        auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV3));

        return Header;
    }

    [[nodiscard]] DyldSharedCache::HeaderV4 &getDscHeaderV4() const noexcept {
        auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV4));

        return Header;
    }

    [[nodiscard]] DyldSharedCache::HeaderV5 &getDscHeaderV5() const noexcept {
        auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV5));

        return Header;
    }

    [[nodiscard]] DyldSharedCache::HeaderV6 &getDscHeaderV6() const noexcept {
        auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV6));

        return Header;
    }

    [[nodiscard]]
    inline const DyldSharedCache::ImageInfo &getImageInfo() const noexcept {
        return ImageInfo;
    }

    [[nodiscard]] inline uint32_t getImageIndex() const noexcept {
        return ImageIndex;
    }

    [[nodiscard]] inline uint64_t getBaseOffset() const noexcept {
        return Map - getDscMap().getBegin();
    }

    [[nodiscard]] inline uint64_t getAddress() const noexcept {
        return getImageInfo().Address;
    }

    [[nodiscard]] inline std::optional<uint64_t>
    GetFileOffsetForAddress(uint64_t Addr,
                            uint64_t *MaxSizeOut) const noexcept
    {
        return getDscHeaderV0().GetFileOffsetForAddress(Addr, MaxSizeOut);
    }

    [[nodiscard]] inline const char *getPath() const noexcept {
        return getImageInfo().getPath(getDscMap().getBegin());
    }
};

struct ConstDscImageMemoryObject : DscImageMemoryObject {
public:
    friend struct DscMemoryObject;
protected:
    ConstDscImageMemoryObject(const ConstMemoryMap &DscMap,
                              const DyldSharedCache::ImageInfo &ImageInfo,
                              uint32_t ImageIndex,
                              const uint8_t *Begin,
                              const uint8_t *End) noexcept;
public:
    [[nodiscard]] inline const ConstMemoryMap &getDscMap() const noexcept {
        return DscMap;
    }

    [[nodiscard]]
    const DyldSharedCache::Header &getDscHeader() const noexcept {
        return *getDscMap().getBeginAs<const DyldSharedCache::Header>();
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV0 &getDscHeaderV0() const noexcept {
        return getDscHeader();
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV1 &getDscHeaderV1() const noexcept {
        const auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV1));

        return Header;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV2 &getDscHeaderV2() const noexcept {
        const auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV2));

        return Header;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV3 &getDscHeaderV3() const noexcept {
        const auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV3));

        return Header;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV4 &getDscHeaderV4() const noexcept {
        const auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV4));

        return Header;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV5 &getDscHeaderV5() const noexcept {
        const auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV5));

        return Header;
    }

    [[nodiscard]]
    const DyldSharedCache::HeaderV6 &getDscHeaderV6() const noexcept {
        const auto &Header = getDscHeader();
        assert(Header.MappingOffset >= sizeof(DyldSharedCache::HeaderV6));

        return Header;
    }
};
