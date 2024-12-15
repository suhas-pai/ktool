//
//  Operations/PrintId.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include "Operations/PrintId.h"
#include "Utils/Print.h"

namespace Operations {
    PrintId::PrintId(FILE *const OutFile,
                     const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintId), OutFile(OutFile), Opt(Options) {}

    bool
    PrintId::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in PrintId::supportsObjectKind()");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return true;
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintId::supportsObjectKind()");
    }

    auto PrintId::run(const Objects::MachO &MachO) const noexcept -> RunResult {
        if (MachO.fileKind() != MachO::FileKind::DynamicLibrary &&
            MachO.fileKind() != MachO::FileKind::DynamicLinker)
        {
            return RunResult(RunResult::Error::NotADylib);
        }

        const auto IsBigEndian = MachO.isBigEndian();
        const auto LoadCommandsMap = MachO.loadCommandsMap();

        for (auto Iter = LoadCommandsMap.begin();
             Iter != LoadCommandsMap.end();
             Iter++)
        {
            using Kind = MachO::LoadCommandKind;
            if (const auto ID = Iter.dyn_cast<Kind::IdDylib>()) {
                auto Name = std::string_view("<malformed>");
                if (const auto NameOpt = ID->name(IsBigEndian)) {
                    Name = NameOpt.value();
                } else {
                    if (!Opt.Verbose) {
                        return RunResult(RunResult::Error::BadIdString);
                    }
                }

                fprintf(OutFile, "ID: \"%s\"\n", Name.data());
                if (Opt.Verbose) {
                    const auto &Dylib = ID->Dylib;
                    const auto CurrentVersion =
                        Dylib.currentVersion(IsBigEndian);
                    const auto CompatVersion = Dylib.compatVersion(IsBigEndian);

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

                return RunResult();
            }
        }

        return RunResult(RunResult::Error::IdNotFound);
    }

    auto PrintId::run(const Objects::DscImage &Image) const noexcept
        -> RunResult
    {
        if (Opt.Verbose) {
            return run(static_cast<const Objects::MachO &>(Image));
        }

        const auto PathOpt = Image.path();
        fprintf(OutFile,
                "\"" STRING_VIEW_FMT "\"",
                STRING_VIEW_FMT_ARGS(
                    PathOpt.has_value() ? PathOpt.value() : "<invalid>"));

        return RunResult();
    }

    auto PrintId::run(const Objects::Base &Base) const noexcept -> RunResult {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "PrintId::run() got Object with Kind::None");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false && "Got unrecognized Object-Kind in PrintId::run()");
    }
}
