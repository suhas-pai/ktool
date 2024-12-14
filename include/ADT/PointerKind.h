//
//  ADT/PointerKind.h
//  ktool
//
//  Created by Suhas Pai on 6/3/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

enum class PointerKind {
    s32Bit,
    s64Bit
};

[[nodiscard]]
constexpr PointerKind PointerKindFromIs64Bit(const bool Is64Bit) noexcept {
    return Is64Bit ? PointerKind::s64Bit : PointerKind::s32Bit;
}

[[nodiscard]]
constexpr auto PointerKindIs32Bit(const PointerKind Kind) noexcept {
    return Kind == PointerKind::s32Bit;
}

[[nodiscard]]
constexpr auto PointerKindIs64Bit(const PointerKind Kind) noexcept {
    return Kind == PointerKind::s64Bit;
}

template <PointerKind Kind>
using PointerAddrTypeFromKind =
    std::conditional_t<PointerKindIs64Bit(Kind), uint64_t, uint32_t>;

template <PointerKind Kind>
using PointerAddrConstTypeFromKind =
    std::add_const_t<PointerAddrTypeFromKind<Kind>>;
