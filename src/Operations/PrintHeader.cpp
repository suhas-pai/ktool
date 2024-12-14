//
//  Operations/PrintHeader.cpp
//  ktool
//
//  Created by Suhas Pai on 4/10/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <format>

#include "Operations/Common.h"
#include "Operations/Operation.h"
#include "Operations/PrintHeader.h"

#include "Utils/MachOTypePrinter.h"
#include "Utils/PrintUtils.h"

PrintHeaderOperation::PrintHeaderOperation() noexcept : Operation(OpKind) {}
PrintHeaderOperation::PrintHeaderOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

int
PrintHeaderOperation::Run(const MachOMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    fputs("Mach-O Single Architecture File\n", stdout);

    const auto Magic = Object.getMagic();
    const auto CpuKind = Object.getCpuKind();
    const auto CpuSubKind = Object.getCpuSubKind();
    const auto FileKind = Object.getFileKind();
    const auto Ncmds = Object.getLoadCommandsCount();
    const auto SizeOfCmds = Object.getLoadCommandsSize();
    const auto Flags = Object.getHeaderFlags();

    const auto MagicDescOpt = MachO::Header::MagicGetDescription(Magic);
    const auto MagicDesc =
        MagicDescOpt.has_value() ?
            std::string(MagicDescOpt.value()) :
            std::format("Unrecognized (Value: 0x{:08x}",
                        static_cast<uint32_t>(Magic));

    const auto FileKindNameOpt = MachO::Header::FileKindGetName(FileKind);
    const auto FileKindDescOpt =
        MachO::Header::FileKindGetDescription(FileKind);

    const auto CpuKindValue = static_cast<int32_t>(CpuKind);
    const auto FileKindValue = static_cast<uint32_t>(FileKind);
    const auto FlagsValue = Flags.value();

    if (Options.Verbose) {
        const auto MagicNameOpt = MachO::Header::MagicGetName(Magic);
        const auto MagicName =
            MagicDescOpt.has_value() ?
                std::string(MagicNameOpt.value()) :
                std::format("Unrecognized (Value: 0x{:08x}",
                            static_cast<uint32_t>(Magic));

        const auto MagicValue = static_cast<uint32_t>(Magic);
        fprintf(Options.OutFile,
                "Magic:\n"
                "\tName:        %s\n"
                "\tDescription: %s\n"
                "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                MagicName.data(),
                MagicDesc.data(),
                MagicValue,
                MagicValue);

        const auto CpuKindNameOpt = Mach::CpuKindGetName(CpuKind);
        if (CpuKindNameOpt.has_value()) {
            const auto CpuKindBrandNameOpt = Mach::CpuKindGetBrandName(CpuKind);
            const auto CpuKindName = std::string(CpuKindNameOpt.value());
            const auto CpuKindBrandName =
                CpuKindBrandNameOpt.has_value() ?
                    std::string(CpuKindBrandNameOpt.value()) :
                    std::format("Unrecognized (Value: 0x{:08x}",
                                static_cast<int32_t>(CpuKind));

            const auto CpuSubKindNameOpt =
                Mach::CpuSubKind::GetName(CpuKind, CpuSubKind);
            const auto CpuSubKindBrandNameOpt =
                Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind);

            if (CpuSubKindNameOpt.has_value()) {
                fprintf(Options.OutFile,
                        "CpuType:\n"
                        "\tName:        %s\n"
                        "\tDescription: %s\n"
                        "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n"
                        "CpuSubType:\n"
                        "\tName:        %s\n"
                        "\tDescription: %s\n"
                        "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                        CpuKindName.c_str(),
                        CpuKindBrandName.c_str(),
                        CpuKindValue,
                        CpuKindValue,
                        CpuSubKindNameOpt.value().data(),
                        CpuSubKindBrandNameOpt.value().data(),
                        CpuSubKind,
                        CpuSubKind);
            } else {
                fprintf(Options.OutFile,
                        "CpuSubType:\n"
                        "\tUnrecognized\n"
                        "\tValue: %" PRIu32 " (0x%" PRIX32 ")\n",
                        CpuSubKind,
                        CpuSubKind);
            }
        } else {
            fprintf(Options.OutFile,
                    "CpuType:\n"
                    "\tUnrecognized\n\tValue: %" PRIu32 " (Value: 0x%X)\n",
                    CpuKindValue,
                    CpuKindValue);
            fprintf(Options.OutFile,
                    "CpuSubType:\n"
                    "\tUnknown\n"
                    "\tValue: %" PRIu32 " (Value: 0x%" PRIX32 ")\n",
                    CpuSubKind,
                    CpuSubKind);
        }

        if (FileKindNameOpt.has_value()) {
            fprintf(Options.OutFile,
                    "FileType:\n"
                    "\tName:        %s\n"
                    "\tDescription: %s\n"
                    "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                    FileKindNameOpt.value().data(),
                    FileKindDescOpt.value().data(),
                    FileKindValue,
                    FileKindValue);
        } else {
            fprintf(Options.OutFile,
                    "FileType:\n"
                    "\tUnrecognized\n\tValue: %" PRIu32 " (0x%" PRIX32 ")\n",
                    FileKindValue,
                    FileKindValue);
        }

        PrintUtilsWriteFormattedNumber(Options.OutFile, Ncmds, "Ncmds: ", "\n");
        PrintUtilsWriteFormattedNumber(Options.OutFile,
                                       SizeOfCmds,
                                       "SizeOfCmds: ",
                                       "\n");

        const auto FlagInfoList = OperationCommon::GetFlagInfoList(Flags);
        fprintf(Options.OutFile,
                "Flags:\n"
                "\tNumber: %" PRIu32 " (0x%X)\n"
                "\tCount:  %" PRIuPTR "\n"
                "\tValues:\n",
                FlagsValue,
                FlagsValue,
                FlagInfoList.size());

        const auto PrintKind = PrintKindFromIsVerbose(Options.Verbose);
        OperationCommon::PrintFlagInfoList(Options.OutFile,
                                           FlagInfoList,
                                           PrintKind,
                                           "\t\t");
    } else {
        const auto CpuBrandNameOpt = Mach::CpuKindGetBrandName(CpuKind);
        const auto SubKindFullNameOpt =
            Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind);

        const auto MagicDesc =
            MagicDescOpt.has_value() ?
                std::string(MagicDescOpt.value()) :
                std::format("Unrecognized (Value: 0x{:08x}",
                            static_cast<int32_t>(Magic));

        const auto CpuBrandName =
            CpuBrandNameOpt.has_value() ?
                std::string(CpuBrandNameOpt.value()) :
                std::format("Unrecognized (Value: 0x{:08x}",
                            static_cast<int32_t>(CpuKind));

        const auto SubKindFullName =
            SubKindFullNameOpt.has_value() ?
                std::string(SubKindFullNameOpt.value()) :
                std::format("Unrecognized (Value: 0x{:08x}",
                            static_cast<int32_t>(CpuSubKind));

        const auto FileKindDesc =
            FileKindDescOpt.has_value() ?
                std::string(FileKindDescOpt.value()) :
                std::format("Unrecognized (Value: 0x{:08x}",
                            static_cast<int32_t>(FileKindValue));

        fprintf(Options.OutFile,
                "Magic:      %s\n"
                "CpuType:    %s\n"
                "CpuSubType: %s\n"
                "FileType:   %s\n",
                MagicDesc.c_str(),
                CpuBrandName.c_str(),
                SubKindFullName.c_str(),
                FileKindDesc.c_str());

        PrintUtilsWriteFormattedNumber(Options.OutFile,
                                       Ncmds,
                                       "Ncmds:      ",
                                       "\n");

        PrintUtilsWriteFormattedNumber(Options.OutFile,
                                       SizeOfCmds,
                                       "SizeOfCmds: ",
                                       "\n");

        fprintf(Options.OutFile, "Flags:      0x%X\n", FlagsValue);
    }

    return 0;
}

