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

        [[nodiscard]] constexpr auto front() const noexcept {
            return this->Begin;
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return this->Size;
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return Utils::AddAndCheckOverflow(this->front(), this->size());
        }

        template <std::integral T>
        [[nodiscard]] constexpr auto canRepresentIn() const noexcept {
            return this->front() <= std::numeric_limits<T>::max() &&
                   this->size() <=
                    std::numeric_limits<T>::max() - this->front();
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return this->size() == 0;
        }

        [[nodiscard]]
        constexpr auto hasLoc(const uint64_t Loc) const noexcept {
            return Loc >= this->front() && (Loc - this->front()) < this->size();
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] constexpr auto
        hasLoc(const uint64_t Loc, const uint64_t Count = 1) const noexcept {
            const auto TotalOpt = Utils::MulAndCheckOverflow(Size, Count);
            if (!TotalOpt.has_value()) {
                return false;
            }

            const auto OffsetOpt =
                Utils::AddAndCheckOverflow(Loc - this->front(),
                                           TotalOpt.value());

            if (!OffsetOpt.has_value()) {
                return false;
            }

            return Loc >= this->front() && OffsetOpt.value() < this->size();
        }

        [[nodiscard]]
        constexpr auto hasEnd(const uint64_t Loc) const noexcept {
            return Loc > this->front() && (Loc - this->front()) <= this->size();
        }

        [[nodiscard]]
        constexpr auto hasIndex(const uint64_t Idx) const noexcept {
            return Idx < this->size();
        }

        [[nodiscard]]
        constexpr auto hasEndIndex(const uint64_t Idx) const noexcept {
            return Idx != 0 && Idx <= this->size();
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] constexpr auto
        hasIndex(const uint64_t Idx, const uint64_t Count = 1) const noexcept {
            if (const auto TotalOpt = Utils::MulAndCheckOverflow(Size, Count)) {
                if (const auto EndOpt =
                        Utils::AddAndCheckOverflow(Idx, TotalOpt.value()))
                {
                    return hasEndIndex(EndOpt.value());
                }
            }

            return false;
        }

        [[nodiscard]]
        constexpr auto isBelow(const Range &Range) const noexcept {
            if (this->empty()) {
                return true;
            }

            if (Range.front() < this->front()) {
                return false;
            }

            const auto BeginIndex = this->front() - Range.front();
            return BeginIndex >= Range.size();
        }

        [[nodiscard]]
        constexpr auto isAbove(const Range &Range) const noexcept {
            if (this->empty()) {
                return true;
            }

            if (this->front() < Range.front()) {
                return false;
            }

            const auto BeginIndex = Range.front() - this->front();
            return BeginIndex >= this->size();
        }

        [[nodiscard]] constexpr auto
        indexForLoc(const uint64_t Loc,
                    uint64_t *const MaxSizeOut = nullptr) const noexcept
        {
            assert(hasLoc(Loc));

            const auto Index = Loc - this->front();
            if (MaxSizeOut != nullptr) {
                *MaxSizeOut = size() - Index;
            }

            return Index;
        }

        [[nodiscard]]
        constexpr auto locForIndex(const uint64_t Index) const noexcept {
            assert(this->hasIndex(Index));
            return this->front() + Index;
        }

        [[nodiscard]]
        constexpr auto contains(const Range &Other) const noexcept {
            if (Other.empty()) {
                return true;
            }

            if (Other.front() < this->front()) {
                return false;
            }

            const auto MinSize = Other.size() + (Other.front() - this->front());
            return this->front() <= Other.front() && this->size() >= MinSize;
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]]
        constexpr auto canContain(const uint64_t Count = 1) const noexcept {
            if (const auto TotalOpt = Utils::MulAndCheckOverflow(Size, Count)) {
                return this->size() >= TotalOpt.value();
            }

            return false;
        }

        [[nodiscard]]
        constexpr auto canContainSize(const uint64_t Size) const noexcept {
            return Size < this->size();
        }

        [[nodiscard]]
        constexpr auto overlaps(const Range &Other) const noexcept {
            if (this->empty() || Other.empty()) {
                return false;
            }

            if (const auto OtherEnd = Other.end()) {
                if (this->hasEnd(OtherEnd.value())) {
                    return true;
                }
            }

            return this->hasLoc(Other.front()) || Other.contains(*this);
        }

        [[nodiscard]]
        constexpr auto containsAsIndex(const Range &Other) const noexcept {
            const auto MinSize = Other.size() - Other.front();
            return hasIndex(Other.front()) && this->size() >= MinSize;
        }

        [[nodiscard]]
        constexpr auto indexForLocRange(const Range &Range) const noexcept {
            assert(this->contains(Range));
            return Range::FromSize(Range.front() - this->front(), Range.size());
        }

        [[nodiscard]]
        constexpr auto locForIndexRange(const Range &Range) const noexcept {
            assert(this->containsAsIndex(Range));
            return Range::FromSize(this->front() + Range.front(), Range.size());
        }

        [[nodiscard]]
        constexpr auto fromIndex(const uint64_t Index) const noexcept {
            assert(this->hasIndex(Index));
            return Range::FromSize(Index, this->size() - Index);
        }

        [[nodiscard]]
        constexpr auto toIndex(const uint64_t Index) const noexcept {
            assert(this->hasIndex(Index));
            return Range::FromSize(0, Index);
        }

        [[nodiscard]]
        constexpr auto operator==(const Range &Range) const noexcept {
            return this->front() == Range.front() &&
                   this->size() == Range.size();
        }

        [[nodiscard]]
        constexpr auto operator!=(const Range &Range) const noexcept {
            return !operator==(Range);
        }
    };
}
