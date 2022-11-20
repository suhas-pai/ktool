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
        const auto SubKindString =
            Mach::CpuKindAndSubKindIsValid(Header.CpuKind, Header.CpuSubKind) ?
                Mach::CpuKindAndSubKindGetString(
                    Header.CpuKind, Header.CpuSubKind).data() :
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
                MachO::MagicGetString(MachO.header().Magic).data(),
                Mach::CpuKindIsValid(MachO.header().CpuKind) ?
                    Mach::CpuKindGetString(MachO.header().CpuKind).data() :
                    "Unrecognized",
                SubKindString,
                MachO::FileKindIsValid(MachO.header().FileKind) ?
                    MachO::FileKindGetString(MachO.header().FileKind).data() :
                    "Unrecognized",
                MachO.header().Ncmds,
                MachO.header().SizeOfCmds,
                MachO.header().Flags);
    }

    bool PrintHeader::run(const Objects::Base &Base) const noexcept {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                run(static_cast<const Objects::MachO &>(Base));
                return true;
        }

        return false;
    }
}
