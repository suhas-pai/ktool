//
//  include/Utils/PrintUtils.h
//  ktool
//
//  Created by Suhas Pai on 4/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <charconv>
#include <cstdio>
#include <inttypes.h>
#include <string>

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

#define MAX_32_NUM_LEN LENGTH_OF(4294967295)
#define MAX_64_NUM_LEN LENGTH_OF(9223372036854775807)

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
#define OFFSET_0x0_LEN LENGTH_OF("0x0")

#define MEM_PROT_FMT "%c%c%c"
#define MEM_PROT_FMT_ARGS(Obj) \
    (Obj.isReadable() ? 'r' : '-'), \
    (Obj.isWritable() ? 'w' : '-'), \
    (Obj.isExecutable() ? 'x' : '-')

#define MEM_PROT_INIT_MAX_RNG_FMT MEM_PROT_FMT "/" MEM_PROT_FMT
#define MEM_PROT_INIT_MAX_RNG_FMT_ARGS(INIT, MAX) \
    MEM_PROT_FMT_ARGS(INIT), \
    MEM_PROT_FMT_ARGS(MAX)

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

#define STRING_VIEW_FMT "%*s"
#define STRING_VIEW_FMT_ARGS(STR) \
    static_cast<int>((STR).length()), (STR).data()

enum PrintKind {
    Default,
    Verbose
};

[[nodiscard]]
constexpr bool PrintKindIsVerbose(PrintKind Kind) noexcept {
    return (Kind == PrintKind::Verbose);
}

[[nodiscard]]
constexpr PrintKind PrintKindFromIsVerbose(bool IsVerbose) noexcept {
    return (IsVerbose) ? PrintKind::Verbose : PrintKind::Default;
}

inline int PrintUtilsCharMultTimes(FILE *OutFile, char Ch, int Times) noexcept {
    assert(Times >= 0 && "PrintUtilsCharMultTimes(): Times less than 0");
    for (auto I = int(); I != Times; I++) {
        fputc(Ch, OutFile);
    }

    return Times;
}

inline int
PrintUtilsStringMultTimes(FILE *OutFile, const char *Str, int Times) noexcept {
    assert(Times >= 0 && "PrintUtilsStringMultTimes(): Times less than 0");

    auto Total = int();
    for (auto I = int(); I != Times; I++) {
        Total += fputs(Str, OutFile);
    }

    return Total;
}

inline int PrintUtilsPadSpaces(FILE *OutFile, int Times) noexcept {
    assert(Times >= 0 && "PrintUtilsPadSpaces(): Times less than 0");
    return fprintf(OutFile, "%" PRINTF_RIGHTPAD_FMT "s", Times, "");
}

inline int PrintUtilsPadTabs(FILE *OutFile, int Times) noexcept {
    assert(Times >= 0 && "PrintUtilsPadTabs(): Times less than 0");
    return PrintUtilsCharMultTimes(OutFile, '\t', Times);
}

inline int
PrintUtilsRightPadSpaces(FILE *OutFile, int WrittenOut, int Total) noexcept {
    const auto PadLength = (Total - WrittenOut);
    return PrintUtilsPadSpaces(OutFile, PadLength);
}

inline int
PrintUtilsWriteOffset32OverflowsRange(FILE *OutFile,
                                      uint32_t Offset,
                                      const char *Prefix = "",
                                      const char *Suffix = "") noexcept
{
    if (Offset != 0) {
        const auto Result =
            fprintf(OutFile,
                    "%s" OFFSET_32_OVERFLOW_FMT "%s",
                    Prefix,
                    Offset,
                    Suffix);

        return Result;
    }

    return fprintf(OutFile, "%s" OFFSET_0x0 "%s", Prefix, Suffix);
}

inline int
PrintUtilsWriteOffset64OverflowsRange(FILE *OutFile,
                                      uint64_t Offset,
                                      const char *Prefix = "",
                                      const char *Suffix = "") noexcept {
    if (Offset != 0) {
        const auto Result =
            fprintf(OutFile,
                    "%s" OFFSET_64_OVERFLOW_FMT "%s",
                    Prefix,
                    Offset,
                    Suffix);

        return Result;
    }

    return fprintf(OutFile, "%s" OFFSET_0x0 "%s", Prefix, Suffix);
}