int
PrintHeaderOperation::Run(const FatMachOMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    fputs("Mach-O Multi-Architecture (FAT) File\n", stdout);

    const auto &Header = Object.getConstHeader();
    const auto &Magic = Header.Magic;

    const auto MagicDescOpt = MachO::FatHeader::MagicGetDescription(Magic);
    const auto MagicDesc =
        MagicDescOpt.has_value() ?
            std::string(MagicDescOpt.value()) :
            std::format("Unrecognized (Value: 0x{:X})",
                        static_cast<uint32_t>(Magic));

    if (Options.Verbose) {
        const auto MagicValue = static_cast<uint32_t>(Magic);
        const auto MagicNameOpt = MachO::FatHeader::MagicGetName(Magic);
        const auto MagicName =
            MagicNameOpt.has_value() ?
                std::string(MagicNameOpt.value()) :
                std::format("Unrecognized (Value: 0x{:X})", MagicValue);

        fprintf(Options.OutFile,
                "Magic:\n"
                "\tName: %s\n"
                "\tDescription: %s\n"
                "\tValue: %" PRIu32 " (0x%" PRIX32 ")\n",
                MagicName.c_str(),
                MagicDesc.c_str(),
                MagicValue,
                MagicValue);
    } else {
        fprintf(Options.OutFile, "Magic: %s\n", MagicDesc.c_str());
    }

    const auto ArchsCount = Object.getArchCount();
    fprintf(Options.OutFile, "File has %" PRIu32 " Archs", ArchsCount);

    if (ArchsCount <= 3) {
        fputs(":\n", Options.OutFile);
        MachOTypePrinter<MachO::FatHeader>::PrintArchList(Options.OutFile,
                                                          Header,
                                                          "\t");
    } else {
        fputc('\n', Options.OutFile);
    }

    return 0;
}

enum class DscRangeKind {
    None,

    // HeaderV0
    MappingInfoList,
    ImageInfoList,

    // HeaderV1
    CodeSignature,
    SlideInfo,

    // HeaderV2
    LocalSymbolInfo,

    // HeaderV4
    AcceleratorInfo,
    ImageTextInfoList,

    // HeaderV5
    DylibsImageGroup,
    OtherImageGroup,
    ProgClosures,
    ProgClosuresTrie,

    // HeaderV6
    DylibsImageArray,
    DylibsTrie,
    OtherImageArray,
    OtherTrie,

    // HeaderV7

    // HeaderV8
    ProgramsPBLSetPool,
    ProgramTrie,
    SwiftOpts,
    RosettaReadOnly,
    RosettaReadWrite,

    End,
};

struct DscRange {
    DscRangeKind Kind;
    Range Range;
    std::bitset<static_cast<int>(DscRangeKind::End) - 1> OverlapKindSet;
};

using DscRangeList = std::unordered_map<DscRangeKind, DscRange>;
static void AddRangeToList(DscRangeList &List, DscRange &&DscRange) noexcept {
    for (const auto &Iter : List) {
        const auto &IterKind = Iter.second.Kind;
        if (DscRange.Kind < IterKind) {
            continue;
        }

        if (DscRange.Range.overlaps(Iter.second.Range)) {
            DscRange.OverlapKindSet.set(static_cast<int>(IterKind) - 1);
        }
    };

    List.insert({ DscRange.Kind, std::move(DscRange) });
}

