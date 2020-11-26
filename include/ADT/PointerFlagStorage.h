//
//  include/ADT/PointerFlagStorage.h
//  ktool
//
//  Created by Suhas Pai on 5/25/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <cstdint>

template <typename T>
struct PointerFlagStorage {
protected:
    T *Ptr = nullptr;
public:
    constexpr PointerFlagStorage() = default;
    constexpr PointerFlagStorage(T *Ptr) : Ptr(Ptr) {}
    constexpr PointerFlagStorage(bool Flag)
    : Ptr(reinterpret_cast<T *>(Flag)) {}

    [[nodiscard]] constexpr inline bool hasFlag() const noexcept {
        return (reinterpret_cast<uint64_t>(Ptr) & 1);
    }

    [[nodiscard]] constexpr inline T *getPtr() const noexcept {
        if (hasFlag()) {
            return (reinterpret_cast<uint64_t>(Ptr) & ~1);
        }

        return Ptr;
    }

    constexpr inline PointerFlagStorage &setFlag(bool Flag) const noexcept {
        reinterpret_cast<uint64_t &>(Ptr) |= static_cast<uint64_t>(Flag);
        return *this;
    }

    [[nodiscard]] constexpr inline operator T *() const noexcept {
        return getPtr();
    }
};
