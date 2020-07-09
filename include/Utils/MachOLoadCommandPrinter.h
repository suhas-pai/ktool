//
//  src/Utils/MachOLoadCommandPrinter.h
//  stool
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
                WrittenOut = PrintUtilsWriteOffsetRange(OutFile, Offset, End);
            } else {
                WrittenOut =
                    PrintUtilsWriteOffset32To64Range(OutFile, Offset, End);
            }
        } else {
            WrittenOut = PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
        }
    } else {
        if (!DidOverflow) {
            WrittenOut = PrintUtilsWriteOffsetRange(OutFile, Offset, End);
        } else {
            WrittenOut = PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
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
                  "SizeType must be an integer-type");

    constexpr static const auto WriteLengthMax = LENGTH_OF(" (+000000");
    constexpr static const auto WriteMax =
        WriteLengthMax + LENGTH_OF(" Bytes)");

    if (SizeOne == SizeTwo) {
        if (Pad) {
            PrintUtilsPadSpaces(OutFile, WriteMax);
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

        PrintUtilsPadSpaces(OutFile, PadSpacesLength);
    }

    if constexpr (std::is_same_v<SizeOneType, uint64_t>) {
        WrittenOut = fprintf(OutFile, " (%c%" PRIu64, Sign, Diff);
    } else {
        WrittenOut = fprintf(OutFile, " (%c%" PRIu32, Sign, Diff);

    }

    WrittenOut += fputs(" Bytes)", OutFile);
    return WrittenOut;
}

template <bool Pad, typename SizeType>
static inline int
__MLCP_WriteSizeInfo(FILE *OutFile, const SizeType &Size) noexcept {
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
    return WrittenOut;
}

static inline void
__MLCP_WritePastEOFWarning(FILE *OutFile,
                           const RelativeRange &FileRange,
                           uint64_t End,
                           const char *LineSuffix) noexcept
{
    if (!FileRange.containsEndLocation(End)) {
        fprintf(OutFile, " (Past EOF!)%s", LineSuffix);
    } else {
        fprintf(OutFile, "%s", LineSuffix);
    }
}

template <bool OneLine,
          typename FileOffType,
          typename VmAddrType,
          typename SizeType>

static inline void
MachOLoadCommandPrinterWriteFileAndVmRange(FILE *OutFile,
                                           const RelativeRange &FileRange,
                                           const char *LinePrefix,
                                           FileOffType FileOff,
                                           VmAddrType VmAddr,
                                           SizeType FileSize,
                                           SizeType VmSize,
                                           bool Verbose) noexcept
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
        __MLCP_WriteSizeInfo<OneLine>(OutFile, FileSize);
    }

    __MLCP_WritePastEOFWarning(OutFile, FileRange, FileOffEnd, "");

    if constexpr (OneLine) {
        fputc('\t', OutFile);
    } else {
        fprintf(OutFile, "\n%s", LinePrefix);
    }

    fprintf(OutFile, "Memory:%c", (OneLine) ? ' ' : '\t');

    const auto ShouldPad = (OneLine && Verbose);
    __MLCP_WriteOffsetRange(OutFile,
                            VmAddr,
                            VmSize,
                            ShouldPad,
                            static_cast<VmAddrType *>(nullptr));

    if (ShouldPrintSizeInfo) {
        __MLCP_WriteSizeInfo<OneLine>(OutFile, VmSize);
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
static inline void MachOLoadCommandPrinterWriteKindName(FILE *OutFile) {
    using KindInfo = MachO::LoadCommandKindInfo<Kind>;

    constexpr auto LCKindLength = KindInfo::Name.length();
    static_assert(LCKindLength <= MLPC_LengthOfLongestName,
                  "LoadCommand Kind has a longer name than current "
                  "record-holder");

    fprintf(OutFile,
            "%" PRINTF_RIGHTPAD_FMT "s",
            MLPC_LengthOfLongestName,
            KindInfo::Name.data());
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
    const auto SegFileOff = SwitchEndianIf(Segment.FileOff, IsBigEndian);
    const auto SegFileSize = SwitchEndianIf(Segment.FileSize, IsBigEndian);

    const auto SegVmAddr = SwitchEndianIf(Segment.VmAddr, IsBigEndian);
    const auto SegVmSize = SwitchEndianIf(Segment.VmSize, IsBigEndian);
    const auto SegNsects = SwitchEndianIf(Segment.Nsects, IsBigEndian);

    MachOLoadCommandPrinterWriteKindName<Kind>(OutFile);
    if (SegmentIs64Bit != Is64Bit) {
        fputs("(Wrong Segment-Kind)", OutFile);
    }

    fprintf(OutFile, "\t\"" CHAR_ARR_FMT(16) "\"", Segment.Name);
    fprintf(OutFile,
            "\t" MEM_PROT_FMT "/" MEM_PROT_FMT "\n",
            MEM_PROT_FMT_ARGS(Segment.getInitProt(IsBigEndian)),
            MEM_PROT_FMT_ARGS(Segment.getInitProt(IsBigEndian)));

    MachOLoadCommandPrinterWriteFileAndVmRange<false>(
        OutFile, FileRange, "\t", SegFileOff, SegVmAddr, SegFileSize, SegVmSize,
        Verbose);

    fputs("\n\t", OutFile);
    if (SegNsects == 0) {
        fputs("0 Sections\n", OutFile);
        return;
    }

    fprintf(OutFile, "%" PRIu32 " Sections:\n", SegNsects);
    for (const auto &Section : Segment.GetConstSectionList(IsBigEndian)) {
        const auto SectVmAddr = SwitchEndianIf(Section.Addr, IsBigEndian);
        const auto SectSize = SwitchEndianIf(Section.Size, IsBigEndian);

        fputs("\t\t", OutFile);

        const auto SectOffset = SwitchEndianIf(Section.Offset, IsBigEndian);
        MachOLoadCommandPrinterWriteFileAndVmRange<true>(
            OutFile, FileRange, "", SectOffset, SectVmAddr, SectSize, SectSize,
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
        const auto StrOff = SwitchEndianIf(SymTab.StrOff, IsBigEndian);
        const auto StrSize = SwitchEndianIf(SymTab.StrSize, IsBigEndian);

        const auto SymOff = SwitchEndianIf(SymTab.SymOff, IsBigEndian);
        const auto Nsyms = SwitchEndianIf(SymTab.Nsyms, IsBigEndian);

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

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        fputs("\n\tString-Table: ", OutFile);

        auto StrTabEnd = uint64_t();
        auto SymTabEnd = uint64_t();

        PrintUtilsWriteSizeRange(OutFile, Is64Bit, StrOff, StrSize, &StrTabEnd);
        fprintf(OutFile, " (%" PRIu32 " Bytes)", StrSize);

        __MLCP_WritePastEOFWarning(OutFile, FileRange, StrTabEnd, "\n");
        fputs("\tSymbol-Table: ", OutFile);

        if (SymOverflows) {
            PrintUtilsWriteOffsetOverflowsRange(OutFile, SymOff);
        } else {
            PrintUtilsWriteOffsetRange32(OutFile, SymOff, SymEnd, Is64Bit);
        }

        fprintf(OutFile, " (%" PRIu32 " Symbols)", Nsyms);
        __MLCP_WritePastEOFWarning(OutFile, FileRange, SymTabEnd, "\n");
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
            Offset, Size);

    if (Size == 0) {
        return;
    }

    [[maybe_unused]] auto End = uint64_t();

    fputs(" (", OutFile);
    PrintUtilsWriteSizeRange(OutFile, Is64Bit, Offset, Size, &End);
    fputs(")", OutFile);

    if constexpr (PrintEOF) {
        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, "");
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        fputc('\t', OutFile);

        const auto Offset = SwitchEndianIf(SymSeg.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(SymSeg.Size, IsBigEndian);

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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
    FILE *OutFile, const MachO::LoadCommandString::GetValueResult &Value)
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, FvmLib.GetName(IsBigEndian));

        fputc('\n', OutFile);
    }
};

