//
//  Objects/DscImageMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 7/12/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "Objects/DscImageMemory.h"

DscImageMemoryObject::DscImageMemoryObject(
    const MemoryMap &DscMap,
    const DyldSharedCache::ImageInfo &ImageInfo,
    uint8_t *const Begin,
    uint8_t *const End) noexcept
: MachOMemoryObject(ObjKind, MemoryMap(Begin, End)), DscMap(DscMap),
  ImageInfo(ImageInfo) {}
