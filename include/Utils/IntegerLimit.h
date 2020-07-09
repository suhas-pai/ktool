//
//  include/Utils/IntegerLimit.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

using IntegerLimitDefaultType = void;

template <typename ValueType, typename MaxValueType = uint64_t>
using IntegerLimitRealValueType =
        typename std::conditional<
            std::is_same_v<ValueType, IntegerLimitDefaultType>,
            MaxValueType,
            ValueType>::type;
