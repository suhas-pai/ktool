//
//  include/Utils/IntegerLimit.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

using IntegerLimitDefaultType = void;

template <typename ValueType>
using IntegerLimitRealValueType =
        typename std::conditional<
            std::is_same_v<ValueType, IntegerLimitDefaultType>,
            uint64_t,
            ValueType>::type;

template <typename ValueType>
using IntegerLimitUseType =
        typename std::conditional<
            sizeof(IntegerLimitRealValueType<ValueType>) <= sizeof(ValueType),
            IntegerLimitRealValueType<ValueType>,
            ValueType>::type;
