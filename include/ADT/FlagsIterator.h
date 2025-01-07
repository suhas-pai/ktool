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
    template <std::unsigned_integral T>
    struct FlagsIterator {
    protected:
        T Value;
        uint8_t BitIndex = bit_sizeof(T);
    public:
        using value_type = uint8_t;
        using difference_type = ptrdiff_t;

        constexpr FlagsIterator() noexcept = default;
        constexpr FlagsIterator(const T Value, const uint8_t BitIndex) noexcept
        : Value(Value), BitIndex(BitIndex) {}

        [[nodiscard]] constexpr
        auto operator<=>(const FlagsIterator<T> &Rhs) const noexcept = default;

        constexpr auto operator++() noexcept -> decltype(*this) {
            if (this->BitIndex < bit_sizeof(T) - 1) {
                this->BitIndex++;
                this->BitIndex +=
                    __builtin_ctzll(
                        static_cast<unsigned long long>(
                            Value >> this->BitIndex));
            } else {
                this->BitIndex = bit_sizeof(T);
            }

            return *this;
        }

        constexpr auto operator++(int) noexcept {
            return this->operator++();
        }

        [[nodiscard]] constexpr auto operator*() const noexcept {
            return this->BitIndex;
        }

        [[nodiscard]] constexpr auto mask() const noexcept {
            return T(1) << this->BitIndex;
        }

        [[nodiscard]]
        constexpr auto operator==(const FlagsIterator<T> &Rhs) const noexcept {
            return this->BitIndex == Rhs.BitIndex;
        }

        struct Sentinel {
        public:
            Sentinel() noexcept = default;
            Sentinel([[maybe_unused]] const FlagsIterator<T> &Iter) noexcept {}

            [[nodiscard]]
            auto operator==(const FlagsIterator<T> &It) const noexcept {
                return It.BitIndex == bit_sizeof(T);
            }

            [[nodiscard]] auto operator==(const Sentinel &) const noexcept {
                return true;
            }
        };

        [[nodiscard]] constexpr
        auto operator==([[maybe_unused]] const Sentinel &End) const noexcept {
            return this->BitIndex == bit_sizeof(T);
        }
    };

    static_assert(std::forward_iterator<FlagsIterator<uint64_t>>);
}