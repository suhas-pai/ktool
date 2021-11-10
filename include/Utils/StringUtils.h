//
//  include/Utils/StringUtils.h
//  ktool
//
//  Created by Suhas Pai on 5/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <type_traits>

[[nodiscard]] constexpr bool IsDigit(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '0') < 10);
}

[[nodiscard]] constexpr bool IsControl(char Ch) noexcept {
    return (static_cast<unsigned>(Ch) < 32);
}

[[nodiscard]] constexpr bool IsSpace(char Ch) noexcept {
    switch (Ch) {
        case ' ':
        case '\n':
        case '\t':
        case '\v':
        case '\f':
        case '\r':
            return true;
    }

    return false;
}

[[nodiscard]] constexpr bool DigitIsValid(uint8_t Digit) noexcept {
    return (Digit < 10);
}

[[nodiscard]] constexpr uint8_t ChToDigit(char Ch) noexcept {
    return (Ch - '0');
}

template <typename T>
[[nodiscard]] constexpr T ParseNumber(const char *String) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type!");

    auto Iter = String;
    auto Number = T();

    for (auto Ch = *Iter; Ch != '\0'; Ch = *(++Iter)) {
        const auto Digit = ChToDigit(Ch);
        if (!DigitIsValid(Digit)) {
            fprintf(stdout, "\"%s\" is not a valid number\n", String);
            exit(1);
        }

        if (DoesMultiplyAndAddOverflow(Number, 10, Digit, &Number)) {
            fprintf(stdout, "Number \"%s\" is too large\n", String);
            exit(1);
        }
    }

    return Number;
}
