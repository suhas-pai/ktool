//
//  include/Utils/PrintUtils.h
//  stool
//
//  Created by Suhas Pai on 4/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <limits>
#include <inttypes.h>
#include <type_traits>
#include <unistd.h>

#include "DoesOverflow.h"
#include "Macros.h"

#define CHAR_ARR_FMT(Size) "%." TO_STRING(Size) "s"
#define CHAR_ARR_AND_RIGHTPAD_FMT(Size) \
    "%" PRINTF_RIGHTPAD_FMT "." TO_STRING(Size) "s"

#define CHAR_ARR_AND_RIGHTPAD_WITH_SIZE_FMT(Pad, Size) \
    "%" PRINTF_RIGHTPAD_WITH_SIZE_FMT(Pad) "." TO_STRING(Size) "s"

#define OFFSET_32_FMT "0x%08" PRIX32
#define OFFSET_64_FMT "0x%016" PRIX64

#define OFFSET_32_LEN LENGTH_OF("0x12345678")
#define OFFSET_64_LEN LENGTH_OF("0x123456789ABCDEFG")

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

#define PRINTF_RIGHTPAD_FMT "-*"
#define PRINTF_RIGHTPAD_WITH_SIZE_FMT(Size) "-" TO_STRING(Size)

#define PRINTF_PRECISION_FMT ".*"
#define PRINTF_PRECISION_WITH_SIZE_FMT(Size) "." TO_STRING(Size)

#define PRINTF_LEFTPAD_FMT "*"
#define PRINTF_LEFTPAD_WITH_SIZE_FMT(Size) TO_STRING(Size)

#define MACHO_PACKED_VERSION_LEN LENGTH_OF("1234.1234.1234")
#define MACHO_PACKED_VERSION_FMT "%" PRIu32 ".%" PRIu16 ".%" PRIu8
#define MACHO_PACKED_VERSION_FMT_ARGS(PackedVersion, IsBigEndian) \
    PackedVersion.GetMajor(IsBigEndian), \
    PackedVersion.GetMinor(IsBigEndian), \
    PackedVersion.GetRevision(IsBigEndian)

inline int
PrintUtilsWriteOffsetOverflowsRange(FILE *OutFile, uint32_t Offset) {
    if (Offset != 0) {
        return fprintf(OutFile, OFFSET_32_OVERFLOW_FMT, Offset);
    }

    return fprintf(OutFile, OFFSET_0x0);
}

inline int
PrintUtilsWriteOffsetOverflowsRange(FILE *OutFile, uint64_t Offset) {
    if (Offset != 0) {
        return fprintf(OutFile, OFFSET_64_OVERFLOW_FMT, Offset);
    }

    return fprintf(OutFile, OFFSET_0x0);
}

inline int
PrintUtilsWriteOffsetRange(FILE *OutFile, uint32_t Begin, uint32_t End) {
    if (Begin != 0) {
        return fprintf(OutFile, OFFSET_32_RNG_FMT, Begin, End);
    }

    return fprintf(OutFile, OFFSET_0x0 "-" OFFSET_32_FMT, End);
}

inline int
PrintUtilsWriteOffsetRange(FILE *OutFile, uint64_t Begin, uint64_t End) {
    if (Begin != 0) {
        return fprintf(OutFile, OFFSET_64_RNG_FMT, Begin, End);
    }

    return fprintf(OutFile, OFFSET_0x0 "-" OFFSET_64_FMT, End);
}

inline int
PrintUtilsWriteOffset32To64Range(FILE *OutFile, uint32_t Begin, uint64_t End) {
    if (Begin != 0) {
        return fprintf(OutFile, OFFSET_32_64_RNG_FMT, Begin, End);
    }

    return fprintf(OutFile, OFFSET_0x0 "-" OFFSET_64_FMT, End);
}

inline int
PrintUtilsWriteSizeRange(FILE *OutFile,
                         bool Is64Bit,
                         uint32_t Offset,
                         uint32_t Size,
                         uint64_t *EndOut)
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
                         uint64_t *EndOut)
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

inline void PrintCharMultTimes(FILE *OutFile, char Ch, uint64_t Times) {
    assert(Times >= 0 && "PrintCharMultTimes(): Times less than 0");
    for (auto I = uint64_t(); I != Times; I++) {
        fputc(Ch, OutFile);
    }
}

inline int PrintUtilsPadSpaces(FILE *OutFile, int Times) {
    assert(Times >= 0 && "PrintUtilsPadSpaces(): Times less than 0");
    return fprintf(OutFile, "%" PRINTF_RIGHTPAD_FMT "s", Times, "");
}

inline int PrintUtilsRightPadSpaces(FILE *OutFile, int WrittenOut, int Total) {
    const auto PadLength = (Total - WrittenOut);
    return PrintUtilsPadSpaces(OutFile, PadLength);
}

template <const char String[], int Length>
inline int PrintUtilsWriteBackwards(FILE *OutFile) {
    PrintCharMultTimes(OutFile, '\b', Length);
    return fprintf(OutFile, "%" PRINTF_PRECISION_FMT "s", Length, String);
}

inline int
PrintUtilsWriteBackwards(FILE *OutFile, const char *String, int Length) {
    PrintCharMultTimes(OutFile, '\b', Length);
    return fprintf(OutFile, "%" PRINTF_PRECISION_FMT "s", Length, String);
}

template <typename T>
inline int PrintUtilsGetIntegerDigitLength(T Integer) {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");

    auto DigitLength = int();
    do {
        DigitLength++;
    } while (Integer /= 10);

    return DigitLength;
}
