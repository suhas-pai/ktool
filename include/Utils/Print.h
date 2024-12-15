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

#include "ADT/Range.h"

namespace Utils {
    template <std::unsigned_integral T>
    [[nodiscard]]
    constexpr auto GetIntegerDigitCount(T Integer) noexcept -> uint8_t {
        auto DigitLength = uint8_t();
        do {
            DigitLength++;
        } while (Integer /= 10);

        return DigitLength;
    }

    template <std::signed_integral T>
    [[nodiscard]]
    constexpr auto GetIntegerDigitCount(T Integer) noexcept -> uint8_t {
        if (Integer < 0) {
            Integer = -Integer;
            return GetIntegerDigitCount(Integer) + 1;
        }

        return GetIntegerDigitCount(
            static_cast<std::make_unsigned_t<T>>(Integer));
    }

    template <std::unsigned_integral T>
    [[nodiscard]] inline auto GetFormattedNumber(const T Number) noexcept {
        char buffer[20] = {};
        std::to_chars(buffer, buffer + sizeof(buffer), Number);

        auto String = std::string(buffer);
        if (String.length() < 4) {
            return String;
        }

        for (auto I = int64_t(String.length() - 3); I > 0; I -= 3) {
            String.insert(static_cast<uint64_t>(I), 1, ',');
        }

        return String;
    }

    auto PadSpaces(FILE *OutFile, uint32_t SpaceAmount) noexcept -> int;
    auto RightPadSpaces(FILE *OutFile, int WrittenOut, int RightPad) -> int;

    auto
    PrintMultTimes(FILE *OutFile, const char *String, uint64_t Times) -> int;

    auto FormattedSize(uint64_t Size) -> std::string;
    auto FormattedSizeForOutput(uint64_t Size) -> std::string;

    auto
    PrintAddress(FILE *OutFile,
                 uint64_t Address,
                 bool Is64Bit,
                 std::string_view Prefix = "",
                 std::string_view Suffix = "") noexcept -> int;

    auto
    PrintOffsetSizeRange(FILE *OutFile,
                         const ADT::Range &Range,
                         bool Is64Bit,
                         bool IsSize64Bit,
                         std::string_view Prefix = "",
                         std::string_view Suffix = "") noexcept -> int;

    auto
    PrintOffsetSizeInfo(FILE *OutFile,
                        const ADT::Range &Range,
                        bool Is64Bit,
                        bool IsSize64Bit,
                        bool IsOutOfBounds,
                        std::string_view OffsetKey,
                        std::string_view SizeKey,
                        std::string_view Prefix = "",
                        std::string_view Suffix = "") noexcept -> int;

    auto
    PrintSegmentSectionPair(FILE *OutFile,
                            std::string_view Segment,
                            std::string_view Section,
                            bool PadSegments,
                            bool PadSections,
                            std::string_view Prefix = "",
                            std::string_view Suffix = "") noexcept -> int;

    auto
    PrintDylibOrdinalPath(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          std::string_view DylibPath,
                          bool PrintPath,
                          bool IsOutOfBounds,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept -> int;

    auto
    PrintDylibOrdinalInfo(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          std::string_view DylibPath,
                          bool PrintPath,
                          bool IsOutOfBounds,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept -> int;

    auto
    PrintOnlyOnce(FILE *OutFile,
                  const char *String,
                  bool &DidPrint) noexcept -> int;

    auto
    PrintOnlyAfterFirst(FILE *OutFile,
                        const char *String,
                        bool &DidPassFirst) noexcept -> int;

    // 32 for Segment+Section Name Max Length, 4 for apostrophes, 1 for comma
    constexpr static inline auto SegmentSectionPairMaxLen =
        uint32_t(32 + 4 + 1);

    // 2 for '0x', 8 for digits
    constexpr static inline auto Address32Length = uint32_t(2 + 8);

    // 2 for '0x', 16 for digits
    constexpr static inline auto Address64Length = uint32_t(2 + 16);
}

#define CHAR_ARR_FMT(Size) "%." TO_STRING(Size) "s"
#define CHAR_ARR_FMT_ARGS(val) val

#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARGS(SV) static_cast<int>((SV).length()), (SV).data()

#define ZEROPAD_FMT_C(C) "0" TO_STRING(C)
#define LEFTPAD_FMT_C(C) TO_STRING(C)
#define RIGHTPAD_FMT_C(C) "-" TO_STRING(C)
#define STRING_VIEW_FMT_C(C) "%." TO_STRING(C) "s"

#define ZEROPAD_FMT "0*"
#define LEFTPAD_FMT "*"
#define RIGHTPAD_FMT "-*"

#define PAD_FMT_ARGS(val) val
#define STR_LENGTH(s) (sizeof(s) - 1)

#define ADDRESS_32_FMT "0x%08" PRIX32
#define ADDRESS_64_FMT "0x%016" PRIX64

#define ADDR_RANGE_32_FMT ADDRESS_32_FMT " - " ADDRESS_32_FMT "%s"
#define ADDR_RANGE_64_FMT ADDRESS_64_FMT " - " ADDRESS_64_FMT "%s"
#define ADDR_RANGE_32_64_FMT ADDRESS_32_FMT " - " ADDRESS_64_FMT "%s"

#define ADDR_RANGE_FMT_ARGS(BEGIN, END) \
    BEGIN, END, ((END) < (BEGIN)) ? " (Overflows)" : ""

#define UUID_FMT "%.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-" \
                 "%.2X%.2x%.2x%.2x%.2X%.2X"

#define UUID_FMT_ARGS(UUID) \
    UUID[0], \
    UUID[1], \
    UUID[2], \
    UUID[3], \
    UUID[4], \
    UUID[5], \
    UUID[6], \
    UUID[7], \
    UUID[8], \
    UUID[9], \
    UUID[10], \
    UUID[11], \
    UUID[12], \
    UUID[13], \
    UUID[14], \
    UUID[15]
