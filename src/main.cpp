//
//  src/main.cpp
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#include <memory>
#include <string_view>
#include <vector>

#include "ADT/FileMap.h"
#include "ADT/Misc.h"

#include "MachO/LoadCommands.h"

#include "Operations/PrintHeader.h"
#include "Operations/PrintId.h"
#include "Operations/PrintLoadCommands.h"
#include "Operations/PrintLibraries.h"

struct OperationInfo {
    std::string Path;
    Operations::Kind Kind;
    std::unique_ptr<Operations::Base> Op;
};

enum class ArgFlags : uint64_t {
    Option = 1 << 0,
    Path = 1 << 1,
    EverythingElse = 1 << 2,

    NotNeeded = 1 << 3
};

MAKE_ENUM_MASK_CLASS(ArgFlags);

struct ArgOptions {
    uint64_t Value = std::numeric_limits<uint64_t>::max();
    [[nodiscard]] constexpr auto option() const noexcept {
        return (Value & ArgFlags::Option);
    }

    [[nodiscard]] constexpr auto path() const noexcept {
        return (Value & ArgFlags::Path);
    }

    [[nodiscard]] constexpr auto everythingElse() const noexcept {
        return (Value & ArgFlags::EverythingElse);
    }

    [[nodiscard]] constexpr auto notNeeded() const noexcept {
        return (Value & ArgFlags::NotNeeded);
    }

    [[nodiscard]] constexpr auto only(const ArgFlags Flag) {
        return (Value == Flag);
    }

    constexpr explicit ArgOptions() noexcept = default;
    constexpr explicit ArgOptions(const uint64_t Value) noexcept
    : Value(Value) {}
};

