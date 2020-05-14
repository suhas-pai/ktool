//
//  include/Utils/MachOPrinter.h
//  stool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <unistd.h>

#include "ADT/Mach/CpuTypeInfoTemplates.h"
#include "ADT/MachO.h"

#include "PrintUtils.h"
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

    fprintf(OutFile, "%s", LinePrefix);
    if (Is64Bit) {
        if (!DidOverflow) {
            if constexpr (OffsetTypeIs64Bit) {
                WrittenOut =
                    PrintUtilsWriteOffsetRange(OutFile, Offset, End);
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

    auto WrittenOut = int();
    auto ExpectedWrittenOut =
        (Is64Bit) ?
            (OffsetTypeIs64Bit) ?
                OFFSET_64_RANGE_LEN :
                OFFSET_32_64_RANGE_LEN :
            OFFSET_32_RANGE_LEN;

    fprintf(OutFile, "%s", LinePrefix);
    if (Is64Bit) {
        WrittenOut = PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
    } else {
        WrittenOut = PrintUtilsWriteOffsetOverflowsRange(OutFile, Offset);
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
        const auto CpuType = Arch.GetCpuType(IsBigEndian);
        const auto CpuSubType = SwitchEndianIf(Arch.CpuSubType, IsBigEndian);

        auto CpuTypeName =
            (Verbose) ?
                Mach::CpuTypeGetName(CpuType).data() :
                Mach::CpuTypeGetBrandName(CpuType).data();

        if (CpuTypeName == nullptr) {
            CpuTypeName = "Unrecognized";
        }

        auto CpuSubTypeName =
            (Verbose) ?
                Mach::CpuSubType::GetName(CpuType, CpuSubType).data() :
                Mach::CpuSubType::GetFullName(CpuType, CpuSubType).data();

        if (CpuSubTypeName == nullptr) {
            CpuSubTypeName = "Unrecognized";
        }

        const auto Offset = SwitchEndianIf(Arch.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(Arch.Size, IsBigEndian);
        const auto Align = SwitchEndianIf(Arch.Align, IsBigEndian);
        const auto ArchEnd = Offset + Size;

        auto WrittenOut = int();
        if constexpr (Verbose) {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s (Value: %" PRIu32 ")%s\n"
                        "%sCpuSubType: %s (Value: %" PRIu32 ")%s\n"
                        "%sOffset:%-5s" OFFSET_32_FMT " (" OFFSET_32_RNG_FMT
                        ")%s\n%sSize:       %" PRIu32 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuTypeName, static_cast<int32_t>(CpuType),
                        LineSuffix,
                        LinePrefix, CpuSubTypeName, CpuSubType, LineSuffix,
                        LinePrefix, "", Offset, Offset, ArchEnd, LineSuffix,
                        LinePrefix, Size, LineSuffix,
                        LinePrefix, Align, LineSuffix);
        } else {
            WrittenOut =
                fprintf(OutFile,
                        "%sCpuType:    %s%s\n"
                        "%sCpuSubType: %s%s\n"
                        "%sOffset:%-5s" OFFSET_32_FMT " (" OFFSET_32_RNG_FMT
                        ")%s\n%sSize:       %" PRIu32 "%s\n"
                        "%sAlign:      %" PRIu32 "%s\n",
                        LinePrefix, CpuTypeName, LineSuffix,
                        LinePrefix, CpuSubTypeName, LineSuffix,
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
          const char *LinePrefix,
          const char *LineSuffix) noexcept
    {
        const auto CpuType = Arch.GetCpuType(IsBigEndian);
        const auto CpuSubType = SwitchEndianIf(Arch.CpuSubType, IsBigEndian);

        auto CpuTypeName =
            (Verbose) ?
                Mach::CpuTypeGetName(CpuType).data() :
                Mach::CpuTypeGetBrandName(CpuType).data();

        if (CpuTypeName == nullptr) {
            CpuTypeName = "Unrecognized";
        }

        auto CpuSubTypeName =
            (Verbose) ?
                Mach::CpuSubType::GetName(CpuType, CpuSubType).data() :
                Mach::CpuSubType::GetFullName(CpuType, CpuSubType).data();

        if (CpuSubTypeName == nullptr) {
            CpuSubTypeName = "Unrecognized";
        }

        const auto Offset = SwitchEndianIf(Arch.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(Arch.Size, IsBigEndian);
        const auto Align = SwitchEndianIf(Arch.Align, IsBigEndian);
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
                        LinePrefix, CpuTypeName, static_cast<int32_t>(CpuType),
                        LineSuffix,
                        LinePrefix, CpuSubTypeName, CpuSubType, LineSuffix,
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
                        LinePrefix, CpuTypeName, LineSuffix,
                        LinePrefix, CpuSubTypeName, LineSuffix,
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
                  const char *LinePrefix,
                  const char *LineSuffix) noexcept
    {
        const auto IsBigEndian = Header.IsBigEndian();

        auto CombinedLinePrefix = std::string(LinePrefix).append("\t");
        auto WrittenOut = int();
        auto I = static_cast<uint32_t>(1);

        if (Header.Is64Bit()) {
            for (const auto &Arch : Header.GetConstArch64List(IsBigEndian)) {
                fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);
                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch64>::Print<false>(
                        OutFile, Arch, IsBigEndian, CombinedLinePrefix.data(),
                        LineSuffix);
                I++;
            }
        } else {
            for (const auto &Arch : Header.GetConstArch32List(IsBigEndian)) {
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
                         const char *LinePrefix,
                         const char *LineSuffix) noexcept
    {
        const auto IsBigEndian = Header.IsBigEndian();

        auto CombinedLinePrefix = std::string(LinePrefix).append("\t");
        auto WrittenOut = int();
        auto I = static_cast<uint32_t>(1);

        if (Header.Is64Bit()) {
            for (const auto &Arch : Header.GetConstArch64List(IsBigEndian)) {
                fprintf(OutFile, "%sArch #%" PRIu32 "\n", LinePrefix, I);
                WrittenOut +=
                    MachOTypePrinter<MachO::FatHeader::Arch64>::Print<true>(
                        OutFile, Arch, IsBigEndian, CombinedLinePrefix.data(),
                        LineSuffix);
                I++;
            }
        } else {
            for (const auto &Arch : Header.GetConstArch32List(IsBigEndian)) {
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
          bool IsBigEndian,
          const char *LinePrefix,
          const char *LineSuffix) noexcept
    {
        auto WrittenOut = fprintf(OutFile, "%s", LinePrefix);
        const auto PackedVersionWrittenOut =
            fprintf(OutFile,
                    MACHO_PACKED_VERSION_FMT,
                    MACHO_PACKED_VERSION_FMT_ARGS(Version, IsBigEndian));

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
                      bool IsBigEndian,
                      const char *LinePrefix,
                      const char *LineSuffix)
    {
        const auto Major = Version.GetMajor(IsBigEndian);
        const auto Minor = Version.GetMinor(IsBigEndian);
        const auto Revision = Version.GetRevision(IsBigEndian);

        auto WrittenOut = fprintf(OutFile, "%s", LinePrefix);
        auto PackedVersionWrittenOut = int();

        if (Revision == 0) {
            if (Minor == 0) {
                if (Major == 0) {
                    PackedVersionWrittenOut = fprintf(OutFile, "0");
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
                bool IsBigEndian,
                const char *LinePrefix,
                const char *LineSuffix)
    {
        const auto Major = Version.GetMajor(IsBigEndian);
        const auto Minor = Version.GetMinor(IsBigEndian);
        const auto Revision = Version.GetRevision(IsBigEndian);

        auto WrittenOut = fprintf(OutFile, "%s", LinePrefix);
        auto PackedVersionWrittenOut = int();

        if (Revision == 0) {
            if (Minor == 0) {
                if (Major == 0) {
                    PackedVersionWrittenOut = fprintf(OutFile, "0");
                } else {
                    PackedVersionWrittenOut =
                        Print(OutFile,
                              Version,
                              IsBigEndian,
                              LinePrefix,
                              LineSuffix);
                }
            } else {
                PackedVersionWrittenOut =
                    Print(OutFile,
                          Version,
                          IsBigEndian,
                          LinePrefix,
                          LineSuffix);
            }
        } else {
            PackedVersionWrittenOut =
                Print(OutFile, Version, IsBigEndian, LinePrefix, LineSuffix);
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
          const char *LinePrefix,
          const char *LineSuffix) noexcept
    {
        auto WrittenOut =
            fprintf(OutFile, "%sCurrent Version:       ", LinePrefix);

        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print(OutFile,
                Info.CurrentVersion, IsBigEndian, "", "");

        WrittenOut +=
            fprintf(OutFile,
                    "%s\n%sCompatibility Version: ",
                    LineSuffix,
                    LinePrefix);

        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print(OutFile,
                Info.CompatibilityVersion, IsBigEndian, "", "");

        const auto Timestamp = SwitchEndianIf(Info.Timestamp, IsBigEndian);
        WrittenOut +=
            fprintf(OutFile,
                    "%s\n%sTimestamp:%s%s (Value: %" PRIu32 ")\n",
                    LineSuffix,
                    LinePrefix,
                    (TimestampAligned) ? "             " : " ",
                    GetHumanReadableTimestamp(Timestamp).data(),
                    Timestamp);

        return WrittenOut;
    }

    static int
    PrintOnOneLine(FILE *OutFile,
                   const Type &Info,
                   bool IsBigEndian,
                   bool Verbose,
                   const char *LinePrefix,
                   const char *LineSuffix) noexcept
    {
        auto WrittenOut = fprintf(OutFile, "%sCurrent-Version: ", LinePrefix);
        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print<true>(OutFile,
                Info.CurrentVersion, IsBigEndian, "", ",");

        WrittenOut += fprintf(OutFile, "Compatibility-Version: ");
        WrittenOut +=
            MachOTypePrinter<MachO::PackedVersion>::Print<true>(OutFile,
                Info.CompatibilityVersion, IsBigEndian, "", ",");

        const auto Timestamp = SwitchEndianIf(Info.Timestamp, IsBigEndian);
        const auto HrTimestamp = GetHumanReadableTimestamp(Timestamp);

        WrittenOut += fprintf(OutFile, "Timestamp: %s", HrTimestamp.data());
        if (Verbose) {
            fprintf(OutFile, " (Value: %" PRIu32 ")", Timestamp);
        }

        WrittenOut += fprintf(OutFile, "%s", LineSuffix);
        return WrittenOut;
    }
};
