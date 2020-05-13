//
//  include/ADT/RelativeRange.h
//  stool
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
    explicit RelativeRange(uint64_t SizeOrEnd) noexcept;

    inline uint64_t GetEnd() const noexcept { return End; }
    inline uint64_t GetSize() const noexcept { return End; }

    bool ContainsLocation(uint64_t Location) const noexcept;
    bool ContainsEndLocation(uint64_t EndLocation) const noexcept;

    bool Contains(const LocationRange &LocRange) const noexcept;
    bool Overlaps(const LocationRange &LocRange) const noexcept;

    bool GoesPastSize(uint64_t Size) const noexcept;
    LocationRange ToLocRange() const noexcept;
};
