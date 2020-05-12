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

PrintHeaderOperation::PrintHeaderOperation() noexcept : Operation(OpKind) {}
PrintHeaderOperation::PrintHeaderOperation(const struct Options &Options)
noexcept : Operation(OpKind), Options(Options) {}

void PrintHeaderOperation::run(const ConstMemoryObject &Object) noexcept {
    run(Object, Options);
}

void
PrintHeaderOperation::run(const ConstMachOMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    fputs("Mach-O Single Architecture File\n", stdout);

    const auto &Header = Object.GetConstHeader();
    const auto IsBigEndian = Object.IsBigEndian();

    const auto Magic = Object.GetMagic();
    const auto CpuType = Object.GetCpuType();
    const auto CpuSubType = SwitchEndianIf(Header.CpuSubType, IsBigEndian);
    const auto FileType = Object.GetFileType();
    const auto Ncmds = SwitchEndianIf(Header.Ncmds, IsBigEndian);
    const auto SizeOfCmds = SwitchEndianIf(Header.SizeOfCmds, IsBigEndian);
    const auto Flags = Header.GetFlags();

    const auto MagicDesc = MachO::Header::MagicGetDescription(Magic).data();
    const auto FileTypeName = MachO::Header::FileTypeGetName(FileType).data();
    const auto FileTypeDesc =
        MachO::Header::FileTypeGetDescription(FileType).data();

    const auto CpuTypeValue = static_cast<int32_t>(CpuType);
    const auto FileTypeValue = static_cast<uint32_t>(FileType);
    const auto FlagsValue = Flags.GetIntegerValue();

    if (Options.Verbose) {
        const auto MagicName = MachO::Header::MagicGetName(Magic).data();
        const auto MagicValue = static_cast<uint32_t>(Magic);

        fprintf(stdout,
                "Magic:\n"
                "\tName:        %s\n"
                "\tDescription: %s\n"
                "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                MagicName,
                MagicDesc,
                MagicValue,
                MagicValue);

        const auto &CpuTypeName = Mach::CpuTypeGetName(CpuType).data();
        if (CpuTypeName != nullptr) {
            const auto CpuTypeDesc =
                Mach::CpuTypeGetDescripton(CpuType).data();
            const auto CpuSubTypeName =
                Mach::CpuSubType::GetName(CpuType, CpuSubType).data();
            const auto CpuSubTypeDesc =
                Mach::CpuSubType::GetDescription(CpuType, CpuSubType).data();

            fprintf(stdout,
                    "CpuType:\n"
                    "\tName:        %s\n"
                    "\tDescription: %s\n"
                    "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                    CpuTypeName,
                    CpuTypeDesc,
                    CpuTypeValue,
                    CpuTypeValue);

            if (CpuSubTypeName != nullptr) {
                fprintf(stdout,
                        "CpuSubType:\n"
                        "\tName:        %s\n"
                        "\tDescription: %s\n"
                        "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                        CpuSubTypeName,
                        CpuSubTypeDesc,
                        CpuSubType,
                        CpuSubType);
            } else {
                fprintf(stdout,
                        "CpuSubType:\n"
                        "\tUnrecognized\n"
                        "\tValue; %" PRIu32 " (0x%" PRIX32 ")\n",
                        CpuSubType, CpuSubType);
            }
        } else {
            fprintf(stdout,
                    "CpuType:\n"
                    "\tUnrecognized\n\tValue: %" PRIu32 " (Value: 0x%X)\n",
                    CpuTypeValue,
                    CpuTypeValue);

            fprintf(stdout,
                    "CpuSubType:\n"
                    "\tUnknown\n"
                    "\tValue; %" PRIu32 " (Value: 0x%" PRIX32 ")\n",
                    CpuSubType,
                    CpuSubType);
        }

        if (FileTypeName != nullptr) {
            fprintf(stdout,
                    "FileType:\n"
                    "\tName:        %s\n"
                    "\tDescription: %s\n"
                    "\tValue:       %" PRIu32 " (0x%" PRIX32 ")\n",
                    FileTypeName,
                    FileTypeDesc,
                    FileTypeValue,
                    FileTypeValue);
        } else {
            fprintf(stdout,
                    "FileType:\n"
                    "\tUnrecognized\n\tValue: %" PRIu32 " (0x%" PRIX32 ")\n",
                    FileTypeValue,
                    FileTypeValue);
        }

        fprintf(stdout, "Ncmds: %" PRIu32 "\n", Ncmds);
        fprintf(stdout, "SizeOfCmds: %" PRIu32 "\n", SizeOfCmds);

        fprintf(stdout,
                "Flags:\n"
                "\tNumber: %u (0x%X)\n"
                "\tCount:  %" PRIu64 "\n",
                FlagsValue,
                FlagsValue,
                Flags.GetSetCount(IsBigEndian));
    } else {
        fprintf(stdout, "Magic:      %s\n", MagicDesc);

        const auto BrandName = Mach::CpuTypeGetBrandName(CpuType).data();
        if (BrandName == nullptr) {
            fprintf(stdout,
                    "CpuType:    Unrecognized (Value: %" PRIu32 ")\n",
                    CpuTypeValue);
            return;
        }

        const auto SubTypeFullName =
            Mach::CpuSubType::GetFullName(CpuType, CpuSubType).data();

        if (SubTypeFullName != nullptr) {
            fprintf(stdout, "CpuType:    %s\n", SubTypeFullName);
        } else {
            fprintf(stdout,
                    "CpuType:    %s (Unrecognized CpuSubType: %" PRIu32 ")\n",
                    BrandName,
                    CpuSubType);
        }

        if (FileTypeDesc != nullptr) {
            fprintf(stdout, "FileType:   %s\n", FileTypeDesc);
        } else {
            fprintf(stdout,
                    "FileType:   Unrecognized (%" PRIu32 ")\n",
                    FileTypeValue);
        }

        fprintf(stdout,
                "Ncmds:      %u\n"
                "SizeOfCmds: %u\n",
                Ncmds,
                SizeOfCmds);

        fprintf(stdout, "Flags:      0x%X\n", FlagsValue);
    }
}

