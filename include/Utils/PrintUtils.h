//
//  include/Utils/PrintUtils.h
//  stool
//
//  Created by Suhas Pai on 4/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <limits>
#include <inttypes.h>
#include <type_traits>
#include <string>
#include <unistd.h>

#include "ADT/LargestIntHelper.h"
#include "DoesOverflow.h"
#include "Macros.h"

#define CHAR_ARR_FMT(Size) "%." TO_STRING(Size) "s"
#define CHAR_ARR_AND_RIGHTPAD_FMT(Size) "%" TO_STRING(Size) "s"

#define CHAR_ARR_AND_RIGHTPAD_WITH_SIZE_FMT(Pad, Size) \
    "%" PRINTF_RIGHTPAD_WITH_SIZE_FMT(Pad) "." TO_STRING(Size) "s"

#define OFFSET_32_FMT "0x%08" PRIX32
#define OFFSET_64_FMT "0x%016" PRIX64

#define OFFSET_32_LEN LENGTH_OF("0x12345678")
#define OFFSET_64_LEN LENGTH_OF("0x123456789ABCDEFG")

#define OFFSET_LEN(Is64Bit) ((Is64Bit) ? OFFSET_64_LEN : OFFSET_32_LEN)

#define OFFSET_32_NO_L_ZEROS_FMT "0x%8" PRIX32
#define OFFSET_64_NO_L_ZEROS_FMT "0x%16" PRIX64

#define OFFSET_32_RNG_FMT OFFSET_32_FMT "-" OFFSET_32_FMT
#define OFFSET_64_RNG_FMT OFFSET_64_FMT "-" OFFSET_64_FMT
#define OFFSET_32_64_RNG_FMT OFFSET_32_FMT "-" OFFSET_64_FMT

#define OFFSET_32_RANGE_LEN OFFSET_32_LEN + LENGTH_OF("-") + OFFSET_32_LEN
#define OFFSET_64_RANGE_LEN OFFSET_64_LEN + LENGTH_OF("-") + OFFSET_64_LEN
#define OFFSET_32_64_RANGE_LEN OFFSET_32_LEN + LENGTH_OF("-") + OFFSET_64_LEN

#define OFFSET_32_OVERFLOW_FMT OFFSET_32_FMT "-(Overflows)"
#define OFFSET_64_OVERFLOW_FMT OFFSET_64_FMT "-(Overflows)"
#define OFFSET_0x0 "0x0"

#define MEM_PROT_FMT "%c%c%c"
#define MEM_PROT_FMT_ARGS(Obj) \
    (Obj.IsReadable() ? 'r' : '-'), \
    (Obj.IsWritable() ? 'w' : '-'), \
    (Obj.IsExecutable() ? 'x' : '-')

#define MEM_PROT_LEN LENGTH_OF("rwx")

#define PRINTF_RIGHTPAD_FMT "-*"
#define PRINTF_RIGHTPAD_WITH_SIZE_FMT(Size) "-" TO_STRING(Size)

#define PRINTF_PRECISION_FMT ".*"
#define PRINTF_PRECISION_WITH_SIZE_FMT(Size) "." TO_STRING(Size)

#define PRINTF_LEFTPAD_FMT "*"
#define PRINTF_LEFTPAD_WITH_SIZE_FMT(Size) TO_STRING(Size)

#define MACHO_PACKED_VERSION_LEN LENGTH_OF("1234.1234.1234")
#define MACHO_PACKED_VERSION_FMT "%" PRIu32 ".%" PRIu16 ".%" PRIu8
#define MACHO_PACKED_VERSION_FMT_ARGS(PackedVersion) \
    PackedVersion.getMajor(), \
    PackedVersion.getMinor(), \
    PackedVersion.getRevision()

inline int
PrintUtilsWriteOffsetOverflowsRange(FILE *OutFile, uint32_t Offset) noexcept {
    if (Offset != 0) {
        return fprintf(OutFile, OFFSET_32_OVERFLOW_FMT, Offset);
    }

    return fprintf(OutFile, OFFSET_0x0);
}

