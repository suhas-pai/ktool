//
//  include/ADT/RelativeRange.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include "Utils/DoesOverflow.h"

struct LocationRange;
struct RelativeRange {
protected:
    uint64_t End;
public:
    constexpr
    RelativeRange(const uint64_t SizeOrEnd) noexcept : End(SizeOrEnd) {}

    RelativeRange(void *End) noexcept : End(reinterpret_cast<uint64_t>(End)) {}

    [[nodiscard]] constexpr uint64_t getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] constexpr uint64_t size() const noexcept {
        return End;
    }

    [[nodiscard]]
    bool containsLocRange(const LocationRange &LocRange) const noexcept;

    [[nodiscard]]
    constexpr bool containsLocation(const uint64_t Location) const noexcept {
        return (Location < getEnd());
    }

    [[nodiscard]] constexpr
    bool containsEndLocation(const uint64_t EndLocation) const noexcept {
        return (EndLocation <= getEnd());
    }

    [[nodiscard]] bool overlaps(const LocationRange &LocRange) const noexcept;

    [[nodiscard]]
    constexpr bool goesPastSize(const uint64_t Size) const noexcept {
        return (getEnd() > Size);
    }

    template <typename T>
    [[nodiscard]] constexpr
    bool isLargeEnoughForType(const uint64_t Count = 1) const noexcept {
        auto Size = uint64_t();
        if (DoesAddOverflow(sizeof(T), Count, &Size)) {
            return false;
        }

        return isLargeEnoughForSize(Size);
    }

    [[nodiscard]]
    constexpr bool isLargeEnoughForSize(const uint64_t Size) const noexcept {
        return (size() >= Size);
    }
};
