//
//  include/ADT/LocationRange.h
//  stool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <limits>
#include <optional>

#include "Utils/DoesOverflow.h"

struct RelativeRange;
struct LocationRange {
    friend class std::optional<LocationRange>;
private:
    uint64_t Begin;
    uint64_t End;

    explicit LocationRange(uint64_t Begin, uint64_t End) noexcept;
public:
    explicit LocationRange() = default;
    
    template <typename T = uint64_t>
    static std::optional<LocationRange>
    CreateWithSize(uint64_t Begin, uint64_t Size) noexcept {
        auto End = T();
        if (DoesAddOverflow(Begin, Size, &End)) {
            return std::optional<LocationRange>();
        }

        return LocationRange(Begin, End);
    }

    template <typename T = uint64_t>
    static bool IsSizeRangeValid(uint64_t Begin, uint64_t Size) noexcept {
        return DoesAddOverflow<T>(Begin, Size);
    }

    static inline
    bool Contains(const LocationRange &Lhs, const LocationRange &Rhs) {
        return Lhs.Contains(Rhs);
    }

    static inline
    bool Overlaps(const LocationRange &Lhs, const LocationRange &Rhs) {
        return Lhs.Overlaps(Rhs);
    }

    static LocationRange
    CreateWithEnd(uint64_t Begin, uint64_t End) noexcept;

    uint64_t GetBegin() const noexcept;
    uint64_t GetEnd() const noexcept;
    uint64_t GetSize() const noexcept;

    bool ContainsLocation(uint64_t Location) const noexcept;
    bool ContainsRelativeLocation(uint64_t Location) const noexcept;
    bool ContainsEndLocation(uint64_t EndLocation) const noexcept;

    bool Contains(const LocationRange &LocRange) const noexcept;
    bool Overlaps(const LocationRange &LocRange) const noexcept;

    uint64_t GetSizeFromLocation(uint64_t Location) const noexcept;
    uint64_t GetSizeFromRelativeLocation(uint64_t Location) const noexcept;

    bool GoesPastSize(uint64_t Size) const noexcept;
    RelativeRange ToRelativeRange() const noexcept;
};