inline int
PrintUtilsWriteOffsetOverflowsRange(FILE *OutFile, uint64_t Offset) noexcept {
    if (Offset != 0) {
        return fprintf(OutFile, OFFSET_64_OVERFLOW_FMT, Offset);
    }

    return fprintf(OutFile, OFFSET_0x0);
}

inline int
PrintUtilsWriteOffsetRange(FILE *OutFile, uint32_t Begin, uint32_t End) noexcept
{
    if (Begin != 0) {
        return fprintf(OutFile, OFFSET_32_RNG_FMT, Begin, End);
    }

    return fprintf(OutFile, OFFSET_0x0 "-" OFFSET_32_FMT, End);
}

inline int
PrintUtilsWriteOffsetRange(FILE *OutFile, uint64_t Begin, uint64_t End) noexcept
{
    if (Begin != 0) {
        return fprintf(OutFile, OFFSET_64_RNG_FMT, Begin, End);
    }

    return fprintf(OutFile, OFFSET_0x0 "-" OFFSET_64_FMT, End);
}

inline int
PrintUtilsWriteOffset32To64Range(FILE *OutFile,
                                 uint32_t Begin,
                                 uint64_t End) noexcept
{
    if (Begin != 0) {
        return fprintf(OutFile, OFFSET_32_64_RNG_FMT, Begin, End);
    }

    return fprintf(OutFile, OFFSET_0x0 "-" OFFSET_64_FMT, End);
}

inline int
PrintUtilsPrintOffset(FILE *OutFile, uint64_t Offset, bool Is64Bit) noexcept {
    if (Is64Bit) {
        return fprintf(OutFile, OFFSET_64_FMT, Offset);
    }

    return fprintf(OutFile, OFFSET_32_FMT, static_cast<uint32_t>(Offset));
}

inline int
PrintUtilsWriteOffsetRange(FILE *OutFile,
                           uint64_t Begin,
                           uint64_t End,
                           bool Is64Bit) noexcept
{
    if (Is64Bit) {
        return PrintUtilsWriteOffsetRange(OutFile, Begin, End);
    }

    const auto Result =
        PrintUtilsWriteOffsetRange(OutFile,
                                   static_cast<uint32_t>(Begin),
                                   static_cast<uint32_t>(End));
    return Result;
}

inline int
PrintUtilsWriteOffsetRange32(FILE *OutFile,
                             uint32_t Begin,
                             uint64_t End,
                             bool Is64Bit) noexcept
{
    if (Is64Bit) {
        return PrintUtilsWriteOffset32To64Range(OutFile, Begin, End);
    }

    const auto Result =
        PrintUtilsWriteOffsetRange(OutFile, Begin, static_cast<uint32_t>(End));

    return Result;
}

inline int
PrintUtilsWriteSizeRange(FILE *OutFile,
                         bool Is64Bit,
                         uint32_t Offset,
                         uint32_t Size,
                         uint64_t *EndOut) noexcept
{
    auto End = uint64_t();
    auto Overflows = false;

    if (Is64Bit) {
        Overflows = DoesAddOverflow(Offset, Size, &End);
    } else {
        Overflows = DoesAddOverflow<uint32_t>(Offset, Size, &End);
    }

    if (Overflows) {
        return PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
    }

    if (EndOut != nullptr) {
        *EndOut = End;
    }

    if (Is64Bit) {
        return PrintUtilsWriteOffset32To64Range(OutFile, Offset, End);
    }

    const auto End32 = static_cast<uint32_t>(End);
    return PrintUtilsWriteOffsetRange(OutFile, Offset, End32);
}

inline int
PrintUtilsWriteSizeRange(FILE *OutFile,
                         bool Is64Bit,
                         uint64_t Offset,
                         uint64_t Size,
                         uint64_t *EndOut) noexcept
{
    auto End = uint64_t();
    auto Overflows = false;

    if (Is64Bit) {
        Overflows = DoesAddOverflow(Offset, Size, &End);
    } else {
        Overflows = DoesAddOverflow<uint32_t>(Offset, Size, &End);
    }
    
    if (Overflows) {
        return PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
    }

    if (EndOut != nullptr) {
        *EndOut = End;
    }

    return PrintUtilsWriteOffsetRange(OutFile, Offset, End);
}

