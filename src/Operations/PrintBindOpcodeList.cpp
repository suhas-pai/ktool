/*
 * Operations/PrintBindOpcodeList.cpp
 * © suhas pai
 */

#include "MachO/BindInfo.h"
#include "MachO/LibraryList.h"

#include "Operations/PrintBindOpcodeList.h"
#include "Utils/Print.h"

namespace Operations {
    PrintBindOpcodeList::PrintBindOpcodeList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintBindOpcodeList), OutFile(OutFile),
      Opt(Options) {}

    bool
    PrintBindOpcodeList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintBindOpcodeList::supportsObjectKind()");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
            case Objects::Kind::DscImage:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintBindOpcodeList::supportsObjectKind()");
    }

    static void
    PrintFlags(FILE *const OutFile, const MachO::BindSymbolFlags Flags) noexcept
    {
        std::print(OutFile, "Flags:");
        if (!Flags.empty()) {
            std::println(OutFile, "");
            if (Flags.hasNonWeakDefinition()) {
                std::println(OutFile, "\t\t" "Has Non-Weak Definition");
            }

            if (Flags.isWeakImport()) {
                std::println(OutFile, "\t\t" "Weak-Import");
            }
        } else {
            std::println(OutFile, " None");
        }
    }

    struct BindOpcodeInfo : public MachO::BindOpcodeIterateInfo {
        MachO::BindByte Byte;

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

    template <MachO::BindInfoKind BindKind, typename ListType>
    auto
    CollectBindOpcodeList(const MachO::SegmentList &SegList,
                          const ListType &List,
                          const bool Is64Bit) noexcept
    {
        auto OpcodeInfo = BindOpcodeInfo();
        auto InfoList = std::vector<BindOpcodeInfo>();

        for (const auto &Iter : List) {
            const auto &Byte = Iter.byte();
            const auto &IterInfo = Iter.info();

            OpcodeInfo.Byte = Byte;
            switch (Byte.opcode()) {
                case MachO::BindByte::Opcode::Done:
                    if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
                        goto done;
                    }

                    break;
                case MachO::BindByte::Opcode::SetDylibOrdinalImm:
                case MachO::BindByte::Opcode::SetDylibOrdinalUleb:
                case MachO::BindByte::Opcode::SetDylibSpecialImm:
                    OpcodeInfo.DylibOrdinal = IterInfo.DylibOrdinal;
                    break;
                case MachO::BindByte::Opcode::SetSymbolTrailingFlagsImm:
                    OpcodeInfo.SymbolName = IterInfo.SymbolName;
                    OpcodeInfo.Flags = IterInfo.Flags;

                    break;

                case MachO::BindByte::Opcode::SetKindImm:
                    OpcodeInfo.WriteKind = IterInfo.WriteKind;
                    break;
                case MachO::BindByte::Opcode::SetAddendSleb:
                    OpcodeInfo.Addend = IterInfo.Addend;
                    break;
                case MachO::BindByte::Opcode::SetSegmentAndOffsetUleb: {
                    OpcodeInfo.AddrInSeg = IterInfo.SegOffset;
                    OpcodeInfo.SegmentIndex = IterInfo.SegmentIndex;

                    GetSegmentAndSection(SegList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);
                    break;
                }
                case MachO::BindByte::Opcode::AddAddrUleb: {
                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(OpcodeInfo.AddrInSeg,
                                                   IterInfo.AddAddr);

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        break;
                    }

                    OpcodeInfo.AddAddr = IterInfo.AddAddr;
                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();

                    GetSegmentAndSection(SegList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);
                    break;
                }
                case MachO::BindByte::Opcode::DoBind: {
                    InfoList.emplace_back(OpcodeInfo);

                    const auto PtrSize = Utils::PointerSize(Is64Bit);
                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(OpcodeInfo.AddrInSeg,
                                                   PtrSize);

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        continue;
                    }

                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();
                    GetSegmentAndSection(SegList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);

                    continue;
                }
                case MachO::BindByte::Opcode::DoBindAddAddrUleb: {
                    InfoList.emplace_back(OpcodeInfo);
                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(
                            OpcodeInfo.AddrInSeg,
                            IterInfo.AddAddr,
                            Utils::PointerSize(Is64Bit));

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        continue;
                    }

                    OpcodeInfo.AddAddr = IterInfo.AddAddr;
                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();

                    GetSegmentAndSection(SegList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);
                    continue;
                }
                case MachO::BindByte::Opcode::DoBindAddAddrImmScaled: {
                    OpcodeInfo.Scale = IterInfo.Scale;
                    InfoList.emplace_back(OpcodeInfo);

                    const auto PtrSize = Utils::PointerSize(Is64Bit);
                    const auto AddOpt =
                        Utils::MulAddAndCheckOverflow(
                            PtrSize, IterInfo.Scale, PtrSize);

                    if (!AddOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        continue;
                    }

                    const auto AddrInSegOpt =
                        Utils::AddAndCheckOverflow(OpcodeInfo.AddrInSeg,
                                                   AddOpt.value());

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        continue;
                    }

                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();
                    GetSegmentAndSection(SegList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);

                    break;
                }
                case MachO::BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                    OpcodeInfo.Skip = IterInfo.Skip;
                    OpcodeInfo.Count = IterInfo.Count;

                    InfoList.emplace_back(OpcodeInfo);

                    const auto PtrSize = Utils::PointerSize(Is64Bit);
                    const auto StepSizeOpt =
                        Utils::AddAndCheckOverflow(IterInfo.Skip, PtrSize);

                    if (!StepSizeOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        break;
                    }

                    const auto AddrInSegOpt =
                        Utils::MulAddAndCheckOverflow(StepSizeOpt.value(),
                                                      IterInfo.Count,
                                                      OpcodeInfo.AddrInSeg);

                    if (!AddrInSegOpt.has_value()) {
                        OpcodeInfo.AddrInSegOverflows = true;
                        break;
                    }

                    OpcodeInfo.AddrInSeg = AddrInSegOpt.value();
                    GetSegmentAndSection(SegList,
                                         OpcodeInfo.SegmentIndex,
                                         OpcodeInfo.AddrInSeg,
                                         &OpcodeInfo.Segment,
                                         &OpcodeInfo.Section);

                    continue;
                }
                case MachO::BindByte::Opcode::Threaded:
                    break;
            }

            InfoList.emplace_back(OpcodeInfo);
        }

    done:
        return InfoList;
    }

    template <MachO::BindInfoKind BindKind>
    void
    PrintBindOpcodeListInfo(
        FILE *const OutFile,
        const std::string_view Name,
        const MachO::LibraryList &LibraryList,
        const std::span<BindOpcodeInfo> List,
        const bool Is64Bit,
        const struct PrintBindOpcodeList::Options &Options) noexcept
    {
        auto Counter = static_cast<uint64_t>(1);
        const auto SizeDigitLength = Utils::GetIntegerDigitCount(List.size());

        for (const auto &Iter : List) {
            const auto &Byte = Iter.Byte;
            const auto OpcodeName =
                MachO::BindByteOpcodeGetName(Byte.opcode())
                    .value_or("<unknown>");

            const auto PtrSize = Utils::PointerSize(Is64Bit);
            std::print(OutFile,
                       "{} {:>{}}: {}",
                       Name,
                       Counter,
                       SizeDigitLength,
                       OpcodeName);

            auto OpcodeAndArgLength = OpcodeName.length();
            const auto PrintArrow = [&]() noexcept {
                constexpr auto MaxArgLength = 32;
                constexpr auto LongestOpcodeNameLength =
                    MachO::BindByteOpcodeGetName(
                        MachO::BindByteOpcode::DoBindUlebTimesSkippingUleb)
                            .value()
                            .length();

                std::print(OutFile, " ");
                const auto PadLength =
                    (LongestOpcodeNameLength + MaxArgLength) -
                    static_cast<uint64_t>(OpcodeAndArgLength);

                Utils::PrintMultTimes(OutFile, "-", PadLength);
                std::print(OutFile, "> ");
            };

            const auto PrintAddressInfo = [&](const uint64_t Add = 0) noexcept {
                const auto FullAddr =
                    Iter.Segment->VmRange.locForIndex(Iter.AddrInSeg + Add);

                std::println(OutFile,
                             "{}, Segment-Address: {}, Full-Address: {}",
                             Utils::SegmentSectionPair(
                              Iter.Segment ? Iter.Segment->Name : "",
                              Iter.Section ? Iter.Section->Name : "",
                              /*PadSegment=*/false,
                              /*PadSection=*/false),
                             Utils::Address(Iter.AddrInSeg + Add),
                             Utils::Address(FullAddr));

                if (Iter.AddrInSegOverflows) {
                    std::print(OutFile, " (Overflows)");
                    return;
                }
            };

            switch (Byte.opcode()) {
                case MachO::BindByte::Opcode::Done:
                    std::println(OutFile, "");
                    if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
                        goto done;
                    }

                    break;
                case MachO::BindByte::Opcode::SetDylibOrdinalImm:
                case MachO::BindByte::Opcode::SetDylibOrdinalUleb:
                case MachO::BindByte::Opcode::SetDylibSpecialImm:
                    std::print(OutFile, "({})", Iter.DylibOrdinal);
                    if (Options.Verbose) {
                        OpcodeAndArgLength +=
                            2 + Utils::GetIntegerDigitCount(Iter.DylibOrdinal);

                        PrintArrow();

                        auto Path = std::string_view();
                        auto IsOutOfBounds = false;

                        if (Iter.DylibOrdinal > 0 &&
                            static_cast<uint64_t>(Iter.DylibOrdinal) <=
                                LibraryList.size())
                        {
                            const auto DylibIndex =
                                static_cast<uint64_t>(Iter.DylibOrdinal) - 1;

                            Path =
                                LibraryList.at(DylibIndex).Path
                                    .value_or("<Malformed>");
                        } else {
                            Path = "<Malformed>";
                            IsOutOfBounds = true;
                        }

                        Utils::PrintDylibOrdinalPath(OutFile,
                                                     Iter.DylibOrdinal,
                                                     Path,
                                                     /*PrintPath=*/true,
                                                     IsOutOfBounds);
                    }

                    std::println(OutFile, "");
                    break;
                case MachO::BindByte::Opcode::SetSymbolTrailingFlagsImm:
                    if (Options.Verbose) {
                        std::print(OutFile, "()", Iter.Flags.value());
                        OpcodeAndArgLength += STR_LENGTH("()");

                        PrintArrow();
                        std::print("\"{}\", ", Iter.SymbolName);

                        PrintFlags(OutFile, Iter.Flags);
                        std::println(OutFile, "");
                    } else {
                        std::println(OutFile,
                                     "(Symbol: \"{}\", Flags: 0x{:x})",
                                     Iter.SymbolName,
                                     Iter.Flags.value());
                    }

                    break;

                case MachO::BindByte::Opcode::SetKindImm: {
                    const auto FallBack = [Kind = Iter.WriteKind]() noexcept {
                        return std::format("<unrecognized, value: {}>)",
                                           static_cast<uint32_t>(Kind));
                    };

                    std::println(OutFile,
                                 "(Kind: {})",
                                 MachO::BindWriteKindGetName(Iter.WriteKind)
                                    .value_or(FallBack()));
                    break;
                }
                case MachO::BindByte::Opcode::SetAddendSleb:
                    std::println(OutFile,
                                 "(Addend: {})",
                                 Utils::CustomAddress(
                                  static_cast<uint64_t>(Iter.Addend),
                                  Is64Bit));

                    break;
                case MachO::BindByte::Opcode::SetSegmentAndOffsetUleb: {
                    std::print(OutFile, "(Segment: {})", Iter.SegmentIndex);
                    if (Options.Verbose) {
                        OpcodeAndArgLength +=
                            11 + Utils::GetIntegerDigitCount(Iter.SegmentIndex);

                        PrintArrow();
                        PrintAddressInfo();
                    }

                    break;
                }
                case MachO::BindByte::Opcode::AddAddrUleb: {
                    std::print(OutFile, "(Add: {})", Iter.AddAddr);
                    if (Options.Verbose) {
                        OpcodeAndArgLength +=
                            7 + Utils::GetIntegerDigitCount(Iter.AddAddr);

                        PrintArrow();
                        PrintAddressInfo();
                    }

                    std::println(OutFile, ")");
                    break;
                }
                case MachO::BindByte::Opcode::DoBind:
                    std::println(OutFile, "");
                    break;
                case MachO::BindByte::Opcode::DoBindAddAddrUleb:
                    std::print(OutFile, "(Add: {})", Iter.AddAddr);
                    if (Options.Verbose) {
                        OpcodeAndArgLength +=
                            9 + Utils::GetIntegerDigitCount(Iter.AddAddr);

                        PrintArrow();
                        PrintAddressInfo(PtrSize);
                    }

                    std::println(OutFile, ")");
                    break;
                case MachO::BindByte::Opcode::DoBindAddAddrImmScaled: {
                    std::print(OutFile, "(Scale: {})", Iter.Scale);
                    OpcodeAndArgLength +=
                        9 + Utils::GetIntegerDigitCount(Iter.Scale);

                    if (Options.Verbose) {
                        PrintArrow();
                        PrintAddressInfo(Iter.Scale * PtrSize);
                    }

                    std::println(OutFile, ")");
                    break;
                }
                case MachO::BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                    std::print(OutFile,
                               "(Skip: {}, Count: {})",
                               Iter.Skip,
                               Iter.Count);

                    if (Options.Verbose) {
                        OpcodeAndArgLength +=
                            17 +
                            Utils::GetIntegerDigitCount(Iter.Skip) +
                            Utils::GetIntegerDigitCount(Iter.Count);

                        const auto Add =
                            static_cast<uint64_t>(Iter.Skip) * Iter.Count +
                            Iter.Count * PtrSize;

                        PrintArrow();
                        PrintAddressInfo(Add);
                    }

                    std::println(OutFile, ")");
                    break;
                }
                case MachO::BindByte::Opcode::Threaded:
                    std::println(OutFile, "");
                    break;
            }

            Counter++;
        }

    done:
        return;
    }

    auto
    PrintBindOpcodeList::run(const Objects::MachO &MachO) const noexcept
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

        const auto LoadCommandsMap = MachO.loadCommandsMap();
        for (auto Iter = LoadCommandsMap.begin();
             Iter != LoadCommandsMap.end();
             Iter++)
        {
            if (Iter->isSharedLibrary(IsBigEndian)) {
                LibraryList.add(Iter.cast<MachO::DylibCommand>(), IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment = Iter.dyn_cast<Kind::Segment64>()) {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment = Iter.dyn_cast<Kind::Segment>()) {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfo = Iter.dyn_cast<MachO::DyldInfoCommand>()) {
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
            return RunResult(RunResult::Error::NoOpcodes);
        }

        auto BindOpcodeList = std::vector<BindOpcodeInfo>();
        auto LazyBindOpcodeList = std::vector<BindOpcodeInfo>();
        auto WeakBindOpcodeList = std::vector<BindOpcodeInfo>();

        if (Opt.PrintNormal) {
            if (!BindRange.empty() && MachO.map().range().contains(BindRange)) {
                const auto BindListMap = ADT::MemoryMap(MachO.map(), BindRange);
                const auto BindList =
                    MachO::BindOpcodeList(BindListMap, Is64Bit);

                BindOpcodeList =
                    CollectBindOpcodeList<MachO::BindInfoKind::Normal>(
                        SegmentList, BindList, Is64Bit);
            }
        }

        if (Opt.PrintLazy) {
            if (!LazyBindRange.empty() &&
                MachO.map().range().contains(LazyBindRange))
            {
                const auto LazyBindListMap =
                    ADT::MemoryMap(MachO.map(), LazyBindRange);
                const auto LazyBindList =
                    MachO::LazyBindOpcodeList(LazyBindListMap, Is64Bit);

                LazyBindOpcodeList =
                    CollectBindOpcodeList<MachO::BindInfoKind::Lazy>(
                        SegmentList, LazyBindList, Is64Bit);
            }
        }

        if (Opt.PrintWeak) {
            if (!WeakBindRange.empty() &&
                MachO.map().range().contains(WeakBindRange))
            {
                const auto WeakBindListMap =
                    ADT::MemoryMap(MachO.map(), WeakBindRange);
                const auto WeakBindList =
                    MachO::WeakBindOpcodeList(WeakBindListMap, Is64Bit);

                WeakBindOpcodeList =
                    CollectBindOpcodeList<MachO::BindInfoKind::Weak>(
                        SegmentList, WeakBindList, Is64Bit);
            }
        }

        const auto OutFile = this->OutFile;
        if (Opt.PrintNormal) {
            if (!BindOpcodeList.empty()) {
                PrintBindOpcodeListInfo<MachO::BindInfoKind::Normal>(
                    OutFile,
                    "Bind",
                    LibraryList,
                    BindOpcodeList,
                    Is64Bit,
                    Opt);
            } else {
                std::println(stderr, "No (Normal) Bind Opcodes found");
            }
        }

        if (Opt.PrintLazy) {
            if (Opt.PrintNormal) {
                std::println(OutFile, "");
            }

            if (!LazyBindOpcodeList.empty()) {
                PrintBindOpcodeListInfo<MachO::BindInfoKind::Lazy>(
                    OutFile,
                    "Lazy-Bind",
                    LibraryList,
                    LazyBindOpcodeList,
                    Is64Bit,
                    Opt);
            } else {
                std::println(stderr, "No Lazy-Bind Opcodes found");
            }
        }

        if (Opt.PrintWeak) {
            if (Opt.PrintNormal || Opt.PrintWeak) {
                std::println(OutFile, "");
            }

            if (!WeakBindOpcodeList.empty()) {
                PrintBindOpcodeListInfo<MachO::BindInfoKind::Weak>(
                    OutFile,
                    "Weak-Bind",
                    LibraryList,
                    WeakBindOpcodeList,
                    Is64Bit,
                    Opt);
            } else {
                std::println(stderr, "No Weak-Bind Opcodes found");
            }
        }

        return RunResult();
    }

    auto
    PrintBindOpcodeList::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintBindOpcodeList::run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::DscImage:
            case Objects::Kind::FatMachO:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintBindOpcodeList::run()");
    }
}
