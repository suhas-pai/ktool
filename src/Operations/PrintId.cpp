//
//  Operations/PrintId.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include "MachO/LoadCommandsMap.h"
#include "Operations/PrintId.h"

#include "Utils/Misc.h"
#include "Utils/Print.h"

namespace Operations {
    PrintId::PrintId(FILE *const OutFile, const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintId::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in PrintId::supportsObjectKind");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintId::supportsObjectKind");
    }

    auto
    PrintId::run(const Objects::MachO &MachO) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::MachO);
        if (MachO.fileKind() != MachO::FileKind::DynamicLibrary) {
            return Result.set(RunError::NotADylib);
        }

        const auto IsBigEndian = MachO.isBigEndian();
        for (const auto &LC : MachO.loadCommandsMap()) {
            using namespace MachO;
            using Kind = LoadCommandKind;

            if (const auto ID = dyn_cast<Kind::IdDylib>(&LC, IsBigEndian)) {
                const auto NameOpt = ID->name(IsBigEndian);
                auto Name = std::string_view("<malformed>");

                if (!NameOpt) {
                    if (!Opt.Verbose) {
                        return Result.set(RunError::BadIdString);
                    }
                } else {
                    Name = NameOpt.value();
                }

                fprintf(OutFile, "ID: %s\n", Name.data());
                if (Opt.Verbose) {
                    const auto &Dylib = ID->Dylib;
                    const auto CurrentVersion =
                        Dylib.currentVersion(IsBigEndian);
                    const auto CompatVersion =
                        Dylib.compatVersion(IsBigEndian);

                    const auto Timestamp = Dylib.timestamp(IsBigEndian);
                    const auto TimestampString =
                        Utils::GetHumanReadableTimestamp(Timestamp);

                    fprintf(OutFile,
                            "\tCurrent Version: " DYLD3_PACKED_VERSION_FMT "\n"
                            "\tCompat Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                            "\tTimestamp:       %s (Value: %" PRIu32 ")\n",
                            DYLD3_PACKED_VERSION_FMT_ARGS(CurrentVersion),
                            DYLD3_PACKED_VERSION_FMT_ARGS(CompatVersion),
                            TimestampString.data(),
                            Timestamp);
                }

                return Result.set(RunError::None);
            }
        }

        return Result.set(RunError::IdNotFound);
    }

    auto PrintId::run(const Objects::Base &Base) const noexcept ->
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

        assert(false && "Got unrecognized Object-Kind in PrintId::run");
    }
}
