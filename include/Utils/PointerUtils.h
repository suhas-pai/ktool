//
//  include/Utils/PointerSize.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

#include "ADT/PointerKind.h"

template <PointerKind Kind>
[[nodiscard]] constexpr inline uint8_t PointerSize() noexcept {
    return sizeof(PointerAddrTypeFromKind<Kind>);
}

[[nodiscard]] constexpr inline uint8_t PointerSize(bool Is64Bit) noexcept {
    const auto Result =
        (Is64Bit) ?
            PointerSize<PointerKind::s64Bit>() :
            PointerSize<PointerKind::s32Bit>();

    return Result;
}

[[nodiscard]] constexpr inline uint8_t PointerSize(PointerKind Kind) noexcept {
    switch (Kind) {
        case PointerKind::s32Bit:
            return PointerSize<PointerKind::s32Bit>();
        case PointerKind::s64Bit:
            return PointerSize<PointerKind::s64Bit>();
    }
}

template <PointerKind Kind, typename T>
[[nodiscard]]
constexpr inline bool IntegerIsPointerAligned(const T &Integer) noexcept {
    static_assert(std::is_integral_v<T>, "Type must be integer-type");
    return ((Integer % PointerSize<Kind>()) == 0);
}

template <typename T>
[[nodiscard]] constexpr
inline bool IntegerIsPointerAligned(const T &Integer, bool Is64Bit) noexcept {
    static_assert(std::is_integral_v<T>, "Type must be integer-type");
    return ((Integer % PointerSize(Is64Bit)) == 0);
}