[[nodiscard]] static DscRangeList
CollectDscRangeList(const DyldSharedCache::Header &Header,
                    const DyldSharedCache::HeaderVersion &Version) noexcept
{
    auto Kind = DscRangeKind::None;
    auto Result = DscRangeList();

    switch (Kind) {
        case DscRangeKind::None:
        case DscRangeKind::MappingInfoList: {
            if (const auto Range = Header.getMappingInfoListRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::MappingInfoList,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::ImageInfoList: {
            if (const auto Range = Header.getImageInfoListRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ImageInfoList,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        if (Version < DyldSharedCache::HeaderVersion::V1) {
            return Result;
        }
        case DscRangeKind::CodeSignature: {
            if (const auto Range = Header.getCodeSignatureRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::CodeSignature,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::SlideInfo: {
            if (const auto Range = Header.getSlideInfoRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::SlideInfo,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        if (Version < DyldSharedCache::HeaderVersion::V2) {
            return Result;
        }
        case DscRangeKind::LocalSymbolInfo: {
            if (const auto Range = Header.getLocalSymbolInfoRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::LocalSymbolInfo,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        if (Version < DyldSharedCache::HeaderVersion::V4) {
            return Result;
        }
        case DscRangeKind::AcceleratorInfo: {
            if (const auto Range = Header.getAcceleratorInfoRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::AcceleratorInfo,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::ImageTextInfoList: {
            if (const auto Range = Header.getImageTextInfoListRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ImageTextInfoList,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        if (Version < DyldSharedCache::HeaderVersion::V5) {
            return Result;
        }
        case DscRangeKind::DylibsImageGroup: {
            if (const auto Range = Header.getDylibsImageGroupRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsImageGroup,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::OtherImageGroup: {
            if (const auto Range = Header.getOtherImageGroupRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherImageGroup,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::ProgClosures: {
            if (const auto Range = Header.getProgClosuresRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgClosures,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::ProgClosuresTrie: {
            if (const auto Range = Header.getProgClosuresTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgClosuresTrie,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        if (Version < DyldSharedCache::HeaderVersion::V6) {
            return Result;
        }
        case DscRangeKind::DylibsImageArray: {
            if (const auto Range = Header.getDylibsImageArrayRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsImageArray,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::DylibsTrie: {
            if (const auto Range = Header.getDylibsTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsTrie,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::OtherImageArray: {
            if (const auto Range = Header.getOtherImageArrayRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherImageArray,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::OtherTrie: {
            if (const auto Range = Header.getOtherTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherTrie,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        // Nothing in V7
        if (Version < DyldSharedCache::HeaderVersion::V8) {
            return Result;
        }
        case DscRangeKind::ProgramsPBLSetPool: {
            if (const auto Range = Header.getProgramsPBLSetPoolRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgramsPBLSetPool,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::ProgramTrie: {
            if (const auto Range = Header.getProgramTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgramTrie,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::SwiftOpts: {
            if (const auto Range = Header.getSwiftOptsRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::SwiftOpts,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::RosettaReadOnly: {
            if (const auto Range = Header.getRosettaReadOnlyRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::RosettaReadOnly,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::RosettaReadWrite: {
            if (const auto Range = Header.getRosettaReadOnlyRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::RosettaReadWrite,
                    .Range = Range.value(),
                    .OverlapKindSet = {}
                });
            }
        }
        case DscRangeKind::End:
            break;
    }

    return Result;
}

static void
PrintDscRangeOverlapsErrorOrNewline(
    const struct PrintHeaderOperation::Options &Options,
    const DscRangeList &List,
    const DscRangeKind Kind) noexcept
{
    if (Options.Verbose) {
        fputc('\n', Options.OutFile);
        return;
    }

    const auto End = List.cend();
    const auto Iter = List.find(Kind);

    if (Iter == End) {
        fputc('\n', Options.OutFile);
        return;
    }

    const auto &OverlapKindSet = Iter->second.OverlapKindSet;
    if (OverlapKindSet.none()) {
        fputc('\n', Options.OutFile);
        return;
    }

    fputs(" (Overlaps with ", Options.ErrFile);

    auto Count = uint8_t();
    auto OverlapKindSetCount = OverlapKindSet.count();

    for (auto KindInt = static_cast<int>(DscRangeKind::None) + 1;; KindInt++) {
        const auto Kind = static_cast<DscRangeKind>(KindInt);
        if (!OverlapKindSet[KindInt - 1]) {
            continue;
        }

        switch (Kind) {
            case DscRangeKind::None:
                assert(0 && "Overlap-Kind None is set");
            case DscRangeKind::MappingInfoList:
                fputs("Mapping-Info Range", Options.ErrFile);
                break;
            case DscRangeKind::ImageInfoList:
                fputs("Image-Info Range", Options.ErrFile);
                break;
            case DscRangeKind::CodeSignature:
                fputs("Code-Signature Range", Options.ErrFile);
                break;
            case DscRangeKind::SlideInfo:
                fputs("Slide-Info Range", Options.ErrFile);
                break;
            case DscRangeKind::LocalSymbolInfo:
                fputs("Local-Symbol Info Range", Options.ErrFile);
                break;
            case DscRangeKind::AcceleratorInfo:
                fputs("Accelerator-Info Range", Options.ErrFile);
                break;
            case DscRangeKind::ImageTextInfoList:
                fputs("Image-Text Info Range", Options.ErrFile);
                break;
            case DscRangeKind::DylibsImageGroup:
                fputs("Dylibs Image-Group Range", Options.ErrFile);
                break;
            case DscRangeKind::OtherImageGroup:
                fputs("Other-Image Group Range", Options.ErrFile);
                break;
            case DscRangeKind::ProgClosures:
                fputs("Prog-Closures Range", Options.ErrFile);
                break;
            case DscRangeKind::ProgClosuresTrie:
                fputs("Prog-Closures Trie Range", Options.ErrFile);
                break;
            case DscRangeKind::DylibsImageArray:
                fputs("Dylibs-Image Range", Options.ErrFile);
                break;
            case DscRangeKind::DylibsTrie:
                fputs("Dylibs-Trie Range", Options.ErrFile);
                break;
            case DscRangeKind::OtherImageArray:
                fputs("Other-Image Array Range", Options.ErrFile);
                break;
            case DscRangeKind::OtherTrie:
                fputs("Other-Trie Range", Options.ErrFile);
                break;
            case DscRangeKind::ProgramsPBLSetPool:
                fputs("Programs Prebuilt Loader Set Pool Range",
                      Options.ErrFile);
                break;
            case DscRangeKind::ProgramTrie:
                fputs("Program Trie Range", Options.ErrFile);
                break;
            case DscRangeKind::SwiftOpts:
                fputs("Swift Optimizations Range", Options.ErrFile);
                break;
            case DscRangeKind::RosettaReadOnly:
                fputs("Rosetta Read Only Range", Options.ErrFile);
                break;
            case DscRangeKind::RosettaReadWrite:
                fputs("Rosetta Read Write Range", Options.ErrFile);
                break;
            case DscRangeKind::End:
                assert(0 && "Overlap-Kind End is set");
        }

        Count++;
        if (Count == OverlapKindSetCount) {
            fputs(")\n", Options.ErrFile);
            break;
        } else {
            fputs(", ", Options.ErrFile);
        }
    }
}

constexpr static auto LongestDscKey =
    LENGTH_OF("Programs Prebuilt Loader Set Pool Address");

static void
PrintDscKey(FILE *const OutFile,
            const char *Key,
            const char *const Prefix = "") noexcept
{
    fputs(Prefix, OutFile);
    fflush(OutFile);

    PrintUtilsRightPadSpaces(OutFile,
                             fprintf(OutFile, "%s: ", Key),
                             LongestDscKey + LENGTH_OF(": "));
}

static void
WarnIfOutOfRange(FILE *const OutFile,
                 const Range &Range,
                 const uint64_t Offset,
                 const uint64_t Size,
                 bool PrintNewLine = true) noexcept
{
    const auto LocRange = Range::CreateWithSize(Offset, Size);
    if (!Range.contains(LocRange)) {
        fputs(" (Past EOF!)", OutFile);
    }

    if (PrintNewLine) {
        fputc('\n', OutFile);
    }
}

template <std::integral T>
static inline int
PrintSize(FILE *const OutFile,
          const T Size,
          const bool Verbose,
          const char *const Suffix = "") noexcept
{
    auto WrittenOut = PrintUtilsWriteFormattedSize(OutFile, Size);
    if (Verbose && Size != 0) {
        WrittenOut += PrintUtilsWriteNumber(OutFile, Size, " (", " bytes)");
    }

    WrittenOut += fputs(Suffix, OutFile);
    fflush(OutFile);

    return WrittenOut;
}

template <
    void (*PrintKeyFunc)(FILE *, const char *, const char *) = PrintDscKey,
    std::integral AddrType,
    std::integral SizeType>

static inline void
PrintDscSizeRange(FILE *const OutFile,
                  const Range &DscRange,
                  const char *AddressName,
                  const char *SizeName,
                  AddrType Address,
                  SizeType Size,
                  bool Verbose,
                  const char *const Prefix = "",
                  const char *const Suffix = "",
                  bool IsOffset = false,
                  bool PrintNewLine = false) noexcept
{
    PrintKeyFunc(OutFile, AddressName, Prefix);
    PrintUtilsWriteOffset(OutFile, Address);

    const auto VerboseAndNonZeroSize = (Verbose && Size != 0);
    constexpr auto SizeIs64Bit = std::is_same_v<SizeType, uint64_t>;

    if (VerboseAndNonZeroSize) {
        constexpr auto AddrIs64Bit = std::is_same_v<AddrType, uint64_t>;
        if constexpr (AddrIs64Bit || SizeIs64Bit) {
            PrintUtilsWriteOffsetSizeRange(OutFile,
                                           static_cast<uint64_t>(Address),
                                           static_cast<uint64_t>(Size),
                                           " (",
                                           ")");
        } else {
            PrintUtilsWriteOffsetSizeRange(OutFile,
                                           static_cast<uint32_t>(Address),
                                           static_cast<uint32_t>(Size),
                                           " (",
                                           ")");
        }
    }

    if (IsOffset) {
        WarnIfOutOfRange(OutFile, DscRange, Address, Size, false);
    }

    fputc('\n', OutFile);

    PrintKeyFunc(OutFile, SizeName, Prefix);
    PrintSize(OutFile, Size, Verbose);

    if (PrintNewLine) {
        fprintf(OutFile, "%s\n", Suffix);
    } else {
        fputc('\n', OutFile);
    }
}

static void
PrintMappingInfoList(const struct PrintHeaderOperation::Options &Options,
                     const DscMemoryObject &Object)
{
    const auto MappingCountDigitLength =
        PrintUtilsGetIntegerDigitLength(Object.getMappingCount());

    constexpr auto LongestKeyLength = LENGTH_OF("File-Offset: ");
    auto Index = static_cast<int>(1);

    for (const auto &Mapping : Object.getMappingInfoList()) {
        const auto InitProt = Mapping.getInitProt();
        const auto MaxProt = Mapping.getMaxProt();

        fprintf(Options.OutFile,
                "\tMapping %0*d: " MEM_PROT_INIT_MAX_RNG_FMT " \n",
                MappingCountDigitLength,
                Index,
                MEM_PROT_INIT_MAX_RNG_FMT_ARGS(InitProt, MaxProt));

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("\t\tFile-Offset: ", Options.OutFile),
                                 LENGTH_OF("\t\t") + LongestKeyLength);

        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.FileOffset,
                              /*Pad=*/true,
                              /*Prefix=*/"");

        const auto PrintRange = (!Mapping.isEmpty() && Options.Verbose);
        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.FileOffset,
                                           Mapping.Size,
                                           " (",
                                           ")");
        }

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("\n\t\tAddress: ", Options.OutFile),
                                 LENGTH_OF("\n\t\t") + LongestKeyLength);

        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.Address,
                              /*Pad=*/true,
                              /*Prefix=*/"");

        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.Address,
                                           Mapping.Size,
                                           " (",
                                           ")");
        }

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("\n\t\tSize: ", Options.OutFile),
                                 LENGTH_OF("\n\t\t") + LongestKeyLength);

        PrintSize(Options.OutFile,
                  Mapping.Size,
                  Options.Verbose,
                  /*Suffix=*/"\n");
        Index++;
    }
}

static void
PrintMappingWithSlideInfoList(
    const struct PrintHeaderOperation::Options &Options,
    const DscMemoryObject &Object)
{
    const auto MappingCountDigitLength =
        PrintUtilsGetIntegerDigitLength(Object.getMappingCount());

    const auto LongestKeyLength = LENGTH_OF("Slide-Info File Offset: ");
    auto Index = static_cast<int>(1);

    const auto &Header = Object.getHeaderV7();
    for (const auto &Mapping : Header.getMappingWithSlideInfoList()) {
        const auto InitProt = Mapping.getInitProt();
        const auto MaxProt = Mapping.getMaxProt();

        fprintf(Options.OutFile,
                "\tMapping %0*d: " MEM_PROT_INIT_MAX_RNG_FMT " \n",
                MappingCountDigitLength,
                Index,
                MEM_PROT_INIT_MAX_RNG_FMT_ARGS(InitProt, MaxProt));

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("\t\tFile-Offset: ", Options.OutFile),
                                 LENGTH_OF("\t\t") + LongestKeyLength);

        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.FileOffset,
                              /*Pad=*/true,
                              /*Prefix=*/"");

        const auto PrintRange = (!Mapping.isEmpty() && Options.Verbose);
        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.FileOffset,
                                           Mapping.Size,
                                           " (",
                                           ")");
        }

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("\n\t\tAddress: ", Options.OutFile),
                                 LENGTH_OF("\n\t\t") + LongestKeyLength);

        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.Address,
                              /*Pad=*/true,
                              /*Prefix=*/"");

        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.Address,
                                           Mapping.Size,
                                           " (",
                                           ")");
        }

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("\n\t\tSize: ", Options.OutFile),
                                 LENGTH_OF("\n\t\t") + LongestKeyLength);

        PrintSize(Options.OutFile,
                  Mapping.Size,
                  Options.Verbose,
                  /*Suffix=*/"\n");

        PrintUtilsRightPadSpaces(
            Options.OutFile,
            fputs("\t\tSlide-Info File Offset: ", Options.OutFile),
            LENGTH_OF("\t\t") + LongestKeyLength);

        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.SlideInfoFileOffset,
                              /*Pad=*/true,
                              /*Prefix=*/"");

        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.SlideInfoFileOffset,
                                           Mapping.SlideInfoFileSize,
                                           " (",
                                           ")");
        }

        PrintUtilsRightPadSpaces(
            Options.OutFile,
            fputs("\n\t\tSlide-Info File Size: ", Options.OutFile),
            LENGTH_OF("\n\t\t") + LongestKeyLength);

        PrintSize(Options.OutFile,
                  Mapping.SlideInfoFileSize,
                  Options.Verbose,
                  /*Suffix=*/"\n");

        Index++;
    }
}

static void
PrintDscHeaderV0Info(const struct PrintHeaderOperation::Options &Options,
                     const DscMemoryObject &Object) noexcept
{
    const auto Header = Object.getHeaderV0();

    PrintDscKey(Options.OutFile, "Magic");
    fprintf(Options.OutFile, "\"" CHAR_ARR_FMT(16) "\"", Header.Magic);

    if (Options.Verbose) {
        auto CpuKind = Mach::CpuKind::Any;
        auto CpuSubKind = int32_t();

        Object.retrieveCpuAndSubKind(CpuKind, CpuSubKind);

        const auto FullNameOpt =
            Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind);

        if (FullNameOpt.has_value()) {
            fprintf(Options.OutFile,
                    " (Cpu-Kind: %s)\n",
                    FullNameOpt.value().data());
        } else {
            fputs(" (Cpu-Kind: unrecognized)\n", Options.OutFile);
        }
    } else {
        fputc('\n', Options.OutFile);
    }

    const auto Version = Object.getVersion();

    PrintDscKey(Options.OutFile, "Version");
    fprintf(Options.OutFile, "%d\n", static_cast<int>(Version));

    PrintDscKey(Options.OutFile, "Mapping Offset");
    PrintUtilsWriteOffset(Options.OutFile,
                          Header.MappingOffset,
                          false,
                          "",
                          "\n");

    PrintDscKey(Options.OutFile, "Mapping Count");
    PrintUtilsWriteFormattedNumber(Options.OutFile,
                                   Header.MappingCount,
                                   /*Prefix=*/"",
                                   /*Suffix=*/"\n");

    if (Object.getMappingCount() <= 10) {
        fputs("Mappings: \n", Options.OutFile);
        PrintMappingInfoList(Options, Object);
    }

    PrintDscKey(Options.OutFile, "Images Offset");
    PrintUtilsWriteOffset(Options.OutFile,
                          Object.getImagesOffset(),
                          false,
                          "",
                          "\n");

    PrintDscKey(Options.OutFile, "Images Count");
    PrintUtilsWriteFormattedNumber(Options.OutFile,
                                   Object.getImageCount(),
                                   /*Prefix=*/"",
                                   /*Suffix=*/"\n");

    PrintDscKey(Options.OutFile, "Dyld Base-Address");
    PrintUtilsWriteOffset(Options.OutFile,
                          Header.DyldBaseAddress,
                          false,
                          "",
                          "\n");
}

static void
PrintDscHeaderV1Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV1();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Code-Signature Offset",
                      "Code-Signature Size",
                      Header.CodeSignatureOffset,
                      Header.CodeSignatureSize,
                      Options.Verbose,
                      /*Prefix=*/"",
                      /*Suffix=*/"",
                      /*IsOffset=*/true);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::CodeSignature);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Slide-Info Offset",
                      "Slide-Info Size",
                      Header.SlideInfoOffset,
                      Header.SlideInfoSize,
                      Options.Verbose,
                      /*Prefix=*/"",
                      /*Suffix=*/"",
                      /*IsOffset=*/true);

    PrintDscRangeOverlapsErrorOrNewline(Options, List, DscRangeKind::SlideInfo);
    fputc('\n', Options.OutFile);
}

static void
PrintDscHeaderV2Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV2();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Local-Symbols Offset",
                      "Local-Symbols Size",
                      Header.LocalSymbolsOffset,
                      Header.LocalSymbolsSize,
                      Options.Verbose,
                      /*Prefix=*/"",
                      /*Suffix=*/"",
                      /*IsOffset=*/true);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::LocalSymbolInfo);

    PrintDscKey(Options.OutFile, "Uuid");
    PrintUtilsWriteUuid(Options.OutFile, Header.Uuid, "", "\n\n");
}

template <std::integral OffsetType, std::integral SizeType>
static void
PrintOffsetCountPair(FILE *const OutFile,
                     const Range &Range,
                     const char *const OffsetKey,
                     const char *const CountKey,
                     const OffsetType Offset,
                     const SizeType Count,
                     const char *const Suffix = "")
{
    PrintDscKey(OutFile, OffsetKey);

    PrintUtilsWriteOffset(OutFile, Offset);
    WarnIfOutOfRange(OutFile, Range, Offset, 1);

    PrintDscKey(OutFile, CountKey);
    PrintUtilsWriteFormattedNumber(OutFile, Count);

    fprintf(OutFile, "%s", Suffix);
}

static void
PrintDscHeaderV3Info(
    const DscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    PrintDscKey(Options.OutFile, "Cache-Kind");

    const auto &Header = Object.getHeaderV3();
    switch (Header.Kind) {
        case DyldSharedCache::CacheKind::Development:
            fputs("Development\n", Options.OutFile);
            return;
        case DyldSharedCache::CacheKind::Production:
            fputs("Production\n", Options.OutFile);
            return;
    }

    fputs("<Unrecognized>\n", Options.OutFile);
}

static void
PrintDscHeaderV4Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV4();
    PrintOffsetCountPair(Options.OutFile,
                         Object.getRange(),
                         "Branch-Pools Offset",
                         "Branch-Pools Count",
                         Header.BranchPoolsOffset,
                         Header.BranchPoolsCount,
                         "\n");

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Accelerate-Info Address",
                      "Accelerate-Info Size",
                      Header.AccelerateInfoAddr,
                      Header.AccelerateInfoSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::AcceleratorInfo);

    PrintOffsetCountPair(Options.OutFile,
                         Object.getRange(),
                         "Images-Text Offset",
                         "Images-Text Count",
                         Header.ImagesTextOffset,
                         Header.ImagesTextCount);

    if (Header.ImagesTextCount != Object.getImageCount()) {
        fprintf(Options.OutFile,
                " (Invalid: %" PRIu64 " Image-Texts vs %" PRIu32 " Images",
                Header.ImagesTextCount,
                Object.getImageCount());
    }

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::ImageTextInfoList);

    fputc('\n', Options.OutFile);
}

static inline void
PrintBoolValue(FILE *const OutFile,
               const char *const Key,
               const bool Value) noexcept
{
    PrintDscKey(OutFile, Key);

    fputs((Value) ? "true" : "false", OutFile);
    fputc('\n', OutFile);
}

static inline void
PrintPlatformKindValue(FILE *const OutFile,
                       const char *const Key,
                       const Dyld3::PlatformKind Platform)
{
    PrintDscKey(OutFile, Key);

    const auto PlatformDescOpt = Dyld3::PlatformKindGetDescription(Platform);
    const auto PlatformDesc =
        PlatformDescOpt.has_value() ?
            std::string(PlatformDescOpt.value()) :
            std::format("<unknown (0x{:02x})>",
                        static_cast<uint32_t>(Platform));

    fprintf(OutFile, "%s\n", PlatformDesc.c_str());
}

static void
PrintDscHeaderV5Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV5();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Dylibs Image-Group Address",
                      "Dylibs Image-Group Size",
                      Header.DylibsImageGroupAddr,
                      Header.DylibsImageGroupSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::DylibsImageGroup);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Image-Group Address",
                      "Other Image-Group Size",
                      Header.OtherImageGroupAddr,
                      Header.OtherImageGroupSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::OtherImageGroup);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Program Closures Address",
                      "Program Closures Size",
                      Header.ProgClosuresAddr,
                      Header.ProgClosuresSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::ProgClosures);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Program Closures Trie Address",
                      "Program Closures Trie Size",
                      Header.ProgClosuresTrieAddr,
                      Header.ProgClosuresTrieSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::ProgClosuresTrie);

    PrintPlatformKindValue(Options.OutFile, "Platform", Header.getPlatform());

    PrintDscKey(Options.OutFile, "Closure-Format Version");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.FormatVersion);

    PrintBoolValue(Options.OutFile,
                   "Dylibs Expected On Disk",
                   Header.DylibsImageGroupSize);

    PrintBoolValue(Options.OutFile, "Simulator", Header.Simulator);
    PrintBoolValue(Options.OutFile,
                   "Locally-Built Cache",
                   Header.LocallyBuiltCache);

    PrintBoolValue(Options.OutFile,
                   "Built From Chained-Fixups",
                   Header.BuiltFromChainedFixups);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Shared-Region Start",
                      "Shared-Region Size",
                      Header.SharedRegionStart,
                      Header.SharedRegionSize,
                      Options.Verbose,
                      /*Prefix=*/"",
                      /*Suffix=*/"",
                      /*IsOffset=*/false,
                      /*PrintNewLine=*/true);

    PrintDscKey(Options.OutFile, "Max Slide");
    PrintSize(Options.OutFile, Header.MaxSlide, Options.Verbose, "\n");

    fputc('\n', Options.OutFile);
}

