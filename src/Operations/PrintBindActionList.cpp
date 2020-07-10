//
//  src/Operations/PrintBindActionList.cpp
//  stool
//
//  Created by Suhas Pai on 5/28/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintBindActionList.h"

PrintBindActionListOperation::PrintBindActionListOperation() noexcept
: Operation(OpKind) {}

PrintBindActionListOperation::PrintBindActionListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static int
CompareActionsBySortKind(
    const MachO::BindActionInfo &Lhs,
    const MachO::BindActionInfo &Rhs,
    const PrintBindActionListOperation::Options::SortKind SortKind) noexcept
{
    switch (SortKind) {
        case PrintBindActionListOperation::Options::SortKind::None:
            assert(0 && "Unrecognized Sort-Type");
        case PrintBindActionListOperation::Options::SortKind::ByName:
            return Lhs.SymbolName.compare(Rhs.SymbolName);
        case PrintBindActionListOperation::Options::SortKind::ByDylibOrdinal:
            if (Lhs.DylibOrdinal < Rhs.DylibOrdinal) {
                return -1;
            } else if (Lhs.DylibOrdinal == Rhs.DylibOrdinal) {
                return 0;
            }

            return 1;
        case PrintBindActionListOperation::Options::SortKind::ByKind: {
            const auto LhsKind = static_cast<uint8_t>(Lhs.WriteKind);
            const auto RhsKind = static_cast<uint8_t>(Rhs.WriteKind);

            if (LhsKind < RhsKind) {
                return -1;
            } else if (LhsKind == RhsKind) {
                return 0;
            }

            return 1;
        }
    }

    return false;
}

template <MachO::BindInfoKind BindKind>
static void
PrintBindAction(
    const char *Name,
    uint64_t Counter,
    int SizeDigitLength,
    const MachO::BindActionInfo &Action,
    uint64_t LongestBindSymbolLength,
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const MachO::SegmentInfoCollection &SegmentCollection,
    bool Is64Bit,
    const struct PrintBindActionListOperation::Options &Options) noexcept
{
    fprintf(Options.OutFile,
            "%s Action %0*" PRIu64 ": ",
            Name,
            SizeDigitLength,
            Counter);

    if (const auto *Segment = SegmentCollection.at(Action.SegmentIndex)) {
        const auto FullAddr = Segment->Memory.getBegin() + Action.AddrInSeg;
        const auto Section = Segment->FindSectionContainingAddress(FullAddr);

        PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                               Segment,
                                               Section,
                                               true);

        fputc(' ', Options.OutFile);
        PrintUtilsPrintOffset(Options.OutFile, FullAddr, Is64Bit);

        if (Action.Addend != 0) {
            fputs(" + ", Options.OutFile);
            PrintUtilsPrintOffset(Options.OutFile, Action.Addend, Is64Bit);
        }
    } else {
        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("<out-of-bounds>", Options.OutFile),
                                 OperationCommon::SegmentSectionPairMaxLength);
    }

    if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
        fprintf(Options.OutFile,
                " %" PRINTF_RIGHTPAD_FMT "s",
                static_cast<int>(MachO::BindWriteKindGetLongestDescription()),
                MachO::BindWriteKindGetDescription(Action.WriteKind).data());
    }

    PrintUtilsRightPadSpaces(Options.OutFile,
                             fprintf(Options.OutFile,
                                     " \"%s\"",
                                     Action.SymbolName.data()),
                             static_cast<int>(LongestBindSymbolLength) + 3);

    if constexpr (BindKind != MachO::BindInfoKind::Weak) {
        fputc(' ', Options.OutFile);
        OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                               LibraryCollection,
                                               Action.DylibOrdinal,
                                               Options.Verbose);
    }

    fputc('\n', Options.OutFile);
}

