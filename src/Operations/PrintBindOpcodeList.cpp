//
//  src/Operations/PrintBindOpcodeList.cpp
//  ktool
//
//  Created by Suhas Pai on 5/22/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintBindOpcodeList.h"

PrintBindOpcodeListOperation::PrintBindOpcodeListOperation() noexcept
: Operation(OpKind) {}

PrintBindOpcodeListOperation::PrintBindOpcodeListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static void PrintFlags(FILE *OutFile, MachO::BindSymbolFlags Flags) noexcept {
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
GetSegmentAndSection(const MachO::SegmentInfoCollection &Collection,
                     int64_t SegmentIndex,
                     uint64_t Address,
                     const MachO::SegmentInfo **SegmentOut,
                     const MachO::SectionInfo **SectionOut) noexcept
{
    const auto *Segment = Collection.atOrNull(SegmentIndex);
    const auto *Section = static_cast<const MachO::SectionInfo *>(nullptr);

    if (Segment != nullptr) {
        Section = Segment->FindSectionContainingRelativeAddress(Address);
    }

    *SegmentOut = Segment;
    *SectionOut = Section;
}

template <MachO::BindInfoKind BindKind, typename ListType>
std::vector<BindOpcodeInfo>
CollectBindOpcodeList(
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const ListType &List,
    bool Is64Bit) noexcept
{
    auto OpcodeInfo = BindOpcodeInfo();
    auto InfoList = std::vector<BindOpcodeInfo>();
    auto Counter = uint64_t();

    for (const auto &Iter : List) {
        const auto &Byte = Iter.getByte();
        const auto &IterInfo = Iter.getInfo();

        OpcodeInfo.Byte = Byte;
        switch (Byte.getOpcode()) {
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

                GetSegmentAndSection(SegmentCollection,
                                     OpcodeInfo.SegmentIndex,
                                     OpcodeInfo.AddrInSeg,
                                     &OpcodeInfo.Segment,
                                     &OpcodeInfo.Section);

                break;
            }
            case MachO::BindByte::Opcode::AddAddrUleb: {
                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    IterInfo.AddAddr,
                                    &OpcodeInfo.AddrInSeg))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    break;
                }

                OpcodeInfo.AddAddr = IterInfo.AddAddr;
                GetSegmentAndSection(SegmentCollection,
                                     OpcodeInfo.SegmentIndex,
                                     OpcodeInfo.AddrInSeg,
                                     &OpcodeInfo.Segment,
                                     &OpcodeInfo.Section);

                break;
            }
            case MachO::BindByte::Opcode::DoBind:
                InfoList.emplace_back(OpcodeInfo);
                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    PointerSize(Is64Bit),
                                    &OpcodeInfo.AddrInSeg))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    continue;
                }

                GetSegmentAndSection(SegmentCollection,
                                     OpcodeInfo.SegmentIndex,
                                     OpcodeInfo.AddrInSeg,
                                     &OpcodeInfo.Segment,
                                     &OpcodeInfo.Section);

                continue;
            case MachO::BindByte::Opcode::DoBindAddAddrUleb:
                InfoList.emplace_back(OpcodeInfo);
                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    IterInfo.AddAddr,
                                    &OpcodeInfo.AddrInSeg))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    continue;
                }

                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    PointerSize(Is64Bit),
                                    &OpcodeInfo.AddrInSeg))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    continue;
                }

                OpcodeInfo.AddAddr = IterInfo.AddAddr;
                GetSegmentAndSection(SegmentCollection,
                                     OpcodeInfo.SegmentIndex,
                                     OpcodeInfo.AddrInSeg,
                                     &OpcodeInfo.Segment,
                                     &OpcodeInfo.Section);

                continue;
            case MachO::BindByte::Opcode::DoBindAddAddrImmScaled: {
                OpcodeInfo.Scale = IterInfo.Scale;
                InfoList.emplace_back(OpcodeInfo);

                const auto PtrSize = PointerSize(Is64Bit);
                auto Add = uint64_t();

                if (DoesMultiplyAndAddOverflow(PtrSize,
                                               IterInfo.Scale,
                                               PtrSize,
                                               &Add))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    continue;
                }

                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    Add,
                                    &OpcodeInfo.AddrInSeg))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    continue;
                }

                GetSegmentAndSection(SegmentCollection,
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

                const auto PtrSize = PointerSize(Is64Bit);
                auto StepSize = int64_t();

                if (DoesAddOverflow(IterInfo.Skip, PtrSize, &StepSize)) {
                    OpcodeInfo.AddrInSegOverflows = true;
                    break;
                }

                auto Add = int64_t();
                if (DoesMultiplyOverflow(StepSize, IterInfo.Count, &Add)) {
                    OpcodeInfo.AddrInSegOverflows = true;
                    break;
                }

                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    Add,
                                    &OpcodeInfo.AddrInSeg))
                {
                    OpcodeInfo.AddrInSegOverflows = true;
                    break;
                }

                GetSegmentAndSection(SegmentCollection,
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
        Counter++;
    }