static void
PrintDscHeaderV6Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV6();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Dylibs Image-Array Address",
                      "Dylibs Image-Array Size",
                      Header.DylibsImageArrayAddr,
                      Header.DylibsImageArraySize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::DylibsImageGroup);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Dylibs Trie Address",
                      "Dylibs Trie Size",
                      Header.DylibsTrieAddr,
                      Header.DylibsTrieSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::DylibsTrie);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Image Array Address",
                      "Other Image Array Size",
                      Header.OtherImageArrayAddr,
                      Header.OtherImageArraySize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::OtherImageArray);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Trie Address",
                      "Other Trie Size",
                      Header.OtherTrieAddr,
                      Header.OtherTrieSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options, List, DscRangeKind::OtherTrie);
    fputc('\n', Options.OutFile);
}

static void
PrintDscHeaderV7Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &) noexcept
{
    const auto &Header = Object.getHeaderV7();
    PrintOffsetCountPair(Options.OutFile,
                         Object.getRange(),
                         "Mapping With Slide Info Address",
                         "Mapping With Slide Info Count",
                         Header.MappingWithSlideOffset,
                         Header.MappingWithSlideCount,
                         /*Suffix=*/"\n");

    if (Header.MappingWithSlideCount <= 10) {
        fputs("Mappings With Slide Info: \n", Options.OutFile);
        PrintMappingWithSlideInfoList(Options, Object);
    }

    fputc('\n', Options.OutFile);
}

