//
//  ADT/Range.h
//  ktool
//
//  Created by suhaspai on 10/4/22.
//

#pragma once

#include <cassert>
#include <concepts>
#include <limits>

#include "Utils/Overflow.h"

namespace ADT {
    struct Range {
    protected:
        uint64_t Begin = 0;
        uint64_t Size = 0;

        constexpr
        explicit Range(const uint64_t Begin, const uint64_t Size) noexcept
        : Begin(Begin), Size(Size) {}
    public:
        [[nodiscard]] constexpr Range() noexcept = default;
        [[nodiscard]] constexpr static
        auto FromSize(const uint64_t Begin, const uint64_t Size) noexcept {
            return Range(Begin, Size);
        }

        [[nodiscard]] constexpr static
        auto FromEnd(const uint64_t Begin, const uint64_t End) noexcept {
            assert(Begin <= End);
            return Range(Begin, (End - Begin));
        }

        [[nodiscard]] constexpr auto begin() const noexcept { return Begin; }
        [[nodiscard]] constexpr auto size() const noexcept { return Size; }
        [[nodiscard]] constexpr auto end() const noexcept {
            return Utils::AddAndCheckOverflow(Begin, Size);
        }

        template <std::integral T>
        [[nodiscard]] constexpr auto canRepresentIn() const noexcept {
            return (Begin <= std::numeric_limits<T>::max() &&
                    Size <= (std::numeric_limits<T>::max() - Begin));
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return Size == 0;
        }

        [[nodiscard]]
        constexpr auto containsLoc(const uint64_t Loc) const noexcept {
            return Loc >= Begin && (Loc - Begin) < Size;
        }

        template <typename T>
        [[nodiscard]] constexpr auto
        containsLoc(const uint64_t Loc,
                    const uint64_t Count = 1) const noexcept
        {
            return Loc >= Begin && (Loc + (sizeof(T) * Count) - Begin) < Size;
        }

        [[nodiscard]]
        constexpr auto containsEnd(const uint64_t Loc) const noexcept {
            return Loc > Begin && (Loc - Begin) <= Size;
        }

        [[nodiscard]]
        constexpr auto containsIndex(const uint64_t Idx) const noexcept {
            return Idx < Size;
        }

        template <typename T>
        [[nodiscard]] constexpr auto
        containsIndex(const uint64_t Idx,
                      const uint64_t Count = 1) const noexcept
        {
            return containsIndex(Idx + (sizeof(T) * Count));
        }

        [[nodiscard]]
        constexpr auto containsEndIndex(const uint64_t Idx) const noexcept {
            return Idx != 0 && Idx <= Size;
        }

        [[nodiscard]] constexpr auto
        indexForLoc(const uint64_t Loc,
                    uint64_t *const MaxSizeOut = nullptr) const noexcept
        {
            assert(containsLoc(Loc));

            const auto Index = Loc - Begin;
            if (MaxSizeOut != nullptr) {
                *MaxSizeOut = Size - Index;
            }

            return Index;
        }

        [[nodiscard]]
        constexpr auto locForIndex(const uint64_t Index) const noexcept {
            assert(containsIndex(Index));
            return Begin + Index;
        }

        [[nodiscard]]
        constexpr auto contains(const Range &Other) const noexcept {
            if (Other.empty()) {
                return true;
            }

            if (Other.Begin < Begin) {
                return false;
            }

            const auto MinSize = (Other.Size + (Other.Begin - Begin));
            return Begin <= Other.Begin && Size >= MinSize;
        }

        template <typename T>
        [[nodiscard]]
        constexpr auto canContain(const uint64_t Count = 1) const noexcept {
            return Size >= (sizeof(T) * Count);
        }

        [[nodiscard]]
        constexpr auto canContainSize(const uint64_t Size) const noexcept {
            return Size < size();
        }

        [[nodiscard]]
        constexpr auto overlaps(const Range &Other) const noexcept {
            if (empty() || Other.empty()) {
                return false;
            }

            if (const auto OtherEnd = Other.end()) {
                if (containsEnd(OtherEnd.value())) {
                    return true;
                }
            }

            return containsLoc(Other.Begin) || Other.contains(*this);
        }

        [[nodiscard]]
        constexpr auto containsAsIndex(const Range &Other) const noexcept {
            const auto MinSize = (Other.Size + (Other.Begin - Begin));
            return containsIndex(Other.Begin) && Size >= MinSize;
        }

        [[nodiscard]]
        constexpr auto indexForLocRange(const Range &Range) const noexcept {
            assert(contains(Range));
            return ADT::Range::FromSize(Range.begin() - Begin, Range.size());
        }

        [[nodiscard]]
        constexpr auto locForIndexRange(const Range &Range) const noexcept {
            assert(containsAsIndex(Range));
            return ADT::Range::FromSize(Begin + Range.begin(), Range.size());
        }

        [[nodiscard]]
        constexpr auto operator==(const Range &Range) const noexcept {
            return Begin == Range.Begin && Size == Range.Size;
        }

        [[nodiscard]]
        constexpr auto operator!=(const Range &Range) const noexcept {
            return !operator==(Range);
        }
    };
}
