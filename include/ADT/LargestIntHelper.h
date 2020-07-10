//
//  include/ADT/LargestIntHelper.h
//  ktool
//
//  Created by Suhas Pai on 5/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

template <typename T = uint64_t>
struct LargestIntHelper {
    static_assert(std::is_integral_v<T>, "Type must be an integer");
protected:
    T Integer;
public:
    constexpr LargestIntHelper() noexcept = default;
    constexpr inline LargestIntHelper(const T &Integer) noexcept
    : Integer(Integer) {}

    constexpr inline LargestIntHelper &operator=(const T &Integer) noexcept {
        if (this->Integer < Integer) {
            this->Integer = Integer;
        }

        return *this;
    }

    [[nodiscard]] inline T value() const noexcept {
        return Integer;
    }

    constexpr inline operator T() const noexcept { return Integer; }
};
