//
//  src/ADT/MemoryMap.cpp
//  stool
//
//  Created by Suhas Pai on 4/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/Casting.h"
#include "MemoryMap.h"

MemoryMap::MemoryMap(uint8_t *Begin, uint8_t *End) noexcept
: Begin(Begin), End(End) {}

ConstMemoryMap::ConstMemoryMap(const uint8_t *Begin,
                               const uint8_t *End) noexcept
: Begin(Begin), End(End) {}

ConstMemoryMap::ConstMemoryMap(const MemoryMap &Map) noexcept
: Begin(Map.GetBegin()), End(Map.GetEnd()) {}
