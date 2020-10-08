//
//  include/Utils/MachOPrinter.h
//  ktool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/MachO.h"
#include "Timestamp.h"

template <typename OffsetType, typename SizeType>
static inline int
MachOPrintOffsetSizeRange(FILE *OutFile,
                          const OffsetType &Offset,
                          const SizeType &Size,
                          const char *LinePrefix,
                          const char *LineSuffix,
                          bool Is64Bit,
                          bool Pad,
                          OffsetType *EndOut = nullptr) noexcept
{
    static_assert(std::is_integral_v<OffsetType> &&
                  std::is_integral_v<SizeType>,
                  "Types must be an integer-types");

    constexpr auto OffsetTypeIs64Bit = std::is_same_v<OffsetType, uint64_t>;

    auto End = OffsetType();
    auto WrittenOut = int();
    auto DidOverflow = DoesAddOverflow(Offset, Size, &End);

    auto ExpectedWrittenOut =
        (Is64Bit) ?
            (OffsetTypeIs64Bit) ?
                OFFSET_64_RANGE_LEN :
                OFFSET_32_64_RANGE_LEN :
            OFFSET_32_RANGE_LEN;

    if (Is64Bit) {
        if (!DidOverflow) {
            if constexpr (OffsetTypeIs64Bit) {
                WrittenOut =
                    PrintUtilsWriteOffsetRange(OutFile,
                                               Offset,
                                               End,
                                               LinePrefix);
            } else {
                WrittenOut =
                    PrintUtilsWriteOffset32To64Range(OutFile,
                                                     Offset,
                                                     End,
                                                     LinePrefix);
            }
        } else {
            WrittenOut =
                PrintUtilsWriteOffsetOverflowsRange(OutFile,
                                                    Offset,
                                                    LinePrefix);
        }
    } else {
        if (!DidOverflow) {
            WrittenOut =
                PrintUtilsWriteOffsetRange(OutFile, Offset, End, LinePrefix);
        } else {
            WrittenOut =
                PrintUtilsWriteOffsetOverflowsRange(OutFile,
                                                    Offset,
                                                    LinePrefix);
        }
    }

    if (Pad) {
        const auto Pad = static_cast<int>(ExpectedWrittenOut - WrittenOut);
        WrittenOut += PrintUtilsPadSpaces(OutFile, Pad);
    }

    fprintf(OutFile, "%s", LineSuffix);
    if (EndOut != nullptr) {
        *EndOut = End;
    }

    return WrittenOut;
}

template <typename OffsetType, typename EndType>
static inline int
MachOPrintOffsetEndRange(FILE *OutFile,
                         const OffsetType &Offset,
                         const EndType &End,
                         const char *LinePrefix,
                         const char *LineSuffix,
                         bool Is64Bit,
                         bool Pad,
                         OffsetType *EndOut = nullptr) noexcept
{
    static_assert(std::is_integral_v<OffsetType> &&
                  std::is_integral_v<EndType>,
                  "Types must be an integer-types");

    constexpr auto OffsetTypeIs64Bit = std::is_same_v<OffsetType, uint64_t>;
    auto ExpectedWrittenOut =
        (Is64Bit) ?
            (OffsetTypeIs64Bit) ?
                OFFSET_64_RANGE_LEN :
                OFFSET_32_64_RANGE_LEN :
            OFFSET_32_RANGE_LEN;

    fprintf(OutFile, "%s", LinePrefix);

    auto WrittenOut = PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
    if (Pad) {
        const auto Pad = static_cast<int>(ExpectedWrittenOut - WrittenOut);
        WrittenOut += PrintUtilsPadSpaces(OutFile, Pad);
    }

    fprintf(OutFile, "%s", LineSuffix);
    if (EndOut != nullptr) {
        *EndOut = End;
    }

    return WrittenOut;
}

template <typename T>
struct MachOTypePrinter {};

template <>
struct MachOTypePrinter<MachO::FatHeader::Arch32> {
    using Type = typename MachO::FatHeader::Arch32;

