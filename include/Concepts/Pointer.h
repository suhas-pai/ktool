//
//  include/Concepts/Pointer.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

namespace Concepts {
    template <typename T>
    concept Pointer = std::is_pointer_v<T>;
}
