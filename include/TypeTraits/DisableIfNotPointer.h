//
//  include/TypeTraits/DisableIfNotPointer.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

namespace TypeTraits {
    template <typename T>
    using DisableIfNotPointer = typename std::enable_if_t<std::is_pointer_v<T>>;
}
