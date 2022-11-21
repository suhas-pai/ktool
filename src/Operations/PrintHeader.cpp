//
//  Operations/PrintHeader.cpp
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#include "Operations/PrintHeader.h"

namespace Operations {
    PrintHeader::PrintHeader(FILE *OutFile, const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}
    
    auto
    PrintHeader::run(const Objects::MachO &MachO) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::MachO);

        const auto Header = MachO.header();
        const auto CpuKind = Header.cpuKind();
        const auto CpuSubKind = Header.cpuSubKind();
        const auto FileKind = Header.fileKind();
        const auto Ncmds = Header.ncmds();
        const auto SizeOfCmds = Header.sizeOfCmds();
        const auto Flags = Header.flags();

        if (Opt.Verbose) {
            const auto SubKindString =
                Mach::CpuKindAndSubKindIsValid(CpuKind, CpuSubKind) ?
                    Mach::CpuKindAndSubKindGetDesc(CpuKind, CpuSubKind).data() :
                    "Unrecognized";

            fprintf(OutFile,
                    "Apple %s Mach-O File\n"
                    "\tMagic:      %s\n"
                    "\tCputype:    %s\n"
                    "\tCpuSubtype: %s\n"
                    "\tFiletype:   %s\n"
                    "\tNcmds:      %d\n"
                    "\tSizeOfCmds: %d\n"
                    "\tFlags:      0x%x\n",
                    MachO.is64Bit() ? "64-Bit" : "32-Bit",
                    MachO::MagicGetDesc(Header.Magic).data(),
                    Mach::CpuKindIsValid(CpuKind) ?
                        Mach::CpuKindGetDesc(CpuKind).data() : "Unrecognized",
                    SubKindString,
                    MachO::FileKindIsValid(FileKind) ?
                        MachO::FileKindGetDesc(FileKind).data() :
                        "Unrecognized",
                    Ncmds,
                    SizeOfCmds,
                    Flags);
        } else {
            const auto SubKindString =
                Mach::CpuKindAndSubKindIsValid(CpuKind, CpuSubKind) ?
                    Mach::CpuKindAndSubKindGetString(CpuKind,
                                                     CpuSubKind).data() :
                    "Unrecognized";

            fprintf(OutFile,
                    "Apple %s Mach-O File\n"
                    "\tMagic:      %s\n"
                    "\tCputype:    %s\n"
                    "\tCpuSubtype: %s\n"
                    "\tFiletype:   %s\n"
                    "\tNcmds:      %d\n"
                    "\tSizeOfCmds: %d\n"
                    "\tFlags:      0x%x\n",
                    MachO.is64Bit() ? "64-Bit" : "32-Bit",
                    MachO::MagicGetString(Header.Magic).data(),
                    Mach::CpuKindIsValid(CpuKind) ?
                        Mach::CpuKindGetString(CpuKind).data() :
                        "Unrecognized",
                    SubKindString,
                    MachO::FileKindIsValid(FileKind) ?
                        MachO::FileKindGetString(FileKind).data() :
                        "Unrecognized",
                    Ncmds,
                    SizeOfCmds,
                    Flags);
        }

        auto FlagBits = static_cast<uint32_t>(Flags);
        auto Counter = 1;
        auto StartIndex = 0;

        while ((FlagBits >> StartIndex) != 0) {
            const auto BitIndex = __builtin_ctz(FlagBits >> StartIndex);
            const auto Mask = 1 << (StartIndex + BitIndex);
            const auto Flag = ::MachO::Flags(FlagBits & Mask);

            if (MachO::FlagsIsValid(Flag)) {
                fprintf(OutFile,
                        "\t\t %d. %s\n",
                        Counter,
                        MachO::FlagsGetString(Flag).data());
            } else {
                fprintf(OutFile,
                        "\t\t %d. Unknown Flag (Bit %d)\n",
                        Counter,
                        (StartIndex + BitIndex));
            }

            StartIndex += (BitIndex + 1);
            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto
    PrintHeader::run(const Objects::FatMachO &Fat) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::FatMachO);
        if (Opt.ArchIndex != -1) {
            const auto ArchIndex = static_cast<int32_t>(Opt.ArchIndex);
            return runOnArchOfFatMachO<RunError>(Result, Fat, ArchIndex);
        }

        const auto Header = Fat.header();
        fprintf(OutFile,
                "Apple %s Fat Mach-O File\n"
                "\tMagic:      %s\n"
                "\tArch Count: %d\n",
                Fat.is64Bit() ? "64-Bit" : "32-Bit",
                MachO::MagicGetString(Header.Magic).data(),
                Header.archCount());

        return Result.set(RunError::None);
    }

    RunResult PrintHeader::run(const Objects::Base &Base) const noexcept {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return run(static_cast<const Objects::FatMachO &>(Base));
        }

        return RunResultUnsupported;
    }
}