template <MachO::BindInfoKind BindKind>
static void
PrintBindActionList(
    const char *Name,
    const std::vector<MachO::BindActionInfo> &List,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    bool Is64Bit,
    const struct PrintBindActionListOperation::Options &Options) noexcept
{
    if (List.empty()) {
        fprintf(Options.OutFile, "No %s Info\n", Name);
        return;
    }

    auto LongestBindSymbolLength = uint64_t();
    for (const auto &Action : List) {
        if (LongestBindSymbolLength < Action.SymbolName.length()) {
            LongestBindSymbolLength = Action.SymbolName.length();
        }
    }

    switch (List.size()) {
        case 0:
            assert(0 && "Bind-Action List shouldn't be empty at this point");
        case 1:
            fprintf(Options.OutFile, "1 %s Action:\n", Name);
            break;
        default:
            fprintf(Options.OutFile,
                    "%" PRIuPTR " %s Actions:\n",
                    List.size(),
                    Name);
            break;
    }

    auto Counter = 1ull;
    const auto SizeDigitLength = PrintUtilsGetIntegerDigitLength(List.size());

    for (const auto &Action : List) {
        PrintBindAction<BindKind>(Name,
                                  Counter,
                                  SizeDigitLength,
                                  Action,
                                  LongestBindSymbolLength,
                                  LibraryCollection,
                                  SegmentCollection,
                                  Is64Bit,
                                  Options);
        Counter++;
    }
}

int
PrintBindActionListOperation::Run(const ConstMachOMemoryObject &Object,
                                  const struct Options &Options) noexcept
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

    auto BindActionInfoList = std::vector<MachO::BindActionInfo>();
    auto LazyBindActionInfoList = std::vector<MachO::BindActionInfo>();
    auto WeakBindActionInfoList = std::vector<MachO::BindActionInfo>();

    auto BindActionListRangeError = MachO::SizeRangeError();
    auto LazyBindActionListRangeError = MachO::SizeRangeError();
    auto WeakBindActionListRangeError = MachO::SizeRangeError();

    auto BindActionListError = MachO::BindOpcodeParseError();
    auto LazyBindActionListError = MachO::BindOpcodeParseError();
    auto WeakBindActionListError = MachO::BindOpcodeParseError();

    const auto Comparator =
        [&](const MachO::BindActionInfo &Lhs, const MachO::BindActionInfo &Rhs)
    {
        for (const auto &SortKind : Options.SortKindList) {
            const auto CmpResult = CompareActionsBySortKind(Lhs, Rhs, SortKind);
            if (CmpResult != 0) {
                return (CmpResult < 0);
            }

            continue;
        }

        return false;
    };

    if (ShouldPrintBindList) {
        const auto BindActionListOpt =
            DyldInfo->GetBindActionList(Object.getConstMap(),
                                        SegmentCollection,
                                        IsBigEndian,
                                        Is64Bit);

        BindActionListRangeError = BindActionListOpt.getError();
        if (BindActionListRangeError == MachO::SizeRangeError::None) {
            const auto BindActionList = BindActionListOpt.getRef();
            BindActionListError = BindActionList.GetAsList(BindActionInfoList);

            if (!Options.SortKindList.empty()) {
                std::sort(BindActionInfoList.begin(),
                          BindActionInfoList.end(),
                          Comparator);
            }
        }
    }

    if (ShouldPrintLazyBindList) {
        const auto LazyBindActionListOpt =
            DyldInfo->GetLazyBindActionList(Object.getConstMap(),
                                            SegmentCollection,
                                            IsBigEndian,
                                            Is64Bit);

        LazyBindActionListRangeError = LazyBindActionListOpt.getError();
        if (LazyBindActionListRangeError == MachO::SizeRangeError::None) {
            const auto LazyBindActionList = LazyBindActionListOpt.getRef();
            LazyBindActionListError =
                LazyBindActionList.GetAsList(LazyBindActionInfoList);

            if (!Options.SortKindList.empty()) {
                std::sort(LazyBindActionInfoList.begin(),
                          LazyBindActionInfoList.end(),
                          Comparator);
            }
        }
    }

    if (ShouldPrintWeakBindList) {
        const auto WeakBindActionListOpt =
            DyldInfo->GetWeakBindActionList(Object.getConstMap(),
                                            SegmentCollection,
                                            IsBigEndian,
                                            Is64Bit);

        WeakBindActionListRangeError = WeakBindActionListOpt.getError();
        if (WeakBindActionListRangeError == MachO::SizeRangeError::None) {
            const auto WeakBindActionList = WeakBindActionListOpt.getRef();
            WeakBindActionListError =
                WeakBindActionList.GetAsList(WeakBindActionInfoList);

            if (!Options.SortKindList.empty()) {
                std::sort(WeakBindActionInfoList.begin(),
                          WeakBindActionInfoList.end(),
                          Comparator);
            }
        }
    }

    const auto TotalLines =
        BindActionInfoList.size() +
        LazyBindActionInfoList.size() +
        WeakBindActionInfoList.size();

    PrintLineSpamWarning(Options.OutFile, TotalLines);
    switch (BindActionListRangeError) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Empty:
            fputs("No Bind Info\n", Options.ErrFile);
            ShouldPrintBindList = false;

            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Bind-List goes past end-of-file\n", Options.ErrFile);
            ShouldPrintBindList = false;

            break;
    }

    if (ShouldPrintBindList) {
        PrintBindActionList<MachO::BindInfoKind::Normal>(
            "Bind",
            BindActionInfoList,
            SegmentCollection,
            SharedLibraryCollection,
            Is64Bit,
            Options);

        OperationCommon::HandleBindOpcodeParseError(
            Options.ErrFile,
            BindActionListError);
    }

    switch (LazyBindActionListRangeError) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Empty:
            if (ShouldPrintBindList) {
                fputc('\n', Options.OutFile);
            }

            fputs("No Lazy-Bind Info\n", Options.ErrFile);
            ShouldPrintLazyBindList = false;

            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            if (ShouldPrintBindList) {
                fputc('\n', Options.OutFile);
            }

            fputs("Lazy-Bind List goes past end-of-file\n", Options.ErrFile);
            ShouldPrintLazyBindList = false;

            break;
    }

    if (ShouldPrintLazyBindList) {
        if (ShouldPrintBindList) {
            fputc('\n', Options.OutFile);
        }

        PrintBindActionList<MachO::BindInfoKind::Lazy>(
            "Lazy-Bind",
            LazyBindActionInfoList,
            SegmentCollection,
            SharedLibraryCollection,
            Is64Bit,
            Options);

        OperationCommon::HandleBindOpcodeParseError(
            Options.ErrFile,
            LazyBindActionListError);
    }

    switch (WeakBindActionListRangeError) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Empty:
            if (ShouldPrintBindList || ShouldPrintLazyBindList) {
                fputc('\n', Options.OutFile);
            }

            fputs("No Weak-Bind Info\n", Options.ErrFile);
            ShouldPrintWeakBindList = false;

            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            if (ShouldPrintBindList || ShouldPrintLazyBindList) {
                fputc('\n', Options.OutFile);
            }

            fputs("Weak-Bind List goes past end-of-file\n", Options.ErrFile);
            ShouldPrintWeakBindList = false;

            break;
    }

    if (ShouldPrintWeakBindList) {
        if (ShouldPrintBindList || ShouldPrintLazyBindList) {
            fputc('\n', Options.OutFile);
        }

        PrintBindActionList<MachO::BindInfoKind::Weak>(
            "Weak-Bind",
            WeakBindActionInfoList,
            SegmentCollection,
            SharedLibraryCollection,
            Is64Bit,
            Options);

        OperationCommon::HandleBindOpcodeParseError(
            Options.ErrFile,
            WeakBindActionListError);
    }

    return 0;
}

