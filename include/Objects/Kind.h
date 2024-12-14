//
//  Objects/Kind.h
//  ktool
//
//  Created by Suhas Pai on 4/2/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

enum class ObjectKind {
    None,
    MachO,
    FatMachO,
    DyldSharedCache,
    DscImage
};