inline int
PrintUtilsCharMultTimes(FILE *OutFile, char Ch, uint64_t Times) noexcept {
    assert(Times >= 0 && "PrintUtilsCharMultTimes(): Times less than 0");
    for (auto I = uint64_t(); I != Times; I++) {
        fputc(Ch, OutFile);
    }

    return static_cast<int>(Times);
}

inline int
PrintUtilsStringMultTimes(FILE *OutFile,
                          const char *Str,
                          uint64_t Times) noexcept
{
    assert(Times >= 0 && "PrintUtilsStringMultTimes(): Times less than 0");

    auto Total = int();
    for (auto I = uint64_t(); I != Times; I++) {
        Total += fputs(Str, OutFile);
    }

    return Total;
}

inline int PrintUtilsPadSpaces(FILE *OutFile, int Times) noexcept {
    assert(Times >= 0 && "PrintUtilsPadSpaces(): Times less than 0");
    return fprintf(OutFile, "%" PRINTF_RIGHTPAD_FMT "s", Times, "");
}

inline int PrintUtilsPadTabs(FILE *OutFile, int Times) noexcept {
    assert(Times >= 0 && "PrintUtilsPadSpaces(): Times less than 0");
    return PrintUtilsCharMultTimes(OutFile, '\t', Times);
}

inline int
PrintUtilsRightPadSpaces(FILE *OutFile, int WrittenOut, int Total) noexcept {
    const auto PadLength = (Total - WrittenOut);
    return PrintUtilsPadSpaces(OutFile, PadLength);
}

template <const char String[], int Length>
inline int PrintUtilsWriteBackwards(FILE *OutFile) noexcept {
    PrintUtilsCharMultTimes(OutFile, '\b', Length);
    return fprintf(OutFile, "%" PRINTF_PRECISION_FMT "s", Length, String);
}

inline int
PrintUtilsWriteBackwards(FILE *OutFile, const char *String, int Length) noexcept
{
    PrintUtilsCharMultTimes(OutFile, '\b', Length);
    return fprintf(OutFile, "%" PRINTF_PRECISION_FMT "s", Length, String);
}

template <typename T>
inline int PrintUtilsGetIntegerDigitLength(T Integer) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");

    auto DigitLength = int();
    do {
        DigitLength++;
    } while (Integer /= 10);

    return DigitLength;
}

template <typename T>
inline int
PrintUtilsGetIntegerDigitLength(const LargestIntHelper<T> &Helper) noexcept {
    return PrintUtilsGetIntegerDigitLength(Helper.value());
}

inline int
PrintUtilsWriteMachOSegmentSectionPair(FILE *OutFile,
                                       const char *SegmentName,
                                       const char *SectionName,
                                       bool Pad) noexcept
{
    constexpr static auto NameLengthMax = 16;
    auto WrittenOut = int();

    if (SegmentName != nullptr) {
        const auto PadLength =
            NameLengthMax - strnlen(SegmentName, NameLengthMax);

        if (PadLength != 0) {
            PrintUtilsPadSpaces(OutFile, static_cast<int>(PadLength));
        }

        WrittenOut +=
            fprintf(OutFile, "\"" CHAR_ARR_FMT(16) "\",", SegmentName);

        if (SectionName != nullptr) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         fprintf(OutFile,
                                                 "\"" CHAR_ARR_FMT(16) "\"",
                                                 SectionName),
                                         NameLengthMax + LENGTH_OF("\"\""));
        } else {
            WrittenOut += fputs("<invalid>", OutFile);
        }
    } else if (Pad) {
        // 2 NameLengthMax for max-names of segment and section, 4 for the
        // apostraphes, and 1 for the comma.

        constexpr auto MaxWrittenOut = (2 * NameLengthMax) + 4 + 1;
        PrintUtilsRightPadSpaces(OutFile, WrittenOut, MaxWrittenOut);
    }

    return WrittenOut;
}

namespace MachO {
    struct SegmentInfo;
    struct SectionInfo;
}

int
PrintUtilsWriteMachOSegmentSectionPair(FILE *OutFile,
                                       const MachO::SegmentInfo *Segment,
                                       const MachO::SectionInfo *Section,
                                       bool Pad) noexcept;