static void
PrintPackedVersion(FILE *const OutFile,
                   const char *const Key,
                   const Dyld3::PackedVersion &Version)
{
    PrintDscKey(OutFile, Key);
    MachOTypePrinter<Dyld3::PackedVersion>::Print(OutFile,
                                                  Version,
                                                  "",
                                                  "\n");
}

static void
PrintDscHeaderV8Info(const DscMemoryObject &Object,
                     const struct PrintHeaderOperation::Options &Options,
                     const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV8();

    PrintDscKey(Options.OutFile, "Dylibs Prebuilt Loader Set Address");
    PrintUtilsWriteOffset(Options.OutFile,
                          Header.DylibsPBLSetAddr,
                          /*pad=*/false,
                          "",
                          "\n");

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Programs Prebuilt Loader Set Pool Address",
                      "Programs Prebuilt Loader Set Pool Size",
                      Header.ProgramsPBLSetPoolAddr,
                      Header.ProgramsPBLSetPoolSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::ProgramsPBLSetPool);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Program Trie Address",
                      "Program Trie Size",
                      Header.ProgramTrieAddr,
                      Header.ProgramTrieSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::ProgramTrie);

    PrintPackedVersion(Options.OutFile, "OS Version", Header.getOsVersion());
    PrintPlatformKindValue(Options.OutFile,
                           "Alternate Platform",
                           Header.getAltPlatform());

    PrintPackedVersion(Options.OutFile,
                       "Alternate OS Version",
                       Header.getAltOsVersion());

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Swift Optimizations Offset",
                      "Swift Optimizations Size",
                      Header.SwiftOptsOffset,
                      Header.SwiftOptsSize,
                      Options.Verbose,
                      /*Prefix=*/"",
                      /*Suffix=*/"",
                      /*IsOffset=*/true);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::SwiftOpts);

    PrintOffsetCountPair(Options.OutFile,
                         Object.getRange(),
                         "SubCache Array Offset",
                         "SubCache Array Count",
                         Header.SubCacheArrayOffset,
                         Header.SubCacheArrayCount,
                         /*Suffix=*/"\n");

    PrintDscKey(Options.OutFile, "Symbol File UUID");
    PrintUtilsWriteUuid(Options.OutFile, Header.SymbolFileUUID, "", "\n");

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Rosetta Read-Only Address",
                      "Rosetta Read-Only Size",
                      Header.RosettaReadOnlyAddr,
                      Header.RosettaReadOnlySize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::RosettaReadOnly);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Rosetta Read-Write Address",
                      "Rosetta Read-Write Size",
                      Header.RosettaReadWriteAddr,
                      Header.RosettaReadWriteSize,
                      Options.Verbose);

    // HACK: Temp fix to bad output
    fflush(Options.OutFile);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::RosettaReadWrite);
}

