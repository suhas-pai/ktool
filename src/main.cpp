//
//  src/main.cpp
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#include <cstdio>
#include <memory>

#include "ADT/FileMap.h"
#include "MachO/LoadCommands.h"

#include "Operations/PrintHeader.h"
#include "Operations/PrintId.h"
#include "Operations/PrintLoadCommands.h"
#include "Operations/PrintLibraries.h"
#include "Operations/PrintArchs.h"
#include "Operations/PrintCStringSection.h"

#include "Operations/PrintSymbolPtrSection.h"
#include "Utils/Misc.h"

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
    constexpr explicit ArgOptions(const ArgFlags Value) noexcept
    : Value(static_cast<uint64_t>(Value)) {}
    constexpr explicit ArgOptions(const uint64_t Value) noexcept
    : Value(Value) {}
};

auto
ParseSegmentSectionPair(std::string &SegmentSectionPair,
                        std::optional<std::string> &SegmentName,
                        std::string &SectionName) noexcept
{
    if (const auto CommaPos = SegmentSectionPair.find(',');
        CommaPos != std::string::npos)
    {
        if (CommaPos == 0) {
            fputs("Please provide section-name by itself if segment-name "
                  "won't be provided\n",
                  stderr);
            return 1;
        }

        if (CommaPos == SegmentSectionPair.length() - 1) {
            fputs("Please provide a section-name\n", stderr);
            return 1;
        }

        SegmentName = SegmentSectionPair.substr(0, CommaPos);
        if (SegmentName->length() > 16) {
            fputs("Segment Name exceeds max length allowed (16)\n", stderr);
            return 1;
        }
        SectionName = SegmentSectionPair.substr(CommaPos + 1);
    } else {
        SectionName = std::move(SegmentSectionPair);
    }

    if (SectionName.length() > 16) {
        fputs("Section Name exceeds max length allowed (16)\n", stderr);
        return 1;
    }

    return 0;
}

