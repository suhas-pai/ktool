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

#include "DscImageMemory.h"
#include "MemoryBase.h"

struct ConstDscMemoryObject : public MemoryObject {
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
        const uint8_t *Map;
        const DyldSharedCache::Header *Header;
        PointerErrorStorage<Error> ErrorStorage;
    };

    const uint8_t *End;
    CpuKind sCpuKind;
    ConstDscMemoryObject(Error Error) noexcept;

    explicit
    ConstDscMemoryObject(const ConstMemoryMap &Map, CpuKind CpuKind) noexcept;
public:
    [[nodiscard]]
    static ConstDscMemoryObject Open(const ConstMemoryMap &Map) noexcept;

    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjKind);
    }

    [[nodiscard]] bool DidMatchFormat() const noexcept override;
    [[nodiscard]] MemoryObject *ToPtr() const noexcept override;

    [[nodiscard]] inline bool hasError() const noexcept override {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] inline Error getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        assert(!hasError());
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return getMap();
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        assert(!hasError());
        return RelativeRange(End - Map);
    }

    [[nodiscard]]
    inline const DyldSharedCache::Header &getHeader() const noexcept {
        assert(!hasError());
        return *Header;
    }

    [[nodiscard]]
    inline const DyldSharedCache::HeaderV0 &getHeaderV0() const noexcept {
        return getHeader();
    }

    [[nodiscard]] inline
    const DyldSharedCache::HeaderV1 &getHeaderV1() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV1));
        return getHeader();
    }

    [[nodiscard]] inline
    const DyldSharedCache::HeaderV2 &getHeaderV2() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV2));
        return getHeader();
    }

    [[nodiscard]] inline
    const DyldSharedCache::HeaderV3 &getHeaderV3() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV3));
        return getHeader();
    }

    [[nodiscard]] inline
    const DyldSharedCache::HeaderV4 &getHeaderV4() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV4));
        return getHeader();
    }

    [[nodiscard]] inline
    const DyldSharedCache::HeaderV5 &getHeaderV5() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV5));
        return getHeader();
    }

    [[nodiscard]] inline
    const DyldSharedCache::HeaderV6 &getHeaderV6() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV6));
        return getHeader();
    }

    [[nodiscard]] inline uint32_t getImageCount() const noexcept {
        return getHeaderV0().ImagesCount;
    }

    [[nodiscard]] inline uint32_t getMappingCount() const noexcept {
        return getHeaderV0().MappingCount;
    }

    [[nodiscard]] Version getVersion() const noexcept;

    const ConstDscMemoryObject &
    getCpuKind(Mach::CpuKind &CpuKind, int32_t &CpuSubKind) const noexcept;

    [[nodiscard]] inline
    BasicContiguousList<const DyldSharedCache::ImageInfo>
    getImageInfoList() const noexcept {
        return getHeaderV0().getImageInfoList();
    }

    [[nodiscard]] inline
    BasicContiguousList<const DyldSharedCache::ImageInfo>
    getConstImageInfoList() const noexcept {
        return getImageInfoList();
    }

    [[nodiscard]]
    inline BasicContiguousList<const DyldSharedCache::MappingInfo>
    getMappingInfoList() const noexcept {
        return getHeaderV0().getMappingInfoList();
    }

    [[nodiscard]]
    inline BasicContiguousList<const DyldSharedCache::MappingInfo>
    getConstMappingInfoList() const noexcept {
        return getMappingInfoList();
    }

    [[nodiscard]] inline std::optional<uint64_t>
    GetFileOffsetForAddress(uint64_t Addr,
                            uint64_t *MaxSizeOut = nullptr) const noexcept
    {
        return getHeaderV0().GetFileOffsetForAddress(Addr, MaxSizeOut);
    }

    template <typename T = uint8_t>
    [[nodiscard]]
    inline const T *GetPtrForAddress(uint64_t Address) const noexcept {
        return getHeaderV0().GetPtrForAddress<T>(Address);
    }

    [[nodiscard]] inline const DyldSharedCache::ImageInfo &
    getImageInfoAtIndex(uint32_t Index) const noexcept {
        return getConstImageInfoList().at(Index);
    }

    [[nodiscard]] inline const DyldSharedCache::ImageInfo &
    getConstImageInfoAtIndex(uint32_t Index) const noexcept {
        return getImageInfoAtIndex(Index);
    }

    const DyldSharedCache::ImageInfo *
    GetImageInfoWithPath(const std::string_view &Path) const noexcept;

    ConstDscImageMemoryObject *
    GetImageWithInfo(const DyldSharedCache::ImageInfo &Info) const noexcept;
};

