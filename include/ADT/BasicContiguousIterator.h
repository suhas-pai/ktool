//
//  include/ADT/BasicContinguousIterator.h
//  ktool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstddef>
#include <cstdint>

#include "BasicWrapperIterator.h"

template <typename T>
struct BasicPointerAbstraction :
    public BasicWrapperIterator<T *, std::ptrdiff_t> {
private:
    using Base = BasicWrapperIterator<T *, std::ptrdiff_t>;
public:
    using BasicWrapperIterator<T *, std::ptrdiff_t>::BasicWrapperIterator;

    constexpr BasicPointerAbstraction(uint8_t *Ptr) noexcept
    : Base(reinterpret_cast<T *>(Ptr)) {}

    inline T *operator->() noexcept { return this->Item; }
    inline const T *operator->() const noexcept { return this->Item; }
};

template <>
struct BasicPointerAbstraction<uint8_t> :
    public BasicWrapperIterator<uint8_t *, std::ptrdiff_t> {
private:
    using Base = BasicWrapperIterator<uint8_t *, std::ptrdiff_t>;
public:
    using BasicWrapperIterator<uint8_t *, std::ptrdiff_t>::BasicWrapperIterator;

    inline uint8_t *operator->() noexcept { return this->Item; }
    inline const uint8_t *operator->() const noexcept { return this->Item; }
};

template <typename T>
struct BasicContiguousIterator : public BasicPointerAbstraction<T> {
public:
    using BasicPointerAbstraction<T>::BasicPointerAbstraction;

    T &operator*() noexcept { return *this->Item; }
    const T &operator*() const noexcept { return *this->Item; }

    [[nodiscard]] inline T *getPtr() noexcept { return this->Item; }
    [[nodiscard]] inline const T *getPtr() const noexcept { return this->Item; }
};
