//
//  src/ADT/MemoryMap.cpp
//  ktool
//
//  Created by Suhas Pai on 4/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/Casting.h"
#include "MemoryMap.h"

ConstMemoryMap::ConstMemoryMap(const uint8_t *Begin,
                               const uint8_t *End) noexcept
: Begin(Begin), End(End) {}

MemoryMap::MemoryMap(uint8_t *Begin, uint8_t *End) noexcept
: ConstMemoryMap(Begin, End) {}
