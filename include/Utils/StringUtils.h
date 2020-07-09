//
//  include/Utils/StringUtils.h
//  stool
//
//  Created by Suhas Pai on 5/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

[[nodiscard]] constexpr static inline bool IsDigit(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '0') < 10);
}

[[nodiscard]] constexpr static inline bool IsControl(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '\0') < 32);
}

[[nodiscard]] constexpr static inline bool IsSpace(char Ch) noexcept {
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

[[nodiscard]] constexpr inline bool DigitIsValid(uint8_t Digit) noexcept {
    return (Digit < 10);
}

[[nodiscard]] constexpr inline uint8_t ToDigit(char Ch) noexcept {
    return (Ch - '0');
}

template <typename T>
[[nodiscard]] constexpr
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
