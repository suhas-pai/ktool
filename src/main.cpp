//
//  src/main.cpp
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#include <memory>

#include "MachO/BindInfo.h"
#include "Operations/PrintHeader.h"
#include "Operations/PrintId.h"
#include "Operations/PrintImageList.h"
#include "Operations/PrintLoadCommands.h"
#include "Operations/PrintLibraries.h"
#include "Operations/PrintArchs.h"
#include "Operations/PrintCStringSection.h"
#include "Operations/PrintSymbolPtrSection.h"
#include "Operations/PrintExportTrie.h"
#include "Operations/PrintBindOpcodeList.h"
#include "Operations/PrintBindActionList.h"
#include "Operations/PrintBindSymbolList.h"
#include "Operations/PrintRebaseOpcodeList.h"
#include "Operations/PrintRebaseActionList.h"
#include "Operations/PrintObjcClassList.h"
#include "Operations/PrintProgramTrie.h"

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
            fprintf(stderr,
                    "%sEncountered invalid uleb128 when parsing %s "
                    "bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::InvalidSegmentIndex:
            fprintf(stderr,
                    "%s%s Bind-Opcodes set segment-index to an invalid "
                    "number\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::InvalidString:
            fprintf(stderr,
                    "%sEncountered invalid string in %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::NotEnoughThreadedBinds:
            fprintf(stderr,
                    "%sNot enough threaded-binds in %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::TooManyThreadedBinds:
            fprintf(stderr,
                    "%sToo many threaded-binds in %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::InvalidThreadOrdinal:
            fprintf(stderr,
                    "%sEncountered invalid thread-ordinal in "
                    "%s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::EmptySymbol:
            fprintf(stderr,
                    "%sEncountered invalid thread-ordinal in "
                    "%s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::IllegalBindOpcode:
            fprintf(stderr,
                    "%sEncountered invalid %s bind-opcode when parsing\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::OutOfBoundsSegmentAddr:
            fprintf(stderr,
                    "%sGot out-of-bounds segment-address in %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::UnrecognizedBindWriteKind:
            fprintf(stderr,
                    "%sEncountered unknown write-kind in %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::UnrecognizedBindOpcode:
            fprintf(stderr,
                    "%sEncountered unknown %s bind-opcode when parsing\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::UnrecognizedBindSubOpcode:
            fprintf(stderr,
                    "%sEncountered unknown %s bind sub-opcode when parsing\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::
            UnrecognizedSpecialDylibOrdinal:
            fprintf(stderr,
                    "%sEncountered unknown specialty dylib-ordinal in "
                    "%s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::NoDylibOrdinal:
            fprintf(stderr,
                    "%sNo dylib-ordinal found when parsing %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::NoSegmentIndex:
            fprintf(stderr,
                    "%sNo segment-index found when parsing %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
        case MachO::BindOpcodeParseError::NoWriteKind:
            fprintf(stderr,
                    "%sNo write-type found when parsing %s bind-opcodes\n",
                    Prefix,
                    BindKindString);
            break;
    }
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
    auto FileOptions = Operations::HandleFileOptions();

    if (OperationString == "-h" || OperationString == "--header") {
        auto Options = Operations::PrintHeader::Options();
        for (; I != argc; I++) {
            const auto Arg = std::string_view(argv[I]);
            if (Arg == "-v" || Arg == "--verbose") {
                Options.Verbose = true;
            } else if (Arg.front() == '-') {
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                    fputs("Option --limit expects a limit-number to be "
                          "provided\n",
                          stderr);
                    return 1;
                }

                const auto LimitArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!LimitArgOpt.has_value()) {
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
            } else if (Arg.front() == '-') {
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
            } else if (Arg.front() == '-') {
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                    fputs("Option --tab-length expects a number to be "
                          "provided\n",
                          stderr);
                    return 1;
                }

                const auto TabLengthArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!TabLengthArgOpt.has_value()) {
                    fprintf(stderr, "%s is not a valid number\n", argv[I]);
                    return 1;
                }

                const auto TabLength = TabLengthArgOpt.value();
                if (TabLength == 0) {
                    fputs("A tab-length of 0 is invalid\n", stderr);
                    return 1;
                }

                Options.TabLength = TabLength;
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
                    MachO::ExportTrieExportKindGetFromString(KindArg);

                if (!KindOpt.has_value()) {
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
            } else if (Arg.front() == '-') {
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                    fputs("Option --tab-length expects a number to be "
                          "provided\n",
                          stderr);
                    return 1;
                }

                const auto TabLengthArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!TabLengthArgOpt.has_value()) {
                    fprintf(stderr, "%s is not a valid number\n", argv[I]);
                    return 1;
                }

                const auto TabLength = TabLengthArgOpt.value();
                if (TabLength == 0) {
                    fputs("A tab-length of 0 is invalid\n", stderr);
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                    fputs("Option --tab-length expects a number to be "
                          "provided\n",
                          stderr);
                    return 1;
                }

                const auto TabLengthArgOpt = Utils::to_uint<uint32_t>(argv[I]);
                if (!TabLengthArgOpt.has_value()) {
                    fprintf(stderr, "%s is not a valid number\n", argv[I]);
                    return 1;
                }

                const auto TabLength = TabLengthArgOpt.value();
                if (TabLength == 0) {
                    fputs("A tab-length of 0 is invalid\n", stderr);
                    return 1;
                }

                Options.TabLength = TabLength;
            } else if (Arg.front() == '-') {
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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
                fprintf(stderr,
                        "Got unrecognized argument \"%s\" for option %s\n",
                        Arg.data(),
                        OperationString.data());
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

    assert(I <= argc);
    if (I == argc) {
        fputs("Provided operation needs a path to a file\n", stderr);
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
        if (Arg == "-arch") {
            I++;
            if (I == argc) {
                fprintf(stderr,
                        "Option %s expects an index to an architecture.\n"
                        "Use option --list-archs to see a list of available "
                        "architectures\n",
                        Arg.data());
                return 1;
            }

            const auto IndexArg = std::string_view(argv[I]);
            const auto ArchIndexOpt = Utils::to_uint<uint32_t>(IndexArg);

            if (!ArchIndexOpt.has_value()) {
                fprintf(stderr,
                        "%s is not a valid index-number\n",
                        argv[I]);
                return 1;
            }

            FileOptions.ArchIndex = ArchIndexOpt.value();
        } else if (Arg == "-image") {
            I++;
            if (I == argc) {
                fprintf(stderr,
                        "Option %s expects an index to an image.\n"
                        "Use option --list-images to see a list of available "
                        "images\n",
                        Arg.data());
                return 1;
            }

            const auto IndexArg = std::string_view(argv[I]);
            const auto ImageIndexOpt = Utils::to_uint<uint32_t>(IndexArg);

            if (!ImageIndexOpt) {
                fprintf(stderr,
                        "%s is not a valid index-number\n",
                        argv[I]);
                return 1;
            }

            FileOptions.ImageIndex = ImageIndexOpt.value();
        } else if (Arg == "-subcache") {
            if (I + 2 >= argc) {
                fprintf(stderr,
                        "Option %s expects an file-suffix and a path to a "
                        "sub-cache.\n"
                        "Use option --list-subcache-suffixes to see a list of "
                        "file-suffixes expected for the associated dyld "
                        "shared-cache\n",
                        Arg.data());
                return 1;
            }

            const auto FileSuffix = std::string(argv[I + 1]);
            if (FileSuffix.front() == '-') {
                fprintf(stderr,
                        "Expected path to a file, got option %s instead\n",
                        PathArg.data());
                return 1;
            }

            if (FileSuffix.length() >
                    sizeof(DyldSharedCache::SubCacheEntry::FileSuffix))
            {
                fprintf(stderr,
                        "File-suffix %sis too long, maximum length is %zu\n",
                        FileSuffix.c_str(),
                        sizeof(DyldSharedCache::SubCacheEntry::FileSuffix));
                return 1;
            }

            const auto PathArg = std::string_view(argv[I + 2]);
            if (PathArg.front() == '-') {
                fprintf(stderr,
                        "Expected path to a file, got option %s instead\n",
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
            fprintf(stderr, "Unrecognized option: \"%s\"\n", Arg.data());
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
                    fputs("Id string not available - not a dylib\n", stderr);
                    return 1;
                case RunResult::Error::BadIdString:
                    fputs("Id String is malformed\n", stderr);
                    return 1;
                case RunResult::Error::IdNotFound:
                    fputs("Id String not found\n", stderr);
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
                    fprintf(stderr,
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
                    fputs("Provided section was not found\n"
                          "This may be because found sections were in a "
                          "protected segment\n",
                          stderr);
                    return 1;
                case RunResult::Error::NotCStringSection:
                    fputs("Provided section is not a c-string section\n",
                          stderr);
                    return 1;
                case RunResult::Error::HasNoStrings:
                    fputs("Provided section has no (printable) c-strings\n",
                          stderr);
                    return 1;
                case RunResult::Error::ProtectedSegment:
                    fputs("Provided section is in a protected segment\n",
                          stderr);
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
                    fputs("Provided section was not found\n"
                          "This may be because found sections were in a "
                          "protected segment\n",
                          stderr);
                    return 1;
                case RunResult::Error::NotSymbolPointerSection:
                    fputs("Provided section is not a symbol pointer section\n",
                          stderr);
                    return 1;
                case RunResult::Error::ProtectedSegment:
                    fputs("Provided section is in a protected segment\n",
                          stderr);
                    return 1;
                case RunResult::Error::InvalidSectionRange:
                    fputs("Provided section has an invalid file-range\n",
                          stderr);
                    return 1;
                case RunResult::Error::SymTabNotFound:
                    fputs("Couldn't find symtab_command\n", stderr);
                    return 1;
                case RunResult::Error::DynamicSymTabNotFound:
                    fputs("Couldn't find dysymtab_command\n", stderr);
                    return 1;
                case RunResult::Error::MultipleSymTabCommands:
                    fputs("Found multiple symtab_commands\n", stderr);
                    return 1;
                case RunResult::Error::MultipleDynamicSymTabCommands:
                    fputs("Found multiple dysymtab_commands\n", stderr);
                    return 1;
                case RunResult::Error::IndexListOutOfBounds:
                    fputs("Index-List is out-of-bounds of mach-o\n", stderr);
                    return 1;
                case RunResult::Error::IndexOutOfBounds:
                    fputs("Index is out-of-bounds of mach-o\n", stderr);
                    return 1;
                case RunResult::Error::SymbolTableOutOfBounds:
                    fputs("Symbol-Table is out-of-bounds of mach-o\n", stderr);
                    return 1;
                case RunResult::Error::StringTableOutOfBounds:
                    fputs("String-Table is out-of-bounds of mach-o\n", stderr);
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
                    fputs("Multiple different export-tries found\n", stderr);
                    return 1;
                case RunResult::Error::NoExportTrieFound:
                    fputs("Failed to find export-trie\n", stderr);
                    return 1;
                case RunResult::Error::ExportTrieOutOfBounds:
                    fputs("Export-trie is out-of-bounds\n", stderr);
                    return 1;
                case RunResult::Error::NoExports:
                    fputs("Export-trie has no exported symbols\n", stderr);
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
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case RunResult::Error::NoOpcodes:
                    fputs("No bind-opcodes found within table\n", stderr);
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
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case RunResult::Error::NoActions:
                    fputs("No bind-actions found within table\n", stderr);
                    return 1;
                case RunResult::Error::BindOpcodeParseError: {
                    const auto &ParseResult =
                        Result.PrintBindActionListResult.BindOpcodeParseResult;

                    fputs("Error parsing bind-opcode:\n", stderr);
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
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case RunResult::Error::NoSymbols:
                    fputs("No bind-symbols found within table\n", stderr);
                    return 1;
                case RunResult::Error::BindOpcodeParseError: {
                    const auto &ParseResult =
                        Result.PrintBindSymbolListResult.BindOpcodeParseResult;

                    fputs("Error parsing bind-opcode:\n", stderr);
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
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case RunResult::Error::NoOpcodes:
                    fputs("No rebase-opcodes found within table\n", stderr);
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
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case RunResult::Error::NoActions:
                    fputs("No rebase-actions found within table\n", stderr);
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
                    fputs("No dyld-info load command was found\n", stderr);
                    return 1;
                case RunResult::Error::NoObjcData:
                    fputs("No objc class-list data was found\n", stderr);
                    return 1;
                case RunResult::Error::UnalignedSection:
                    fputs("Objc class-list section is mis-aligned\n", stderr);
                    return 1;
                case RunResult::Error::ObjcDataOutOfBounds:
                    fputs("Objc class-list data is out-of-bounds of file\n",
                          stderr);
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
                    fputs("File has no program-trie\n", stderr);
                    return 1;
                case RunResult::Error::OutOfBounds:
                    fputs("Program-trie is out-of-bounds of file\n", stderr);
                    return 1;
                case RunResult::Error::NoExports:
                    fputs("Program-trie has no exported nodes\n", stderr);
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
                    fputs("File has no images\n", stderr);
                    return 1;
            }

            break;
        }
    }

    return 0;
}
