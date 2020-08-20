//
//  src/Utils/MachOLoadCommandPrinter.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>

#include "ADT/Mach-O/LoadCommands.h"
#include "ADT/MachO.h"
#include "ADT/RelativeRange.h"

#include "DoesOverflow.h"
#include "MachOPrinter.h"
#include "PrintUtils.h"
#include "SwitchEndian.h"
#include "Timestamp.h"

template <typename OffsetType, typename SizeType>
static inline int
__MLCP_WriteOffsetRange(FILE *OutFile,
                        const OffsetType &Offset,
                        const SizeType &Size,
                        bool Pad,
                        SizeType *EndOut) noexcept
{
    static_assert(std::is_integral_v<OffsetType> &&
                  std::is_integral_v<SizeType>,
                  "SizeType must be an integer-type");

    auto End = SizeType();
    auto WrittenOut = int();
    auto DidOverflow = DoesAddOverflow(Offset, Size, &End);

    constexpr auto Is64Bit = std::is_same_v<SizeType, uint64_t>;
    constexpr auto OffsetTypeIs64Bit = std::is_same_v<OffsetType, uint64_t>;
    constexpr auto ExpectedWrittenOut =
        (Is64Bit) ?
            (OffsetTypeIs64Bit) ?
                OFFSET_64_RANGE_LEN :
                OFFSET_32_64_RANGE_LEN :
            OFFSET_32_RANGE_LEN;

    if constexpr (Is64Bit) {
        if (!DidOverflow) {
            if constexpr (OffsetTypeIs64Bit) {
                WrittenOut = PrintUtilsWriteOffset64Range(OutFile, Offset, End);
            } else {
                WrittenOut =
                    PrintUtilsWriteOffset32To64Range(OutFile, Offset, End);
            }
        } else {
            if constexpr (OffsetTypeIs64Bit) {
                WrittenOut =
                    PrintUtilsWriteOffset64OverflowsRange(OutFile, Offset);
            } else {
                WrittenOut =
                    PrintUtilsWriteOffset32OverflowsRange(OutFile, Offset);
            }
        }
    } else {
        if (!DidOverflow) {
            WrittenOut = PrintUtilsWriteOffset32Range(OutFile, Offset, End);
        } else {
            WrittenOut = PrintUtilsWriteOffset32OverflowsRange(OutFile, Offset);
        }
    }

    if (Pad) {
        WrittenOut +=
            PrintUtilsPadSpaces(OutFile, ExpectedWrittenOut - WrittenOut);
    }

    if (EndOut != nullptr) {
        *EndOut = End;
    }

    return WrittenOut;
}

template <typename SizeOneType, typename SizeTwoType>
static inline int
__MLCP_WriteSizeDiff(FILE *OutFile,
                     const SizeOneType &SizeOne,
                     const SizeTwoType &SizeTwo,
                     bool Pad) noexcept
{
    static_assert(std::is_integral_v<SizeOneType> &&
                  std::is_integral_v<SizeTwoType>,
                  "SizeType must be an integer-type");

    static_assert(std::is_same_v<SizeOneType, SizeTwoType>,
                  "SizeOneType and SizeTwoSize muwt be the same type");

    constexpr static const auto WriteLengthMax = LENGTH_OF(" (+000000");
    constexpr static const auto WriteMax =
        WriteLengthMax + LENGTH_OF(" Bytes)");

    if (SizeOne == SizeTwo) {
        if (Pad) {
            return PrintUtilsPadSpaces(OutFile, WriteMax);
        }

        return 0;
    }

    auto Diff = SizeOneType();
    auto Sign = '\0';

    if (SizeTwo > SizeOne) {
        Diff = SizeTwo - SizeOne;
        Sign = '+';
    } else {
        Diff = SizeOne - SizeTwo;
        Sign = '-';
    }

    auto WrittenOut = int();
    if (Pad) {
        const auto WriteLength =
            LENGTH_OF("(+") + PrintUtilsGetIntegerDigitLength(Diff);
        const auto PadSpacesLength =
            static_cast<int>(WriteLengthMax - WriteLength);

        WrittenOut += PrintUtilsPadSpaces(OutFile, PadSpacesLength);
    }

    if constexpr (std::is_same_v<SizeOneType, uint64_t>) {
        WrittenOut += fprintf(OutFile, " (%c%" PRIu64, Sign, Diff);
    } else {
        WrittenOut += fprintf(OutFile, " (%c%" PRIu32, Sign, Diff);

    }

    WrittenOut += fputs(" Bytes)", OutFile);
    return WrittenOut;
}

