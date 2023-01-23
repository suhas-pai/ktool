/*
 * Utils/Leb128.h
 * © suhas pai
 */

#pragma once
#include <concepts>

namespace Utils {
    [[nodiscard]]
    constexpr static auto Leb128ByteIsDone(const uint8_t &Byte) noexcept {
        return !(Byte & 0x80);
    }

    [[nodiscard]]
    constexpr static auto Leb128ByteGetBits(const uint8_t &Byte) noexcept {
        return (Byte & 0x7f);
    }

    template <std::integral T>
    [[nodiscard]] constexpr static uint8_t Leb128GetIntegerMaxShift() {
        constexpr auto BitSize = uint8_t(sizeof(T) * 8);
        constexpr auto MaxFactor = uint8_t(BitSize / 7);

        return uint8_t(MaxFactor * 7);
    }

    template <std::integral T>
    [[nodiscard]] constexpr static auto Leb128GetLastByteValueMax() {
        constexpr auto BitSize = sizeof(T) * 8;
        constexpr auto Mod = BitSize % 7;

        if constexpr (Mod == 0) {
            return 0;
        }

        return (1ull << Mod) - 1;
    }

    [[nodiscard]] constexpr auto
    Leb128SignExtendIfNecessary(const uint64_t Value,
                                const uint8_t Byte,
                                const uint64_t Shift) noexcept
    {
        if (!(Byte & 0x40)) {
            return Value;
        }

        const auto Mask = (~0ull << Shift);
        return Value | Mask;
    }

    template <std::integral T, typename U, bool Signed>
    [[nodiscard]] constexpr static auto
    ReadLeb128Base(U *const Begin, U *const End, T *const ValueOut) noexcept
        -> U *
    {
        static_assert(Signed ^ std::unsigned_integral<T>,
                      "T must be an unsigned integer is Signed=False");

        auto Iter = static_cast<const uint8_t *>(Begin);
        auto Byte = *Iter;

        Iter++;
        if (Leb128ByteIsDone(Byte)) {
            *ValueOut = Byte;
            return Iter;
        }

        if (Begin == End) {
            return nullptr;
        }

        constexpr auto MaxShift = Leb128GetIntegerMaxShift<T>();

        auto Bits = Leb128ByteGetBits(Byte);
        auto Value = T(Bits);

        for (auto Shift = uint8_t(7); Shift != MaxShift; Shift += 7) {
            Byte = *Iter;
            Bits = Leb128ByteGetBits(Byte);
            Value |= T(Bits) << Shift;
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

        if (constexpr auto ValueMax = Leb128GetLastByteValueMax<T>()) {
            Byte = *Iter;
            if (Byte > ValueMax) {
                return nullptr;
            }

            Iter++;
            Value |= T(Leb128ByteGetBits(Byte)) << MaxShift;

            if (Leb128ByteIsDone(Byte)) {
                *ValueOut = Value;
                return Iter;
            }
        }

        return nullptr;
    }

    template <std::unsigned_integral T, typename U>
    [[nodiscard]] constexpr static
    auto ReadUleb128(U *const Begin, U *const End, T *const ValueOut) noexcept {
        return ReadLeb128Base<T, U, false>(Begin, End, ValueOut);
    }

    template <std::integral T = uint64_t>
    [[nodiscard]]
    auto SkipLeb128(const void *const Begin, const void *const End) noexcept {
        auto Value = T();
        return ReadLeb128Base<T, std::is_signed_v<T>>(Begin, End, &Value);
    }

    template <std::signed_integral T, typename U>
    [[nodiscard]] constexpr static auto
    ReadSleb128(U *const Begin, U *const End, T *const ValueOut) noexcept {
        return ReadLeb128Base<T, U, true>(Begin, End, ValueOut);
    }
}