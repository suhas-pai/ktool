//
//  src/ADT/LocationRange.cpp
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LocationRange.h"
#include "RelativeRange.h"

RelativeRange LocationRange::toRelativeRange() const noexcept {
    return RelativeRange(this->size());
}
