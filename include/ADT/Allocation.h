//
//  include/ADT/Allocation.h
//  stool
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
    explicit Allocation() noexcept = default;

    template <typename T>
    explicit Allocation(T *Ptr) noexcept : Ptr(reinterpret_cast<void *>(Ptr)) {}

    ~Allocation() noexcept {
        delete reinterpret_cast<uint8_t *>(Ptr);
        Ptr = nullptr;
    }

    template <typename T>
    inline T GetAs() const noexcept { return reinterpret_cast<T>(Ptr); }

    template <typename T>
    inline void Set(T *Ptr) noexcept {
        delete reinterpret_cast<uint8_t *>(this->Ptr);
        this->Ptr = Ptr;
    }

    template <typename T>
    inline void Replace(T *Ptr) noexcept {
        this->Ptr = Ptr;
    }

    template <typename T>
    inline void operator=(T *Ptr) noexcept { Set(Ptr); }

    template <typename T>
    operator T *() const noexcept { return static_cast<T *>(Ptr); }

    template <typename T>
    inline bool operator==(const T &Rhs) const noexcept { return Ptr == Rhs; }

    template <typename T>
    inline bool operator!=(const T &Rhs) const noexcept { return Ptr != Rhs; }
};
