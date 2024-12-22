//
//  src/main.cpp
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#include <memory>
#include "Operations/Run.h"

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
            std::print(stderr,
                       "Please provide section-name by itself if segment-name "
                       "won't be provided\n");
            return 1;
        }

        if (CommaPos == SegmentSectionPair.length() - 1) {
            std::print(stderr, "Please provide a section-name\n");
            return 1;
        }

        SegmentName = SegmentSectionPair.substr(0, CommaPos);
        if (SegmentName->length() > MachO::SegmentSectionMaxNameLength) {
            std::print(stderr,
                       "Segment Name exceeds max length allowed ({})\n",
                       MachO::SegmentSectionMaxNameLength);
            return 1;
        }

        SectionName = SegmentSectionPair.substr(CommaPos + 1);
    } else {
        SectionName = std::move(SegmentSectionPair);
    }

    if (SectionName.length() > MachO::SegmentSectionMaxNameLength) {
        std::print(stderr,
                   "Section Name exceeds max length allowed ({})\n",
                   MachO::SegmentSectionMaxNameLength);
        return 1;
    }

    return 0;
}

static void
PrintBindOpcodeParseError(
    const MachO::BindInfoKind BindKind,
    const MachO::BindOpcodeParseResult ParseResult,
    const char *const Prefix = "") noexcept
{
    const auto BindKindString =
        BindKind == MachO::BindInfoKind::Normal ? "normal" :
        BindKind == MachO::BindInfoKind::Lazy ? "lazy" :
        BindKind == MachO::BindInfoKind::Weak ? "weak" : "<unknown>";

    switch (ParseResult.Error) {
        case MachO::BindOpcodeParseError::None:
            break;
        case MachO::BindOpcodeParseError::InvalidLeb128:
            std::print(stderr,
                       "{}Encountered invalid uleb128 when parsing {} "
                       "bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break;
        case MachO::BindOpcodeParseError::InvalidSegmentIndex:
            std::print(stderr,
                       "{}{} Bind-Opcodes set segment-index to an invalid "
                       "number\n",
                       Prefix,
                       BindKindString);
            break;
        case MachO::BindOpcodeParseError::InvalidString:
            std::print(stderr,
                       "{}Encountered invalid string in {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::NotEnoughThreadedBinds:
            std::print(stderr,
                       "{}Not enough threaded-binds in {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::TooManyThreadedBinds:
            std::print(stderr,
                       "{}Too many threaded-binds in {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::InvalidThreadOrdinal:
            std::print(stderr,
                       "{}Encountered invalid thread-ordinal in "
                       "{} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break;
        case MachO::BindOpcodeParseError::EmptySymbol:
            std::print(stderr,
                       "{}Encountered invalid thread-ordinal in "
                       "{} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break;
        case MachO::BindOpcodeParseError::IllegalBindOpcode:
            std::print(stderr,
                       "{}Encountered invalid {} bind-opcode when parsing\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::OutOfBoundsSegmentAddr:
            std::print(stderr,
                       "{}Got out-of-bounds segment-address in {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::UnrecognizedBindWriteKind:
            std::print(stderr,
                       "{}Encountered unknown write-kind in {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::UnrecognizedBindOpcode:
            std::print(stderr,
                       "{}Encountered unknown {} bind-opcode when parsing\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::UnrecognizedBindSubOpcode:
            std::print(stderr,
                       "{}Encountered unknown {} bind sub-opcode when parsing\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::
            UnrecognizedSpecialDylibOrdinal:
            std::print(stderr,
                       "{}Encountered unknown specialty dylib-ordinal in "
                       "{} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break;
        case MachO::BindOpcodeParseError::NoDylibOrdinal:
            std::print(stderr,
                       "{}No dylib-ordinal found when parsing {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::NoSegmentIndex:
            std::print(stderr,
                       "{}No segment-index found when parsing {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
        case MachO::BindOpcodeParseError::NoWriteKind:
            std::print(stderr,
                       "{}No write-type found when parsing {} bind-opcodes\n",
                       Prefix,
                       BindKindString);
            break  ;
    }
}

static void
PrintRebaseOpcodeParseError(
    const MachO::RebaseOpcodeParseResult &ParseResult,
    const char *const Prefix = "") noexcept
{
    switch (ParseResult.Error) {
        case MachO::RebaseOpcodeParseError::None:
            break;
        case MachO::RebaseOpcodeParseError::InvalidLeb128:
            std::print(stderr,
                       "{}Encountered invalid uleb128 when rebase-opcodes\n",
                       Prefix);
            break;
        case MachO::RebaseOpcodeParseError::IllegalRebaseOpcode:
            std::print(stderr,
                       "{}Encountered invalid rebase-opcode when parsing\n",
                       Prefix);
            break;
        case MachO::RebaseOpcodeParseError::OutOfBoundsSegmentAddr:
            std::print(stderr,
                       "{}Got out-of-bounds segment-address in rebase-opcodes\n",
                       Prefix);
            break;
        case MachO::RebaseOpcodeParseError::UnrecognizedRebaseWriteKind:
            std::print(stderr,
                       "{}Encountered unknown write-kind in rebase-opcodes\n",
                       Prefix);
            break;
        case MachO::RebaseOpcodeParseError::UnrecognizedRebaseOpcode:
            std::print(stderr,
                       "{}Encountered unknown rebase-opcode when parsing\n",
                       Prefix);
            break;
        case MachO::RebaseOpcodeParseError::NoSegmentIndex:
            std::print(stderr,
                       "{}No segment-index found when rebase-opcodes\n",
                       Prefix);
            break;
        case MachO::RebaseOpcodeParseError::NoWriteKind:
            std::print(stderr,
                       "{}No write-type found when rebase-opcodes\n",
                       Prefix);
            break;
    }
}

auto main(const int argc, const char *const argv[]) noexcept -> int {
    if (argc < 2) {
        std::print("Help Menu:\n");
        return 0;
    }

    auto I = 1;
    const auto OperationString = std::string_view(argv[1]);

    I++;
    if (OperationString.front() != '-') {
        std::print(stderr,
                   "Expected option, found {} instead\n",
                   OperationString);
        return 1;
    }

    auto Operation = OperationInfo();
    auto FileOptions = Operations::HandleFileOptions();

    if (OperationString == "-h" || OperationString == "--header") {
        auto Options = Operations::PrintHeader::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintHeader;
        Operation.Op =
            std::unique_ptr<Operations::PrintHeader>(
                new Operations::PrintHeader(stdout, Options));
    } else if (OperationString == "-id" || OperationString == "--identity") {
        auto Options = Operations::PrintId::Options();
        auto Path = std::string();

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
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
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintLibraries;
        Operation.Op =
            std::unique_ptr<Operations::PrintLibraries>(
                new Operations::PrintLibraries(stdout, Options));
    } else if (OperationString == "--list-archs") {
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
                    std::print(stderr,
                               "Option --limit expects a limit-number to be "
                               "provided\n");
                    return 1;
                }

                const auto LimitArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!LimitArgOpt.has_value()) {
                    std::print(stderr,
                               "{} is not a valid limit-number\n",
                               argv[I]);
                    return 1;
                }

                const auto Limit = LimitArgOpt.value();
                if (Limit == 0) {
                    std::print(stderr, "A limit of 0 is invalid\n");
                    return 1;
                }

                Options.Limit = Limit;
            } else if (Arg == "--sort") {
                Options.Sort = true;
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
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
                    std::print(stderr,
                               "Option --limit expects a limit-number to be "
                               "provided\n");
                    return 1;
                }

                const auto LimitArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!LimitArgOpt) {
                    std::print(stderr,
                               "{} is not a valid limit-number\n",
                               argv[I]);
                    return 1;
                }

                const auto Limit = LimitArgOpt.value();
                if (Limit == 0) {
                    std::print(stderr, "A limit of 0 is invalid\n");
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
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
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
            } else if (Arg == "--tab-length") {
                I++;
                if (I == argc) {
                    std::print("Option --tab-length expects a number to be "
                               "provided\n");
                    return 1;
                }

                const auto TabLengthArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!TabLengthArgOpt.has_value()) {
                    std::print(stderr, "{} is not a valid number\n", argv[I]);
                    return 1;
                }

                const auto TabLength = TabLengthArgOpt.value();
                if (TabLength == 0) {
                    std::print(stderr, "A tab-length of 0 is invalid\n");
                    return 1;
                }

                Options.TabLength = TabLength;
            } else if (Arg == "--require-kind") {
                I++;
                if (I == argc) {
                    std::print(stderr,
                               "Option {} needs to be provided an "
                               "export-kind\n",
                               Arg);
                    return 1;
                }

                const auto KindArg = std::string_view(argv[I]);
                const auto KindOpt =
                    MachO::ExportTrieExportKindGetFromString(KindArg);

                if (!KindOpt.has_value()) {
                    std::print(stderr,
                               "{} is not a valid export-kind\n",
                               KindArg);
                    return 1;
                }

                Options.KindRequirements.emplace(KindOpt.value());
            } else if (Arg == "--require-section") {
                I++;
                if (I == argc) {
                    std::print(stderr,
                               "Option {} needs to be provided a section\n",
                               Arg);
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
                    std::print(stderr,
                               "Option {} needs to be provided a section\n",
                               Arg);
                    return 1;
                }

                const auto SegmentArg = std::string_view(argv[I]);
                if (SegmentArg.length() > MachO::SegmentMaxNameLength) {
                    std::print(stderr,
                               "Segment-Name of \"{}\" is too long to be valid\n",
                               SegmentArg);
                    return 1;
                }

                Options.SectionRequirements.emplace_back(
                    Operations::PrintExportTrie::Options::SegmentSectionPair {
                        .SegmentName = std::string(SegmentArg),
                        .SectionName = ""
                    }
                );
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
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
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
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
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintBindActionList;
        Operation.Op =
            std::unique_ptr<Operations::PrintBindActionList>(
                new Operations::PrintBindActionList(stdout, Options));
    } else if (OperationString == "--bind-symbols") {
        auto Options = Operations::PrintBindSymbolList::Options();
        auto Path = std::string();
        auto HasResetKinds = false;

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            using SortKind =
                Operations::PrintBindSymbolList::Options::SortKind;

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
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintBindSymbolList;
        Operation.Op =
            std::unique_ptr<Operations::PrintBindSymbolList>(
                new Operations::PrintBindSymbolList(stdout, Options));
    } else if (OperationString == "--rebase-opcodes") {
        auto Options = Operations::PrintRebaseOpcodeList::Options();
        auto Path = std::string();

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintRebaseOpcodeList;
        Operation.Op =
            std::unique_ptr<Operations::PrintRebaseOpcodeList>(
                new Operations::PrintRebaseOpcodeList(stdout, Options));
    } else if (OperationString == "--rebase-actions") {
        auto Options = Operations::PrintRebaseActionList::Options();
        auto Path = std::string();

        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--sort") {
                Options.Sort = true;
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintRebaseActionList;
        Operation.Op =
            std::unique_ptr<Operations::PrintRebaseActionList>(
                new Operations::PrintRebaseActionList(stdout, Options));
    } else if (OperationString == "--objc-classes") {
        auto Options = Operations::PrintObjcClassList::Options();
        auto Path = std::string();

        using SortKind = Operations::PrintObjcClassList::Options::SortKind;
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--tree") {
                Options.PrintTree = true;
            } else if (Arg == "--tab-length") {
                I++;
                if (I == argc) {
                    std::print(stderr,
                               "Option --tab-length expects a number to be "
                               "provided\n");
                    return 1;
                }

                const auto TabLengthArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!TabLengthArgOpt.has_value()) {
                    std::print(stderr, "{} is not a valid number\n", argv[I]);
                    return 1;
                }

                const auto TabLength = TabLengthArgOpt.value();
                if (TabLength == 0) {
                    std::print(stderr, "A tab-length of 0 is invalid\n");
                    return 1;
                }

                Options.TabLength = TabLength;
            } else if (Arg == "--include-categories") {
                Options.PrintCategories = true;
            } else if (Arg == "--sort-by-name") {
                Options.SortKindList.emplace_back(SortKind::ByName);
            } else if (Arg == "--sort-by-dylib-ordinal") {
                Options.SortKindList.emplace_back(SortKind::ByDylibOrdinal);
            } else if (Arg == "--sort-by-kind") {
                Options.SortKindList.emplace_back(SortKind::ByKind);
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintObjcClassList;
        Operation.Op =
            std::unique_ptr<Operations::PrintObjcClassList>(
                new Operations::PrintObjcClassList(stdout, Options));
    } else if (OperationString == "--program-trie") {
        auto Options = Operations::PrintProgramTrie::Options();
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
            } else if (Arg == "--tab-length") {
                I++;
                if (I == argc) {
                    std::print(stderr,
                               "Option --tab-length expects a number to be "
                               "provided\n");
                    return 1;
                }

                const auto TabLengthArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!TabLengthArgOpt.has_value()) {
                    std::print(stderr, "{} is not a valid number\n", argv[I]);
                    return 1;
                }

                const auto TabLength = TabLengthArgOpt.value();
                if (TabLength == 0) {
                    std::print(stderr, "A tab-length of 0 is invalid\n");
                    return 1;
                }

                Options.TabLength = TabLength;
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintProgramTrie;
        Operation.Op =
            std::unique_ptr<Operations::PrintProgramTrie>(
                new Operations::PrintProgramTrie(stdout, Options));
    } else if (OperationString == "--list-images") {
        auto Options = Operations::PrintImageList::Options();
        auto Path = std::string();

        using SortKind = Operations::PrintImageList::Options::SortKind;
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg == "--only-count") {
                Options.OnlyCount = true;
            } else if (Arg == "--sort-by-address") {
                Options.SortKindList.emplace_back(SortKind::ByAddress);
            } else if (Arg == "--sort-by-name") {
                Options.SortKindList.emplace_back(SortKind::ByName);
            } else if (Arg == "--sort-by-inode") {
                Options.SortKindList.emplace_back(SortKind::ByInode);
            } else if (Arg == "--sort-by-modtime") {
                Options.SortKindList.emplace_back(SortKind::ByModTime);
            } else if (Arg.front() == '-') {
                std::print(stderr,
                           "Got unrecognized argument \"{}\" for option {}\n",
                           Arg,
                           OperationString);
                return 1;
            } else {
                break;
            }
        }

        Operation.Kind = Operations::Kind::PrintImageList;
        Operation.Op =
            std::unique_ptr<Operations::PrintImageList>(
                new Operations::PrintImageList(stdout, Options));
    } else if (OperationString.front() == '-') {
        std::print(stderr,
                   "Unrecognized operation: \"{}\"\n",
                   OperationString);
        return 1;
    } else {
        std::print(stderr,
                   "Expected operation, got \"{}\" instead\n",
                   OperationString);
        return 1;
    }

    assert(I <= argc);
    if (I == argc) {
        std::print(stderr, "Provided operation needs a path to a file\n");
        return 1;
    }

    const auto PathArg = std::string_view(argv[I]);
    if (PathArg.front() == '-') {
        std::print(stderr,
                   "Expected path to a file, got option {} instead\n",
                   PathArg);
        return 1;
    }

    Operation.Path = Utils::getFullPath(PathArg);
    FileOptions.Path = Operation.Path;

    for (I++; I != argc; I++) {
        const auto Arg = std::string_view(argv[I]);
        if (Arg == "-arch") {
            I++;
            if (I == argc) {
                std::print(stderr,
                           "Option {} expects an ordinal (number) to an "
                           "architecture.\nUse option --list-archs to see a "
                           "list of available architectures\n",
                           Arg);
                return 1;
            }

            const auto Arg = std::string_view(argv[I]);
            const auto ArchOrdinalOpt = Utils::to_uint<uint32_t>(Arg);

            if (!ArchOrdinalOpt.has_value()) {
                std::print(stderr, "{} is not a valid integer\n", argv[I]);
                return 1;
            }

            if (ArchOrdinalOpt.value() == 0) {
                std::print(stderr,
                           "0 is not a valid ordinal. For the first "
                           "image, use -arch 1\n",
                           argv[I]);
                return 1;
            }

            FileOptions.ArchIndex = ArchOrdinalOpt.value();
        } else if (Arg == "-image") {
            I++;
            if (I == argc) {
                std::print(stderr,
                           "Option {} expects an ordinal (number) to an "
                           "image.\nUse option --list-images to see a list of "
                           "available images\n",
                           Arg);
                return 1;
            }

            const auto IndexArg = std::string_view(argv[I]);
            const auto ImageIndexOpt = Utils::to_uint<uint32_t>(IndexArg);

            if (!ImageIndexOpt.has_value()) {
                std::print(stderr, "{} is not a valid number\n", argv[I]);
                return 1;
            }

            if (ImageIndexOpt.value() == 0) {
                std::print(stderr,
                           "An image number of 0 is invalid. For the first "
                           "image use -image 1\n");
                return 1;
            }

            FileOptions.ImageOrdinal = ImageIndexOpt.value();
        } else if (Arg == "-subcache") {
            if (I + 2 >= argc) {
                std::print(stderr,
                           "Option {} expects an file-suffix and a path to a "
                           "sub-cache.\n"
                           "Use option --list-subcache-suffixes to see a list "
                           "of file-suffixes expected for the associated dyld "
                           "shared-cache\n",
                           Arg);
                return 1;
            }

            const auto FileSuffix = std::string(argv[I + 1]);
            if (FileSuffix.front() == '-') {
                std::print(stderr,
                           "Expected path to a file, got option {} instead\n",
                           PathArg.data());
                return 1;
            }

            if (FileSuffix.length() >
                    sizeof(DyldSharedCache::SubCacheEntry::FileSuffix))
            {
                std::print(stderr,
                           "File-suffix {}is too long, maximum length is %zu\n",
                           FileSuffix.c_str(),
                           sizeof(DyldSharedCache::SubCacheEntry::FileSuffix));
                return 1;
            }

            const auto PathArg = std::string_view(argv[I + 2]);
            if (PathArg.front() == '-') {
                std::print(stderr,
                           "Expected path to a file, got option {} instead\n",
                           PathArg.data());
                return 1;
            }

            auto SubCacheProvidedPathInfo =
                Objects::DyldSharedCache::SubCacheProvidedPathInfo{
                    .Path = std::filesystem::absolute(PathArg),
                    .ReturnOnSubCacheError = true,
                };

            FileOptions.SubCacheProvidedPathMap.emplace(
                FileSuffix, std::move(SubCacheProvidedPathInfo));
        } else {
            std::print(stderr, "Unrecognized option: \"{}\"\n", Arg.data());
            return 1;
        }
    }

    const auto Result =
        Operations::RunAndHandleFile(*Operation.Op, FileOptions);

    assert(!Result.isUnsupportedError() &&
           "Internal Error: Operation is unsupported for object, but "
           "marked as supported in supportsObjectKind()");

    switch (Result.Kind) {
        case Operations::Kind::PrintHeader: {
            using RunResult = Operations::PrintHeader::RunResult;
            switch (Result.PrintHeaderResult.Error) {
                case RunResult::Error::None:
                    break;
            }

            break;
        }
        case Operations::Kind::PrintId: {
            using RunResult = Operations::PrintId::RunResult;
            switch (Result.PrintIdResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NotADylib:
                    std::print(stderr,
                               "Id string not available - not a dylib\n");
                    return 1;
                case RunResult::Error::BadIdString:
                    std::print(stderr, "Id String is malformed\n");
                    return 1;
                case RunResult::Error::IdNotFound:
                    std::print(stderr, "Id String not found\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintLoadCommands: {
            using RunResult = Operations::PrintLoadCommands::RunResult;
            switch (Result.PrintLoadCommandsResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoLoadCommands:
                    std::print(stderr,
                               "File does not contain any load commands\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintLibraries: {
            using RunResult = Operations::PrintLibraries::RunResult;
            switch (Result.PrintLibrariesResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
            }

            break;
        }
        case Operations::Kind::PrintArchs: {
            using RunResult = Operations::PrintArchs::RunResult;
            switch (Result.PrintArchsResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
            }

            break;
        }
        case Operations::Kind::PrintCStringSection: {
            using RunResult = Operations::PrintCStringSection::RunResult;
            switch (Result.PrintCStringSectionResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::EmptySectionName:
                    assert(false && "Internal Error: Empty Section Name");
                case RunResult::Error::SectionNotFound:
                    std::print(stderr,
                               "Provided section was not found\n"
                               "This may be because found sections were in a "
                               "protected segment\n");
                    return 1;
                case RunResult::Error::NotCStringSection:
                    std::print(stderr,
                               "Provided section is not a c-string section\n");
                    return 1;
                case RunResult::Error::HasNoStrings:
                    std::print(stderr,
                               "Provided section has no (printable) "
                               "c-strings\n");
                    return 1;
                case RunResult::Error::ProtectedSegment:
                    std::print(stderr,
                               "Provided section is in a protected segment\n");
                    break;
                }

            break;
        }
        case Operations::Kind::PrintSymbolPtrSection: {
            using RunResult = Operations::PrintSymbolPtrSection::RunResult;
            switch (Result.PrintSymbolPtrSectionResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::EmptySectionName:
                    assert(false && "Internal Error: Empty Section Name");
                case RunResult::Error::SectionNotFound:
                    std::print(stderr, "Provided section was not found\n"
                          "This may be because found sections were in a "
                          "protected segment\n");
                    return 1;
                case RunResult::Error::NotSymbolPointerSection:
                    std::print(stderr,
                               "Provided section is not a symbol pointer "
                               "section\n");
                    return 1;
                case RunResult::Error::ProtectedSegment:
                    std::print(stderr,
                               "Provided section is in a protected segment\n");
                    return 1;
                case RunResult::Error::InvalidSectionRange:
                    std::print(stderr,
                               "Provided section has an invalid file-range\n");
                    return 1;
                case RunResult::Error::SymTabNotFound:
                    std::print(stderr, "Couldn't find symtab_command\n");
                    return 1;
                case RunResult::Error::DynamicSymTabNotFound:
                    std::print(stderr, "Couldn't find dysymtab_command\n");
                    return 1;
                case RunResult::Error::MultipleSymTabCommands:
                    std::print(stderr, "Found multiple symtab_commands\n");
                    return 1;
                case RunResult::Error::MultipleDynamicSymTabCommands:
                    std::print(stderr, "Found multiple dysymtab_commands\n");
                    return 1;
                case RunResult::Error::IndexListOutOfBounds:
                    std::print(stderr,
                               "Index-List is out-of-bounds of mach-o\n");
                    return 1;
                case RunResult::Error::IndexOutOfBounds:
                    std::print(stderr, "Index is out-of-bounds of mach-o\n");
                    return 1;
                case RunResult::Error::SymbolTableOutOfBounds:
                    std::print(stderr,
                               "Symbol-Table is out-of-bounds of mach-o\n");
                    return 1;
                case RunResult::Error::StringTableOutOfBounds:
                    std::print(stderr,
                               "String-Table is out-of-bounds of mach-o\n");
                    return 1;
                }

            break;
        }
        case Operations::Kind::PrintExportTrie: {
            using RunResult = Operations::PrintExportTrie::RunResult;
            switch (Result.PrintExportTrieResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::MultipleExportTries:
                    std::print(stderr,
                               "Multiple different export-tries found\n");
                    return 1;
                case RunResult::Error::NoExportTrieFound:
                    std::print(stderr, "Failed to find export-trie\n");
                    return 1;
                case RunResult::Error::ExportTrieOutOfBounds:
                    std::print(stderr, "Export-trie is out-of-bounds\n");
                    return 1;
                case RunResult::Error::NoExports:
                    std::print(stderr, "Export-trie has no exported symbols\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintBindOpcodeList: {
            using RunResult = Operations::PrintBindOpcodeList::RunResult;
            switch (Result.PrintBindOpcodeListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoDyldInfo:
                    std::print(stderr, "No dyld-info load command was found\n");
                    return 1;
                case RunResult::Error::NoOpcodes:
                    std::print(stderr, "No bind-opcodes found within table\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintBindActionList: {
            using RunResult = Operations::PrintBindActionList::RunResult;
            switch (Result.PrintBindActionListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoDyldInfo:
                    std::print(stderr, "No dyld-info load command was found\n");
                    return 1;
                case RunResult::Error::NoActions:
                    std::print(stderr, "No bind-actions found within table\n");
                    return 1;
                case RunResult::Error::BindOpcodeParseError: {
                    const auto &ParseResult =
                        Result.PrintBindActionListResult.BindOpcodeParseResult;

                    std::print(stderr, "Error parsing bind-opcode:\n");
                    PrintBindOpcodeParseError(ParseResult.BindKind,
                                              ParseResult.ParseResult,
                                              "\t");
                    break;
                }
            }

            break;
        }
        case Operations::Kind::PrintBindSymbolList: {
            using RunResult = Operations::PrintBindSymbolList::RunResult;
            switch (Result.PrintBindSymbolListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoDyldInfo:
                    std::print(stderr, "No dyld-info load command was found\n");
                    return 1;
                case RunResult::Error::NoSymbols:
                    std::print(stderr, "No bind-symbols found within table\n");
                    return 1;
                case RunResult::Error::BindOpcodeParseError: {
                    const auto &ParseResult =
                        Result.PrintBindSymbolListResult.BindOpcodeParseResult;

                    std::print(stderr, "Error parsing bind-opcode:\n");
                    PrintBindOpcodeParseError(ParseResult.BindKind,
                                              ParseResult.ParseResult,
                                              "\t");
                    break;
                }
            }

            break;
        }
        case Operations::Kind::PrintRebaseOpcodeList: {
            using RunResult = Operations::PrintRebaseOpcodeList::RunResult;
            switch (Result.PrintRebaseOpcodeListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoDyldInfo:
                    std::print(stderr, "No dyld-info load command was found\n");
                    return 1;
                case RunResult::Error::NoOpcodes:
                    std::print(stderr,
                               "No rebase-opcodes found within table\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintRebaseActionList: {
            using RunResult = Operations::PrintRebaseActionList::RunResult;
            switch (Result.PrintRebaseActionListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoDyldInfo:
                    std::print(stderr, "No dyld-info load command was found\n");
                    return 1;
                case RunResult::Error::NoActions:
                    std::print(stderr,
                               "No rebase-actions found within table\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintObjcClassList: {
            using RunResult = Operations::PrintObjcClassList::RunResult;
            switch (Result.PrintObjcClassListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoDyldInfo:
                    std::print(stderr, "No dyld-info load command was found\n");
                    return 1;
                case RunResult::Error::NoObjcData:
                    std::print(stderr, "No objc class-list data was found\n");
                    return 1;
                case RunResult::Error::UnalignedSection:
                    std::print(stderr, "Objc class-list section is mis-aligned\n");
                    return 1;
                case RunResult::Error::ObjcDataOutOfBounds:
                    std::print(stderr,
                               "Objc class-list data is out-of-bounds of "
                               "file\n");
                    return 1;
                case RunResult::Error::BindOpcodeParseError: {
                    const auto &ParseResult =
                        Result.PrintObjcClassListResult;

                    PrintBindOpcodeParseError(
                        ParseResult.BindOpcodeParseResult.BindKind,
                        ParseResult.BindOpcodeParseResult.ParseResult,
                        "\t");

                    break;
                }
                case RunResult::Error::RebaseOpcodeParseError:
                    PrintRebaseOpcodeParseError(
                        Result.PrintObjcClassListResult
                            .RebaseOpcodeParseResult.ParseResult,
                        "\t");
                    break;
            }

            break;
        }
        case Operations::Kind::PrintProgramTrie: {
            using RunResult = Operations::PrintProgramTrie::RunResult;
            switch (Result.PrintProgramTrieResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoProgramTrie:
                    std::print(stderr, "File has no program-trie\n");
                    return 1;
                case RunResult::Error::OutOfBounds:
                    std::print(stderr,
                               "Program-trie is out-of-bounds of file\n");
                    return 1;
                case RunResult::Error::NoExports:
                    std::print(stderr, "Program-trie has no exported nodes\n");
                    return 1;
            }

            break;
        }
        case Operations::Kind::PrintImageList: {
            using RunResult = Operations::PrintImageList::RunResult;
            switch (Result.PrintImageListResult.Error) {
                case RunResult::Error::None:
                    break;
                case RunResult::Error::Unsupported:
                    assert(0 && "got unexpected unsupported error");
                case RunResult::Error::NoImages:
                    std::print(stderr, "File has no images\n");
                    return 1;
            }

            break;
        }
    }

    return 0;
}
