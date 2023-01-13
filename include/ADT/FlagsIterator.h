//
//  ADT/FlagsIterator.h
//  ktool
//
//  Created by suhaspai on 11/23/22.
//

#pragma once

#include <concepts>
#include "ADT/FlagsBase.h"
#include "Utils/Misc.h"

namespace ADT {
    template <std::integral T>
    struct FlagsIterator {
    protected:
        FlagsBase<T> Value;
        uint8_t StartIndex = bit_sizeof(T);
    public:
        constexpr FlagsIterator(const T Value) noexcept
        : Value(Value), StartIndex(this->Value.getFirstSet()) {}

        constexpr FlagsIterator(const FlagsBase<T> Value) noexcept
        : Value(Value), StartIndex(Value.getFirstSet()) {}

        [[nodiscard]] inline auto begin() noexcept -> decltype(*this) {
            return *this;
        }

        struct EndValue {};
        [[nodiscard]] constexpr auto end() noexcept {
            return EndValue();
        }

        [[nodiscard]] constexpr
        auto operator<=>(const FlagsIterator<T> &Rhs) const noexcept = default;

        constexpr auto operator++() noexcept -> decltype(*this) {
            StartIndex = Value.getFirstSet(StartIndex + 1);
            return *this;
        }

        constexpr auto operator++(int) noexcept -> decltype(auto) {
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
            return StartIndex == bit_sizeof(T);
        }

        [[nodiscard]] constexpr
        auto operator!=([[maybe_unused]] const EndValue &End) const noexcept {
            return !operator==(End);
        }
    };
}