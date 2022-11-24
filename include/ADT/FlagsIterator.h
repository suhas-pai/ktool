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
    struct FlagsIteratorEndValue {};

    template <std::integral T>
    struct FlagsIterator {
    protected:
        T Value;
        T StartIndex = 0;
    public:
        constexpr FlagsIterator(const T Value) noexcept
        : Value(Value), StartIndex(Value != 0 ? __builtin_ctz(Value) : 0) {}

        [[nodiscard]] inline auto &begin() {
            return *this;
        }

        [[nodiscard]] inline auto end() {
            return FlagsIteratorEndValue();
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

            StartIndex += __builtin_ctz(Value >> (StartIndex + 1)) + 1;
            return *this;
        }

        constexpr auto &operator++(int) noexcept {
            if (StartIndex + 1 == bit_sizeof(T)) {
                StartIndex = bit_sizeof(T);
                return *this;
            }

            const auto ShiftedValue = Value >> (StartIndex + 1);
            if (ShiftedValue == 0) {
                StartIndex = bit_sizeof(T);
                return *this;
            }

            StartIndex += __builtin_ctz(Value >> (StartIndex + 1)) + 1;
            return *this;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept {
            return StartIndex;
        }

        [[nodiscard]] constexpr auto mask() const noexcept {
            return static_cast<T>(1) << StartIndex;
        }

        [[nodiscard]] constexpr
        auto operator==(const FlagsIteratorEndValue &End) const noexcept {
            return StartIndex == bit_sizeof(T) || (Value >> StartIndex == 0);
        }

        [[nodiscard]] constexpr
        auto operator!=(const FlagsIteratorEndValue &End) const noexcept {
            return StartIndex != bit_sizeof(T) && (Value >> StartIndex != 0);
        }
    };
}