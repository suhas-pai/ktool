//
//  src/Objects/MemoryBase.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "FatMachOMemory.h"
#include "MachOMemory.h"
#include "MemoryBase.h"

MemoryObject::MemoryObject(ObjectKind Kind) noexcept : Kind(Kind) {}
MemoryObject *MemoryObject::Open(const MemoryMap &Map) noexcept {
    const auto Kind = ObjectKind::None;
    switch (Kind) {
        case ObjectKind::None:
        case ObjectKind::MachO: {
            const auto Object = MachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }

        case ObjectKind::FatMachO: {
            const auto Object = FatMachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }
    }

    return nullptr;
}
