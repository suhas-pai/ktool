//
//  Utils/MiscTemplates.h
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <type_traits>

template <std::integral T, std::integral U>
[[nodiscard]]
constexpr auto IndexOutOfBounds(T Index, U Bounds) noexcept {
    return static_cast<U>(Index) >= Bounds;
}

template <std::integral T>
[[nodiscard]] constexpr bool OrdinalToIndex(T Ordinal) noexcept {
    assert(Ordinal != 0);
    return Ordinal - 1;
}

template <typename ... Ts>
struct SmallestIntegerTypeCalculator {};

template <typename T>
struct SmallestIntegerTypeCalculator<T> {
    using type = T;
};

template <typename... Rest>
using SmallestIntegerType =
    typename SmallestIntegerTypeCalculator<Rest...>::type;

template <typename T, typename U>
struct SmallestIntegerTypeCalculator<T, U> {
    using type = typename std::conditional<sizeof(T) < sizeof(U), T, U>::type;
};

template <typename T, typename U, typename... Rest>
struct SmallestIntegerTypeCalculator<T, U, Rest...>  {
    using type = SmallestIntegerType<SmallestIntegerType<T, U>, Rest...>;
};

template <typename... Rest>
struct LargestIntegerTypeCalculator {};

template <typename... Rest>
using LargestIntegerType = typename LargestIntegerTypeCalculator<Rest...>::type;

template <typename T>
struct LargestIntegerTypeCalculator<T> {
    using type = T;
};

template <typename T, typename U>
struct LargestIntegerTypeCalculator<T, U> {
    using type = typename std::conditional<(sizeof(T) > sizeof(U)), T, U>::type;
};

template <typename T, typename U, typename... Rest>
struct LargestIntegerTypeCalculator<T, U, Rest...>  {
    using type = LargestIntegerType<LargestIntegerType<T, U>, Rest...>;
};

template <typename T, size_t N>
[[nodiscard]] constexpr size_t countof(T (&arr)[N]) noexcept {
    (void)arr;
    return std::extent<T[N]>::value;
}