constexpr static auto LongestAccKey = LENGTH_OF("Bottom-Up List Offset");
static inline void
PrintAcceleratorKey(FILE *const OutFile,
                    const char *const Key,
                    const char *const Prefix = "") noexcept
{
    fputs(Prefix, OutFile);
    fflush(OutFile);

    PrintUtilsRightPadSpaces(OutFile,
                             fprintf(OutFile, "%s: ", Key),
                             LongestAccKey + LENGTH_OF(": "));
}

static void
WriteAcceleratorOffsetCountPair(FILE *const OutFile,
                                const Range &Range,
                                const char *OffsetName,
                                const char *CountName,
                                uint32_t Offset,
                                uint32_t Count) noexcept
{
    PrintAcceleratorKey(OutFile, OffsetName);
    PrintUtilsWriteOffset(OutFile, Offset);

    WarnIfOutOfRange(OutFile, Range, Offset, 1);

    PrintAcceleratorKey(OutFile, CountName);
    PrintUtilsWriteFormattedNumber(OutFile, Count, "", "\n");
}

static void
PrintAcceleratorInfo(
    FILE *const OutFile,
    const Range &Range,
    const DyldSharedCache::AcceleratorInfo &Info,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    fputs("Apple Dyld Shared-Cache Accelerator Info\n", OutFile);

    PrintAcceleratorKey(OutFile, "Version");
    fprintf(OutFile, "%" PRIu32 "\n", Info.Version);

    PrintAcceleratorKey(OutFile, "Image-Extras Count");
    PrintUtilsWriteFormattedNumber(Options.OutFile,
                                   Info.ImageExtrasCount,
                                   /*Prefix=*/"",
                                   /*Suffix=*/"\n");

    PrintAcceleratorKey(OutFile, "Image-Extras Offset");
    PrintUtilsWriteOffset(OutFile, Info.ImageExtrasOffset, true, "", "\n");

    PrintAcceleratorKey(OutFile, "Bottom-Up List-Offset");
    PrintUtilsWriteOffset(OutFile, Info.BottomUpListOffset, true,  "", "\n");

    PrintDscSizeRange<PrintAcceleratorKey>(OutFile,
                                           Range,
                                           "Dylib-Trie Offset",
                                           "Dylib-Trie Size",
                                           Info.DylibTrieOffset,
                                           Info.DylibTrieSize,
                                           Options.Verbose,
                                           /*Prefix=*/"",
                                           /*Suffix=*/"",
                                           /*IsOffset=*/false,
                                           /*PrintNewLine=*/true);

    WriteAcceleratorOffsetCountPair(OutFile,
                                    Range,
                                    "Initializers Offset",
                                    "Initializers Count",
                                    Info.InitializersOffset,
                                    Info.InitializersCount);

    WriteAcceleratorOffsetCountPair(OutFile,
                                    Range,
                                    "Dof-Sections Offset",
                                    "Dof-Sections Count",
                                    Info.DofSectionsOffset,
                                    Info.DofSectionsCount);

    WriteAcceleratorOffsetCountPair(OutFile,
                                    Range,
                                    "Re-Export List Offset",
                                    "Re-Export Count",
                                    Info.ReExportListOffset,
                                    Info.ReExportCount);

    WriteAcceleratorOffsetCountPair(OutFile,
                                    Range,
                                    "Dep List Offset",
                                    "Dep List Count",
                                    Info.DepListOffset,
                                    Info.DepListCount);

    WriteAcceleratorOffsetCountPair(OutFile,
                                    Range,
                                    "Range Table Offset",
                                    "Range Table Count",
                                    Info.RangeTableOffset,
                                    Info.RangeTableCount);

    PrintAcceleratorKey(OutFile, "Dyld Section-Address");
    PrintUtilsWriteOffset(OutFile, Info.DyldSectionAddr);
}

