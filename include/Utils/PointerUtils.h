//
//  include/Utils/PointerSize.h
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

template <bool Is64Bit>
using PointerAddrType = std::conditional_t<Is64Bit, uint64_t, uint32_t>;

template <bool Is64Bit>
using PointerAddrConstType = std::conditional_t<Is64Bit,
                                                const uint64_t,
                                                const uint32_t>;


template <bool Is64Bit>
static inline uint8_t PointerSize() noexcept {
    return (Is64Bit) ? sizeof(uint64_t) : sizeof(uint32_t);
}

static inline uint8_t PointerSize(bool Is64Bit) noexcept {
    return (Is64Bit) ? PointerSize<true>() : PointerSize<false>();
}

template <typename T, bool Is64Bit>
static inline
bool IntegerIsPointerAligned(const T &Integer, uint64_t Pointer) noexcept {
    static_assert(std::is_integral_v<T>, "Type must be integer-type");
    return ((Integer % PointerSize<Is64Bit>()) == 0);
}

template <typename T>
static inline bool IntegerIsPointerAligned(T Integer, bool Is64Bit) noexcept {
    static_assert(std::is_integral_v<T>, "Type must be integer-type");
    return ((Integer % PointerSize(Is64Bit)) == 0);
}