static inline bool
ListHasSortKind(
    std::vector<PrintBindActionListOperation::Options::SortKind> &List,
    const PrintBindActionListOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(const char *Opt,
            struct PrintBindActionListOperation::Options &Options,
            PrintBindActionListOperation::Options::SortKind SortKind) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Opt);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

struct PrintBindActionListOperation::Options
PrintBindActionListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                               int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--only-normal") == 0) {
            if (Options.PrintAll == 7) {
                Options.PrintAll = 0;
            }

            Options.PrintNormal = true;
        } else if (strcmp(Argument, "--only-lazy") == 0) {
            if (Options.PrintAll == 7) {
                Options.PrintAll = 0;
            }

            Options.PrintLazy = true;
        } else if (strcmp(Argument, "--only-weak") == 0) {
            if (Options.PrintAll == 7) {
                Options.PrintAll = 0;
            }

            Options.PrintWeak = true;
        } else if (strcmp(Argument, "--sort-by-name") == 0) {
            AddSortKind("--sort-by-name", Options, Options::SortKind::ByName);
        } else if (strcmp(Argument, "--sort-by-dylib-ordinal") == 0) {
            AddSortKind("--sort-by-dylib-ordinal",
                        Options,
                        Options::SortKind::ByDylibOrdinal);
        } else if (strcmp(Argument, "--sort-by-type") == 0) {
            AddSortKind("--sort-by-type", Options, Options::SortKind::ByKind);
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
PrintBindActionListOperation::ParseOptions(const ArgvArray &Argv,
                                           int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintBindActionListOperation::Run(const MemoryObject &Object) const noexcept {
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
