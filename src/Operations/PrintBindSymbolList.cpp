//
//  src/Operations/PrintBindSymbolList.cpp
//  ktool
//
//  Created by Suhas Pai on 5/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintBindSymbolList.h"

PrintBindSymbolListOperation::PrintBindSymbolListOperation() noexcept
: Operation(OpKind) {}

PrintBindSymbolListOperation::PrintBindSymbolListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static int
CompareActionsBySortKind(
    const MachO::BindActionInfo &Lhs,
    const MachO::BindActionInfo &Rhs,
    const PrintBindSymbolListOperation::Options::SortKind SortKind) noexcept
{
    switch (SortKind) {
        case PrintBindSymbolListOperation::Options::SortKind::ByName:
            return Lhs.SymbolName.compare(Rhs.SymbolName);
        case PrintBindSymbolListOperation::Options::SortKind::ByDylibOrdinal:
            if (Lhs.DylibOrdinal == Rhs.DylibOrdinal) {
                return 0;
            } else if (Lhs.DylibOrdinal < Rhs.DylibOrdinal) {
                return -1;
            }

            return 1;
        case PrintBindSymbolListOperation::Options::SortKind::ByType:
            const auto LhsKind = static_cast<uint8_t>(Lhs.WriteKind);
            const auto RhsKind = static_cast<uint8_t>(Rhs.WriteKind);

            if (LhsKind == RhsKind) {
                return 0;
            } else if (LhsKind < RhsKind) {
                return -1;
            }

            return 1;
    }

    assert(0 && "Unrecognized (and invalid) Sort-Kind");
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
    const struct PrintBindSymbolListOperation::Options &Options) noexcept
{
    fprintf(Options.OutFile,
            "%s Symbol %0*" PRIu64 ": ",
            Name,
            SizeDigitLength,
            Counter);

    if (const auto *Segment = SegmentCollection.atOrNull(Action.SegmentIndex)) {
        const auto &MemoryRange = Segment->getMemoryRange();
        const auto FullAddr = MemoryRange.getBegin() + Action.AddrInSeg;
        const auto Section = Segment->FindSectionContainingAddress(FullAddr);

        PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                               Segment,
                                               Section,
                                               true);

        PrintUtilsWriteOffset32Or64(Options.OutFile,
                                    Is64Bit,
                                    FullAddr,
                                    false,
                                    " ");
        if (Action.Addend) {
            PrintUtilsWriteOffset32Or64(Options.OutFile,
                                        Is64Bit,
                                        Action.Addend,
                                        false,
                                        " + ");
        }
    } else {
        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("<unknown>", Options.OutFile),
                                 OperationCommon::SegmentSectionPairMaxLength);
    }

    if constexpr (BindKind != MachO::BindInfoKind::Lazy) {
        fprintf(Options.OutFile,
                " %" PRINTF_RIGHTPAD_FMT "s",
                static_cast<int>(MachO::BindWriteKindGetLongestDescription()),
                MachO::BindWriteKindGetDescription(Action.WriteKind).data());
    }

    const auto RightPad =
        static_cast<int>(LongestBindSymbolLength + LENGTH_OF(" \"\""));

    PrintUtilsRightPadSpaces(Options.OutFile,
                             fprintf(Options.OutFile,
                                     " \"%s\"",
                                     Action.SymbolName.data()),
                             RightPad);

    if constexpr (BindKind != MachO::BindInfoKind::Weak) {
        fputc(' ', Options.OutFile);

        const auto PrintKind = PrintKindFromIsVerbose(Options.Verbose);
        OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                               LibraryCollection,
                                               Action.DylibOrdinal,
                                               PrintKind);
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
    const struct PrintBindSymbolListOperation::Options &Options) noexcept
{
    if (List.empty()) {
        fprintf(Options.OutFile, "No %s Info\n", Name);
        return;
    }

    auto LongestBindSymbolLength = LargestIntHelper();
    for (const auto &Action : List) {
        LongestBindSymbolLength = Action.SymbolName.length();
    }

    switch (List.size()) {
        case 0:
            assert(0 && "Bind-Symbol List shouldn't be empty at this point");
        case 1:
            fprintf(Options.OutFile, "1 %s Symbol:\n", Name);
            break;
        default:
            fprintf(Options.OutFile,
                    "%" PRIuPTR " %s Symbols:\n",
                    List.size(),
                    Name);
            break;
    }

    auto Counter = 1ull;
    const auto SizeDigitLength = PrintUtilsGetIntegerDigitLength(List.size());

    for (const auto &Symbol : List) {
        PrintBindAction<BindKind>(Name,
                                  Counter,
                                  SizeDigitLength,
                                  Symbol,
                                  LongestBindSymbolLength,
                                  LibraryCollection,
                                  SegmentCollection,
                                  Is64Bit,
                                  Options);
        Counter++;
    }
}

