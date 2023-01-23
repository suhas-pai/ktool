//
//  Operations/PrintLibraries.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <algorithm>

#include "MachO/LoadCommandsMap.h"
#include "Operations/PrintLibraries.h"

#include "Utils/Print.h"

namespace Operations {
    PrintLibraries::PrintLibraries(FILE *const OutFile,
                                   const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintLibraries::supportsObjectKind(const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintLibraries::supportsObjectKind()");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
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
        const PrintLibraries::Options::SortKind SortKind) noexcept -> int
    {
        switch (SortKind) {
            case PrintLibraries::Options::SortKind::ByCurrentVersion:
                if (Lhs.CurrentVersion == Rhs.CurrentVersion) {
                    return 0;
                }

                if (Lhs.CurrentVersion < Rhs.CurrentVersion) {
                    return -1;
                }

                return 1;
            case PrintLibraries::Options::SortKind::ByCompatVersion: {
                if (Lhs.CompatVersion == Rhs.CompatVersion) {
                    return 0;
                } else if (Lhs.CompatVersion < Rhs.CompatVersion) {
                    return -1;
                }

                return 1;
            }
            case PrintLibraries::Options::SortKind::ByIndex: {
                if (Lhs.Index == Rhs.Index) {
                    return 0;
                } else if (Lhs.Index < Rhs.Index) {
                    return -1;
                }

                return 1;
            }
            case PrintLibraries::Options::SortKind::ByTimeStamp: {
                if (Lhs.Timestamp == Rhs.Timestamp) {
                    return 0;
                } else if (Lhs.Timestamp < Rhs.Timestamp) {
                    return -1;
                }

                return 1;
            }
            case PrintLibraries::Options::SortKind::ByName:
                return Lhs.Name.compare(Rhs.Name);
        }

        assert(false && "CompareEntriesBySortKind() got unrecognized SortKind");
    }

    auto
    PrintLibraries::run(const Objects::MachO &MachO) const noexcept ->
        RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);

        const auto IsBigEndian = MachO.isBigEndian();
        constexpr auto Malformed = std::string_view("<malformed>");

        auto DylibList = std::vector<DylibInfo>();
        auto LoadCommandIndex = uint32_t();

        for (const auto &LoadCommand : MachO.loadCommandsMap()) {
            using namespace MachO;
            using Kind = LoadCommandKind;

            if (const auto DylibCmd =
                    dyn_cast<Kind::LoadDylib,
                             Kind::LoadWeakDylib,
                             Kind::ReexportDylib,
                             Kind::LazyLoadDylib,
                             Kind::LoadUpwardDylib>(&LoadCommand, IsBigEndian))
            {
                const auto NameOpt = DylibCmd->name(IsBigEndian);
                const auto Info = DylibInfo {
                    .Name = NameOpt.has_value() ? NameOpt.value() : Malformed,
                    .Kind = DylibCmd->kind(IsBigEndian),
                    .CurrentVersion = DylibCmd->currentVersion(IsBigEndian),
                    .CompatVersion = DylibCmd->compatVersion(IsBigEndian),
                    .Timestamp = DylibCmd->timestamp(IsBigEndian),
                    .Index = LoadCommandIndex
                };

                DylibList.emplace_back(std::move(Info));
            }

            LoadCommandIndex++;
        }

        if (!Opt.SortKindList.empty()) {
            const auto Lambda = [&](const auto &Lhs, const auto &Rhs) noexcept {
                auto Compare = int();
                for (const auto &Sort : Opt.SortKindList) {
                    Compare = CompareEntriesBySortKind(Lhs, Rhs, Sort);
                    if (Compare != 0) {
                        break;
                    }
                }

                return (Compare < 0);
            };

            std::sort(DylibList.begin(), DylibList.end(), Lambda);
        }

        fprintf(OutFile,
                "Provided file has %" PRIuPTR " Shared Libraries:\n",
                DylibList.size());

        const auto NcmdsDigitCount =
            Utils::GetIntegerDigitCount(MachO.header().ncmds());
        const auto LongestLCDylibKindLength =
            MachO::LoadCommandKindGetString(
                MachO::LoadCommandKind::LoadUpwardDylib).length();

        auto Counter = static_cast<uint32_t>(1);
        for (const auto &DylibInfo : DylibList) {
            const auto TimestampString =
                Utils::GetHumanReadableTimestamp(DylibInfo.Timestamp);

            fprintf(OutFile,
                    "%" PRIu32 ". LC %" ZEROPAD_FMT PRIu32 ": "
                        "%" RIGHTPAD_FMT "s \"%s\"\n"
                    "\tCurrent Version: " DYLD3_PACKED_VERSION_FMT "\n"
                    "\tCompat Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                    "\tTimestamp:       %s (Value: %" PRIu32 ")\n",
                    Counter,
                    ZEROPAD_FMT_ARGS(NcmdsDigitCount),
                    DylibInfo.Index,
                    RIGHTPAD_FMT_ARGS(
                        static_cast<int>(LongestLCDylibKindLength)),
                    MachO::LoadCommandKindGetString(DylibInfo.Kind).data(),
                    DylibInfo.Name.data(),
                    DYLD3_PACKED_VERSION_FMT_ARGS(DylibInfo.CurrentVersion),
                    DYLD3_PACKED_VERSION_FMT_ARGS(DylibInfo.CompatVersion),
                    TimestampString.c_str(),
                    DylibInfo.Timestamp);

            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto PrintLibraries::run(const Objects::Base &Base) const noexcept ->
        RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintLibraries::run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false && "Got unrecognized Object-Kind in PrintLibraries::run");
    }
}
