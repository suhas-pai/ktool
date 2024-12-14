//
//  Objects/MemoryBase.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cassert>

#include "Objects/DscMemory.h"
#include "Objects/FatMachOMemory.h"
#include "Objects/MachOMemory.h"
#include "Objects/MemoryBase.h"

MemoryObject::MemoryObject(const ObjectKind Kind) noexcept : Kind(Kind) {
    assert(Kind != ObjectKind::None);
}

auto MemoryObject::Open(const MemoryMap &Map) noexcept
    -> MemoryObjectOrError
{
    const auto Kind = ObjectKind::None;
    switch (Kind) {
        case ObjectKind::None:
        case ObjectKind::MachO: {
            const auto ObjectOrError = MachOMemoryObject::Open(Map);
            const auto Error = ObjectOrError.getError();

            if (MachOMemoryObject::errorDidMatchFormat(Error)) {
                if (ObjectOrError.hasError()) {
                    auto Result =
                        MemoryObjectOrError(ObjectKind::MachO,
                                            static_cast<uint8_t>(Error));

                    return Result;
                }

                return ObjectOrError.value();
            }
        }
        case ObjectKind::FatMachO: {
            const auto ObjectOrError = FatMachOMemoryObject::Open(Map);
            const auto Error = ObjectOrError.getError();

            if (FatMachOMemoryObject::errorDidMatchFormat(Error)) {
                if (ObjectOrError.hasError()) {
                    auto Result =
                        MemoryObjectOrError(ObjectKind::FatMachO,
                                            static_cast<uint8_t>(Error));

                    return Result;
                }

                return ObjectOrError.value();
            }
        }
        case ObjectKind::DyldSharedCache: {
            const auto ObjectOrError = DscMemoryObject::Open(Map);
            const auto Error = ObjectOrError.getError();

            if (DscMemoryObject::errorDidMatchFormat(Error)) {
                if (ObjectOrError.hasError()) {
                    auto Result =
                        MemoryObjectOrError(ObjectKind::DyldSharedCache,
                                            static_cast<uint8_t>(Error));

                    return Result;
                }

                return ObjectOrError.value();
            }
        }
    }

    return MemoryObjectOrError(ObjectKind::None, -1);
}
