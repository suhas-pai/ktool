//
//  src/ADT/RelativeRange.cpp
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LocationRange.h"
#include "RelativeRange.h"

bool
RelativeRange::containsRange(const LocationRange &LocRange) const noexcept {
    const auto DoesContain =
        (containsLocation(LocRange.getBegin()) &&
         containsEndLocation(LocRange.getEnd()));

    return DoesContain;
}

bool RelativeRange::overlaps(const LocationRange &LocRange) const noexcept {
    return containsLocation(LocRange.getBegin());
}
