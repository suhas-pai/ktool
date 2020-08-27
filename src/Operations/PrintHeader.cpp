//
//  src/Operations/PrintHeader.cpp
//  ktool
//
//  Created by Suhas Pai on 4/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "Utils/MachOPrinter.h"
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintHeader.h"

PrintHeaderOperation::PrintHeaderOperation() noexcept : Operation(OpKind) {}
PrintHeaderOperation::PrintHeaderOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

int
PrintHeaderOperation::Run(const ConstMachOMemoryObject &Object,
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

    const auto MagicDesc = MachO::Header::MagicGetDescription(Magic).data();
    const auto FileKindName = MachO::Header::FileKindGetName(FileKind).data();
    const auto FileKindDesc =
        MachO::Header::FileKindGetDescription(FileKind).data();

    const auto CpuKindValue = static_cast<int32_t>(CpuKind);
    const auto FileKindValue = static_cast<uint32_t>(FileKind);
    const auto FlagsValue = Flags.value();

    if (Options.Verbose) {
        const auto MagicName = MachO::Header::MagicGetName(Magic).data();
        const auto MagicValue = static_cast<uint32_t>(Magic);

        fprintf(Options.OutFile,
                "Magic:\n"
                "\tName:        %s\n"
                "\tDescription: %s\n"
                "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                MagicName,
                MagicDesc,
                MagicValue,
                MagicValue);

        const auto CpuKindName = Mach::CpuKindGetName(CpuKind).data();
        if (CpuKindName != nullptr) {
            const auto CpuKindBrandName =
                Mach::CpuKindGetBrandName(CpuKind).data();
            const auto CpuSubKindName =
                Mach::CpuSubKind::GetName(CpuKind, CpuSubKind).data();
            const auto CpuSubKindBrandName =
                Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind).data();

            fprintf(Options.OutFile,
                    "CpuType:\n"
                    "\tName:        %s\n"
                    "\tDescription: %s\n"
                    "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                    CpuKindName,
                    CpuKindBrandName,
                    CpuKindValue,
                    CpuKindValue);

            if (CpuSubKindName != nullptr) {
                fprintf(Options.OutFile,
                        "CpuSubType:\n"
                        "\tName:        %s\n"
                        "\tDescription: %s\n"
                        "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                        CpuSubKindName,
                        CpuSubKindBrandName,
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

        if (FileKindName != nullptr) {
            fprintf(Options.OutFile,
                    "FileType:\n"
                    "\tName:        %s\n"
                    "\tDescription: %s\n"
                    "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                    FileKindName,
                    FileKindDesc,
                    FileKindValue,
                    FileKindValue);
        } else {
            fprintf(Options.OutFile,
                    "FileType:\n"
                    "\tUnrecognized\n\tValue: %" PRIu32 " (0x%" PRIX32 ")\n",
                    FileKindValue,
                    FileKindValue);
        }

        fprintf(Options.OutFile, "Ncmds: %" PRIu32 "\n", Ncmds);
        fprintf(Options.OutFile, "SizeOfCmds: %" PRIu32, SizeOfCmds);

        PrintUtilsWriteFormattedSize(Options.OutFile,
                                     SizeOfCmds,
                                     " (",
                                     ")");

        const auto FlagInfoList = OperationCommon::GetFlagInfoList(Flags);

        fputc('\n', Options.OutFile);
        fprintf(Options.OutFile,
                "Flags:\n"
                "\tNumber: %" PRIu32 " (0x%X)\n"
                "\tCount:  %" PRIuPTR "\n"
                "\tValues:\n",
                FlagsValue,
                FlagsValue,
                FlagInfoList.size());

        OperationCommon::PrintFlagInfoList(Options.OutFile,
                                           FlagInfoList,
                                           Options.Verbose,
                                           "\t\t");
    } else {
        fprintf(Options.OutFile, "Magic:      %s\n", MagicDesc);

        const auto CpuBrandName = Mach::CpuKindGetBrandName(CpuKind).data();
        if (CpuBrandName != nullptr) {
            fprintf(Options.OutFile, "CpuType:    %s\n", CpuBrandName);
        } else {
            fprintf(Options.OutFile,
                    "CpuType:    Unrecognized (Value: %" PRIu32 ")\n",
                    CpuKindValue);
        }

        const auto SubKindFullName =
            Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind).data();

        if (SubKindFullName != nullptr) {
            fprintf(Options.OutFile, "CpuSubType: %s\n", SubKindFullName);
        } else {
            fprintf(Options.OutFile,
                    "CpuSubType: Unrecognized (Value: %" PRIu32 ")\n",
                    CpuSubKind);
        }

        if (FileKindDesc != nullptr) {
            fprintf(Options.OutFile, "FileType:   %s\n", FileKindDesc);
        } else {
            fprintf(Options.OutFile,
                    "FileType:   Unrecognized (%" PRIu32 ")\n",
                    FileKindValue);
        }

        fprintf(Options.OutFile,
                "Ncmds:      %" PRIu32 "\n"
                "SizeOfCmds: %" PRIu32 "\n",
                Ncmds,
                SizeOfCmds);

        fprintf(Options.OutFile, "Flags:      0x%X\n", FlagsValue);
    }

    return 0;
}

int
PrintHeaderOperation::Run(const ConstFatMachOMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    fputs("Mach-O Multi-Architecture (FAT) File\n", stdout);

    const auto &Header = Object.getConstHeader();
    const auto &Magic = Header.Magic;
    const auto MagicDesc = MachO::FatHeader::MagicGetDescription(Magic).data();

    if (Options.Verbose) {
        const auto MagicValue = static_cast<uint32_t>(Magic);
        const auto MagicName = MachO::FatHeader::MagicGetName(Magic).data();

        fprintf(Options.OutFile,
                "Magic:\n"
                "\tName: %s\n"
                "\tDescription: %s\n"
                "\tValue: %" PRIu32 " (0x%" PRIX32 ")\n",
                MagicName,
                MagicDesc,
                MagicValue,
                MagicValue);
    } else {
        fprintf(Options.OutFile, "Magic: %s\n", MagicDesc);
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

    End,
};

struct DscRange {
    DscRangeKind Kind;
    LocationRange Range;
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
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::ImageInfoList: {
            if (const auto Range = Header.getImageInfoListRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ImageInfoList,
                    .Range = Range.value()
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
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::SlideInfo: {
            if (const auto Range = Header.getSlideInfoRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::SlideInfo,
                    .Range = Range.value()
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
                    .Range = Range.value()
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
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::ImageTextInfoList: {
            if (const auto Range = Header.getImageTextInfoListRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ImageTextInfoList,
                    .Range = Range.value()
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
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::OtherImageGroup: {
            if (const auto Range = Header.getOtherImageGroupRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherImageGroup,
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::ProgClosures: {
            if (const auto Range = Header.getProgClosuresRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgClosures,
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::ProgClosuresTrie: {
            if (const auto Range = Header.getProgClosuresTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgClosuresTrie,
                    .Range = Range.value()
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
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::DylibsTrie: {
            if (const auto Range = Header.getDylibsTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsTrie,
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::OtherImageArray: {
            if (const auto Range = Header.getOtherImageArrayRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherImageArray,
                    .Range = Range.value()
                });
            }
        }

        case DscRangeKind::OtherTrie: {
            if (const auto Range = Header.getOtherTrieRange()) {
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherTrie,
                    .Range = Range.value()
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
    DscRangeKind Kind) noexcept
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
                break;
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
            case DscRangeKind::End:
                assert(0 && "Overlap-Kind End is set");
                break;
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

static constexpr auto LongestDscKey =
    LENGTH_OF("Program Closures Trie Address");

static void PrintDscKey(FILE *OutFile, const char *Key) noexcept {
    PrintUtilsRightPadSpaces(OutFile,
                             fprintf(OutFile, "%s: ", Key),
                             LongestDscKey + LENGTH_OF(": "));
}

static void
WarnIfOutOfRange(FILE *OutFile,
                 const RelativeRange &Range,
                 const uint64_t Offset,
                 const uint64_t Size,
                 bool PrintNewLine = true) noexcept
{
    const auto LocRange = LocationRange::CreateWithSize(Offset, Size);
    if (!LocRange || !Range.containsLocRange(LocRange.value())) {
        fprintf(OutFile, " (Past EOF!)");
    }

    if (PrintNewLine) {
        fputc('\n', OutFile);
    }
}

static void
PrintMappingInfoList(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object)
{
    const auto MappingCountDigitLength =
        PrintUtilsGetIntegerDigitLength(Object.getMappingCount());
    
    auto Index = static_cast<int>(1);
    for (const auto &Mapping : Object.getMappingInfoList()) {
        fprintf(Options.OutFile,
                "\tMapping %0*d: " MEM_PROT_INIT_MAX_RNG_FMT " \n",
                MappingCountDigitLength,
                Index,
                MEM_PROT_INIT_MAX_RNG_FMT_ARGS(Mapping.getInitProt(),
                                               Mapping.getMaxProt()));
        
        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.FileOffset,
                              true,
                              "\t\tFile-Offset: ");

        const auto PrintRange = (Mapping.Size != 0 && Options.Verbose);
        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.FileOffset,
                                           Mapping.Size,
                                           " (",
                                           ")");
        }
        
        PrintUtilsWriteOffset(Options.OutFile,
                              Mapping.Address,
                              true,
                              "\n\t\tAddress:     ");

        if (PrintRange) {
            PrintUtilsWriteOffsetSizeRange(Options.OutFile,
                                           Mapping.Address,
                                           Mapping.Size,
                                           " (",
                                           ")");
        }
        
        const auto RightPad = LENGTH_OF("\n\t\tSize:        ") + OFFSET_64_LEN;
        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fprintf(Options.OutFile,
                                         "\n\t\tSize:        %" PRIu64,
                                         Mapping.Size),
                                 RightPad);

        if (Options.Verbose) {
            PrintUtilsWriteFormattedSize(Options.OutFile,
                                         Mapping.Size,
                                         " (",
                                         ")");
        }

        fputc('\n', Options.OutFile);
        Index++;
    }
}

static void
PrintDscHeaderV0Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    auto CpuKind = Mach::CpuKind::Any;
    auto CpuSubKind = int32_t();

    Object.getCpuKind(CpuKind, CpuSubKind);
    const auto Header = Object.getHeaderV0();

    PrintDscKey(Options.OutFile, "Magic");
    fprintf(Options.OutFile,
            "\"" CHAR_ARR_FMT(16) "\"",
            Header.Magic);

    if (Options.Verbose) {
        fprintf(Options.OutFile,
                " (Cpu-Kind: %s)\n",
                Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind).data());
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
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.MappingCount);

    if (Object.getMappingCount() <= 10) {
        PrintMappingInfoList(Options, Object);
    }

    PrintDscKey(Options.OutFile, "Images Offset");
    PrintUtilsWriteOffset(Options.OutFile,
                          Header.ImagesOffset,
                          false,
                          "",
                          "\n");

    PrintDscKey(Options.OutFile, "Images Count");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.ImagesCount);

    PrintDscKey(Options.OutFile, "Dyld Base-Address");
    PrintUtilsWriteOffset(Options.OutFile,
                          Header.DyldBaseAddress,
                          false,
                          "",
                          "\n");
}

template <void (*PrintKeyFunc)(FILE *, const char *) = PrintDscKey, typename T>
static inline void
PrintDscSizeRange(FILE *OutFile,
                  const RelativeRange &DscRange,
                  const char *AddressName,
                  const char *SizeName,
                  T Address,
                  T Size,
                  bool Verbose,
                  bool IsOffset = false,
                  bool PrintNewLine = false) noexcept
{
    static_assert(std::is_integral_v<T>);
    constexpr auto Is64Bit = std::is_same_v<T, uint64_t>;

    PrintKeyFunc(OutFile, AddressName);
    PrintUtilsWriteOffset(OutFile, Address);

    const auto VerboseAndNonZeroSize = (Verbose && Size != 0);
    if (VerboseAndNonZeroSize) {
        PrintUtilsWriteOffsetSizeRange(OutFile, Address, Size, " (", ")");
    }

    if (IsOffset) {
        WarnIfOutOfRange(OutFile, DscRange, Address, Size, false);
    }

    fputc('\n', OutFile);
    PrintKeyFunc(OutFile, SizeName);

    auto SizeWrittenOut = 0;
    auto SizeMaxLength = 0;

    if constexpr (Is64Bit) {
        SizeWrittenOut = fprintf(OutFile, "%" PRIu64, Size);
        SizeMaxLength = OFFSET_64_LEN;
    } else {
        SizeWrittenOut = fprintf(OutFile, "%" PRIu32, Size);
        SizeMaxLength = OFFSET_32_LEN;
    }

    if (VerboseAndNonZeroSize) {
        PrintUtilsRightPadSpaces(OutFile, SizeWrittenOut, SizeMaxLength);
        PrintUtilsWriteFormattedSize(OutFile, Size, " (", ")");
    }

    if (PrintNewLine) {
        fputc('\n', OutFile);
    }
}

static void
PrintDscHeaderV1Info(
    const ConstDscMemoryObject &Object,
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
                      true);

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
                      true);

    PrintDscRangeOverlapsErrorOrNewline(Options, List, DscRangeKind::SlideInfo);
    fputc('\n', Options.OutFile);
}

static void
PrintDscHeaderV2Info(
    const ConstDscMemoryObject &Object,
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
                      true);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::LocalSymbolInfo);

    PrintDscKey(Options.OutFile, "Uuid");
    PrintUtilsWriteUuid(Options.OutFile, Header.Uuid, "", "\n\n");
}

static void
PrintDscHeaderV3Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    PrintDscKey(Options.OutFile, "Cache-Kind");

    const auto &Header = Object.getHeaderV3();
    switch (Header.Kind) {
        case DyldSharedCache::CacheKind::Development:
            fputs("Development\n", Options.OutFile);
            goto done;
        case DyldSharedCache::CacheKind::Production:
            fputs("Production\n", Options.OutFile);
            goto done;
    }

    fputs("<Unrecognized>", Options.OutFile);
done:
    fputc('\n', Options.OutFile);
}

static void
PrintDscHeaderV4Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options,
    const DscRangeList &List) noexcept
{
    const auto &Header = Object.getHeaderV4();

    PrintDscKey(Options.OutFile, "Branch-Pools Offset");
    PrintUtilsWriteOffset(Options.OutFile, Header.BranchPoolsOffset);

    WarnIfOutOfRange(Options.OutFile,
                     Object.getRange(),
                     Header.BranchPoolsOffset,
                     1);

    PrintDscKey(Options.OutFile, "Branch-Pools Count");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.BranchPoolsCount);

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

    PrintDscKey(Options.OutFile, "Images-Text Offset");
    PrintUtilsWriteOffset(Options.OutFile, Header.ImagesTextOffset);

    WarnIfOutOfRange(Options.OutFile,
                     Object.getRange(),
                     Header.ImagesTextOffset,
                     1);

    PrintDscKey(Options.OutFile, "Images-Text Count");
    fprintf(Options.OutFile, "%" PRIu64, Header.ImagesTextCount);

    if (Header.ImagesTextCount != Header.ImagesCount) {
        fprintf(Options.OutFile,
                " (Invalid: %" PRIu64 " Image-Texts vs %" PRIu32 " Images",
                Header.ImagesTextCount,
                Header.ImagesCount);
    }

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::ImageTextInfoList);

    fputc('\n', Options.OutFile);
}

static
void PrintBoolValue(FILE *OutFile, const char *Key, bool Value) noexcept {
    PrintDscKey(OutFile, Key);

    fputs((Value) ? "true" : "false", OutFile);
    fputc('\n', OutFile);
}

static void
PrintDscHeaderV5Info(
    const ConstDscMemoryObject &Object,
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

    PrintDscKey(Options.OutFile, "Platform");

    const auto Platform = MachO::PlatformKind(Header.Platform);
    auto PlatformStr = MachO::PlatformKindGetDescription(Platform).data();

    if (PlatformStr == nullptr) {
        PlatformStr = "<unrecognized>";
    }

    fprintf(Options.OutFile, "%s\n", PlatformStr);

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
                      false,
                      true);

    PrintDscKey(Options.OutFile, "Max Slide");
    const auto WrittenOut =
        fprintf(Options.OutFile, "%" PRIu64, Header.MaxSlide);

    if (Options.Verbose) {
        PrintUtilsRightPadSpaces(Options.OutFile, WrittenOut, OFFSET_64_LEN);
        PrintUtilsWriteFormattedSize(Options.OutFile,
                                     Header.MaxSlide,
                                     " (",
                                     ")");
    }

    fputs("\n\n", Options.OutFile);
}

