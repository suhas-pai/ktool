//
//  include/Operations/Common.h
//  stool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <string_view>

#include "ADT/Mach-O/LoadCommandStorage.h"
#include "Objects/MachOMemory.h"

struct OperationCommon {
    static MachO::LoadCommandStorage
    GetLoadCommandStorage(MachOMemoryObject &Object) noexcept;

    static MachO::ConstLoadCommandStorage
    GetConstLoadCommandStorage(const ConstMachOMemoryObject &Object) noexcept;

    static const std::string_view &
    GetLoadCommandStringValue(
        const MachO::LoadCommandString::GetValueResult &) noexcept;
};
