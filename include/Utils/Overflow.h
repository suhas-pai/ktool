/*
 * Utils/Overflow.h
 * © suhas pai
 */

#pragma once

#include <concepts>
#include <cstdint>
#include <optional>

namespace Utils {
    template <std::integral T = uint64_t, std::integral U, std::integral V>
    [[nodiscard]]
    constexpr auto WillAddOverflow(const U Lhs, const V Rhs) noexcept {
        auto Result = T();
        return __builtin_add_overflow(Lhs, Rhs, &Result);
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    [[nodiscard]] constexpr
    auto WillAddOverflow(const U Lhs, const V Rhs, Rest... rest) noexcept {
        auto Result = T();
        if (__builtin_add_overflow(Lhs, Rhs, &Result)) {
            return true;
        }

        return WillAddOverflow(Result, rest...);
    }

    template <std::integral T = uint64_t, std::integral U, std::integral V>
    [[nodiscard]]
    constexpr auto AddAndCheckOverflow(const U Lhs, const V Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_add_overflow(Lhs, Rhs, &Result)) {
            return Result;
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    [[nodiscard]]
    constexpr auto
    AddAndCheckOverflow(const U Lhs, const V Rhs, Rest... rest) noexcept
        -> std::optional<T>
    {
        if (const auto FirstOpt = AddAndCheckOverflow(Lhs, Rhs)) {
            return AddAndCheckOverflow(FirstOpt.value(), rest...);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t, std::integral U, std::integral V>
    [[nodiscard]]
    constexpr auto SubAndCheckOverflow(const U Lhs, const V Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_sub_overflow(Lhs, Rhs, &Result)) {
            return Result;
        }

        return std::nullopt;
    }

    template <std::integral T,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    [[nodiscard]] constexpr auto
    SubAndCheckOverflow(const T Lhs, const T Rhs, Rest... rest) noexcept
        -> std::optional<T>
    {
        if (const auto FirstOpt = SubAndCheckOverflow(Lhs, Rhs)) {
            return SubAndCheckOverflow(FirstOpt.value(), rest...);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t, std::integral U, std::integral V>
    [[nodiscard]]
    constexpr auto MulAndCheckOverflow(const U Lhs, const V Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_mul_overflow(Lhs, Rhs, &Result)) {
            return Result;
        }

        return std::nullopt;
    }

    template <std::integral T,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    [[nodiscard]] constexpr auto
    MulAndCheckOverflow(const U Lhs, const V Rhs, Rest... rest) noexcept
        -> std::optional<T>
    {
        if (const auto FirstOpt = MulAndCheckOverflow(Lhs, Rhs)) {
            return MulAndCheckOverflow(FirstOpt.value(), rest...);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral W>

    [[nodiscard]] constexpr auto
    MulAddAndCheckOverflow(const U F, const V S, const W Th) noexcept
        -> std::optional<T>
    {
        if (const auto FirstOpt = MulAndCheckOverflow(F, S)) {
            return AddAndCheckOverflow(FirstOpt.value(), Th);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral W>

    [[nodiscard]] constexpr auto
    AddMulAndCheckOverflow(const U F, const V S, const W Th) noexcept
        -> std::optional<T>
    {
        if (const auto FirstOpt = AddAndCheckOverflow(F, S)) {
            return MulAndCheckOverflow(FirstOpt.value(), Th);
        }

        return std::nullopt;
    }

    template <typename T, std::integral U>
    [[nodiscard]]
    inline auto AddPtrAndCheckOverflow(T *const Ptr, const U Value) noexcept
        -> std::optional<T *>
    {
        const auto Result =
            MulAddAndCheckOverflow(sizeof(T),
                                   Value,
                                   reinterpret_cast<uint64_t>(Ptr));

        if (Result.has_value()) {
            return std::optional(reinterpret_cast<T *>(Result.value()));
        }

        return std::nullopt;
    }

    template <typename T, std::integral U, std::integral... Rest>
    [[nodiscard]] inline auto
    AddPtrAndCheckOverflow(T *const Ptr, const U Rhs, Rest... rest) noexcept
        -> std::optional<T *>
    {
        if (const auto FirstOpt = AddPtrAndCheckOverflow(Ptr, Rhs)) {
            return AddPtrAndCheckOverflow(FirstOpt.value(), rest...);
        }

        return std::nullopt;
    }
}