auto main(const int argc, const char *const argv[]) noexcept -> int {
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
                        "Option %s needs %s. Found option instead\n",
                        Option,
                        Need);
                exit(1);
            }
        }

        if (!ArgOptions.path()) {
            if (Result.front() == '/') {
                fprintf(stderr,
                        "Option %s needs %s. Found path instead\n",
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
                    return Utils::getFullPath(Result);
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
    } else if (OperationString == "--archs") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        auto Options = Operations::PrintArchs::Options();
        while (true) {
            const auto NextArg =
                GetNextArg("", OperationString.data(), ArgOptions);

            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintArchs;
        Operation.Op =
            std::unique_ptr<Operations::PrintArchs>(
                new Operations::PrintArchs(stdout, Options));
    } else if (OperationString == "--cstrings") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::EverythingElse);

        auto Options = Operations::PrintCStringSection::Options();
        auto SegmentSectionPair = std::string();

        while (true) {
            const auto NextArg =
                GetNextArg("Section Name", OperationString.data(), ArgOptions);

            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else if (NextArg == "--limit") {
                const auto LimitArg =
                    GetNextArg("limit number",
                               OperationString.data(),
                               ::ArgOptions(ArgFlags::EverythingElse));

                const auto LimitArgOpt = Utils::to_int<uint32_t>(LimitArg);
                if (!LimitArgOpt) {
                    fprintf(stderr,
                            "%s is not a valid limit-number\n",
                            argv[I]);
                    return 1;
                }

                const auto Limit = LimitArgOpt.value();
                if (Limit == 0) {
                    fputs("A limit of 0 is invalid\n", stderr);
                    return 1;
                }

                Options.Limit = Limit;
            } else if (NextArg == "--sort") {
                Options.Sort = true;
            } else {
                SegmentSectionPair = std::move(NextArg.data());
                I++;

                break;
            }
        }

        auto SegmentName = std::optional<std::string>(std::nullopt);
        auto SectionName = std::string();

        const auto ParseSegSectPair =
            ParseSegmentSectionPair(SegmentSectionPair,
                                    SegmentName,
                                    SectionName);

        if (ParseSegSectPair != 0) {
            return ParseSegSectPair;
        }

        Operation.Kind = Operations::Kind::PrintCStringSection;
        Operation.Op =
            std::unique_ptr<Operations::PrintCStringSection>(
                new Operations::PrintCStringSection(stdout,
                                                    std::move(SegmentName),
                                                    std::move(SectionName),
                                                    Options));
    } else if (OperationString == "--symbol-ptrs") {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::EverythingElse);

        auto Options = Operations::PrintSymbolPtrSection::Options();
        auto SegmentSectionPair = std::string();

        while (true) {
            const auto NextArg =
                GetNextArg("Section Name", OperationString.data(), ArgOptions);

            using SortKind =
                Operations::PrintSymbolPtrSection::Options::SortKind;

            if (NextArg == "-v" || NextArg == "--verbose") {
                Options.Verbose = true;
            } else if (NextArg == "--limit") {
                const auto LimitArg =
                    GetNextArg("limit number",
                               OperationString.data(),
                               ::ArgOptions(ArgFlags::EverythingElse));

                const auto LimitArgOpt = Utils::to_int<uint32_t>(LimitArg);
                if (!LimitArgOpt) {
                    fprintf(stderr,
                            "%s is not a valid limit-number\n",
                            argv[I]);
                    return 1;
                }

                const auto Limit = LimitArgOpt.value();
                if (Limit == 0) {
                    fputs("A limit of 0 is invalid\n", stderr);
                    return 1;
                }

                Options.Limit = Limit;
            } else if (NextArg == "--sort-by-dylib-ordinal") {
                Options.SortKindList.emplace_back(SortKind::ByDylibOrdinal);
            } else if (NextArg == "--sort-by-dylib-path") {
                Options.SortKindList.emplace_back(SortKind::ByDylibPath);
            } else if (NextArg == "--sort-by-index") {
                Options.SortKindList.emplace_back(SortKind::ByIndex);
            } else if (NextArg == "--sort-by-string") {
                Options.SortKindList.emplace_back(SortKind::ByString);
            } else {
                SegmentSectionPair = std::move(NextArg.data());
                I++;

                break;
            }
        }

        auto SegmentName = std::optional<std::string>(std::nullopt);
        auto SectionName = std::string();

        ParseSegmentSectionPair(SegmentSectionPair, SegmentName, SectionName);

        Operation.Kind = Operations::Kind::PrintSymbolPtrSection;
        Operation.Op =
            std::unique_ptr<Operations::PrintSymbolPtrSection>(
                new Operations::PrintSymbolPtrSection(stdout,
                                                      std::move(SegmentName),
                                                      std::move(SectionName),
                                                      Options));
    } else {
        fprintf(stderr,
                "Unrecognized option: \"%s\"\n",
                OperationString.data());
        return 1;
    }

    const auto PathArg = std::string_view(argv[I]);
    if (PathArg.front() == '-') {
        fprintf(stderr,
                "Expected path to a file, got option %s instead\n",
                PathArg.data());
        return 1;
    }

    Operation.Path = Utils::getFullPath(PathArg);
    FileOptions.Path = Operation.Path;

    while (true) {
        const auto ArgOptions =
            ::ArgOptions(ArgFlags::Option | ArgFlags::NotNeeded);

        const auto NextArg = GetNextArg("", "", ArgOptions);
        if (NextArg == "--arch-index") {
            const auto IndexArg =
                GetNextArg("an index to an arch", NextArg.data());

            const auto ArchIndexOpt = Utils::to_int<uint32_t>(IndexArg);
            if (!ArchIndexOpt) {
                fprintf(stderr,
                        "%s is not a valid index-number\n",
                        argv[I]);
                return 1;
            }

            FileOptions.ArchIndex = ArchIndexOpt.value();
        } else if (NextArg.empty()) {
            break;
        } else {
            fprintf(stderr, "Unrecognized option \"%s\"\n", NextArg.c_str());
            return 1;
        }
    }

    const auto Result = Operation.Op->runAndHandleFile(FileOptions);
    assert(!Result.isUnsupportedError() &&
           "Internal Error: Operation is unsupported for object, but "
           "marked as supported in supportsObjectKind()");

    switch (Operation.Kind) {
        case Operations::Kind::PrintHeader: {
            switch (Operations::PrintHeader::RunError(Result.Error)) {
                case Operations::PrintHeader::RunError::None:
                    break;
            }

            break;
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

            break;
        }
        case Operations::Kind::PrintLoadCommands: {
            switch (Operations::PrintLoadCommands::RunError(Result.Error)) {
                case Operations::PrintLoadCommands::RunError::None:
                    break;
            }

            break;
        }
        case Operations::Kind::PrintLibraries: {
            switch (Operations::PrintLibraries::RunError(Result.Error)) {
                case Operations::PrintLibraries::RunError::None:
                    break;
            }

            break;
        }
        case Operations::Kind::PrintArchs: {
            switch (Operations::PrintArchs::RunError(Result.Error)) {
                case Operations::PrintArchs::RunError::None:
                    break;
            }

            break;
        }
        case Operations::Kind::PrintCStringSection: {
            switch (Operations::PrintCStringSection::RunError(Result.Error)) {
                using RunError = Operations::PrintCStringSection::RunError;
                case RunError::None:
                    break;
                case RunError::EmptySectionName:
                    assert(false && "Internal Error: Empty Section Name");
                case RunError::SectionNotFound:
                    fputs("Provided section was not found\n"
                          "This may be because found sections were in a "
                          "protected segment\n",
                          stderr);
                    return 1;
                case RunError::NotCStringSection:
                    fputs("Provided section is not a c-string section\n",
                          stderr);
                    return 1;
                case RunError::HasNoStrings:
                    fputs("Provided section has no (printable) c-strings\n",
                          stderr);
                    return 1;
                case RunError::ProtectedSegment:
                    fputs("Provided section is in a protected segment\n",
                          stderr);
                    break;
                }

            break;
        }
        case Operations::Kind::PrintSymbolPtrSection: {
            switch (Operations::PrintSymbolPtrSection::RunError(Result.Error)) {
                using RunError = Operations::PrintSymbolPtrSection::RunError;
                case RunError::None:
                    break;
                case RunError::EmptySectionName:
                    assert(false && "Internal Error: Empty Section Name");
                case RunError::SectionNotFound:
                    fputs("Provided section was not found\n"
                          "This may be because found sections were in a "
                          "protected segment\n",
                          stderr);
                    return 1;
                case RunError::NotSymbolPointerSection:
                    fputs("Provided section is not a symbol pointer section\n",
                          stderr);
                    return 1;
                case RunError::ProtectedSegment:
                    fputs("Provided section is in a protected segment\n",
                          stderr);
                    return 1;
                case RunError::InvalidSectionRange:
                    fputs("Provided section has an invalid file-range\n",
                          stderr);
                    return 1;
                case RunError::SymTabNotFound:
                    fputs("Couldn't find symtab_command\n", stderr);
                    return 1;
                case RunError::DynamicSymTabNotFound:
                    fputs("Couldn't find dysymtab_command\n", stderr);
                    return 1;
                case RunError::MultipleSymTabCommands:
                    fputs("Found multiple symtab_commands\n", stderr);
                    return 1;
                case RunError::MultipleDynamicSymTabCommands:
                    fputs("Found multiple dysymtab_commands\n", stderr);
                    return 1;
                case RunError::IndexListOutOfBounds:
                    fputs("Index-List is out-of-bounds of mach-o\n", stderr);
                    return 1;
                case RunError::IndexOutOfBounds:
                    fputs("Index is out-of-bounds of mach-o\n", stderr);
                    return 1;
                case RunError::SymbolTableOutOfBounds:
                    fputs("Symbol-Table is out-of-bounds of mach-o\n", stderr);
                    return 1;
                case RunError::StringTableOutOfBounds:
                    fputs("String-Table is out-of-bounds of mach-o\n", stderr);
                    return 1;
                }

            break;
        }
    }

    return 0;
}
