//
//  include/TypeTraits/DisableIfNotEnumClass.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "IsEnumClass.h"

namespace TypeTraits {
    template <typename T>
    using DisableIfNotEnumClass =
        typename std::enable_if_t<IsEnumClassValue<T>>;
}
