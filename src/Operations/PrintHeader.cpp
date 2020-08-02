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

    const auto &Header = Object.getConstHeader();
    const auto IsBigEndian = Object.IsBigEndian();

    const auto Magic = Object.getMagic();
    const auto CpuKind = Object.getCpuKind();
    const auto CpuSubKind = SwitchEndianIf(Header.CpuSubKind, IsBigEndian);
    const auto FileKind = Object.getFileKind();
    const auto Ncmds = SwitchEndianIf(Header.Ncmds, IsBigEndian);
    const auto SizeOfCmds = SwitchEndianIf(Header.SizeOfCmds, IsBigEndian);
    const auto Flags = Header.getFlags();

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
            const auto CpuKindDesc = Mach::CpuKindGetDescripton(CpuKind).data();
            const auto CpuSubKindName =
                Mach::CpuSubKind::GetName(CpuKind, CpuSubKind).data();
            const auto CpuSubKindDesc =
                Mach::CpuSubKind::GetDescription(CpuKind, CpuSubKind).data();

            fprintf(Options.OutFile,
                    "CpuType:\n"
                    "\tName:        %s\n"
                    "\tDescription: %s\n"
                    "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                    CpuKindName,
                    CpuKindDesc,
                    CpuKindValue,
                    CpuKindValue);

            if (CpuSubKindName != nullptr) {
                fprintf(Options.OutFile,
                        "CpuSubType:\n"
                        "\tName:        %s\n"
                        "\tDescription: %s\n"
                        "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                        CpuSubKindName,
                        CpuSubKindDesc,
                        CpuSubKind,
                        CpuSubKind);
            } else {
                fprintf(Options.OutFile,
                        "CpuSubType:\n"
                        "\tUnrecognized\n"
                        "\tValue; %" PRIu32 " (0x%" PRIX32 ")\n",
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
                    "\tValue; %" PRIu32 " (Value: 0x%" PRIX32 ")\n",
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
    if (!LocRange || !Range.containsRange(LocRange.value())) {
        fprintf(OutFile, " (Past EOF!)");
    }

    if (PrintNewLine) {
        fputc('\n', OutFile);
    }
}

static DscMemoryObject::Version
PrintDscHeaderV0Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    auto CpuKind = Mach::CpuKind::Any;
    auto CpuSubKind = int32_t();

    Object.getCpuKind(CpuKind, CpuSubKind);
    const auto Header = Object.getHeaderV0();

    PrintDscKey(Options.OutFile, "Magic");
    fprintf(Options.OutFile,
            "\"" CHAR_ARR_FMT(16) "\" (Cpu-Kind: %s)\n",
            Header.Magic,
            Mach::CpuSubKind::GetFullName(CpuKind, CpuSubKind).data());

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

    return Version;
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
                  bool IsOffset = false) noexcept
{
    static_assert(std::is_integral_v<T>);
    constexpr auto Is64Bit = std::is_same_v<T, uint64_t>;

    PrintKeyFunc(OutFile, AddressName);
    PrintUtilsWriteOffset(OutFile, Address);

    if (Size != 0) {
        if (Verbose) {
            PrintUtilsWriteOffsetSizeRange(OutFile,
                                           Address,
                                           Size,
                                           " (",
                                           ")");
        }
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

    if (Verbose && Size != 0) {
        PrintUtilsRightPadSpaces(OutFile, SizeWrittenOut, SizeMaxLength);
        PrintUtilsWriteFormattedSize(OutFile, Size, " (", ")");
    }

    fputc('\n', OutFile);
}

static void
PrintDscHeaderV1Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    const auto Header = Object.getHeaderV1();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Code-Signature Offset",
                      "Code-Signature Size",
                      Header.CodeSignatureOffset,
                      Header.CodeSignatureSize,
                      Options.Verbose,
                      true);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Slide-Info Offset",
                      "Slide-Info Size",
                      Header.SlideInfoOffset,
                      Header.SlideInfoSize,
                      Options.Verbose,
                      true);
}

