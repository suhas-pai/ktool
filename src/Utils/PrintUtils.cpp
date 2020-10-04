//
//  src/Utils/PrintUtils.cpp
//  ktool
//
//  Created by Suhas Pai on 6/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/MachO.h"
#include "PrintUtils.h"

int
PrintUtilsWriteMachOSegmentSectionPair(FILE *OutFile,
                                       const char *SegmentName,
                                       const char *SectionName,
                                       bool Pad,
                                       const char *Prefix,
                                       const char *Suffix) noexcept
{
    constexpr static auto NameLengthMax = 16;
    auto WrittenOut = fprintf(OutFile, "%s", Prefix);

    if (SegmentName != nullptr) {
        if (Pad) {
            const auto PadLength =
                NameLengthMax - strnlen(SegmentName, NameLengthMax);

            if (PadLength != 0) {
                PrintUtilsPadSpaces(OutFile, static_cast<int>(PadLength));
            }
        }

        WrittenOut +=
            fprintf(OutFile, "\"" CHAR_ARR_FMT(16) "\",", SegmentName);

        auto PrintSectLength = 0;
        if (SectionName != nullptr) {
            PrintSectLength =
                fprintf(OutFile, "\"" CHAR_ARR_FMT(16) "\"", SectionName);
        } else {
            PrintSectLength = fputs("<invalid>", OutFile);
        }

        if (Pad) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         PrintSectLength,
                                         NameLengthMax + LENGTH_OF("\"\""));
        }

        WrittenOut += PrintSectLength;
    } else if (Pad) {
        // 2 NameLengthMax for max-names of segment and section.
        constexpr auto MaxWrittenOut =
            (NameLengthMax * 2) + LENGTH_OF("\"\",\"\"");

        PrintUtilsPadSpaces(OutFile, MaxWrittenOut);
    }

    WrittenOut += fprintf(OutFile, "%s", Suffix);
    return WrittenOut;
}

int
PrintUtilsWriteMachOSegmentSectionPair(FILE *OutFile,
                                       const MachO::SegmentInfo *Segment,
                                       const MachO::SectionInfo *Section,
                                       bool Pad,
                                       const char *Prefix,
                                       const char *Suffix) noexcept
{
    auto SegmentName = static_cast<const char *>(nullptr);
    auto SectionName = static_cast<const char *>(nullptr);

    if (Segment != nullptr) {
        SegmentName = Segment->getName().data();
    }

    if (Section != nullptr) {
        SectionName = Section->getName().data();
    }

    const auto Result =
        PrintUtilsWriteMachOSegmentSectionPair(OutFile,
                                               SegmentName,
                                               SectionName,
                                               Pad,
                                               Prefix,
                                               Suffix);
    return Result;
}

int
PrintUtilsWriteUuid(FILE *OutFile,
                    const uint8_t Uuid[16],
                    const char *Prefix,
                    const char *Suffix) noexcept
{
    const auto Result =
        fprintf(OutFile,
                "%s%.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2x%.2x%.2x"
                "%.2X%.2X%s",
                Prefix,
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
                Suffix);

    return Result;
}

using namespace std::literals;

constexpr static std::string_view FormatNames[] = {
    "KiloBytes"sv,
    "MegaBytes"sv,
    "GigaBytes"sv,
    "TeraBytes"sv,
    "PetaBytes"sv,
    "ExaBytes"sv,
    "Zettabyte"sv,
    "Yottabyte"sv
};

int
PrintUtilsWriteFormattedSize(FILE *OutFile,
                             uint64_t Size,
                             const char *Prefix,
                             const char *Suffix) noexcept
{
    constexpr auto Base = 1000;

    auto Result = double(Size);
    auto Index = uint32_t();

    if (Result < Base) {
        return 0;
    }

    Result /= Base;
    while (Result >= Base) {
        Result /= Base;
        Index++;
    };

    assert(Index < countof(FormatNames));

    const auto &Name = FormatNames[Index];
    fprintf(stdout, "%s%.3f %s%s", Prefix, Result, Name.data(), Suffix);

    return 0;
}
