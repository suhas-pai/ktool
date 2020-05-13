//
//  src/ADT/RelativeRange.cpp
//  stool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LocationRange.h"
#include "RelativeRange.h"

RelativeRange::RelativeRange(uint64_t SizeOrEnd) noexcept : End(SizeOrEnd) {}

bool RelativeRange::ContainsLocation(uint64_t Location) const noexcept {
    return (Location < End);
}

bool RelativeRange::ContainsEndLocation(uint64_t EndLocation) const noexcept {
    return (EndLocation <= End);
}

bool RelativeRange::Contains(const LocationRange &LocRange) const noexcept {
    const auto DoesContain =
        (ContainsLocation(LocRange.GetBegin()) &&
         ContainsEndLocation(LocRange.GetEnd()));

    return DoesContain;
}

bool RelativeRange::Overlaps(const LocationRange &LocRange) const noexcept {
    return ContainsLocation(LocRange.GetBegin());
}

bool RelativeRange::GoesPastSize(uint64_t Size) const noexcept {
    return (End > Size);
}

LocationRange RelativeRange::ToLocRange() const noexcept {
    return LocationRange::CreateWithEnd(0, End);
}