template <bool Pad, typename SizeType>
static inline int
__MLCP_WriteSizeInfo(FILE *OutFile,
                     const SizeType &Size,
                     bool PrintFormatted) noexcept
{
    auto WrittenOut = int();
    if constexpr (Pad) {
        // Pad to the left of the Byte Info [which is (0000 Bytes)], so that the
        // " Bytes)" line up

        const auto SizeLength = PrintUtilsGetIntegerDigitLength(Size);
        WrittenOut = PrintUtilsPadSpaces(OutFile, 9 - (SizeLength + 1));
    }

    if constexpr (std::is_same_v<SizeType, uint64_t>) {
        WrittenOut += fprintf(OutFile, " (%" PRIu64, Size);
    } else {
        WrittenOut += fprintf(OutFile, " (%" PRIu32, Size);
    }

    WrittenOut += fputs(" Bytes)", OutFile);
    if (PrintFormatted) {
        WrittenOut += PrintUtilsWriteFormattedSize(OutFile, Size, " (", ")");
    }

    return WrittenOut;
}

static inline void
__MLCP_WritePastEOFWarning(FILE *OutFile,
                           const RelativeRange &FileRange,
                           uint64_t End,
                           bool Pad,
                           const char *LineSuffix = "") noexcept
{
    using namespace std::literals;
    constexpr auto Warning = " (Past EOF!)"sv;

    if (FileRange.containsEndLocation(End)) {
        if (Pad) {
            PrintUtilsPadSpaces(OutFile, Warning.length());
        }

        fprintf(OutFile, "%s", LineSuffix);
    } else {
        fprintf(OutFile, "%s%s", Warning.data(), LineSuffix);
    }
}

template <bool OneLine,
          typename FileOffType,
          typename VmAddrType,
          typename SizeType>

static inline void
MachOLoadCommandPrinterWriteFileAndVmRange(FILE *OutFile,
                                           const RelativeRange &FileRange,
                                           FileOffType FileOff,
                                           VmAddrType VmAddr,
                                           SizeType FileSize,
                                           SizeType VmSize,
                                           bool Verbose,
                                           const char *LinePrefix = "") noexcept
{
    static_assert(std::is_integral_v<FileOffType> &&
                  std::is_integral_v<VmAddrType> &&
                  std::is_integral_v<SizeType>,
                  "Types need to be integer-types!");

    fprintf(OutFile, "%sFile:%c", LinePrefix, (OneLine) ? ' ' : '\t');

    const auto ShouldPrintSizeInfo = (!OneLine || Verbose);
    auto FileOffEnd = SizeType();

    __MLCP_WriteOffsetRange(OutFile, FileOff, FileSize, OneLine, &FileOffEnd);
    if (ShouldPrintSizeInfo) {
        __MLCP_WriteSizeInfo<OneLine>(OutFile, FileSize, !OneLine);
    }

    __MLCP_WritePastEOFWarning(OutFile, FileRange, FileOffEnd, OneLine, "");
    if constexpr (!OneLine) {
        fprintf(OutFile, "\n%s", LinePrefix);
    } else {
        fputc('\t', OutFile);
    }

    fprintf(OutFile, "Memory:%c", (OneLine) ? ' ' : '\t');

    const auto ShouldPad = (OneLine && Verbose);
    __MLCP_WriteOffsetRange(OutFile,
                            VmAddr,
                            VmSize,
                            ShouldPad,
                            static_cast<VmAddrType *>(nullptr));

    if (ShouldPrintSizeInfo) {
        __MLCP_WriteSizeInfo<OneLine>(OutFile, VmSize, !OneLine);
    }

    if (!OneLine || (OneLine && Verbose)) {
        __MLCP_WriteSizeDiff(OutFile, FileSize, VmSize, ShouldPad);
    }
}

using MLPC_LongestNameKindInfo =
        MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LinkerOptimizationHint>;

constexpr static auto MLPC_LengthOfLongestName =
    static_cast<int>(MLPC_LongestNameKindInfo::Name.length());

template <MachO::LoadCommand::Kind Kind>
static inline
void MachOLoadCommandPrinterWriteKindName(FILE *OutFile, bool Verbose) {
    using KindInfo = MachO::LoadCommandKindInfo<Kind>;

    constexpr auto LCKindLength = KindInfo::Name.length();
    static_assert(LCKindLength <= MLPC_LengthOfLongestName,
                  "LoadCommand Kind has a longer name than current "
                  "record-holder");

    fprintf(OutFile,
            "%" PRINTF_RIGHTPAD_FMT "s",
            MLPC_LengthOfLongestName,
            KindInfo::Name.data());

    if (Verbose) {
        fputs("\t\t\t\t", OutFile);
    }
}

template <MachO::LoadCommand::Kind Kind,
          typename SegmentCommand =
            typename MachO::LoadCommandTypeFromKind<Kind>,
          bool SegmentIs64Bit =
            std::is_same_v<SegmentCommand, MachO::SegmentCommand64>>

