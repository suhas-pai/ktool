//
//  Concepts/Const.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

namespace Concepts {
    template <typename T>
    concept NotConst = !std::is_const_v<T>;
}
