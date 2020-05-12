//
//  src/Objects/MemoryObject.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "FatMachOMemory.h"
#include "MachOMemory.h"
#include "MemoryBase.h"

MemoryObject::MemoryObject(ObjectKind Kind) noexcept : Kind(Kind) {}
ConstMemoryObject::ConstMemoryObject(ObjectKind Kind) noexcept : Kind(Kind) {}

MemoryObject *MemoryObject::Open(const MemoryMap &Map) noexcept {
    auto Kind = ObjectKind::None;
    switch (Kind) {
        case ObjectKind::None:
        case ObjectKind::MachO: {
            auto Object = MachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }

        case ObjectKind::FatMachO: {
            auto Object = FatMachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }
    }

    return nullptr;
}

ConstMemoryObject *ConstMemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    auto Kind = ObjectKind::None;
    switch (Kind) {
        case ObjectKind::None:
        case ObjectKind::MachO: {
            auto Object = ConstMachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }

        case ObjectKind::FatMachO: {
            auto Object = ConstFatMachOMemoryObject::Open(Map);
            if (Object.DidMatchFormat()) {
                return Object.ToPtr();
            }
        }
    }

    return nullptr;
}

bool MemoryObject::HasError() const noexcept {
    assert(0 && "MemoryObject::HasError() called");
}

bool MemoryObject::DidMatchFormat() const noexcept {
    assert(0 && "MemoryObject::DidMatchFormat() called");
}

MemoryMap MemoryObject::GetMap() const noexcept {
    assert(0 && "MemoryObject::GetMap() called");
}

ConstMemoryMap MemoryObject::GetConstMap() const noexcept {
    assert(0 && "MemoryObject::GetConstMap() called");
}

RelativeRange MemoryObject::GetRange() const noexcept {
    assert(0 && "MemoryObject::GetRange() called");
}

MemoryObject *MemoryObject::ToPtr() const noexcept {
    assert(0 && "MemoryObject::ToPtr() called");
}

bool ConstMemoryObject::HasError() const noexcept {
    assert(0 && "ConstMemoryObject::HasError() called");
}

bool ConstMemoryObject::DidMatchFormat() const noexcept {
    assert(0 && "ConstMemoryObject::DidMatchFormat() called");
}

ConstMemoryMap ConstMemoryObject::GetMap() const noexcept {
    assert(0 && "ConstMemoryObject::GetMap() called");
}

ConstMemoryMap ConstMemoryObject::GetConstMap() const noexcept {
    assert(0 && "ConstMemoryObject::GetConstMap() called");
}

RelativeRange ConstMemoryObject::GetRange() const noexcept {
    assert(0 && "MemoryObject::GetRange() called");
}

ConstMemoryObject *ConstMemoryObject::ToPtr() const noexcept {
    assert(0 && "ConstMemoryObject::ToPtr() called");
}

ConstMemoryObject::operator ConstMemoryMap() const noexcept {
    assert(0 && "ConstMemoryObject::operator ConstMemoryMap() called");
}
