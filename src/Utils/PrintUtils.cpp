//
//  Utils/PrintUtils.cpp
//  ktool
//
//  Created by Suhas Pai on 6/4/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/SegmentInfo.h"
#include "Utils/PrintUtils.h"

int
PrintUtilsWriteMachOSegmentSectionPair(FILE *const OutFile,
                                       const char *const SegmentName,
                                       const char *const SectionName,
                                       const bool Pad,
                                       const char *const Prefix,
                                       const char *const Suffix) noexcept
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
PrintUtilsWriteMachOSegmentSectionPair(FILE *const OutFile,
                                       const MachO::SegmentInfo *const Segment,
                                       const MachO::SectionInfo *const Section,
                                       const bool Pad,
                                       const char *const Prefix,
                                       const char *const Suffix) noexcept
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
