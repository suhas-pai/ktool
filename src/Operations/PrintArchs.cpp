//
//  Operations/PrintArchs.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include "Operations/PrintArchs.h"
#include "Utils/Print.h"

namespace Operations {
    PrintArchs::PrintArchs(FILE *const OutFile, const struct Options &Options)
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

    void
    PrintArchs::PrintArch(FILE *const OutFile,
                          const MachO::FatArch &Arch,
                          const Objects::Base *const ArchObject,
                          const uint32_t Ordinal,
                          const bool Verbose,
                          const bool IsBigEndian,
                          const char *const Prefix) noexcept
    {
        auto ObjectDesc = std::string_view("<Unrecognized>");
        if (ArchObject != nullptr) {
            ObjectDesc = Objects::KindGetString(ArchObject->kind());
        }

        const auto CpuKind = Arch.cpuKind(IsBigEndian);
        const auto SubKind = Arch.cpuSubKind(IsBigEndian);

        const auto SubKindString =
            Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind) ?
                Verbose ?
                    Mach::CpuKindAndSubKindGetString(
                        CpuKind,
                        SubKind).data() :
                    Mach::CpuKindAndSubKindGetDesc(
                        CpuKind,
                        SubKind).data()
                : "Unrecognized";

        const auto Offset = Arch.offset(IsBigEndian);
        const auto Size = Arch.size(IsBigEndian);
        const auto Align = Arch.align(IsBigEndian);

        fprintf(OutFile,
                "%sArch #%" PRIu32 ": %s\n"
                "%s\tCpuKind:    %s\n"
                "%s\tCpuSubKind: %s\n"
                "%s\tOffset:     " ADDRESS_32_FMT " ("
                    ADDR_RANGE_32_FMT ")\n"
                "%s\tSize:       %s\n"
                "%s\tAlign:      %" PRIu32 " (%s)\n",
                Prefix, Ordinal, ObjectDesc.data(),
                Prefix, Mach::CpuKindIsValid(CpuKind) ?
                    Verbose ?
                        Mach::CpuKindGetString(CpuKind).data() :
                        Mach::CpuKindGetDesc(CpuKind).data()
                    : "Unrecognized",
                Prefix, SubKindString,
                Prefix, Offset, ADDR_RANGE_FMT_ARGS(Offset, Offset + Size),
                Prefix, Utils::FormattedSizeForOutput(Size).data(),
                Prefix, Align,
                    Utils::FormattedSizePowerOf2(1ull << Align).data());
    }

    void
    PrintArchs::PrintArch64(FILE *const OutFile,
                            const MachO::FatArch64 &Arch,
                            const Objects::Base *const ArchObject,
                            const uint32_t Ordinal,
                            const bool Verbose,
                            const bool IsBigEndian,
                            const char *const Prefix) noexcept
    {
        auto ObjectDesc = std::string_view("<Unrecognized>");
        if (ArchObject != nullptr) {
            ObjectDesc = Objects::KindGetString(ArchObject->kind());
        }

        const auto CpuKind = Arch.cpuKind(IsBigEndian);
        const auto SubKind = Arch.cpuSubKind(IsBigEndian);

        const auto SubKindString =
            Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind) ?
                Verbose ?
                    Mach::CpuKindAndSubKindGetString(
                        CpuKind,
                        SubKind).data() :
                    Mach::CpuKindAndSubKindGetDesc(
                        CpuKind,
                        SubKind).data()
                : "Unrecognized";

        const auto Offset = Arch.offset(IsBigEndian);
        const auto Size = Arch.size(IsBigEndian);
        const auto Align = Arch.align(IsBigEndian);

        fprintf(OutFile,
                "%sArch #%" PRIu32 ": %s\n"
                "%s\tCpuKind:    %s\n"
                "%s\tCpuSubKind: %s\n"
                "%s\tOffset:     " ADDRESS_64_FMT " ("
                    ADDR_RANGE_64_FMT ")\n"
                "%s\tSize:       %s\n"
                "%s\tAlign:      %" PRIu32 " (%s)\n",
                Prefix, Ordinal, ObjectDesc.data(),
                Prefix, Mach::CpuKindIsValid(CpuKind) ?
                    Verbose ?
                        Mach::CpuKindGetString(CpuKind).data() :
                        Mach::CpuKindGetDesc(CpuKind).data()
                    : "Unrecognized",
                Prefix, SubKindString,
                Prefix, Offset, ADDR_RANGE_FMT_ARGS(Offset, Offset + Size),
                Prefix, Utils::FormattedSizeForOutput(Size).data(),
                Prefix, Align,
                    Utils::FormattedSizePowerOf2(1ull << Align).data());
    }

    auto
    PrintArchs::run(const Objects::FatMachO &Fat) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::FatMachO);
        const auto IsBigEndian = Fat.isBigEndian();

        auto I = uint32_t();
        if (Fat.is64Bit()) {
            for (const auto &Arch : Fat.arch64List()) {
                PrintArch64(OutFile,
                            Arch,
                            Fat.getArchObjectAtIndex(I).ptr(),
                            I + 1,
                            Opt.Verbose,
                            IsBigEndian);
                I++;
            }
        } else {
            for (const auto &Arch : Fat.archList()) {
                PrintArch(OutFile,
                          Arch,
                          Fat.getArchObjectAtIndex(I).ptr(),
                          I + 1,
                          Opt.Verbose,
                          IsBigEndian);
                I++;
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

