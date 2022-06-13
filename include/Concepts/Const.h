//
//  include/Concepts/Const.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

namespace Concepts {
    template <typename T>
    concept Const = std::is_const_v<T>;

    template <typename T>
    concept NotConst = !std::is_const_v<T>;
}
