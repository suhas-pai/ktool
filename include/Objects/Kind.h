//
//  include/Objects/Kind.h
//  ktool
//
//  Created by Suhas Pai on 4/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

enum class ObjectKind {
    None,
    MachO,
    FatMachO
};
