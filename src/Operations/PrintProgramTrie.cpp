/*
 * Operations/PrintProgramTrie.cpp
 * © suhas pai
 */

#include <algorithm>

#include "ADT/Maximizer.h"
#include "Operations/PrintProgramTrie.h"
#include "DyldSharedCache/ProgramTrie.h"

#include "Utils/Print.h"

namespace Operations {
    PrintProgramTrie::PrintProgramTrie(FILE *const OutFile,
                     const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintProgramTrie::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in PrintId::supportsObjectKind()");
            case Objects::Kind::DyldSharedCache:
                return true;
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintId::supportsObjectKind()");
    }

    static void
    PrintTreeExportInfo(
        FILE *const OutFile,
        const ::DyldSharedCache::ProgramTrieExportChildNode &Export,
        const int WrittenOut,
        const uint64_t LongestLength) noexcept
    {
        const auto RightPad =
            static_cast<int>(LongestLength + STR_LENGTH("\"\" -"));

        fputc(' ', OutFile);

        const auto PadLength = RightPad - WrittenOut - 1;
        Utils::PrintMultTimes(OutFile,
                              "-",
                              static_cast<uint64_t>(PadLength));

        Utils::PrintAddress(OutFile,
                            Export.index(),
                            /*Is64Bit=*/false,
                            /*Prefix=*/"> (Exported - Index = ",
                            /*Suffix=*/")");
    }

    [[nodiscard]] static auto
    GetSymbolLengthForLongestPrintedLineAndCount(
        const ::DyldSharedCache::ProgramTrieEntryCollection &Collection,
        uint64_t &Count,
        const PrintProgramTrie::Options &Opt) noexcept
    {
        auto LongestLength = ADT::Maximizer<uint64_t>();

        const auto End = Collection.end();
        for (auto Iter = Collection.begin(); Iter != End; Iter++, Count++) {
            if (!Iter->isExport()) {
                continue;
            }

            const auto Length =
                Iter.printLineLength(Opt.TabLength) + Iter->string().length();

            LongestLength.set(Length);
        }

        return LongestLength.value();
    }

    static auto
    HandleTreeOption(
        RunResult &Result,
        FILE *const OutFile,
        ::DyldSharedCache::ProgramTrieEntryCollection &EntryCollection,
        const struct PrintProgramTrie::Options &Options) noexcept
            -> RunResult &
    {
        using RunError = PrintProgramTrie::RunError;
        if (EntryCollection.empty()) {
            fputs("Provided file has an empty export-trie\n", OutFile);
            return Result.set(RunError::None);
        }

        auto Count = uint64_t();
        const auto LongestLength =
            GetSymbolLengthForLongestPrintedLineAndCount(EntryCollection,
                                                         Count,
                                                         Options);

        if (Options.OnlyCount) {
            fprintf(OutFile,
                    "Provided file's program-trie has %" PRIu64 " nodes\n",
                    Count);
            return Result.set(RunError::None);
        }

        if (Options.Sort) {
            EntryCollection.sort([](const auto &Lhs, const auto &Rhs) noexcept {
                const auto &Left =
                    reinterpret_cast<
                        const ::DyldSharedCache::ProgramTrieChildNode &>(Lhs);
                const auto &Right =
                    reinterpret_cast<
                        const ::DyldSharedCache::ProgramTrieChildNode &>(Rhs);

                return Left.string() <= Right.string();
            });
        }

        const auto PrintNode =
            [&](FILE *const OutFile,
                int WrittenOut,
                [[maybe_unused]] const uint64_t DepthLevel,
                const ADT::TreeNode &Node) noexcept
        {
            const auto &Info =
                reinterpret_cast<
                    const ::DyldSharedCache::ProgramTrieChildNode &>(Node);

            WrittenOut += fprintf(OutFile, "\"%s\"", Info.string().data());
            if (const auto ExportInfo = Info.getIfExportNode()) {
                PrintTreeExportInfo(OutFile,
                                    *ExportInfo,
                                    WrittenOut,
                                    LongestLength);
            }

            return true;
        };

        EntryCollection.printHorizontal(OutFile, Options.TabLength, PrintNode);
        return Result.set(RunError::None);
    }

    struct SExportInfo {
        std::string String;
        uint32_t Index;
    };