void
PrintHeaderOperation::run(const ConstFatMachOMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    fputs("Mach-O Multi-Architecture (FAT) File\n", stdout);

    const auto &Header = Object.GetConstHeader();
    const auto &Magic = Header.Magic;
    const auto &MagicDesc = MachO::FatHeader::MagicGetDescription(Magic).data();

    if (Options.Verbose) {
        const auto MagicValue = static_cast<uint32_t>(Magic);
        const auto &MagicName = MachO::FatHeader::MagicGetName(Magic).data();

        fprintf(stdout,
                "Magic:\n"
                "\tName:%s\n"
                "\tDescription: %s\n"
                "\tValue: %" PRIu32 "(0x%" PRIX32 ")\n",
                MagicName, MagicDesc, MagicValue, MagicValue);
    } else {
        fprintf(stdout, "Magic: %s\n", MagicDesc);
    }

    const auto ArchsCount = Object.GetArchCount();
    fprintf(stdout, "Archs Count: %" PRIu32 "\n", ArchsCount);

    if (ArchsCount <= 3) {
        MachOTypePrinter<MachO::FatHeader>::PrintArchList(stdout, Header, "\t");
    }
}

struct PrintHeaderOperation::Options
PrintHeaderOperation::ParseOptionsImpl(int Argc,
                                       const char *Argv[],
                                       int *IndexOut) noexcept
{
    struct Options Options;
    for (auto I = int(); I != Argc; I++) {
        const auto Argument = Argv[I];
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (Argument[0] != '-') {
            if (IndexOut != nullptr) {
                *IndexOut = I;
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument);
            exit(1);
        }
    }

    return Options;
}

struct PrintHeaderOperation::Options *
PrintHeaderOperation::ParseOptions(int Argc, const char *Argv[], int *IndexOut)
noexcept {
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

void
PrintHeaderOperation::run(const ConstMemoryObject &Object,
                          const struct Options &Options) noexcept
{
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object Type is None");
        case ObjectKind::MachO:
            run(cast<ObjectKind::MachO>(Object), Options);
            break;
        case ObjectKind::FatMachO:
            run(cast<ObjectKind::FatMachO>(Object), Options);
            break;
    }
}

void
PrintHeaderOperation::run(const ConstMemoryObject &Object,
                          int Argc,
                          const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
