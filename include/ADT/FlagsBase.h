//
//  ADT/FlagsBase.h
//  ktool
//
//  Created by suhaspai on 10/5/22.
//

#pragma once

#include <compare>
#include <concepts>
#include <type_traits>

namespace ADT {
    template <std::integral T>
    struct FlagsBase {
    protected:
        T Flags = 0;

        template <typename E>
        constexpr auto addMask(const E Mask) noexcept {
            Flags |= static_cast<T>(Mask);
            return *this;
        }

        template <typename E>
        constexpr auto removeMask(const E Mask) noexcept {
            Flags &= ~static_cast<T>(Mask);
            return *this;
        }

        template <typename E>
        constexpr auto setForValue(const E Mask, const bool Value) noexcept {
            return Value ? addMask(Mask) : removeMask(Mask);
        }
    public:
        constexpr FlagsBase() {}
        constexpr FlagsBase(const T Flags) : Flags(Flags) {}

        [[nodiscard]]
        constexpr auto operator|(const FlagsBase<T> Mask) const noexcept {
            return (Flags | Mask);
        }

        [[nodiscard]]
        constexpr auto operator&(const FlagsBase<T> Mask) const noexcept {
            return (Flags & Mask);
        }

        constexpr auto operator|=(const FlagsBase<T> Mask) const noexcept {
            Flags |= Mask;
            return *this;
        }

        constexpr auto operator&=(const FlagsBase<T> Mask) const noexcept {
            Flags |= Mask;
            return *this;
        }

        constexpr auto operator~() const noexcept {
            return ~Flags;
        }

        constexpr auto value() const noexcept {
            return Flags;
        }

        constexpr auto
        operator<=>(const FlagsBase<T> &Rhs) const noexcept = default;

        constexpr auto operator<=>(const T Rhs) const noexcept {
            return (Flags <=> Rhs);
        }
    };
}

#define MAKE_ENUM_MASK_CLASS(ENUM) \
    [[maybe_unused]] [[nodiscard]] static constexpr auto operator&(const std::underlying_type_t<ENUM> Num, const ENUM Mask) noexcept { \
        return (Num & static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] static constexpr auto operator&=(std::underlying_type_t<ENUM> &Num, const ENUM Mask) noexcept { \
        Num &= static_cast<std::underlying_type_t<ENUM>>(Mask); \
    } \
\
    [[maybe_unused]] [[nodiscard]] static constexpr auto operator|(const std::underlying_type_t<ENUM> Num, const ENUM Mask) noexcept { \
        return (Num | static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    }\
\
    [[maybe_unused]] [[nodiscard]] static constexpr auto operator|(const ENUM Num, const ENUM Mask) noexcept { \
        return (static_cast<std::underlying_type_t<ENUM>>(Num) | static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    }\
\
    [[maybe_unused]] static constexpr auto operator|=(std::underlying_type_t<ENUM> &Num, const ENUM Mask) noexcept { \
        return (Num |= static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] static constexpr auto operator==(std::underlying_type_t<ENUM> Num, const ENUM Mask) noexcept { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] static constexpr auto operator==(const ENUM Mask, std::underlying_type_t<ENUM> Num) noexcept { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] static constexpr auto operator!=(std::underlying_type_t<ENUM> Num, const ENUM Mask) noexcept { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
\
    [[maybe_unused]] static constexpr auto operator!=(const ENUM Mask, std::underlying_type_t<ENUM> Num) noexcept { \
        return (Num == static_cast<std::underlying_type_t<ENUM>>(Mask)); \
    } \
