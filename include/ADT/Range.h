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
                    size() <= (std::numeric_limits<T>::max() - Begin));
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return size() == 0;
        }

        [[nodiscard]]
        constexpr auto containsLoc(const uint64_t Loc) const noexcept {
            return Loc >= Begin && (Loc - Begin) < Size;
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] constexpr auto
        containsLoc(const uint64_t Loc,
                    const uint64_t Count = 1) const noexcept
        {
            const auto TotalOpt = Utils::MulAndCheckOverflow(Size, Count);
            if (!TotalOpt.has_value()) {
                return false;
            }

            const auto OffsetOpt =
                Utils::AddAndCheckOverflow(Loc - Begin, TotalOpt.value());

            if (!OffsetOpt.has_value()) {
                return false;
            }

            return Loc >= Begin && OffsetOpt.value() < this->Size;
        }

        [[nodiscard]]
        constexpr auto containsEnd(const uint64_t Loc) const noexcept {
            return Loc > Begin && (Loc - Begin) <= Size;
        }

        [[nodiscard]]
        constexpr auto containsIndex(const uint64_t Idx) const noexcept {
            return Idx < Size;
        }

        [[nodiscard]]
        constexpr auto containsEndIndex(const uint64_t Idx) const noexcept {
            return Idx != 0 && Idx <= Size;
        }

        [[nodiscard]]
        constexpr auto isBelow(const Range &Range) const noexcept {
            if (empty()) {
                return true;
            }

            if (Range.begin() < Begin) {
                return false;
            }

            const auto BeginIndex = Begin - Range.begin();
            return BeginIndex >= Range.size();
        }

        [[nodiscard]]
        constexpr auto isAbove(const Range &Range) const noexcept {
            if (empty()) {
                return true;
            }

            if (Begin < Range.begin()) {
                return false;
            }

            const auto BeginIndex = Range.begin() - Begin;
            return BeginIndex >= size();
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] constexpr auto
        containsIndex(const uint64_t Idx,
                      const uint64_t Count = 1) const noexcept
        {
            if (const auto TotalOpt = Utils::MulAndCheckOverflow(Size, Count)) {
                if (const auto EndOpt =
                        Utils::AddAndCheckOverflow(Idx, TotalOpt.value()))
                {
                    return containsEndIndex(EndOpt.value());
                }
            }

            return false;
        }

        [[nodiscard]] constexpr auto
        indexForLoc(const uint64_t Loc,
                    uint64_t *const MaxSizeOut = nullptr) const noexcept
        {
            assert(containsLoc(Loc));

            const auto Index = Loc - Begin;
            if (MaxSizeOut != nullptr) {
                *MaxSizeOut = size() - Index;
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
            return Begin <= Other.Begin && size() >= MinSize;
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]]
        constexpr auto canContain(const uint64_t Count = 1) const noexcept {
            if (const auto TotalOpt = Utils::MulAndCheckOverflow(Size, Count)) {
                return size() >= TotalOpt.value();
            }

            return false;
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
            return containsIndex(Other.Begin) && size() >= MinSize;
        }

        [[nodiscard]]
        constexpr auto indexForLocRange(const Range &Range) const noexcept {
            assert(contains(Range));
            return Range::FromSize(Range.begin() - Begin, Range.size());
        }

        [[nodiscard]]
        constexpr auto locForIndexRange(const Range &Range) const noexcept {
            assert(containsAsIndex(Range));
            return Range::FromSize(Begin + Range.begin(), Range.size());
        }

        [[nodiscard]]
        constexpr auto fromIndex(const uint64_t Index) const noexcept {
            assert(containsIndex(Index));
            return Range::FromSize(Index, size() - Index);
        }

        [[nodiscard]]
        constexpr auto toIndex(const uint64_t Index) const noexcept {
            assert(containsIndex(Index));
            return Range::FromSize(0, Index);
        }

        [[nodiscard]]
        constexpr auto operator==(const Range &Range) const noexcept {
            return Begin == Range.Begin && size() == Range.Size;
        }

        [[nodiscard]]
        constexpr auto operator!=(const Range &Range) const noexcept {
            return !operator==(Range);
        }
    };
}
