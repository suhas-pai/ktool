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

    constexpr auto AddAndCheckOverflow(const U Lhs, const V Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_add_overflow(Lhs, Rhs, &Result)) {
            return std::optional<T>(Result);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    constexpr auto
    AddAndCheckOverflow(const U Lhs, const V Rhs, Rest... rest) noexcept
        -> std::optional<T>
    {
        const auto FirstOpt = AddAndCheckOverflow(Lhs, Rhs);
        if (FirstOpt.has_value()) {
            return AddAndCheckOverflow(FirstOpt.value(), rest...);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t, std::integral U, std::integral V>
    constexpr auto SubAndCheckOverflow(const U Lhs, const V Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_sub_overflow(Lhs, Rhs, &Result)) {
            return std::optional<T>(Result);
        }

        return std::nullopt;
    }

    template <std::integral T,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    constexpr auto
    SubAndCheckOverflow(const T Lhs, const T Rhs, Rest... rest) noexcept
        -> std::optional<T>
    {
        const auto FirstOpt = SubAndCheckOverflow(Lhs, Rhs);
        if (FirstOpt.has_value()) {
            return SubAndCheckOverflow(FirstOpt.value(), rest...);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t, std::integral U, std::integral V>
    constexpr auto MulAndCheckOverflow(const U Lhs, const V Rhs) noexcept
        -> std::optional<T>
    {
        auto Result = T();
        if (!__builtin_mul_overflow(Lhs, Rhs, &Result)) {
            return std::optional<T>(Result);
        }

        return std::nullopt;
    }

    template <std::integral T,
              std::integral U,
              std::integral V,
              std::integral... Rest>

    constexpr auto
    MulAndCheckOverflow(const U Lhs, const V Rhs, Rest... rest) noexcept
        -> std::optional<T>
    {
        const auto FirstOpt = MulAndCheckOverflow(Lhs, Rhs);
        if (!FirstOpt.has_value()) {
            return std::nullopt;
        }

        return MulAndCheckOverflow(FirstOpt.value(), rest...);
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral W>

    constexpr auto
    MulAddAndCheckOverflow(const U F, const V S, const W Th) noexcept
        -> std::optional<T>
    {
        const auto FirstOpt = MulAndCheckOverflow(F, S);
        if (FirstOpt.has_value()) {
            return AddAndCheckOverflow(FirstOpt.value(), Th);
        }

        return std::nullopt;
    }

    template <std::integral T = uint64_t,
              std::integral U,
              std::integral V,
              std::integral W>

    constexpr auto
    AddMulAndCheckOverflow(const U F, const V S, const W Th) noexcept
        -> std::optional<T>
    {
        const auto FirstOpt = AddAndCheckOverflow(F, S);
        if (FirstOpt.has_value()) {
            return MulAndCheckOverflow(FirstOpt.value(), Th);
        }

        return std::nullopt;
    }

    template <typename T, std::integral U>
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
    inline auto
    AddPtrAndCheckOverflow(T *const Ptr, const U Rhs, Rest... rest) noexcept
        -> std::optional<T *>
    {
        const auto FirstOpt = AddPtrAndCheckOverflow(Ptr, Rhs);
        if (FirstOpt.has_value()) {
            return AddPtrAndCheckOverflow(FirstOpt.value(), Rhs, rest...);
        }

        return std::nullopt;
    }

}