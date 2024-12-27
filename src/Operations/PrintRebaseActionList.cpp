/*
 * Operations/PrintRebaseActionList.cpp
 * © suhas pai
 *
 */

#include "MachO/RebaseInfo.h"
#include "MachO/SegmentList.h"

#include "Objects/MachO.h"
#include "Operations/PrintRebaseActionList.h"

#include "Utils/Print.h"

namespace Operations {
    PrintRebaseActionList::PrintRebaseActionList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintRebaseActionList), OutFile(OutFile),
      Opt(Options) {}

    bool
    PrintRebaseActionList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintRebaseActionList::supportsObjectKind()");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
            case Objects::Kind::DscImage:
            case Objects::Kind::DyldSharedCache:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintRebaseActionList::supportsObjectKind()");
    }

    static void
    PrintRebaseAction(FILE *const OutFile,
                      const uint64_t Counter,
                      const int SizeDigitLength,
                      const MachO::RebaseActionInfo &Action,
                      const MachO::SegmentList &SegmentList,
                      const bool Is64Bit) noexcept
    {
        std::print(OutFile,
                   "Rebase-Action {:>{}}: ",
                   Utils::FormattedNumber(Counter),
                   SizeDigitLength);

        constexpr auto RebaseWriteKindLongestDescLength =
            MachO::RebaseWriteKindGetDesc(
                MachO::RebaseWriteKind::TextAbsolute32).length();

        std::print(OutFile,
                   " {:>{}}",
                   MachO::RebaseWriteKindGetDesc(Action.Kind),
                   RebaseWriteKindLongestDescLength);

        if (const auto Segment = SegmentList.atOrNull(Action.SegmentIndex)) {
            auto FullAddr = uint64_t();
            const auto Section =
                Segment->findSectionWithVmAddrIndex(Action.AddrInSeg,
                                                    &FullAddr);

            std::print(OutFile,
                       "{}{}",
                       Utils::SegmentSectionPair(Segment ? Segment->Name : "",
                                                 Section ? Section->Name : "",
                                                 /*PadSegment=*/true,
                                                 /*PadSection=*/true),
                       Utils::CustomAddress(FullAddr, Is64Bit));

        } else {
            std::print(OutFile,
                       "{:<{}}",
                       "<unknown>",
                       Utils::SegmentSectionPairMaxLen);
        }

        std::println(OutFile);
    }

    auto PrintRebaseActionList::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        auto SegmentList = MachO::SegmentList();
        auto RebaseRange = ADT::Range();
        auto FoundDyldInfo = false;

        for (const auto &LC : MachO.loadCommandsMap()) {
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
                RebaseRange = DyldInfo->rebaseRange(IsBigEndian);
                FoundDyldInfo = true;
            }
        }

        if (!FoundDyldInfo) {
            return RunResult(RunResult::Error::NoDyldInfo);
        }

        if (RebaseRange.empty()) {
            return RunResult(RunResult::Error::NoActions);
        }

        auto RebaseActionList = std::vector<MachO::RebaseActionInfo>();
        if (MachO.map().range().contains(RebaseRange)) {
            const auto RebaseListMap = ADT::MemoryMap(MachO.map(), RebaseRange);
            const auto RebaseList =
                MachO::RebaseActionList(RebaseListMap, Is64Bit);

            RebaseList.getAsList(RebaseActionList);
        }

        if (RebaseActionList.empty()) {
            return RunResult(RunResult::Error::NoActions);
        }

        auto Counter = uint64_t();

        const auto OutFile = this->OutFile;
        const auto SizeDigitLength =
            Utils::GetIntegerDigitCount(RebaseActionList.size());

        for (const auto &Action : RebaseActionList) {
            PrintRebaseAction(OutFile,
                              Counter,
                              static_cast<int>(SizeDigitLength),
                              Action,
                              SegmentList,
                              Is64Bit);
            Counter++;
        }

        return RunResult(RunResult::Error::None);
    }

    auto PrintRebaseActionList::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintRebaseActionList::run() got Object with "
                       "Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
            case Objects::Kind::DscImage:
            case Objects::Kind::DyldSharedCache:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintRebaseActionList::run()");
    }
}