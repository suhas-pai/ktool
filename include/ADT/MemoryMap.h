//
//  include/ADT/MemoryMap.h
//  ktool
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

    [[nodiscard]] inline uint8_t *getBegin() const noexcept { return Begin; }
    [[nodiscard]] inline uint8_t *getEnd() const noexcept { return End; }
    [[nodiscard]] inline uint64_t size() const noexcept {
        return (End - Begin);
    }

    inline operator ConstMemoryMap &() noexcept {
        return reinterpret_cast<ConstMemoryMap &>(*this);
    }

    inline operator const ConstMemoryMap &() const noexcept {
        return reinterpret_cast<const ConstMemoryMap &>(*this);
    }

    template <typename T>
    [[nodiscard]] inline T *getBeginAs() const noexcept {
        return reinterpret_cast<T *>(getBegin());
    }

    template <typename T>
    [[nodiscard]] inline T *getEndAs() const noexcept {
        return reinterpret_cast<T *>(getEnd());
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept {
        return RelativeRange(size());
    }
};

struct ConstMemoryMap : public MemoryMap {
public:
    explicit ConstMemoryMap(const uint8_t *Begin, const uint8_t *End) noexcept;
    explicit ConstMemoryMap(const MemoryMap &Map) noexcept;

    [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const uint8_t *getEnd() const noexcept { return End; }

    template <typename T>
    [[nodiscard]] inline const T *getBeginAs() const noexcept {
        return reinterpret_cast<const T *>(getBegin());
    }

    template <typename T>
    [[nodiscard]] inline const T *getEndAs() const noexcept {
        return reinterpret_cast<const T *>(getEnd());
    }
};