static inline void
MachOLoadCommandPrinterWriteSegmentCommand(FILE *OutFile,
                                           const RelativeRange &FileRange,
                                           const SegmentCommand &Segment,
                                           bool IsBigEndian,
                                           bool Is64Bit,
                                           bool Verbose) noexcept
{
    const auto SegFileOff = Segment.getFileOffset(IsBigEndian);
    const auto SegFileSize = Segment.getFileSize(IsBigEndian);

    const auto SegVmAddr = Segment.getVmAddr(IsBigEndian);
    const auto SegVmSize = Segment.getVmSize(IsBigEndian);
    const auto SectionsCount = Segment.getSectionCount(IsBigEndian);

    MachOLoadCommandPrinterWriteKindName<Kind>(OutFile, Verbose);
    if (SegmentIs64Bit != Is64Bit) {
        fputs("(Wrong Segment-Kind)", OutFile);
    }

    fprintf(OutFile, "\t\"" CHAR_ARR_FMT(16) "\"", Segment.Name);
    fprintf(OutFile,
            "\t" MEM_PROT_FMT "/" MEM_PROT_FMT "\n",
            MEM_PROT_FMT_ARGS(Segment.getInitProt(IsBigEndian)),
            MEM_PROT_FMT_ARGS(Segment.getMaxProt(IsBigEndian)));

    MachOLoadCommandPrinterWriteFileAndVmRange<false>(OutFile,
                                                      FileRange,
                                                      SegFileOff,
                                                      SegVmAddr,
                                                      SegFileSize,
                                                      SegVmSize,
                                                      Verbose,
                                                      "\t");

    fputs("\n\t", OutFile);
    if (SectionsCount == 0) {
        fputs("0 Sections\n", OutFile);
        return;
    }

    fprintf(OutFile, "%" PRIu32 " Sections:\n", SectionsCount);
    for (const auto &Section : Segment.GetConstSectionList(IsBigEndian)) {
        const auto SectVmAddr = Section.getAddr(IsBigEndian);
        const auto SectSize = Section.getSize(IsBigEndian);

        fputs("\t\t", OutFile);

        const auto SectOffset = Section.getFileOffset(IsBigEndian);
        MachOLoadCommandPrinterWriteFileAndVmRange<true>(OutFile,
                                                         FileRange,
                                                         SectOffset,
                                                         SectVmAddr,
                                                         SectSize,
                                                         SectSize,
                                                         Verbose);

        // If we're on Verbose, the padding added by *WriteFileAndVmRange()
        // extends well past 120 columns.
        // Restrict columns on verbose by not skippping to the next tab-stop.

        if (Verbose) {
            fputc(' ', OutFile);
        } else {
            fputc('\t', OutFile);
        }

        fprintf(OutFile, "\"" CHAR_ARR_FMT(16) "\"", Section.Name);
        if (const auto Length = strnlen(Section.Name, 16); Length != 16) {
            PrintUtilsPadSpaces(OutFile, static_cast<int>(16 - Length));
        }

        const auto Type = Section.getFlags(IsBigEndian).getKind();
        if (Type != MachO::SegmentSectionKind::Regular) {
            // Print Section-Types with space-padding to align all of them in
            // the terminal.

            const auto Desc = MachO::SegmentSectionKindGetDescription(Type);
            fprintf(OutFile, " (%s)", Desc.data());
        }

        fputc('\n', OutFile);
    }
}

template <MachO::LoadCommand::Kind Kind>
struct MachOLoadCommandPrinter {};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Segment> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Segment>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Segment,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteSegmentCommand<LCKindInfo::Kind>(
            OutFile, FileRange, Segment,IsBigEndian, Is64Bit, Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Segment64> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::Segment64>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Segment,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteSegmentCommand<LCKindInfo::Kind>(
            OutFile, FileRange, Segment, IsBigEndian, Is64Bit, Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SymbolTable> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SymbolTable>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SymTab,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        const auto StrOff = SymTab.getStringTableOffset(IsBigEndian);
        const auto StrSize = SymTab.getStringTableSize(IsBigEndian);

        const auto SymOff = SymTab.getSymbolTableOffset(IsBigEndian);
        const auto Nsyms = SymTab.getSymbolCount(IsBigEndian);

        auto SymOverflows = false;
        auto SymEnd = uint64_t();

        if (Is64Bit) {
            if (DoesMultiplyAndAddOverflow(
                    sizeof(MachO::SymTabCommand::Entry64), Nsyms, StrOff,
                    &SymEnd))
            {
                SymOverflows = true;
            }
        } else {
            if (DoesMultiplyAndAddOverflow<uint32_t>(
                    sizeof(MachO::SymTabCommand::Entry32), Nsyms, StrOff,
                    &SymEnd))
            {
                SymOverflows = true;
            }
        }

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputs("\n\tString-Table: ", OutFile);

        auto StrTabEnd = uint64_t();
        auto SymTabEnd = uint64_t();

        PrintUtilsWriteSizeRange32(OutFile,
                                   Is64Bit,
                                   StrOff,
                                   StrSize,
                                   &StrTabEnd);

        fprintf(OutFile, " (%" PRIu32 " Bytes)", StrSize);
        PrintUtilsWriteFormattedSize(OutFile, StrSize, " (", ")");

        __MLCP_WritePastEOFWarning(OutFile, FileRange, StrTabEnd, false, "\n");
        fputs("\tSymbol-Table: ", OutFile);

        if (SymOverflows) {
            PrintUtilsWriteOffset32OverflowsRange(OutFile, SymOff);
        } else {
            PrintUtilsWriteOffsetRange32(OutFile, SymOff, SymEnd, Is64Bit);
        }

        fprintf(OutFile, " (%" PRIu32 " Symbols)", Nsyms);
        __MLCP_WritePastEOFWarning(OutFile, FileRange, SymTabEnd, false, "\n");
    }
};