    template <bool Verbose>
    static int
    Print(FILE *OutFile,
          const Type &Arch,
          bool IsBigEndian,
          const char *LinePrefix,
          const char *LineSuffix) noexcept
    {
        const auto CpuKind = Arch.getCpuKind(IsBigEndian);
        const auto CpuSubKind = Arch.getCpuSubKind(IsBigEndian);

        auto CpuKindName =
            (Verbose) ?
                Mach::CpuKindGetName(CpuKind).data() :
                Mach::CpuKindGetBrandName(CpuKind).data();

        if (CpuKindName == nullptr) {
            CpuKindName = "Unrecognized";
        }

        auto CpuSubKindName =
            (Verbose) ?
                Mach::CpuSubKind::GetName(CpuKind, CpuSubKind).data() :
                Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind).data();

        if (CpuSubKindName == nullptr) {
            CpuSubKindName = "Unrecognized";
        }

        const auto Offset = Arch.getFileOffset(IsBigEndian);
        const auto Size = Arch.getFileSize(IsBigEndian);
        const auto Align = Arch.getAlign(IsBigEndian);
        const auto ArchEnd = Offset + Size;

        auto WrittenOut = int();
        if constexpr (Verbose) {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s (Value: %" PRIu32 ")%s\n"
                        "%sCpuSubType: %s (Value: %" PRIu32 ")%s\n"
                        "%sOffset:%-5s" OFFSET_32_FMT " (" OFFSET_32_RNG_FMT ")"
                        "%s\n%sSize:       %" PRIu32 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindName, static_cast<int32_t>(CpuKind),
                        LineSuffix,
                        LinePrefix, CpuSubKindName, CpuSubKind, LineSuffix,
                        LinePrefix, "", Offset, Offset, ArchEnd, LineSuffix,
                        LinePrefix, Size, LineSuffix,
                        LinePrefix, Align, LineSuffix);
        } else {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s%s\n"
                        "%sCpuSubType: %s%s\n"
                        "%sOffset:%-5s" OFFSET_32_FMT " (" OFFSET_32_RNG_FMT ")"
                        "%s\n%sSize:       %" PRIu32 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindName, LineSuffix,
                        LinePrefix, CpuSubKindName, LineSuffix,
                        LinePrefix, "", Offset, Offset, ArchEnd, LineSuffix,
                        LinePrefix, Size, LineSuffix,
                        LinePrefix, Align, LineSuffix);
        }

        return WrittenOut;
    }
};

template <>
struct MachOTypePrinter<MachO::FatHeader::Arch64> {
    using Type = typename MachO::FatHeader::Arch64;

    template <bool Verbose>
    static int
    Print(FILE *OutFile,
          const Type &Arch,
          bool IsBigEndian,
          const char *LinePrefix = "",
          const char *LineSuffix = "") noexcept
    {
        const auto CpuKind = Arch.getCpuKind(IsBigEndian);
        const auto CpuSubKind = Arch.getCpuSubKind(IsBigEndian);

        auto CpuKindName =
            (Verbose) ?
                Mach::CpuKindGetName(CpuKind).data() :
                Mach::CpuKindGetBrandName(CpuKind).data();

        if (CpuKindName == nullptr) {
            CpuKindName = "Unrecognized";
        }

        auto CpuSubKindName =
            (Verbose) ?
                Mach::CpuSubKind::GetName(CpuKind, CpuSubKind).data() :
                Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind).data();

        if (CpuSubKindName == nullptr) {
            CpuSubKindName = "Unrecognized";
        }

        const auto Offset = Arch.getFileOffset(IsBigEndian);
        const auto Size = Arch.getFileSize(IsBigEndian);
        const auto Align = Arch.getAlign(IsBigEndian);
        const auto ArchEnd = Offset + Size;

        auto WrittenOut = int();
        if constexpr (Verbose) {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s (Value: %" PRIu32 ")%s\n"
                        "%sCpuSubType: %s (Value: %" PRIu32 ")%s\n"
                        "%sOffset:%-5s" OFFSET_64_FMT " (" OFFSET_64_RNG_FMT
                        ")%s\n%sSize:       %" PRIu64 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindName, static_cast<int32_t>(CpuKind),
                        LineSuffix,
                        LinePrefix, CpuSubKindName, CpuSubKind, LineSuffix,
                        LinePrefix, "", Offset, Offset, ArchEnd, LineSuffix,
                        LinePrefix, Size, LineSuffix,
                        LinePrefix, Align, LineSuffix);
        } else {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s%s\n"
                        "%sCpuSubType: %s%s\n"
                        "%sOffset:%-5s" OFFSET_64_FMT " (" OFFSET_64_RNG_FMT
                        ")%s\n%sSize:       %" PRIu64 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindName, LineSuffix,
                        LinePrefix, CpuSubKindName, LineSuffix,
                        LinePrefix, "", Offset, Offset, ArchEnd, LineSuffix,
                        LinePrefix, Size, LineSuffix,
                        LinePrefix, Align, LineSuffix);
        }

        return WrittenOut;
    }
};

