//
//  Operations/PrintArchs.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <format>
#include <memory>
#include <sys/stat.h>

#include "Objects/Open.h"
#include "Operations/PrintArchs.h"
#include "Utils/Print.h"

namespace Operations {
    PrintArchs::PrintArchs(FILE *const OutFile,
                           const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintArchs), OutFile(OutFile), Opt(Options) {}

    bool
    PrintArchs::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintArchs::supportsObjectKind()");
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return false;
            case Objects::Kind::FatMachO:
                return true;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintArchs::supportsObjectKind()");
    }

    static auto
    StringForCpuKind(const Mach::CpuKind CpuKind,
                     const bool Verbose) noexcept -> std::string
    {
        return Mach::CpuKindIsValid(CpuKind) ?
            Verbose ?
                std::string(Mach::CpuKindGetString(CpuKind)) :
                std::string(Mach::CpuKindGetDesc(CpuKind))
            : std::format("<Unknown: 0x{:02x}>",
                          static_cast<int32_t>(CpuKind));
    }

    static auto
    StringForSubKind(const Mach::CpuKind CpuKind,
                     const int32_t SubKind,
                     const bool Verbose) noexcept -> std::string
    {
        return Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind) ?
            Verbose ?
                std::string(
                        Mach::CpuKindAndSubKindGetString(CpuKind, SubKind)) :
                    std::string(
                        Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind))
            : std::format("<Unknown: 0x{:02x}>", SubKind);
    }

    void
    PrintArchs::PrintArch(FILE *const OutFile,
                          const MachO::FatArch &Arch,
                          const Objects::Base *const ArchObject,
                          const uint32_t Ordinal,
                          const bool Verbose,
                          const bool IsBigEndian,
                          const std::string_view Prefix) noexcept
    {
        auto ObjectDesc = std::string_view("<Unrecognized>");
        if (ArchObject != nullptr) {
            ObjectDesc = Objects::KindGetString(ArchObject->kind());
        }

        const auto CpuKind = Arch.cpuKind(IsBigEndian);
        const auto SubKind = Arch.cpuSubKind(IsBigEndian);

        const auto CpuKindString = StringForCpuKind(CpuKind, Verbose);
        const auto SubKindString = StringForSubKind(CpuKind, SubKind, Verbose);

        const auto Offset = Arch.offset(IsBigEndian);
        const auto Size = Arch.size(IsBigEndian);
        const auto Align = Arch.align(IsBigEndian);

        fprintf(OutFile,
                STRING_VIEW_FMT "Arch #%" PRIu32 ": %s\n"
                STRING_VIEW_FMT "\tCpuKind:    %s\n"
                STRING_VIEW_FMT "\tCpuSubKind: %s\n"
                STRING_VIEW_FMT "\tOffset:     " ADDRESS_32_FMT
                    " (" ADDR_RANGE_32_FMT ")\n"
                STRING_VIEW_FMT "\tSize:       %s\n"
                STRING_VIEW_FMT "\tAlign:      %" PRIu32 " (%s)\n",
                STRING_VIEW_FMT_ARGS(Prefix), Ordinal, ObjectDesc.data(),
                STRING_VIEW_FMT_ARGS(Prefix), CpuKindString.c_str(),
                STRING_VIEW_FMT_ARGS(Prefix), SubKindString.c_str(),
                STRING_VIEW_FMT_ARGS(Prefix), Offset,
                    ADDR_RANGE_FMT_ARGS(Offset, Offset + Size),
                STRING_VIEW_FMT_ARGS(Prefix),
                    Utils::FormattedSizeForOutput(Size).c_str(),
                STRING_VIEW_FMT_ARGS(Prefix), Align,
                    Utils::FormattedSize(1ull << Align).c_str());
    }

    void
    PrintArchs::PrintArch64(FILE *const OutFile,
                            const MachO::FatArch64 &Arch,
                            const Objects::Base *const ArchObject,
                            const uint32_t Ordinal,
                            const bool Verbose,
                            const bool IsBigEndian,
                            const std::string_view Prefix) noexcept
    {
        auto ObjectDesc = std::string_view("<Unrecognized>");
        if (ArchObject != nullptr) {
            ObjectDesc = Objects::KindGetString(ArchObject->kind());
        }

        const auto CpuKind = Arch.cpuKind(IsBigEndian);
        const auto SubKind = Arch.cpuSubKind(IsBigEndian);

        const auto CpuKindString = StringForCpuKind(CpuKind, Verbose);
        const auto SubKindString = StringForSubKind(CpuKind, SubKind, Verbose);

        const auto Offset = Arch.offset(IsBigEndian);
        const auto Size = Arch.size(IsBigEndian);
        const auto Align = Arch.align(IsBigEndian);

        fprintf(OutFile,
                STRING_VIEW_FMT "Arch #%" PRIu32 ": %s\n"
                STRING_VIEW_FMT "\tCpuKind:    %s\n"
                STRING_VIEW_FMT "\tCpuSubKind: %s\n"
                STRING_VIEW_FMT "\tOffset:     " ADDRESS_64_FMT
                    " (" ADDR_RANGE_64_FMT ")\n"
                STRING_VIEW_FMT "\tSize:       %s\n"
                STRING_VIEW_FMT "\tAlign:      %" PRIu32 " (%s)\n",
                STRING_VIEW_FMT_ARGS(Prefix), Ordinal, ObjectDesc.data(),
                STRING_VIEW_FMT_ARGS(Prefix), CpuKindString.c_str(),
                STRING_VIEW_FMT_ARGS(Prefix), SubKindString.c_str(),
                STRING_VIEW_FMT_ARGS(Prefix), Offset,
                    ADDR_RANGE_FMT_ARGS(Offset, Offset + Size),
                STRING_VIEW_FMT_ARGS(Prefix),
                    Utils::FormattedSizeForOutput(Size).c_str(),
                STRING_VIEW_FMT_ARGS(Prefix), Align,
                    Utils::FormattedSize(1ull << Align).c_str());
    }

    auto PrintArchs::run(const Objects::FatMachO &Fat) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = Fat.isBigEndian();
        auto I = uint32_t();

        if (Fat.is64Bit()) {
            for (const auto &Arch : Fat.arch64List()) {
                const auto Object =
                    std::unique_ptr<Objects::Base>(
                       Objects::OpenArch(Fat, I).value());

                PrintArch64(OutFile,
                            Arch,
                            Object.get(),
                            I + 1,
                            Opt.Verbose,
                            IsBigEndian);
                I++;
            }
        } else {
            for (const auto &Arch : Fat.archList()) {
                const auto Object =
                    std::unique_ptr<Objects::Base>(
                       Objects::OpenArch(Fat, I).value());

                PrintArch(OutFile,
                          Arch,
                          Object.get(),
                          I + 1,
                          Opt.Verbose,
                          IsBigEndian);

                I++;
            }
        }

        return RunResult();
    }

    auto PrintArchs::run(const Objects::Base &Base) const noexcept -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "PrintArchs::run() got Object with Kind::None");
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return RunResult(RunResult::Error::Unsupported);
            case Objects::Kind::FatMachO:
                return run(static_cast<const Objects::FatMachO &>(Base));
        }

        assert(false && "Got unrecognized Object-Kind in PrintArchs::run()");
    }
}

