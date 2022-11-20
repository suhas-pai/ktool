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
    
    void PrintHeader::run(const Objects::MachO &MachO) const noexcept {
        const auto Header = MachO.header();

        const auto CpuKind = Header.cpuKind();
        const auto CpuSubKind = Header.cpuSubKind();
        const auto FileKind = Header.fileKind();
        const auto Ncmds = Header.ncmds();
        const auto SizeOfCmds = Header.sizeOfCmds();
        const auto Flags = Header.flags();

        const auto SubKindString =
            Mach::CpuKindAndSubKindIsValid(CpuKind, CpuSubKind) ?
                Mach::CpuKindAndSubKindGetString(CpuKind, CpuSubKind).data() :
                "Unrecognized";

        fprintf(OutFile,
                "Apple %s Mach-O File\n"
                "\tMagic:      %s\n"
                "\tCputype:    %s\n"
                "\tCpuSubtype: %s\n"
                "\tFiletype:   %s\n"
                "\tNcmds:      %d\n"
                "\tSizeOfCmds: %d\n"
                "\tFlags:      %d\n",
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

    void PrintHeader::run(const Objects::FatMachO &MachO) const noexcept {
        const auto Header = MachO.header();
        fprintf(OutFile,
                "Apple %s Fat Mach-O File\n"
                "\tMagic:      %s\n"
                "\tArch Count: %d\n",
                MachO.is64Bit() ? "64-Bit" : "32-Bit",
                MachO::MagicGetString(Header.Magic).data(),
                Header.archCount());
    }

    bool PrintHeader::run(const Objects::Base &Base) const noexcept {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                run(static_cast<const Objects::MachO &>(Base));
                return true;
            case Objects::Kind::FatMachO:
                run(static_cast<const Objects::FatMachO &>(Base));
                return true;
        }

        return false;
    }
}