int
PrintHeaderOperation::Run(const DscMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    fputs("Apple Dyld Shared-Cache File\n", Options.OutFile);

    const auto Version = Object.getVersion();
    const auto List = CollectDscRangeList(Object.getHeader(), Version);

    PrintDscHeaderV0Info(Options, Object);
    if (Version < DyldSharedCache::HeaderVersion::V1) {
        return 0;
    }

    PrintDscHeaderV1Info(Object, Options, List);
    if (Version < DyldSharedCache::HeaderVersion::V2) {
        return 0;
    }

    PrintDscHeaderV2Info(Object, Options, List);
    if (Version < DyldSharedCache::HeaderVersion::V3) {
        return 0;
    }

    PrintDscHeaderV3Info(Object, Options);
    if (Version < DyldSharedCache::HeaderVersion::V4) {
        return 0;
    }

    PrintDscHeaderV4Info(Object, Options, List);
    if (Version < DyldSharedCache::HeaderVersion::V5) {
        goto done;
    }

    PrintDscHeaderV5Info(Object, Options, List);
    if (Version < DyldSharedCache::HeaderVersion::V6) {
        goto done;
    }

    PrintDscHeaderV6Info(Object, Options, List);
    if (Version < DyldSharedCache::HeaderVersion::V7) {
        goto done;
    }

    PrintDscHeaderV7Info(Object, Options, List);
    if (Version < DyldSharedCache::HeaderVersion::V8) {
        goto done;
    }

    PrintDscHeaderV8Info(Object, Options, List);

done:
    if (const auto Info = Object.getHeaderV4().GetAcceleratorInfo()) {
        fputc('\n', Options.OutFile);
        PrintAcceleratorInfo(Options.OutFile,
                             Object.getRange(),
                             *Info,
                             Options);
    }

    return 0;
}

auto
PrintHeaderOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                       int *IndexOut) noexcept
    -> struct PrintHeaderOperation::Options
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (!Argument.isOption()) {
            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }

        Index++;
    }

    if (IndexOut != nullptr) {
        *IndexOut = Index;
    }

    return Options;
}

int PrintHeaderOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int PrintHeaderOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
            return Run(cast<ObjectKind::FatMachO>(Object), Options);
        case ObjectKind::DyldSharedCache:
            return Run(cast<ObjectKind::DyldSharedCache>(Object), Options);
        case ObjectKind::DscImage:
            return Run(cast<ObjectKind::DscImage>(Object), Options);
    }

    assert(0 && "Unrecognized Object-Kind");
}
