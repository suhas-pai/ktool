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

            std::print(OutFile,
                       "Rebase-Opcode {:>{}}: {}",
                       Utils::FormattedNumber(Counter),
                       SizeDigitLength,
                       OpcodeName);

            auto OpcodeAndArgLength = OpcodeName.length();
            const auto PrintAddressInfo = [&](const uint64_t Add = 0) noexcept {
                constexpr auto MaxArgLength = 38;
                constexpr auto LongestOpcodeNameLength =
                    MachO::RebaseByteOpcodeGetName(
                        MachO::RebaseByteOpcode::DoRebaseUlebTimesSkipUleb)
                            .length();

                std::print(OutFile, " ");
                const auto PadLength =
                    (LongestOpcodeNameLength + MaxArgLength) -
                    static_cast<uint64_t>(OpcodeAndArgLength);

                Utils::PrintMultTimes(OutFile, "-", PadLength);
                const auto FullAddr =
                    Iter.Segment->VmRange.locForIndex(Iter.AddrInSeg + Add);

                std::println(OutFile,
                             "> Segment: {}"
                             "Segment-Address: {}, Full-Address: {}{}",
                             Utils::SegmentSectionPair(
                              Iter.Segment ? Iter.Segment->Name : "",
                              Iter.Section ? Iter.Section->Name : "",
                              /*PadSegment=*/true,
                              /*PadSection=*/true),
                             Utils::CustomAddress(Iter.AddrInSeg + Add,
                                                  Is64Bit),
                             Utils::CustomAddress(FullAddr, Is64Bit),
                             Iter.AddrInSegOverflows ? " (Overflows)" : "");
            };

            switch (Byte.opcode()) {
                case MachO::RebaseByte::Opcode::Done:
                    std::println(OutFile);
                    break;
                case MachO::RebaseByte::Opcode::SetKindImm: {
                    const auto KindName =
                        MachO::RebaseWriteKindIsValid(Iter.Kind) ?
                            MachO::RebaseWriteKindGetString(Iter.Kind) :
                            std::string_view("<unrecognized>");

                    if (MachO::RebaseWriteKindIsValid(Iter.Kind)) {
                        std::println(OutFile, "({})", KindName);
                    } else {
                        std::println(OutFile,
                                     "(<unrecognized, Kind: {}>)",
                                     static_cast<uint32_t>(Iter.Kind));
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::SetSegmentAndOffsetUleb: {
                    std::println(OutFile,
                                 "(Segment: {}, Offset: {})",
                                 Iter.SegmentIndex,
                                 Utils::CustomAddress(Iter.SegOffset, Is64Bit));

                    if (Opt.Verbose) {
                        OpcodeAndArgLength +=
                            22 +
                            Utils::GetIntegerDigitCount(Iter.SegmentIndex) +
                            Utils::AddressLength(Is64Bit);

                        PrintAddressInfo();
                    } else {
                        std::println(OutFile);
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::AddAddrImmScaled: {
                    std::print(OutFile, "(Scale: {})", Iter.Scale);
                    if (Opt.Verbose) {
                        OpcodeAndArgLength +=
                            9 + Utils::GetIntegerDigitCount(Iter.Scale);

                        PrintAddressInfo(Iter.Scale * PtrSize);
                    } else {
                        std::println(OutFile);
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::AddAddrUleb: {
                    std::print(OutFile, "(Add: {})", Iter.AddAddr);
                    if (Opt.Verbose) {
                        OpcodeAndArgLength +=
                            7 + Utils::GetIntegerDigitCount(Iter.AddAddr);

                        PrintAddressInfo();
                    } else {
                        std::println(OutFile);
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::DoRebaseAddAddrUleb:
                    std::print(OutFile, "(Add: {})", Iter.AddAddr);
                    if (Opt.Verbose) {
                        OpcodeAndArgLength +=
                            7 + Utils::GetIntegerDigitCount(Iter.AddAddr);

                        PrintAddressInfo(static_cast<uint64_t>(Iter.AddAddr));
                    } else {
                        std::println(OutFile);
                    }

                    break;
                case MachO::RebaseByte::Opcode::DoRebaseImmTimes:
                case MachO::RebaseByte::Opcode::DoRebaseUlebTimes: {
                    std::print(OutFile, "(Count: {})", Iter.Count);
                    if (Opt.Verbose) {
                        OpcodeAndArgLength +=
                            9 + Utils::GetIntegerDigitCount(Iter.Count);

                        PrintAddressInfo(Iter.Count * PtrSize);
                    } else {
                        std::println(OutFile);
                    }

                    break;
                }
                case MachO::RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb: {
                    std::print(OutFile,
                               "(Skip: {}, Count: {})",
                               Iter.Skip,
                               Iter.Count);

                    if (Opt.Verbose) {
                        OpcodeAndArgLength +=
                            17 +
                            Utils::GetIntegerDigitCount(Iter.Skip) +
                            Utils::GetIntegerDigitCount(Iter.Count);

                        const auto Add =
                            static_cast<uint64_t>(Iter.Skip) * Iter.Count +
                            Iter.Count * PtrSize;

                        PrintAddressInfo(Add);
                    } else {
                        std::println(OutFile);
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
                        dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.add(*Segment, IsBigEndian);
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

        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

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