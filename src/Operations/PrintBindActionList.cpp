/*
 * Operations/PrintBindActionList.cpp
 * © suhas pai
 */

#include <algorithm>
#include "ADT/Maximizer.h"

#include "MachO/BindInfo.h"
#include "MachO/LibraryList.h"

#include "Operations/Common.h"
#include "Operations/PrintBindActionList.h"

#include "Utils/Print.h"

namespace Operations {
    PrintBindActionList::PrintBindActionList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintBindActionList), OutFile(OutFile),
      Opt(Options) {}

    bool
    PrintBindActionList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintBindActionList::supportsObjectKind()");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return true;
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintBindActionList::supportsObjectKind()");
    }

    static int
    CompareActionsBySortKind(
        const MachO::BindActionInfo &Lhs,
        const MachO::BindActionInfo &Rhs,
        const PrintBindActionList::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            case PrintBindActionList::Options::SortKind::None:
                assert(false &&
                       "Unrecognized PrintBindActionList::Options::SortKind");
            case PrintBindActionList::Options::SortKind::ByName:
                return Lhs.SymbolName.compare(Rhs.SymbolName);
            case PrintBindActionList::Options::SortKind::ByDylibOrdinal:
                if (Lhs.DylibOrdinal < Rhs.DylibOrdinal) {
                    return -1;
                } else if (Lhs.DylibOrdinal == Rhs.DylibOrdinal) {
                    return 0;
                }

                return 1;
            case PrintBindActionList::Options::SortKind::ByKind: {
                const auto LhsKind = static_cast<uint8_t>(Lhs.WriteKind);
                const auto RhsKind = static_cast<uint8_t>(Rhs.WriteKind);

                if (LhsKind < RhsKind) {
                    return -1;
                } else if (LhsKind == RhsKind) {
                    return 0;
                }

                return 1;
            }
        }

        return false;
    }

    template <MachO::BindInfoKind BindKind>
    static void
    PrintBindAction(FILE *const OutFile,
                    const char *const Name,
                    const uint64_t Counter,
                    const unsigned SizeDigitLength,
                    const MachO::BindActionInfo &Action,
                    const uint64_t LongestBindSymbolLength,
                    const MachO::LibraryList &LibraryList,
                    const MachO::SegmentList &SegmentList,
                    const bool Is64Bit,
                    const struct PrintBindActionList::Options &Options) noexcept
    {
        std::print(OutFile,
                   "{} Action {:>{}}: ",
                   Name,
                   Counter,
                   SizeDigitLength);

        if (const auto *const Segment =
                SegmentList.atOrNull(
                    static_cast<uint64_t>(Action.SegmentIndex)))
        {
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

            if (Action.Addend != 0) {
                std::print(OutFile,
                           " + {}",
                           Utils::CustomAddress(
                            static_cast<uint64_t>(Action.Addend), Is64Bit));
            }
        } else {
            std::print(OutFile,
                       "{:<{}}",
                       "<out-of-bounds>",
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

        std::print(OutFile, " \"{}\"", Action.SymbolName);
        Utils::PadSpaces(
            OutFile,
            static_cast<uint32_t>(
                LongestBindSymbolLength - Action.SymbolName.length()));

        if constexpr (BindKind != MachO::BindInfoKind::Weak) {
            Operations::PrintDylibOrdinalInfo(OutFile,
                                              Action.DylibOrdinal,
                                              LibraryList,
                                              Options.Verbose,
                                              " ");
        }

        std::println(OutFile);
    }

    template <MachO::BindInfoKind BindKind>
    static void
    PrintBindActionInfoList(
        FILE *const OutFile,
        const char *const Name,
        const std::vector<MachO::BindActionInfo> &List,
        const MachO::SegmentList &SegmentList,
        const MachO::LibraryList &LibraryList,
        const bool Is64Bit,
        const struct PrintBindActionList::Options &Options) noexcept
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
                       "MachO::BindActionList shouldn't be empty at this "
                       "point");
            case 1:
                std::println(OutFile, "1 {} Action:", Name);
                break;
            default:
                std::println(OutFile, "{} {} Actions:", List.size(), Name);
                break;
        }

        auto Counter = 1ull;
        const auto SizeDigitLength = Utils::GetIntegerDigitCount(List.size());

        for (const auto &Action : List) {
            PrintBindAction<BindKind>(OutFile,
                                      Name,
                                      Counter,
                                      SizeDigitLength,
                                      Action,
                                      LongestBindSymbolLength.value(),
                                      LibraryList,
                                      SegmentList,
                                      Is64Bit,
                                      Options);
            Counter++;
        }
    }

    auto
    PrintBindActionList::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        const auto &Opt = this->Opt;

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
            return RunResult(RunResult::Error::NoActions);
        }

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

                ParseResult = BindList.getAsList(BindActionInfoList);
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

                ParseResult = LazyBindList.getAsList(LazyBindActionInfoList);
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

                ParseResult = WeakBindList.getAsList(WeakBindActionInfoList);
                if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                    return RunResult(MachO::BindInfoKind::Weak, ParseResult);
                }
            }
        }

        if (!Opt.SortKindList.empty()) {
            const auto Comparator =
                [&](const MachO::BindActionInfo &Lhs,
                    const MachO::BindActionInfo &Rhs) noexcept
            {
                auto Compare = int();
                for (const auto &SortKind : Opt.SortKindList) {
                    Compare = CompareActionsBySortKind(Lhs, Rhs, SortKind);
                    if (Compare != 0) {
                        break;
                    }

                    continue;
                }

                return Compare < 0;
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
                std::print(OutFile, "No Lazy-Bind Actions were found");
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

    auto PrintBindActionList::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintBindActionList::run() got Object with Kind::None");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintBindActionList::run()");
    }
}
