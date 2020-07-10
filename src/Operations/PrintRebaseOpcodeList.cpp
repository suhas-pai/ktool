//
//  src/Operations/PrintRebaseOpcodeList.cpp
//  stool
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

static void
PrintRebaseInfo(FILE *OutFile,
                const MachO::SharedLibraryInfoCollection &LibraryCollection,
                const RebaseOpcodeInfo &Info,
                bool Is64Bit,
                bool Verbose) noexcept
{
    if (!Verbose) {
        return;
    }

    fputs("Section: ", OutFile);
    PrintUtilsWriteMachOSegmentSectionPair(OutFile,
                                           Info.Segment,
                                           Info.Section,
                                           true);

    fputs("Address: ", OutFile);
    PrintUtilsPrintOffset(OutFile, Info.AddrInSeg, Is64Bit);
}

static inline void
GetSegmentAndSection(const MachO::SegmentInfoCollection &Collection,
                     int64_t SegmentIndex,
                     uint64_t Address,
                     const MachO::SegmentInfo **SegmentOut,
                     const MachO::SectionInfo **SectionOut) noexcept
{
    const auto *Segment = Collection.at(SegmentIndex);
    const auto *Section = static_cast<const MachO::SectionInfo *>(nullptr);

    if (Segment != nullptr) {
        const auto FullAddr = Segment->Memory.getBegin() + Address;
        Section = Segment->FindSectionContainingAddress(FullAddr);
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
            case MachO::RebaseByte::Opcode::SetTypeImm:
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

void
PrintRebaseOpcodeList(
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const std::vector<RebaseOpcodeInfo> &List,
    bool Is64Bit,
    const struct PrintRebaseOpcodeListOperation::Options &Options) noexcept
{
    auto Counter = static_cast<uint64_t>(1);
    const auto SizeDigitLength = PrintUtilsGetIntegerDigitLength(List.size());

    for (const auto &Iter : List) {
        constexpr auto LongestOpcodeLength =
            EnumHelper<MachO::RebaseByte::Opcode>::GetLongestAssocLength(
                MachO::RebaseByteOpcodeGetName);

        const auto &Byte = Iter.Byte;
        const auto &OpcodeName =
            MachO::RebaseByteOpcodeGetName(Byte.getOpcode());

        fprintf(Options.OutFile,
                "Rebase-Opcode %0*" PRIu64 ": %s",
                SizeDigitLength,
                Counter,
                OpcodeName.data());

        const auto PadSpacesAfterOpcode = [&]() noexcept {
            PrintUtilsRightPadSpaces(Options.OutFile,
                                     static_cast<int>(OpcodeName.length()),
                                     LongestOpcodeLength + 25);

            fputs("\t\t", Options.OutFile);
        };

        switch (Byte.getOpcode()) {
            case MachO::RebaseByte::Opcode::Done:
                fputc('\n', Options.OutFile);
                break;
            case MachO::RebaseByte::Opcode::SetTypeImm: {
                auto KindName = MachO::RebaseWriteKindGetName(Iter.Kind).data();
                if (KindName == nullptr) {
                    KindName = "<unrecognized>";
                }

                fprintf(Options.OutFile, "(%s)\n", KindName);
                break;
            }
            case MachO::RebaseByte::Opcode::SetSegmentAndOffsetUleb: {
                fprintf(Options.OutFile, "(%" PRIu32 ", ", Iter.SegmentIndex);
                PrintUtilsPrintOffset(Options.OutFile, Iter.AddrInSeg, Is64Bit);
                fputc(')', Options.OutFile);

                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                           Iter.Segment,
                                                           Iter.Section,
                                                           true);

                    if (Iter.Segment != nullptr) {
                        const auto FullAddr =
                            Iter.Segment->Memory.getBegin() + Iter.AddrInSeg;

                        fputc(' ', Options.OutFile);
                        PrintUtilsPrintOffset(Options.OutFile,
                                              FullAddr,
                                              Is64Bit);
                    }
                }

                fputc('\n', Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::AddAddrImmScaled:
            case MachO::RebaseByte::Opcode::AddAddrUleb: {
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.AddAddr);

                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintUtilsPrintOffset(Options.OutFile,
                                          Iter.AddrInSeg,
                                          Is64Bit);
                }

                fputc('\n', Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::DoRebaseAddAddrUleb:
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.AddAddr);

                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintRebaseInfo(Options.OutFile,
                                    LibraryCollection,
                                    Iter,
                                    Is64Bit,
                                    Options.Verbose);
                }

                fputc('\n', Options.OutFile);
                break;
            case MachO::RebaseByte::Opcode::DoRebaseImmTimes:
            case MachO::RebaseByte::Opcode::DoRebaseUlebTimes: {
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.Count);
                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintRebaseInfo(Options.OutFile,
                                    LibraryCollection,
                                    Iter,
                                    Is64Bit,
                                    Options.Verbose);
                }

                fputc('\n', Options.OutFile);
                break;
            }
            case MachO::RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb: {
                fprintf(Options.OutFile,
                        "(%" PRId64 ", %" PRIu64 ")",
                        Iter.Skip,
                        Iter.Count);

                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintRebaseInfo(Options.OutFile,
                                    LibraryCollection,
                                    Iter,
                                    Is64Bit,
                                    Options.Verbose);
                }

                fputc('\n', Options.OutFile);
                break;
            }
        }

        Counter++;
    }

    return;
}

int
PrintRebaseOpcodeListOperation::Run(const ConstMachOMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    auto FoundDyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);

    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto *DyldInfo =
            dyn_cast<MachO::DyldInfoCommand>(LoadCmd, IsBigEndian);
        
        if (DyldInfo == nullptr) {
            continue;
        }

        if (FoundDyldInfo != nullptr) {
            if (DyldInfo->RebaseOff != FoundDyldInfo->RebaseOff ||
                DyldInfo->RebaseSize != FoundDyldInfo->RebaseSize)
            {
                fputs("Provided file has multiple (conflicting) Rebase-Lists\n",
                      Options.ErrFile);
                return 1;
            }
        }

        if (DyldInfo->RebaseSize == 0) {
            fputs("Provided file has no Rebase-Opcodes\n", Options.ErrFile);
            return 0;
        }

        FoundDyldInfo = DyldInfo;
    }

    if (FoundDyldInfo == nullptr) {
        fputs("Provided file does not have a Dyld-Info Load Command\n",
              Options.ErrFile);
        return 0;
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

    const auto Is64Bit = Object.Is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    const auto RebaseOpcodeListOpt =
        FoundDyldInfo->GetRebaseOpcodeList(Object.getMap(),
                                           IsBigEndian,
                                           Is64Bit);

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

    PrintLineSpamWarning(Options.OutFile, Collection.size());
    PrintRebaseOpcodeList(SharedLibraryCollection,
                          SegmentCollection,
                          Collection,
                          Object.Is64Bit(),
                          Options);

    return 0;
}

struct PrintRebaseOpcodeListOperation::Options
PrintRebaseOpcodeListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                 int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (!Argument.IsOption()) {
            if (IndexOut != nullptr) {
                *IndexOut = Argv.indexOf(Argument);
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }
    }

    return Options;
}

void
PrintRebaseOpcodeListOperation::ParseOptions(const ArgvArray &Argv,
                                             int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintRebaseOpcodeListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}