template <bool PrintEOF>
static inline void
__MLCP_WriteLCOffsetSizePair(FILE *OutFile,
                             const RelativeRange &FileRange,
                             uint32_t Offset,
                             uint32_t Size,
                             bool Is64Bit,
                             uint64_t *EndOut)
{
    fprintf(OutFile,
            "Offset: " OFFSET_32_FMT ", "
            "Size: %" PRINTF_LEFTPAD_WITH_SIZE_FMT(6) PRIu32,
            Offset,
            Size);

    if (Size == 0) {
        return;
    }

    auto End = uint64_t();

    PrintUtilsWriteSizeRange32(OutFile, Is64Bit, Offset, Size, &End, " (", ")");
    PrintUtilsWriteFormattedSize(OutFile, Size, " (", ")");

    if constexpr (PrintEOF) {
        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, false);
    }

    if (EndOut != nullptr) {
        *EndOut = End;
    }
}

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SymbolSegment> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SymbolSegment>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SymSeg,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\t', OutFile);

        const auto Offset = SymSeg.getOffset(IsBigEndian);
        const auto Size = SymSeg.getSize(IsBigEndian);

        __MLCP_WriteLCOffsetSizePair<true>(OutFile, FileRange, Offset, Size,
                                           Is64Bit, nullptr);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Thread> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Thread>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Thread,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::UnixThread> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::UnixThread>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &UnixThread,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\n', OutFile);
    }
};

static inline void
MachOLoadCommandPrinterWriteLoadCommandString(
    FILE *OutFile,
    const MachO::LoadCommandString::GetValueResult &Value)
{
    switch (Value.getError()) {
        case MachO::LoadCommandString::GetStringError::None:
            fprintf(OutFile, "\"%s\"", Value.getString().data());
            break;
        case MachO::LoadCommandString::GetStringError::OffsetOutOfBounds:
            fprintf(OutFile, "(Name-Offset out-of-bounds)");
            break;
        case MachO::LoadCommandString::GetStringError::NoNullTerminator:
            fprintf(OutFile, "(No Null-Terminator)");
            break;
        case MachO::LoadCommandString::GetStringError::EmptyString:
            fprintf(OutFile, "(Empty-String)");
            break;
    }
}

static inline void
MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
    FILE *OutFile,
    const MachO::LoadCommandString::GetValueResult &Value)
{
    fputc('\t', OutFile);
    MachOLoadCommandPrinterWriteLoadCommandString(OutFile, Value);
}

template <>
struct MachOLoadCommandPrinter<
    MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &FvmLib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            FvmLib.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &FvmLib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            FvmLib.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Ident> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Ident>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &FvmLib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::FixedVMFile> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::FixedVMFile>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &FvmFile,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            FvmFile.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::PrePage> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::PrePage>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &FvmFile,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DynamicSymbolTable> {
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DynamicSymbolTable>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DySymtab,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);

        const auto LocalSymbolsIndex =
            DySymtab.getLocalSymbolsIndex(IsBigEndian);
        const auto LocalSymbolsCount =
            DySymtab.getLocalSymbolsCount(IsBigEndian);
        const auto ExternalSymbolsIndex =
            DySymtab.getExternalSymbolsIndex(IsBigEndian);
        const auto ExternalSymbolsCount =
            DySymtab.getExternalSymbolsCount(IsBigEndian);
        const auto UndefinedSymbolsIndex =
            DySymtab.getUndefinedSymbolsIndex(IsBigEndian);
        const auto UndefinedSymbolsCount =
            DySymtab.getUndefinedSymbolsCount(IsBigEndian);

        if (LocalSymbolsCount != 0) {
            fprintf(OutFile,
                    "\n\t%5d Local Symbols At Index     %" PRIu32,
                    LocalSymbolsCount,
                    LocalSymbolsIndex);
        } else {
            fprintf(OutFile, "\n\t%-2s No Local Symbols", "");
        }

        if (ExternalSymbolsCount != 0) {
            fprintf(OutFile,
                    "\n\t%5d External Symbols At Index  %" PRIu32,
                    ExternalSymbolsCount,
                    ExternalSymbolsIndex);
        } else {
            fprintf(OutFile, "\n\t%-2s No External Symbols", "");
        }

        if (UndefinedSymbolsCount != 0) {
            fprintf(OutFile,
                    "\n\t%5d Undefined Symbols At Index %" PRIu32,
                    UndefinedSymbolsCount,
                    UndefinedSymbolsIndex);
        } else {
            fprintf(OutFile, "\n\t%-2s No Undefined Symbols", "");
        }

        fputc('\n', OutFile);
    }
};

