//
//  Concepts/DisableIfNotEnumClass.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

namespace Concepts {
    template <typename T>
    concept EnumClass = std::is_enum_v<T> && !std::is_convertible_v<T, int>;
};
