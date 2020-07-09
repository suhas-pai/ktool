//
//  include/TypeTraits/DisableIfNotEnumClass.h
//  stool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

namespace TypeTraits {
    template <typename T>
    struct IsEnumClass {
        constexpr static auto Value =
            std::is_enum_v<T> && !std::is_convertible_v<T, int>;
    };

    template <typename Enum>
    constexpr auto IsEnumClassValue = IsEnumClass<Enum>::Value;
};