template <>
struct MachOTypePrinter<MachO::FatHeader> {
    using Type = MachO::FatHeader;

    static int
    PrintArchList(FILE *OutFile,
                  const Type &Header,
                  const char *LinePrefix = "",
                  const char *LineSuffix = "") noexcept
    {
        const auto IsBigEndian = Header.isBigEndian();

        auto CombinedLinePrefix = std::string(LinePrefix).append("\t");
        auto WrittenOut = int();
        auto I = static_cast<uint32_t>(1);

        if (Header.is64Bit()) {
            for (const auto &Arch : Header.getConstArch64List()) {
                WrittenOut +=
                    fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);

                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch64>::Print<false>(
                        OutFile, Arch, IsBigEndian, CombinedLinePrefix.data(),
                        LineSuffix);
                I++;
            }
        } else {
            for (const auto &Arch : Header.getConstArch32List()) {
                WrittenOut +=
                    fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);

                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch32>::Print<false>(
                        OutFile, Arch, IsBigEndian, CombinedLinePrefix.data(),
                        LineSuffix);
                I++;
            }
        }

        return WrittenOut;
    }

    static int
    PrintArchListVerbose(FILE *OutFile,
                         const Type &Header,
                         const char *LinePrefix = "",
                         const char *LineSuffix = "") noexcept
    {
        const auto IsBigEndian = Header.isBigEndian();

        auto CombinedLinePrefix = std::string(LinePrefix).append("\t");
        auto WrittenOut = int();
        auto I = static_cast<uint32_t>(1);

        if (Header.is64Bit()) {
            for (const auto &Arch : Header.getConstArch64List()) {
                WrittenOut +=
                    fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);

                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch64>::Print<true>(
                        OutFile, Arch, IsBigEndian, CombinedLinePrefix.data(),
                        LineSuffix);
                I++;
            }
        } else {
            for (const auto &Arch : Header.getConstArch32List()) {
                WrittenOut +=
                    fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);

                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch32>::Print<true>(
                        OutFile, Arch, IsBigEndian, CombinedLinePrefix.data(),
                        LineSuffix);
                I++;
            }
        }

        return WrittenOut;
    }
};

template <>
struct MachOTypePrinter<MachO::PackedVersion> {
    using Type = MachO::PackedVersion;

    template <bool RightPad = false>
    static int
    Print(FILE *OutFile,
          const Type &Version,
          const char *LinePrefix = "",
          const char *LineSuffix = "") noexcept
    {
        auto WrittenOut = fprintf(OutFile, "%s", LinePrefix);
        const auto PackedVersionWrittenOut =
            fprintf(OutFile,
                    MACHO_PACKED_VERSION_FMT,
                    MACHO_PACKED_VERSION_FMT_ARGS(Version));

        WrittenOut += PackedVersionWrittenOut;
        if constexpr (RightPad) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         PackedVersionWrittenOut,
                                         MACHO_PACKED_VERSION_LEN);

