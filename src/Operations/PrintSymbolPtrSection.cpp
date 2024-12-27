//
//  Operations/PrintSymbolPtrSection.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <algorithm>
#include <compare>

#include "ADT/Maximizer.h"
#include "Operations/PrintSymbolPtrSection.h"
#include "Utils/Print.h"

namespace Operations {
    PrintSymbolPtrSection::PrintSymbolPtrSection(
        FILE *const OutFile,
        const std::optional<std::string> &SegmentName,
        const std::string_view SectionName,
        const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintSymbolPtrSection), OutFile(OutFile),
      Opt(Options), SegmentName(SegmentName), SectionName(SectionName) {}

    bool
    PrintSymbolPtrSection::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintSymbolPtrSection::supportsObjectKind()");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return true;
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintSymbolPtrSection::supportsObjectKind()");
    }

    template <MachO::LoadCommandDerived T>
    static auto
    IterateSections(const T &Segment,
                    const std::string_view SectionName,
                    const bool IsBigEndian,
                    std::vector<std::string_view> &SectionNameList,
                    const typename T::Section *&SectionOut) noexcept
        -> PrintSymbolPtrSection::RunResult
    {
        using RunResult = PrintSymbolPtrSection::RunResult;

        SectionNameList.reserve(Segment.sectionCount(IsBigEndian));
        for (const auto &Section : Segment.sectionList(IsBigEndian)) {
            if (Section.sectionName() != SectionName) {
                continue;
            }

            using Kind = typename T::Section::Kind;

            const auto SectionKind = Section.kind(IsBigEndian);
            if (SectionKind != Kind::LazySymbolPointers &&
                SectionKind != Kind::NonLazySymbolPointers)
            {
                return RunResult(RunResult::Error::NotSymbolPointerSection);
            }

            SectionOut = &Section;
            SectionNameList.emplace_back(Section.sectionName());
        }

        return RunResult();
    }

    struct DylibInfo {
        std::string Path;

        Dyld3::PackedVersion CurrentVersion;
        Dyld3::PackedVersion CompatVersion;

        uint32_t Timestamp;
    };

    struct SegmentInfo {
        std::string_view Name;
        std::vector<std::string_view> SectionNameList;
    };

    static auto
    GetSectionAtOrdinal(const std::vector<SegmentInfo> &SegmentList,
                        const uint32_t Ordinal,
                        std::string_view &SegmentName) noexcept
        -> std::optional<std::string_view>
    {
        assert(Ordinal != 0);
        auto Index = Ordinal - 1;

        for (const auto &Segment : SegmentList) {
            if (Utils::IndexOutOfBounds(Index, Segment.SectionNameList.size()))
            {
                Index -= Segment.SectionNameList.size();
                continue;
            }

            SegmentName = Segment.Name;
            return std::optional(Segment.SectionNameList.at(Index));
        }

        return std::nullopt;
    }

    struct SymbolInfo {
        std::string_view String;

        uint64_t Index;
        MachO::SymTabCommand::Entry::Kind Kind;
        uint8_t Section;
        uint8_t DylibOrdinal;

        bool PrivateExternal : 1;
        bool DebugSymbol : 1;
    };

    template <bool Is64Bit>
    auto
    IterateIndices(const MachO::SymTabCommand &SymTab,
                   const MachO::DynamicSymTabCommand &DynamicSymTab,
                   const ADT::MemoryMap &Map,
                   const uint32_t Reserved1,
                   const bool SkipInvalidIndices,
                   const uint64_t Limit,
                   const bool IsBigEndian,
                   std::vector<SymbolInfo> &SymbolInfoList,
                   uint32_t &LongestSymbolLengthOut,
                   uint64_t &LargestIndexOut,
                   uint8_t &LongestKindLengthOut) noexcept
        -> PrintSymbolPtrSection::RunResult
    {
        using RunResult = PrintSymbolPtrSection::RunResult;

        const auto IndirectSymbolOffset =
            DynamicSymTab.indirectSymbolsOffset(IsBigEndian);
        const auto IndirectSymbolCount =
            DynamicSymTab.indirectSymbolsCount(IsBigEndian);

        const auto IndexList =
            Map.get<uint32_t>(IndirectSymbolOffset, IndirectSymbolCount);

        if (IndexList == nullptr) {
            return RunResult(RunResult::Error::IndexListOutOfBounds);
        }

        if (Reserved1 >= IndirectSymbolCount) {
            return RunResult(RunResult::Error::IndexListOutOfBounds);
        }

        using namespace MachO;
        using SymTabEntry =
            std::conditional_t<Is64Bit,
                               SymTabCommand::Entry64,
                               SymTabCommand::Entry>;

        const auto SymbolCount = SymTab.symCount(IsBigEndian);
        const auto SymbolTableOpt =
            Map.getRange<SymTabEntry>(SymTab.symRange(IsBigEndian, Is64Bit));

        if (!SymbolTableOpt.has_value()) {
            return RunResult(RunResult::Error::SymbolTableOutOfBounds);
        }

        auto IndexMaximizer = ADT::Maximizer<uint64_t>();
        auto SymbolLengthMaximizer = ADT::Maximizer<uint32_t>();
        auto KindLengthMaximizer = ADT::Maximizer<uint8_t>();

        const auto StringTableOpt =
            Map.getRange<const char>(SymTab.strRange(IsBigEndian));

        if (!StringTableOpt.has_value()) {
            return RunResult(RunResult::Error::StringTableOutOfBounds);
        }

        const auto &SymbolTable = SymbolTableOpt.value();
        const auto &StringTable = StringTableOpt.value();

        auto SymbolsAdded = uint64_t();
        SymbolInfoList.reserve(IndirectSymbolCount - Reserved1);

        for (auto I = Reserved1;
             I != IndirectSymbolCount && SymbolsAdded < Limit;
             I++)
        {
            const auto Index = IndexList[I];
            if (Index == IndirectSymbolLocal || Index == IndirectSymbolAbs) {
                continue;
            }

            if (Utils::IndexOutOfBounds(Index, SymbolCount)) {
                if (!SkipInvalidIndices) {
                    return RunResult(RunResult::Error::IndexOutOfBounds);
                }

                continue;
            }

            const auto &Entry = SymbolTable[Index];
            const auto Info = SymbolInfo {
                .String = StringTable.data() + Entry.index(IsBigEndian),
                .Index = Index,
                .Kind = Entry.kind(),
                .Section = Entry.Section,
                .DylibOrdinal = Entry.dylibOrdinal(IsBigEndian),
                .PrivateExternal = Entry.privateExternal(),
                .DebugSymbol = Entry.debugSymbol()
            };

            IndexMaximizer.set(Info.Index);
            SymbolLengthMaximizer.set(
                static_cast<uint32_t>(Info.String.length()));

            if (MachO::SymTabCommand::Entry::KindIsValid(Info.Kind)) {
                const auto Desc =
                    MachO::SymTabCommand::Entry::KindGetDesc(Info.Kind);

                KindLengthMaximizer.set(Desc.length());
            }

            SymbolInfoList.emplace_back(std::move(Info));
            SymbolsAdded++;
        }

        LargestIndexOut = IndexMaximizer.value();
        LongestSymbolLengthOut = SymbolLengthMaximizer.value();
        LongestKindLengthOut = KindLengthMaximizer.value();

        return RunResult();
    }

    static auto
    CompareEntriesBySortKind(
        const SymbolInfo &Lhs,
        const SymbolInfo &Rhs,
        const std::vector<DylibInfo> &DylibInfoList,
        const PrintSymbolPtrSection::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            using SortKind = PrintSymbolPtrSection::Options::SortKind;
            case SortKind::ByDylibOrdinal:
                return Lhs.DylibOrdinal <=> Rhs.DylibOrdinal;
            case SortKind::ByDylibPath: {
                auto LhsDylibPath = std::string_view();
                auto RhsDylibPath = std::string_view();

                if (Lhs.DylibOrdinal >= DylibInfoList.size()) {
                    if (Rhs.DylibOrdinal >= DylibInfoList.size()) {
                        return std::strong_ordering::equivalent;
                    }

                    return std::strong_ordering::greater;
                } else if (Rhs.DylibOrdinal >= DylibInfoList.size()) {
                    return std::strong_ordering::less;
                }

                LhsDylibPath = DylibInfoList.at(Lhs.DylibOrdinal - 1).Path;
                RhsDylibPath = DylibInfoList.at(Rhs.DylibOrdinal - 1).Path;

                return LhsDylibPath <=> RhsDylibPath;
            }
            case SortKind::ByIndex:
                return Lhs.Index <=> Rhs.Index;
            case SortKind::ByString:
                return Lhs.String <=> Rhs.String;
        }

        assert(false && "CompareEntriesBySortKind() got unrecognized SortKind");
    }

    auto PrintSymbolPtrSection::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        using namespace MachO;
        if (this->SectionName.empty()) {
            return RunResult(RunResult::Error::EmptySectionName);
        }

        const auto &Opt = this->Opt;
        const auto OutFile = this->OutFile;

        auto SectionReserved1 = uint32_t();

        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();
        const auto Map = MachO.getMapForFileOffsets();

        constexpr auto Malformed = std::string_view("<malformed>");

        auto SymTabCmd = static_cast<const SymTabCommand *>(nullptr);
        auto DynamicSymTabCmd =
            static_cast<const DynamicSymTabCommand *>(nullptr);

        auto DylibList = std::vector<DylibInfo>();
        auto SegmentList = std::vector<SegmentInfo>();

        for (const auto &LC : MachO.loadCommandsMap()) {
            using Kind = LoadCommandKind;
            switch (LC.kind(IsBigEndian)) {
                case Kind::Segment: {
                    if (Is64Bit) {
                        break;
                    }

                    const auto Segment = cast<SegmentCommand>(&LC, IsBigEndian);
                    auto Section =
                        static_cast<const SegmentCommand::Section *>(nullptr);

                    auto SegmentInfo = ::Operations::SegmentInfo();
                    const auto IterateResult =
                        IterateSections(*Segment,
                                        SectionName,
                                        IsBigEndian,
                                        SegmentInfo.SectionNameList,
                                        Section);

                    if (IterateResult.Error != RunResult::Error::None) {
                        return IterateResult;
                    }

                    if (Section == nullptr) {
                        continue;
                    }

                    if (const auto SegName = SegmentName) {
                        if (Segment->segmentName() != SegmentName) {
                            continue;
                        }
                    }

                    if (Segment->isProtected(IsBigEndian)) {
                        return RunResult(RunResult::Error::ProtectedSegment);
                    }

                    SectionReserved1 = Section->reserved1(IsBigEndian);
                    SegmentList.emplace_back(std::move(SegmentInfo));

                    break;
                }
                case Kind::Segment64: {
                    if (!Is64Bit) {
                        break;
                    }

                    const auto Segment =
                        cast<SegmentCommand64>(&LC, IsBigEndian);
                    auto Section =
                        static_cast<const SegmentCommand64::Section *>(nullptr);

                    auto SegmentInfo = ::Operations::SegmentInfo();
                    const auto IterateResult =
                        IterateSections(*Segment,
                                        this->SectionName,
                                        IsBigEndian,
                                        SegmentInfo.SectionNameList,
                                        Section);

                    if (IterateResult.Error != RunResult::Error::None) {
                        return IterateResult;
                    }

                    if (Section == nullptr) {
                        continue;
                    }

                    if (const auto SegName = this->SegmentName) {
                        if (Segment->segmentName() != this->SegmentName) {
                            continue;
                        }
                    }

                    if (Segment->isProtected(IsBigEndian)) {
                        return RunResult(RunResult::Error::ProtectedSegment);
                    }

                    SectionReserved1 = Section->reserved1(IsBigEndian);
                    SegmentList.emplace_back(std::move(SegmentInfo));

                    break;
                }
                case Kind::SymbolTable: {
                    if (SymTabCmd != nullptr) {
                        return RunResult(
                            RunResult::Error::MultipleSymTabCommands);
                    }

                    SymTabCmd = cast<SymTabCommand>(&LC, IsBigEndian);
                    break;
                }
                case Kind::DynamicSymbolTable: {
                    if (DynamicSymTabCmd != nullptr) {
                        return RunResult(
                            RunResult::Error::MultipleDynamicSymTabCommands);
                    }

                    DynamicSymTabCmd =
                        cast<DynamicSymTabCommand>(&LC, IsBigEndian);
                    break;
                }
                case Kind::SymbolSegment:
                case Kind::Thread:
                case Kind::UnixThread:
                case Kind::LoadFixedVMSharedLib:
                case Kind::IdFixedVMSharedLib:
                case Kind::Identity:
                case Kind::FixedVMFile:
                case Kind::PrePage:
                case Kind::LoadDylib:
                case Kind::IdDylib:
                case Kind::LoadDylinker:
                case Kind::IdDylinker:
                case Kind::PreBoundDylib:
                case Kind::Routines:
                case Kind::SubFramework:
                case Kind::SubUmbrella:
                case Kind::SubClient:
                case Kind::SubLibrary:
                case Kind::TwoLevelHints:
                case Kind::PreBindChecksum:
                case Kind::LoadWeakDylib:
                case Kind::Routines64:
                case Kind::Uuid:
                case Kind::Rpath:
                case Kind::CodeSignature:
                case Kind::SegmentSplitInfo:
                case Kind::ReexportDylib:
                case Kind::LazyLoadDylib:
                case Kind::EncryptionInfo:
                case Kind::DyldInfo:
                case Kind::DyldInfoOnly:
                case Kind::LoadUpwardDylib:
                case Kind::VersionMinMacOS:
                case Kind::VersionMinIOS:
                case Kind::FunctionStarts:
                case Kind::DyldEnvironment:
                case Kind::Main:
                case Kind::DataInCode:
                case Kind::SourceVersion:
                case Kind::DylibCodeSignDRS:
                case Kind::EncryptionInfo64:
                case Kind::LinkerOption:
                case Kind::LinkerOptimizationHint:
                case Kind::VersionMinTVOS:
                case Kind::VersionMinWatchOS:
                case Kind::Note:
                case Kind::BuildVersion:
                case Kind::DyldExportsTrie:
                case Kind::DyldChainedFixups:
                case Kind::FileSetEntry:
                    break;
            }

            if (LC.isSharedLibrary(IsBigEndian)) {
                const auto DylibCmd =
                    MachO::cast<MachO::DylibCommand>(LC, IsBigEndian);

                const auto PathOpt = DylibCmd.name(IsBigEndian);
                const auto Info = DylibInfo {
                    .Path =
                        std::string(PathOpt.has_value() ?
                            PathOpt.value() : Malformed),
                    .CurrentVersion = DylibCmd.currentVersion(IsBigEndian),
                    .CompatVersion = DylibCmd.compatVersion(IsBigEndian),
                    .Timestamp = DylibCmd.timestamp(IsBigEndian)
                };

                DylibList.push_back(Info);
            }
        }

        if (SymTabCmd == nullptr) {
            return RunResult(RunResult::Error::SymTabNotFound);
        }

        if (DynamicSymTabCmd == nullptr) {
            return RunResult(RunResult::Error::DynamicSymTabNotFound);
        }

        auto SymbolInfoList = std::vector<SymbolInfo>();
        auto IterateResult = RunResult();
        auto LongestSymbolLength = uint32_t();
        auto LargestIndex = uint64_t();
        auto LongestKindLength = uint8_t();

        if (Is64Bit) {
            IterateResult =
                IterateIndices<true>(*SymTabCmd,
                                     *DynamicSymTabCmd,
                                     Map,
                                     SectionReserved1,
                                     Opt.SkipInvalidIndices,
                                     Opt.Limit,
                                     IsBigEndian,
                                     SymbolInfoList,
                                     LongestSymbolLength,
                                     LargestIndex,
                                     LongestKindLength);

        } else {
            IterateResult =
                IterateIndices<false>(*SymTabCmd,
                                      *DynamicSymTabCmd,
                                      Map,
                                      SectionReserved1,
                                      Opt.SkipInvalidIndices,
                                      Opt.Limit,
                                      IsBigEndian,
                                      SymbolInfoList,
                                      LongestSymbolLength,
                                      LargestIndex,
                                      LongestKindLength);
        }

        if (IterateResult.Error != RunResult::Error::None) {
            return IterateResult;
        }

        if (!Opt.SortKindList.empty()) {
            const auto Lambda = [&](const auto &Lhs, const auto &Rhs) noexcept {
                auto Compare = std::strong_ordering::equivalent;
                for (const auto &Sort : Opt.SortKindList) {
                    Compare =
                        CompareEntriesBySortKind(Lhs, Rhs, DylibList, Sort);

                    if (Compare != std::strong_ordering::equivalent) {
                        break;
                    }
                }

                return Compare == std::strong_ordering::less;
            };

            std::sort(SymbolInfoList.begin(), SymbolInfoList.end(), Lambda);
        }

        const auto MaxIndexDigitCount =
            Utils::GetIntegerDigitCount(LargestIndex);
        const auto SymbolInfoListSizeDigitCount =
            Utils::GetIntegerDigitCount(SymbolInfoList.size());

        auto Counter = uint64_t();
        for (const auto &SymbolInfo : SymbolInfoList) {
            std::print(OutFile,
                       "Indirect-Symbol {:0{}}: ",
                       Counter + 1,
                       SymbolInfoListSizeDigitCount);

            std::print(OutFile, "\"{}\"", SymbolInfo.String);
            const auto PrintLength =
                STR_LENGTH("\"\"") + SymbolInfo.String.length();

            if (Opt.Verbose) {
                const auto RightPad = LongestSymbolLength + STR_LENGTH("\"\"");
                std::print(OutFile, "{:<{}}", "", RightPad - PrintLength);

                const auto SymbolKind = SymbolInfo.Kind;
                const auto SymbolKindDescription =
                    SymTabCommand::Entry::KindIsValid(SymbolKind) ?
                        SymTabCommand::Entry::KindGetDesc(SymbolKind) :
                        "<unknown>";

                const auto KindRightPad =
                    static_cast<int>(LongestKindLength +
                                     STR_LENGTH(" <Kind: , "));

                std::print(OutFile,
                           "{:<{}}Index: {:0{}}",
                           std::format(" <Kind: {}, ", SymbolKindDescription),
                           KindRightPad,
                           SymbolInfo.Index,
                           MaxIndexDigitCount);

                if (SymbolInfo.PrivateExternal) {
                    std::print(OutFile, ", Private-External");
                }

                if (SymbolInfo.DebugSymbol) {
                    std::print(OutFile, ", Debug-Symbol");
                }

                std::print(OutFile, ", Section: ");

                const auto SectionOrdinal = SymbolInfo.Section;
                if (SymbolInfo.Kind == SymTabCommand::Entry::Kind::Section &&
                    SectionOrdinal != 0)
                {
                    auto SegmentName = std::string_view();
                    const auto SectionOpt =
                        GetSectionAtOrdinal(SegmentList,
                                            SectionOrdinal,
                                            SegmentName);

                    if (const auto SectionName = SectionOpt) {
                        std::print(OutFile,
                                   "{}, ",
                                   Utils::SegmentSectionPair(
                                    SegmentName,
                                    *SectionName,
                                    /*PadSegments=*/true,
                                    /*PadSections=*/true));
                    }
                } else {
                    std::print(OutFile, "No-Section, ");
                }

                if (SymbolInfo.DylibOrdinal <= DylibList.size()) {
                    const auto &DylibInfo =
                        DylibList.at(SymbolInfo.DylibOrdinal - 1);

                    Utils::PrintDylibOrdinalInfo(OutFile,
                                                 SymbolInfo.DylibOrdinal,
                                                 DylibInfo.Path,
                                                 /*PrintPath=*/true,
                                                 /*IsOutOfBounds=*/false);
                } else {
                    Utils::PrintDylibOrdinalInfo(OutFile,
                                                 SymbolInfo.DylibOrdinal,
                                                 /*DylibPath=*/"",
                                                 /*PrintPath=*/true,
                                                 /*IsOutOfBounds=*/true);
                }

                std::print(OutFile, ">");
            }

            std::println(OutFile);
            Counter++;
        }

        return RunResult(RunResult::Error::None);
    }

    auto
    PrintSymbolPtrSection::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintSymbolPtrSection::run() got Object with "
                       "Kind::None");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintSymbolPtrSection::run()");
    }
}