inline int
PrintUtilsWriteOffset32Range(FILE *OutFile,
                             uint32_t Begin,
                             uint32_t End,
                             const char *Prefix = "",
                             const char *Suffix = "") noexcept
{
    if (Begin != 0) {
        const auto Result =
            fprintf(OutFile,
                    "%s" OFFSET_32_RNG_FMT "%s",
                    Prefix,
                    Begin,
                    End,
                    Suffix);

        return Result;
    }

    const auto Result =
        fprintf(OutFile,
                "%s" OFFSET_0x0 "-" OFFSET_32_FMT "%s",
                Prefix,
                End,
                Suffix);

    return Result;
}

inline int
PrintUtilsWriteOffset64Range(FILE *OutFile,
                             uint64_t Begin,
                             uint64_t End,
                             const char *Prefix = "",
                             const char *Suffix = "") noexcept
{
    if (Begin != 0) {
        const auto Result =
            fprintf(OutFile,
                    "%s" OFFSET_64_RNG_FMT "%s",
                    Prefix,
                    Begin,
                    End,
                    Suffix);

        return Result;
    }

    const auto Result =
        fprintf(OutFile,
                "%s" OFFSET_0x0 "-" OFFSET_64_FMT "%s",
                Prefix,
                End,
                Suffix);

    return Result;
}

inline int
PrintUtilsWriteOffset32To64Range(FILE *OutFile,
                                 uint32_t Begin,
                                 uint64_t End,
                                 const char *Prefix = "",
                                 const char *Suffix = "") noexcept
{
    if (Begin != 0) {
        const auto Result =
            fprintf(OutFile,
                    "%s" OFFSET_32_64_RNG_FMT "%s",
                    Prefix,
                    Begin,
                    End,
                    Suffix);

        return Result;
    }

    const auto Result =
        fprintf(OutFile,
                "%s" OFFSET_0x0 "-" OFFSET_64_FMT "%s",
                Prefix,
                End,
                Suffix);

    return Result;
}

inline int
PrintUtilsWriteOffsetSizeRange(FILE *OutFile,
                               uint32_t Begin,
                               uint32_t Size,
                               const char *Prefix = "",
                               const char *Suffix = "") noexcept
{
    auto End = uint32_t();
    auto Result = int();

    if (DoesAddOverflow(Begin, Size, &End)) {
        Result =
            PrintUtilsWriteOffset32OverflowsRange(OutFile,
                                                  Begin,
                                                  Prefix,
                                                  Suffix);
    } else {
        Result =
            PrintUtilsWriteOffset32Range(OutFile, Begin, End, Prefix, Suffix);
    }

    return Result;
}

template <std::integral T>
inline int
PrintUtilsWriteNumber(FILE *const OutFile,
                      const T Number,
                      const char *const Prefix = "",
                      const char *const Suffix = "")
{
    if constexpr (std::is_same_v<T, uint64_t>) {
        return fprintf(OutFile, "%s%" PRIu64 "%s", Prefix, Number, Suffix);
    }

    if constexpr (std::is_same_v<T, uint32_t>){
        return fprintf(OutFile, "%s%" PRIu32 "%s", Prefix, Number, Suffix);
    }

    if constexpr (std::is_same_v<T, uint16_t>){
        return fprintf(OutFile, "%s%" PRIu16 "%s", Prefix, Number, Suffix);
    }

    return fprintf(OutFile, "%s%" PRIu8 "%s", Prefix, (uint8_t)Number, Suffix);
}

inline int
PrintUtilsWriteOffsetSizeRange(FILE *OutFile,
                               uint64_t Begin,
                               uint64_t Size,
                               const char *Prefix = "",
                               const char *Suffix = "") noexcept
{
    auto End = uint64_t();
    auto Result = int();

    if (DoesAddOverflow(Begin, Size, &End)) {
        Result =
            PrintUtilsWriteOffset64OverflowsRange(OutFile,
                                                  Begin,
                                                  Prefix,
                                                  Suffix);
    } else {
        Result =
            PrintUtilsWriteOffset64Range(OutFile, Begin, End, Prefix, Suffix);
    }

    return Result;
}

