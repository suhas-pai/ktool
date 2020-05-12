//
//  include/Utils/Uleb128.
//  stool
//
//  Created by Suhas Pai on 5/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>
#include "IntegerLimit.h"

// The templates Convert [T = uint8_t *] to [T = const uint8_t *]

template <typename T>
constexpr static inline bool EnableIfUleb128Type =
    std::is_same_v<
        std::add_pointer_t<
            std::add_const_t<
                std::remove_pointer_t<T>>>, const uint8_t *>;

[[maybe_unused]]
constexpr static inline bool Uleb128ByteIsDone(const uint8_t &Byte) noexcept {
    return !(Byte & 0x80);
}

[[maybe_unused]]
constexpr static inline uint8_t Uleb128ByteGetBits(const uint8_t &Byte)
noexcept {
    return (Byte & 0x7f);
}

template <typename Integer>
constexpr static inline uint8_t Uleb128GetIntegerMaxShift() {
    static_assert(std::is_integral_v<Integer>,
                  "Integer-Type is not an integer");

    constexpr auto BitSize = static_cast<uint8_t>(sizeof(Integer) * 8);
    constexpr auto MaxFactor = static_cast<uint8_t>(BitSize / 7);

    return static_cast<uint8_t>(MaxFactor * 7);
}

template <typename Integer>
constexpr static inline uint8_t Uleb128GetLastByteValueMax() {
    static_assert(std::is_integral_v<Integer>,
                  "Integer-Type is not an integer");

    constexpr auto BitSize = (sizeof(Integer) * 8);
    constexpr auto Mod = (BitSize % 7);

    if constexpr (Mod == 0) {
        return 0;
    }

    return ((1ull << Mod) - 1);
}

template <typename FakeIntegerLimit = IntegerLimitDefaultType,
          typename T,
          typename ValueType>

static inline T ReadUleb128(T Begin, T End, ValueType *ValueOut) noexcept {
    using RealIntegerLimitType = IntegerLimitRealValueType<ValueType>;

    static_assert(EnableIfUleb128Type<T>, "Uleb128 Type is not uint8_t *");
    static_assert(std::is_integral_v<RealIntegerLimitType>,
                  "Integer-Type is not an integer");

    auto Iter = Begin;
    auto Byte = *Iter;

    Iter++;
    if (Uleb128ByteIsDone(Byte)) {
        *ValueOut = Byte;
        return Iter;
    }

    if (Begin == End) {
        return nullptr;
    }

    using IntegerType = IntegerLimitUseType<ValueType>;
    constexpr auto MaxShift = Uleb128GetIntegerMaxShift<IntegerType>();

    auto Bits = Uleb128ByteGetBits(Byte);
    auto Value = IntegerType(Bits);

    for (auto Shift = static_cast<uint8_t>(7); Shift != MaxShift; Shift += 7) {
        Byte = *Iter;
        Bits = Uleb128ByteGetBits(Byte);
        Value |= (IntegerType(Bits) << Shift);
        Iter++;

        if (Uleb128ByteIsDone(Byte)) {
            *ValueOut = Value;
            return Iter;
        }

        if (Iter == End) {
            return nullptr;
        }
    }

    if (constexpr auto ValueMax = Uleb128GetLastByteValueMax<IntegerType>()) {
        Byte = *Iter;
        if (Byte > ValueMax) {
            return nullptr;
        }

        Iter++;
        Value |= (IntegerType(Uleb128ByteGetBits(Byte)) << MaxShift);

        if (Uleb128ByteIsDone(Byte)) {
            *ValueOut = Value;
            return Iter;
        }
    }

    return nullptr;
}

template <typename Integer = uint64_t, typename T>
T SkipUleb128(T Begin, T End) noexcept {
    auto Value = Integer();
    return ReadUleb128(Begin, End, &Value);
}
