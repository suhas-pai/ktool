//
//  src/Operations/PrintRebaseOpcodeList.cpp
//  ktool
//
//  Created by Suhas Pai on 5/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintRebaseOpcodeList.h"

PrintRebaseOpcodeListOperation::PrintRebaseOpcodeListOperation() noexcept
: Operation(OpKind) {}

PrintRebaseOpcodeListOperation::PrintRebaseOpcodeListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

struct RebaseOpcodeInfo : public MachO::RebaseOpcodeIterateInfo {
    MachO::RebaseByte Byte;

    uint64_t AddrInSeg;
    bool AddrInSegOverflows : 1;

    const MachO::SegmentInfo *Segment;
    const MachO::SectionInfo *Section;
};

static inline void
GetSegmentAndSection(const MachO::SegmentInfoCollection &Collection,
                     const int64_t SegmentIndex,
                     const uint64_t Address,
                     const MachO::SegmentInfo **const SegmentOut,
                     const MachO::SectionInfo **const SectionOut) noexcept
{
    const auto *Segment = Collection.atOrNull(SegmentIndex);
    const auto *Section = static_cast<const MachO::SectionInfo *>(nullptr);

    if (Segment != nullptr) {
        Section = Segment->FindSectionContainingAddress(Address);
    }

    *SegmentOut = Segment;
    *SectionOut = Section;
}

template <typename ListType>
std::vector<RebaseOpcodeInfo>
CollectRebaseOpcodeList(
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const ListType &List,
    bool Is64Bit) noexcept
{
    auto OpcodeInfo = RebaseOpcodeInfo();
    auto InfoList = std::vector<RebaseOpcodeInfo>();

    auto AddressOverflows = false;
    auto Counter = uint64_t();

    for (const auto &Iter : List) {
        const auto &Byte = Iter.getByte();
        const auto &IterInfo = Iter.getInfo();

        OpcodeInfo.Byte = Byte;
        switch (Byte.getOpcode()) {
            case MachO::RebaseByte::Opcode::Done:
                break;
            case MachO::RebaseByte::Opcode::SetKindImm:
                OpcodeInfo.Kind = IterInfo.Kind;
                break;
            case MachO::RebaseByte::Opcode::SetSegmentAndOffsetUleb: {
                OpcodeInfo.AddrInSeg = IterInfo.SegOffset;
                OpcodeInfo.SegmentIndex = IterInfo.SegmentIndex;

                GetSegmentAndSection(SegmentCollection,
                                     OpcodeInfo.SegmentIndex,
                                     OpcodeInfo.AddrInSeg,
                                     &OpcodeInfo.Segment,
                                     &OpcodeInfo.Section);

                break;
            }
            case MachO::RebaseByte::Opcode::AddAddrImmScaled:
                OpcodeInfo.AddAddr = OpcodeInfo.Scale * PointerSize(Is64Bit);
            case MachO::RebaseByte::Opcode::AddAddrUleb: {
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
            case MachO::RebaseByte::Opcode::DoRebaseAddAddrUleb:
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
            case MachO::RebaseByte::Opcode::DoRebaseImmTimes:
            case MachO::RebaseByte::Opcode::DoRebaseUlebTimes: {
                OpcodeInfo.Count = IterInfo.Count;
                InfoList.emplace_back(OpcodeInfo);

                const auto PtrSize = PointerSize(Is64Bit);
                auto Add = uint64_t();

                if (DoesMultiplyOverflow(PtrSize, OpcodeInfo.Count, &Add)) {
                    AddressOverflows = true;
                    continue;
                }

                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    Add,
                                    &OpcodeInfo.AddrInSeg))
                {
                    AddressOverflows = true;
                    continue;
                }

                GetSegmentAndSection(SegmentCollection,
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

                const auto PtrSize = PointerSize(Is64Bit);
                auto StepSize = int64_t();

                if (DoesAddOverflow(IterInfo.Skip, PtrSize, &StepSize)) {
                    AddressOverflows = true;
                    break;
                }

                auto Add = int64_t();
                if (DoesMultiplyOverflow(StepSize, IterInfo.Count, &Add)) {
                    AddressOverflows = true;
                    break;
                }

                if (DoesAddOverflow(OpcodeInfo.AddrInSeg,
                                    Add,
                                    &OpcodeInfo.AddrInSeg))
                {
                    AddressOverflows = true;
                    break;
                }

                GetSegmentAndSection(SegmentCollection,
                                     OpcodeInfo.SegmentIndex,
                                     OpcodeInfo.AddrInSeg,
                                     &OpcodeInfo.Segment,
                                     &OpcodeInfo.Section);

                continue;
            }
        }

        InfoList.emplace_back(OpcodeInfo);
        Counter++;
    }

    return InfoList;
}

