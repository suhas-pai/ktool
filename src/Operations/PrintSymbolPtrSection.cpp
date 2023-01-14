//
//  Operations/PrintSymbolPtrSection.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>

#include "ADT/Maximizer.h"
#include "ADT/MemoryMap.h"

#include "MachO/LoadCommands.h"

#include "Operations/Base.h"
#include "Operations/PrintSymbolPtrSection.h"

#include "Utils/Misc.h"
#include "Utils/Print.h"

namespace Operations {
    PrintSymbolPtrSection::PrintSymbolPtrSection(
        FILE *const OutFile,
        const std::optional<std::string> &SegmentName,
        const std::string &SectionName,
        const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options), SegmentName(SegmentName),
      SectionName(SectionName) {}

    bool
    PrintSymbolPtrSection::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintSymbolPtrSection::supportsObjectKind");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintSymbolPtrSection::supportsObjectKind");
    }

    template <MachO::LoadCommandDerived T>
    static auto
    IterateSections(const T &Segment,
                    const std::string_view SectionName,
                    const bool IsBigEndian,
                    RunResult &Result,
                    std::vector<std::string_view> &SectionNameList,
                    const typename T::Section *&SectionOut) noexcept -> int
    {
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
                using RunError = PrintSymbolPtrSection::RunError;
                Result.set(RunError::NotSymbolPointerSection);

                return 1;
            }

            SectionOut = &Section;
            SectionNameList.emplace_back(Section.sectionName());
        }

        return 0;
    }

    struct DylibInfo {
        std::string_view Path;

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
            if (Index >= Segment.SectionNameList.size()) {
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
                   Operations::RunResult &Result,
                   const bool SkipInvalidIndices,
                   const uint64_t Limit,
                   const bool IsBigEndian,
                   std::vector<SymbolInfo> &SymbolInfoList,
                   uint32_t &LongestSymbolLength,
                   uint64_t &LargestIndex,
                   uint8_t &LongestKindLength) noexcept
    {
        using RunError = PrintSymbolPtrSection::RunError;
        using T = std::conditional_t<Is64Bit, uint64_t, uint32_t>;

        const auto IndirectSymbolOffset =
            DynamicSymTab.indirectSymbolsOffset(IsBigEndian);
        const auto IndirectSymbolCount =
            DynamicSymTab.indirectSymbolsCount(IsBigEndian);

        const auto IndexList =
            Map.get<T>(IndirectSymbolOffset, IndirectSymbolCount);

        if (IndexList == nullptr) {
            Result.set(RunError::IndexListOutOfBounds);
            return 1;
        }

        if (Reserved1 >= IndirectSymbolCount) {
            Result.set(RunError::IndexListOutOfBounds);
            return 1;
        }

        using namespace MachO;
        using SymTabEntry =
            std::conditional_t<Is64Bit,
                               SymTabCommand::Entry64,
                               SymTabCommand::Entry>;

        const auto SymbolCount = SymTab.symCount(IsBigEndian);
        const auto SymbolTable =
            Map.getFromRange<SymTabEntry>(
                SymTab.symRange(IsBigEndian, Is64Bit));

        if (SymbolTable == nullptr) {
            Result.set(RunError::SymbolTableOutOfBounds);
            return 1;
        }

        auto IndexMaximizer = ADT::Maximizer<uint64_t>();
        auto SymbolLengthMaximizer = ADT::Maximizer<uint32_t>();
        auto KindLengthMaximizer = ADT::Maximizer<uint8_t>();

        const auto StringTable =
            Map.getFromRange<const char>(SymTab.strRange(IsBigEndian));

        if (StringTable == nullptr) {
            Result.set(RunError::StringTableOutOfBounds);
            return 1;
        }

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

            if (Index >= SymbolCount) {
                if (!SkipInvalidIndices) {
                    Result.set(RunError::IndexOutOfBounds);
                    return 1;
                }

                continue;
            }

            const auto &Entry = SymbolTable[Index];
            const auto Info = SymbolInfo {
                .String = StringTable + Entry.index(IsBigEndian),
                .Index = Index,
                .Kind = Entry.kind(),
                .Section = Entry.Section,
                .DylibOrdinal = Entry.dylibOrdinal(IsBigEndian),
                .PrivateExternal = Entry.privateExternal(),
                .DebugSymbol = Entry.debugSymbol()
            };

            IndexMaximizer.set(Info.Index);
            SymbolLengthMaximizer.set(Info.String.length());
            if (MachO::SymTabCommand::Entry::KindIsValid(Info.Kind)) {
                const auto Desc =
                    MachO::SymTabCommand::Entry::KindGetDesc(Info.Kind);

                KindLengthMaximizer.set(Desc.length());
            }

            SymbolInfoList.emplace_back(std::move(Info));
            SymbolsAdded++;
        }

        LargestIndex = IndexMaximizer.value();
        LongestSymbolLength = SymbolLengthMaximizer.value();
        LongestKindLength = KindLengthMaximizer.value();

        return 0;
    }

    static auto
    CompareEntriesBySortKind(
        const SymbolInfo &Lhs,
        const SymbolInfo &Rhs,
        const std::vector<DylibInfo> &DylibInfoList,
        const PrintSymbolPtrSection::Options::SortKind SortKind) noexcept
        -> int
    {
        switch (SortKind) {
            using SortKind = PrintSymbolPtrSection::Options::SortKind;
            case SortKind::ByDylibOrdinal:
                if (Lhs.DylibOrdinal == Rhs.DylibOrdinal) {
                    return 0;
                }

                if (Lhs.DylibOrdinal < Rhs.DylibOrdinal) {
                    return -1;
                }

                return 1;
            case SortKind::ByDylibPath: {
                auto LhsDylibPath = std::string_view();
                auto RhsDylibPath = std::string_view();

                if (Lhs.DylibOrdinal >= DylibInfoList.size()) {
                    if (Rhs.DylibOrdinal >= DylibInfoList.size()) {
                        return 0;
                    }

                    return 1;
                } else if (Rhs.DylibOrdinal >= DylibInfoList.size()) {
                    return -1;
                }

                LhsDylibPath = DylibInfoList.at(Lhs.DylibOrdinal - 1).Path;
                RhsDylibPath = DylibInfoList.at(Rhs.DylibOrdinal - 1).Path;

                return LhsDylibPath.compare(RhsDylibPath);
            }
            case SortKind::ByIndex: {
                if (Lhs.Index == Rhs.Index) {
                    return 0;
                } else if (Lhs.Index < Rhs.Index) {
                    return -1;
                }

                return 1;
            }
            case SortKind::ByString:
                return Lhs.String.compare(Rhs.String);
        }

        assert(false && "CompareEntriesBySortKind() got unrecognized SortKind");
    }

    auto
    PrintSymbolPtrSection::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        using namespace MachO;

        auto Result = RunResult(Objects::Kind::MachO);
        if (SectionName.empty()) {
            return Result.set(RunError::EmptySectionName);
        }

        auto SectionData = static_cast<const char *>(nullptr);
        auto SectionReserved1 = uint32_t();

        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        auto SymTabCmd = static_cast<const SymTabCommand *>(nullptr);
        auto DynamicSymTabCmd =
            static_cast<const DynamicSymTabCommand *>(nullptr);

        constexpr auto Malformed = std::string_view("<malformed>");

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
                                        Result,
                                        SegmentInfo.SectionNameList,
                                        Section);

                    if (IterateResult != 0) {
                        return Result;
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
                        return Result.set(RunError::ProtectedSegment);
                    }

                    const auto SectionRange = Section->fileRange(IsBigEndian);
                    SectionData =
                        MachO.map().getFromRange<const char>(SectionRange);

                    if (SectionData == nullptr) {
                        return Result.set(RunError::InvalidSectionRange);
                    }

                    SegmentList.emplace_back(std::move(SegmentInfo));
                    break;
                }
                case Kind::Segment64: {
                    if (Is64Bit) {
                        break;
                    }

                    const auto Segment =
                        cast<SegmentCommand64>(&LC, IsBigEndian);
                    auto Section =
                        static_cast<const SegmentCommand64::Section *>(nullptr);

                    auto SegmentInfo = ::Operations::SegmentInfo();
                    const auto IterateResult =
                        IterateSections(*Segment,
                                        SectionName,
                                        IsBigEndian,
                                        Result,
                                        SegmentInfo.SectionNameList,
                                        Section);

                    if (IterateResult != 0) {
                        return Result;
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
                        return Result.set(RunError::ProtectedSegment);
                    }

                    const auto SectionRange = Section->fileRange(IsBigEndian);
                    SectionData =
                        MachO.map().getFromRange<const char>(SectionRange);

                    if (SectionData == nullptr) {
                        return Result.set(RunError::InvalidSectionRange);
                    }

                    SegmentList.emplace_back(std::move(SegmentInfo));
                    break;
                }
                case Kind::SymbolTable: {
                    if (SymTabCmd != nullptr) {
                        return Result.set(RunError::MultipleSymTabCommands);
                    }

                    SymTabCmd = cast<SymTabCommand>(&LC, IsBigEndian);
                    break;
                }
                case Kind::DynamicSymbolTable: {
                    if (DynamicSymTabCmd != nullptr) {
                        Result.set(RunError::MultipleDynamicSymTabCommands);
                        return Result;
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

            if (const auto DylibCmd =
                    dyn_cast<Kind::LoadDylib,
                             Kind::LoadWeakDylib,
                             Kind::ReexportDylib,
                             Kind::LazyLoadDylib,
                             Kind::LoadUpwardDylib>(&LC, IsBigEndian))
            {
                const auto PathOpt = DylibCmd->name(IsBigEndian);
                const auto Info = DylibInfo {
                    .Path = PathOpt.has_value() ? PathOpt.value() : Malformed,
                    .CurrentVersion = DylibCmd->currentVersion(IsBigEndian),
                    .CompatVersion = DylibCmd->compatVersion(IsBigEndian),
                    .Timestamp = DylibCmd->timestamp(IsBigEndian)
                };

                DylibList.emplace_back(std::move(Info));
            }
        }

        if (SectionData == nullptr) {
            return Result.set(RunError::SectionNotFound);
        }

        if (SymTabCmd == nullptr) {
            return Result.set(RunError::SymTabNotFound);
        }

        if (DynamicSymTabCmd == nullptr) {
            return Result.set(RunError::DynamicSymTabNotFound);
        }

        auto SymbolInfoList = std::vector<SymbolInfo>();
        auto IterateResult = int();
        auto LongestSymbolLength = uint32_t();
        auto LargestIndex = uint64_t();
        auto LongestKindLength = uint8_t();

        if (Is64Bit) {
            IterateResult =
                IterateIndices<true>(*SymTabCmd,
                                     *DynamicSymTabCmd,
                                     MachO.map(),
                                     SectionReserved1,
                                     Result,
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
                                      MachO.map(),
                                      SectionReserved1,
                                      Result,
                                      Opt.SkipInvalidIndices,
                                      Opt.Limit,
                                      IsBigEndian,
                                      SymbolInfoList,
                                      LongestSymbolLength,
                                      LargestIndex,
                                      LongestKindLength);
        }

        if (IterateResult != 0) {
            return Result;
        }

        if (!Opt.SortKindList.empty()) {
            const auto Lambda = [&](const auto &Lhs, const auto &Rhs) noexcept {
                auto Compare = int();
                for (const auto &Sort : Opt.SortKindList) {
                    Compare =
                        CompareEntriesBySortKind(Lhs, Rhs, DylibList, Sort);

                    if (Compare != 0) {
                        break;
                    }
                }

                return (Compare < 0);
            };

            std::sort(SymbolInfoList.begin(), SymbolInfoList.end(), Lambda);
        }

        const auto MaxIndexDigitCount =
            Utils::GetIntegerDigitCount(LargestIndex);
        const auto SymbolInfoListSizeDigitCount =
            Utils::GetIntegerDigitCount(SymbolInfoList.size());

        auto Counter = uint64_t();
        for (const auto &SymbolInfo : SymbolInfoList) {
            fprintf(OutFile,
                    "Indirect-Symbol %" ZEROPAD_FMT PRIu64 ": ",
                    ZEROPAD_FMT_ARGS(SymbolInfoListSizeDigitCount),
                    Counter + 1);

            const auto PrintLength =
                fprintf(OutFile, "\"%s\"", SymbolInfo.String.data());

            if (Opt.Verbose) {
                const auto RightPad =
                    static_cast<int>(LongestSymbolLength + STR_LENGTH("\"\""));

                Utils::RightPadSpaces(OutFile, PrintLength, RightPad);

                const auto SymbolKind = SymbolInfo.Kind;
                const auto SymbolKindDescription =
                    SymTabCommand::Entry::KindIsValid(SymbolKind) ?
                        SymTabCommand::Entry::KindGetDesc(SymbolKind) :
                        "<unknown>";

                const auto KindRightPad =
                    static_cast<int>(LongestKindLength +
                                     STR_LENGTH(" <Kind: , "));

                Utils::RightPadSpaces(OutFile,
                                      fprintf(OutFile,
                                              " <Kind: %s, ",
                                              SymbolKindDescription.data()),
                                      KindRightPad);

                fprintf(OutFile,
                        "Index: %" ZEROPAD_FMT PRIu64,
                        ZEROPAD_FMT_ARGS(MaxIndexDigitCount),
                        SymbolInfo.Index);

                if (SymbolInfo.PrivateExternal) {
                    fputs(", Private-External", OutFile);
                }

                if (SymbolInfo.DebugSymbol) {
                    fputs(", Debug-Symbol", OutFile);
                }

                fputs(", Section: ", OutFile);

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
                        Utils::PrintSegmentSectionPair(OutFile,
                                                       SegmentName,
                                                       *SectionName,
                                                       /*PadSegments=*/true,
                                                       /*PadSections=*/true,
                                                       "",
                                                       ", ");
                    }
                } else {
                    fputs("No-Section, ", OutFile);
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
                                                 std::string_view(),
                                                 /*PrintPath=*/true,
                                                 /*IsOutOfBounds=*/true);
                }

                fputc('>', OutFile);
            }

            fputc('\n', OutFile);
            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto
    PrintSymbolPtrSection::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintSymbolPtrSection::run");
    }
}
