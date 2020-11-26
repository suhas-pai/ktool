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