template <>
struct MachOLoadCommandPrinter<
    MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, FvmLib.GetName(IsBigEndian));

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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, FvmFile.GetName(IsBigEndian));

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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);

        const auto LocalSymbolsIndex =
            SwitchEndianIf(DySymtab.ILocalSymbols, IsBigEndian);
        const auto LocalSymbolsCount =
            SwitchEndianIf(DySymtab.NLocalSymbols, IsBigEndian);
        const auto ExternalSymbolsIndex =
            SwitchEndianIf(DySymtab.IExternallyDefinedSymbols, IsBigEndian);
        const auto ExternalSymbolsCount =
            SwitchEndianIf(DySymtab.NExternallyDefinedSymbols, IsBigEndian);
        const auto UndefinedSymbolsIndex =
            SwitchEndianIf(DySymtab.IUndefinedSymbols, IsBigEndian);
        const auto UndefinedSymbolsCount =
            SwitchEndianIf(DySymtab.NUndefinedSymbols, IsBigEndian);

        if (LocalSymbolsCount) {
            fprintf(OutFile,
                    "\n\t%5d Local Symbols At Index     %" PRIu32,
                    LocalSymbolsCount,
                    LocalSymbolsIndex);
        } else {
            fprintf(OutFile, "\n\t%-3s No Local Symbols", "");
        }

        if (ExternalSymbolsCount != 0) {
            fprintf(OutFile,
                    "\n\t%5d External Symbols At Index  %" PRIu32,
                    ExternalSymbolsCount,
                    ExternalSymbolsIndex);
        } else {
            fprintf(OutFile, "\n\t%-3s No External Symbols", "");
        }

        if (UndefinedSymbolsCount != 0) {
            fprintf(OutFile,
                    "\n\t%5d Undefined Symbols At Index %" PRIu32,
                    UndefinedSymbolsCount,
                    UndefinedSymbolsIndex);
        } else {
            fprintf(OutFile, "\n\t%-3s No Undefined Symbols", "");
        }

        fputc('\n', OutFile);
    }
};