template <typename LCKindInfo>
static inline void
MachOLoadCommandPrinterWriteDylibCommand(FILE *OutFile,
                                         const typename LCKindInfo::Type &Dylib,
                                         bool IsBigEndian,
                                         bool Verbose) noexcept
{
    static_assert(MachO::DylibCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a DylibCommand");

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile, Verbose);
    MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
        OutFile,
        Dylib.GetName(IsBigEndian));

    fputc('\n', OutFile);

    if (Verbose) {
        MachOTypePrinter<struct MachO::DylibCommand::Info>::Print(OutFile,
            Dylib.Info, IsBigEndian, false, "\t");
    }
}

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LoadDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDylibCommand<LCKindInfo>(OutFile,
                                                             Dylib,
                                                             IsBigEndian,
                                                             Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::IdDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::IdDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDylibCommand<LCKindInfo>(OutFile,
                                                             Dylib,
                                                             IsBigEndian,
                                                             Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LoadDylinker>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadDylinker>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylinker,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            Dylinker.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::IdDylinker>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::IdDylinker>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylinker,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            Dylinker.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::PreBoundDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::PreBoundDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            Dylib.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Routines>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Routines>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DySymtab,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Routines64>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::Routines64>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DySymtab,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SubFramework>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SubFramework>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SubFramework,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            SubFramework.GetUmbrella(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SubUmbrella>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SubUmbrella>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SubUmbrella,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            SubUmbrella.GetUmbrella(IsBigEndian));
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SubClient>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SubClient>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SubClient,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            SubClient.GetClient(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SubLibrary>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SubLibrary>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SubLibrary,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            SubLibrary.GetLibrary(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::TwoLevelHints>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::TwoLevelHints>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &TwoLevelHints,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        const auto Offset = TwoLevelHints.getHintsOffset(IsBigEndian);
        const auto NHints = TwoLevelHints.getHintsCount(IsBigEndian);
        const auto HintSize = sizeof(MachO::TwoLevelHintsCommand::Hint);

        auto End = uint64_t();
        auto Overflows = false;

        if (Is64Bit) {
            if (DoesMultiplyAndAddOverflow(HintSize, NHints, Offset, &End)) {
                Overflows = true;
            }
        } else {
            if (DoesMultiplyAndAddOverflow<uint32_t>(HintSize, NHints, Offset,
                                                     &End))
            {
                Overflows = false;
            }
        }

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        if (Overflows) {
            PrintUtilsWriteOffset32OverflowsRange(OutFile, Offset, "\t");
        } else {
            PrintUtilsWriteOffsetRange32(OutFile, Offset, End, Is64Bit, "\t");
        }

        fprintf(OutFile, "%" PRIu32 " Hints\n", NHints);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::PrebindChecksum>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::PrebindChecksum>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &PrebindLC,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);

        const auto CheckSum = PrebindLC.getCheckSum(IsBigEndian);
        fprintf(OutFile, "\tCheckSum: %0x08x\n", CheckSum);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Uuid>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Uuid>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Uuid,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);

        fputc('\t', OutFile);
        PrintUtilsWriteUuid(OutFile, Uuid.Uuid, "", "\n");
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LoadWeakDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadWeakDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDylibCommand<LCKindInfo>(OutFile,
                                                             Dylib,
                                                             IsBigEndian,
                                                             Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Rpath>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Rpath>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Rpath,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            Rpath.GetPath(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <typename LCKindInfo, bool WriteEOF>
static inline void
MachOLoadCommandPrinterWriteLinkeditCmd(
    FILE *OutFile,
    const RelativeRange &FileRange,
    const typename LCKindInfo::Type &LoadCmd,
    bool IsBigEndian,
    bool Is64Bit,
    bool Verbose,
    uint64_t *EndOut)
{
    static_assert(MachO::LinkeditDataCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a LinkeditDataCommand");

    const auto Offset = LoadCmd.getDataOffset(IsBigEndian);
    const auto Size = LoadCmd.getDataSize(IsBigEndian);

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile, Verbose);
    fputc('\t', OutFile);

    __MLCP_WriteLCOffsetSizePair<WriteEOF>(OutFile, FileRange, Offset, Size,
                                           Is64Bit, EndOut);
}

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::CodeSignature>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::CodeSignature>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &CodeSignature,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, true>(
            OutFile, FileRange, CodeSignature, IsBigEndian, Is64Bit, Verbose,
            nullptr);

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SegmentSplitInfo>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SegmentSplitInfo>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &SegmentSplitInfo,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, true>(
            OutFile, FileRange, SegmentSplitInfo, IsBigEndian, Is64Bit, Verbose,
            nullptr);

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::ReexportDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::ReexportDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDylibCommand<LCKindInfo>(OutFile,
                                                             Dylib,
                                                             IsBigEndian,
                                                             Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LazyLoadDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LazyLoadDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDylibCommand<LCKindInfo>(OutFile,
                                                             Dylib,
                                                             IsBigEndian,
                                                             Verbose);
    }
};

template <typename EncryptionInfoCommand>
static void
MachOLoadCommandPrinterWriteEncryptionInfoCmd(
    FILE *OutFile,
    const RelativeRange &FileRange,
    const EncryptionInfoCommand &Info,
    bool IsBigEndian,
    bool Is64Bit) noexcept
{
    const auto Offset = Info.getCryptOffset(IsBigEndian);
    const auto Size = Info.getCryptSize(IsBigEndian);
    const auto CryptID = Info.getCryptId(IsBigEndian);

    fprintf(OutFile,
            "\tCryptOff: " OFFSET_32_FMT ", CryptSize: %" PRIu32 " (",
            Offset, Size);

    auto End = uint64_t();

    PrintUtilsWriteSizeRange32(OutFile, Is64Bit, Offset, Size, &End, "", "), ");
    fprintf(OutFile, "CryptID: %" PRIu32, CryptID);

    __MLCP_WritePastEOFWarning(OutFile, FileRange, End, false, "\n");
}

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::EncryptionInfo>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::EncryptionInfo>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &EncryptionInfo,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteEncryptionInfoCmd(
            OutFile, FileRange, EncryptionInfo, IsBigEndian, Is64Bit);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::EncryptionInfo64>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::EncryptionInfo64>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &EncryptionInfo,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        MachOLoadCommandPrinterWriteEncryptionInfoCmd(
            OutFile, FileRange, EncryptionInfo, IsBigEndian, Is64Bit);
    }
};

static constexpr const char DyldInfoRebaseName[] = "Rebase Info";
static constexpr const char DyldInfoBindName[] = "Bind Info";
static constexpr const char DyldInfoLazyBindName[] = "Lazy Bind Info";
static constexpr const char DyldInfoWeakBindName[] = "Weak Bind Info";
static constexpr const char DyldInfoExportName[] = "Export Info";

static constexpr const auto DyldInfoRebaseNameLength =
    LENGTH_OF(DyldInfoRebaseName);

static constexpr const auto DyldInfoBindNameLength =
    LENGTH_OF(DyldInfoBindName);

static constexpr const auto DyldInfoWeakBindNameLength =
    LENGTH_OF(DyldInfoWeakBindName);

static constexpr const auto DyldInfoLazyBindNameLength =
    LENGTH_OF(DyldInfoLazyBindName);

static constexpr const auto DyldInfoExportNameLength =
    LENGTH_OF(DyldInfoExportName);

template <const char Name[], int NameLength>
static inline void
PrintDyldInfoField(FILE *OutFile,
                   const RelativeRange &FileRange,
                   bool Is64Bit,
                   uint32_t Offset,
                   uint32_t Size)
{
    constexpr static const auto Prefix = std::string_view("No ");
    constexpr static const auto LongestNameLength =
        DyldInfoWeakBindNameLength;

    static_assert(NameLength <= LongestNameLength,
                  "DyldInfo Name has longer length than current record-holder");

    fputc('\t', OutFile);

    if (Size != 0) {
        auto End = uint64_t();
        static const auto RightPadSize =
            static_cast<int>(LongestNameLength - NameLength);

        fprintf(OutFile,
                "%" PRINTF_LEFTPAD_FMT "s"
                "%" PRINTF_LEFTPAD_FMT "s"
                "%s: "
                "%5" PRIu32 " Bytes at Offset " OFFSET_32_FMT " (",
                static_cast<int>(Prefix.length()),
                "",
                RightPadSize,
                "",
                Name,
                Size,
                Offset);

        PrintUtilsWriteSizeRange32(OutFile,
                                   Is64Bit,
                                   Offset,
                                   Size,
                                   &End,
                                   "",
                                   ")");

        PrintUtilsWriteFormattedSize(OutFile, Size, " (", ")");
        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, false, "\n");
    } else {
        fprintf(OutFile, "%s%s\n", Prefix.data(), Name);
    }
}

