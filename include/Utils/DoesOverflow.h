//
//  include/Utils/DoesOverflow.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

#include "IntegerLimit.h"

template <typename W = IntegerLimitDefaultType,
          typename T,
          typename U,
          typename V,
          typename = std::enable_if_t<!std::is_pointer_v<T>>>

inline bool DoesAddOverflow(T Lhs, U Rhs, V *ResultOut) {
    static_assert(!std::is_const_v<V>, "ResultOut-Type cannot be const");
    
    using RealW = IntegerLimitRealValueType<V>;
    static_assert(std::is_integral_v<T> && std::is_integral_v<U> &&
                  std::is_integral_v<V> && std::is_integral_v<RealW>,
                  "All 3/4 types must be integers");

    static_assert(sizeof(RealW) <= sizeof(V),
                  "Invalid integer-limit type specified");

    auto Result = IntegerLimitUseType<V>();
    if (__builtin_add_overflow(Lhs, Rhs, &Result)) {
        return true;
    }

    *ResultOut = static_cast<V>(Result);
    return false;
}


template <typename V = uint64_t, typename T, typename U>
inline bool DoesAddOverflow(T Lhs, U Rhs) {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U> &&
                  std::is_integral_v<V>, "All 3 types must be integers");

    auto Result = V();
    return __builtin_add_overflow(Lhs, Rhs, &Result);
}

template <typename W = IntegerLimitDefaultType,
          typename T,
          typename U,
          typename V>

inline bool DoesMultiplyOverflow(T Lhs, U Rhs, V *ResultOut) {
    using RealW = IntegerLimitRealValueType<V>;
    static_assert(std::is_integral_v<T> && std::is_integral_v<U> &&
                  std::is_integral_v<V> && std::is_integral_v<RealW>,
                  "All 3/4 types must be integers");

    static_assert(sizeof(RealW) <= sizeof(V),
                  "Invalid integer-limit type specified");

    auto Result = IntegerLimitUseType<V>();
    if (__builtin_mul_overflow(Lhs, Rhs, &Result)) {
        return true;
    }

    *ResultOut = static_cast<V>(Result);
    return false;
}

template <typename V = uint64_t, typename T, typename U>
inline bool DoesMultiplyOverflow(T Lhs, U Rhs) {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U> &&
                  std::is_integral_v<V>, "All 3 types must be integers");

    auto Result = V();
    return __builtin_mul_overflow(Lhs, Rhs, &Result);
}

template <typename X = IntegerLimitDefaultType,
          typename T,
          typename U,
          typename V,
          typename W>

inline bool DoesMultiplyAndAddOverflow(T Lhs, U Rhs, V Add, W *ResultOut) {
    using RealX = IntegerLimitRealValueType<W>;
    static_assert(std::is_integral_v<T> && std::is_integral_v<U> &&
                  std::is_integral_v<V> && std::is_integral_v<W> &&
                  std::is_integral_v<RealX>,
                  "All 3/4 types must be integers");

    static_assert(sizeof(RealX) <= sizeof(W),
                  "Invalid integer-limit type specified");

    auto Result = IntegerLimitUseType<W>();
    if (DoesMultiplyOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    if (DoesAddOverflow(Result, Add, &Result)) {
        return true;
    }

    *ResultOut = static_cast<W>(Result);
    return false;
}

template <typename W = uint64_t, typename T, typename U, typename V>
inline bool DoesMultiplyAndAddOverflow(T Lhs, U Rhs, V Add) {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U> &&
                  std::is_integral_v<V> && std::is_integral_v<W>,
                  "All 4 types must be integers");

    auto Result = W();
    if (DoesMultiplyOverflow(Lhs, Rhs, &Result)) {
        return true;
    }

    return DoesAddOverflow(Result, Add, &Result);
}

template <typename T, typename U>
inline bool DoesAddOverflow(T *Lhs, U Rhs) {
    static_assert(std::is_integral_v<U>, "Add-Type must be an integer-type");

    auto Add = uint64_t();
    auto Result = T();

    if (DoesMultiplyOverflow(sizeof(T), Rhs, &Add)) {
        return true;
    }

    const auto Begin = reinterpret_cast<uintptr_t>(Lhs);
    return __builtin_add_overflow(Begin, Add, &Result);
}

template <typename T, typename U>
inline bool DoesAddOverflow(T *Lhs, U Rhs, T **ResultOut) {
    static_assert(std::is_integral_v<U>, "Add-Type must be an integer-type");

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
