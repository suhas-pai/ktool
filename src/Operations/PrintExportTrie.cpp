/*
 * Operations/PrintExportTrie.cpp
 * © suhas pai
 */

#include <algorithm>
#include "ADT/Maximizer.h"

#include "MachO/ExportTrie.h"
#include "MachO/LibraryList.h"

#include "Operations/PrintExportTrie.h"
#include "Utils/Print.h"

namespace Operations {
    PrintExportTrie::PrintExportTrie(FILE *const OutFile,
                                     const struct Options &Options)
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintExportTrie::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintExportTrie::supportsObjectKind()");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintExportTrie::supportsObjectKind()");
    }

    static auto
    ExportMeetsRequirements(
        const MachO::ExportTrieExportKind Kind,
        std::string_view SegmentName,
        std::string_view SectionName,
        const struct PrintExportTrie::Options &Options) noexcept
    {
        if (!Options.KindRequirements.empty()) {
            if (!Options.KindRequirements.contains(Kind)) {
                return false;
            }

            // Re-exports, if they're explicitly allowed, automatically pass all
            // section requirements.

            if (Kind == MachO::ExportTrieExportKind::Reexport) {
                return true;
            }
        }

        if (!Options.SectionRequirements.empty()) {
            if (Kind == MachO::ExportTrieExportKind::Reexport) {
                return false;
            }

            for (const auto &Req : Options.SectionRequirements) {
                if (Req.SegmentName.has_value()) {
                    if (SegmentName != Req.SegmentName.value()) {
                        continue;
                    }
                }

                if (Req.SectionName.has_value()) {
                    if (SectionName == Req.SectionName.value()) {
                        return true;
                    }
                } else {
                    return true;
                }
            }

            return false;
        }

        return true;
    }

    struct ExportInfo {
        MachO::ExportTrieExportKind Kind;
        MachO::ExportTrieExportInfo Info;

        std::string_view SegmentName;
        std::string_view SectionName;

        std::string String;
    };

    static void
    PrintDylibOrdinalInfo(FILE *const OutFile,
                          const uint32_t DylibOrdinal,
                          const MachO::LibraryList &LibraryList) noexcept
    {
        auto LibraryPath = std::string_view("<Invalid>");
        auto IsOutOfBounds = true;

        if (DylibOrdinal != 0 && DylibOrdinal <= LibraryList.size()) {
            const auto &PathOpt = LibraryList.at(DylibOrdinal - 1).Path;
            if (PathOpt.has_value()) {
                LibraryPath = PathOpt.value();
            } else {
                LibraryPath = "<Malformed>";
            }

            IsOutOfBounds = false;
        }

        Utils::PrintDylibOrdinalInfo(OutFile,
                                     DylibOrdinal,
                                     LibraryPath,
                                     /*PrintPath=*/true,
                                     IsOutOfBounds);
        fputc(')', OutFile);
    }

    constexpr static inline auto TabLength = 8;

    [[nodiscard]] static auto
    GetSymbolLengthForLongestPrintedLineAndCount(
        const MachO::ExportTrieEntryCollection &Collection,
        uint64_t &Count) noexcept
    {
        auto LongestLength = ADT::Maximizer<uint64_t>();

        const auto End = Collection.end();
        for (auto Iter = Collection.begin(); Iter != End; Iter++, Count++) {
            if (!Iter->isExport()) {
                continue;
            }

            const auto Length =
                Iter.printLineLength(TabLength) + Iter->string().length();

            LongestLength.set(Length);
        }

        return LongestLength.value();
    }

    static void
    PrintTreeExportInfo(
        FILE *const OutFile,
        const MachO::ExportTrieExportChildNode &Export,
        const MachO::LibraryList &LibraryList,
        const int WrittenOut,
        const uint64_t LongestLength,
        const bool Is64Bit,
        const struct PrintExportTrie::Options &Options) noexcept
    {
        const auto RightPad =
            static_cast<int>(LongestLength + STR_LENGTH("\"\" -"));
        const auto KindDesc =
            ExportTrieExportKindIsValid(Export.kind()) ?
                ExportTrieExportKindGetDesc(Export.kind()) :
                "<unrecognized>";

        fputc(' ', OutFile);

        const auto PadLength = RightPad - WrittenOut - 1;
        Utils::PrintMultTimes(OutFile,
                              "-",
                              static_cast<uint64_t>(PadLength));

        fputs("> (Exported - ", OutFile);
        if (Options.Verbose) {
            fprintf(OutFile,
                    "%" RIGHTPAD_FMT "s",
                    RIGHTPAD_FMT_ARGS(
                        static_cast<int>(STR_LENGTH("Re-Export"))),
                    KindDesc.data());

            if (!Export.isReexport()) {
                Utils::PrintSegmentSectionPair(OutFile,
                                               Export.segment()->Name,
                                               Export.section()->Name,
                                               true,
                                               " - ",
                                               " - ");

                const auto ImageOffset = Export.info().imageOffset();
                Utils::PrintAddress(OutFile, ImageOffset, Is64Bit);
            } else {
                fputs(" - ", OutFile);
                if (!Export.info().reexportImportName().empty()) {
                    fprintf(OutFile,
                            "As \"%s\" - ",
                            Export.info().reexportImportName().data());
                }

                const auto DylibOrdinal = Export.info().reexportDylibOrdinal();
                PrintDylibOrdinalInfo(OutFile, DylibOrdinal, LibraryList);
            }
        } else {
            fprintf(OutFile, "%s", KindDesc.data());
        }

        fputc(')', OutFile);
    }

    static int
    HandleTreeOption(
        FILE *const OutFile,
        MachO::ExportTrieEntryCollection &EntryCollection,
        const MachO::LibraryList &LibraryList,
        const bool Is64Bit,
        const struct PrintExportTrie::Options &Options) noexcept
    {
        if (EntryCollection.empty()) {
            fputs("Provided file has an empty export-trie\n", OutFile);
            return 1;
        }

        if (!Options.SectionRequirements.empty()) {
            auto CollectionEnd = EntryCollection.end();
            for (auto Iter = EntryCollection.begin(); Iter != CollectionEnd;) {
                const auto ExportNode = Iter->getIfExportNode();
                if (ExportNode == nullptr) {
                    Iter++;
                    continue;
                }

                const auto Kind = ExportNode->kind();

                auto Segment = std::string_view();
                auto Section = std::string_view();

                if (Kind != MachO::ExportTrieExportKind::Reexport) {
                    if (const auto ExportSegment = ExportNode->segment()) {
                        Segment = ExportSegment->Name;
                    }

                    if (const auto ExportSection = ExportNode->section()) {
                        Section = ExportSection->Name;
                    }
                }

                if (ExportMeetsRequirements(Kind, Segment, Section, Options)) {
                    Iter++;
                    continue;
                }

                Iter = EntryCollection.RemoveNode(*Iter.node(), true);
            }

            if (EntryCollection.empty()) {
                fputs("Provided file has no export-trie after filtering with "
                      "provided requirements\n",
                      OutFile);
                return 1;
            }
        }

        auto Count = uint64_t();
        const auto LongestLength =
            GetSymbolLengthForLongestPrintedLineAndCount(EntryCollection,
                                                         Count);

        if (Options.OnlyCount) {
            fprintf(OutFile,
                    "Provided file's export-trie has %" PRIu64 " nodes\n",
                    Count);
            return 0;
        }

        if (Options.Sort) {
            EntryCollection.sort([](const auto &Lhs, const auto &Rhs) noexcept {
                const auto &Left =
                    reinterpret_cast<const MachO::ExportTrieChildNode &>(Lhs);
                const auto &Right =
                    reinterpret_cast<const MachO::ExportTrieChildNode &>(Rhs);

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
                reinterpret_cast<const MachO::ExportTrieChildNode &>(Node);

            WrittenOut += fprintf(OutFile, "\"%s\"", Info.string().data());
            if (const auto ExportInfo = Info.getIfExportNode()) {
                PrintTreeExportInfo(OutFile,
                                    *ExportInfo,
                                    LibraryList,
                                    WrittenOut,
                                    LongestLength,
                                    Is64Bit,
                                    Options);
            }

            return true;
        };

        EntryCollection.PrintHorizontal(OutFile, TabLength, PrintNode);
        return 0;
    }

    static auto
    CollectLoadCommands(const Objects::MachO &MachO,
                        MachO::LibraryList &LibraryList,
                        MachO::SegmentList &SegmentList,
                        std::optional<ADT::Range> &ExportTrieRangeOpt,
                        RunResult &Result,
                        const bool Is64Bit,
                        const bool IsBigEndian) noexcept
    {
        using RunError = PrintExportTrie::RunError;
        for (const auto &LC : MachO.loadCommandsMap()) {
            using Kind = MachO::LoadCommandKind;
            switch (LC.kind(IsBigEndian)) {
                case Kind::Segment:
                    if (Is64Bit) {
                        continue;
                    }

                    SegmentList.addSegment(cast<Kind::Segment>(LC, IsBigEndian),
                                           IsBigEndian);
                    break;
                case Kind::Segment64:
                    if (!Is64Bit) {
                        continue;
                    }

                    SegmentList.addSegment(
                        cast<Kind::Segment64>(LC, IsBigEndian),
                        IsBigEndian);
                    break;
                case Kind::DyldInfo:
                case Kind::DyldInfoOnly: {
                    const auto &DyldInfo =
                        cast<MachO::DyldInfoCommand>(LC, IsBigEndian);
                    const auto DyldInfoTrieRange =
                        DyldInfo.exportTrieRange(IsBigEndian);

                    if (ExportTrieRangeOpt.has_value()) {
                        if (DyldInfoTrieRange != ExportTrieRangeOpt.value()) {
                            return Result.set(RunError::MultipleExportTries);
                        }

                        continue;
                    }

                    ExportTrieRangeOpt = DyldInfoTrieRange;
                    break;
                }
                case Kind::DyldExportsTrie: {
                    const auto &ET =
                        cast<Kind::DyldExportsTrie>(LC, IsBigEndian);

                    const auto TrieRange = ET.dataRange(IsBigEndian);
                    if (ExportTrieRangeOpt.has_value()) {
                        if (TrieRange != ExportTrieRangeOpt.value()) {
                            return Result.set(RunError::MultipleExportTries);
                        }

                        continue;
                    }

                    break;
                }
                case MachO::LoadCommandKind::LoadDylib:
                case MachO::LoadCommandKind::LoadWeakDylib:
                case MachO::LoadCommandKind::ReexportDylib:
                case MachO::LoadCommandKind::LazyLoadDylib:
                case MachO::LoadCommandKind::LoadUpwardDylib:
                    LibraryList.addLibrary(
                        cast<MachO::DylibCommand>(LC, IsBigEndian),
                        IsBigEndian);
                    break;
                case MachO::LoadCommandKind::SymbolTable:
                case MachO::LoadCommandKind::SymbolSegment:
                case MachO::LoadCommandKind::Thread:
                case MachO::LoadCommandKind::UnixThread:
                case MachO::LoadCommandKind::LoadFixedVMSharedLib:
                case MachO::LoadCommandKind::IdFixedVMSharedLib:
                case MachO::LoadCommandKind::Identity:
                case MachO::LoadCommandKind::FixedVMFile:
                case MachO::LoadCommandKind::PrePage:
                case MachO::LoadCommandKind::DynamicSymbolTable:
                case MachO::LoadCommandKind::IdDylib:
                case MachO::LoadCommandKind::LoadDylinker:
                case MachO::LoadCommandKind::IdDylinker:
                case MachO::LoadCommandKind::PreBoundDylib:
                case MachO::LoadCommandKind::Routines:
                case MachO::LoadCommandKind::SubFramework:
                case MachO::LoadCommandKind::SubUmbrella:
                case MachO::LoadCommandKind::SubClient:
                case MachO::LoadCommandKind::SubLibrary:
                case MachO::LoadCommandKind::TwoLevelHints:
                case MachO::LoadCommandKind::PreBindChecksum:
                case MachO::LoadCommandKind::Routines64:
                case MachO::LoadCommandKind::Uuid:
                case MachO::LoadCommandKind::Rpath:
                case MachO::LoadCommandKind::CodeSignature:
                case MachO::LoadCommandKind::SegmentSplitInfo:
                case MachO::LoadCommandKind::EncryptionInfo:
                case MachO::LoadCommandKind::VersionMinMacOS:
                case MachO::LoadCommandKind::VersionMinIOS:
                case MachO::LoadCommandKind::FunctionStarts:
                case MachO::LoadCommandKind::DyldEnvironment:
                case MachO::LoadCommandKind::Main:
                case MachO::LoadCommandKind::DataInCode:
                case MachO::LoadCommandKind::SourceVersion:
                case MachO::LoadCommandKind::DylibCodeSignDRS:
                case MachO::LoadCommandKind::EncryptionInfo64:
                case MachO::LoadCommandKind::LinkerOption:
                case MachO::LoadCommandKind::LinkerOptimizationHint:
                case MachO::LoadCommandKind::VersionMinTVOS:
                case MachO::LoadCommandKind::VersionMinWatchOS:
                case MachO::LoadCommandKind::Note:
                case MachO::LoadCommandKind::BuildVersion:
                case MachO::LoadCommandKind::DyldChainedFixups:
                case MachO::LoadCommandKind::FileSetEntry:
                    break;
            }
        }

        return Result.set(RunError::None);
    }

    auto PrintExportTrie::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        auto Result = RunResult(Objects::Kind::MachO);
        auto ExportTrieRangeOpt = std::optional<ADT::Range>(std::nullopt);

        auto LibraryList = MachO::LibraryList();
        auto SegmentList = MachO::SegmentList();

        CollectLoadCommands(MachO,
                            LibraryList,
                            SegmentList,
                            ExportTrieRangeOpt,
                            Result,
                            Is64Bit,
                            IsBigEndian);

        if (Result.get<RunError>() != RunError::None) {
            return Result;
        }

        if (!ExportTrieRangeOpt.has_value()) {
            return Result.set(RunError::NoExportTrieFound);
        }

        const auto &ExportTrieRange = ExportTrieRangeOpt.value();
        if (!MachO.map().range().contains(ExportTrieRange)) {
            return Result.set(RunError::ExportTrieOutOfBounds);
        }

        const auto ExportTrieMap =
            MachO::ExportTrieMap(ADT::MemoryMap(MachO.map(), ExportTrieRange));

        if (Opt.PrintTree) {
            auto Error = MachO::ExportTrieParseError::None;
            auto EntryCollection =
                MachO::ExportTrieEntryCollection::Open(ExportTrieMap,
                                                       &SegmentList,
                                                       &Error);
            HandleTreeOption(OutFile,
                             EntryCollection,
                             LibraryList,
                             MachO.is64Bit(),
                             Opt);
        } else {
            auto Count = uint64_t();
            auto ExportList = std::vector<ExportInfo>();
            auto LongestExportLength = ADT::Maximizer<uint64_t>();

            if (Opt.OnlyCount && Opt.SectionRequirements.empty()) {
                for ([[maybe_unused]] const auto &Info :
                        ExportTrieMap.exportMap())
                {
                    Count++;
                    continue;
                }
            } else {
                for (const auto &Info : ExportTrieMap.exportMap()) {
                    LongestExportLength.set(Info.string().length());

                    auto SegmentName = std::string_view();
                    auto SectionName = std::string_view();

                    if (!Info.isReexport()) {
                        const auto Addr = Info.exportInfo().imageOffset();
                        if (const auto Segment =
                                SegmentList.findSegmentWithVmAddr(Addr))
                        {
                            if (const auto Section =
                                    Segment->findSectionWithVmAddr(Addr))
                            {
                                SectionName = Section->Name;
                            }

                            SegmentName = Segment->Name;
                        }
                    }

                    const auto &Kind = Info.kind();
                    if (!ExportMeetsRequirements(Kind,
                                                 SegmentName,
                                                 SectionName,
                                                 Opt))
                    {
                        continue;
                    }

                    if (Opt.OnlyCount) {
                        Count++;
                        continue;
                    }

                    ExportList.emplace_back(ExportInfo {
                        .Kind = Kind,
                        .Info = Info.exportInfo(),
                        .SegmentName = SegmentName,
                        .SectionName = SectionName,
                        .String = std::string(Info.string())
                    });
                }
            }

            if (ExportList.empty()) {
                return Result.set(RunError::NoExports);
            }

            if (Opt.OnlyCount) {
                fprintf(OutFile,
                        "Provided file's export-trie has %" PRIu64 " nodes\n",
                        Count);
                return Result.set(RunError::None);
            }

            if (Opt.Sort) {
                const auto Comparator =
                    [](const ExportInfo &Lhs, const ExportInfo &Rhs) noexcept
                {
                    return Lhs.String < Rhs.String;
                };

                std::sort(ExportList.begin(), ExportList.end(), Comparator);
            }

            auto Counter = uint32_t();
            const auto SizeDigitLength =
                Utils::GetIntegerDigitCount(ExportList.size());

            constexpr auto LongestDescLength =
                MachO::ExportTrieExportKindGetDesc(
                    MachO::ExportTrieExportKind::StubAndResolver).length();

            for (const auto &Export : ExportList) {
                const auto RightPadAmt =
                    static_cast<int>(STR_LENGTH("Export : ") + SizeDigitLength);

                Utils::RightPadSpaces(OutFile,
                                      fprintf(OutFile,
                                              "Export %0*" PRIu32 ": ",
                                              SizeDigitLength,
                                              Counter),
                                      RightPadAmt);

                if (!Export.Info.isReexport()) {
                    Utils::PrintSegmentSectionPair(OutFile,
                                                   Export.SegmentName.data(),
                                                   Export.SectionName.data(),
                                                   true,
                                                   false);

                    const auto ImageOffset = Export.Info.imageOffset();
                    Utils::PrintAddress(OutFile, Is64Bit, ImageOffset);
                } else {
                    const auto OffsetLength =
                        Is64Bit ?
                            Utils::Address64Length : Utils::Address32Length;
                    const auto PadLength =
                        Utils::SegmentSectionPairMaxLen + OffsetLength;

                    Utils::PadSpaces(OutFile, static_cast<int>(PadLength));
                }

                const auto KindDesc =
                    MachO::ExportTrieExportKindGetDesc(Export.Kind);

                fprintf(OutFile,
                        "\t%" RIGHTPAD_FMT "s",
                        RIGHTPAD_FMT_ARGS(static_cast<int>(LongestDescLength)),
                        KindDesc.data());

                const auto RightPad =
                    static_cast<int>(LongestExportLength.value() +
                                     STR_LENGTH("\"\""));

                Utils::RightPadSpaces(OutFile,
                                      fprintf(OutFile,
                                              "\"%s\"",
                                              Export.String.data()),
                                      RightPad);

                if (Export.Info.isReexport()) {
                    const auto ImportName = Export.Info.reexportImportName();
                    const auto DylibOrdinal =
                        Export.Info.reexportDylibOrdinal();

                    if (!ImportName.empty()) {
                        fprintf(OutFile,
                                " (Re-exported as %s, ",
                                ImportName.data());
                    } else {
                        fputs(" (Re-exported from ", OutFile);
                    }

                    PrintDylibOrdinalInfo(OutFile, DylibOrdinal, LibraryList);
                    fputc(')', OutFile);
                }

                fputc('\n', OutFile);
                Counter++;
            }
        }

        return Result.set(RunError::None);
    }

    auto PrintExportTrie::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintExportTrie::run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintExportTrie::run()");
    }
}