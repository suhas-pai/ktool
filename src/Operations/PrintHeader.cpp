//
//  src/Operations/PrintHeader.cpp
//  stool
//
//  Created by Suhas Pai on 4/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "Utils/MachOPrinter.h"
#include "Utils/PrintUtils.h"

#include "Operation.h"
#include "PrintHeader.h"

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
        fprintf(Options.OutFile, "SizeOfCmds: %" PRIu32 "\n", SizeOfCmds);
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
                "\tName:%s\n"
                "\tDescription: %s\n"
                "\tValue: %" PRIu32 "(0x%" PRIX32 ")\n",
                MagicName,
                MagicDesc,
                MagicValue,
                MagicValue);
    } else {
        fprintf(Options.OutFile, "Magic: %s\n", MagicDesc);
    }

    const auto ArchsCount = Object.getArchCount();
    fprintf(Options.OutFile, "Archs Count: %" PRIu32 "\n", ArchsCount);

    if (ArchsCount <= 3) {
        MachOTypePrinter<MachO::FatHeader>::PrintArchList(Options.OutFile,
                                                          Header,
                                                          "\t",
                                                          "");
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
    }

    assert(0 && "Unrecognized Object-Kind");
}
