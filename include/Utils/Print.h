//
//  Utils/Print.h
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
#include <string_view>

namespace Utils {
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

    auto FormattedSizeForOutput(uint64_t Size) -> std::string;

    auto
    PrintAddress(FILE *OutFile,
                 uint64_t Address,
                 bool Is64Bit,
                 std::string_view Prefix = "",
                 std::string_view Suffix = "") noexcept -> int;

    auto
    PrintSegmentSectionPair(FILE *OutFile,
                            std::string_view Segment,
                            std::string_view Section,
                            bool PadSegments,
                            bool PadSections,
                            const char *Prefix = "",
                            const char *Suffix = "") noexcept -> int;

    auto
    PrintDylibOrdinalInfo(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          std::string_view DylibPath,
                          bool PrintPath,
                          bool IsOutOfBounds,
                          const char *Prefix = "",
                          const char *Suffix = "") noexcept -> int;
}

#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARGS(SV) static_cast<int>((SV).length()), (SV).data()

#define ZEROPAD_FMT "0*"
#define ZEROPAD_FMT_ARGS(val) val
#define RIGHTPAD_FMT "-*"
#define RIGHTPAD_FMT_ARGS(val) val
#define STR_LENGTH(s) (sizeof(s) - 1)

#define ADDRESS_32_FMT "0x%08" PRIX32
#define ADDRESS_64_FMT "0x%016" PRIX64

#define ADDR_RANGE_32_FMT ADDRESS_32_FMT " - " ADDRESS_32_FMT "%s"
#define ADDR_RANGE_64_FMT ADDRESS_64_FMT " - " ADDRESS_64_FMT "%s"
#define ADDR_RANGE_32_64_FMT ADDRESS_32_FMT " - " ADDRESS_64_FMT "%s"

#define ADDR_RANGE_FMT_ARGS(BEGIN, END) \
    BEGIN, END, ((END) < (BEGIN)) ? " (Overflows)" : ""