inline int
PrintUtilsWriteOffset(FILE *OutFile,
                      uint32_t Offset,
                      bool Pad = true,
                      const char *Prefix = "",
                      const char *Suffix = "") noexcept
{
    if (Offset == 0) {
        auto WrittenOut =
            fprintf(OutFile, "%s" OFFSET_0x0 "%s", Prefix, Suffix);

        if (Pad) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         OFFSET_0x0_LEN,
                                         OFFSET_64_LEN);
        }

        return WrittenOut;
    }

    return fprintf(OutFile, "%s" OFFSET_32_FMT "%s", Prefix, Offset, Suffix);
}

inline int
PrintUtilsWriteOffset(FILE *OutFile,
                      uint64_t Offset,
                      bool Pad = true,
                      const char *Prefix = "",
                      const char *Suffix = "") noexcept
{
    if (Offset == 0) {
        auto WrittenOut = fprintf(OutFile, "%s", Prefix);

        WrittenOut += fputs(OFFSET_0x0, OutFile);
        WrittenOut += fprintf(OutFile, "%s", Suffix);

        if (Pad) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         OFFSET_0x0_LEN,
                                         OFFSET_64_LEN);
        }

        return WrittenOut;
    }

    return fprintf(OutFile, "%s" OFFSET_64_FMT "%s", Prefix, Offset, Suffix);
}

inline int
PrintUtilsWriteOffset32Or64(FILE *OutFile,
                            bool Is64Bit,
                            uint64_t Offset,
                            bool Pad = true,
                            const char *Prefix = "",
                            const char *Suffix = "") noexcept
{
    if (Is64Bit) {
        return PrintUtilsWriteOffset(OutFile, Offset, Pad, Prefix, Suffix);
    }

    const auto Result =
        PrintUtilsWriteOffset(OutFile,
                              static_cast<uint32_t>(Offset),
                              Pad,
                              Prefix,
                              Suffix);

    return Result;
}

inline int
PrintUtilsWriteOffsetRange(FILE *OutFile,
                           uint64_t Begin,
                           uint64_t End,
                           bool Is64Bit,
                           const char *Prefix = "",
                           const char *Suffix = "") noexcept
{
    if (Is64Bit) {
        const auto Result =
            PrintUtilsWriteOffset64Range(OutFile, Begin, End, Prefix, Suffix);

        return Result;
    }

    const auto Result =
        PrintUtilsWriteOffset32Range(OutFile,
                                     static_cast<uint32_t>(Begin),
                                     static_cast<uint32_t>(End),
                                     Prefix,
                                     Suffix);
    return Result;
}

inline int
PrintUtilsWriteOffsetRange32(FILE *OutFile,
                             uint32_t Begin,
                             uint64_t End,
                             bool Is64Bit,
                             const char *Prefix = "",
                             const char *Suffix = "") noexcept
{
    if (Is64Bit) {
        const auto Result =
            PrintUtilsWriteOffset32To64Range(OutFile,
                                             Begin,
                                             End,
                                             Prefix,
                                             Suffix);

        return Result;
    }

    const auto Result =
        PrintUtilsWriteOffset32Range(OutFile,
                                     Begin,
                                     static_cast<uint32_t>(End),
                                     Prefix,
                                     Suffix);
    return Result;
}

inline int
PrintUtilsWriteSizeRange32(FILE *OutFile,
                           bool Is64Bit,
                           uint32_t Offset,
                           uint32_t Size,
                           uint64_t *EndOut,
                           const char *Prefix = "",
                           const char *Suffix = "") noexcept
{
    auto End = uint64_t();
    auto Overflows = false;

    if (Is64Bit) {
        Overflows = DoesAddOverflow(Offset, Size, &End);
    } else {
        Overflows = DoesAddOverflow<uint32_t>(Offset, Size, &End);
    }

    if (Overflows) {
        const auto Result =
            PrintUtilsWriteOffset32OverflowsRange(OutFile,
                                                  Offset,
                                                  Prefix,
                                                  Suffix);

        return Result;
    }

    if (EndOut != nullptr) {
        *EndOut = End;
    }

    if (Is64Bit) {
        const auto Result =
            PrintUtilsWriteOffset32To64Range(OutFile,
                                             Offset,
                                             End,
                                             Prefix,
                                             Suffix);

        return Result;
    }

    const auto End32 = static_cast<uint32_t>(End);
    return PrintUtilsWriteOffset32Range(OutFile, Offset, End32, Prefix, Suffix);
}

