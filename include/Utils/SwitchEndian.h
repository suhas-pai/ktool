//
//  Utils/SwitchEndian.h
//  ktool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <concepts>
#include <cstdint>

struct EndianSwitcherFuncs {
    [[nodiscard]]
    constexpr static uint16_t SwitchEndian16(uint16_t Value) noexcept {
        Value = ((Value >> 8) & 0x00ff) | ((Value << 8) & 0xff00);
        return Value;
    }

    [[nodiscard]]
    constexpr static uint32_t SwitchEndian32(uint32_t Value) noexcept {
        Value = ((Value >> 8) & 0x00ff00ff) | ((Value << 8) & 0xff00ff00);
        Value = ((Value >> 16) & 0x0000ffff) | ((Value << 16) & 0xffff0000);

        return Value;
    }

    [[nodiscard]]
    constexpr static uint64_t SwitchEndian64(uint64_t Value) noexcept {
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
    constexpr static int8_t SwitchEndian(int8_t Value) noexcept {
        return Value;
    }
};

template <>
struct EndianSwitcher<uint8_t> {
    [[nodiscard]]
    constexpr static uint8_t SwitchEndian(uint8_t Value) noexcept {
        return Value;
    }
};

template <>
struct EndianSwitcher<int16_t> {
    [[nodiscard]]
    constexpr static int16_t SwitchEndian(int16_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian16(Value);
    }
};

template <>
struct EndianSwitcher<uint16_t> {
    [[nodiscard]]
    constexpr static uint16_t SwitchEndian(uint16_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian16(static_cast<int16_t>(Value));
    }
};

template <>
struct EndianSwitcher<int32_t> {
    [[nodiscard]]
    constexpr static int32_t SwitchEndian(int32_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian32(Value);
    }
};

template <>
struct EndianSwitcher<uint32_t> {
    [[nodiscard]]
    constexpr static uint32_t SwitchEndian(uint32_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian32(Value);
    }
};

template <>
struct EndianSwitcher<int64_t> {
    [[nodiscard]]
    constexpr static int64_t SwitchEndian(int64_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian64(Value);
    }
};

template <>
struct EndianSwitcher<uint64_t> {
    [[nodiscard]]
    constexpr static uint64_t SwitchEndian(uint64_t Value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian64(Value);
    }
};

template <std::integral T>
[[nodiscard]] constexpr T SwitchEndian(T Value) noexcept {
    return EndianSwitcher<T>::SwitchEndian(Value);
}

template <std::integral T>
[[nodiscard]] constexpr T SwitchEndianIf(T Value, bool Cond) noexcept {
    return Cond ? SwitchEndian(Value) : Value;
}
