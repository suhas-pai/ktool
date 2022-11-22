//
//  ADT/PrintUtils.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include <charconv>
#include <concepts>
#include <cstdio>
#include <inttypes.h>
#include <string>

namespace ADT {
    template <std::integral T>
    [[nodiscard]] constexpr auto GetIntegerDigitCount(T Integer) noexcept {
        auto DigitLength = int();
        do {
            DigitLength++;
        } while (Integer /= 10);

        return DigitLength;
    }

    auto PadSpaces(FILE *OutFile, int SpaceAmount) noexcept -> int;
    auto RightPadSpaces(FILE *OutFile, int WrittenOut, int RightPad) -> int;
    auto FormattedSize(uint64_t Size) -> std::string;
}

#define STRING_VIEW_FMT "%*s"
#define STRING_VIEW_FMT_ARGS(SV) static_cast<int>((SV).length()), (SV).data()

#define RIGHTPAD_FMT "-*"
#define STR_LENGTH(s) (sizeof(s) - 1)

#define ADDRESS_32_FMT "0x%08" PRIX32
#define ADDRESS_64_FMT "0x%016" PRIX64

#define ADDR_RANGE_32_FMT ADDRESS_32_FMT " - " ADDRESS_32_FMT
#define ADDR_RANGE_64_FMT ADDRESS_64_FMT " - " ADDRESS_64_FMT

#define ADDR_RANGE_FMT_ARGS(BEGIN, END) BEGIN, END
