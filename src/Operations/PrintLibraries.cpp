//
//  Operations/PrintLibraries.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <algorithm>
#include <compare>

#include "MachO/LoadCommands.h"
#include "Operations/PrintLibraries.h"

#include "Utils/Print.h"

namespace Operations {
    PrintLibraries::PrintLibraries(FILE *const OutFile,
                                   const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintLibraries), OutFile(OutFile), Opt(Options) {}

    bool
    PrintLibraries::supportsObjectKind(const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintLibraries::supportsObjectKind()");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return true;
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintLibraries::supportsObjectKind()");
    }

    struct DylibInfo {
        std::string_view Name;
        MachO::LoadCommandKind Kind;

        Dyld3::PackedVersion CurrentVersion;
        Dyld3::PackedVersion CompatVersion;

        uint32_t Timestamp;
        uint32_t Index;
    };

    static auto
    CompareEntriesBySortKind(
        const DylibInfo &Lhs,
        const DylibInfo &Rhs,
        const PrintLibraries::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            case PrintLibraries::Options::SortKind::ByCurrentVersion:
                return Lhs.CurrentVersion <=> Rhs.CurrentVersion;
            case PrintLibraries::Options::SortKind::ByCompatVersion:
                return Lhs.CompatVersion <=> Rhs.CompatVersion;
            case PrintLibraries::Options::SortKind::ByIndex:
                return Lhs.Index <=> Rhs.Index;
            case PrintLibraries::Options::SortKind::ByTimeStamp:
                return Lhs.Timestamp <=> Rhs.Timestamp;
            case PrintLibraries::Options::SortKind::ByName:
                return Lhs.Name <=> Rhs.Name;
        }

        assert(false && "CompareEntriesBySortKind() got unrecognized SortKind");
    }

    auto
    PrintLibraries::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = MachO.isBigEndian();
        constexpr auto Malformed = std::string_view("<malformed>");

        auto DylibList = std::vector<DylibInfo>();
        auto LoadCommandIndex = uint32_t();

        for (const auto &LoadCommand : MachO.loadCommandsMap()) {
            using namespace MachO;
            if (LoadCommand.isSharedLibrary(IsBigEndian)) {
                const auto &DylibCmd =
                    cast<DylibCommand>(LoadCommand, IsBigEndian);

                const auto NameOpt = DylibCmd.name(IsBigEndian);
                const auto Info = DylibInfo {
                    .Name = NameOpt.has_value() ? NameOpt.value() : Malformed,
                    .Kind = DylibCmd.kind(IsBigEndian),
                    .CurrentVersion = DylibCmd.currentVersion(IsBigEndian),
                    .CompatVersion = DylibCmd.compatVersion(IsBigEndian),
                    .Timestamp = DylibCmd.timestamp(IsBigEndian),
                    .Index = LoadCommandIndex
                };

                DylibList.emplace_back(std::move(Info));
            }

            LoadCommandIndex++;
        }

        const auto &Opt = this->Opt;
        if (!Opt.SortKindList.empty()) {
            const auto Lambda = [&](const auto &Lhs, const auto &Rhs) noexcept {
                auto Compare = std::strong_ordering::equivalent;
                for (const auto &Sort : Opt.SortKindList) {
                    Compare = CompareEntriesBySortKind(Lhs, Rhs, Sort);
                    if (Compare != std::strong_ordering::equivalent) {
                        break;
                    }
                }

                return Compare == std::strong_ordering::less;
            };

            std::sort(DylibList.begin(), DylibList.end(), Lambda);
        }

        const auto OutFile = this->OutFile;
        std::println(OutFile,
                     "Provided file has {} Shared Libraries:",
                     Utils::FormattedNumber(DylibList.size()));

        const auto NcmdsDigitCount =
            Utils::GetIntegerDigitCount(MachO.header().ncmds());
        const auto LongestLCDylibKindLength =
            MachO::LoadCommandKindGetString(
                MachO::LoadCommandKind::LoadUpwardDylib).length();

        auto Counter = static_cast<uint32_t>(1);
        for (const auto &DylibInfo : DylibList) {
            std::print(OutFile,
                       "{}. LC {:>{}}: {:<{}} \"{}\"\n"
                       "\tCurrent Version: {}\n"
                       "\tCompat Version:  {}\n"
                       "\tTimestamp:       {} (Value: {})\n",
                       Counter,
                       DylibInfo.Index,
                       NcmdsDigitCount,
                       MachO::LoadCommandKindGetString(DylibInfo.Kind),
                       LongestLCDylibKindLength,
                       DylibInfo.Name,
                       DylibInfo.CurrentVersion,
                       DylibInfo.CompatVersion,
                       Utils::Timestamp(DylibInfo.Timestamp),
                       DylibInfo.Timestamp);

            Counter++;
        }

        return RunResult();
    }

    auto PrintLibraries::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintLibraries::run() got Object with Kind::None");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false && "Got unrecognized Object-Kind in PrintLibraries::run");
    }
}
