//
//  ADT/LargestIntHelper.h
//  ktool
//
//  Created by Suhas Pai on 5/10/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <concepts>
#include <cstdint>

template <std::integral T = uint64_t>
struct LargestIntHelper {
protected:
    T Integer;
public:
    constexpr LargestIntHelper() noexcept = default;
    constexpr LargestIntHelper(const T &Integer) noexcept
    : Integer(Integer) {}

    constexpr auto operator=(const T &Integer) noexcept
        -> decltype(*this)
    {
        if (this->Integer < Integer) {
            this->Integer = Integer;
        }

        return *this;
    }

    [[nodiscard]] inline auto value() const noexcept {
        return Integer;
    }

    constexpr operator T() const noexcept { return Integer; }
};
