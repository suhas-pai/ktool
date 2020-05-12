//
//  include/ADT/BasicContinguousIterator.h
//  stool
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
public:
    using BasicWrapperIterator<T *, std::ptrdiff_t>::BasicWrapperIterator;

    explicit BasicPointerAbstraction(uint8_t *Ptr) noexcept
    : BasicWrapperIterator<T, std::ptrdiff_t>(reinterpret_cast<T>(Ptr)) {}

    T *operator->() const noexcept { return this->Item; }
};

template <typename T>
struct BasicContiguousIterator : public BasicPointerAbstraction<T> {
public:
    using BasicPointerAbstraction<T>::BasicPointerAbstraction;

    T &operator*() noexcept { return *this->Item; }
    const T &operator*() const noexcept { return *this->Item; }
    T *GetPtr() const noexcept { return this->Item; }
};
