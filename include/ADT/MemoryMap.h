//
//  include/ADT/MemoryMap.h
//  stool
//
//  Created by Suhas Pai on 4/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

#include "RelativeRange.h"

struct ConstMemoryMap;
struct MemoryMap {
protected:
    uint8_t *Begin;
    uint8_t *End;
public:
    explicit MemoryMap(uint8_t *Begin, uint8_t *End) noexcept;

    inline uint8_t *GetBegin() const noexcept { return Begin; }
    inline uint8_t *GetEnd() const noexcept { return End; }
    inline uint64_t GetSize() const noexcept { return (End - Begin); }

    inline operator ConstMemoryMap &() noexcept {
        return reinterpret_cast<ConstMemoryMap &>(*this);
    }

    inline operator const ConstMemoryMap &() const noexcept {
        return reinterpret_cast<const ConstMemoryMap &>(*this);
    }

    inline RelativeRange GetRange() const noexcept {
        return RelativeRange(GetSize());
    }
};

struct ConstMemoryMap {
protected:
    const uint8_t *Begin;
    const uint8_t *End;
public:
    explicit ConstMemoryMap(const uint8_t *Begin, const uint8_t *End) noexcept;
    explicit ConstMemoryMap(const MemoryMap &Map) noexcept;

    inline const uint8_t *GetBegin() const noexcept { return Begin; }
    inline const uint8_t *GetEnd() const noexcept { return End; }
    inline uint64_t GetSize() const noexcept { return (End - Begin); }

    inline RelativeRange GetRange() const noexcept {
        return RelativeRange(GetSize());
    }
};

static_assert(sizeof(MemoryMap) == sizeof(ConstMemoryMap),
              "MemoryMap and ConstMemoryMap are not the same-size");

template <typename T>
static inline ConstMemoryMap &cast(MemoryMap &Map) {
    return reinterpret_cast<ConstMemoryMap &>(Map);
}

template <typename T>
static inline const ConstMemoryMap &cast(const MemoryMap &Map) {
    return reinterpret_cast<const ConstMemoryMap &>(Map);
}

template <typename T>
static inline ConstMemoryMap *cast(MemoryMap *Map) {
    return reinterpret_cast<ConstMemoryMap *>(Map);
}

template <typename T>
static inline const ConstMemoryMap *cast(const MemoryMap *Map) {
    return reinterpret_cast<const ConstMemoryMap *>(Map);
}
