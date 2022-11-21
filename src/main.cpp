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

struct Operation {
    std::string Path;
    Operations::Kind Kind;
    std::unique_ptr<Operations::Base> Op;
};

enum class ArgFlags : uint64_t {
    Option = 1 << 0,
    Path = 1 << 1,
    EverythingElse = 1 << 2
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

    [[nodiscard]] constexpr auto only(const ArgFlags Flag) {
        return (Value == Flag);
    }

    constexpr explicit ArgOptions() noexcept = default;
    constexpr explicit ArgOptions(const uint64_t Value) noexcept
    : Value(Value) {}
};

static void
HandleFatMachOArchObjectOpenError(const Operations::RunResult &Result,
                                  const uint32_t ArchIndex) noexcept
{
    if (Result.OpenError.isUnrecognizedFormat()) {
        fprintf(stderr,
                "Arch at index %d is of an unsupported "
                "format",
                ArchIndex);
        exit(1);
    }

    switch (Result.OpenError.Kind) {
        case Objects::Kind::None:
            assert(false && "Got Object-Kind None for OpenError\n");
        case Objects::Kind::MachO:
            using ErrorKind = Objects::MachO::OpenError;
            switch (ErrorKind(Result.OpenError.Error)) {
                case Objects::MachO::OpenError::None:
                    assert(false && "Got Error None for MachO OpenError");
                case Objects::MachO::OpenError::WrongFormat:
                    assert(false &&
                           "Got Error WrongFormat for MachO OpenError");
                case ErrorKind::SizeTooSmall:
                    fprintf(stderr,
                            "Arch at index %d is too small to be a valid "
                            "mach-o\n",
                            ArchIndex);
                    exit(1);
                case ErrorKind::TooManyLoadCommands:
                    fprintf(stderr,
                            "Arch at index %d has too many load-commands for "
                            "its size\n",
                            ArchIndex);
                    exit(1);
            }
        case Objects::Kind::FatMachO:
            assert(false && "Arch-Object is somehow a Fat-MachO");
    }
}

auto main(const int argc, const char *const argv[]) -> int {
    auto OperationList = std::vector<Operation>();
    for (auto I = 1; I < argc; I++) {
        auto Arg = std::string_view(argv[I]);
        if (Arg.front() != '-') {
            fprintf(stderr,
                    "Expected an option, found \"%s\" instead\n",
                    Arg.data());
            return 1;
        }

        const auto GetNextArg =
            [&](const char *const Need,
                const char *const Option,
                const ArgOptions ArgOptions = ::ArgOptions()) noexcept
        {
            if (I + 1 == argc) {
                fprintf(stderr, "Option %s needs %s\n", Option, Need);
                exit(1);
            }

            I++;

            const auto Result = std::string_view(argv[I]);
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

            return std::string(Result);
        };

        if (Arg == "-h" || Arg == "--header") {
            const char *const OptionArg = Arg.data();
            const auto ArgOptions =
                ::ArgOptions(ArgFlags::Option | ArgFlags::Path);

            auto Options = Operations::PrintHeader::Options();
            auto Path = std::string();

            while (true) {
                const auto NextArg =
                    GetNextArg("a path to a file", OptionArg, ArgOptions);

                if (NextArg == "-v" || NextArg == "--verbose") {
                    Options.Verbose = true;
                } else if (NextArg == "--arch-index") {
                    const auto IndexArg =
                        GetNextArg("an index to an arch", NextArg.data());

                    const auto ArchIndexOpt = ADT::to_int<uint32_t>(IndexArg);
                    if (!ArchIndexOpt) {
                        fprintf(stderr,
                                "%s is not a valid index-number\n",
                                argv[I]);
                        return 1;
                    }

                    Options.ArchIndex = ArchIndexOpt.value();
                } else {
                    Path = std::move(NextArg);
                    break;
                }
            }

            OperationList.push_back({
                std::move(Path),
                Operations::Kind::PrintHeader,
                std::unique_ptr<Operations::PrintHeader>(
                    new Operations::PrintHeader(stdout, Options))
            });
        } else {
            fprintf(stderr, "Unrecognized option: %s\n", Arg.data());
            return 1;
        }
    }

    for (const auto &Operation : OperationList) {
        switch (Operation.Kind) {
            case Operations::Kind::PrintHeader: {
                const auto FileMap =
                    ADT::FileMap(Operation.Path.c_str(),
                                 ADT::FileMap::Prot::Read);

                const auto Object = Objects::Open(FileMap.map());
                if (Object.hasError()) {
                    fprintf(stderr,
                            "File (at path %s) is not recognized\n",
                            Operation.Path.c_str());
                    return 1;
                }

                const auto Result = Operation.Op->run(*Object.getPtr());
                if (Result.isUnsupportedError()) {
                    fprintf(stderr,
                            "print-header isn't supported for file at "
                            "path %s\n",
                            Operation.Path.c_str());
                    return 1;
                }

                const auto Op =
                    static_cast<Operations::PrintHeader &>(*Operation.Op);

                switch (Operations::PrintHeader::RunError(Result.Error)) {
                    case Operations::PrintHeader::RunError::None:
                        break;
                    case Operations::PrintHeader::RunError::InvalidArchIndex: {
                        const auto ArchIndex =
                            static_cast<int32_t>(Op.options().ArchIndex);

                        fprintf(stderr,
                                "Arch-Index %d is invalid for path %s\n",
                                ArchIndex,
                                Operation.Path.c_str());
                        return 1;
                    }
                    case Operations::PrintHeader::RunError::ArchObjectOpenError:
                    {
                        const auto ArchIndex =
                            static_cast<int32_t>(Op.options().ArchIndex);

                        HandleFatMachOArchObjectOpenError(Result, ArchIndex);
                    }
                }

                delete Object.getPtr();
            }
        }
    }

    return 0;
}
