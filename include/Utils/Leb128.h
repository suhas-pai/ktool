//
//  include/Utils/Leb128.h
//  ktool
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
concept Leb128Type =
    std::is_same_v<
        std::add_pointer_t<
            std::add_const_t<
                std::remove_pointer_t<T>>>, const uint8_t *>;

[[maybe_unused]]
constexpr static bool Leb128ByteIsDone(const uint8_t &Byte) noexcept {
    return !(Byte & 0x80);
}

[[maybe_unused]] constexpr
static inline uint8_t Leb128ByteGetBits(const uint8_t &Byte) noexcept {
    return (Byte & 0x7f);
}

template <std::integral Integer>
constexpr static uint8_t Leb128GetIntegerMaxShift() {
    constexpr auto BitSize = static_cast<uint8_t>(sizeof(Integer) * 8);
    constexpr auto MaxFactor = static_cast<uint8_t>(BitSize / 7);

    return static_cast<uint8_t>(MaxFactor * 7);
}

template <std::integral Integer>
constexpr static uint8_t Leb128GetLastByteValueMax() {
    constexpr auto BitSize = (sizeof(Integer) * 8);
    constexpr auto Mod = (BitSize % 7);

    if constexpr (Mod == 0) {
        return 0;
    }

    return ((1ull << Mod) - 1);
}

constexpr uint64_t
Leb128SignExtendIfNecessary(uint64_t Value,
                            uint8_t Byte,
                            uint64_t Shift) noexcept
{
    if (!(Byte & 0x40)) {
        return Value;
    }

    const auto Mask = (~0ull << Shift);
    return (Value | Mask);
}

template <bool Signed,
          typename FakeIntegerLimit = IntegerLimitDefaultType,
          Leb128Type T,
          typename ValueType>

constexpr static
T ReadLeb128Base(T Begin, T End, ValueType *ValueOut) noexcept {
    using MaxValueType = std::conditional_t<Signed, int64_t, uint64_t>;
    using RealIntegerLimitType =
        IntegerLimitRealValueType<ValueType, MaxValueType>;

    using IntegerType =
        std::conditional_t<sizeof(RealIntegerLimitType) < sizeof(ValueType),
                           RealIntegerLimitType,
                           ValueType>;

    static_assert(std::is_integral_v<RealIntegerLimitType>,
                  "Integer-Type is not an integer-type");
    static_assert(Signed ^ std::is_unsigned_v<RealIntegerLimitType>,
                  "Integer-Type is not of proper kind (signed/unsigned)");

    auto Iter = Begin;
    auto Byte = *Iter;

    Iter++;
    if (Leb128ByteIsDone(Byte)) {
        *ValueOut = Byte;
        return Iter;
    }

    if (Begin == End) {
        return nullptr;
    }

    constexpr auto MaxShift = Leb128GetIntegerMaxShift<IntegerType>();

    auto Bits = Leb128ByteGetBits(Byte);
    auto Value = IntegerType(Bits);

    for (auto Shift = static_cast<uint8_t>(7); Shift != MaxShift; Shift += 7) {
        Byte = *Iter;
        Bits = Leb128ByteGetBits(Byte);
        Value |= (IntegerType(Bits) << Shift);
        Iter++;

        if (Leb128ByteIsDone(Byte)) {
            if constexpr (Signed) {
                Leb128SignExtendIfNecessary(Value, Byte, Shift + 7);
            }

            *ValueOut = Value;
            return Iter;
        }

        if (Iter == End) {
            return nullptr;
        }
    }

    if (constexpr auto ValueMax = Leb128GetLastByteValueMax<IntegerType>()) {
        Byte = *Iter;
        if (Byte > ValueMax) {
            return nullptr;
        }

        Iter++;
        Value |= (IntegerType(Leb128ByteGetBits(Byte)) << MaxShift);

        if (Leb128ByteIsDone(Byte)) {
            *ValueOut = Value;
            return Iter;
        }
    }

    return nullptr;
}


template <typename FakeIntegerLimit = IntegerLimitDefaultType,
          typename T,
          typename ValueType>

constexpr
static inline T ReadUleb128(T Begin, T End, ValueType *ValueOut) noexcept {
    const auto Result =
        ReadLeb128Base<false, FakeIntegerLimit, T, ValueType>(Begin,
                                                              End,
                                                              ValueOut);
    return Result;
}

template <typename Integer = uint64_t, typename T>
[[nodiscard]] T SkipUleb128(const T &Begin, const T &End) noexcept {
    auto Value = Integer();
    return ReadUleb128(Begin, End, &Value);
}

template <typename FakeIntegerLimit = IntegerLimitDefaultType,
          typename T,
          typename ValueType>

[[nodiscard]] constexpr static T
ReadSleb128(const T &Begin, const T &End, ValueType *ValueOut) noexcept {
    const auto Result =
        ReadLeb128Base<true, FakeIntegerLimit, T, ValueType>(Begin,
                                                             End,
                                                             ValueOut);
    return Result;
}

