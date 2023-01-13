/*
 * ADT/List.h
 * © suhas pai
 */

#pragma once

#include <compare>
#include <cstdint>

namespace ADT {
    template <typename T>
    struct List {
    protected:
        T *Begin = nullptr;
        T *End = nullptr;
    public:
        constexpr List() noexcept = default;
        constexpr List(T *const Begin, T *const End) noexcept
        : Begin(Begin), End(End) {}

        constexpr List(T *const Begin, const uint64_t Size) noexcept
        : Begin(Begin), End(Begin + Size) {}

        [[nodiscard]] constexpr auto size() const noexcept {
            return (End - Begin);
        }

        struct Iterator {
        protected:
            T *Data = nullptr;
        public:
            constexpr Iterator() noexcept = default;
            constexpr Iterator(T *const Data) noexcept : Data(Data) {}

            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            constexpr auto operator++() noexcept -> decltype (*this) {
                Data++;
                return *this;
            }

            constexpr auto operator++(int) noexcept {
                return operator++();
            }

            constexpr auto operator--() noexcept -> decltype (*this) {
                Data--;
                return *this;
            }

            constexpr auto operator--(int) noexcept {
                return operator--();
            }

            [[nodiscard]] constexpr
            auto operator+(const difference_type Amt) const noexcept {
                return Iterator(Data + Amt);
            }

            [[nodiscard]] constexpr
            auto operator-(const difference_type Amt) const noexcept {
                return Iterator(Data - Amt);
            }

            [[nodiscard]]
            constexpr auto operator-(const Iterator &Rhs) const noexcept {
                return (Data - Rhs.Data);
            }

            constexpr auto operator+=(const difference_type Amt) noexcept
                -> decltype (*this)
            {
                for (auto I = difference_type(); I != Amt; I++) {
                    operator++();
                }

                return *this;
            }

            [[nodiscard]]
            constexpr auto operator[](const difference_type Index) noexcept
                -> decltype(auto)
            {
                return Data[Index];
            }

            [[nodiscard]] constexpr
            auto operator<=>(const Iterator &Rhs) const noexcept = default;

            [[nodiscard]]
            constexpr auto operator==(const Iterator &Rhs) const noexcept {
                return (Rhs.Data == Data);
            }

            [[nodiscard]]
            constexpr auto operator!=(const Iterator &Rhs) const noexcept {
                return !operator==(Rhs);
            }

            [[nodiscard]]
            constexpr auto operator*() const noexcept -> decltype(auto) {
                return *Data;
            }
        };

        [[nodiscard]] constexpr auto begin() const noexcept {
            return Iterator(Begin);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return Iterator(End);
        }
    };
}
