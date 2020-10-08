//
//  include/Utils/MiscTemplates.h
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

template <typename T, typename U>
constexpr
inline bool IndexOutOfBounds(const T &Index, const U &Bounds) noexcept {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>,
                  "Types must be integer-types");

    return (Index >= Bounds);
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
[[nodiscard]] constexpr inline size_t countof(T (&arr)[N]) noexcept {
    return std::extent<T[N]>::value;
}
