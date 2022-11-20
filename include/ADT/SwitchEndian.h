//
//  SwitchEndian.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once

#include <concepts>
#include <type_traits>

struct EndianSwitcherFuncs {
    [[nodiscard]]
    constexpr static auto SwitchEndian16(uint16_t Value) noexcept {
        Value = ((Value >> 8) & 0x00ff) | ((Value << 8) & 0xff00);
        return Value;
    }

    [[nodiscard]]
    constexpr static auto SwitchEndian32(uint32_t Value) noexcept {
        Value = ((Value >> 8) & 0x00ff00ff) | ((Value << 8) & 0xff00ff00);
        Value = ((Value >> 16) & 0x0000ffff) | ((Value << 16) & 0xffff0000);

        return Value;
    }

    [[nodiscard]]
    constexpr static auto SwitchEndian64(uint64_t Value) noexcept {
        Value =
            (Value & 0x00000000ffffffffULL) << 32 |
            (Value & 0xffffffff00000000ULL) >> 32;

        Value =
            (Value & 0x0000ffff0000ffffULL) << 16 |
            (Value & 0xffff0000ffff0000ULL) >> 16;

        Value =
            (Value & 0x00ff00ff00ff00ffULL) << 8 |
            (Value & 0xff00ff00ff00ff00ULL) >> 8;

        return Value;
    }
};

template <std::integral T>
struct EndianSwitcher {};

template <>
struct EndianSwitcher<int8_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const int8_t Value) noexcept {
        return Value;
    }
};

template <>
struct EndianSwitcher<uint8_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const uint8_t Value) noexcept {
        return Value;
    }
};

template <>
struct EndianSwitcher<int16_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const int16_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian16(Value);
    }
};

template <>
struct EndianSwitcher<uint16_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const uint16_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian16(static_cast<int16_t>(Value));
    }
};

template <>
struct EndianSwitcher<int32_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const int32_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian32(Value);
    }
};

template <>
struct EndianSwitcher<uint32_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const uint32_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian32(Value);
    }
};

template <>
struct EndianSwitcher<int64_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const int64_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian64(Value);
    }
};

template <>
struct EndianSwitcher<uint64_t> {
    [[nodiscard]]
    constexpr static auto SwitchEndian(const uint64_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian64(Value);
    }
};

template <std::integral T>
[[nodiscard]] constexpr auto SwitchEndian(const T Value) noexcept {
    return EndianSwitcher<T>::SwitchEndian(Value);
}

template <std::integral T>
[[nodiscard]]
constexpr auto SwitchEndianIf(const T Value, const bool Cond) noexcept {
    return Cond ? SwitchEndian(Value) : Value;
}

template <typename T,
          typename U = std::underlying_type_t<T>,
          typename = std::enable_if_t<std::is_integral_v<U>>>

[[nodiscard]]
constexpr auto SwitchEndianIf(const T Value, const bool Cond) noexcept {
    return Cond ?
        static_cast<T>(SwitchEndian(static_cast<U>(Value))) :
        Value;
}
