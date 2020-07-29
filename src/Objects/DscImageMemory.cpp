//
//  src/Objects/DscImageMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 7/12/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "DscImageMemory.h"

ConstDscImageMemoryObject::ConstDscImageMemoryObject(
    const ConstMemoryMap &DscMap,
    const DyldSharedCache::ImageInfo &ImageInfo,
    const uint8_t *Begin,
    const uint8_t *End) noexcept
: ConstMachOMemoryObject(ObjKind, ConstMemoryMap(Begin, End)), DscMap(DscMap),
ImageInfo(ImageInfo) {}

DscImageMemoryObject::DscImageMemoryObject(
    const MemoryMap &DscMap,
    const DyldSharedCache::ImageInfo &ImageInfo,
    uint8_t *Begin,
    uint8_t *End) noexcept
: ConstDscImageMemoryObject(DscMap, ImageInfo, Begin, End) {}

