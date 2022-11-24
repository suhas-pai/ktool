//
//  Operations/PrintHeader.cpp
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#include "ADT/FlagsIterator.h"

#include "Mach/Machine.h"
#include "MachO/Header.h"

#include "Operations/PrintHeader.h"
#include "Utils/Print.h"

namespace Operations {
    PrintHeader::PrintHeader(FILE *const OutFile, const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintHeader::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintHeader::supportsObjectKind");
            case Objects::Kind::MachO:
            case Objects::Kind::FatMachO:
                return true;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintHeader::supportsObjectKind");
    }

    auto
    PrintHeader::run(const Objects::MachO &MachO) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::MachO);

        const auto Header = MachO.header();
        const auto CpuKind = Header.cpuKind();
        const auto SubKind = Header.cpuSubKind();
        const auto FileKind = Header.fileKind();
        const auto Ncmds = Header.ncmds();
        const auto SizeOfCmds = Header.sizeOfCmds();
        const auto Flags = Header.flags();

        const auto SubKindString =
            Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind) ?
                Opt.Verbose ?
                    Mach::CpuKindAndSubKindGetString(CpuKind,
                                                        SubKind).data() :
                    Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind).data()
                : "Unrecognized";

        fprintf(OutFile,
                "Apple %s Mach-O File\n"
                "\tMagic:      %s\n"
                "\tCputype:    %s\n"
                "\tCpuSubtype: %s\n"
                "\tFiletype:   %s\n"
                "\tNcmds:      %d\n"
                "\tSizeOfCmds: %d\n"
                "\tFlags:      0x%" PRIx32 "\n",
                MachO.is64Bit() ? "64-Bit" : "32-Bit",
                Opt.Verbose ?
                    MachO::MagicGetString(Header.Magic).data() :
                    MachO::MagicGetDesc(Header.Magic).data(),
                Mach::CpuKindIsValid(CpuKind) ?
                    Opt.Verbose ?
                        Mach::CpuKindGetString(CpuKind).data() :
                        Mach::CpuKindGetDesc(CpuKind).data()
                    : "Unrecognized",
                SubKindString,
                MachO::FileKindIsValid(FileKind) ?
                    Opt.Verbose ?
                        MachO::FileKindGetString(FileKind).data() :
                        MachO::FileKindGetDesc(FileKind).data()
                    : "Unrecognized",
                Ncmds,
                SizeOfCmds,
                Flags.value());

        auto Counter = static_cast<uint8_t>(1);
        for (const auto &Bit : ADT::FlagsIterator(Flags.value())) {
            const auto Flag = ::MachO::Flags::Kind(1 << Bit);
            fprintf(OutFile,
                    "\t\t %" PRIu8 ". Bit %" PRIu32 ": %s\n",
                    Counter,
                    Bit,
                    MachO::Flags::KindIsValid(Flag) ?
                        MachO::Flags::KindGetString(Flag).data() :
                        "<unknown>");

            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto
    PrintHeader::run(const Objects::FatMachO &Fat) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::FatMachO);
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

    auto PrintHeader::run(const Objects::Base &Base) const noexcept ->
        RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return run(static_cast<const Objects::FatMachO &>(Base));
        }

        assert(false && "Got unrecognized Object-Kind in PrintHeader::run");
    }
}
