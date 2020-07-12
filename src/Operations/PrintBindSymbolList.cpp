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

    assert(0 && "Unrecognized (and invalid) Sort-Type");
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

    if (const auto *Segment = SegmentCollection.at(Action.SegmentIndex)) {
        const auto FullAddr = Segment->Memory.getBegin() + Action.AddrInSeg;
        const auto Section = Segment->FindSectionContainingAddress(FullAddr);

        PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                               Segment,
                                               Section,
                                               true);

        PrintUtilsWriteOffset(Options.OutFile, FullAddr, Is64Bit);
        if (Action.Addend) {
            fputs(" + ", Options.OutFile);
            PrintUtilsWriteOffset(Options.OutFile, Action.Addend, Is64Bit);
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

    PrintUtilsRightPadSpaces(Options.OutFile,
                             fprintf(Options.OutFile,
                                     " \"%s\"",
                                     Action.SymbolName.data()),
                             static_cast<int>(LongestBindSymbolLength) + 3);

    if constexpr (BindKind != MachO::BindInfoKind::Weak) {
        fputs("\t\t", Options.OutFile);
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
    const struct PrintBindSymbolListOperation::Options &Options) noexcept
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

int
PrintBindSymbolListOperation::Run(const ConstMachOMemoryObject &Object,
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

    const auto Is64Bit = Object.Is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    auto ShouldPrintBind = Options.PrintNormal;
    auto ShouldPrintLazyBind = Options.PrintLazy;
    auto ShouldPrintWeakBind = Options.PrintWeak;

    auto BindSymbolListError = MachO::BindOpcodeParseError();
    auto LazyBindSymbolListError = MachO::BindOpcodeParseError();
    auto WeakBindSymbolListError = MachO::BindOpcodeParseError();

    const auto Comparator =
        [&](const MachO::BindActionInfo &Lhs, const MachO::BindActionInfo &Rhs)
    {
        for (const auto &SortKind : Options.SortKindList) {
            const auto CmpResult =
                CompareActionsBySortKind(Lhs, Rhs, SortKind);

            if (CmpResult != 0) {
                return (CmpResult < 0);
            }
        }

        return false;
    };

    if (ShouldPrintBind) {
        const auto BindActionListOpt =
            FoundDyldInfo->GetBindActionList(Object.getConstMap(),
                                             SegmentCollection,
                                             IsBigEndian,
                                             Is64Bit);

        switch (BindActionListOpt.getError()) {
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
            auto BindSymbolActionList = std::vector<MachO::BindActionInfo>();
            const auto BindActionList = BindActionListOpt.getRef();

            BindSymbolListError =
                BindActionList.GetListOfSymbols(BindSymbolActionList);

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

        const auto LazyBindActionListOpt =
            FoundDyldInfo->GetLazyBindActionList(Object.getConstMap(),
                                                 SegmentCollection,
                                                 IsBigEndian,
                                                 Is64Bit);

        switch (LazyBindActionListOpt.getError()) {
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
            const auto LazyBindActionList = LazyBindActionListOpt.getRef();
            auto LazyBindSymbolActionList =
                std::vector<MachO::BindActionInfo>();

            LazyBindSymbolListError =
                LazyBindActionList.GetListOfSymbols(LazyBindSymbolActionList);

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

            OperationCommon::HandleBindOpcodeParseError(Options.ErrFile,
                                                        BindSymbolListError);
        }
    }

    if (ShouldPrintWeakBind) {
        if (ShouldPrintBind || ShouldPrintLazyBind) {
            fputc('\n', Options.OutFile);
        }

        const auto WeakBindActionListOpt =
            FoundDyldInfo->GetWeakBindActionList(Object.getConstMap(),
                                                 SegmentCollection,
                                                 IsBigEndian,
                                                 Is64Bit);

        switch (WeakBindActionListOpt.getError()) {
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
            const auto WeakBindActionList = WeakBindActionListOpt.getRef();
            auto WeakBindSymbolActionList =
                std::vector<MachO::BindActionInfo>();

            WeakBindSymbolListError =
                WeakBindActionList.GetListOfSymbols(WeakBindSymbolActionList);

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

            OperationCommon::HandleBindOpcodeParseError(Options.ErrFile,
                                                        BindSymbolListError);
        }
    }

    return 0;
}

static inline bool
ListHasSortKind(
    std::vector<PrintBindSymbolListOperation::Options::SortKind> &List,
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
PrintBindSymbolListOperation::ParseOptions(const ArgvArray &Argv,
                                           int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintBindSymbolListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
        case ObjectKind::DyldSharedCache:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}
