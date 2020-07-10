//
//  include/ADT/PointerKind.h
//  ktool
//
//  Created by Suhas Pai on 6/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <cstdint>

enum class PointerKind {
    s32Bit,
    s64Bit
};

[[nodiscard]]
constexpr PointerKind PointerKindFromIs64Bit(bool Is64Bit) noexcept {
    return (Is64Bit) ? PointerKind::s64Bit : PointerKind::s32Bit;
}

[[nodiscard]]
constexpr static inline bool PointerKindIs32Bit(PointerKind Kind) noexcept {
    return (Kind == PointerKind::s32Bit);
}

[[nodiscard]]
constexpr static inline bool PointerKindIs64Bit(PointerKind Kind) noexcept {
    return (Kind == PointerKind::s64Bit);
}

template <PointerKind Kind>
using PointerAddrTypeFromKind =
    std::conditional_t<PointerKindIs64Bit(Kind), uint64_t, uint32_t>;

template <PointerKind Kind>
using PointerAddrConstTypeFromKind =
    std::conditional_t<PointerKindIs64Bit(Kind),
                       const uint64_t,
                       const uint32_t>;
