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
    : Base(Operations::Kind::PrintProgramTrie), OutFile(OutFile),
      Opt(Options) {}

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

        std::print(OutFile, " ");

        const auto PadLength = RightPad - WrittenOut - 1;
        Utils::PrintMultTimes(OutFile,
                              "-",
                              static_cast<uint64_t>(PadLength));

        std::print(OutFile,
                   "> (Exported - Index: {})",
                   Utils::FormattedNumber(Export.index()),
                   /*Is64Bit=*/false);
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
        FILE *const OutFile,
        ::DyldSharedCache::ProgramTrieEntryCollection &EntryCollection,
        const struct PrintProgramTrie::Options &Options) noexcept
            -> PrintProgramTrie::RunResult
    {
        using RunResult = PrintProgramTrie::RunResult;
        if (EntryCollection.empty()) {
            std::println(OutFile, "Provided file has an empty export-trie");
            return RunResult(RunResult::Error::None);
        }

        auto Count = uint64_t();
        const auto LongestLength =
            GetSymbolLengthForLongestPrintedLineAndCount(EntryCollection,
                                                         Count,
                                                         Options);

        if (Options.OnlyCount) {
            std::println(OutFile,
                         "Provided file's program-trie has {} nodes",
                         Count);
            return RunResult(RunResult::Error::None);
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

            std::print(OutFile, "\"{}\"", Info.string());
            WrittenOut += STR_LENGTH("\"\"") + Info.string().length();

            if (const auto ExportInfo = Info.getIfExportNode()) {
                PrintTreeExportInfo(OutFile,
                                    *ExportInfo,
                                    WrittenOut,
                                    LongestLength);
            }

            return true;
        };

        EntryCollection.printHorizontal(OutFile, Options.TabLength, PrintNode);
        return RunResult(RunResult::Error::None);
    }

    struct SExportInfo {
        std::string String;
        uint32_t Index;
    };

    static auto
    PrintExportList(FILE *const OutFile,
                    const ::DyldSharedCache::ProgramTrieMap &ProgramTrieMap,
                    const PrintProgramTrie::Options &Opt) noexcept
        -> PrintProgramTrie::RunResult
    {
        using RunResult = PrintProgramTrie::RunResult;

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

                ExportList.emplace_back(SExportInfo {
                    .String = std::string(Info.string()),
                    .Index = Info.exportInfo().index(),
                });
            }
        }

        if (ExportList.empty()) {
            return RunResult(RunResult::Error::NoExports);
        }

        if (Opt.OnlyCount) {
            std::println(OutFile,
                         "Provided file's program-trie has {} nodes",
                         Count);
            return RunResult(RunResult::Error::None);
        }

        if (Opt.Sort) {
            const auto Comparator =
                [](const SExportInfo &Lhs, const SExportInfo &Rhs) noexcept
            {
                return Lhs.String < Rhs.String;
            };

            std::sort(ExportList.begin(), ExportList.end(), Comparator);
        }

        auto Counter = static_cast<uint32_t>(1);
        const auto SizeDigitLength =
            Utils::GetIntegerDigitCount(ExportList.size());

        for (const auto &Export : ExportList) {
            const auto RightPadAmt =
                STR_LENGTH("Program : ") + SizeDigitLength;

            std::println(OutFile,
                         "{:<{}}\t{}\t{}\"",
                         std::format("Program {:>{}}: ",
                                     Counter,
                                     SizeDigitLength),
                         RightPadAmt,
                         Utils::Address(Export.Index),
                         Export.String);

            Counter++;
        }

        return RunResult(RunResult::Error::None);
    }

    auto
    PrintProgramTrie::run(const Objects::DyldSharedCache &Dsc) const noexcept
        -> RunResult
    {
        if (!Dsc.isAtleastV8()) {
            return RunResult(RunResult::Error::NoProgramTrie);
        }

        const auto Header = Dsc.headerV8();
        if (Header.ProgramTrieAddr == 0 || Header.ProgramTrieSize == 0) {
            return RunResult(RunResult::Error::NoProgramTrie);
        }

        const auto ProgramTrieMemMapOpt =
            Dsc.getMapForAddrRange(Header.programTrieRange());

        if (!ProgramTrieMemMapOpt.has_value()) {
            return RunResult(RunResult::Error::OutOfBounds);
        }

        const auto &[CacheInfo, ProgramTrie] = ProgramTrieMemMapOpt.value();

        auto TrieParser = ADT::TrieParser();
        auto ProgramTrieMap =
            ::DyldSharedCache::ProgramTrieMap(ProgramTrie, TrieParser);

        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

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
                    return RunResult(RunResult::Error::InvalidTrieUleb128);
                case ADT::TrieParseError::InvalidFormat:
                    return RunResult(RunResult::Error::InvalidTrieFormat);
                case ADT::TrieParseError::OverlappingRanges:
                    return RunResult(RunResult::Error::OverlappingTrieRanges);
                case ADT::TrieParseError::TooDeep:
                    return RunResult(RunResult::Error::TrieIsTooDeep);
            }

            return HandleTreeOption(OutFile, EntryCollection, Opt);
        }

        return PrintExportList(OutFile, ProgramTrieMap, Opt);
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
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintExportTrie::run()");
    }
}