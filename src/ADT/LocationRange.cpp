//
//  src/ADT/LocationRange.cpp
//  stool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cassert>

#include "Utils/DoesOverflow.h"
#include "LocationRange.h"
#include "RelativeRange.h"

LocationRange::LocationRange(uint64_t Begin, uint64_t End) noexcept
: Begin(Begin), End(End) {}

LocationRange
LocationRange::CreateWithEnd(uint64_t Begin, uint64_t End) noexcept {
    assert(Begin <= End &&
           "LocationRange::CreateWithEnd(): End > Begin");
    
    return LocationRange(Begin, End);
}

uint64_t LocationRange::GetBegin() const noexcept {
    return Begin;
}

uint64_t LocationRange::GetEnd() const noexcept {
    return End;
}

uint64_t LocationRange::GetSize() const noexcept {
    return (End - Begin);
}

bool LocationRange::ContainsLocation(uint64_t Location) const noexcept {
    return (Location >= Begin && Location < End);
}

bool LocationRange::ContainsRelativeLocation(uint64_t Location) const noexcept {
    return (Location < GetSize());
}

uint64_t LocationRange::GetSizeFromLocation(uint64_t Location) const noexcept {
    return GetSizeFromRelativeLocation(Location - Begin);
}

uint64_t
LocationRange::GetSizeFromRelativeLocation(uint64_t Location) const noexcept {
    return (End - Location);
}

bool
LocationRange::ContainsEndLocation(uint64_t EndLocation) const noexcept {
    return (EndLocation > Begin && EndLocation <= End);
}

bool LocationRange::Contains(const LocationRange &LocRange) const noexcept {
    const auto DoesContain =
        ContainsLocation(LocRange.Begin) && ContainsEndLocation(LocRange.End);

    return DoesContain;
}

bool LocationRange::Overlaps(const LocationRange &LocRange) const noexcept {
    if (ContainsLocation(LocRange.Begin) || ContainsEndLocation(LocRange.End)) {
        return true;
    }

    return LocRange.Contains(*this);
}

bool LocationRange::GoesPastSize(uint64_t Size) const noexcept {
    return (End > Size);
}

RelativeRange LocationRange::ToRelativeRange() const noexcept {
    return RelativeRange(GetSize());
}
