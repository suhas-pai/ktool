//
//  ADT/FlagsBase.h
//  ktool
//
//  Created by suhaspai on 10/5/22.
//

#pragma once

#include <assert.h>
#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "Utils/Misc.h"

namespace ADT {
    template <std::unsigned_integral T>
    struct FlagsBase {
    protected:
        T Flags = 0;

        template <typename E>
        constexpr auto addMask(const E Mask) noexcept {
            Flags |= static_cast<T>(Mask);
        }

        template <typename E>
        constexpr auto removeMask(const E Mask) noexcept {
            Flags &= ~static_cast<T>(Mask);
        }

        template <typename E, typename S, typename V>
        constexpr void
        setValueForMask(const E Mask, const S Shift, const V Value) noexcept {
            removeMask(Mask);
            addMask(static_cast<T>(Value) << static_cast<T>(Shift));
        }

        template <typename E>
        constexpr auto valueForMask(const E Mask) const noexcept {
            return Flags & static_cast<T>(Mask);
        }

        template <typename E>
        [[nodiscard]] constexpr auto has(const E Mask) const noexcept {
            return valueForMask(Mask) != 0;
        }
    public:
        constexpr FlagsBase() noexcept = default;
        constexpr FlagsBase(const T Flags) noexcept : Flags(Flags) {}

        [[nodiscard]]
        constexpr auto operator|(const FlagsBase<T> Mask) const noexcept {
            return (Flags | Mask);
        }

        [[nodiscard]]
        constexpr auto operator&(const FlagsBase<T> Mask) const noexcept {
            return (Flags & Mask);
        }

        constexpr auto operator|=(const FlagsBase<T> Mask) const noexcept
            -> decltype(*this)
        {
            Flags |= Mask;
            return *this;
        }

        constexpr auto operator&=(const FlagsBase<T> Mask) const noexcept
            -> decltype(*this)
        {
            Flags &= Mask;
            return *this;
        }

        [[nodiscard]] constexpr auto operator~() const noexcept {
            return FlagsBase(~Flags);
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return Flags == 0;
        }

        [[nodiscard]] constexpr auto value() const noexcept {
            return Flags;
        }

        [[nodiscard]]
        constexpr auto getFirstSet(const uint8_t Index = 0) const noexcept
            -> uint8_t
        {
            assert(Index < bit_sizeof(T));

            const uint64_t Flags = this->Flags >> Index;
            if (Flags == 0) {
                return bit_sizeof(T);
            }

            if constexpr (sizeof(T) == sizeof(long long)) {
                return __builtin_ctzll(Flags) + Index;
            } else if constexpr (sizeof(T) == sizeof(long long)) {
                return __builtin_ctzl(Flags) + Index;
            }

            return __builtin_ctz(Flags) + Index;
        }

        [[nodiscard]]
        constexpr auto getCount(const uint8_t Index = 0) const noexcept
            -> uint8_t
        {
            assert(Index < bit_sizeof(T));

            if constexpr (sizeof(T) == sizeof(long long)) {
                return __builtin_popcountll(Flags >> Index);
            } else if constexpr (sizeof(T) == sizeof(long)) {
                return __builtin_popcountl(Flags >> Index);
            }

            return __builtin_popcount(Flags >> Index);
        }

        [[nodiscard]]
        constexpr auto removeFirstBits(const uint8_t BitCount) const noexcept
            -> decltype(*this)
        {
            Flags >>= BitCount;
            return *this;
        }

        [[nodiscard]] constexpr
        auto operator<=>(const FlagsBase<T> &Rhs) const noexcept = default;

        [[nodiscard]] constexpr auto operator<=>(const T Rhs) const noexcept {
            return (Flags <=> Rhs);
        }
    };
}

#define MAKE_ENUM_MASK_CLASS(ENUM) \
    [[maybe_unused]] [[nodiscard]] \
    static constexpr auto operator&(const std::underlying_type_t<ENUM> Num, \
                                    const ENUM Mask) noexcept\
    { \
        return (Num & static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] \
    static constexpr auto operator&=(std::underlying_type_t<ENUM> &Num, \
                                     const ENUM Mask) noexcept\
    { \
        Num &= static_cast<std::underlying_type_t<ENUM>>(Mask); \
    } \
\
    [[maybe_unused]] [[nodiscard]] \
    static constexpr auto operator|(const std::underlying_type_t<ENUM> Num, \
                                    const ENUM Mask) noexcept\
    { \
        return (Num | static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    }\
\
    [[maybe_unused]] [[nodiscard]] \
    static constexpr auto operator|(const ENUM Num, const ENUM Mask) noexcept \
    { \
        return (static_cast<std::underlying_type_t<ENUM>>(Num) | \
                static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    }\
\
    [[maybe_unused]] \
    static constexpr auto operator|=(std::underlying_type_t<ENUM> &Num,\
                                     const ENUM Mask) noexcept\
    { \
        return (Num |= static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] \
    static constexpr auto operator==(std::underlying_type_t<ENUM> Num,\
                                     const ENUM Mask) noexcept\
    { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] \
    static constexpr auto operator==(const ENUM Mask,\
                                     std::underlying_type_t<ENUM> Num) noexcept\
    { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] \
    static constexpr auto operator!=(std::underlying_type_t<ENUM> Num,\
                                     const ENUM Mask) noexcept\
    { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] \
    static constexpr auto operator!=(const ENUM Mask,\
                                     std::underlying_type_t<ENUM> Num) noexcept\
    { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    }
