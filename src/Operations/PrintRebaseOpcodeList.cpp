/*
 * Operations/PrintRebaseOpcodeList.cpp
 * © suhas pai
 */

#include "MachO/RebaseInfo.h"
#include "MachO/SegmentList.h"

#include "Operations/PrintRebaseOpcodeList.h"
#include "Operations/Kind.h"
#include "Utils/Print.h"

namespace Operations {
    PrintRebaseOpcodeList::PrintRebaseOpcodeList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintRebaseOpcodeList), OutFile(OutFile),
      Opt(Options) {}

    bool
    PrintRebaseOpcodeList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintRebaseOpcodeList::supportsObjectKind()");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::DscImage:
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintRebaseOpcodeList::supportsObjectKind()");
    }

    struct RebaseOpcodeInfo : public MachO::RebaseOpcodeIterateInfo {
        MachO::RebaseByte Byte;

        uint64_t AddrInSeg;
        bool AddrInSegOverflows : 1;

        const MachO::SegmentInfo *Segment;
        const MachO::SectionInfo *Section;
    };

    static inline void
    GetSegmentAndSection(const MachO::SegmentList &SegList,
                         const int64_t SegmentIndex,
                         const uint64_t AddrInSeg,
                         const MachO::SegmentInfo **const SegmentOut,
                         const MachO::SectionInfo **const SectionOut) noexcept
    {
        const auto Segment =
            SegList.atOrNull(static_cast<uint64_t>(SegmentIndex));

        if (Segment != nullptr) {
            *SegmentOut = Segment;
            *SectionOut = Segment->findSectionWithVmAddrIndex(AddrInSeg);
        }
    }

    template <typename ListType>
    std::vector<RebaseOpcodeInfo>
    CollectRebaseOpcodeList(const MachO::SegmentList &SegmentList,
                            const ListType &List,
                            const bool Is64Bit) noexcept
    {
        auto OpcodeInfo = RebaseOpcodeInfo();
        auto InfoList = std::vector<RebaseOpcodeInfo>();

        for (const auto &Iter : List) {
            const auto &Byte = Iter.byte();
            const auto &IterInfo = Iter.info();

            OpcodeInfo.Byte = Byte;
            switch (Byte.opcode()) {
                case MachO::RebaseByte::Opcode::Done:
                    break;
                case MachO::RebaseByte::Opcode::SetKindImm:
                    OpcodeInfo.Kind = IterInfo.Kind;
                    break;
                case MachO::RebaseByte::Opcode::SetSegmentAndOffsetUleb: {
                    OpcodeInfo.AddrInSeg = IterInfo.SegOffset;
                    OpcodeInfo.SegmentIndex = IterInfo.SegmentIndex;

                    GetSegmentAndSection(SegmentList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);
                    break;
                }
                case MachO::RebaseByte::Opcode::AddAddrImmScaled:
                    OpcodeInfo.AddAddr =
                        static_cast<int64_t>(
                            OpcodeInfo.Scale * Utils::PointerSize(Is64Bit));

                    [[fallthrough]];
                case MachO::RebaseByte::Opcode::AddAddrUleb: {
                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(OpcodeInfo.AddrInSeg,
                                                   IterInfo.AddAddr);

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        break;
                    }

                    OpcodeInfo.AddAddr = IterInfo.AddAddr;
                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();

                    GetSegmentAndSection(SegmentList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);

                    break;
                }
                case MachO::RebaseByte::Opcode::DoRebaseAddAddrUleb: {
                    InfoList.emplace_back(OpcodeInfo);
                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(OpcodeInfo.AddrInSeg,
                                                   IterInfo.AddAddr,
                                                   Utils::PointerSize(Is64Bit));

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        continue;
                    }

                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();
                    OpcodeInfo.AddAddr = IterInfo.AddAddr;

                    GetSegmentAndSection(SegmentList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);
                    continue;
                }
                case MachO::RebaseByte::Opcode::DoRebaseImmTimes:
                case MachO::RebaseByte::Opcode::DoRebaseUlebTimes: {
                    OpcodeInfo.Count = IterInfo.Count;
                    InfoList.emplace_back(OpcodeInfo);

                    const auto PtrSize = Utils::PointerSize(Is64Bit);
                    const auto AddrInSegOpt =
                        Utils::MulAddAndCheckOverflow(PtrSize,
                                                      OpcodeInfo.Count,
                                                      OpcodeInfo.AddrInSeg);

                    if (!AddrInSegOpt.has_value()) {
                        continue;
                    }

                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();
                    GetSegmentAndSection(SegmentList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);

                    break;
                }
                case MachO::RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb: {
                    OpcodeInfo.Skip = IterInfo.Skip;
                    OpcodeInfo.Count = IterInfo.Count;

                    InfoList.emplace_back(OpcodeInfo);

                    const auto PtrSize = Utils::PointerSize(Is64Bit);
                    const auto AddOpt =
                        Utils::AddMulAndCheckOverflow<int64_t>(IterInfo.Skip,
                                                               PtrSize,
                                                               IterInfo.Count);

                    if (!AddOpt.has_value()) {
                        break;
                    }

                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(OpcodeInfo.AddrInSeg,
                                                   AddOpt.value());

                    if (!AddrInSegOpt.has_value()) {
                        break;
                    }

                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();
                    GetSegmentAndSection(SegmentList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);

                    continue;
                }
            }

            InfoList.emplace_back(OpcodeInfo);
        }

        return InfoList;
    }

    static auto
    PrintRebaseOpcodeCollection(
        FILE *const OutFile,
        const std::vector<RebaseOpcodeInfo> &Collection,
        const bool Is64Bit,
        const PrintRebaseOpcodeList::Options &Opt) noexcept
    {
        const auto PtrSize = Utils::PointerSize(Is64Bit);
        const auto SizeDigitLength =
            Utils::GetIntegerDigitCount(Collection.size());

        auto Counter = static_cast<uint64_t>(1);
        for (const auto &Iter : Collection) {
            const auto &Byte = Iter.Byte;
            const auto &OpcodeName =
                MachO::RebaseByteOpcodeGetName(Byte.opcode());

            fprintf(OutFile,
                    "Rebase-Opcode %" LEFTPAD_FMT "s: %s",
                    PAD_FMT_ARGS(SizeDigitLength),
                    Utils::GetFormattedNumber(Counter).c_str(),
                    OpcodeName.data());

            auto OpcodeAndArgLength = static_cast<int>(OpcodeName.length());
            const auto PrintAddressInfo = [&](const uint64_t Add = 0) noexcept {
                constexpr auto MaxArgLength = 32;
                constexpr auto LongestOpcodeNameLength =
                    MachO::RebaseByteOpcodeGetName(
                        MachO::RebaseByteOpcode::DoRebaseUlebTimesSkipUleb)
                            .length();

                fputs(" ", OutFile);
                const auto PadLength =
                    (LongestOpcodeNameLength + MaxArgLength) -
                    static_cast<uint64_t>(OpcodeAndArgLength);

                Utils::PrintMultTimes(OutFile, "-", PadLength);
                fputs("> ", OutFile);
                Utils::PrintSegmentSectionPair(
                    OutFile,
                    Iter.Segment ? Iter.Segment->Name : "",
                    Iter.Section ? Iter.Section->Name : "",
                    /*PadSegment=*/true,
                    /*PadSection=*/true,
                    /*Prefix=*/"Segment: ");

                Utils::PrintAddress(OutFile,
                                    Iter.AddrInSeg + Add,
                                    Is64Bit,
                                    "Segment-Address: ",
                                    ", ");

                const auto FullAddr =
                    Iter.Segment->VmRange.locForIndex(Iter.AddrInSeg + Add);

                Utils::PrintAddress(OutFile,
                                    FullAddr,
                                    Is64Bit,
                                    "Full-Address: ",
                                    "\n");

                if (Iter.AddrInSegOverflows) {
                    fputs(" (Overflows)", OutFile);
                    return;
                }
            };

            switch (Byte.opcode()) {
                case MachO::RebaseByte::Opcode::Done:
                    fputc('\n', OutFile);
                    break;
                case MachO::RebaseByte::Opcode::SetKindImm: {
                    const auto KindName =
                        MachO::RebaseWriteKindIsValid(Iter.Kind) ?
                            MachO::RebaseWriteKindGetString(Iter.Kind).data() :
                            "<unrecognized>";

                    if (MachO::RebaseWriteKindIsValid(Iter.Kind)) {
                        OpcodeAndArgLength +=
                            fprintf(OutFile, "(%s)\n", KindName);
                    } else {
                        OpcodeAndArgLength +=
                            fprintf(OutFile,
                                    "(<unrecognized, Kind: %" PRIu32 ">)\n",
                                    static_cast<uint32_t>(Iter.Kind));
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::SetSegmentAndOffsetUleb: {
                    OpcodeAndArgLength +=
                        fprintf(OutFile,
                                "(Segment: %" PRIu32 ", ",
                                Iter.SegmentIndex);

                    OpcodeAndArgLength +=
                        Utils::PrintAddress(OutFile,
                                            Iter.SegOffset,
                                            Is64Bit,
                                            "Offset: ",
                                            ")");

                    if (Opt.Verbose) {
                        PrintAddressInfo();
                    }

                    fputc('\n', OutFile);
                    break;
                }
                case MachO::RebaseByte::Opcode::AddAddrImmScaled: {
                    OpcodeAndArgLength +=
                        fprintf(OutFile, "(Scale: %" PRId64 ")", Iter.Scale);

                    if (Opt.Verbose) {
                        PrintAddressInfo(Iter.Scale * PtrSize);
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::AddAddrUleb: {
                    OpcodeAndArgLength +=
                        fprintf(OutFile, "(Add: %" PRId64 ")", Iter.AddAddr);

                    if (Opt.Verbose) {
                        PrintAddressInfo();
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::DoRebaseAddAddrUleb:
                    OpcodeAndArgLength +=
                        fprintf(OutFile, "(Add: %" PRId64 ")", Iter.AddAddr);

                    if (Opt.Verbose) {
                        PrintAddressInfo(static_cast<uint64_t>(Iter.AddAddr));
                    }

                    break;
                case MachO::RebaseByte::Opcode::DoRebaseImmTimes:
                case MachO::RebaseByte::Opcode::DoRebaseUlebTimes: {
                    OpcodeAndArgLength +=
                        fprintf(OutFile, "(Count: %" PRId64 ")", Iter.Count);

                    if (Opt.Verbose) {
                        PrintAddressInfo(Iter.Count * PtrSize);
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb: {
                    OpcodeAndArgLength +=
                        fprintf(OutFile,
                                "(Skip: %" PRId64 ", Count: %" PRIu64 ")",
                                Iter.Skip,
                                Iter.Count);

                    if (Opt.Verbose) {
                        const auto Add =
                            static_cast<uint64_t>(Iter.Skip) * Iter.Count +
                            Iter.Count * PtrSize;

                        PrintAddressInfo(Add);
                    }

                    break;
                }
            }

            Counter++;
        }
    }

    auto
    PrintRebaseOpcodeList::run(const Objects::MachO &MachO) const noexcept
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
                RebaseRange = DyldInfo->rebaseRange(IsBigEndian);
                FoundDyldInfo = true;
            }
        }

        if (!FoundDyldInfo) {
            return RunResult(RunResult::Error::NoDyldInfo);
        }

        if (RebaseRange.empty()) {
            return RunResult(RunResult::Error::NoOpcodes);
        }

        auto RebaseOpcodeList = std::vector<RebaseOpcodeInfo>();
        if (MachO.map().range().contains(RebaseRange)) {
            const auto RebaseListMap = ADT::MemoryMap(MachO.map(), RebaseRange);
            const auto RebaseList =
                MachO::RebaseOpcodeList(RebaseListMap, Is64Bit);

            RebaseOpcodeList =
                CollectRebaseOpcodeList(SegmentList, RebaseList, Is64Bit);
        }

        if (RebaseOpcodeList.empty()) {
            return RunResult(RunResult::Error::NoOpcodes);
        }

        PrintRebaseOpcodeCollection(OutFile,
                                    RebaseOpcodeList,
                                    Is64Bit,
                                    Opt);

        return RunResult(RunResult::Error::None);
    }

    auto
    PrintRebaseOpcodeList::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintRebaseOpcodeList::run() got Object with "
                       "Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
            case Objects::Kind::DscImage:
            case Objects::Kind::DyldSharedCache:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintRebaseOpcodeList::run()");
    }
}