/*
 * Operations/PrintBindSymbolList.cpp
 * © suhas pai
 */

#include <algorithm>
#include <compare>
#include <string_view>
#include "ADT/Maximizer.h"

#include "MachO/BindInfo.h"
#include "MachO/LibraryList.h"
#include "MachO/SegmentList.h"

#include "Operations/Common.h"
#include "Operations/PrintBindSymbolList.h"

#include "Utils/Print.h"

namespace Operations {
    PrintBindSymbolList::PrintBindSymbolList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintBindSymbolList), OutFile(OutFile),
      Opt(Options) {}

    bool
    PrintBindSymbolList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintBindSymbolList::supportsObjectKind()");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
            case Objects::Kind::DscImage:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintBindSymbolList::supportsObjectKind()");
    }

    static auto
    CompareActionsBySortKind(
        const MachO::BindActionInfo &Lhs,
        const MachO::BindActionInfo &Rhs,
        const PrintBindSymbolList::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            case PrintBindSymbolList::Options::SortKind::None:
                assert(false &&
                       "Got SortKind-None in "
                       "Operations::PrintBindSymbolList::"
                       "CompareActionsBySortKind()");
            case PrintBindSymbolList::Options::SortKind::ByName:
                return Lhs.SymbolName <=> Rhs.SymbolName;
            case PrintBindSymbolList::Options::SortKind::ByDylibOrdinal:
                return Lhs.DylibOrdinal <=> Rhs.DylibOrdinal;
            case PrintBindSymbolList::Options::SortKind::ByKind:
                return Lhs.Kind <=> Rhs.Kind;
        }

        assert(false &&
               "Unrecognized (and invalid) Sort-Kind in "
                "Operations::PrintBindSymbolList::CompareActionsBySortKind()");
    }

    template <MachO::BindInfoKind BindKind>
    static void
    PrintBindAction(FILE *const OutFile,
                    const std::string_view Name,
                    const uint64_t Counter,
                    const unsigned SizeDigitLength,
                    const MachO::BindActionInfo &Action,
                    const uint64_t LongestBindSymbolLength,
                    const MachO::LibraryList &LibraryList,
                    const MachO::SegmentList &SegmentList,
                    const bool Is64Bit,
                    const struct PrintBindSymbolList::Options &Options) noexcept
    {
        std::print(OutFile,
                   "{} Symbol {:>{}}: ",
                   Name,
                   Counter,
                   SizeDigitLength);

        const auto SegmentIndex = static_cast<uint64_t>(Action.SegmentIndex);
        if (const auto Segment = SegmentList.atOrNull(SegmentIndex)) {
            auto FullAddr = uint64_t();
            const auto Section =
                Segment->findSectionWithVmAddrIndex(Action.AddrInSeg,
                                                    &FullAddr);

            std::print(OutFile,
                       "{} {}",
                       Utils::SegmentSectionPair(Segment ? Segment->Name : "",
                                                 Section ? Section->Name : "",
                                                 /*PadSegment=*/true,
                                                 /*PadSection=*/true),
                       Utils::CustomAddress(FullAddr, Is64Bit));

            if (Action.Addend) {
                std::print(OutFile,
                           " + {}",
                           Utils::CustomAddress(
                            static_cast<uint64_t>(Action.Addend),
                            Is64Bit));
            }
        } else {
            std::print(OutFile,
                       "{:<{}}",
                       "<unknown>",
                       Utils::SegmentSectionPairMaxLen);
        }

        if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
            constexpr auto LongestDesc =
                MachO::BindWriteKindGetDesc(
                    MachO::BindWriteKind::TextAbsolute32).length();

            std::print(OutFile,
                       " {:<{}}",
                       MachO::BindWriteKindGetDesc(Action.WriteKind),
                       LongestDesc);
        }

        const auto RightPad =
            LongestBindSymbolLength + STR_LENGTH(" \"\"");

        std::print(OutFile,
                   "{:<{}}",
                   std::format(" \"{}\"", Action.SymbolName),
                   RightPad);

        if constexpr (BindKind != MachO::BindInfoKind::Weak) {
            Operations::PrintDylibOrdinalInfo(OutFile,
                                              Action.DylibOrdinal,
                                              LibraryList,
                                              Options.Verbose,
                                              /*Prefix=*/" ");
        }

        std::println(OutFile);
    }

    template <MachO::BindInfoKind BindKind>
    static void
    PrintBindActionInfoList(
        FILE *const OutFile,
        const std::string_view Name,
        const std::vector<MachO::BindActionInfo> &List,
        const MachO::SegmentList &SegmentList,
        const MachO::LibraryList &LibraryList,
        bool Is64Bit,
        const struct PrintBindSymbolList::Options &Options) noexcept
    {
        if (List.empty()) {
            std::println(OutFile, "No {} Info", Name);
            return;
        }

        auto LongestBindSymbolLength = ADT::Maximizer<uint64_t>();
        for (const auto &Action : List) {
            LongestBindSymbolLength.set(Action.SymbolName.length());
        }

        switch (List.size()) {
            case 0:
                assert(false &&
                       "Bind-Symbol List shouldn't be empty at this point");
            case 1:
                std::println(OutFile, "1 {} Symbol:", Name);
                break;
            default:
                std::println(OutFile, "{} {} Symbols:", List.size(), Name);
                break;
        }

        auto Counter = 1ull;
        const auto SizeDigitLength = Utils::GetIntegerDigitCount(List.size());

        for (const auto &Symbol : List) {
            PrintBindAction<BindKind>(OutFile,
                                      Name,
                                      Counter,
                                      SizeDigitLength,
                                      Symbol,
                                      LongestBindSymbolLength.value(),
                                      LibraryList,
                                      SegmentList,
                                      Is64Bit,
                                      Options);
            Counter++;
        }
    }

    auto PrintBindSymbolList::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        auto LibraryList = MachO::LibraryList();
        auto SegmentList = MachO::SegmentList();

        auto BindRange = ADT::Range();
        auto LazyBindRange = ADT::Range();
        auto WeakBindRange = ADT::Range();
        auto FoundDyldInfo = false;

        for (const auto &LC : MachO.loadCommandsMap()) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                LibraryList.addLibrary(
                    cast<MachO::DylibCommand>(LC, IsBigEndian), IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment =
                        dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfo =
                    dyn_cast<MachO::DyldInfoCommand>(&LC, IsBigEndian))
            {
                BindRange = DyldInfo->bindRange(IsBigEndian);
                LazyBindRange = DyldInfo->lazyBindRange(IsBigEndian);
                WeakBindRange = DyldInfo->weakBindRange(IsBigEndian);

                FoundDyldInfo = true;
            }
        }

        if (!FoundDyldInfo) {
            return RunResult(RunResult::Error::NoDyldInfo);
        }

        if (BindRange.empty() && LazyBindRange.empty() && WeakBindRange.empty())
        {
            return RunResult(RunResult::Error::NoSymbols);
        }

        const auto &Opt = this->Opt;

        auto BindActionInfoList = std::vector<MachO::BindActionInfo>();
        auto LazyBindActionInfoList = std::vector<MachO::BindActionInfo>();
        auto WeakBindActionInfoList = std::vector<MachO::BindActionInfo>();

        auto ParseResult = MachO::BindOpcodeParseResult();
        if (Opt.PrintNormal) {
            if (MachO.map().range().contains(BindRange)) {
                const auto BindList =
                    MachO::BindActionList(MachO.map(),
                                          BindRange,
                                          SegmentList,
                                          Is64Bit);

                ParseResult = BindList.getListOfSymbols(BindActionInfoList);
                if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                    return RunResult(MachO::BindInfoKind::Normal, ParseResult);
                }
            }
        }

        if (Opt.PrintLazy) {
            if (MachO.map().range().contains(LazyBindRange)) {
                const auto LazyBindList =
                    MachO::LazyBindActionList(MachO.map(),
                                              LazyBindRange,
                                              SegmentList,
                                              Is64Bit);

                ParseResult =
                    LazyBindList.getListOfSymbols(LazyBindActionInfoList);

                if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                    return RunResult(MachO::BindInfoKind::Lazy, ParseResult);
                }
            }
        }

        if (Opt.PrintWeak) {
            if (MachO.map().range().contains(WeakBindRange)) {
                if (Opt.PrintNormal || Opt.PrintLazy) {
                    std::println(OutFile);
                }

                const auto WeakBindList =
                    MachO::WeakBindActionList(MachO.map(),
                                              WeakBindRange,
                                              SegmentList,
                                              Is64Bit);

                ParseResult =
                    WeakBindList.getListOfSymbols(WeakBindActionInfoList);

                if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                    return RunResult(MachO::BindInfoKind::Lazy, ParseResult);
                }
            }
        }

        if (!Opt.SortKindList.empty()) {
            const auto Comparator =
                [&](const MachO::BindActionInfo &Lhs,
                    const MachO::BindActionInfo &Rhs) noexcept
            {
                for (const auto &SortKind : Opt.SortKindList) {
                    const auto CmpResult =
                        CompareActionsBySortKind(Lhs, Rhs, SortKind);

                    if (CmpResult != std::strong_ordering::equal) {
                        return CmpResult == std::strong_ordering::less;
                    }
                }

                return false;
            };

            std::sort(BindActionInfoList.begin(),
                      BindActionInfoList.end(),
                      Comparator);
            std::sort(LazyBindActionInfoList.begin(),
                      LazyBindActionInfoList.end(),
                      Comparator);
            std::sort(WeakBindActionInfoList.begin(),
                      WeakBindActionInfoList.end(),
                      Comparator);
        }

        const auto OutFile = this->OutFile;
        if (Opt.PrintNormal) {
            if (!BindActionInfoList.empty()) {
                PrintBindActionInfoList<MachO::BindInfoKind::Normal>(
                    OutFile,
                    "Bind",
                    BindActionInfoList,
                    SegmentList,
                    LibraryList,
                    Is64Bit,
                    Opt);
            } else {
                std::println(OutFile, "No Bind-Actions were found");
            }
        }

        if (Opt.PrintLazy) {
            if (Opt.PrintNormal) {
                std::println(OutFile);
            }

            if (!LazyBindActionInfoList.empty()) {
                PrintBindActionInfoList<MachO::BindInfoKind::Lazy>(
                    OutFile,
                    "Lazy-Bind",
                    LazyBindActionInfoList,
                    SegmentList,
                    LibraryList,
                    Is64Bit,
                    Opt);
            } else {
                std::println(OutFile, "No Lazy-Bind Actions were found");
            }
        }

        if (Opt.PrintWeak) {
            if (Opt.PrintNormal || Opt.PrintLazy) {
                std::println(OutFile);
            }

            if (!WeakBindActionInfoList.empty()) {
                PrintBindActionInfoList<MachO::BindInfoKind::Weak>(
                    OutFile,
                    "Weak-Bind",
                    WeakBindActionInfoList,
                    SegmentList,
                    LibraryList,
                    Is64Bit,
                    Opt);
            } else {
                std::println(OutFile, "No Weak-Bind Actions were found");
            }
        }

        return RunResult();
    }

    auto PrintBindSymbolList::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintBindSymbolList::run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::DscImage:
            case Objects::Kind::FatMachO:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintBindSymbolList::run()");
    }
}