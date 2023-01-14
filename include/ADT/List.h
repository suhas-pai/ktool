/*
 * ADT/List.h
 * © suhas pai
 */

#pragma once

#include <__ranges/concepts.h>
#include <compare>
#include <cstdint>
#include <iterator>
#include <ranges>

namespace ADT {
    template <typename T>
    struct List : public std::ranges::view_base {
    protected:
        T *Begin = nullptr;
        T *End = nullptr;
    public:
        constexpr List() noexcept = default;
        constexpr List(T *const Begin, T *const End) noexcept
        : Begin(Begin), End(End) {
            if (Begin != End) {
                assert(Begin != nullptr);
            }
        }

        constexpr List(T *const Begin, const uint64_t Size) noexcept
        : Begin(Begin), End(Begin + Size) {
            if (Size != 0) {
                assert(Begin != nullptr);
            }
        }

        constexpr List(std::ranges::contiguous_range auto&& Range) noexcept
        : Begin(std::ranges::begin(Range)), End(std::ranges::end(Range)) {}

        [[nodiscard]] constexpr auto data() const noexcept {
            return Begin;
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return (End - Begin);
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return size() == 0;
        }

        [[nodiscard]] constexpr auto &front() noexcept {
            assert(!empty());
            return *Begin;
        }

        [[nodiscard]] constexpr auto &back() noexcept {
            assert(!empty());
            return End[-1];
        }

        [[nodiscard]] constexpr auto &front() const noexcept {
            assert(!empty());
            return const_cast<const T&>(*Begin);
        }

        [[nodiscard]] constexpr auto &back() const noexcept {
            assert(!empty());
            return const_cast<const T&>(End[-1]);
        }

        [[nodiscard]] constexpr auto &at(const size_t Idx) noexcept {
            assert(Idx < size());
            return Begin[Idx];
        }

        [[nodiscard]] constexpr auto &at(const size_t Idx) const noexcept {
            assert(Idx < size());
            return const_cast<const T&>(Begin[Idx]);
        }

        struct Iterator {
        protected:
            T *Data = nullptr;
        public:
            constexpr Iterator() noexcept = default;
            constexpr Iterator(T *const Data) noexcept : Data(Data) {}

            using iterator_category = std::contiguous_iterator_tag;
            using value_type = T;
            using element_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            constexpr auto &operator++() noexcept {
                Data++;
                return *this;
            }

            constexpr auto operator++(int) noexcept {
                return operator++();
            }

            constexpr auto &operator--() noexcept {
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

            [[nodiscard]] constexpr friend auto
            operator+(const difference_type Amt, const Iterator &It) noexcept {
                return It + Amt;
            }

            [[nodiscard]] constexpr friend auto
            operator-(const difference_type Amt, const Iterator &It) noexcept {
                return It - Amt;
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
                -> decltype(*this)
            {
                for (auto I = difference_type(); I != Amt; I++) {
                    operator++();
                }

                return *this;
            }

            constexpr auto operator-=(const difference_type Amt) noexcept
                -> decltype(*this)
            {
                for (auto I = difference_type(); I != Amt; I++) {
                    operator--();
                }

                return *this;
            }

            [[nodiscard]] constexpr
            auto &operator[](const difference_type Index) const noexcept {
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

            [[nodiscard]] constexpr auto &operator*() const noexcept {
                return *Data;
            }

            [[nodiscard]] constexpr auto operator->() const noexcept {
                return Data;
            }
        };

        [[nodiscard]] constexpr auto begin() const noexcept {
            return Iterator(Begin);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return Iterator(End);
        }

        [[nodiscard]] constexpr auto cbegin() const noexcept {
            return Iterator(const_cast<const T *>(Begin));
        }

        [[nodiscard]] constexpr auto cend() const noexcept {
            return Iterator(const_cast<const T *>(End));
        }

        [[nodiscard]] constexpr auto rbegin() const noexcept {
            return std::reverse_iterator(begin());
        }

        [[nodiscard]] constexpr auto rend() const noexcept {
            return std::reverse_iterator(end());
        }

        [[nodiscard]] constexpr auto rcbegin() const noexcept {
            return std::reverse_iterator(cbegin());
        }

        [[nodiscard]] constexpr auto rcend() const noexcept {
            return std::reverse_iterator(cend());
        }
    };
}

static_assert(std::ranges::contiguous_range<ADT::List<int>>);
static_assert(std::ranges::view<ADT::List<int>>);