inline int
PrintUtilsWriteSizeRange64(FILE *OutFile,
                           bool Is64Bit,
                           uint64_t Offset,
                           uint64_t Size,
                           uint64_t *EndOut,
                           const char *Prefix = "",
                           const char *Suffix = "") noexcept
{
    auto End = uint64_t();
    auto Overflows = false;

    if (Is64Bit) {
        Overflows = DoesAddOverflow(Offset, Size, &End);
    } else {
        Overflows = DoesAddOverflow<uint32_t>(Offset, Size, &End);
    }

    if (Overflows) {
        const auto Result =
            PrintUtilsWriteOffset64OverflowsRange(OutFile,
                                                  Offset,
                                                  Prefix,
                                                  Suffix);

        return Result;
    }

    if (EndOut != nullptr) {
        *EndOut = End;
    }

    return PrintUtilsWriteOffset64Range(OutFile, Offset, End, Prefix, Suffix);
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

template <std::integral T>
[[nodiscard]] inline int PrintUtilsGetIntegerDigitLength(T Integer) noexcept {
    auto DigitLength = int();
    do {
        DigitLength++;
    } while (Integer /= 10);

    return DigitLength;
}

template <typename T>
[[nodiscard]] inline int
PrintUtilsGetIntegerDigitLength(const LargestIntHelper<T> &Helper) noexcept {
    return PrintUtilsGetIntegerDigitLength(Helper.value());
}

int
PrintUtilsWriteMachOSegmentSectionPair(FILE *OutFile,
                                       const char *SegmentName,
                                       const char *SectionName,
                                       bool Pad,
                                       const char *Prefix = "",
                                       const char *Suffix = "") noexcept;

namespace MachO {
    struct SegmentInfo;
    struct SectionInfo;
}

int
PrintUtilsWriteMachOSegmentSectionPair(FILE *OutFile,
                                       const MachO::SegmentInfo *Segment,
                                       const MachO::SectionInfo *Section,
                                       bool Pad,
                                       const char *Prefix = "",
                                       const char *Suffix = "") noexcept;

inline int
PrintUtilsWriteUuid(FILE *OutFile,
                    const uint8_t Uuid[16],
                    const char *Prefix = "",
                    const char *Suffix = "") noexcept
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

constexpr static std::string_view PrintUtilFormatSizeNames[] = {
    "KiloBytes",
    "MegaBytes",
    "GigaBytes",
    "TeraBytes",
    "PetaBytes",
    "ExaBytes",
    "Zettabyte",
    "Yottabyte"
};

template <std::integral T>
inline int
PrintUtilsWriteFormattedNumber(FILE *const OutFile,
                               const T Number,
                               const char *const Prefix = "",
                               const char *const Suffix = "") noexcept
{
    char buffer[MAX_64_NUM_LEN + 1] = {};
    std::to_chars(buffer, buffer + sizeof(buffer), Number);

    auto String = std::string(buffer);
    for (auto I = (int64_t)String.length() - 3; I > 0; I -= 3) {
        String.insert(I, 1, ',');
    }

    return fprintf(OutFile, "%s%s%s", Prefix, String.c_str(), Suffix);
}

inline int
PrintUtilsWriteFormattedSize(FILE *OutFile,
                             uint64_t Size,
                             const char *Prefix = "",
                             const char *Suffix = "") noexcept
{
    constexpr auto Base = 1000;

    auto Result = double(Size);
    auto Index = uint32_t();

    if (Result < Base) {
        fprintf(OutFile, "%s%" PRIu64 " bytes%s", Prefix, Size, Suffix);
        return 0;
    }

    Result /= Base;
    while (Result >= Base) {
        Result /= Base;
        Index++;
    };

    assert(Index < countof(PrintUtilFormatSizeNames));

    const auto &Name = PrintUtilFormatSizeNames[Index];
    fprintf(OutFile, "%s%.3f %s%s", Prefix, Result, Name.data(), Suffix);

    return 0;
}

inline int
PrintUtilsWriteItemAfterFirstForList(FILE *OutFile,
                                     const char *String,
                                     bool &DidPassFirst) noexcept
{
    auto WrittenOut = int();
    if (DidPassFirst) {
        WrittenOut = fprintf(OutFile, "%s", String);
    }

    DidPassFirst = true;
    return WrittenOut;
}
