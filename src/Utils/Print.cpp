//
//  Utils/Print.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <array>
#include <assert.h>
#include <cstdio>
#include <limits>
#include <sstream>
#include <string>

#include "Utils/Print.h"

namespace Utils {
    [[nodiscard]] static inline
    auto ToStringWithPrecision(const auto &Value, const int n = 3) noexcept {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << Value;
        return out.str();
    }

    constexpr static std::array<std::string_view, 9> FormatSizeNames = {
        "KiB",
        "MiB",
        "GiB",
        "TiB",
        "PiB",
        "EiB",
        "ZiB",
        "YiB",
        "BiB"
    };

    auto FormattedSize(const uint64_t Size) -> std::string {
        constexpr auto Base = 1024;
        auto ResultAmount = double(Size);

        if (ResultAmount < Base) {
            auto Result = std::string();

            Result.reserve(STR_LENGTH("1023") + STR_LENGTH(" bytes"));
            Result.append(ToStringWithPrecision(ResultAmount));
            Result.append(" Bytes");

            return Result;
        }

        auto Index = uint32_t();

        ResultAmount /= Base;
        while (ResultAmount >= Base) {
            ResultAmount /= Base;
            Index++;
        };

        assert(Index < FormatSizeNames.size());

        const auto &Name = FormatSizeNames[Index];
        auto Result = std::string();

        Result.reserve(STR_LENGTH("1023.999 ") + 1 + Name.length());
        Result.append(ToStringWithPrecision(ResultAmount));
        Result.append(1, ' ');
        Result.append(Name);

        return Result;
    }

    auto FormattedSizeForOutput(const uint64_t Size) -> std::string {
        const auto SizeString = std::to_string(Size);
        if (Size < 1024) {
            return SizeString;
        }

        auto Formatted = FormattedSize(Size);
        const auto ResultLength =
            Formatted.length() + SizeString.length() + STR_LENGTH(" ()");

        Formatted.reserve(ResultLength);
        Formatted.insert(0, SizeString);
        Formatted.insert(SizeString.length(), " (");
        Formatted.append(")");

        return Formatted;
    }

    auto
    PadSpaces(FILE *const OutFile, const int SpaceAmount) noexcept -> int {
        assert(SpaceAmount >= 0);
        return fprintf(OutFile,
                       "%" RIGHTPAD_FMT "s",
                       RIGHTPAD_FMT_ARGS(SpaceAmount),
                       "");
    }

    auto
    RightPadSpaces(FILE *const OutFile,
                   const int WrittenOut,
                   const int Total) -> int
    {
        assert(WrittenOut >= 0);
        assert(WrittenOut <= Total);

        return PadSpaces(OutFile, Total - WrittenOut);
    }

    auto
    PrintAddress(FILE *const OutFile,
                 const uint64_t Address,
                 const bool Is64Bit,
                 const std::string_view Prefix,
                 const std::string_view Suffix) noexcept -> int
    {
        auto Result = int();
        if (Is64Bit) {
            Result =
                 fprintf(OutFile,
                         STRING_VIEW_FMT ADDRESS_64_FMT STRING_VIEW_FMT,
                         STRING_VIEW_FMT_ARGS(Prefix),
                         Address,
                         STRING_VIEW_FMT_ARGS(Suffix));
        } else {
            assert(Address <= std::numeric_limits<uint32_t>::max());
            Result =
                 fprintf(OutFile,
                         STRING_VIEW_FMT ADDRESS_32_FMT STRING_VIEW_FMT,
                         STRING_VIEW_FMT_ARGS(Prefix),
                         static_cast<uint32_t>(Address),
                         STRING_VIEW_FMT_ARGS(Suffix));
        }

        return Result;
    }

    auto
    PrintSegmentSectionPair(FILE *const OutFile,
                            const std::string_view Segment,
                            const std::string_view Section,
                            const bool PadSegments,
                            const bool PadSections,
                            const char *const Prefix,
                            const char *const Suffix) noexcept -> int
    {
        auto WrittenOut = fprintf(OutFile, "%s", Prefix);
        if (PadSegments) {
            if (Segment.length() < 16) {
                WrittenOut +=
                    PadSpaces(OutFile, static_cast<int>(16 - Segment.length()));
            }
        }

        WrittenOut +=
            fprintf(OutFile,
                    "\"" STRING_VIEW_FMT "\",",
                    STRING_VIEW_FMT_ARGS(Segment));

        WrittenOut +=
            fprintf(OutFile,
                    "\"" STRING_VIEW_FMT "\"",
                    STRING_VIEW_FMT_ARGS(Section));

        if (PadSections) {
            if (Section.length() < 16) {
                WrittenOut +=
                    PadSpaces(OutFile, static_cast<int>(16 - Section.length()));
            }
        }

        return WrittenOut + fprintf(OutFile, "%s", Suffix);
    }

    auto
    PrintDylibOrdinalInfo(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const std::string_view DylibPath,
                          const bool PrintPath,
                          const bool IsOutOfBounds,
                          const char *const Prefix,
                          const char *const Suffix) noexcept -> int
    {
        auto WrittenOut =
            fprintf(OutFile,
                    "%sDylib-Ordinal %02" PRId8,
                    Prefix,
                    DylibOrdinal);

        if (IsOutOfBounds) {
            return WrittenOut + fprintf(OutFile, " (Out Of Bounds!)%s", Suffix);
        }

        if (PrintPath) {
            WrittenOut +=
                fprintf(OutFile,
                        " - \"" STRING_VIEW_FMT "\"%s",
                        STRING_VIEW_FMT_ARGS(DylibPath),
                        Suffix);
        } else {
            WrittenOut += fprintf(OutFile, "%s", Suffix);
        }

        return WrittenOut;
    }
}
