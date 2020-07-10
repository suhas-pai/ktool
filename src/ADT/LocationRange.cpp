//
//  src/ADT/LocationRange.cpp
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LocationRange.h"
#include "RelativeRange.h"

RelativeRange LocationRange::ToRelativeRange() const noexcept {
    return RelativeRange(size());
}
