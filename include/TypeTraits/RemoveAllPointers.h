//
//  include/TypeTraits/RemoveAllPointers.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <type_traits>
#include "Identity.h"

namespace TypeTraits {
    template<typename T>
    struct RemoveAllPointers :
        std::conditional_t<std::is_pointer_v<T>,
                           RemoveAllPointers<std::remove_pointer_t<T>>,
                           TypeTraits::Identity<T>>
    {};

    template <typename T>
    using RemoveAllPointersType = typename RemoveAllPointers<T>::Type;
}
