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
    PadSpaces(FILE *const OutFile, const uint32_t SpaceAmount) noexcept -> int {
        return fprintf(OutFile,
                       "%" RIGHTPAD_FMT "s",
                       RIGHTPAD_FMT_ARGS(static_cast<int>(SpaceAmount)),
                       "");
    }

    auto
    RightPadSpaces(FILE *const OutFile,
                   const int WrittenOut,
                   const int Total) -> int
    {
        assert(WrittenOut >= 0);
        assert(WrittenOut <= Total);

        return PadSpaces(OutFile, static_cast<uint32_t>(Total - WrittenOut));
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
    PrintOffsetSizeRange(FILE *const OutFile,
                         const ADT::Range &Range,
                         const bool Is64Bit,
                         const bool SizeIs64Bit,
                         const std::string_view Prefix,
                         const std::string_view Suffix) noexcept -> int
    {
        const auto Begin = Range.begin();
        const auto End = Range.end().value();

        auto WrittenOut = int();
        if (Begin != 0) {
            if (Is64Bit) {
                WrittenOut +=
                    fprintf(OutFile,
                            STRING_VIEW_FMT ADDRESS_64_FMT "-",
                            STRING_VIEW_FMT_ARGS(Prefix),
                            Begin);
            } else {
                assert(Begin <= std::numeric_limits<uint32_t>::max());
                WrittenOut +=
                    fprintf(OutFile,
                            STRING_VIEW_FMT ADDRESS_32_FMT "-",
                            STRING_VIEW_FMT_ARGS(Prefix),
                            static_cast<uint32_t>(Begin));
            }
        } else {
            WrittenOut += fputs("0x0-", OutFile);
        }

        if (Is64Bit || SizeIs64Bit){
            WrittenOut +=
                fprintf(OutFile,
                        ADDRESS_64_FMT STRING_VIEW_FMT,
                        End,
                        STRING_VIEW_FMT_ARGS(Suffix));
        } else {
            assert(End <= std::numeric_limits<uint32_t>::max());
            WrittenOut +=
                fprintf(OutFile,
                        ADDRESS_32_FMT STRING_VIEW_FMT,
                        static_cast<uint32_t>(End),
                        STRING_VIEW_FMT_ARGS(Suffix));
        }

        return WrittenOut;
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
                    PadSpaces(OutFile,
                              MachO::SegmentMaxNameLength - SegmentLength);
            }
        }

        if (!Segment.empty()) {
            WrittenOut +=
                fprintf(OutFile,
                        "\"" STRING_VIEW_FMT "\",",
                        STRING_VIEW_FMT_ARGS(Segment));
        } else {
            WrittenOut += fputs("<null>,", OutFile);
        }

        if (!Section.empty()) {
            WrittenOut +=
                fprintf(OutFile,
                        "\"" STRING_VIEW_FMT "\"",
                        STRING_VIEW_FMT_ARGS(Section));
        } else {
            WrittenOut += fputs("<null>", OutFile);
        }

        if (PadSections) {
            const auto SectionLength =
                !Section.empty() ? Section.length() : STR_LENGTH("<null>");

            if (SectionLength < MachO::SegmentSectionMaxNameLength) {
                const auto PadLength =
                    MachO::SegmentSectionMaxNameLength - SectionLength;

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

    auto
    PrintOnlyOnce(FILE *const OutFile,
                  const char *const String,
                  bool &DidPrint) noexcept -> int
    {
        if (DidPrint) {
            return 0;
        }

        DidPrint = true;
        return fprintf(OutFile, "%s", String);
    }

    auto
    PrintOnlyAfterFirst(FILE *const OutFile,
                        const char *const String,
                        bool &DidPassFirst) noexcept -> int
    {
        if (DidPassFirst) {
            return fprintf(OutFile, "%s", String);
        }

        DidPassFirst = true;
        return 0;
    }

    auto
    PrintUuid(FILE *OutFile,
              const uint8_t Uuid[16],
              const std::string_view Prefix,
              const std::string_view Suffix) noexcept -> int
    {
        const auto Result =
            fprintf(OutFile,
                    "%s%.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2x%.2x"
                    "%.2x%.2X%.2X%s",
                    Prefix.data(),
                    Uuid[0],
                    Uuid[1],
                    Uuid[2],
                    Uuid[3],
                    Uuid[4],
                    Uuid[5],
                    Uuid[6],
                    Uuid[7],
                    Uuid[8],
                    Uuid[9],
                    Uuid[10],
                    Uuid[11],
                    Uuid[12],
                    Uuid[13],
                    Uuid[14],
                    Uuid[15],
                    Suffix.data());

        return Result;
    }
}
