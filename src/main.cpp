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
#include "Operations/PrintExportTrie.h"
#include "Operations/PrintBindOpcodeList.h"
#include "Operations/PrintBindActionList.h"

struct OperationInfo {
    std::string Path;
    Operations::Kind Kind;

    std::unique_ptr<Operations::Base> Op;
};

auto
ParseSegmentSectionPair(std::string_view SegmentSectionPair,
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
        if (SegmentName->length() > MachO::SegmentSectionMaxNameLength) {
            fprintf(stderr,
                    "Segment Name exceeds max length allowed (%" PRIu32 ")\n",
                    MachO::SegmentSectionMaxNameLength);
            return 1;
        }
        SectionName = SegmentSectionPair.substr(CommaPos + 1);
    } else {
        SectionName = std::move(SegmentSectionPair);
    }

    if (SectionName.length() > MachO::SegmentSectionMaxNameLength) {
        fprintf(stderr,
                "Section Name exceeds max length allowed (%" PRIu32 ")\n",
                MachO::SegmentSectionMaxNameLength);
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
    const auto OperationString = std::string_view(argv[1]);

    I++;
    if (OperationString.front() != '-') {
        fprintf(stderr,
                "Expected option, found %s instead\n",
                OperationString.data());
        return 1;
    }

    auto Operation = OperationInfo();
    auto FileOptions = Operations::Base::HandleFileOptions();

    if (OperationString == "-h" || OperationString == "--print-header") {
        auto Options = Operations::PrintHeader::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
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
        auto Options = Operations::PrintId::Options();
        auto Path = std::string();

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
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
        auto Options = Operations::PrintLoadCommands::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
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
        auto Options = Operations::PrintLibraries::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            using SortKind = Operations::PrintLibraries::Options::SortKind;

            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--sort-by-current-version") {
                Options.SortKindList.emplace_back(SortKind::ByCurrentVersion);
            } else if (Arg == "--sort-by-compat-version") {
                Options.SortKindList.emplace_back(SortKind::ByCurrentVersion);
            } else if (Arg == "--sort-by-index") {
                Options.SortKindList.emplace_back(SortKind::ByIndex);
            } else if (Arg == "--sort-by-name") {
                Options.SortKindList.emplace_back(SortKind::ByName);
            } else if (Arg == "--sort-by-timestamp") {
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
        auto Options = Operations::PrintArchs::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
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
        auto Options = Operations::PrintCStringSection::Options();
        auto SegmentSectionPair = std::string();

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--limit") {
                I++;
                if (I == argc) {
                    fputs("Option --limit expects a limit-number to be "
                          "provided\n",
                          stderr);
                    return 1;
                }

                const auto LimitArgOpt = Utils::to_uint<uint32_t>(argv[I]);
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
            } else if (Arg == "--sort") {
                Options.Sort = true;
            } else {
                if (Arg.front() == '-') {
                    fprintf(stderr,
                            "Got unrecognized argument (%s) for option %s",
                            Arg.data(),
                            OperationString.data());
                }

                SegmentSectionPair = std::move(Arg.data());
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
        auto Options = Operations::PrintSymbolPtrSection::Options();
        auto SegmentSectionPair = std::string();

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            using SortKind =
                Operations::PrintSymbolPtrSection::Options::SortKind;

            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--limit") {
                I++;
                if (I == argc) {
                    fputs("Option --limit expects a limit-number to be "
                          "provided\n",
                          stderr);
                    return 1;
                }

                const auto LimitArgOpt = Utils::to_uint<uint32_t>(argv[I]);
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
            } else if (Arg == "--sort-by-dylib-ordinal") {
                Options.SortKindList.emplace_back(SortKind::ByDylibOrdinal);
            } else if (Arg == "--sort-by-dylib-path") {
                Options.SortKindList.emplace_back(SortKind::ByDylibPath);
            } else if (Arg == "--sort-by-index") {
                Options.SortKindList.emplace_back(SortKind::ByIndex);
            } else if (Arg == "--sort-by-string") {
                Options.SortKindList.emplace_back(SortKind::ByString);
            } else {
                if (Arg.front() == '-') {
                    fprintf(stderr,
                            "Got unrecognized argument (%s) for option %s",
                            Arg.data(),
                            OperationString.data());
                }

                SegmentSectionPair = std::move(Arg.data());
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
    } else if (OperationString == "--export-trie") {
        auto Options = Operations::PrintExportTrie::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--tree") {
                Options.PrintTree = true;
            } else if (Arg == "--only-count") {
                Options.OnlyCount = true;
            } else if (Arg == "--sort") {
                Options.Sort = true;
            } else if (Arg == "--require-kind") {
                I++;
                if (I == argc) {
                    fprintf(stderr,
                            "Option %s needs to be provided an export-kind\n",
                            Arg.data());
                    return 1;
                }

                const auto KindArg = std::string_view(argv[I]);
                const auto KindOpt =
                    MachO::ExportTrieExportKindGetFromString(argv[I]);

                if (!KindOpt) {
                    fprintf(stderr,
                            "%s is not a valid export-kind\n",
                            KindArg.data());
                    return 1;
                }

                Options.KindRequirements.emplace(KindOpt.value());
            } else if (Arg == "--require-section") {
                I++;
                if (I == argc) {
                    fprintf(stderr,
                            "Option %s needs to be provided a section\n",
                            Arg.data());
                    return 1;
                }

                auto SegmentSectionPair = std::string_view(argv[I]);
                auto SegmentName = std::optional<std::string>(std::nullopt);
                auto SectionName = std::string();

                ParseSegmentSectionPair(SegmentSectionPair,
                                        SegmentName,
                                        SectionName);

                Options.SectionRequirements.emplace_back(
                    Operations::PrintExportTrie::Options::SegmentSectionPair {
                        .SegmentName = SegmentName,
                        .SectionName = SectionName
                    }
                );
            } else if (Arg == "--require-segment") {
                I++;
                if (I == argc) {
                    fprintf(stderr,
                            "Option %s needs to be provided a section\n",
                            Arg.data());
                    return 1;
                }

                const auto SegmentArg = std::string_view(argv[I]);
                if (SegmentArg.length() > MachO::SegmentMaxNameLength) {
                    fprintf(stderr,
                            "Segment-Name of \"%s\" is too long to be valid\n",
                            SegmentArg.data());
                    return 1;
                }

                Options.SectionRequirements.emplace_back(
                    Operations::PrintExportTrie::Options::SegmentSectionPair {
                        .SegmentName = std::string(SegmentArg),
                        .SectionName = ""
                    }
                );
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintExportTrie;
        Operation.Op =
            std::unique_ptr<Operations::PrintExportTrie>(
                new Operations::PrintExportTrie(stdout, Options));
    } else if (OperationString == "--bind-opcodes") {
        auto Options = Operations::PrintBindOpcodeList::Options();
        auto Path = std::string();
        auto HasResetKinds = false;

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--only-normal") {
                if (!HasResetKinds) {
                    Options.PrintNormal = false;
                    Options.PrintLazy = false;
                    Options.PrintWeak = false;

                    HasResetKinds = true;
                }

                Options.PrintNormal = true;
            } else if (Arg == "--only-lazy") {
                if (!HasResetKinds) {
                    Options.PrintNormal = false;
                    Options.PrintLazy = false;
                    Options.PrintWeak = false;

                    HasResetKinds = true;
                }

                Options.PrintLazy = true;
            } else if (Arg == "--only-weak") {
                if (!HasResetKinds) {
                    Options.PrintNormal = false;
                    Options.PrintLazy = false;
                    Options.PrintWeak = false;

                    HasResetKinds = true;
                }

                Options.PrintWeak = true;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintBindOpcodeList;
        Operation.Op =
            std::unique_ptr<Operations::PrintBindOpcodeList>(
                new Operations::PrintBindOpcodeList(stdout, Options));
    } else if (OperationString == "--bind-actions") {
        auto Options = Operations::PrintBindActionList::Options();
        auto Path = std::string();
        auto HasResetKinds = false;

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            using SortKind =
                Operations::PrintBindActionList::Options::SortKind;

            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--only-normal") {
                if (!HasResetKinds) {
                    Options.PrintNormal = false;
                    Options.PrintLazy = false;
                    Options.PrintWeak = false;

                    HasResetKinds = true;
                }

                Options.PrintNormal = true;
            } else if (Arg == "--only-lazy") {
                if (!HasResetKinds) {
                    Options.PrintNormal = false;
                    Options.PrintLazy = false;
                    Options.PrintWeak = false;

                    HasResetKinds = true;
                }

                Options.PrintLazy = true;
            } else if (Arg == "--only-weak") {
                if (!HasResetKinds) {
                    Options.PrintNormal = false;
                    Options.PrintLazy = false;
                    Options.PrintWeak = false;

                    HasResetKinds = true;
                }

                Options.PrintWeak = true;
            } else if (Arg == "--sort-by-name") {
                Options.SortKindList.emplace_back(SortKind::ByName);
            } else if (Arg == "--sort-by-dylib-ordinal") {
                Options.SortKindList.emplace_back(SortKind::ByDylibOrdinal);
            } else if (Arg == "--sort-by-kind") {
                Options.SortKindList.emplace_back(SortKind::ByKind);
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintBindActionList;
        Operation.Op =
            std::unique_ptr<Operations::PrintBindActionList>(
                new Operations::PrintBindActionList(stdout, Options));
    } else if (OperationString.front() == '-') {
        fprintf(stderr,
                "Unrecognized operation: \"%s\"\n",
                OperationString.data());
        return 1;
    } else {
        fprintf(stderr,
                "Expected operation, got \"%s\" instead\n",
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

    for (I++; I != argc; I++) {
        const auto Arg = std::string_view(argv[I]);
        if (Arg == "--arch-index") {
            I++;
            if (I == argc) {
                fprintf(stderr,
                        "Option %s expects an index to an architecture.\n"
                        "Use option --archs to see a list of available "
                        "architectures\n",
                        Arg.data());
                return 1;
            }

            const auto IndexArg = std::string_view(argv[I]);
            const auto ArchIndexOpt = Utils::to_uint<uint32_t>(IndexArg);

            if (!ArchIndexOpt) {
                fprintf(stderr,
                        "%s is not a valid index-number\n",
                        argv[I]);
                return 1;
            }

            FileOptions.ArchIndex = ArchIndexOpt.value();
        } else {
            fprintf(stderr, "Unrecognized option: \"%s\"\n", Arg.data());
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
        case Operations::Kind::PrintExportTrie:
            switch (Operations::PrintExportTrie::RunError(Result.Error)) {
                using RunError = Operations::PrintExportTrie::RunError;

                case RunError::None:
                    break;
                case RunError::MultipleExportTries:
                    fputs("Multiple different export-tries found\n", stderr);
                    return 1;
                case RunError::NoExportTrieFound:
                    fputs("Failed to find export-trie\n", stderr);
                    return 1;
                case RunError::ExportTrieOutOfBounds:
                    fputs("Export-trie is out-of-bounds\n", stderr);
                    return 1;
                case RunError::NoExports:
                    fputs("Export-trie has no exported symbols\n", stderr);
                    return 1;
            }

            break;
        case Operations::Kind::PrintBindOpcodeList:
            switch (Operations::PrintBindOpcodeList::RunError(Result.Error)) {
                case Operations::PrintBindOpcodeList::RunError::None:
                    break;
                case Operations::PrintBindOpcodeList::RunError::NoDyldInfo:
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case Operations::PrintBindOpcodeList::RunError::NoOpcodes:
                    fputs("No bind-opcodes found within table\n", stderr);
                    return 1;
            }

            break;
        case Operations::Kind::PrintBindActionList:
            switch (Operations::PrintBindActionList::RunError(Result.Error)) {
                case Operations::PrintBindActionList::RunError::None:
                    break;
                case Operations::PrintBindActionList::RunError::NoDyldInfo:
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case Operations::PrintBindActionList::RunError::NoActions:
                    fputs("No bind-actions found within table\n", stderr);
                    return 1;
            }
    }

    return 0;
}