static void
PrintDscHeaderV6Info(
    const ConstDscMemoryObject &Object,
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
                      Header.DylibsImageArrayAddr,
                      Header.DylibsImageArraySize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options,
                                        List,
                                        DscRangeKind::OtherImageArray);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Trie Address",
                      "Other Trie Size",
                      Header.DylibsTrieAddr,
                      Header.DylibsTrieSize,
                      Options.Verbose);

    PrintDscRangeOverlapsErrorOrNewline(Options, List, DscRangeKind::OtherTrie);
}

static constexpr auto LongestAccKey = LENGTH_OF("Bottom-Up List Offset");
static void PrintAcceleratorKey(FILE *OutFile, const char *Key) noexcept {
    PrintUtilsRightPadSpaces(OutFile,
                             fprintf(OutFile, "%s: ", Key),
                             LongestAccKey + LENGTH_OF(": "));
}

static void
WriteAcceleratorOffsetCountPair(FILE *OutFile,
                                const RelativeRange &Range,
                                const char *OffsetName,
                                const char *CountName,
                                uint32_t Offset,
                                uint32_t Count) noexcept
{
    PrintAcceleratorKey(OutFile, OffsetName);
    PrintUtilsWriteOffset(OutFile, Offset);

    WarnIfOutOfRange(OutFile, Range, Offset, 1);

    PrintAcceleratorKey(OutFile, CountName);
    fprintf(OutFile, "%" PRIu32 "\n", Count);
}

static void
PrintAcceleratorInfo(
    FILE *OutFile,
    const RelativeRange &Range,
    const DyldSharedCache::AcceleratorInfo &Info,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    fputs("Apple Dyld Shared-Cache Accelerator Info\n", OutFile);

    PrintAcceleratorKey(OutFile, "Version");
    fprintf(OutFile, "%" PRIu32 "\n", Info.Version);

    PrintAcceleratorKey(OutFile, "Image-Extras Count");
    fprintf(OutFile, "%" PRIu32 "\n", Info.ImageExtrasCount);

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
                                           false,
                                           true);

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
PrintHeaderOperation::Run(const ConstDscMemoryObject &Object,
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

struct PrintHeaderOperation::Options
PrintHeaderOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                       int *IndexOut) noexcept
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (!Argument.IsOption()) {
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

void
PrintHeaderOperation::ParseOptions(const ArgvArray &Argv,
                                   int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
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
