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

    explicit BasicPointerAbstraction(uint8_t *Ptr) noexcept
    : Base(reinterpret_cast<T *>(Ptr)) {}

    T *operator->() noexcept { return this->Item; }
    const T *operator->() const noexcept { return this->Item; }
};

template <typename T>
struct BasicContiguousIterator : public BasicPointerAbstraction<T> {
public:
    using BasicPointerAbstraction<T>::BasicPointerAbstraction;

    T &operator*() noexcept { return *this->Item; }
    const T &operator*() const noexcept { return *this->Item; }

    [[nodiscard]] T *getPtr() noexcept { return this->Item; }
    [[nodiscard]] const T *getPtr() const noexcept { return this->Item; }
};