static void
PrintDscHeaderV2Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    const auto Header = Object.getHeaderV2();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Local-Symbols Offset",
                      "Local-Symbols Size",
                      Header.LocalSymbolsOffset,
                      Header.LocalSymbolsSize,
                      Options.Verbose,
                      true);

    PrintDscKey(Options.OutFile, "Uuid");
    PrintUtilsWriteUuid(Options.OutFile, Header.Uuid, "", "\n");
}

static void
PrintDscHeaderV3Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    PrintDscKey(Options.OutFile, "Cache-Kind");

    const auto Header = Object.getHeaderV3();
    switch (Header.Kind) {
        case DyldSharedCache::CacheKind::Development:
            fputs("Development\n", Options.OutFile);
            return;
        case DyldSharedCache::CacheKind::Production:
            fputs("Production\n", Options.OutFile);
            return;
    }

    fputs("<Unrecognized>", Options.OutFile);
}

static void
PrintDscHeaderV4Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    const auto Header = Object.getHeaderV4();

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

    PrintDscKey(Options.OutFile, "Images-Text Offset");
    PrintUtilsWriteOffset(Options.OutFile, Header.ImagesTextOffset);

    WarnIfOutOfRange(Options.OutFile,
                     Object.getRange(),
                     Header.ImagesTextOffset,
                     1);

    PrintDscKey(Options.OutFile, "Images-Text Count");
    fprintf(Options.OutFile, "%" PRIu64 "\n", Header.ImagesTextCount);
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
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    const auto Header = Object.getHeaderV5();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Dylibs Image-Group Address",
                      "Dylibs Image-Group Size",
                      Header.DylibsImageGroupAddr,
                      Header.DylibsImageGroupSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Image-Group Address",
                      "Other Image-Group Size",
                      Header.OtherImageGroupAddr,
                      Header.OtherImageGroupSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Program Closures Address",
                      "Program Closures Size",
                      Header.ProgClosuresAddr,
                      Header.ProgClosuresSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Program Closures Trie Address",
                      "Program Closures Trie Size",
                      Header.ProgClosuresTrieAddr,
                      Header.ProgClosuresTrieSize,
                      Options.Verbose);

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
                      Options.Verbose);

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

    fputc('\n', Options.OutFile);
}

static void
PrintDscHeaderV6Info(
    const ConstDscMemoryObject &Object,
    const struct PrintHeaderOperation::Options &Options) noexcept
{
    const auto Header = Object.getHeaderV6();
    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Dylibs Image-Array Address",
                      "Dylibs Image-Array Size",
                      Header.DylibsImageArrayAddr,
                      Header.DylibsImageArraySize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Dylibs Trie Address",
                      "Dylibs Trie Size",
                      Header.DylibsTrieAddr,
                      Header.DylibsTrieSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Image Array Address",
                      "Other Image Array Size",
                      Header.DylibsImageArrayAddr,
                      Header.DylibsImageArraySize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      Object.getRange(),
                      "Other Trie Address",
                      "Other Trie Size",
                      Header.DylibsTrieAddr,
                      Header.DylibsTrieSize,
                      Options.Verbose);
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
                                           Options.Verbose);

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

    const auto Version = PrintDscHeaderV0Info(Options, Object);
    if (Version < DscMemoryObject::Version::v1) {
        return 0;
    }

    PrintDscHeaderV1Info(Object, Options);
    if (Version < DscMemoryObject::Version::v2) {
        return 0;
    }

    PrintDscHeaderV2Info(Object, Options);
    if (Version < DscMemoryObject::Version::v3) {
        return 0;
    }

    PrintDscHeaderV3Info(Object, Options);
    if (Version < DscMemoryObject::Version::v4) {
        return 0;
    }

    PrintDscHeaderV4Info(Object, Options);
    if (Version < DscMemoryObject::Version::v5) {
        goto done;
    }

    PrintDscHeaderV5Info(Object, Options);
    if (Version < DscMemoryObject::Version::v6) {
        goto done;
    }

    PrintDscHeaderV6Info(Object, Options);

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
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (!Argument.IsOption()) {
            if (IndexOut != nullptr) {
                *IndexOut = Argv.indexOf(Argument);
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }
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
