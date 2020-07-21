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

struct ConstMemoryMap {
protected:
    const uint8_t *Begin;
    const uint8_t *End;
public:
    explicit ConstMemoryMap(const uint8_t *Begin, const uint8_t *End) noexcept;
    [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const uint8_t *getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return (End - Begin);
    }

    template <typename T>
    [[nodiscard]] inline const T *getBeginAs() const noexcept {
        return reinterpret_cast<const T *>(getBegin());
    }

    template <typename T>
    [[nodiscard]] inline const T *getEndAs() const noexcept {
        return reinterpret_cast<const T *>(getEnd());
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept {
        return RelativeRange(size());
    }
};

struct MemoryMap : public ConstMemoryMap {
public:
    explicit MemoryMap(uint8_t *Begin, uint8_t *End) noexcept;

    [[nodiscard]] inline uint8_t *getBegin() const noexcept {
        return const_cast<uint8_t *>(Begin);
    }

    [[nodiscard]] inline uint8_t *getEnd() const noexcept {
        return const_cast<uint8_t *>(End);
    }

    template <typename T>
    [[nodiscard]] inline T *getBeginAs() const noexcept {
        return reinterpret_cast<T *>(getBegin());
    }

    template <typename T>
    [[nodiscard]] inline T *getEndAs() const noexcept {
        return reinterpret_cast<T *>(getEnd());
    }
};
