/*
 * Operations/PrintBindSymbolList.cpp
 * © suhas pai
 */

#include <algorithm>
#include "ADT/Maximizer.h"

#include "MachO/BindInfo.h"
#include "MachO/LibraryList.h"
#include "MachO/LoadCommands.h"
#include "MachO/SegmentList.h"

#include "Operations/Common.h"
#include "Operations/PrintBindSymbolList.h"

#include "Utils/Print.h"

namespace Operations {
    PrintBindSymbolList::PrintBindSymbolList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options) {}

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
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintBindSymbolList::supportsObjectKind()");
    }

    static int
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
                return Lhs.SymbolName.compare(Rhs.SymbolName);
            case PrintBindSymbolList::Options::SortKind::ByDylibOrdinal:
                if (Lhs.DylibOrdinal == Rhs.DylibOrdinal) {
                    return 0;
                } else if (Lhs.DylibOrdinal < Rhs.DylibOrdinal) {
                    return -1;
                }

                return 1;
            case PrintBindSymbolList::Options::SortKind::ByKind:
                const auto LhsKind = static_cast<uint8_t>(Lhs.WriteKind);
                const auto RhsKind = static_cast<uint8_t>(Rhs.WriteKind);

                if (LhsKind == RhsKind) {
                    return 0;
                } else if (LhsKind < RhsKind) {
                    return -1;
                }

                return 1;
        }

        assert(false &&
               "Unrecognized (and invalid) Sort-Kind in "
                "Operations::PrintBindSymbolList::CompareActionsBySortKind()");
    }

    template <MachO::BindInfoKind BindKind>
    static void
    PrintBindAction(
        FILE *const OutFile,
        const char *const Name,
        const uint64_t Counter,
        const unsigned SizeDigitLength,
        const MachO::BindActionInfo &Action,
        const uint64_t LongestBindSymbolLength,
        const MachO::LibraryList &LibraryList,
        const MachO::SegmentList &SegmentList,
        const bool Is64Bit,
        const struct PrintBindSymbolList::Options &Options) noexcept
    {
        fprintf(OutFile,
                "%s Symbol %0*" PRIu64 ": ",
                Name,
                SizeDigitLength,
                Counter);

        if (const auto *Segment =
                SegmentList.atOrNull(
                    static_cast<uint64_t>(Action.SegmentIndex)))
        {
            auto FullAddr = uint64_t();
            const auto Section =
                Segment->findSectionWithVmAddrIndex(Action.AddrInSeg,
                                                    &FullAddr);

            Utils::PrintSegmentSectionPair(OutFile,
                                           Segment ? Segment->Name : "<null>",
                                           Section ? Section->Name : "<null>",
                                           /*PadSegment=*/true,
                                           /*PadSection=*/true);

            Utils::PrintAddress(OutFile, FullAddr, Is64Bit, " ");
            if (Action.Addend) {
                Utils::PrintAddress(OutFile,
                                    static_cast<uint64_t>(Action.Addend),
                                    Is64Bit,
                                    " + ");
            }
        } else {
            Utils::RightPadSpaces(OutFile,
                                  fputs("<unknown>", OutFile),
                                  Utils::SegmentSectionPairMaxLen);
        }

        if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
            constexpr auto LongestDesc =
                MachO::BindWriteKindGetDesc(
                    MachO::BindWriteKind::TextAbsolute32).length();

            fprintf(OutFile,
                    " %" RIGHTPAD_FMT "s",
                    RIGHTPAD_FMT_ARGS(static_cast<int>(LongestDesc)),
                    MachO::BindWriteKindGetDesc(Action.WriteKind).data());
        }

        const auto RightPad =
            static_cast<int>(LongestBindSymbolLength + STR_LENGTH(" \"\""));

        Utils::RightPadSpaces(OutFile,
                              fprintf(OutFile,
                                      " \"%s\"",
                                      Action.SymbolName.data()),
                              RightPad);

        if constexpr (BindKind != MachO::BindInfoKind::Weak) {
            Operations::PrintDylibOrdinalInfo(OutFile,
                                              Action.DylibOrdinal,
                                              LibraryList,
                                              Options.Verbose,
                                              " ");
        }

        fputc('\n', OutFile);
    }

    template <MachO::BindInfoKind BindKind>
    static void
    PrintBindActionInfoList(
        FILE *const OutFile,
        const char *Name,
        const std::vector<MachO::BindActionInfo> &List,
        const MachO::SegmentList &SegmentList,
        const MachO::LibraryList &LibraryList,
        bool Is64Bit,
        const struct PrintBindSymbolList::Options &Options) noexcept
    {
        if (List.empty()) {
            fprintf(OutFile, "No %s Info\n", Name);
            return;
        }

        auto LongestBindSymbolLength = ADT::Maximizer<uint64_t>();
        for (const auto &Action : List) {
            LongestBindSymbolLength.set(Action.SymbolName.length());
        }

        switch (List.size()) {
            case 0:
                assert(0 && "Bind-Symbol List shouldn't be empty at this point");
            case 1:
                fprintf(OutFile, "1 %s Symbol:\n", Name);
                break;
            default:
                fprintf(OutFile,
                        "%" PRIuPTR " %s Symbols:\n",
                        List.size(),
                        Name);
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

    auto
    PrintBindSymbolList::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);

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
                        MachO::dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        MachO::dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfo =
                    MachO::dyn_cast<MachO::DyldInfoCommand>(&LC, IsBigEndian))
            {
                BindRange = DyldInfo->bindRange(IsBigEndian);
                LazyBindRange = DyldInfo->lazyBindRange(IsBigEndian);
                WeakBindRange = DyldInfo->weakBindRange(IsBigEndian);

                FoundDyldInfo = true;
            }
        }

        if (!FoundDyldInfo) {
            return Result.set(RunError::NoDyldInfo);
        }

        if (BindRange.empty() && LazyBindRange.empty() && WeakBindRange.empty())
        {
            return Result.set(RunError::NoSymbols);
        }

        auto BindActionInfoList = std::vector<MachO::BindActionInfo>();
        auto LazyBindActionInfoList = std::vector<MachO::BindActionInfo>();
        auto WeakBindActionInfoList = std::vector<MachO::BindActionInfo>();

        if (Opt.PrintNormal) {
            if (MachO.map().range().contains(BindRange)) {
                const auto BindList =
                    MachO::BindActionList(MachO.map(),
                                          BindRange,
                                          SegmentList,
                                          Is64Bit);

                BindList.getListOfSymbols(BindActionInfoList);
            }
        }

        if (Opt.PrintLazy) {
            if (MachO.map().range().contains(BindRange)) {
                const auto LazyBindList =
                    MachO::BindActionList(MachO.map(),
                                          LazyBindRange,
                                          SegmentList,
                                          Is64Bit);

                LazyBindList.getListOfSymbols(LazyBindActionInfoList);
            }
        }

        if (Opt.PrintWeak) {
            if (MachO.map().range().contains(WeakBindRange)) {
                if (Opt.PrintNormal || Opt.PrintLazy) {
                    fputc('\n', OutFile);
                }

                const auto WeakBindList =
                    MachO::BindActionList(MachO.map(),
                                          WeakBindRange,
                                          SegmentList,
                                          Is64Bit);

                WeakBindList.getListOfSymbols(WeakBindActionInfoList);
            }
        }

        const auto Comparator =
            [&](const MachO::BindActionInfo &Lhs,
                const MachO::BindActionInfo &Rhs) noexcept
        {
            for (const auto &SortKind : Opt.SortKindList) {
                const auto CmpResult =
                    CompareActionsBySortKind(Lhs, Rhs, SortKind);

                if (CmpResult != 0) {
                    return CmpResult < 0;
                }

                continue;
            }

            return false;
        };

        if (!Opt.SortKindList.empty()) {
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
                fputs("No Bind-Actions were found\n", OutFile);
            }
        }

        if (Opt.PrintLazy) {
            if (Opt.PrintNormal) {
                fputc('\n', OutFile);
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
                fputs("No Lazy-Bind Actions were found\n", OutFile);
            }
        }

        if (Opt.PrintWeak) {
            if (Opt.PrintNormal || Opt.PrintLazy) {
                fputc('\n', OutFile);
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
                fputs("No Weak-Bind Actions were found\n", OutFile);
            }
        }

        return Result.set(RunError::None);
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
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintBindSymbolList::run()");
    }
}