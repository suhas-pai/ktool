//
//  include/ADT/Allocation.h
//  ktool
//
//  Created by Suhas Pai on 3/9/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

struct Allocation {
protected:
    void *Ptr = nullptr;
public:
    constexpr Allocation() noexcept = default;

    template <typename T>
    explicit Allocation(T *Ptr) noexcept : Ptr(reinterpret_cast<void *>(Ptr)) {}

    inline ~Allocation() noexcept {
        delete reinterpret_cast<uint8_t *>(Ptr);
        Ptr = nullptr;
    }

    template <typename T>
    [[nodiscard]] inline T *getAs() const noexcept {
        return reinterpret_cast<T *>(Ptr);
    }

    template <typename T>
    inline Allocation &set(T *Ptr) noexcept {
        delete reinterpret_cast<uint8_t *>(this->Ptr);
        this->Ptr = Ptr;

        return *this;
    }

    template <typename T>
    inline Allocation &replace(T *Ptr) noexcept {
        this->Ptr = Ptr;
        return *this;
    }

    template <typename T>
    inline void operator=(T *Ptr) noexcept { Set(Ptr); }

    template <typename T>
    inline operator T *() noexcept { return static_cast<T *>(Ptr); }

    template <typename T>
    inline operator const T *() const noexcept {
        return static_cast<const T *>(Ptr);
    }

    template <typename T>
    [[nodiscard]] inline bool operator==(const T &Rhs) const noexcept {
        return Ptr == Rhs;
    }

    template <typename T>
    [[nodiscard]] inline bool operator!=(const T &Rhs) const noexcept {
        return Ptr != Rhs;
    }
};
