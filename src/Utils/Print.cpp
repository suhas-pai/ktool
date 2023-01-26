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

#include "MachO/LoadCommands.h"
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
        if (Size < Base) {
            auto Result = std::string();

            Result.reserve(STR_LENGTH("1023") + STR_LENGTH(" bytes"));
            Result.append(std::to_string(Size));
            Result.append(" Bytes");

            return Result;
        }

        auto Index = uint32_t();
        auto ResultAmount = double(Size);

        ResultAmount /= Base;
        while (ResultAmount >= Base) {
            ResultAmount /= Base;
            Index++;
        };

        assert(Index < FormatSizeNames.size());

        const auto &Name = FormatSizeNames[Index];
        auto Result = std::string();

        if (static_cast<uint64_t>(ResultAmount) == ResultAmount) {
            Result.reserve(STR_LENGTH("1023.999 ") + 1 + Name.length());
            Result.append(std::to_string(static_cast<uint64_t>(ResultAmount)));
        } else {
            Result.reserve(STR_LENGTH("1023 ") + 1 + Name.length());
            Result.append(ToStringWithPrecision(ResultAmount));
        }

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
    PrintMultTimes(FILE *const OutFile,
                   const char *const String,
                   const uint64_t Times) -> int
    {
        auto Result = int();
        for (auto I = uint64_t(); I != Times; I++) {
            if (const auto Tmp = fputs(String, OutFile); Tmp != EOF) {
                Result += Tmp;
                continue;
            }

            return EOF;
        }

        return Result;
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
                            const std::string_view Prefix,
                            const std::string_view Suffix) noexcept -> int
    {
        auto WrittenOut = fprintf(OutFile, "%s", Prefix.data());
        if (PadSegments) {
            const auto SegmentLength =
                Segment.empty() ? Segment.length() : STR_LENGTH("<null>");

            if (SegmentLength < MachO::SegmentMaxNameLength) {
                WrittenOut +=
                    PadSpaces(
                        OutFile,
                        static_cast<int>(
                            MachO::SegmentMaxNameLength - SegmentLength));
            }
        }

        const auto SegmentName =
            !Segment.empty() ? Segment : "<null>";
        const auto SectionName =
            !Section.empty() ? Section : "<null>";

        if (Segment.empty()) {
            WrittenOut += fputs("<null>,", OutFile);
        } else {
            WrittenOut +=
                fprintf(OutFile,
                        "\"" STRING_VIEW_FMT "\",",
                        STRING_VIEW_FMT_ARGS(SegmentName));
        }

        if (Section.empty()) {
            WrittenOut += fputs("<null>", OutFile);
        } else {
            WrittenOut +=
                fprintf(OutFile,
                        "\"" STRING_VIEW_FMT "\"",
                        STRING_VIEW_FMT_ARGS(SectionName));
        }

        if (PadSections) {
            const auto SectionLength =
                !Section.empty() ? Section.length() : STR_LENGTH("<null>");

            if (SectionLength < MachO::SegmentSectionMaxNameLength) {
                const auto PadLength =
                    static_cast<int>(
                        MachO::SegmentSectionMaxNameLength -SectionLength);

                WrittenOut += PadSpaces(OutFile, PadLength);
            }
        }

        return WrittenOut + fprintf(OutFile, "%s", Suffix.data());
    }

    auto
    PrintDylibOrdinalPath(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const std::string_view DylibPath,
                          const bool PrintPath,
                          const bool IsOutOfBounds,
                          const std::string_view Prefix,
                          const std::string_view Suffix) noexcept -> int
    {
        if (DylibOrdinal == 0) {
            return
                fprintf(stderr, "%s<invalid>%s", Prefix.data(), Suffix.data());
        }

        if (IsOutOfBounds) {
            return
                fprintf(OutFile,
                        "%s<out-of-bounds!>%s",
                        Prefix.data(),
                        Suffix.data());
        }

        auto WrittenOut = int();
        if (PrintPath) {
            WrittenOut +=
                fprintf(OutFile,
                        "\"" STRING_VIEW_FMT "\"%s",
                        STRING_VIEW_FMT_ARGS(DylibPath),
                        Suffix.data());
        } else {
            WrittenOut += fprintf(OutFile, "%s", Suffix.data());
        }

        return WrittenOut;
    }

    auto
    PrintDylibOrdinalInfo(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const std::string_view DylibPath,
                          const bool PrintPath,
                          const bool IsOutOfBounds,
                          const std::string_view Prefix,
                          const std::string_view Suffix) noexcept -> int
    {
        auto WrittenOut =
            fprintf(OutFile,
                    "%sDylib-Ordinal %02" PRId8,
                    Prefix.data(),
                    DylibOrdinal);

        if (IsOutOfBounds) {
            return
                WrittenOut +
                fprintf(OutFile, " (Out Of Bounds!)%s", Suffix.data());
        }

        if (PrintPath) {
            WrittenOut +=
                fprintf(OutFile,
                        " - \"" STRING_VIEW_FMT "\"%s",
                        STRING_VIEW_FMT_ARGS(DylibPath),
                        Suffix.data());
        } else {
            WrittenOut += fprintf(OutFile, "%s", Suffix.data());
        }

        return WrittenOut;
    }
}
