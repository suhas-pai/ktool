//
//  Objects/DscMemory.h
//  ktool
//
//  Created by Suhas Pai on 7/10/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/DyldSharedCache/Headers.h"
#include "ADT/ExpectedPointer.h"

#include "ADT/Mach/Info.h"
#include "MemoryBase.h"

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

    enum class Error : uintptr_t {
        None,

        WrongFormat,
        SizeTooSmall,

        UnknownCpuKind,

        InvalidMappingInfoListRange,
        InvalidImageInfoListRange,
        OverlappingImageMappingInfoListRange,
    };

    enum class DscImageOpenError {
        None,
        InvalidAddress,
        NotAMachO,
        InvalidMachO,
        InvalidLoadCommands,
        NotADylib,
        NotMarkedAsImage,
        SizeTooLarge,
    };
protected:
    union {
        uint8_t *Map;
        DyldSharedCache::Header *Header;
    };

    uint8_t *End;

    [[nodiscard]] static auto
    ValidateImageMapAndGetEnd(const ConstMemoryMap &Map) noexcept
        -> ExpectedPointer<const uint8_t, DscImageOpenError>;

    CpuKind sCpuKind;

    explicit DscMemoryObject(const MemoryMap &Map, CpuKind CpuKind) noexcept;

    [[nodiscard]] constexpr auto getMutMap() const noexcept {
        return MemoryMap(Map, End);
    }
