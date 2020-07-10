//
//  include/Utils/MiscTemplates.h
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
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

template <typename ... Ts>
using SmallestIntegerType = typename SmallestIntegerTypeCalculator<Ts...>::Type;

template <typename T>
struct SmallestIntegerTypeCalculator<T> {
    using Type = T;
};

template <typename T, typename U, typename ... Ts>
struct SmallestIntegerTypeCalculator<T, U, Ts...>  {
private:
    using HelperType =
        typename std::conditional<sizeof(T) < sizeof(U), T, U>::type;
public:
    using Type = SmallestIntegerType<HelperType, Ts...>;
};

template <typename ... Ts>
struct LargestIntegerTypeCalculator {};

template <typename ... Ts>
using LargestIntegerType = typename LargestIntegerTypeCalculator<Ts...>::Type;

template <typename T>
struct LargestIntegerTypeCalculator<T> {
    using Type = T;
};

template <typename T, typename U, typename ... Ts>
struct LargestIntegerTypeCalculator<T, U, Ts...>  {
private:
    using HelperType =
        typename std::conditional<(sizeof(T) > sizeof(U)), T, U>::type;
public:
    using Type = LargestIntegerType<HelperType, Ts...>;
};
