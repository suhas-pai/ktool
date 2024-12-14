//
//  ADT/BasicContiguousIterator.h
//  ktool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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

    constexpr BasicPointerAbstraction(uint8_t *const Ptr) noexcept
    : Base(reinterpret_cast<T *>(Ptr)) {}

    [[nodiscard]] inline auto operator->() noexcept { return this->Item; }
    [[nodiscard]] inline auto operator->() const noexcept {
        return this->Item;
    }
};

template <>
struct BasicPointerAbstraction<uint8_t> :
    public BasicWrapperIterator<uint8_t *, std::ptrdiff_t> {
private:
    using Base = BasicWrapperIterator<uint8_t *, std::ptrdiff_t>;
public:
    using BasicWrapperIterator<uint8_t *, std::ptrdiff_t>::BasicWrapperIterator;

    [[nodiscard]] inline auto operator->() noexcept {
        return this->Item;
    }

    [[nodiscard]] inline auto operator->() const noexcept {
        return this->Item;
    }
};

template <typename T>
struct BasicContiguousIterator : public BasicPointerAbstraction<T> {
public:
    using BasicPointerAbstraction<T>::BasicPointerAbstraction;

    auto &operator*() noexcept { return *this->Item; }
    auto &operator*() const noexcept { return *this->Item; }

    [[nodiscard]] constexpr auto getPtr() noexcept { return this->Item; }
    [[nodiscard]] constexpr auto getPtr() const noexcept { return this->Item; }
};