    static auto
    PrintExportList(RunResult &Result,
                    FILE *const OutFile,
                    const ::DyldSharedCache::ProgramTrieMap &ProgramTrieMap,
                    const PrintProgramTrie::Options &Opt) noexcept
        -> RunResult &
    {
        using RunError = PrintProgramTrie::RunError;

        auto Count = uint64_t();
        auto ExportList = std::vector<SExportInfo>();

        auto LongestExportLength = ADT::Maximizer<uint64_t>();
        auto IndexDigitCountMaximizer = ADT::Maximizer<uint32_t>();

        if (Opt.OnlyCount) {
            for ([[maybe_unused]] const auto &Info : ProgramTrieMap.exportMap())
            {
                Count++;
                continue;
            }
        } else {
            for (const auto &Info : ProgramTrieMap.exportMap()) {
                LongestExportLength.set(Info.string().length());
                IndexDigitCountMaximizer.set(
                    Utils::GetIntegerDigitCount(Info.exportInfo().index()));

                ExportList.push_back(SExportInfo {
                    .String = std::string(Info.string()),
                    .Index = Info.exportInfo().index(),
                });
            }
        }

        if (ExportList.empty()) {
            return Result.set(RunError::NoExports);
        }

        if (Opt.OnlyCount) {
            fprintf(OutFile,
                    "Provided file's program-trie has %" PRIu64 " nodes\n",
                    Count);
            return Result.set(RunError::None);
        }

        if (Opt.Sort) {
            const auto Comparator =
                [](const SExportInfo &Lhs, const SExportInfo &Rhs) noexcept
            {
                return Lhs.String < Rhs.String;
            };

            std::sort(ExportList.begin(), ExportList.end(), Comparator);
        }

        auto Counter = uint32_t(1);
        const auto SizeDigitLength =
            Utils::GetIntegerDigitCount(ExportList.size());

        for (const auto &Export : ExportList) {
            const auto RightPadAmt =
                static_cast<int>(STR_LENGTH("Program : ") + SizeDigitLength);

            Utils::RightPadSpaces(OutFile,
                                  fprintf(OutFile,
                                          "Program %" ZEROPAD_FMT PRIu32 ": ",
                                          ZEROPAD_FMT_ARGS(SizeDigitLength),
                                          Counter),
                                  RightPadAmt);

            fprintf(OutFile,
                    "\t" ADDRESS_32_FMT "\t%s\"\n",
                    Export.Index,
                    Export.String.c_str());

            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto
    PrintProgramTrie::run(const Objects::DyldSharedCache &Dsc) const noexcept
        -> RunResult
    {
        auto Result = RunResult(Objects::Kind::DyldSharedCache);
        if (!Dsc.isV8()) {
            return Result.set(RunError::NoProgramTrie);
        }

        const auto Header = Dsc.headerV8();
        if (Header.ProgramTrieAddr == 0 || Header.ProgramTrieSize == 0) {
            return Result.set(RunError::NoProgramTrie);
        }

        const auto ProgramTrieMemMapOpt =
            Dsc.getMapForAddrRange(Header.programTrieRange());

        if (!ProgramTrieMemMapOpt.has_value()) {
            return Result.set(RunError::OutOfBounds);
        }

        const auto ProgramTriePair = ProgramTrieMemMapOpt.value();

        auto TrieParser = ADT::TrieParser();
        auto ProgramTrieMap =
            ::DyldSharedCache::ProgramTrieMap(ProgramTriePair.second,
                                              TrieParser);

        if (Opt.PrintTree) {
            auto Error = ::DyldSharedCache::ProgramTrieMap::ParseError::None;
            auto Options =
                ::DyldSharedCache::ProgramTrieEntryCollection::ParseOptions();

            auto EntryCollection =
                ::DyldSharedCache::ProgramTrieEntryCollection::Open(
                    ProgramTrieMap,
                    Options,
                    &Error);

            switch (Error) {
                case ADT::TrieParseError::None:
                    break;
                case ADT::TrieParseError::InvalidUleb128:
                    fputs("Encountered an invalid uleb128 while parsing trie\n",
                          stderr);
                    return Result.set(RunError::None);
                case ADT::TrieParseError::InvalidFormat:
                    fputs("Trie is invalid\n",stderr);
                    return Result.set(RunError::None);
                case ADT::TrieParseError::OverlappingRanges:
                    fputs("At least two nodes in trie are overlapping\n",
                          stderr);
                    return Result.set(RunError::None);
                case ADT::TrieParseError::TooDeep:
                    fputs("Trie is too deep\n", stderr);
                    return Result.set(RunError::None);
            }

            return HandleTreeOption(Result,
                                    OutFile,
                                    EntryCollection,
                                    Opt);
        }

        return PrintExportList(Result, OutFile, ProgramTrieMap, Opt);
    }

    auto PrintProgramTrie::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintProgramTrie::run() got Object with Kind::None");
            case Objects::Kind::DyldSharedCache:
                return run(static_cast<const Objects::DyldSharedCache &>(Base));
            case Objects::Kind::DscImage:
            case Objects::Kind::MachO:
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintExportTrie::run()");
    }
}