//
//  Operations/PrintLibraries.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <algorithm>

#include "ADT/Misc.h"
#include "ADT/PrintUtils.h"

#include "MachO/LoadCommandsMap.h"
#include "Operations/PrintLibraries.h"

namespace Operations {
    PrintLibraries::PrintLibraries(FILE *OutFile, const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintLibraries::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintLibraries::supportsObjectKind");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintLibraries::supportsObjectKind");
    }

    struct DylibInfo {
        std::string_view Name;
        MachO::LoadCommandKind Kind;

        Dyld3::PackedVersion CurrentVersion;
        Dyld3::PackedVersion CompatVersion;

        uint32_t Timestamp;
        uint32_t Index;
    };

    static int
    CompareEntriesBySortKind(
        const DylibInfo &Lhs,
        const DylibInfo &Rhs,
        const Operations::PrintLibraries::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            using SortKind = Operations::PrintLibraries::Options::SortKind;
            case SortKind::ByCurrentVersion:
                if (Lhs.CurrentVersion == Rhs.CurrentVersion) {
                    return 0;
                }

                if (Lhs.CurrentVersion < Rhs.CurrentVersion) {
                    return -1;
                }

                return 1;
            case SortKind::ByCompatVersion: {
                if (Lhs.CompatVersion == Rhs.CompatVersion) {
                    return 0;
                } else if (Lhs.CompatVersion < Rhs.CompatVersion) {
                    return -1;
                }

                return 1;
            }
            case SortKind::ByIndex: {
                if (Lhs.Index == Rhs.Index) {
                    return 0;
                } else if (Lhs.Index < Rhs.Index) {
                    return -1;
                }

                return 1;
            }
            case SortKind::ByTimeStamp: {
                if (Lhs.Timestamp == Rhs.Timestamp) {
                    return 0;
                } else if (Lhs.Timestamp < Rhs.Timestamp) {
                    return -1;
                }

                return 1;
            }
            case SortKind::ByName:
                return Lhs.Name.compare(Rhs.Name);
        }

        assert(false && "CompareEntriesBySortKind got unrecognized SortKind");
    }

    auto
    PrintLibraries::run(const Objects::MachO &MachO) const noexcept ->
        RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);
        const auto LoadCommandsMemoryMap =
            ADT::MemoryMap(MachO.map(), MachO.header().loadCommandsRange());

        const auto IsBigEndian = MachO.isBigEndian();
        const auto LoadCommandsMap =
            ::MachO::LoadCommandsMap(LoadCommandsMemoryMap, IsBigEndian);

        constexpr auto Malformed = std::string_view("<malformed>");

        auto DylibList = std::vector<DylibInfo>();
        auto LoadCommandIndex = uint32_t();

        for (const auto &LoadCommand : LoadCommandsMap) {
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
            const auto Comparator =
                [&](const auto &Lhs, const auto &Rhs) noexcept
            {
                for (const auto &Sort : Opt.SortKindList) {
                    const auto Compare =
                        CompareEntriesBySortKind(Lhs, Rhs, Sort);

                    if (Compare != 0) {
                        return (Compare < 0);
                    }
                }

                return false;
            };

            std::sort(DylibList.begin(), DylibList.end(), Comparator);
        }

        const auto DylibListSize = DylibList.size();
        fprintf(OutFile,
                "Provided file has %" PRIuPTR " Shared Libraries:\n",
                DylibListSize);

        auto Counter = static_cast<uint32_t>(1);
        for (const auto &DylibInfo : DylibList) {
            const auto TimestampString =
                ADT::GetHumanReadableTimestamp(DylibInfo.Timestamp);

            fprintf(OutFile,
                    "Library %" PRIu32 ": (LC %" PRIu32 ")\n"
                    "\tKind:            %s\n"
                    "\tPath:            \"%s\"\t\n"
                    "\tCurrent Version: " DYLD3_PACKED_VERSION_FMT "\n"
                    "\tCompat Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                    "\tTimestamp:       %s (Value: %" PRIu32 ")\n",
                    Counter,
                    DylibInfo.Index,
                    MachO::LoadCommandKindGetString(DylibInfo.Kind).data(),
                    DylibInfo.Name.data(),
                    DYLD3_PACKED_VERSION_FMT_ARGS(DylibInfo.CurrentVersion),
                    DYLD3_PACKED_VERSION_FMT_ARGS(DylibInfo.CompatVersion),
                    TimestampString.c_str(),
                    DylibInfo.Timestamp);

            fputc('\n', OutFile);
            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto PrintLibraries::run(const Objects::Base &Base) const noexcept ->
        RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false && "Got unrecognized Object-Kind in PrintLibraries::run");
    }
}