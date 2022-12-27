//
//  ADT/FlagsIterator.h
//  ktool
//
//  Created by suhaspai on 11/23/22.
//

#pragma once

#include <concepts>
#include "Utils/Misc.h"

namespace ADT {
    template <std::integral T>
    struct FlagsIterator {
    protected:
        T Value = bit_sizeof(T);
        uint8_t StartIndex = 0;
    public:
        constexpr FlagsIterator(const T Value) noexcept
        : Value(Value),
          StartIndex(
            Value == 0 ?: static_cast<uint8_t>(__builtin_ctz(Value))) {}

        [[nodiscard]] inline auto &begin() {
            return *this;
        }

        struct EndValue {};
        [[nodiscard]] inline auto end() {
            return EndValue();
        }

        [[nodiscard]] constexpr auto
        operator<=>(const FlagsIterator<T> &Rhs) const noexcept = default;

        constexpr auto &operator++() noexcept {
            if (StartIndex + 1 == bit_sizeof(T)) {
                StartIndex = bit_sizeof(T);
                return *this;
            }

            const auto ShiftedValue = Value >> (StartIndex + 1);
            if (ShiftedValue == 0) {
                StartIndex = bit_sizeof(T);
                return *this;
            }

            StartIndex +=
                static_cast<uint8_t>(__builtin_ctz(Value >> (StartIndex + 1))) +
                1;

            return *this;
        }

        constexpr auto &operator++(int) noexcept {
            return operator++();
        }

        [[nodiscard]] constexpr auto operator*() const noexcept {
            return StartIndex;
        }

        [[nodiscard]] constexpr auto mask() const noexcept {
            return static_cast<T>(1) << StartIndex;
        }

        [[nodiscard]] constexpr
        auto operator==([[maybe_unused]] const EndValue &End) const noexcept {
            return StartIndex == bit_sizeof(T) || (Value >> StartIndex == 0);
        }

        [[nodiscard]] constexpr
        auto operator!=([[maybe_unused]] const EndValue &End) const noexcept {
            return !operator==(End);
        }
    };
}