static int
PrintBindSymbolList(
    const ConstMachOMemoryObject &Object,
    const ConstMemoryMap &Map,
    const struct PrintBindSymbolListOperation::Options &Options) noexcept
{
    auto FoundDyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);

    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    for (const auto &LC : LoadCmdStorage) {
        const auto *DyldInfo =
            dyn_cast<MachO::DyldInfoCommand>(LC, IsBigEndian);

        if (DyldInfo == nullptr) {
            continue;
        }

        if (FoundDyldInfo != nullptr) {
            if (DyldInfo->BindOff != FoundDyldInfo->BindOff ||
                DyldInfo->BindSize != FoundDyldInfo->BindSize)
            {
                fputs("Provided file has multiple (conflicting) Bind-List "
                      "information\n",
                      Options.ErrFile);
                return 1;
            }

            if (DyldInfo->LazyBindOff != FoundDyldInfo->LazyBindOff ||
                DyldInfo->LazyBindSize != FoundDyldInfo->LazyBindSize)
            {
                fputs("Provided file has multiple (conflicting) Lazy-Bind "
                      "list information\n",
                      Options.ErrFile);
                return 1;
            }

            if (DyldInfo->WeakBindOff != FoundDyldInfo->WeakBindOff ||
                DyldInfo->WeakBindSize != FoundDyldInfo->WeakBindSize)
            {
                fputs("Provided file has multiple (conflicting) Weak-Bind "
                      "list information\n",
                      Options.ErrFile);
                return 1;
            }
        }

        if (DyldInfo->BindSize == 0 &&
            DyldInfo->LazyBindSize == 0 &&
            DyldInfo->WeakBindSize == 0)
        {
            fputs("Provided file has no Bind-Opcodes\n", Options.ErrFile);
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

    const auto Is64Bit = Object.is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    auto ShouldPrintBind = Options.PrintNormal;
    auto ShouldPrintLazyBind = Options.PrintLazy;
    auto ShouldPrintWeakBind = Options.PrintWeak;

    auto BindActionListError = MachO::SizeRangeError::None;
    auto LazyBindActionListError = MachO::SizeRangeError::None;
    auto WeakBindActionListError = MachO::SizeRangeError::None;

    auto BindSymbolListError = MachO::BindOpcodeParseError();
    auto LazyBindSymbolListError = MachO::BindOpcodeParseError();
    auto WeakBindSymbolListError = MachO::BindOpcodeParseError();

    auto BindSymbolActionList = std::vector<MachO::BindActionInfo>();
    auto LazyBindSymbolActionList = std::vector<MachO::BindActionInfo>();
    auto WeakBindSymbolActionList = std::vector<MachO::BindActionInfo>();

    const auto Comparator =
        [&](const MachO::BindActionInfo &Lhs,
            const MachO::BindActionInfo &Rhs) noexcept
    {
        for (const auto &SortKind : Options.SortKindList) {
            const auto CmpResult = CompareActionsBySortKind(Lhs, Rhs, SortKind);
            if (CmpResult != 0) {
                return (CmpResult < 0);
            }
        }

        return false;
    };

    if (ShouldPrintBind) {
        const auto BindActionListOpt =
            FoundDyldInfo->GetBindActionList(Map,
                                             SegmentCollection,
                                             IsBigEndian,
                                             Is64Bit);

        BindActionListError = BindActionListOpt.getError();
        if (BindActionListError == MachO::SizeRangeError::None) {
            const auto BindActionList = BindActionListOpt.getRef();
            BindSymbolListError =
                BindActionList.GetListOfSymbols(BindSymbolActionList);
        }
    }

    if (ShouldPrintLazyBind) {
        const auto LazyBindActionListOpt =
            FoundDyldInfo->GetLazyBindActionList(Map,
                                                 SegmentCollection,
                                                 IsBigEndian,
                                                 Is64Bit);

        LazyBindActionListError = LazyBindActionListOpt.getError();
        if (LazyBindActionListError == MachO::SizeRangeError::None) {
            const auto LazyBindActionList = LazyBindActionListOpt.getRef();
            LazyBindSymbolListError =
                LazyBindActionList.GetListOfSymbols(LazyBindSymbolActionList);
        }
    }

    if (ShouldPrintWeakBind) {
        const auto WeakBindActionListOpt =
            FoundDyldInfo->GetWeakBindActionList(Map,
                                                 SegmentCollection,
                                                 IsBigEndian,
                                                 Is64Bit);

        WeakBindActionListError = WeakBindActionListOpt.getError();
        if (WeakBindActionListError == MachO::SizeRangeError::None) {
            const auto WeakBindActionList = WeakBindActionListOpt.getRef();
            WeakBindSymbolListError =
                WeakBindActionList.GetListOfSymbols(WeakBindSymbolActionList);
        }
    }

    const auto TotalLineCount =
        BindSymbolActionList.size() +
        LazyBindSymbolActionList.size() +
        WeakBindSymbolActionList.size();

    Operation::PrintLineSpamWarning(Options.OutFile, TotalLineCount);
    if (ShouldPrintBind) {
        switch (BindActionListError) {
            case MachO::SizeRangeError::None:
                break;
            case MachO::SizeRangeError::Empty:
                fputs("No Bind Info\n", Options.ErrFile);
                ShouldPrintBind = false;

                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fputs("Bind-List goes past end-of-file\n", Options.ErrFile);
                ShouldPrintBind = false;

                break;
        }

        if (ShouldPrintBind) {
            if (!Options.SortKindList.empty()) {
                std::sort(BindSymbolActionList.begin(),
                          BindSymbolActionList.end(),
                          Comparator);
            }

            PrintBindActionList<MachO::BindInfoKind::Normal>(
                "Bind",
                BindSymbolActionList,
                SegmentCollection,
                SharedLibraryCollection,
                Is64Bit,
                Options);

            OperationCommon::HandleBindOpcodeParseError(Options.ErrFile,
                                                        BindSymbolListError);
        }
    }

    if (ShouldPrintLazyBind) {
        if (ShouldPrintBind) {
            fputc('\n', Options.OutFile);
        }

        switch (LazyBindActionListError) {
            case MachO::SizeRangeError::None:
                break;
            case MachO::SizeRangeError::Empty:
                fputs("No Lazy-Bind Info\n", Options.ErrFile);
                ShouldPrintLazyBind = false;

                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fputs("Lazy-Bind List goes past end-of-file\n",
                      Options.ErrFile);

                ShouldPrintLazyBind = false;
                break;
        }

        if (ShouldPrintLazyBind) {
            if (!Options.SortKindList.empty()) {
                std::sort(LazyBindSymbolActionList.begin(),
                          LazyBindSymbolActionList.end(),
                          Comparator);
            }

            PrintBindActionList<MachO::BindInfoKind::Lazy>(
                "Lazy-Bind",
                LazyBindSymbolActionList,
                SegmentCollection,
                SharedLibraryCollection,
                Is64Bit,
                Options);

            OperationCommon::HandleBindOpcodeParseError(
                Options.ErrFile,
                LazyBindSymbolListError);
        }
    }

    if (ShouldPrintWeakBind) {
        if (ShouldPrintBind || ShouldPrintWeakBind) {
            fputc('\n', Options.OutFile);
        }

        switch (WeakBindActionListError) {
            case MachO::SizeRangeError::None:
                break;
            case MachO::SizeRangeError::Empty:
                fputs("No Weak-Bind Info\n", Options.ErrFile);
                ShouldPrintWeakBind = false;

                break;
            case MachO::SizeRangeError::Overflows:
            case MachO::SizeRangeError::PastEnd:
                fputs("Weak-Bind List goes past end-of-file\n",
                      Options.ErrFile);

                ShouldPrintWeakBind = false;
                break;
        }

        if (ShouldPrintWeakBind) {
            if (!Options.SortKindList.empty()) {
                std::sort(WeakBindSymbolActionList.begin(),
                          WeakBindSymbolActionList.end(),
                          Comparator);
            }

            PrintBindActionList<MachO::BindInfoKind::Weak>(
                "Weak-Bind",
                WeakBindSymbolActionList,
                SegmentCollection,
                SharedLibraryCollection,
                Is64Bit,
                Options);

            OperationCommon::HandleBindOpcodeParseError(
                Options.ErrFile,
                WeakBindSymbolListError);
        }
    }

    return 0;
}

int
PrintBindSymbolListOperation::Run(const ConstDscImageMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    return PrintBindSymbolList(Object, Object.getDscMap(), Options);
}

int
PrintBindSymbolListOperation::Run(const ConstMachOMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    return PrintBindSymbolList(Object, Object.getMap(), Options);
}

static inline bool
ListHasSortKind(
    const std::vector<PrintBindSymbolListOperation::Options::SortKind> &List,
    const PrintBindSymbolListOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(PrintBindSymbolListOperation::Options::SortKind SortKind,
            const char *Option,
            struct PrintBindSymbolListOperation::Options &Options) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Option);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

struct PrintBindSymbolListOperation::Options
PrintBindSymbolListOperation::ParseOptionsImpl(const ArgvArray &Argv,
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
        } else if (strcmp(Argument, "--sort-by-name") == 0) {
            AddSortKind(Options::SortKind::ByName, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-dylib-ordinal") == 0) {
            AddSortKind(Options::SortKind::ByDylibOrdinal, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-type") == 0) {
            AddSortKind(Options::SortKind::ByType, Argument, Options);
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

int PrintBindSymbolListOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintBindSymbolListOperation::Run(const MemoryObject &Object) const noexcept {
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