template <typename LCKindInfo>
static inline void
MachOLoadCommandPrinterWriteDyldInfoCmd(
    FILE *OutFile,
    const RelativeRange &FileRange,
    const typename LCKindInfo::Type &DyldInfo,
    bool IsBigEndian,
    bool Is64Bit,
    bool Verbose) noexcept
{
    static_assert(MachO::DyldInfoCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a DyldInfoCommand");

    const auto RebaseOff = DyldInfo.getRebaseOffset(IsBigEndian);
    const auto RebaseSize = DyldInfo.getRebaseSize(IsBigEndian);

    const auto BindOff = DyldInfo.getBindOffset(IsBigEndian);
    const auto BindSize = DyldInfo.getBindSize(IsBigEndian);

    const auto WeakBindOff = DyldInfo.getWeakBindOffset(IsBigEndian);
    const auto WeakBindSize = DyldInfo.getWeakBindSize(IsBigEndian);

    const auto LazyBindOff = DyldInfo.getLazyBindOffset(IsBigEndian);
    const auto LazyBindSize = DyldInfo.getLazyBindSize(IsBigEndian);

    const auto ExportOff = DyldInfo.getExportOffset(IsBigEndian);
    const auto ExportSize = DyldInfo.getExportSize(IsBigEndian);

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile, Verbose);
    fputc('\n', OutFile);

    PrintDyldInfoField<DyldInfoRebaseName, DyldInfoRebaseNameLength>(
        OutFile, FileRange, Is64Bit, RebaseOff, RebaseSize);

    PrintDyldInfoField<DyldInfoBindName, DyldInfoBindNameLength>(
        OutFile, FileRange, Is64Bit, BindOff, BindSize);

    PrintDyldInfoField<DyldInfoWeakBindName, DyldInfoWeakBindNameLength>(
        OutFile, FileRange, Is64Bit, WeakBindOff, WeakBindSize);

    PrintDyldInfoField<DyldInfoLazyBindName, DyldInfoLazyBindNameLength>(
        OutFile, FileRange, Is64Bit, LazyBindOff, LazyBindSize);

    PrintDyldInfoField<DyldInfoExportName, DyldInfoExportNameLength>(
        OutFile, FileRange, Is64Bit, ExportOff, ExportSize);
}

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DyldInfo>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::DyldInfo>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DyldInfo,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDyldInfoCmd<LCKindInfo>(
            OutFile, FileRange, DyldInfo, IsBigEndian, Is64Bit, Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DyldInfoOnly>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DyldInfoOnly>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DyldInfo,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDyldInfoCmd<LCKindInfo>(
            OutFile, FileRange, DyldInfo, IsBigEndian, Is64Bit, Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LoadUpwardDylib>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadUpwardDylib>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteDylibCommand<LCKindInfo>(OutFile,
                                                             Dylib,
                                                             IsBigEndian,
                                                             Verbose);
    }
};