auto main(const int argc, const char *const argv[]) -> int {
    if (argc < 2) {
        fprintf(stderr, "Help Menu:\n");
        return 0;
    }

    auto I = 1;
    const auto GetNextArg =
        [&](const char *const Need,
            const char *const Option,
            const ArgOptions ArgOptions = ::ArgOptions()) noexcept
    {
        if (I + 1 == argc) {
            if (ArgOptions.notNeeded()) {
                return std::string();
            }

            fprintf(stderr, "Option %s needs %s\n", Option, Need);
            exit(1);
        }

        I++;

        const auto Result = std::string_view(argv[I]);
        if (ArgOptions.notNeeded()) {
            return std::string(Result);
        }

        if (!ArgOptions.option()) {
            if (Result.front() == '-') {
                fprintf(stderr,
                        "Option %s needs %s. Found path instead\n",
                        Option,
                        Need);
                exit(1);
            }
        }

        if (!ArgOptions.path()) {
            if (Result.front() == '/') {
                fprintf(stderr,
                        "Option %s needs %s. Found option instead\n",
                        Option,
                        Need);
                exit(1);
            }
        }

        if (ArgOptions.option() &&
            ArgOptions.path() &&
            !ArgOptions.everythingElse())
        {
            if (Result.front() != '/') {
                if (Result.front() != '-') {
                    return ADT::getFullPath(Result);
                }
            }
        }

        if (ArgOptions.option() &&
            !ArgOptions.path() &&
            !ArgOptions.everythingElse())
        {
            if (Result.front() != '-') {
                fprintf(stderr,
                        "Expected option, found %s instead\n",
                        Result.data());
                exit(1);
            }
        }

        return std::string(Result);
    };

    const auto OperationString = std::string_view(argv[1]);
    if (OperationString.front() != '-') {
        fprintf(stderr,
                "Expected option, found %s instead\n",
                OperationString.data());
        return 1;
    }

    auto Operation = OperationInfo();
    auto FileOptions = Operations::Base::HandleFileOptions();

    if (OperationString == "-h" || OperationString == "--print-header") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        auto Options = Operations::PrintHeader::Options();
        while (true) {
            const auto NextArg =
                GetNextArg("", OperationString.data(), ArgOptions);

            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintHeader;
        Operation.Op =
            std::unique_ptr<Operations::PrintHeader>(
                new Operations::PrintHeader(stdout, Options));
    } else if (OperationString == "--id") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        auto Options = Operations::PrintId::Options();
        auto Path = std::string();

        while (true) {
            const auto NextArg =
                GetNextArg("", OperationString.data(), ArgOptions);

            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintId;
        Operation.Op =
            std::unique_ptr<Operations::PrintId>(
                new Operations::PrintId(stdout, Options));
    } else if (OperationString == "-l" || OperationString == "--lc") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        auto Options = Operations::PrintLoadCommands::Options();
        while (true) {
            const auto NextArg =
                GetNextArg("", OperationString.data(), ArgOptions);

            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintLoadCommands;
        Operation.Op =
            std::unique_ptr<Operations::PrintLoadCommands>(
                new Operations::PrintLoadCommands(stdout, Options));
    } else if (OperationString == "-L" || OperationString == "--libraries") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        auto Options = Operations::PrintLibraries::Options();
        while (true) {
            const auto NextArg =
                GetNextArg("", OperationString.data(), ArgOptions);

            using SortKind = Operations::PrintLibraries::Options::SortKind;
            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else if (NextArg == "--sort-by-current-version") {
                Options.SortKindList.emplace_back(SortKind::ByCurrentVersion);
            } else if (NextArg == "--sort-by-compat-version") {
                Options.SortKindList.emplace_back(SortKind::ByCurrentVersion);
            } else if (NextArg == "--sort-by-index") {
                Options.SortKindList.emplace_back(SortKind::ByIndex);
            } else if (NextArg == "--sort-by-name") {
                Options.SortKindList.emplace_back(SortKind::ByName);
            } else if (NextArg == "--sort-by-timestamp") {
                Options.SortKindList.emplace_back(SortKind::ByTimeStamp);
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintLibraries;
        Operation.Op =
            std::unique_ptr<Operations::PrintLibraries>(
                new Operations::PrintLibraries(stdout, Options));
    } else {
        fprintf(stderr, "Unrecognized option: %s\n", OperationString.data());
        return 1;
    }

    const auto PathArg = std::string_view(argv[I]);
    if (PathArg.front() == '-') {
        fprintf(stderr,
                "Expected path to a file, got option %s instead\n",
                PathArg.data());
        return 1;
    }

    Operation.Path = ADT::getFullPath(PathArg);
    FileOptions.Path = Operation.Path;
    
    while (true) {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        const auto NextArg = GetNextArg("", "", ArgOptions);
        if (NextArg == "--arch-index") {
            const auto IndexArg =
                GetNextArg("an index to an arch", NextArg.data());

            const auto ArchIndexOpt = ADT::to_int<uint32_t>(IndexArg);
            if (!ArchIndexOpt) {
                fprintf(stderr,
                        "%s is not a valid index-number\n",
                        argv[I]);
                return 1;
            }

            FileOptions.ArchIndex = ArchIndexOpt.value();
        } else {
            break;
        }
    }

    const auto Result = Operation.Op->runAndHandleFile(FileOptions);
    switch (Operation.Kind) {
        case Operations::Kind::PrintHeader: {
            switch (Operations::PrintHeader::RunError(Result.Error)) {
                case Operations::PrintHeader::RunError::None:
                    break;
            }
        }
        case Operations::Kind::PrintId: {
            switch (Operations::PrintId::RunError(Result.Error)) {
                case Operations::PrintId::RunError::None:
                    break;
                case Operations::PrintId::RunError::NotADylib:
                    fputs("Id string not available - not a dylib\n", stderr);
                    return 1;
                case Operations::PrintId::RunError::BadIdString:
                    fputs("Id String is malformed\n", stderr);
                    return 1;
                case Operations::PrintId::RunError::IdNotFound:
                    fputs("Id String not found\n", stderr);
                    return 1;
            }
        }
        case Operations::Kind::PrintLibraries: {
            switch (Operations::PrintLibraries::RunError(Result.Error)) {
                case Operations::PrintLibraries::RunError::None:
                    break;
            }
        }
        case Operations::Kind::PrintLoadCommands: {
            switch (Operations::PrintLoadCommands::RunError(Result.Error)) {
                case Operations::PrintLoadCommands::RunError::None:
                    break;
            }
        }
    }

    return 0;
}
