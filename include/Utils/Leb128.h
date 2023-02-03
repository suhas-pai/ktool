/*
 * Utils/Leb128.h
 * © suhas pai
 */

#pragma once

#include <concepts>
#include <cstdint>

namespace Utils {
    [[nodiscard]]
    constexpr static auto Leb128ByteIsDone(const uint8_t &Byte) noexcept {
        return !(Byte & 0x80);
    }

    [[nodiscard]]
    constexpr static auto Leb128ByteGetBits(const uint8_t &Byte) noexcept {
        return Byte & 0x7f;
    }

    template <std::integral T>
    [[nodiscard]] constexpr static uint8_t Leb128GetIntegerMaxShift() noexcept {
        constexpr auto BitSize = uint8_t(sizeof(T) * 8);
        constexpr auto MaxFactor = uint8_t(BitSize / 7);

        return uint8_t(MaxFactor * 7);
    }

    template <std::integral T>
    [[nodiscard]] constexpr static auto Leb128GetLastByteValueMax() noexcept {
        constexpr auto BitSize = sizeof(T) * 8;
        constexpr auto Mod = BitSize % 7;

        if constexpr (Mod == 0) {
            return 0;
        }

        return (1ull << Mod) - 1;
    }

    template <std::integral T, typename U, bool Signed>
    constexpr static auto
    ReadLeb128Base(U *const Begin, U *const End, U **const PtrOut) noexcept -> T
    {
        static_assert(Signed ^ std::unsigned_integral<T>,
                      "T must be an unsigned integer if Signed=False");

        auto Iter = static_cast<const uint8_t *>(Begin);
        auto Byte = *Iter;

        Iter++;
        if (Leb128ByteIsDone(Byte)) {
            *PtrOut = (U *)Iter;
            return Byte;
        }

        if (Begin == End) {
            *PtrOut = nullptr;
            return Byte;
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
                *PtrOut = (U *)Iter;
                return Value;
            }

            if (Iter == End) {
                *PtrOut = nullptr;
                return Value;
            }
        }

        if (constexpr auto ValueMax = Leb128GetLastByteValueMax<T>()) {
            Byte = *Iter;
            if (Byte > ValueMax) {
                *PtrOut = nullptr;
                return Value;
            }

            Iter++;
            Value |= T(Leb128ByteGetBits(Byte)) << MaxShift;

            if (Leb128ByteIsDone(Byte)) {
                *PtrOut = (U *)Iter;
                return Value;
            }
        }

        *PtrOut = nullptr;
        return Value;
    }

    template <std::unsigned_integral T = uint64_t, typename U>
    [[nodiscard]] constexpr static
    auto ReadUleb128(U *const Begin, U *const End, U **const PtrOut) noexcept {
        return ReadLeb128Base<T, U, false>(Begin, End, PtrOut);
    }

    template <std::integral T = uint64_t, typename U>
    [[nodiscard]] auto SkipLeb128(U *const Begin, U *const End) noexcept {
        auto Ptr = U(nullptr);
        ReadLeb128Base<T, U, std::is_signed_v<T>>(Begin, End, &Ptr);

        return Ptr;
    }

    template <std::signed_integral T = int64_t, typename U>
    [[nodiscard]] constexpr static
    auto ReadSleb128(U *const Begin, U *const End, U **const PtrOut) noexcept {
        return ReadLeb128Base<T, U, true>(Begin, End, PtrOut);
    }
}