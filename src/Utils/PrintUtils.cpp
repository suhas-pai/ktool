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
                                       const MachO::SegmentInfo *Segment,
                                       const MachO::SectionInfo *Section,
                                       bool Pad,
                                       const char *Prefix,
                                       const char *Suffix) noexcept
{
    auto SegmentName = static_cast<const char *>(nullptr);
    auto SectionName = static_cast<const char *>(nullptr);

    if (Segment != nullptr) {
        SegmentName = Segment->Name.data();
    }

    if (Section != nullptr) {
        SectionName = Section->Name.data();
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
                "%s%.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X"
                "%.2X%.2X%.2X%.2X%.2X%s",
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

constexpr static const std::string_view FormatNames[] = {
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
    auto Index = 0;

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