static int
PrintRebaseOpcodeList(
    const ConstMachOMemoryObject &Object,
    const ConstMemoryMap &Map,
    const struct PrintRebaseOpcodeListOperation::Options &Options) noexcept
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

    const auto RebaseOpcodeListOpt =
        DyldInfo->GetRebaseOpcodeList(Map, IsBigEndian, Is64Bit);

    switch (RebaseOpcodeListOpt.getError()) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Empty:
            fputs("No Rebase Info\n", Options.ErrFile);
            return 0;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Rebase-Info goes past end-of-file\n", Options.ErrFile);
            return 1;
    }

    const auto Collection =
        CollectRebaseOpcodeList(SharedLibraryCollection,
                                SegmentCollection,
                                RebaseOpcodeListOpt.getRef(),
                                Is64Bit);

    Operation::PrintLineSpamWarning(Options.OutFile, Collection.size());

    const auto PtrSize = PointerSize(Is64Bit);
    const auto SizeDigitLength =
        PrintUtilsGetIntegerDigitLength(Collection.size());

    auto Counter = static_cast<uint64_t>(1);
    for (const auto &Iter : Collection) {
        const auto &Byte = Iter.Byte;
        const auto &OpcodeName =
            MachO::RebaseByteOpcodeGetName(Byte.getOpcode());

        fprintf(Options.OutFile,
                "Rebase-Opcode %0*" PRIu64 ": %s",
                SizeDigitLength,
                Counter,
                OpcodeName.data());

        const auto PrintAddressInfo = [&](uint64_t Add = 0) noexcept {
            if ((&Iter) != (&Collection.back() + 1)) {
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
                                        true,
                                        "Full-Address: ",
                                        ">");
        };

        switch (Byte.getOpcode()) {
            case MachO::RebaseByte::Opcode::Done:
                fputc('\n', Options.OutFile);
                break;
            case MachO::RebaseByte::Opcode::SetKindImm: {
                const auto KindName =
                    MachO::RebaseWriteKindGetName(Iter.Kind).data() ?:
                    "<unrecognized>";

                fprintf(Options.OutFile, "(%s)\n", KindName);
                break;
            }
            case MachO::RebaseByte::Opcode::SetSegmentAndOffsetUleb: {
                fprintf(Options.OutFile, "(%" PRIu32, Iter.SegmentIndex);
                if (Options.Verbose) {
                    PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                           Iter.Segment,
                                                           Iter.Section,
                                                           false,
                                                           " - <",
                                                           ">, ");
                }

                PrintUtilsWriteOffset32Or64(Options.OutFile,
                                            Is64Bit,
                                            Iter.AddrInSeg);

                if (Options.Verbose) {
                    if (Iter.Segment != nullptr) {
                        const auto FullAddr =
                            Iter.Segment->getMemoryRange().getBegin() +
                            Iter.AddrInSeg;

                        PrintUtilsWriteOffset32Or64(Options.OutFile,
                                                    Is64Bit,
                                                    FullAddr);
                    }
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::AddAddrImmScaled: {
                fprintf(Options.OutFile, "(%" PRId64, Iter.Scale);
                if (Options.Verbose) {
                    PrintAddressInfo(Iter.Scale * PtrSize);
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::AddAddrUleb: {
                fprintf(Options.OutFile, "(%" PRId64, Iter.AddAddr);
                if (Options.Verbose) {
                    PrintAddressInfo();
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::DoRebaseAddAddrUleb:
                fprintf(Options.OutFile, "(%" PRId64, Iter.AddAddr);
                if (Options.Verbose) {
                    PrintAddressInfo(Iter.AddAddr);
                }

                fputs(")\n", Options.OutFile);
                break;
            case MachO::RebaseByte::Opcode::DoRebaseImmTimes:
            case MachO::RebaseByte::Opcode::DoRebaseUlebTimes: {
                fprintf(Options.OutFile, "(%" PRId64, Iter.Count);
                if (Options.Verbose) {
                    PrintAddressInfo(Iter.Count * PtrSize);
                }

                fputs(")\n", Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb: {
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
        }

        Counter++;
    }

    return 0;
}

int
PrintRebaseOpcodeListOperation::Run(const ConstDscImageMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    return PrintRebaseOpcodeList(Object, Object.getDscMap(), Options);
}

int
PrintRebaseOpcodeListOperation::Run(const ConstMachOMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    return PrintRebaseOpcodeList(Object, Object.getMap(), Options);
}

struct PrintRebaseOpcodeListOperation::Options
PrintRebaseOpcodeListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                 int *IndexOut) noexcept
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
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

int
PrintRebaseOpcodeListOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintRebaseOpcodeListOperation::Run(const MemoryObject &Object) const noexcept {
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
