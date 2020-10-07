//
//  include/ADT/LocationRange.h
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstdint>

#include <limits>
#include <optional>

#include "Utils/DoesOverflow.h"

struct RelativeRange;
struct LocationRange {
private:
    uint64_t Begin = 0;
    uint64_t End = 0;

    constexpr explicit LocationRange(uint64_t Begin, uint64_t End) noexcept
    : Begin(Begin), End(End) {}
public:
    constexpr LocationRange() noexcept = default;
    [[nodiscard]] static LocationRange Empty() noexcept {
        return LocationRange();
    }

    template <typename T = uint64_t>
    [[nodiscard]] constexpr static std::optional<LocationRange>
    CreateWithSize(uint64_t Begin, uint64_t Size) noexcept {
        auto End = T();
        if (DoesAddOverflow(Begin, Size, &End)) {
            return std::nullopt;
        }

        return LocationRange(Begin, End);
    }

    template <typename T = uint64_t>
    [[nodiscard]] constexpr static std::optional<LocationRange>
    CreateWithSize(void *Begin, uint64_t Size) noexcept {
        const auto BeginInt = reinterpret_cast<uint64_t>(Begin);
        return LocationRange::CreateWithSize<T>(BeginInt, Size);
    }

    [[nodiscard]] constexpr static inline
    LocationRange CreateWithEnd(uint64_t Begin, uint64_t End) noexcept {
        assert(Begin <= End);
        return LocationRange(Begin, End);
    }

    [[nodiscard]] static inline
    LocationRange CreateWithEnd(const void *Begin, const void *End) noexcept {
        const auto BeginInt = reinterpret_cast<uint64_t>(Begin);
        const auto EndInt = reinterpret_cast<uint64_t>(End);

        return LocationRange::CreateWithEnd(BeginInt, EndInt);
    }

    template <typename T = uint64_t>
    [[nodiscard]] constexpr
    static bool IsSizeRangeValid(uint64_t Begin, uint64_t Size) noexcept {
        return DoesAddOverflow<T>(Begin, Size);
    }

    [[nodiscard]] constexpr inline uint64_t getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] constexpr inline uint64_t getEnd() const noexcept {
        return End;
    }

    constexpr inline LocationRange &setBegin(uint64_t Begin) noexcept {
        this->Begin = Begin;
        return *this;
    }

    constexpr inline LocationRange &setEnd(uint64_t End) noexcept {
        this->End = End;
        return *this;
    }

    [[nodiscard]] constexpr inline uint64_t size() const noexcept {
        return (End - Begin);
    }

    [[nodiscard]] constexpr inline bool empty() const noexcept {
        return (size() == 0);
    }

    [[nodiscard]]
    constexpr inline bool containsLocation(uint64_t Location) const noexcept {
        return (!empty() && (Location >= Begin && Location < End));
    }

    [[nodiscard]] constexpr
    inline bool containsEndLocation(uint64_t EndLocation) const noexcept {
        return (!empty() && (EndLocation > Begin && EndLocation <= End));
    }

    [[nodiscard]] constexpr
    inline bool containsLocation(const void *Location) const noexcept {
        return containsLocation(reinterpret_cast<uint64_t>(Location));
    }

    [[nodiscard]] constexpr
    inline bool containsEndLocation(const void *EndLocation) const noexcept {
        return containsEndLocation(reinterpret_cast<uint64_t>(EndLocation));
    }

    [[nodiscard]] constexpr inline
    bool containsRelativeEndLocation(uint64_t EndLocation) const noexcept {
        return (!empty() && (EndLocation <= End));
    }

    [[nodiscard]] constexpr
    inline bool containsRelativeLocation(uint64_t Location) const noexcept {
        return (Location < size());
    }

    template <typename T>
    [[nodiscard]] constexpr
    inline bool containsTypeAtLocation(uint64_t Location) const noexcept {
        auto End = uint64_t();
        if (DoesAddOverflow(Location, sizeof(T), &End)) {
            return false;
        }

        return (containsLocation(Location) && containsEndLocation(End));
    }

    template <typename T>
    [[nodiscard]] constexpr
    inline bool containsTypeAtLocation(const void *Location) const noexcept {
        return containsTypeAtLocation<T>(reinterpret_cast<uint64_t>(Location));
    }

    template <typename T>
    [[nodiscard]] constexpr inline
    bool containsTypeAtRelativeLocation(uint64_t Location) const noexcept {
        auto End = uint64_t();
        if (DoesAddOverflow(Location, sizeof(T), &End)) {
            return false;
        }

        const auto Result =
            (containsRelativeLocation(Location) &&
             containsRelativeEndLocation(End));

        return Result;
    }

    [[nodiscard]] constexpr
    inline bool contains(const LocationRange &LocRange) const noexcept {
        if (empty()) {
            return false;
        }

        const auto ContainsBegin = containsLocation(LocRange.Begin);
        const auto ContainsEnd = containsEndLocation(LocRange.End);

        if (LocRange.empty()) {
            return (ContainsBegin || ContainsEnd);
        }

        return (ContainsBegin && ContainsEnd);
    }

    [[nodiscard]] constexpr inline
    bool containsAsRelative(const LocationRange &LocRange) const noexcept {
        if (empty()) {
            return false;
        }

        const auto ContainsBegin = containsRelativeLocation(LocRange.Begin);
        const auto ContainsEnd = containsRelativeEndLocation(LocRange.End);

        if (LocRange.empty()) {
            return (ContainsBegin || ContainsEnd);
        }

        return (ContainsBegin && ContainsEnd);
    }

    [[nodiscard]] constexpr
    inline bool overlaps(const LocationRange &LocRange) const noexcept {
        if (empty() || LocRange.empty()) {
            return false;
        }

        const auto DoesOverlap =
            (containsLocation(LocRange.Begin) ||
             containsEndLocation(LocRange.End));

        if (DoesOverlap) {
            return true;
        }

        return LocRange.contains(*this);
    }

    [[nodiscard]] constexpr
    inline uint64_t sizeFromLocation(uint64_t Location) const noexcept {
        return sizeFromRelativeLocation(Location - Begin);
    }

    [[nodiscard]] constexpr
    inline uint64_t sizeFromRelativeLocation(uint64_t Location) const noexcept {
        return (End - Location);
    }

    [[nodiscard]] constexpr bool goesPastEnd(uint64_t Size) const noexcept {
        return (End > Size);
    }

    [[nodiscard]] RelativeRange toRelativeRange() const noexcept;
};