            return MACHO_PACKED_VERSION_LEN;
        }

        WrittenOut += fprintf(OutFile, "%s", LineSuffix);
        return WrittenOut;
    }

    template <bool RightPad = false>
    static inline int
    PrintWithoutZeros(FILE *OutFile,
                      const Type &Version,
                      const char *LinePrefix = "",
                      const char *LineSuffix = "")
    {
        const auto Major = Version.getMajor();
        const auto Minor = Version.getMinor();
        const auto Revision = Version.getRevision();

        auto WrittenOut = fprintf(OutFile, "%s", LinePrefix);
        auto PackedVersionWrittenOut = int();

        if (Revision == 0) {
            if (Minor == 0) {
                if (Major == 0) {
                    fputc('0', OutFile);
                    PackedVersionWrittenOut = 1;
                } else {
                    PackedVersionWrittenOut =
                        fprintf(OutFile, "%" PRIu16, Major);
                }
            } else {
                PackedVersionWrittenOut =
                    fprintf(OutFile, "%" PRIu16 ".%" PRIu16, Major, Minor);
            }
        } else {
            PackedVersionWrittenOut =
                fprintf(OutFile,
                        MACHO_PACKED_VERSION_FMT,
                        Major,
                        Minor,
                        Revision);
        }

        WrittenOut += PackedVersionWrittenOut;
        if constexpr (RightPad) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         PackedVersionWrittenOut,
                                         MACHO_PACKED_VERSION_LEN);
        }

        WrittenOut += fprintf(OutFile, "%s", LineSuffix);
        return WrittenOut;
    }

    template <bool RightPad = false>
    static inline int
    PrintOrZero(FILE *OutFile,
                const Type &Version,
                const char *LinePrefix = "",
                const char *LineSuffix = "")
    {
        const auto Major = Version.getMajor();
        const auto Minor = Version.getMinor();
        const auto Revision = Version.getRevision();

        auto WrittenOut = fprintf(OutFile, "%s", LinePrefix);
        auto PackedVersionWrittenOut = int();

        if (Revision == 0) {
            if (Minor == 0) {
                if (Major == 0) {
                    fputc('0', OutFile);
                    PackedVersionWrittenOut = 1;
                } else {
                    PackedVersionWrittenOut =
                        Print(OutFile, Version, LinePrefix, LineSuffix);
                }
            } else {
                PackedVersionWrittenOut =
                    Print(OutFile, Version, LinePrefix, LineSuffix);
            }
        } else {
            PackedVersionWrittenOut =
                Print(OutFile, Version, LinePrefix, LineSuffix);
        }

        WrittenOut += PackedVersionWrittenOut;
        if constexpr (RightPad) {
            WrittenOut +=
                PrintUtilsRightPadSpaces(OutFile,
                                         PackedVersionWrittenOut,
                                         MACHO_PACKED_VERSION_LEN);

            return MACHO_PACKED_VERSION_LEN;
        }

        WrittenOut += fprintf(OutFile, "%s", LinePrefix);
        return WrittenOut;
    }
};

template <>
struct MachOTypePrinter<struct MachO::DylibCommand::Info> {
    using Type = struct MachO::DylibCommand::Info;

    static int
    Print(FILE *OutFile,
          const Type &Info,
          bool IsBigEndian,
          bool TimestampAligned,
          const char *LinePrefix = "",
          const char *LineSuffix = "") noexcept
    {
        auto WrittenOut =
            fprintf(OutFile,
                    "%sName-Offset:           " OFFSET_32_FMT "\n",
                    LinePrefix,
                    Info.getNameOffset(IsBigEndian));

        WrittenOut +=
            fprintf(OutFile, "%sCurrent Version:       ", LinePrefix);

        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print(OutFile,
                Info.getCurrentVersion(IsBigEndian), "", LineSuffix);

        WrittenOut +=
            fprintf(OutFile, "\n%sCompatibility Version: ", LinePrefix);

        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print(OutFile,
                Info.getCompatVersion(IsBigEndian), "", LineSuffix);

        const auto Timestamp = Info.getTimestamp(IsBigEndian);
        WrittenOut +=
            fprintf(OutFile,
                    "\n%sTimestamp:%s%s (Value: %" PRIu32 ")%s\n",
                    LinePrefix,
                    (TimestampAligned) ? "             " : " ",
                    GetHumanReadableTimestamp(Timestamp).data(),
                    Timestamp,
                    LineSuffix);

        return WrittenOut;
    }

    static int
    PrintOnOneLine(FILE *OutFile,
                   const Type &Info,
                   bool IsBigEndian,
                   bool Verbose,
                   const char *LinePrefix = "",
                   const char *LineSuffix = "") noexcept
    {
        auto WrittenOut = fprintf(OutFile, "%sCurrent-Version: ", LinePrefix);
        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print<true>(OutFile,
                Info.getCurrentVersion(IsBigEndian), "", ",");

        WrittenOut += fprintf(OutFile, "Compatibility-Version: ");
        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print<true>(OutFile,
                Info.getCompatVersion(IsBigEndian), "", ",");

        const auto Timestamp = Info.getTimestamp(IsBigEndian);
        const auto HrTimestamp = GetHumanReadableTimestamp(Timestamp);

        WrittenOut += fprintf(OutFile, "Timestamp: %s", HrTimestamp.data());
        if (Verbose) {
            WrittenOut += fprintf(OutFile, " (Value: %" PRIu32 ")", Timestamp);
        }

        WrittenOut += fprintf(OutFile, "%s", LineSuffix);
        return WrittenOut;
    }
};
