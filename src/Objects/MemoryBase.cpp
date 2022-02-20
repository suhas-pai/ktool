//
//  src/Objects/MemoryBase.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cassert>

#include "DscMemory.h"
#include "FatMachOMemory.h"
#include "MachOMemory.h"
#include "MemoryBase.h"

MemoryObject::MemoryObject(const ObjectKind Kind) noexcept : Kind(Kind) {
    assert(Kind != ObjectKind::None);
}

MemoryObjectOrError MemoryObject::Open(const ConstMemoryMap &Map) noexcept {
    const auto Kind = ObjectKind::None;
    switch (Kind) {
        case ObjectKind::None:
        case ObjectKind::MachO: {
            const auto ObjectOrError = ConstMachOMemoryObject::Open(Map);
            const auto Error = ObjectOrError.getError();

            if (ConstMachOMemoryObject::errorDidMatchFormat(Error)) {
                if (ObjectOrError.hasError()) {
                    const auto Result =
                        MemoryObjectOrError(ObjectKind::MachO,
                                            static_cast<uint8_t>(Error));

                    return Result;
                }

                return ObjectOrError.getPtr();
            }
        }

        case ObjectKind::FatMachO: {
            const auto ObjectOrError = ConstFatMachOMemoryObject::Open(Map);
            const auto Error = ObjectOrError.getError();

            if (ConstFatMachOMemoryObject::errorDidMatchFormat(Error)) {
                if (ObjectOrError.hasError()) {
                    const auto Result =
                        MemoryObjectOrError(ObjectKind::FatMachO,
                                            static_cast<uint8_t>(Error));

                    return Result;
                }

                return ObjectOrError.getPtr();
            }
        }

        case ObjectKind::DyldSharedCache: {
            const auto ObjectOrError = ConstDscMemoryObject::Open(Map);
            const auto Error = ObjectOrError.getError();

            if (ConstDscMemoryObject::errorDidMatchFormat(Error)) {
                if (ObjectOrError.hasError()) {
                    const auto Result =
                        MemoryObjectOrError(ObjectKind::DyldSharedCache,
                                            static_cast<uint8_t>(Error));

                    return Result;
                }

                return ObjectOrError.getPtr();
            }
        }
    }

    return nullptr;
}
