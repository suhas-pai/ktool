//
//  PrintLibraries.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

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

    auto
    PrintLibraries::run(const Objects::MachO &MachO) const noexcept ->
        RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);
        if (MachO.fileKind() != MachO::FileKind::DynamicLibrary) {
            return Result.set(RunError::NotADylib);
        }

        const auto LoadCommandsMemoryMap =
            ADT::MemoryMap(MachO.map(), MachO.header().loadCommandsRange());

        const auto IsBigEndian = MachO.isBigEndian();
        const auto LoadCommandsMap =
            ::MachO::LoadCommandsMap(LoadCommandsMemoryMap, IsBigEndian);

        for (const auto &LoadCommand : LoadCommandsMap) {
            if (const auto ID =
                    MachO::dyn_cast<MachO::LoadCommandKind::IdDylib>(
                        &LoadCommand, IsBigEndian))
            {
                const auto NameOpt = ID->name(IsBigEndian);
                if (!NameOpt) {
                    return Result.set(RunError::BadIdString);
                }

                fprintf(OutFile, "ID: %s\n", NameOpt.value().data());
                if (Opt.Verbose) {
                    const auto &Dylib = ID->Dylib;
                    const auto CurrentVersion =
                        Dylib.currentVersion(IsBigEndian);
                    const auto CompatVersion =
                        Dylib.compatVersion(IsBigEndian);

                    fprintf(OutFile,
                            "\tCurrent Version: " DYLD3_PACKED_VERSION_FMT "\n"
                            "\tCompat Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                            "\tTimestamp:       %d\n",
                            DYLD3_PACKED_VERSION_FMT_ARGS(CurrentVersion),
                            DYLD3_PACKED_VERSION_FMT_ARGS(CompatVersion),
                            Dylib.Timestamp);
                }

                return Result.set(RunError::None);
            }
        }

        return Result.set(RunError::IdNotFound);
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
