//
//  ADT/Range.h
//  ktool
//
//  Created by suhaspai on 10/4/22.
//

#pragma once
#include <cassert>

namespace ADT {
    struct Range {
    protected:
        uint64_t Begin;
        uint64_t End;

        constexpr explicit
        Range(const uint64_t Begin, const uint64_t End) noexcept
        : Begin(Begin), End(End) {}
    public:
        [[nodiscard]] constexpr static
        auto FromSize(const uint64_t Begin, const uint64_t Size) noexcept {
            return Range(Begin, Begin + Size);
        }

        [[nodiscard]] constexpr static
        auto FromEnd(const uint64_t Begin, const uint64_t End) noexcept {
            assert(Begin <= End);
            return Range(Begin, End);
        }

        [[nodiscard]] constexpr static auto FromEmpty() noexcept {
            return Range(0, 0);
        }

        [[nodiscard]] constexpr auto getBegin() const noexcept { return Begin; }
        [[nodiscard]] constexpr auto getEnd() const noexcept { return End; }
        [[nodiscard]] constexpr auto size() const noexcept {
            return (End - Begin);
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return (Begin == End);
        }

        [[nodiscard]]
        constexpr auto containsLoc(const uint64_t Loc) const noexcept {
            return (Loc >= Begin && Loc < End);
        }

        [[nodiscard]]
        constexpr auto containsEnd(const uint64_t Loc) const noexcept {
            return (Loc > Begin && Loc <= End);
        }

        [[nodiscard]]
        constexpr auto containsIdx(const uint64_t Idx) const noexcept {
            return (Idx < size());
        }

        [[nodiscard]]
        constexpr auto contains(const Range &Other) const noexcept {
            return (Begin <= Other.Begin && End >= Other.End);
        }

        [[nodiscard]]
        constexpr auto containsAsIdx(const Range &Other) const noexcept {
            return (Other.End <= End);
        }
    };
}