struct DscMemoryObject : public ConstDscMemoryObject {
protected:
    DscMemoryObject(Error Error) noexcept;
    explicit DscMemoryObject(const MemoryMap &Map, CpuKind CpuKind) noexcept;
public:
    [[nodiscard]] static DscMemoryObject Open(const MemoryMap &Map) noexcept;

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        assert(!hasError());

        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline DyldSharedCache::Header &getHeader() const noexcept {
        auto &Result = ConstDscMemoryObject::getHeader();
        return const_cast<DyldSharedCache::Header &>(Result);
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV0 &getHeaderV0() const noexcept {
        return getHeader();
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV1 &getHeaderV1() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV1));
        return getHeader();
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV2 &getHeaderV2() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV2));
        return getHeader();
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV3 &getHeaderV3() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV3));
        return getHeader();
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV4 &getHeaderV4() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV4));
        return getHeader();
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV5 &getHeaderV5() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV5));
        return getHeader();
    }

    [[nodiscard]]
    inline DyldSharedCache::HeaderV6 &getHeaderV6() const noexcept {
        assert(getHeader().MappingOffset >= sizeof(DyldSharedCache::HeaderV6));
        return getHeader();
    }

    [[nodiscard]] inline
    DyldSharedCache::ImageList getImageInfoList() const noexcept {
        return getHeaderV0().getImageInfoList();
    }

    [[nodiscard]] inline
    DyldSharedCache::MappingList getMappingInfoList() const noexcept {
        return getHeaderV0().getMappingInfoList();
    }

    [[nodiscard]] inline
    DyldSharedCache::ConstImageList getConstImageInfoList() const noexcept {
        return getHeaderV0().getConstImageInfoList();
    }

    [[nodiscard]] inline
    DyldSharedCache::ConstMappingList getConstMappingInfoList() const noexcept {
        return getHeaderV0().getConstMappingInfoList();
    }

    inline const DscMemoryObject &
    getCpuKind(Mach::CpuKind &CpuKind, int32_t &CpuSubKind) const noexcept {
        ConstDscMemoryObject::getCpuKind(CpuKind, CpuSubKind);
        return *this;
    }

    template <typename T = uint8_t>
    [[nodiscard]] inline T *GetPtrForAddress(uint64_t Address) const noexcept {
        return getHeaderV0().GetPtrForAddress<T>(Address);
    }

    [[nodiscard]] inline const DyldSharedCache::ImageInfo &
    getImageInfoAtIndex(uint32_t Index) const noexcept {
        return ConstDscMemoryObject::getConstImageInfoAtIndex(Index);
    }

    inline DyldSharedCache::ImageInfo *
    GetImageInfoWithPath(const std::string_view &Path) const noexcept {
        const auto Result = ConstDscMemoryObject::GetImageInfoWithPath(Path);
        return const_cast<DyldSharedCache::ImageInfo *>(Result);
    }

    [[nodiscard]] inline DscImageMemoryObject *
    GetImageWithInfo(const DyldSharedCache::ImageInfo &Info) const noexcept {
        const auto Result = ConstDscMemoryObject::GetImageWithInfo(Info);
        return reinterpret_cast<DscImageMemoryObject *>(Result);
    }
};
