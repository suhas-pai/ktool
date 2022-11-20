//
//  ADT/Range.h
//  ktool
//
//  Created by suhaspai on 10/4/22.
//

#pragma once

#include <cassert>
#include <concepts>

namespace ADT {
    struct Range {
    protected:
        uint64_t Begin;
        uint64_t Size;

        constexpr explicit
        Range(const uint64_t Begin, const uint64_t Size) noexcept
        : Begin(Begin), Size(Size) {}
    public:
        [[nodiscard]] constexpr static
        auto FromSize(const uint64_t Begin, const uint64_t Size) noexcept {
            return Range(Begin, Size);
        }

        [[nodiscard]] constexpr static
        auto FromEnd(const uint64_t Begin, const uint64_t End) noexcept {
            assert(Begin <= End);
            return Range(Begin, (End - Begin));
        }

        [[nodiscard]] constexpr static auto FromEmpty() noexcept {
            return Range(0, 0);
        }

        [[nodiscard]] constexpr auto getBegin() const noexcept { return Begin; }
        [[nodiscard]] constexpr auto size() const noexcept { return Size; }
        [[nodiscard]] constexpr auto getEnd() const noexcept {
            return Begin + Size;
        }

        template <std::integral T>
        [[nodiscard]] constexpr auto canRepresentIn() {
            return (Begin <= std::numeric_limits<T>::max() &&
                    (Begin + Size) <= std::numeric_limits<T>::max());
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return (Size == 0);
        }

        [[nodiscard]]
        constexpr auto containsLoc(const uint64_t Loc) const noexcept {
            return (Loc >= Begin && (Loc - Begin) < Size);
        }

        [[nodiscard]]
        constexpr auto containsEnd(const uint64_t Loc) const noexcept {
            return (Loc > Begin && (Loc - Begin) <= Size);
        }

        [[nodiscard]]
        constexpr auto containsIndex(const uint64_t Idx) const noexcept {
            return (Idx < Size);
        }

        [[nodiscard]]
        constexpr auto contains(const Range &Other) const noexcept {
            if (Other.empty()) {
                return false;
            }

            const auto MinSize = (Other.Size + (Other.Begin - Begin));
            return (Begin <= Other.Begin && Size >= MinSize);
        }

        template <typename T>
        [[nodiscard]]
        constexpr auto canContain(const uint64_t Count = 1) const noexcept {
            return (Size >= (sizeof(T) * Count));
        }

        [[nodiscard]]
        constexpr auto overlaps(const Range &Other) const noexcept {
            if (empty() || Other.empty()) {
                return false;
            }

            return (containsLoc(Other.Begin) ||
                    containsEnd(Other.getEnd()) ||
                    Other.contains(*this));
        }

        [[nodiscard]]
        constexpr auto containsAsIndex(const Range &Other) const noexcept {
            return containsIndex(Other.Begin) && containsEnd(Other.getEnd());
        }

        [[nodiscard]]
        constexpr auto offsetSubrange(const auto &Range) const noexcept {
            return Range(Begin + Range.Offset, Range.Size);
        }
    };
}
