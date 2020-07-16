//
//  include/Objects/DscImageMemory.h
//  ktool
//
//  Created by Suhas Pai on 7/12/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/DyldSharedCache.h"
#include "ADT/PointerErrorStorage.h"
#include "MachOMemory.h"

struct DscMemoryObject;
struct ConstDscImageMemoryObject;

struct DscImageMemoryObject : MachOMemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::DscImage;
    friend struct DscMemoryObject;
protected:
    const MemoryMap &DscMap;
    const DyldSharedCache::ImageInfo &ImageInfo;

    DscImageMemoryObject(const MemoryMap &DscMap,
                         const DyldSharedCache::ImageInfo &ImageInfo,
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
        return DscMap.getRange();
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

    [[nodiscard]] inline const char *getPath() const noexcept {
        return ImageInfo.getPath(getDscMap().getBegin());
    }
};

struct ConstDscImageMemoryObject : DscImageMemoryObject {
public:
    friend struct DscMemoryObject;
protected:
    ConstDscImageMemoryObject(const ConstMemoryMap &DscMap,
                              const DyldSharedCache::ImageInfo &ImageInfo,
                              const uint8_t *Begin,
                              const uint8_t *End) noexcept;
public:
    [[nodiscard]] inline const ConstMemoryMap &getDscMap() const noexcept {
        return DscMap;
    }
};
