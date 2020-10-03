//
//  include/Utils/StringUtils.h
//  ktool
//
//  Created by Suhas Pai on 5/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

[[nodiscard]] constexpr static inline bool isDigit(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '0') < 10);
}

[[nodiscard]] constexpr static inline bool isControl(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '\0') < 32);
}

[[nodiscard]] constexpr static inline bool isSpace(char Ch) noexcept {
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

[[nodiscard]] constexpr inline bool digitIsValid(uint8_t Digit) noexcept {
    return (Digit < 10);
}

[[nodiscard]] constexpr inline uint8_t toDigit(char Ch) noexcept {
    return (Ch - '0');
}

template <typename T>
[[nodiscard]] constexpr
static inline T ParseNumber(const char *String) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type!");

    auto Iter = String;
    auto Number = T();

    for (auto Ch = *Iter; Ch != '\0'; Ch = *(++Iter)) {
        const auto Digit = toDigit(Ch);
        if (!digitIsValid(Digit)) {
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