public:
    [[nodiscard]] static auto Open(const MemoryMap &Map) noexcept
        -> ExpectedPointer<DscMemoryObject, Error>;

    [[nodiscard]]
    static inline auto IsOfKind(const MemoryObject &Obj) noexcept {
        return Obj.getKind() == ObjKind;
    }

    [[nodiscard]] static auto errorDidMatchFormat(const Error Error) noexcept
        -> bool;

    [[nodiscard]]
    static inline auto getErrorFromInt(const uint8_t Int) noexcept {
        return static_cast<Error>(Int);
    }

    [[nodiscard]]
    static inline auto errorDidMatchFormat(const uint8_t Int) noexcept {
        return errorDidMatchFormat(getErrorFromInt(Int));
    }

    [[nodiscard]] inline auto getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline auto getMap() noexcept {
        return MemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return this->getMap();
    }

    [[nodiscard]] inline Range getRange() const noexcept override {
        return this->getMap().getRange();
    }

    [[nodiscard]] inline auto &getHeader() const noexcept {
        return *this->Header;
    }

    [[nodiscard]] inline auto &getHeader() noexcept {
        return *this->Header;
    }

    [[nodiscard]] inline const auto &getConstHeader() const noexcept {
        return this->getHeader();
    }

    [[nodiscard]] inline auto getHeaderV0() const noexcept
        -> const DyldSharedCache::HeaderV0 &
    {
        return this->getHeader();
    }

    [[nodiscard]] inline auto getHeaderV1() const noexcept
        -> const DyldSharedCache::HeaderV1 &
    {
        assert(this->getHeader().isV1());
        return this->getHeader();
    }

    [[nodiscard]] inline auto getHeaderV2() const noexcept
        -> const DyldSharedCache::HeaderV2 &
    {
        assert(this->getHeader().isV2());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV3() const noexcept
        -> const DyldSharedCache::HeaderV3 &
    {
        assert(this->getHeader().isV3());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV4() const noexcept
        -> const DyldSharedCache::HeaderV4 &
    {
        assert(this->getHeader().isV4());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV5() const noexcept
        -> const DyldSharedCache::HeaderV5 &
    {
        assert(this->getHeader().isV5());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV6() const noexcept
        -> const DyldSharedCache::HeaderV6 &
    {
        assert(this->getHeader().isV6());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV7() const noexcept
        -> const DyldSharedCache::HeaderV7 &
    {
        assert(this->getHeader().isV7());
        return this->getHeader();
    }

    [[nodiscard]] inline auto getHeaderV8() const noexcept
        -> const DyldSharedCache::HeaderV8 &
    {
        assert(this->getHeader().isV8());
        return this->getHeader();
    }

    [[nodiscard]] auto getHeaderV0() noexcept -> DyldSharedCache::HeaderV0 & {
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV1() noexcept -> DyldSharedCache::HeaderV1 & {
        assert(this->getHeader().isV1());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV2() noexcept -> DyldSharedCache::HeaderV2 & {
        assert(this->getHeader().isV2());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV3() noexcept -> DyldSharedCache::HeaderV3 & {
        assert(this->getHeader().isV3());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV4() noexcept -> DyldSharedCache::HeaderV4 & {
        assert(this->getHeader().isV4());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV5() noexcept -> DyldSharedCache::HeaderV5 & {
        assert(this->getHeader().isV5());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV6() noexcept -> DyldSharedCache::HeaderV6 & {
        assert(this->getHeader().isV6());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV7() noexcept -> DyldSharedCache::HeaderV7 & {
        assert(this->getHeader().isV7());
        return this->getHeader();
    }

    [[nodiscard]]
    inline auto getHeaderV8() noexcept -> DyldSharedCache::HeaderV8 & {
        assert(this->getHeader().isV8());
        return this->getHeader();
    }

    [[nodiscard]] inline auto getImagesOffset() const noexcept {
        if (this->getHeader().isV8()) {
            return this->getHeaderV8().ImagesOffset;
        }

        return this->getHeaderV8().ImagesOffsetOld;
    }

    [[nodiscard]] inline auto getImageCount() const noexcept {
        if (this->getHeader().isV8()) {
            return this->getHeaderV8().ImagesCount;
        }

        return this->getHeaderV8().ImagesCountOld;
    }

    [[nodiscard]] inline auto getMappingCount() const noexcept {
        return this->getHeaderV0().MappingCount;
    }

    [[nodiscard]] inline auto getVersion() const noexcept {
        return this->getHeaderV0().getVersion();
    }

    auto
    retrieveCpuAndSubKind(Mach::CpuKind &CpuKind,
                          int32_t &CpuSubKind) const noexcept
        -> decltype(*this);

    [[nodiscard]] inline auto getImageInfoList() const noexcept {
        if (this->getHeader().isV8()) {
            return this->getHeaderV8().getImageInfoList();
        }

        return this->getHeaderV0().getImageInfoList();
    }

    [[nodiscard]] inline auto getImageInfoList() noexcept {
        if (this->getHeader().isV8()) {
            return this->getHeaderV8().getImageInfoList();
        }

        return this->getHeaderV0().getImageInfoList();
    }

    [[nodiscard]] inline auto getConstImageInfoList() const noexcept {
        return this->getImageInfoList();
    }

    [[nodiscard]] inline auto getMappingInfoList() const noexcept {
        return this->getHeaderV0().getMappingInfoList();
    }

    [[nodiscard]] inline auto getMappingInfoList() noexcept {
        return this->getHeaderV0().getMappingInfoList();
    }

    [[nodiscard]]
    inline auto getConstMappingInfoList() const noexcept
        -> DyldSharedCache::ConstMappingInfoList
    {
        return this->getMappingInfoList();
    }

    [[nodiscard]] inline auto
    GetFileOffsetForAddress(const uint64_t Addr,
                            uint64_t *const MaxSizeOut = nullptr) const noexcept
    {
        return this->getHeaderV0().GetFileOffsetForAddress(Addr, MaxSizeOut);
    }

    template <typename T = uint8_t>
    [[nodiscard]]
    inline auto GetPtrForAddress(const uint64_t Address) const noexcept {
        return this->getHeaderV0().GetPtrForAddress<T>(Address);
    }

    template <typename T = uint8_t>
    [[nodiscard]]
    inline auto GetPtrForAddress(const uint64_t Address) noexcept {
        return this->getHeaderV0().GetPtrForAddress<T>(Address);
    }

    [[nodiscard]]
    inline auto &getImageInfoAtIndex(const uint32_t Index) const noexcept {
        return this->getImageInfoList().at(Index);
    }

    [[nodiscard]]
    inline auto &getImageInfoAtIndex(const uint32_t Index) noexcept {
        return this->getImageInfoList().at(Index);
    }

    [[nodiscard]]
    inline auto &getConstImageInfoAtIndex(const uint32_t Index) const noexcept {
        return getImageInfoAtIndex(Index);
    }

    [[nodiscard]] auto
    GetImageInfoWithPath(std::string_view Path) const noexcept
        -> const DyldSharedCache::ImageInfo *;

    [[nodiscard]]
    auto GetImageWithInfo(const DyldSharedCache::ImageInfo &Info) const noexcept
        -> ExpectedPointer<const DscImageMemoryObject, DscImageOpenError>;

    [[nodiscard]]
    auto GetImageWithInfo(const DyldSharedCache::ImageInfo &Info) noexcept
        -> ExpectedPointer<DscImageMemoryObject, DscImageOpenError>;
};
