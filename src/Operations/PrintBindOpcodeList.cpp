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
    fputc('(', OutFile);

    if (!Flags.empty()) {
        const auto HasNonWeakDefinition = Flags.HasNonWeakDefinition();
        if (HasNonWeakDefinition) {
            fputs("Has Non-Weak Definition", OutFile);
        }

        if (Flags.IsWeakImport()) {
            if (HasNonWeakDefinition) {
                fputs(", ", OutFile);
            }

            fputs("Weak-Import", OutFile);
        }
    } else {
        fputs("None", OutFile);
    }

    fputc(')', OutFile);
}

static
void PrintSymbolName(FILE *OutFile, const std::string_view &Name) noexcept {
    if (Name.empty()) {
        fputs("\"\"", OutFile);
        return;
    }

    fprintf(OutFile, "\"%s\"", Name.data());
}

struct BindOpcodeInfo : public MachO::BindOpcodeIterateInfo {
    MachO::BindByte Byte;

    uint64_t AddrInSeg;
    bool AddrInSegOverflows : 1;

    const MachO::SegmentInfo *Segment;
    const MachO::SectionInfo *Section;
};

static void
PrintBindInfo(FILE *OutFile,
              const MachO::SharedLibraryInfoCollection &LibraryCollection,
              const BindOpcodeInfo &Info,
              bool Is64Bit) noexcept
{
    PrintUtilsWriteMachOSegmentSectionPair(OutFile,
                                           Info.Segment,
                                           Info.Section,
                                           true,
                                           "Section: ");

    PrintUtilsWriteOffset(OutFile,
                          Info.AddrInSeg,
                          Is64Bit,
                          "Address: ",
                          ", ");

    OperationCommon::PrintDylibOrdinalInfo(OutFile,
                                           LibraryCollection,
                                           Info.DylibOrdinal,
                                           true);

    fputs(", Symbol: ", OutFile);
    PrintSymbolName(OutFile, Info.SymbolName);
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

    auto AddressOverflows = false;
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

            case MachO::BindByte::Opcode::SetTypeImm:
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
                    AddressOverflows = true;
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
            case MachO::BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
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
        constexpr auto LongestOpcodeLength =
            EnumHelper<MachO::BindByte::Opcode>::GetLongestAssocLength(
                MachO::BindByteOpcodeGetName);

        const auto &Byte = Iter.Byte;
        const auto &OpcodeName = MachO::BindByteOpcodeGetName(Byte.getOpcode());

        fprintf(Options.OutFile,
                "%s %0*" PRIu64 ": %s",
                Name,
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
            case MachO::BindByte::Opcode::Done:
                fputc('\n', Options.OutFile);
                if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
                    goto done;
                }

                break;
            case MachO::BindByte::Opcode::SetDylibOrdinalImm:
            case MachO::BindByte::Opcode::SetDylibOrdinalUleb:
            case MachO::BindByte::Opcode::SetDylibSpecialImm:
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.DylibOrdinal);
                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    OperationCommon::PrintDylibOrdinalPath(Options.OutFile,
                                                           LibraryCollection,
                                                           Iter.DylibOrdinal);
                }

                fputc('\n', Options.OutFile);
                break;
            case MachO::BindByte::Opcode::SetSymbolTrailingFlagsImm:
                fprintf(Options.OutFile,
                        "(%s, %" PRIu8 ")",
                        Iter.SymbolName.data(),
                        Iter.Flags.value());

                if (Options.Verbose && !Iter.Flags.empty()) {
                    PadSpacesAfterOpcode();
                    fputc('(', Options.OutFile);
                    PrintFlags(Options.OutFile, Iter.Flags);
                    fputc(')', Options.OutFile);
                }

                fputc('\n', Options.OutFile);
                break;

            case MachO::BindByte::Opcode::SetTypeImm: {
                auto KindName = MachO::BindWriteKindGetName(Iter.WriteKind).data();
                if (KindName == nullptr) {
                    KindName = "<unrecognized>";
                }

                fprintf(Options.OutFile, "(%s)\n", KindName);
                break;
            }
            case MachO::BindByte::Opcode::SetAddendSleb:
                PrintUtilsWriteOffset(Options.OutFile,
                                      Iter.Addend,
                                      Is64Bit,
                                      "(",
                                      ")\n");

                break;
            case MachO::BindByte::Opcode::SetSegmentAndOffsetUleb: {
                fprintf(Options.OutFile, "(%" PRId64 ", ", Iter.SegmentIndex);
                PrintUtilsWriteOffset(Options.OutFile,
                                      Iter.AddrInSeg,
                                      Is64Bit,
                                      "",
                                      ")");

                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                           Iter.Segment,
                                                           Iter.Section,
                                                           true);

                    if (Iter.Segment != nullptr) {
                        const auto FullAddr =
                            Iter.Segment->Memory.getBegin() + Iter.AddrInSeg;

                        PrintUtilsWriteOffset(Options.OutFile,
                                              FullAddr,
                                              Is64Bit,
                                              " ");
                    }
                }

                fputc('\n', Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::AddAddrUleb: {
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.AddAddr);
                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintUtilsWriteOffset(Options.OutFile,
                                          Iter.AddrInSeg,
                                          Is64Bit);
                }

                fputc('\n', Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::DoBind:
                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintBindInfo(Options.OutFile,
                                  LibraryCollection,
                                  Iter,
                                  Is64Bit);
                }

                fputc('\n', Options.OutFile);
                break;
            case MachO::BindByte::Opcode::DoBindAddAddrUleb:
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.AddAddr);
                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintBindInfo(Options.OutFile,
                                  LibraryCollection,
                                  Iter,
                                  Is64Bit);
                }

                fputc('\n', Options.OutFile);
                break;
            case MachO::BindByte::Opcode::DoBindAddAddrImmScaled: {
                fprintf(Options.OutFile, "(%" PRId64 ")", Iter.Scale);
                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintBindInfo(Options.OutFile,
                                  LibraryCollection,
                                  Iter,
                                  Is64Bit);
                }

                fputc('\n', Options.OutFile);
                break;
            }
            case MachO::BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                fprintf(Options.OutFile,
                        "(%" PRId64 ", %" PRIu64 ")",
                        Iter.Skip,
                        Iter.Count);

                if (Options.Verbose) {
                    PadSpacesAfterOpcode();
                    PrintBindInfo(Options.OutFile,
                                  LibraryCollection,
                                  Iter,
                                  Is64Bit);
                }

                fputc('\n', Options.OutFile);
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
    const auto IsBigEndian = Object.IsBigEndian();
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

    const auto Is64Bit = Object.Is64Bit();
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
                Object.Is64Bit(),
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
                Object.Is64Bit(),
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
                Object.Is64Bit(),
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
PrintBindOpcodeListOperation::ParseOptions(const ArgvArray &Argv,
                                           int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintBindOpcodeListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::DscImage:
            return Run(cast<ObjectKind::DscImage>(Object).toConst(), Options);
        case ObjectKind::FatMachO:
        case ObjectKind::DyldSharedCache:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}
