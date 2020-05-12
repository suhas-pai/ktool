//
//  include?RemovePointersAndRefs.h
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "RemoveAllPointers.h"

namespace TypeTraits {
    template<typename T>
    struct __RemovePtrRefLoopBase :
        std::conditional_t<std::is_reference_v<T>,
                           __RemovePtrRefLoopBase<std::remove_reference_t<T>>,
                           Identity<T>>
    {};

    template<typename T, typename U = RemoveAllPointersType<T>>
    struct RemovePointersAndRefs :
        std::conditional_t<std::is_reference_v<U>,
                           __RemovePtrRefLoopBase<std::remove_reference_t<U>>,
                           Identity<U>>
    {};

    template <typename T>
    using RemovePointersAndRefsType = typename RemovePointersAndRefs<T>::Type;
}