template <typename LCKindInfo>
static inline void
MachOLoadCommandPrinterWriteVersionMinimumCmd(
    FILE *OutFile,
    const typename LCKindInfo::Type &VersionMin,
    bool IsBigEndian,
    bool Verbose) noexcept
{
    static_assert(MachO::VersionMinimumCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a VersionMinimumCommand");

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile, Verbose);
    fputs("\tVersion: ", OutFile);

    MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
        OutFile, VersionMin.getVersion(IsBigEndian));

    fputs("\tSDK: ", OutFile);

    MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
        OutFile, VersionMin.getSdk(IsBigEndian));

    fputc('\n', OutFile);
}

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::VersionMinimumMacOSX>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::VersionMinimumMacOSX>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &VersionMin,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteVersionMinimumCmd<LCKindInfo>(OutFile,
                                                                  VersionMin,
                                                                  IsBigEndian,
                                                                  Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::VersionMinimumIPhoneOS>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::VersionMinimumIPhoneOS>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &VersionMin,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteVersionMinimumCmd<LCKindInfo>(OutFile,
                                                                  VersionMin,
                                                                  IsBigEndian,
                                                                  Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::FunctionStarts>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::FunctionStarts>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &FuncStarts,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        auto End = uint64_t();
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, false>(
            OutFile, FileRange, FuncStarts, IsBigEndian, Is64Bit, Verbose,
            &End);

        const auto Size = FuncStarts.getDataSize(IsBigEndian);
        auto FunctionsCount = uint64_t();

        if (Is64Bit) {
            if (Size & 7ull) {
                fputs(" (Invalid Size)\n", OutFile);
            } else {
                FunctionsCount = (Size / sizeof(uint64_t));
                fprintf(OutFile, " (%" PRIu64 " Functions)", FunctionsCount);
            }
        } else {
            if (Size & 3ull) {
                fputs(" (Invalid Size)\n", OutFile);
            } else {
                FunctionsCount = (Size / sizeof(uint32_t));
                fprintf(OutFile, " (%" PRIu64 " Functions)", FunctionsCount);
            }
        }

        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, false, "\n");
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DyldEnvironment>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DyldEnvironment>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DyldEnvironment,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);

        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile,
            DyldEnvironment.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Main>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Main>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Main,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        const auto EntryOffset = Main.getEntryOffset(IsBigEndian);
        const auto StackSize = Main.getStackSize(IsBigEndian);

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fprintf(OutFile,
                "\tEntry-Offset: " OFFSET_64_FMT ", Stack-Size: %" PRIu64 "\n",
                EntryOffset,
                StackSize);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DataInCode>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DataInCode>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DataInCode,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, true>(
            OutFile, FileRange, DataInCode, IsBigEndian, Is64Bit, Verbose,
            nullptr);

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::SourceVersion>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::SourceVersion>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &DataInCode,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fputs("\tSource Version:\t", OutFile);

        const auto Version = DataInCode.getVersion(IsBigEndian);
        if (Version.empty()) {
            fputs("0\n", OutFile);
            return;
        }

        fprintf(OutFile,
                "%" PRIu32 ".%" PRIu16 ".%" PRIu16 ".%" PRIu16 ".%" PRIu16 "\n",
                Version.getMajor(),
                Version.getMinor(),
                Version.getRevision1(),
                Version.getRevision2(),
                Version.getRevision3());
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DylibCodeSignDRS>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DylibCodeSignDRS>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, true>(
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, Verbose, nullptr);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LinkerOption>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LinkerOption>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &LinkerOpt,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        const auto Offset = LinkerOpt.getOffset(IsBigEndian);
        const auto Size = LinkerOpt.getSize(IsBigEndian);

        __MLCP_WriteLCOffsetSizePair<true>(OutFile, FileRange, Offset, Size,
                                           Is64Bit, nullptr);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::LinkerOptimizationHint>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LinkerOptimizationHint>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, false>(
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, Verbose, nullptr);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::VersionMinimumTvOS>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::VersionMinimumTvOS>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &VersionMin,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteVersionMinimumCmd<LCKindInfo>(OutFile,
                                                                  VersionMin,
                                                                  IsBigEndian,
                                                                  Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::VersionMinimumWatchOS>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::VersionMinimumWatchOS>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &VersionMin,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteVersionMinimumCmd<LCKindInfo>(OutFile,
                                                                  VersionMin,
                                                                  IsBigEndian,
                                                                  Verbose);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::Note>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<MachO::LoadCommand::Kind::Note>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Note,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        const auto Offset = Note.getOffset(IsBigEndian);
        const auto Size = Note.getSize(IsBigEndian);

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fprintf(OutFile,
                "\tOwner: " CHAR_ARR_FMT(16) ", Offset: " OFFSET_64_FMT
                ", Size: %" PRIu64 " (",
                Note.DataOwner,
                Offset,
                Size);

        auto End = uint64_t();
        PrintUtilsWriteSizeRange64(OutFile,
                                   Is64Bit,
                                   Offset,
                                   Size,
                                   &End,
                                   "",
                                   ")");

        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, false, "\n");
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::BuildVersion>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::BuildVersion>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &BuildVersion,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        const auto Platform = BuildVersion.getPlatform(IsBigEndian);
        auto PlatformDesc = MachO::PlatformKindGetDescription(Platform).data();

        if (PlatformDesc == nullptr) {
            PlatformDesc = "<Unrecognized>";
        }

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile,
                                                               Verbose);
        fprintf(OutFile,
                "\n\tPlatform: %s\n"
                "\tMinimum OS Version: ",
                PlatformDesc);

        MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
            OutFile, BuildVersion.getMinOS(IsBigEndian));

        fputs("\n\tSdk: ", OutFile);

        MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
            OutFile, BuildVersion.getSdk(IsBigEndian));

        const auto &ToolList = BuildVersion.getConstToolList(IsBigEndian);
        const auto ToolCount = BuildVersion.getToolCount(IsBigEndian);

        switch (ToolList.getError()) {
            case MachO::SizeRangeError::None:
            case MachO::SizeRangeError::Empty:
                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fprintf(OutFile,
                        "Error: Tool-Count (%d) too high for LoadCommand "
                        "CmdSize",
                        ToolCount);
                return;
        }

        fputs("\n\t", OutFile);
        switch (ToolCount) {
            case 0:
                fputs("0 Tools\n", OutFile);
                return;
            case 1:
                fputs("1 Tool:\n", OutFile);
                break;
            default:
                fprintf(OutFile, "%" PRIu32 " Tools:\n", ToolCount);
                break;
        }

        for (const auto &Tool : ToolList.getRef()) {
            const auto ToolKind = Tool.getKind(IsBigEndian);
            auto KindDesc = Tool.KindGetDescription(ToolKind).data();

            if (KindDesc == nullptr) {
                KindDesc = "Unrecognized";
            }

            fprintf(OutFile, "\t\tKind:    %s\n\t\tVersion: ", KindDesc);
            MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(OutFile,
                Tool.getVersion(IsBigEndian), "", "");

            fputc('\n', OutFile);
        }
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DyldExportsTrie>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DyldExportsTrie>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, true>(
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, Verbose, nullptr);
        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<MachO::LoadCommand::Kind::DyldChainedFixups>
{
    using LCKindInfo =
        typename MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::DyldChainedFixups>;

    using LCType = LCKindInfo::Type;

    static void
    Print(FILE *OutFile,
          const RelativeRange &FileRange,
          const LCType &Dylib,
          bool IsBigEndian,
          bool Is64Bit,
          bool Verbose) noexcept
    {
        MachOLoadCommandPrinterWriteLinkeditCmd<LCKindInfo, true>(
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, Verbose, nullptr);
        fputc('\n', OutFile);
    }
};
