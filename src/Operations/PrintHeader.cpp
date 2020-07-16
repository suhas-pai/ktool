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

        fputc('\n', Options.OutFile);
        fprintf(Options.OutFile,
                "Flags:\n"
                "\tNumber: %" PRIu32 " (0x%X)\n"
                "\tCount:  %" PRIu64 "\n",
                FlagsValue,
                FlagsValue,
                Flags.GetSetCount(IsBigEndian));
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
                                                          "\t",
                                                          "");
    } else {
        fputc('\n', Options.OutFile);
    }

    return 0;
}

static constexpr auto LongestName = LENGTH_OF("Program Closures Trie Address");
static void PrintDscKey(FILE *OutFile, const char *Key) noexcept {
    PrintUtilsRightPadSpaces(OutFile,
                             fprintf(OutFile, "%s: ", Key),
                             LongestName + LENGTH_OF(": "));
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
    PrintUtilsWriteOffset32(Options.OutFile,
                            Header.MappingOffset,
                            "",
                            "\n");

    PrintDscKey(Options.OutFile, "Mapping Count");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.MappingCount);

    PrintDscKey(Options.OutFile, "Images Count");
    PrintUtilsWriteOffset32(Options.OutFile,
                            Header.ImagesOffset,
                            "",
                            "\n");

    PrintDscKey(Options.OutFile, "Images Count");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.ImagesCount);

    PrintDscKey(Options.OutFile, "Dyld Base-Address");
    PrintUtilsWriteOffset64(Options.OutFile,
                            Header.DyldBaseAddress,
                            "",
                            "\n");

    return Version;
}

static void
PrintDscSizeRange(FILE *OutFile,
                  const char *OffsetName,
                  const char *SizeName,
                  uint64_t Offset,
                  uint64_t Size,
                  bool Verbose) noexcept
{
    PrintDscKey(OutFile, OffsetName);
    PrintUtilsWriteOffset64(OutFile, Offset);

    if (Size != 0) {
        if (Verbose) {
            PrintUtilsWriteOffset64SizeRange(OutFile,
                                             Offset,
                                             Size,
                                             " (",
                                             ")\n");
        } else {
            fputc('\n', OutFile);
        }

        PrintDscKey(OutFile, SizeName);

        const auto SizeWrittenOut = fprintf(OutFile, "%" PRIu64, Size);
        if (Verbose) {
            PrintUtilsRightPadSpaces(OutFile, SizeWrittenOut, OFFSET_64_LEN);
            PrintUtilsWriteFormattedSize(OutFile, Size, " (", ")");
        }

        fputc('\n', OutFile);
    } else {
        fputc('\n', OutFile);
        PrintDscKey(OutFile, SizeName);

        fprintf(OutFile, "%" PRIu64 "\n", Size);
    }
}

static void
PrintDscHeaderV1Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    const auto Header = Object.getHeaderV1();
    PrintDscSizeRange(Options.OutFile,
                      "Code-Signature Offset",
                      "Code-Signature Size",
                      Header.CodeSignatureOffset,
                      Header.CodeSignatureSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Slide-Info Offset",
                      "Slide-Info Size",
                      Header.SlideInfoOffset,
                      Header.SlideInfoSize,
                      Options.Verbose);
}

static void
PrintDscHeaderV2Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    const auto Header = Object.getHeaderV2();
    PrintDscSizeRange(Options.OutFile,
                      "Local-Symbols Offset",
                      "Local-Symbols Size",
                      Header.LocalSymbolsOffset,
                      Header.LocalSymbolsSize,
                      Options.Verbose);

    PrintDscKey(Options.OutFile, "Uuid");
    PrintUtilsWriteUuid(Options.OutFile, Header.Uuid, "", "\n");
}

static void
PrintDscHeaderV3Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
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
PrintDscHeaderV4Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    const auto Header = Object.getHeaderV4();

    PrintDscKey(Options.OutFile, "Branch-Pools Offset");
    PrintUtilsWriteOffset32(Options.OutFile,
                            Header.BranchPoolsOffset,
                            "",
                            "\n");

    PrintDscKey(Options.OutFile, "Branch-Pools Count");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.BranchPoolsCount);

    PrintDscSizeRange(Options.OutFile,
                      "Accelerate-Info Address",
                      "Accelerate-Info Size",
                      Header.LocalSymbolsOffset,
                      Header.LocalSymbolsSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Local-Symbols Offset",
                      "Local-Symbols Size",
                      Header.LocalSymbolsOffset,
                      Header.LocalSymbolsSize,
                      Options.Verbose);

    PrintDscKey(Options.OutFile, "Images-Text Offset");
    PrintUtilsWriteOffset64(Options.OutFile,
                            Header.ImagesTextOffset,
                            "",
                            "\n");

    PrintDscKey(Options.OutFile, "Images-Text Count");
    fprintf(Options.OutFile, "%" PRIu32 "\n", Header.BranchPoolsCount);
}

static
void PrintBoolValue(FILE *OutFile, const char *Key, bool Value) noexcept {
    PrintDscKey(OutFile, Key);

    fputs((Value) ? "true" : "false", OutFile);
    fputc('\n', OutFile);
}

static void
PrintDscHeaderV5Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    const auto Header = Object.getHeaderV5();
    PrintDscSizeRange(Options.OutFile,
                      "Dylibs Image-Group Address",
                      "Dylibs Image-Group Size",
                      Header.DylibsImageGroupAddr,
                      Header.DylibsImageGroupSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Other Image-Group Address",
                      "Other Image-Group Size",
                      Header.OtherImageGroupAddr,
                      Header.OtherImageGroupSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Program Closures Address",
                      "Program Closures Size",
                      Header.ProgClosuresAddr,
                      Header.ProgClosuresSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
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
PrintDscHeaderV6Info(const struct PrintHeaderOperation::Options &Options,
                     const ConstDscMemoryObject &Object) noexcept
{
    const auto Header = Object.getHeaderV6();
    PrintDscSizeRange(Options.OutFile,
                      "Dylibs Image-Array Address",
                      "Dylibs Image-Array Size",
                      Header.DylibsImageArrayAddr,
                      Header.DylibsImageArraySize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Dylibs Trie Address",
                      "Dylibs Trie Size",
                      Header.DylibsTrieAddr,
                      Header.DylibsTrieSize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Other Image Array Address",
                      "Other Image Array Size",
                      Header.DylibsImageArrayAddr,
                      Header.DylibsImageArraySize,
                      Options.Verbose);

    PrintDscSizeRange(Options.OutFile,
                      "Other Trie Address",
                      "Other Trie Size",
                      Header.DylibsTrieAddr,
                      Header.DylibsTrieSize,
                      Options.Verbose);
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

    PrintDscHeaderV1Info(Options, Object);
    if (Version < DscMemoryObject::Version::v2) {
        return 0;
    }

    PrintDscHeaderV2Info(Options, Object);
    if (Version < DscMemoryObject::Version::v3) {
        return 0;
    }

    PrintDscHeaderV3Info(Options, Object);
    if (Version < DscMemoryObject::Version::v4) {
        return 0;
    }

    PrintDscHeaderV4Info(Options, Object);
    if (Version < DscMemoryObject::Version::v5) {
        return 0;
    }

    PrintDscHeaderV5Info(Options, Object);
    if (Version < DscMemoryObject::Version::v6) {
        return 0;
    }

    PrintDscHeaderV6Info(Options, Object);
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
        case ObjectKind::DscImage: {
            const auto &Obj = cast<ObjectKind::DscImage>(Object);
            return Run(Obj, Options);
        }
    }

    assert(0 && "Unrecognized Object-Kind");
}
