//
//  src/Objects/MemoryBase.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "DscMemory.h"
#include "FatMachOMemory.h"
#include "MachOMemory.h"
#include "MemoryBase.h"

MemoryObject::MemoryObject(ObjectKind Kind) noexcept : Kind(Kind) {}
MemoryObject *MemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    const auto Kind = ObjectKind::None;
    switch (Kind) {
        case ObjectKind::None:
        case ObjectKind::MachO: {
            const auto Object = ConstMachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }

        case ObjectKind::FatMachO: {
            const auto Object = ConstFatMachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }

        case ObjectKind::DyldSharedCache: {
            const auto Object = ConstDscMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }
    }

    return nullptr;
}
