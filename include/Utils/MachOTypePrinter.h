//
//  Utils/MachOPrinter.h
//  ktool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include "ADT/Mach-O/Headers.h"

#include "ADT/Mach/CpuKindInfoTemplates.h"
#include "ADT/Mach/CpuSubKindInfo.h"

#include "PrintUtils.h"
#include "Timestamp.h"

template <std::integral OffsetType, std::integral SizeType>
static inline int
MachOPrintOffsetSizeRange(FILE *const OutFile,
                          const OffsetType &Offset,
                          const SizeType &Size,
                          const char *LinePrefix,
                          const char *LineSuffix,
                          bool Is64Bit,
                          bool Pad,
                          OffsetType *EndOut = nullptr) noexcept
{
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

template <std::integral OffsetType, std::integral EndType>
static inline int
MachOPrintOffsetEndRange(FILE *const OutFile,
                         const OffsetType &Offset,
                         const EndType &End,
                         const char *LinePrefix,
                         const char *LineSuffix,
                         bool Is64Bit,
                         bool Pad,
                         OffsetType *EndOut = nullptr) noexcept
{
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

    template <PrintKind PrintKind>
    static int
    Print(FILE *const OutFile,
          const Type &Arch,
          bool IsBigEndian,
          const char *LinePrefix,
          const char *LineSuffix) noexcept
    {
        const auto CpuKind = Arch.getCpuKind(IsBigEndian);
        const auto CpuSubKind = Arch.getCpuSubKind(IsBigEndian);
        const auto CpuKindBrandName =
            Mach::CpuKindGetBrandName(CpuKind).value_or("<Unrecognized>");

        const auto CpuKindName =
            Mach::CpuKindGetName(CpuKind).value_or("<Unrecognized>");

        const auto CpuSubKindFullName =
            Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind)
                .value_or("<Unrecognized>");

        const auto CpuSubKindName =
            Mach::CpuSubKind::GetName(CpuKind, CpuSubKind)
                .value_or("<Unrecognized>");

        const auto Offset = Arch.getFileOffset(IsBigEndian);
        const auto Size = Arch.getFileSize(IsBigEndian);
        const auto Align = Arch.getAlign(IsBigEndian);
        const auto ArchEnd = Offset + Size;

        auto WrittenOut = int();
        if constexpr (PrintKindIsVerbose(PrintKind)) {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s (%s) (Value: %" PRIu32 ")%s\n"
                        "%sCpuSubType: %s (%s) (Value: %" PRIu32 ")%s\n"
                        "%sOffset:%-5s" OFFSET_32_FMT " (" OFFSET_32_RNG_FMT ")"
                        "%s\n%sSize:       %" PRIu32 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindBrandName.data(), CpuKindName.data(),
                            static_cast<int32_t>(CpuKind), LineSuffix,
                        LinePrefix, CpuSubKindName.data(),
                            CpuSubKindFullName.data(),
                        CpuSubKind, LineSuffix,
                        LinePrefix, "", Offset, Offset, ArchEnd, LineSuffix,
                        LinePrefix, Size, LineSuffix,
                        LinePrefix, Align, LineSuffix);
        } else {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s (%s) %s\n"
                        "%sCpuSubType: %s (%s) %s\n"
                        "%sOffset:%-5s" OFFSET_32_FMT " (" OFFSET_32_RNG_FMT ")"
                        "%s\n%sSize:       %" PRIu32 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindBrandName.data(), CpuKindName.data(),
                            LineSuffix,
                        LinePrefix, CpuSubKindFullName.data(),
                            CpuSubKindName.data(),
                        LineSuffix,
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

    template <PrintKind PrintKind>
    static int
    Print(FILE *const OutFile,
          const Type &Arch,
          bool IsBigEndian,
          const char *LinePrefix = "",
          const char *LineSuffix = "") noexcept
    {
        const auto CpuKind = Arch.getCpuKind(IsBigEndian);
        const auto CpuSubKind = Arch.getCpuSubKind(IsBigEndian);
        const auto CpuKindName =
            PrintKindIsVerbose(PrintKind) ?
                Mach::CpuKindGetBrandName(CpuKind).value_or("<Unrecognized>") :
                Mach::CpuKindGetName(CpuKind).value_or("<Unrecognized>");

        const auto CpuSubKindName =
            PrintKindIsVerbose(PrintKind) ?
                Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind)
                    .value_or("<Unrecognized>") :
                Mach::CpuSubKind::GetName(CpuKind, CpuSubKind)
                    .value_or("<Unrecognized>");

        const auto Offset = Arch.getFileOffset(IsBigEndian);
        const auto Size = Arch.getFileSize(IsBigEndian);
        const auto Align = Arch.getAlign(IsBigEndian);
        const auto ArchEnd = Offset + Size;

        auto WrittenOut = int();
        if constexpr (PrintKindIsVerbose(PrintKind)) {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s (Value: %" PRIu32 ")%s\n"
                        "%sCpuSubType: %s (Value: %" PRIu32 ")%s\n"
                        "%sOffset:%-5s" OFFSET_64_FMT " (" OFFSET_64_RNG_FMT ")"
                        "%s\n%sSize:       %" PRIu64 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuKindName.data(),
                            static_cast<int32_t>(CpuKind), LineSuffix,
                        LinePrefix, CpuSubKindName.data(), CpuSubKind,
                            LineSuffix,
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
                        LinePrefix, CpuKindName.data(), LineSuffix,
                        LinePrefix, CpuSubKindName.data(), LineSuffix,
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
    PrintArchList(FILE *const OutFile,
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
                    MachOTypePrinter<MachO::FatHeader::Arch64>::Print<
                        PrintKind::Default>(OutFile,
                                            Arch,
                                            IsBigEndian,
                                            CombinedLinePrefix.data(),
                                            LineSuffix);
                I++;
            }
        } else {
            for (const auto &Arch : Header.getConstArch32List()) {
                WrittenOut +=
                    fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);

                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch32>::Print<
                        PrintKind::Default>(OutFile,
                                            Arch,
                                            IsBigEndian,
                                            CombinedLinePrefix.data(),
                                            LineSuffix);
                I++;
            }
        }

        return WrittenOut;
    }

    static int
    PrintArchListVerbose(FILE *const OutFile,
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
                    MachOTypePrinter<MachO::FatHeader::Arch64>::Print<
                        PrintKind::Verbose>(OutFile,
                                            Arch,
                                            IsBigEndian,
                                            CombinedLinePrefix.data(),
                                            LineSuffix);
                I++;
            }
        } else {
            for (const auto &Arch : Header.getConstArch32List()) {
                WrittenOut +=
                    fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);

                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch32>::Print<
                        PrintKind::Verbose>(OutFile,
                                            Arch,
                                            IsBigEndian,
                                            CombinedLinePrefix.data(),
                                            LineSuffix);
                I++;
            }
        }

        return WrittenOut;
    }
};

