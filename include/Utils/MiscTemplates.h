//
//  include/Utils/MiscTemplates.h
//  stool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

template <typename T, typename U>
inline bool IndexOutOfBounds(const T &Index, const U &Bounds) noexcept {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>,
                  "Types must be integer-types");

    return (Index >= Bounds);
}

inline bool DigitIsValid(uint8_t Digit) noexcept {
    return (Digit < 10);
}

inline uint8_t ToDigit(char Ch) noexcept {
    return (Ch - '0');
}

template <typename T>
static inline T ParseNumber(const char *String) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type!");

    auto Number = T();
    for (auto Ch = *String; Ch != '\0'; Ch = *(++String)) {
        const auto Digit = ToDigit(Ch);
        if (!DigitIsValid(Digit)) {
            fprintf(stdout, "%s is not a valid number\n", String);
            exit(1);
        }

        if (DoesMultiplyAndAddOverflow(Number, 10, Digit, &Number)) {
            fprintf(stdout, "Number %s is too large\n", String);
            exit(1);
        }
    }

    return Number;
}
