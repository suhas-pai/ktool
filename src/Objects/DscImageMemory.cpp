//
//  src/Objects/DscImageMemory.cpp
//  ktool
//
//  Created by Suhas Pai on 7/12/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "DscImageMemory.h"

DscImageMemoryObject::DscImageMemoryObject(
    const MemoryMap &DscMap,
    const DyldSharedCache::ImageInfo &ImageInfo,
    uint8_t *Begin,
    uint8_t *End) noexcept
: MachOMemoryObject(ObjKind, MemoryMap(Begin, End)), DscMap(DscMap),
  ImageInfo(ImageInfo) {}

ConstDscImageMemoryObject::ConstDscImageMemoryObject(
    const ConstMemoryMap &DscMap,
    const DyldSharedCache::ImageInfo &ImageInfo,
    const uint8_t *Begin,
    const uint8_t *End) noexcept
: DscImageMemoryObject(reinterpret_cast<const MemoryMap &>(DscMap),
                       ImageInfo,
                       const_cast<uint8_t *>(Begin),
                       const_cast<uint8_t *>(End)) {}
