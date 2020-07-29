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

    virtual ~ConstDscImageMemoryObject() noexcept = default;

    [[nodiscard]] inline RelativeRange getDscRange() const noexcept {
        return getDscMap().getRange();
    }

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

    [[nodiscard]]
    inline const DyldSharedCache::ImageInfo &getImageInfo() const noexcept {
        return ImageInfo;
    }

    [[nodiscard]] inline uint32_t getImageIndex() const noexcept {
        const auto List =
            reinterpret_cast<const DyldSharedCache::ImageInfo *>(
                DscMap.getBegin() + getDscHeaderV0().ImagesOffset);

        return static_cast<uint32_t>(&ImageInfo - List);
    }

    [[nodiscard]] inline uint64_t getFileOffset() const noexcept {
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

struct DscImageMemoryObject : public ConstDscImageMemoryObject {
protected:
    DscImageMemoryObject(const MemoryMap &DscMap,
                         const DyldSharedCache::ImageInfo &ImageInfo,
                         uint8_t *Begin,
                         uint8_t *End) noexcept;
public:
    [[nodiscard]] DyldSharedCache::Header &getDscHeader() const noexcept {
        const auto &Result = ConstDscImageMemoryObject::getDscHeader();
        return const_cast<DyldSharedCache::Header &>(Result);
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

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]]
    inline DyldSharedCache::ImageInfo &getImageInfo() const noexcept {
        return const_cast<DyldSharedCache::ImageInfo &>(ImageInfo);
    }

    [[nodiscard]] inline MachO::Header &getHeader() noexcept { return *Header; }

    [[nodiscard]] inline MachO::LoadCommandStorage
    GetLoadCommands(bool Verify = true) noexcept {
        auto Result = ConstMachOMemoryObject::GetLoadCommands(Verify);
        return *reinterpret_cast<const MachO::LoadCommandStorage *>(&Result);
    }
};

