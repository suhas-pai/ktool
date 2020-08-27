//
//  include/ADT/RelativeRange.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <cstdint>

struct LocationRange;
struct RelativeRange {
protected:
    uint64_t End;
public:
    constexpr RelativeRange(uint64_t SizeOrEnd) noexcept : End(SizeOrEnd) {}
    RelativeRange(void *End) noexcept : End(reinterpret_cast<uint64_t>(End)) {}

    [[nodiscard]] constexpr inline uint64_t getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] constexpr inline uint64_t size() const noexcept {
        return End;
    }

    [[nodiscard]]
    bool containsLocRange(const LocationRange &LocRange) const noexcept;

    [[nodiscard]]
    constexpr inline bool containsLocation(uint64_t Location) const noexcept {
        return (Location < End);
    }

    [[nodiscard]] constexpr
    inline bool containsEndLocation(uint64_t EndLocation) const noexcept {
        return (EndLocation <= End);
    }

    [[nodiscard]] bool overlaps(const LocationRange &LocRange) const noexcept;

    [[nodiscard]]
    constexpr inline bool goesPastSize(uint64_t Size) const noexcept {
        return (End > Size);
    }
};
