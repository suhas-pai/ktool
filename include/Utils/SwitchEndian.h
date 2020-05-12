//
//  include/Utils/SwitchEndian.h
//  stool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

struct EndianSwitcherFuncs {
    static uint16_t SwitchEndian16(uint16_t value) noexcept;
    static uint32_t SwitchEndian32(uint32_t value) noexcept;
    static uint64_t SwitchEndian64(uint64_t value) noexcept;
};

template <typename T>
struct EndianSwitcher {};

template <>
struct EndianSwitcher<int8_t> {
    [[nodiscard]]
    static inline int8_t SwitchEndian(int8_t value) noexcept { return value; }
};

template <>
struct EndianSwitcher<uint8_t> {
    [[nodiscard]]
    static inline uint8_t SwitchEndian(uint8_t value) noexcept { return value; }
};

template <>
struct EndianSwitcher<int16_t> {
    [[nodiscard]] static inline int16_t SwitchEndian(int16_t value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian16(value);
    }
};

template <>
struct EndianSwitcher<uint16_t> {
    [[nodiscard]] static inline uint16_t SwitchEndian(uint16_t value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian16(static_cast<int16_t>(value));
    }
};

template <>
struct EndianSwitcher<int32_t> {
    [[nodiscard]] static inline int32_t SwitchEndian(int32_t value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian32(value);
    }
};

template <>
struct EndianSwitcher<uint32_t> {
    [[nodiscard]] static inline uint32_t SwitchEndian(uint32_t value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian32(value);
    }
};

template <>
struct EndianSwitcher<int64_t> {
    [[nodiscard]] static inline int64_t SwitchEndian(int64_t value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian64(value);
    }
};

template <>
struct EndianSwitcher<uint64_t> {
    [[nodiscard]] static inline uint64_t SwitchEndian(uint64_t value) noexcept {
        return EndianSwitcherFuncs::SwitchEndian64(value);
    }
};

template <typename T>
inline T SwitchEndian(T value) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");
    return EndianSwitcher<T>::SwitchEndian(value);
}

template <typename T>
inline T SwitchEndianIf(T Value, bool ShouldSwitch) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");
    if (ShouldSwitch) {
        Value = EndianSwitcher<T>::SwitchEndian(Value);
    }

    return Value;
}
