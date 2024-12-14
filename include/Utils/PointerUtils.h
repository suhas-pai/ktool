//
//  Utils/PointerSize.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <concepts>
#include "ADT/PointerKind.h"

template <PointerKind Kind>
[[nodiscard]] constexpr uint8_t PointerSize() noexcept {
    return sizeof(PointerAddrTypeFromKind<Kind>);
}

[[nodiscard]] constexpr uint8_t PointerSize(bool Is64Bit) noexcept {
    const auto Result =
        (Is64Bit) ?
            PointerSize<PointerKind::s64Bit>() :
            PointerSize<PointerKind::s32Bit>();

    return Result;
}

[[nodiscard]] constexpr uint8_t PointerSize(PointerKind Kind) noexcept {
    switch (Kind) {
        case PointerKind::s32Bit:
            return PointerSize<PointerKind::s32Bit>();
        case PointerKind::s64Bit:
            return PointerSize<PointerKind::s64Bit>();
    }
}

template <PointerKind Kind, std::integral T>
[[nodiscard]] constexpr bool IntegerIsPointerAligned(T Integer) noexcept {
    return (Integer % PointerSize<Kind>()) == 0;
}

template <std::integral T>
[[nodiscard]] constexpr
inline bool IntegerIsPointerAligned(T Integer, bool Is64Bit) noexcept {
    return (Integer % PointerSize(Is64Bit)) == 0;
}
