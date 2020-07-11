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
                                       bool Pad) noexcept
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
                                               Pad);
    return Result;
}

int PrintUtilsWriteUuid(FILE *OutFile, const uint8_t Uuid[16]) noexcept {
    const auto Result =
        fprintf(OutFile,
                "%.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X"
                "%.2X%.2X%.2X%.2X%.2X",
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
                Uuid[15]);

    return Result;
}
