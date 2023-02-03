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
    : OutFile(OutFile), Opt(Options) {}

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
    PrintFlags(FILE *const OutFile,
               const MachO::BindSymbolFlags Flags) noexcept
    {
        fputs(" - <", OutFile);

        if (!Flags.empty()) {
            const auto HasNonWeakDefinition = Flags.hasNonWeakDefinition();
            if (HasNonWeakDefinition) {
                fputs("Has Non-Weak Definition", OutFile);
            }

            if (Flags.isWeakImport()) {
                if (HasNonWeakDefinition) {
                    fputs(", ", OutFile);
                }

                fputs("Weak-Import", OutFile);
            }
        } else {
            fputs("None", OutFile);
        }

        fputc('>', OutFile);
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
    std::vector<BindOpcodeInfo>
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
                        Utils::AddAndCheckOverflow(
                            OpcodeInfo.AddrInSeg, PtrSize);

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

    [[maybe_unused]] done:
        return InfoList;
    }

    template <MachO::BindInfoKind BindKind>
    void
    PrintBindOpcodeListInfo(
        FILE *const OutFile,
        const char *const Name,
        const MachO::LibraryList &LibraryList,
        const std::vector<BindOpcodeInfo> &List,
        const bool Is64Bit,
        const struct PrintBindOpcodeList::Options &Options) noexcept
    {
        auto Counter = uint64_t(1);
        const auto SizeDigitLength = Utils::GetIntegerDigitCount(List.size());

        for (const auto &Iter : List) {
            const auto &Byte = Iter.Byte;
            const auto OpcodeName = MachO::BindByteOpcodeGetName(Byte.opcode());
            const auto PtrSize = Utils::PointerSize(Is64Bit);

            fprintf(OutFile,
                    "%s %" ZEROPAD_FMT PRIu64 ": %s",
                    Name,
                    ZEROPAD_FMT_ARGS(SizeDigitLength),
                    Counter,
                    OpcodeName.data());

            const auto PrintAddressInfo = [&](const uint64_t Add = 0) noexcept {
                if ((&Iter) != (&List.back() + 1)) {
                    if ((&Iter)[1].AddrInSegOverflows) {
                        fputs(" (Overflows)", OutFile);
                        return;
                    }
                }

                Utils::PrintAddress (OutFile,
                                     Iter.AddrInSeg + Add,
                                     Is64Bit,
                                     " - <Segment-Address: ",
                                     ", ");

                const auto FullAddr =
                    Iter.Segment->VmRange.begin() + Iter.AddrInSeg + Add;

                Utils::PrintAddress(OutFile,
                                    FullAddr,
                                    Is64Bit,
                                    "Full-Address: ",
                                    ">");
            };

            switch (Byte.opcode()) {
                case MachO::BindByte::Opcode::Done:
                    fputc('\n', OutFile);
                    if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
                        goto done;
                    }

                    break;
                case MachO::BindByte::Opcode::SetDylibOrdinalImm:
                case MachO::BindByte::Opcode::SetDylibOrdinalUleb:
                case MachO::BindByte::Opcode::SetDylibSpecialImm:
                    fprintf(OutFile, "(%" PRId64, Iter.DylibOrdinal);
                    if (Options.Verbose) {
                        fputs(" - ", OutFile);

                        auto Path = std::string_view("<Malformed>");
                        auto IsOutOfBounds = false;

                        if (Iter.DylibOrdinal > 0 &&
                            static_cast<uint64_t>(Iter.DylibOrdinal)
                                <= LibraryList.size())
                        {
                            const auto DylibIndex =
                                static_cast<uint64_t>(Iter.DylibOrdinal) - 1;
                            const auto &PathOpt =
                                LibraryList.at(DylibIndex).Path;

                            if (PathOpt.has_value()) {
                                Path = PathOpt.value();
                            }
                        } else {
                            IsOutOfBounds = true;
                        }

                        Utils::PrintDylibOrdinalPath(OutFile,
                                                     Iter.DylibOrdinal,
                                                     Path,
                                                     /*PrintPath=*/true,
                                                     IsOutOfBounds);
                    }

                    fputs(")\n", OutFile);
                    break;
                case MachO::BindByte::Opcode::SetSymbolTrailingFlagsImm:
                    fprintf(OutFile,
                            "(\"%s\", 0x%" PRIx8,
                            Iter.SymbolName.data(),
                            Iter.Flags.value());

                    if (Options.Verbose) {
                        if (!Iter.Flags.empty()) {
                            PrintFlags(OutFile, Iter.Flags);
                        }
                    }

                    fputs(")\n", OutFile);
                    break;

                case MachO::BindByte::Opcode::SetKindImm: {
                    const auto KindName =
                        MachO::BindWriteKindIsValid(Iter.WriteKind) ?
                            MachO::BindWriteKindGetName(Iter.WriteKind) :
                            "<unrecognized>";

                    fprintf(OutFile, "(%s)\n", KindName.data());
                    break;
                }
                case MachO::BindByte::Opcode::SetAddendSleb:
                    Utils::PrintAddress(OutFile,
                                        static_cast<uint64_t>(Iter.Addend),
                                        Is64Bit,
                                        "(",
                                        ")\n");
                    break;
                case MachO::BindByte::Opcode::SetSegmentAndOffsetUleb: {
                    fprintf(OutFile, "(%" PRId64, Iter.SegmentIndex);
                    if (Options.Verbose) {
                        Utils::PrintSegmentSectionPair(
                            OutFile,
                            Iter.Segment ? Iter.Segment->Name : "",
                            Iter.Section ? Iter.Section->Name : "",
                            /*PadSegment=*/false,
                            /*PadSection=*/false,
                            /*Prefix=*/" - <",
                            /*Suffix=*/">");
                    }

                    Utils::PrintAddress(OutFile, Iter.AddrInSeg, Is64Bit, ", ");
                    if (Options.Verbose) {
                        if (Iter.Segment != nullptr) {
                            const auto FullAddr =
                                Iter.Segment->VmRange.begin() + Iter.AddrInSeg;

                            Utils::PrintAddress(OutFile,
                                                FullAddr,
                                                Is64Bit,
                                                " <Full Address: ",
                                                ">");
                        }
                    }

                    fputs(")\n", OutFile);
                    break;
                }
                case MachO::BindByte::Opcode::AddAddrUleb: {
                    fprintf(OutFile, "(%" PRId64, Iter.AddAddr);
                    if (Options.Verbose) {
                        PrintAddressInfo();
                    }

                    fputs(")\n", OutFile);
                    break;
                }
                case MachO::BindByte::Opcode::DoBind:
                    fputc('\n', OutFile);
                    break;
                case MachO::BindByte::Opcode::DoBindAddAddrUleb:
                    fprintf(OutFile, "(%" PRId64, Iter.AddAddr);
                    if (Options.Verbose) {
                        PrintAddressInfo(PtrSize);
                    }

                    fputs(")\n", OutFile);
                    break;
                case MachO::BindByte::Opcode::DoBindAddAddrImmScaled: {
                    fprintf(OutFile, "(%" PRId64, Iter.Scale);
                    if (Options.Verbose) {
                        PrintAddressInfo(Iter.Scale * PtrSize);
                    }

                    fputs(")\n", OutFile);
                    break;
                }
                case MachO::BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                    fprintf(OutFile,
                            "(Skip: %" PRId64 ", Count: %" PRIu64,
                            Iter.Skip,
                            Iter.Count);

                    if (Options.Verbose) {
                        const auto Add =
                            (static_cast<uint64_t>(Iter.Skip) * Iter.Count) +
                            (Iter.Count * PtrSize);

                        PrintAddressInfo(Add);
                    }

                    fputs(")\n", OutFile);
                    break;
                }
                case MachO::BindByte::Opcode::Threaded:
                    fputc('\n', OutFile);
                    break;
            }

            Counter++;
        }

    [[maybe_unused]] done:
        return;
    }

    auto
    PrintBindOpcodeList::run(const Objects::MachO &MachO) const noexcept
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

        const auto LoadCommandsMap = MachO.loadCommandsMap();
        for (auto Iter = LoadCommandsMap.begin();
             Iter != LoadCommandsMap.end();
             Iter++)
        {
            if (Iter->isSharedLibrary(IsBigEndian)) {
                LibraryList.addLibrary(Iter.cast<MachO::DylibCommand>(),
                                       IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment = Iter.dyn_cast<Kind::Segment64>()) {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment = Iter.dyn_cast<Kind::Segment>()) {
                    SegmentList.addSegment(*Segment, IsBigEndian);
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
            return Result.set(RunError::NoDyldInfo);
        }

        if (BindRange.empty() && LazyBindRange.empty() && WeakBindRange.empty())
        {
            return Result.set(RunError::NoOpcodes);
        }

        auto BindOpcodeList = std::vector<BindOpcodeInfo>();
        auto LazyBindOpcodeList = std::vector<BindOpcodeInfo>();
        auto WeakBindOpcodeList = std::vector<BindOpcodeInfo>();

        if (Opt.PrintNormal) {
            if (MachO.map().range().contains(BindRange)) {
                const auto BindListMap = ADT::MemoryMap(MachO.map(), BindRange);
                const auto BindList =
                    MachO::BindOpcodeList(BindListMap, Is64Bit);

                BindOpcodeList =
                    CollectBindOpcodeList<MachO::BindInfoKind::Normal>(
                        SegmentList, BindList, Is64Bit);
            }
        }

        if (Opt.PrintLazy) {
            if (MachO.map().range().contains(LazyBindRange)) {
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
            if (MachO.map().range().contains(WeakBindRange)) {
                const auto WeakBindListMap =
                    ADT::MemoryMap(MachO.map(), WeakBindRange);
                const auto WeakBindList =
                    MachO::WeakBindOpcodeList(WeakBindListMap, Is64Bit);

                WeakBindOpcodeList =
                    CollectBindOpcodeList<MachO::BindInfoKind::Weak>(
                        SegmentList, WeakBindList, Is64Bit);
            }
        }

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
                fputs("No (Normal) Bind Opcodes found\n", stderr);
            }
        }

        if (Opt.PrintLazy) {
            if (Opt.PrintNormal) {
                fputc('\n', OutFile);
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
                fputs("No Lazy-Bind Opcodes found\n", stderr);
            }
        }

        if (Opt.PrintWeak) {
            if (Opt.PrintNormal || Opt.PrintWeak) {
                fputc('\n', OutFile);
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
                fputs("No Weak-Bind Opcodes found\n", stderr);
            }
        }

        return Result.set(RunError::None);
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
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintBindOpcodeList::run()");
    }
}
