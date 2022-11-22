//
//  PrintArchs.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include "ADT/PrintUtils.h"
#include "Operations/PrintArchs.h"

namespace Operations {
    PrintArchs::PrintArchs(FILE *OutFile, const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintArchs::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintArchs::supportsObjectKind");
            case Objects::Kind::MachO:
                return false;
            case Objects::Kind::FatMachO:
                return true;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintArchs::supportsObjectKind");
    }

    auto
    PrintArchs::run(const Objects::FatMachO &Fat) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::FatMachO);

        const auto ArchCount = Fat.archCount();
        const auto IsBigEndian = Fat.isBigEndian();

        if (Fat.is64Bit()) {
            const auto ArchList = Fat.archs64();
            for (auto I = uint32_t(); I != ArchCount; I++) {
                const auto &Arch = ArchList[I];

                const auto CpuKind = Arch.cpuKind(IsBigEndian);
                const auto SubKind = Arch.cpuSubKind(IsBigEndian);
                const auto Offset = Arch.offset(IsBigEndian);
                const auto Size = Arch.size(IsBigEndian);
                const auto Align = Arch.align(IsBigEndian);

                fprintf(OutFile, "Arch #%" PRIu32 ":\n", I + 1);
                if (Mach::CpuKindIsValid(CpuKind)) {
                    fprintf(OutFile,
                            "\tCputype:    %s\n",
                            Opt.Verbose ?
                                Mach::CpuKindGetDesc(CpuKind).data() :
                                Mach::CpuKindGetString(CpuKind).data());
                } else {
                    fprintf(OutFile,
                            "\tCputype:    <unknown> (Value: %" PRId32 ")\n",
                            CpuKind);
                }

                if (Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind)) {
                    const auto CpuSubKindString =
                        Opt.Verbose ?
                            Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind) :
                            Mach::CpuKindAndSubKindGetString(CpuKind, SubKind);

                    fprintf(OutFile,
                            "\tCpusubtype: %s\n",
                            CpuSubKindString.data());
                } else {
                    fprintf(OutFile,
                            "\tCpusubtype: <unknown> (Value: %" PRId32 ")\n",
                            SubKind);
                }

                fprintf(OutFile,
                        "\tOffset:     " ADDRESS_64_FMT " ("
                            ADDR_RANGE_64_FMT ")\n"
                        "\tSize:       %" PRIu64 "\n"
                        "\tAlign:      %" PRIu32 "\n",
                        Offset,
                        ADDR_RANGE_FMT_ARGS(Offset, Size),
                        Size,
                        Align);
            }
        } else {
            const auto ArchList = Fat.archs();
            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];

                const auto CpuKind = Arch.cpuKind(IsBigEndian);
                const auto SubKind = Arch.cpuSubKind(IsBigEndian);
                const auto Offset = Arch.offset(IsBigEndian);
                const auto Size = Arch.size(IsBigEndian);
                const auto Align = Arch.align(IsBigEndian);

                fprintf(OutFile, "Arch #%" PRIu32 ":\n", I + 1);
                if (Mach::CpuKindIsValid(CpuKind)) {
                    fprintf(OutFile,
                            "\tCputype:    %s\n",
                            Opt.Verbose ?
                                Mach::CpuKindGetDesc(CpuKind).data() :
                                Mach::CpuKindGetString(CpuKind).data());
                } else {
                    fprintf(OutFile,
                            "\tCputype:    <unknown> (Value: %" PRId32 ")\n",
                            CpuKind);
                }

                if (Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind)) {
                    const auto CpuSubKindString =
                        Opt.Verbose ?
                            Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind) :
                            Mach::CpuKindAndSubKindGetString(CpuKind, SubKind);

                    fprintf(OutFile,
                            "\tCpusubtype: %s\n",
                            CpuSubKindString.data());
                } else {
                    fprintf(OutFile,
                            "\tCpusubtype: <unknown> (Value: %" PRId32 ")\n",
                            SubKind);
                }

                fprintf(OutFile,
                        "\tOffset:     " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "\tSize:       %" PRIu32 "\n"
                        "\tAlign:      %" PRIu32 "\n",
                        Offset,
                        ADDR_RANGE_FMT_ARGS(Offset, Size),
                        Size,
                        Align);
            }
        }

        return Result.set(RunError::None);
    }

    auto PrintArchs::run(const Objects::Base &Base) const noexcept ->
        RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return RunResultUnsupported;
            case Objects::Kind::FatMachO:
                return run(static_cast<const Objects::FatMachO &>(Base));
        }

        assert(false && "Got unrecognized Object-Kind in PrintArchs::run");
    }
}

