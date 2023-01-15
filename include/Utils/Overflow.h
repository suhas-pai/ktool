/*
 * Utils/Overflow.h
 * © suhas pai
 */

#pragma once

#include <concepts>
#include <optional>

namespace Utils {
    template <std::integral T>
    constexpr auto AddAndCheckOverflow(const T Lhs, const T Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_add_overflow(Lhs, Rhs, &Result)) {
            return std::optional<T>(Result);
        }

        return std::nullopt;
    }

    template <std::integral T>
    constexpr auto SubAndCheckOverflow(const T Lhs, const T Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_sub_overflow(Lhs, Rhs, &Result)) {
            return std::optional<T>(Result);
        }

        return std::nullopt;
    }

    template <std::integral T>
    constexpr auto MulAndCheckOverflow(const T Lhs, const T Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_mul_overflow(Lhs, Rhs, &Result)) {
            return std::optional<T>(Result);
        }

        return std::nullopt;
    }
}