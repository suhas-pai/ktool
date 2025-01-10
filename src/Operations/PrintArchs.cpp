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
        const auto FallBack = [CpuKind]() noexcept {
            return std::format("<Unknown: 0x{:02x}>",
                               static_cast<int32_t>(CpuKind));
        };

        return
            Verbose ?
                Mach::CpuKindGetString(CpuKind)
                    .and_then([](const auto &&V) noexcept {
                        return std::optional<std::string>(V);
                    })
                    .value_or(FallBack()) :
                Mach::CpuKindGetDesc(CpuKind)
                    .and_then([](const auto &&V) noexcept {
                        return std::optional<std::string>(V);
                    })
                    .value_or(FallBack());
    }

    static auto
    StringForSubKind(const Mach::CpuKind CpuKind,
                     const int32_t SubKind,
                     const bool Verbose) noexcept -> std::string
    {
        const auto FallBack = [SubKind]() noexcept {
            return std::format("<Unknown: 0x{:02x}>", SubKind);
        };

        return
            Verbose ?
                Mach::CpuKindAndSubKindGetString(CpuKind, SubKind)
                    .and_then([](const auto &&V) noexcept {
                        return std::optional<std::string>(V);
                    })
                    .value_or(FallBack()) :
                Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind)
                    .and_then([](const auto &&V) noexcept {
                        return std::optional<std::string>(V);
                    })
                    .value_or(FallBack());
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

        auto String = std::format("{}", Utils::FormattedNumber<uint64_t>(0));
        std::print(OutFile, "{}", Utils::ByteSize(1ull << Align));

        const auto OffsetRange = Utils::PrintRange(Offset, Size);
        std::print(OutFile,
                   "{}Arch #{}: {}\n"
                   "{}\tCpuKind:    {}\n"
                   "{}\tCpuSubKind: {}\n"
                   "{}\tOffset:     {} ({})\n"
                   "{}\tSize:       {}\n"
                   "{}\tAlignment:  {} ({})\n",
                   Prefix, Ordinal, ObjectDesc,
                   Prefix, CpuKindString,
                   Prefix, SubKindString,
                   Prefix, Utils::Address(Offset), OffsetRange,
                   Prefix, Utils::ByteSize(Size),
                   Prefix, Align, Utils::ByteSize(1ull << Align));
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

        std::print(OutFile,
                   "{}Arch #{}: {}\n"
                   "{}\tCpuKind:    {}\n"
                   "{}\tCpuSubKind: {}\n"
                   "{}\tOffset:     {} ({})\n"
                   "{}\tSize:       {}\n"
                   "{}\tAlignment:  {} ({})\n",
                   Prefix, Ordinal, ObjectDesc,
                   Prefix, CpuKindString,
                   Prefix, SubKindString,
                   Prefix, Utils::Address(Offset),
                    Utils::PrintRange(Offset, Size),
                   Prefix, Utils::ByteSize(Size),
                   Prefix, Align, Utils::ByteSize(1ull << Align));
    }

    auto PrintArchs::run(const Objects::FatMachO &Fat) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = Fat.isBigEndian();

        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

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