done:
    return InfoList;
}

template <MachO::BindInfoKind BindKind>
void
PrintBindOpcodeList(
    const char *Name,
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const std::vector<BindOpcodeInfo> &List,
    bool Is64Bit,
    const struct PrintBindOpcodeListOperation::Options &Options) noexcept
{
    auto Counter = static_cast<uint64_t>(1);
    const auto SizeDigitLength = PrintUtilsGetIntegerDigitLength(List.size());

    for (const auto &Iter : List) {
        const auto &Byte = Iter.Byte;
        const auto &OpcodeName = MachO::BindByteOpcodeGetName(Byte.getOpcode());
        const auto PtrSize = PointerSize(Is64Bit);

        fprintf(Options.OutFile,
                "%s %0*" PRIu64 ": %s",
                Name,
                SizeDigitLength,
                Counter,
                OpcodeName.data());

        const auto PrintAddressInfo = [&](uint64_t Add = 0) noexcept {
            if ((&Iter) != (&List.back() + 1)) {
                if ((&Iter)[1].AddrInSegOverflows) {
                    fputs(" (Overflows)", Options.OutFile);
                    return;
                }
            }

            PrintUtilsWriteOffset32Or64(Options.OutFile,
                                        Is64Bit,
                                        Iter.AddrInSeg + Add,
                                        false,
                                        " - <Segment-Address: ",
                                        ", ");

            const auto FullAddr =
                Iter.Segment->getMemoryRange().getBegin() +
                Iter.AddrInSeg + Add;

            PrintUtilsWriteOffset32Or64(Options.OutFile,
                                        Is64Bit,
                                        FullAddr,
                                        false,
                                        "Full-Address: ",
                                        ">");
        };

        switch (Byte.getOpcode()) {
            case MachO::BindByte::Opcode::Done:
                fputc('\n', Options.OutFile);
                if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
                    goto done;
                }

                break;
            case MachO::BindByte::Opcode::SetDylibOrdinalImm:
            case MachO::BindByte::Opcode::SetDylibOrdinalUleb:
            case MachO::BindByte::Opcode::SetDylibSpecialImm:
                fprintf(Options.OutFile, "(%" PRId64, Iter.DylibOrdinal);
                if (Options.Verbose) {
                    fputs(" - ", Options.OutFile);
                    OperationCommon::PrintDylibOrdinalPath(Options.OutFile,
                                                           LibraryCollection,
                                                           Iter.DylibOrdinal);
                }

                fputs(")\n", Options.OutFile);
                break;
            case MachO::BindByte::Opcode::SetSymbolTrailingFlagsImm:
                fprintf(Options.OutFile,
                        "(\"%s\", 0x%" PRIx8,
                        Iter.SymbolName.data(),
                        Iter.Flags.value());

                if (Options.Verbose) {
                    if (!Iter.Flags.empty()) {
                        PrintFlags(Options.OutFile, Iter.Flags);
                    }
                }

                fputs(")\n", Options.OutFile);
                break;

            case MachO::BindByte::Opcode::SetKindImm: {
                auto KindName =
                    MachO::BindWriteKindGetName(Iter.WriteKind).data();

                if (KindName == nullptr) {
                    KindName = "<unrecognized>";
                }

                fprintf(Options.OutFile, "(%s)\n", KindName);
                break;
            }
            case MachO::BindByte::Opcode::SetAddendSleb:
                PrintUtilsWriteOffset32Or64(Options.OutFile,
                                            Is64Bit,
                                            Iter.Addend,
                                            false,
                                            "(",
                                            ")\n");

                break;
            case MachO::BindByte::Opcode::SetSegmentAndOffsetUleb: {
                fprintf(Options.OutFile, "(%" PRId64, Iter.SegmentIndex);
                if (Options.Verbose) {
                    PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                           Iter.Segment,
                                                           Iter.Section,
                                                           false,
                                                           " - <",
                                                           ">");
                }

                PrintUtilsWriteOffset32Or64(Options.OutFile,
                                            Is64Bit,
                                            Iter.AddrInSeg,
                                            false,
                                            ", ");

                if (Options.Verbose) {
                    if (Iter.Segment != nullptr) {
                        const auto FullAddr =
                            Iter.Segment->getMemoryRange().getBegin() +
                            Iter.AddrInSeg;

                        PrintUtilsWriteOffset32Or64(Options.OutFile,
                                                    Is64Bit,
                                                    FullAddr,
                                                    false,
                                                    " <Full Address: ",
                                                    ">");
                    }
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::AddAddrUleb: {
                fprintf(Options.OutFile, "(%" PRId64, Iter.AddAddr);
                if (Options.Verbose) {
                    PrintAddressInfo();
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::DoBind:
                fputc('\n', Options.OutFile);
                break;
            case MachO::BindByte::Opcode::DoBindAddAddrUleb:
                fprintf(Options.OutFile, "(%" PRId64, Iter.AddAddr);
                if (Options.Verbose) {
                    PrintAddressInfo(PtrSize);
                }

                fputs(")\n", Options.OutFile);
                break;
            case MachO::BindByte::Opcode::DoBindAddAddrImmScaled: {
                fprintf(Options.OutFile, "(%" PRId64, Iter.Scale);
                if (Options.Verbose) {
                    PrintAddressInfo(Iter.Scale * PtrSize);
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                fprintf(Options.OutFile,
                        "(Skip: %" PRId64 ", Count: %" PRIu64,
                        Iter.Skip,
                        Iter.Count);

                if (Options.Verbose) {
                    const auto Add =
                        (Iter.Skip * Iter.Count) + (Iter.Count * PtrSize);

                    PrintAddressInfo(Add);
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::Threaded:
                fputc('\n', Options.OutFile);
                break;
        }

        Counter++;
    }

done:
    return;
}

static int
PrintOpcodeList(
    const ConstMachOMemoryObject &Object,
    const ConstMemoryMap &Map,
    const struct PrintBindOpcodeListOperation::Options &Options) noexcept
{
    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    auto DyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);
    const auto GetDyldInfoResult =
        OperationCommon::GetDyldInfoCommand(Options.ErrFile,
                                            LoadCmdStorage,
                                            DyldInfo);

    if (GetDyldInfoResult != 0) {
        return GetDyldInfoResult;
    }

    if (DyldInfo->BindSize == 0 &&
        DyldInfo->LazyBindSize == 0 &&
        DyldInfo->WeakBindSize == 0)
    {
        fputs("Provided file has no Bind-Opcodes\n", Options.ErrFile);
        return 1;
    }

    auto LibraryCollectionError =
        MachO::SharedLibraryInfoCollection::Error::None;
    const auto SharedLibraryCollection =
        MachO::SharedLibraryInfoCollection::Open(LoadCmdStorage,
                                                 &LibraryCollectionError);

    switch (LibraryCollectionError) {
        case MachO::SharedLibraryInfoCollection::Error::None:
        case MachO::SharedLibraryInfoCollection::Error::InvalidPath:
            break;
    }

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    auto ShouldPrintBindList = Options.PrintNormal;
    auto ShouldPrintLazyBindList = Options.PrintLazy;
    auto ShouldPrintWeakBindList = Options.PrintWeak;

    auto BindOpcodeError = MachO::SizeRangeError::None;
    auto LazyBindOpcodeError = MachO::SizeRangeError::None;
    auto WeakBindOpcodeError = MachO::SizeRangeError::None;

    auto BindOpcodeList = std::vector<BindOpcodeInfo>();
    auto LazyBindOpcodeList = std::vector<BindOpcodeInfo>();
    auto WeakBindOpcodeList = std::vector<BindOpcodeInfo>();

    if (ShouldPrintBindList) {
        const auto BindOpcodeListOpt =
            DyldInfo->GetBindOpcodeList(Map, IsBigEndian, Is64Bit);

        BindOpcodeError = BindOpcodeListOpt.getError();
        if (BindOpcodeError == MachO::SizeRangeError::None) {
            BindOpcodeList =
                CollectBindOpcodeList<MachO::BindInfoKind::Normal>(
                    SharedLibraryCollection,
                    SegmentCollection,
                    BindOpcodeListOpt.getRef(),
                    Is64Bit);
        }
    }

    if (ShouldPrintLazyBindList) {
        if (Options.PrintNormal && !ShouldPrintBindList) {
            fputc('\n', Options.OutFile);
        }

        const auto LazyBindOpcodeListOpt =
            DyldInfo->GetLazyBindOpcodeList(Map, IsBigEndian, Is64Bit);

        LazyBindOpcodeError = LazyBindOpcodeListOpt.getError();
        if (LazyBindOpcodeError == MachO::SizeRangeError::None) {
            LazyBindOpcodeList =
                CollectBindOpcodeList<MachO::BindInfoKind::Lazy>(
                    SharedLibraryCollection,
                    SegmentCollection,
                    LazyBindOpcodeListOpt.getRef(),
                    Is64Bit);
        }
    }

    if (ShouldPrintWeakBindList) {
        const auto WeakBindOpcodeListOpt =
            DyldInfo->GetWeakBindOpcodeList(Map, IsBigEndian, Is64Bit);

        WeakBindOpcodeError = WeakBindOpcodeListOpt.getError();
        if (WeakBindOpcodeError == MachO::SizeRangeError::None) {
            WeakBindOpcodeList =
                CollectBindOpcodeList<MachO::BindInfoKind::Lazy>(
                    SharedLibraryCollection,
                    SegmentCollection,
                    WeakBindOpcodeListOpt.getRef(),
                    Is64Bit);
        }
    }

    const auto TotalLineCount =
        BindOpcodeList.size() +
        LazyBindOpcodeList.size() +
        WeakBindOpcodeList.size();

    Operation::PrintLineSpamWarning(Options.OutFile, TotalLineCount);
    if (ShouldPrintBindList) {
        switch (BindOpcodeError) {
            case MachO::SizeRangeError::None:
                break;
            case MachO::SizeRangeError::Empty:
                fputs("No Bind Info\n", Options.ErrFile);
                ShouldPrintBindList = false;

                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fputs("Bind List goes past end-of-file\n", Options.ErrFile);
                ShouldPrintBindList = false;

                break;
        }

        if (ShouldPrintBindList) {
            PrintBindOpcodeList<MachO::BindInfoKind::Normal>(
                "Bind",
                SharedLibraryCollection,
                SegmentCollection,
                BindOpcodeList,
                Object.is64Bit(),
                Options);
        }
    }

    if (ShouldPrintLazyBindList) {
        if (ShouldPrintBindList) {
            fputc('\n', Options.OutFile);
        }

        switch (LazyBindOpcodeError) {
            case MachO::SizeRangeError::None:
                break;
            case MachO::SizeRangeError::Empty:
                fputs("No Lazy-Bind Info\n", Options.ErrFile);
                ShouldPrintLazyBindList = false;

                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fputs("Lazy-Bind List goes past end-of-file\n",
                      Options.ErrFile);

                ShouldPrintLazyBindList = false;
                break;
        }

        if (ShouldPrintLazyBindList) {
            PrintBindOpcodeList<MachO::BindInfoKind::Lazy>(
                "Lazy-Bind",
                SharedLibraryCollection,
                SegmentCollection,
                LazyBindOpcodeList,
                Object.is64Bit(),
                Options);
        }
    }

    if (ShouldPrintWeakBindList) {
        if (ShouldPrintBindList || ShouldPrintLazyBindList) {
            fputc('\n', Options.OutFile);
        }

        switch (WeakBindOpcodeError) {
            case MachO::SizeRangeError::None:
                break;
            case MachO::SizeRangeError::Empty:
                fputs("No Weak-Bind Info\n", Options.ErrFile);
                ShouldPrintWeakBindList = false;

                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fputs("Weak-Bind List goes past end-of-file\n",
                      Options.ErrFile);

                ShouldPrintWeakBindList = false;
                break;
        }

        if (ShouldPrintWeakBindList) {
            PrintBindOpcodeList<MachO::BindInfoKind::Weak>(
                "Weak-Bind",
                SharedLibraryCollection,
                SegmentCollection,
                WeakBindOpcodeList,
                Object.is64Bit(),
                Options);
        }
    }

    return 0;
}

int
PrintBindOpcodeListOperation::Run(const ConstDscImageMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    return PrintOpcodeList(Object, Object.getDscMap(), Options);
}

int
PrintBindOpcodeListOperation::Run(const ConstMachOMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    return PrintOpcodeList(Object, Object.getMap(), Options);
}

struct PrintBindOpcodeListOperation::Options
PrintBindOpcodeListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                               int *IndexOut) noexcept
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--only-normal") == 0) {
            Options.PrintAll = 0;
            Options.PrintNormal = true;
        } else if (strcmp(Argument, "--only-lazy") == 0) {
            Options.PrintAll = 0;
            Options.PrintLazy = true;
        } else if (strcmp(Argument, "--only-weak") == 0) {
            Options.PrintAll = 0;
            Options.PrintWeak = true;
        } else if (!Argument.isOption()) {
            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }

        Index++;
    }

    if (IndexOut != nullptr) {
        *IndexOut = Index;
    }

    return Options;
}

int PrintBindOpcodeListOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintBindOpcodeListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::DscImage:
            return Run(cast<ObjectKind::DscImage>(Object), Options);
        case ObjectKind::FatMachO:
        case ObjectKind::DyldSharedCache:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}
