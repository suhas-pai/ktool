//
//  include/Utils/DoesOverflow.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <concepts>

#include "IntegerLimit.h"
#include "MiscTemplates.h"

template <typename W = IntegerLimitDefaultType,
          std::integral T,
          std::integral U,
          std::integral V,
          typename = std::enable_if_t<!std::is_pointer_v<T>>>

[[nodiscard]] inline bool DoesAddOverflow(T Lhs, U Rhs, V *ResultOut) noexcept {
    using RealW = IntegerLimitRealValueType<V>;

    static_assert(!std::is_const_v<V>, "ResultOut-Type cannot be const");
    static_assert(sizeof(RealW) <= sizeof(V),
                  "Integer-Limit Type is larger than ResultOut-Type");

    auto Result = SmallestIntegerType<RealW, V>();
    if (__builtin_add_overflow(Lhs, Rhs, &Result)) {
        return true;
    }

    *ResultOut = static_cast<V>(Result);
    return false;
}

template <std::integral T, std::integral U>
[[nodiscard]] inline bool DoesAddOverflow(T Lhs, U Rhs) noexcept {
    auto Result = LargestIntegerType<T, U>();
    return __builtin_add_overflow(Lhs, Rhs, &Result);
}


template <std::integral V, std::integral T, std::integral U>
[[nodiscard]] inline bool DoesAddOverflow(T Lhs, U Rhs) noexcept {
    auto Result = V();
    return __builtin_add_overflow(Lhs, Rhs, &Result);
}

template <typename W = IntegerLimitDefaultType,
          std::integral T,
          std::integral U,
          std::integral V>

[[nodiscard]]
inline bool DoesMultiplyOverflow(T Lhs, U Rhs, V *ResultOut) noexcept {
    using RealW = IntegerLimitRealValueType<V>;
    static_assert(sizeof(RealW) <= sizeof(V),
                  "Integer-Limit Type is larger than ResultOut-Type");

    auto Result = LargestIntegerType<T, U, V, RealW>();
    if (__builtin_mul_overflow(Lhs, Rhs, &Result)) {
        return true;
    }

    *ResultOut = static_cast<V>(Result);
    return false;
}

template <std::integral T, std::integral U>
[[nodiscard]] inline bool DoesMultiplyOverflow(T Lhs, U Rhs) noexcept {
    auto Result = LargestIntegerType<T, U>();
    return __builtin_mul_overflow(Lhs, Rhs, &Result);
}

template <std::integral V, std::integral T, std::integral U>
[[nodiscard]] inline bool DoesMultiplyOverflow(T Lhs, U Rhs) noexcept {
    auto Result = V();
    return __builtin_mul_overflow(Lhs, Rhs, &Result);
}

template <typename X = IntegerLimitDefaultType,
          std::integral T,
          std::integral U,
          std::integral V,
          std::integral W>

[[nodiscard]] inline
bool DoesMultiplyAndAddOverflow(T Lhs, U Rhs, V Add, W *ResultOut) noexcept {
    using RealX = IntegerLimitRealValueType<W>;
    static_assert(sizeof(RealX) <= sizeof(W),
                  "Integer-Limit Type is larger than ResultOut-Type");

    auto Result = RealX();
    if (DoesMultiplyOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    if (DoesAddOverflow(Result, Add, &Result)) {
        return true;
    }

    *ResultOut = static_cast<W>(Result);
    return false;
}

template <std::integral T, std::integral U, std::integral V>
[[nodiscard]]
inline bool DoesMultiplyAndAddOverflow(T Lhs, U Rhs, V Add) noexcept {
    auto Result = LargestIntegerType<T, U, V>();
    if (DoesMultiplyOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    return DoesAddOverflow(Result, Add, &Result);
}

template <std::integral W, std::integral T, std::integral U, std::integral V>
[[nodiscard]]
inline bool DoesMultiplyAndAddOverflow(T Lhs, U Rhs, V Add) noexcept {
    auto Result = W();
    if (DoesMultiplyOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    return DoesAddOverflow(Result, Add, &Result);
}

template <typename X = IntegerLimitDefaultType,
          std::integral T,
          std::integral U,
          std::integral V,
          typename W>

[[nodiscard]] inline bool
DoesAddAndMultiplyOverflow(T Lhs, U Rhs, V Multiply, W *ResultOut) noexcept {
    using RealX = IntegerLimitRealValueType<W>;
    static_assert(sizeof(RealX) <= sizeof(W),
                  "Integer-Limit Type is larger than ResultOut-Type");

    auto Result = RealX();
    if (DoesAddOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    if (DoesMultiplyOverflow(Result, Multiply, &Result)) {
        return true;
    }

    *ResultOut = static_cast<W>(Result);
    return false;
}

template <std::integral T, std::integral U, std::integral V>
[[nodiscard]]
inline bool DoesAddAndMultiplyOverflow(T Lhs, U Rhs, V Multiply) noexcept {
    auto Result = LargestIntegerType<T, U, V>();
    if (DoesAddOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    return DoesMultiplyOverflow(Result, Multiply, &Result);
}

template <std::integral T, std::integral U>
[[nodiscard]] inline bool DoesAddOverflow(T *Lhs, U Rhs) noexcept {
    auto Add = uint64_t();
    auto Result = T();

    if (DoesMultiplyOverflow(sizeof(T), Rhs, &Add)) {
        return true;
    }

    const auto Begin = reinterpret_cast<uintptr_t>(Lhs);
    return __builtin_add_overflow(Begin, Add, &Result);
}

template <typename T, std::integral U>
[[nodiscard]]
inline bool DoesAddOverflow(T *Lhs, U Rhs, T **ResultOut) noexcept {
    auto Add = uint64_t();
    auto Result = uintptr_t();

    if (DoesMultiplyOverflow(sizeof(T), Rhs, &Add)) {
        return true;
    }

    const auto Begin = reinterpret_cast<uintptr_t>(Lhs);
    if (DoesAddOverflow(Begin, Add, &Result)) {
        return true;
    }

    *ResultOut = reinterpret_cast<T *>(Result);
    return false;
}