template <typename LCKindInfo>
static inline void
MachOLoadCommandPrinterWriteDylibCommand(
    FILE *OutFile,
    const typename LCKindInfo::Type &Dylib,
    bool IsBigEndian,
    bool Verbose)
{
    static_assert(MachO::DylibCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a DylibCommand");

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
    if (Verbose) {
        fputs("\t\t\t", OutFile);
    }

    MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
        OutFile, Dylib.GetName(IsBigEndian));

    fputc('\n', OutFile);

    if (Verbose) {
        MachOTypePrinter<struct MachO::DylibCommand::Info>::Print(OutFile,
            Dylib.Info, IsBigEndian, false, "\t", "");
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(OutFile,
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(OutFile,
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(OutFile,
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, SubFramework.GetUmbrella(IsBigEndian));

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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, SubUmbrella.GetUmbrella(IsBigEndian));
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, SubClient.GetClient(IsBigEndian));

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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, SubLibrary.GetLibrary(IsBigEndian));

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
        const auto Offset = SwitchEndianIf(TwoLevelHints.Offset, IsBigEndian);
        const auto NHints = SwitchEndianIf(TwoLevelHints.NHints, IsBigEndian);
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

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        fputc('\t', OutFile);

        if (Overflows) {
            PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
        } else {
            PrintUtilsWriteOffsetRange32(OutFile, Offset, End, Is64Bit);
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
        const auto CheckSum = SwitchEndianIf(PrebindLC.CheckSum, IsBigEndian);

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        fprintf(OutFile,
                "\t%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X"
                "%02X%02X\n",
                (unsigned int)Uuid.uuid[0], (unsigned int)Uuid.uuid[1],
                (unsigned int)Uuid.uuid[2], (unsigned int)Uuid.uuid[3],
                (unsigned int)Uuid.uuid[4], (unsigned int)Uuid.uuid[5],
                (unsigned int)Uuid.uuid[6], (unsigned int)Uuid.uuid[7],
                (unsigned int)Uuid.uuid[8], (unsigned int)Uuid.uuid[9],
                (unsigned int)Uuid.uuid[10], (unsigned int)Uuid.uuid[11],
                (unsigned int)Uuid.uuid[12], (unsigned int)Uuid.uuid[13],
                (unsigned int)Uuid.uuid[14], (unsigned int)Uuid.uuid[15]);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, Rpath.GetPath(IsBigEndian));

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
    uint64_t *EndOut)
{
    static_assert(MachO::LinkeditDataCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a LinkeditDataCommand");

    const auto Offset = SwitchEndianIf(LoadCmd.DataOff, IsBigEndian);
    const auto Size = SwitchEndianIf(LoadCmd.DataSize, IsBigEndian);

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
            OutFile, FileRange, CodeSignature, IsBigEndian, Is64Bit, nullptr);
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
            OutFile, FileRange, SegmentSplitInfo, IsBigEndian, Is64Bit,
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
    const EncryptionInfoCommand &EncryptionInfo,
    bool IsBigEndian,
    bool Is64Bit) noexcept
{
    const auto Offset = SwitchEndianIf(EncryptionInfo.CryptOff, IsBigEndian);
    const auto Size = SwitchEndianIf(EncryptionInfo.CryptSize, IsBigEndian);
    const auto CryptID =
        SwitchEndianIf(EncryptionInfo.CryptId, IsBigEndian);

    fprintf(OutFile,
            "\tCryptOff: " OFFSET_32_FMT ", CryptSize: %" PRIu32 " (",
            Offset, Size);

    auto End = uint64_t();

    PrintUtilsWriteSizeRange(OutFile, Is64Bit, Offset, Size, &End);
    fprintf(OutFile, "), CryptID: %" PRIu32, CryptID);

    __MLCP_WritePastEOFWarning(OutFile, FileRange, End, "\n");
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
                "%" PRIu32 " Bytes at Offset " OFFSET_32_FMT " (",
                static_cast<int>(Prefix.length()),
                "",
                RightPadSize,
                "",
                Name,
                Size,
                Offset);

        PrintUtilsWriteSizeRange(OutFile, Is64Bit, Offset, Size, &End);
        fputc(')', OutFile);

        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, "\n");
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
    bool Is64Bit) noexcept
{
    static_assert(MachO::DyldInfoCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a DyldInfoCommand");

    const auto RebaseOff = SwitchEndianIf(DyldInfo.RebaseOff, IsBigEndian);
    const auto RebaseSize = SwitchEndianIf(DyldInfo.RebaseSize, IsBigEndian);

    const auto BindOff = SwitchEndianIf(DyldInfo.BindOff, IsBigEndian);
    const auto BindSize = SwitchEndianIf(DyldInfo.BindSize, IsBigEndian);

    const auto WeakBindOff = SwitchEndianIf(DyldInfo.WeakBindOff, IsBigEndian);
    const auto WeakBindSize = SwitchEndianIf(DyldInfo.WeakBindSize, IsBigEndian);

    const auto LazyBindOff = SwitchEndianIf(DyldInfo.LazyBindOff, IsBigEndian);
    const auto LazyBindSize = SwitchEndianIf(DyldInfo.LazyBindSize, IsBigEndian);

    const auto ExportOff = SwitchEndianIf(DyldInfo.ExportOff, IsBigEndian);
    const auto ExportSize = SwitchEndianIf(DyldInfo.ExportSize, IsBigEndian);

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
            OutFile, FileRange, DyldInfo, IsBigEndian, Is64Bit);
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
            OutFile, FileRange, DyldInfo, IsBigEndian, Is64Bit);
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
    bool IsBigEndian) noexcept
{
    static_assert(MachO::VersionMinimumCommand::IsOfKind(LCKindInfo::Kind),
                  "LoadCommand is not a VersionMinimumCommand");

    MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
    fputs("\tVersion: ", OutFile);

    MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
        OutFile, VersionMin.GetVersion(IsBigEndian), "", "");

    fputs("\tSDK: ", OutFile);

    MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
        OutFile, VersionMin.GetSdk(IsBigEndian), "", "");

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
                                                                  IsBigEndian);
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
                                                                  IsBigEndian);
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
            OutFile, FileRange, FuncStarts, IsBigEndian, Is64Bit, &End);

        const auto Size = SwitchEndianIf(FuncStarts.DataSize, IsBigEndian);
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

        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, "\n");
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        MachOLoadCommandPrinterWriteLoadCommandStringAsDesc(
            OutFile, DyldEnvironment.GetName(IsBigEndian));

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
        const auto EntryOffset = SwitchEndianIf(Main.EntryOffset, IsBigEndian);
        const auto StackSize = SwitchEndianIf(Main.StackSize, IsBigEndian);

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
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
            OutFile, FileRange, DataInCode, IsBigEndian, Is64Bit, nullptr);
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
        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);

        const auto Version = DataInCode.GetVersion(IsBigEndian);

        fputs("\tSource Version:\t", OutFile);
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
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, nullptr);
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
        const auto Offset = SwitchEndianIf(LinkerOpt.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(LinkerOpt.Size, IsBigEndian);

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
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, nullptr);
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
                                                                  IsBigEndian);
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
                                                                  IsBigEndian);
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
        const auto Offset = SwitchEndianIf(Note.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(Note.Size, IsBigEndian);

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        fprintf(OutFile,
                "\tOwner: " CHAR_ARR_FMT(16) ", Offset: " OFFSET_64_FMT
                ", Size: %" PRIu64 " (",
                Note.DataOwner, Offset, Size);

        auto End = uint64_t();

        PrintUtilsWriteSizeRange(OutFile, Is64Bit, Offset, Size, &End);
        fputc(')', OutFile);

        __MLCP_WritePastEOFWarning(OutFile, FileRange, End, "\n");
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
        const auto NTools = SwitchEndianIf(BuildVersion.NTools, IsBigEndian);
        const auto Platform = BuildVersion.GetPlatform(IsBigEndian);
        auto PlatformDesc =
            MachO::BuildVersionCommand::PlatformKindGetDescription(
                Platform).data();

        if (PlatformDesc == nullptr) {
            PlatformDesc = "(Unrecognized)";
        }

        MachOLoadCommandPrinterWriteKindName<LCKindInfo::Kind>(OutFile);
        fprintf(OutFile,
                "\n\tPlatform: %s\n"
                "\tMinimum OS Version: ",
                PlatformDesc);

        MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
            OutFile, BuildVersion.GetMinOS(IsBigEndian), "", "");

        fputs("\n\tSdk: ", OutFile);

        MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(
            OutFile, BuildVersion.GetSdk(IsBigEndian), "", "");

        const auto &ToolList = BuildVersion.GetConstToolList(IsBigEndian);
        switch (ToolList.getError()) {
            case MachO::SizeRangeError::None:
            case MachO::SizeRangeError::Empty:
                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fprintf(OutFile,
                        "Error: Tool-Count (%d) too high for LoadCommand "
                        "CmdSize",
                        NTools);
                return;
        }

        switch (NTools) {
            case 0:
                fputs("\n\t0 Tools\n", OutFile);
                return;
            case 1:
                fputs("\n\t1 Tool:\n", OutFile);
                break;
            default:
                fprintf(OutFile, "\n\t%" PRIu32 " Tools:\n", NTools);
                break;
        }

        for (const auto &Tool : ToolList.getRef()) {
            auto KindDesc =
                Tool.KindGetDescription(Tool.GetKind(IsBigEndian)).data();
            
            if (KindDesc == nullptr) {
                KindDesc = "Unrecognized";
            }

            fprintf(OutFile,"\t\tKind:    %s\n\t\tVersion: ", KindDesc);
            MachOTypePrinter<MachO::PackedVersion>::PrintWithoutZeros(OutFile,
                Tool.GetVersion(IsBigEndian), "", "");

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
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, nullptr);
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
            OutFile, FileRange, Dylib, IsBigEndian, Is64Bit, nullptr);
        fputc('\n', OutFile);
    }
};