template <>
struct MachOTypePrinter<Dyld3::PackedVersion> {
    using Type = Dyld3::PackedVersion;

    template <bool RightPad = false>
    static int
    Print(FILE *const OutFile,
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
    PrintWithoutZeros(FILE *const OutFile,
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
    PrintOrZero(FILE *const OutFile,
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
    Print(FILE *const OutFile,
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
            MachOTypePrinter<Dyld3::PackedVersion>::Print(OutFile,
                Info.getCurrentVersion(IsBigEndian), "", LineSuffix);

        WrittenOut +=
            fprintf(OutFile, "\n%sCompatibility Version: ", LinePrefix);

        WrittenOut +=
            MachOTypePrinter<Dyld3::PackedVersion>::Print(OutFile,
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
    PrintOnOneLine(FILE *const OutFile,
                   const Type &Info,
                   bool IsBigEndian,
                   PrintKind PrintKind,
                   const char *LinePrefix = "",
                   const char *LineSuffix = "") noexcept
    {
        auto WrittenOut = fprintf(OutFile, "%sCurrent-Version: ", LinePrefix);
        WrittenOut +=
            MachOTypePrinter<Dyld3::PackedVersion>::Print<true>(OutFile,
                Info.getCurrentVersion(IsBigEndian), "", ",");

        WrittenOut += fprintf(OutFile, "Compatibility-Version: ");
        WrittenOut +=
            MachOTypePrinter<Dyld3::PackedVersion>::Print<true>(OutFile,
                Info.getCompatVersion(IsBigEndian), "", ",");

        const auto Timestamp = Info.getTimestamp(IsBigEndian);
        const auto HrTimestamp = GetHumanReadableTimestamp(Timestamp);

        WrittenOut += fprintf(OutFile, "Timestamp: %s", HrTimestamp.data());
        if (PrintKindIsVerbose(PrintKind)) {
            WrittenOut += fprintf(OutFile, " (Value: %" PRIu32 ")", Timestamp);
        }

        WrittenOut += fprintf(OutFile, "%s", LineSuffix);
        return WrittenOut;
    }
};
