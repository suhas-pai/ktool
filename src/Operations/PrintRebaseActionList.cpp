//
//  src/Operations/PrintRebaseActionList.cpp
//  ktool
//
//  Created by Suhas Pai on 5/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintRebaseActionList.h"

PrintRebaseActionListOperation::PrintRebaseActionListOperation() noexcept
: Operation(OpKind) {}

PrintRebaseActionListOperation::PrintRebaseActionListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static void
PrintRebaseAction(
    uint64_t Counter,
    int SizeDigitLength,
    const MachO::RebaseActionInfo &Action,
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const MachO::SegmentInfoCollection &SegmentCollection,
    bool Is64Bit,
    const struct PrintRebaseActionListOperation::Options &Options) noexcept
{
    fprintf(Options.OutFile,
            "Rebase Action %0*" PRIu64 ": ",
            SizeDigitLength,
            Counter);

    fprintf(Options.OutFile,
            " %" PRINTF_RIGHTPAD_FMT "s",
            static_cast<int>(MachO::RebaseWriteKindGetLongestDescription()),
            MachO::RebaseWriteKindGetDescription(Action.Kind).data());

    if (const auto *Segment = SegmentCollection.at(Action.SegmentIndex)) {
        const auto FullAddr = Segment->Memory.getBegin() + Action.AddrInSeg;
        const auto Section = Segment->FindSectionContainingAddress(FullAddr);

        PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                               Segment,
                                               Section,
                                               true);

        PrintUtilsWriteOffset32Or64(Options.OutFile, Is64Bit, FullAddr);
    } else {
        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fputs("<unknown>", Options.OutFile),
                                 OperationCommon::SegmentSectionPairMaxLength);
    }

    fputc('\n', Options.OutFile);
}

static int
PrintRebaseActionList(
    const ConstMachOMemoryObject &Object,
    const ConstMemoryMap &Map,
    const struct PrintRebaseActionListOperation::Options &Options) noexcept
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
                fputs("Provided file has multiple (conflicting) Rebase-List "
                      "information\n",
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

    const auto RebaseActionListOpt =
        FoundDyldInfo->GetRebaseActionList(Map, IsBigEndian, Is64Bit);

    switch (RebaseActionListOpt.getError()) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Empty:
            fputs("Provided file has no Rebase Info\n", Options.OutFile);
            return 0;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Rebase-Info goes past end-of-file\n", Options.ErrFile);
            return 1;
    }

    auto RebaseActionInfoList = std::vector<MachO::RebaseActionInfo>();

    const auto RebaseActionList = RebaseActionListOpt.getRef();
    const auto RebaseActionListError =
        RebaseActionList.GetAsList(RebaseActionInfoList);

    if (Options.Sort) {
        const auto Comparator =
            [&](const MachO::RebaseActionInfo &Lhs,
                const MachO::RebaseActionInfo &Rhs) noexcept
        {
            return (Lhs.AddrInSeg < Rhs.AddrInSeg);
        };


        std::sort(RebaseActionInfoList.begin(),
                  RebaseActionInfoList.end(),
                  Comparator);
    }

    Operation::PrintLineSpamWarning(Options.OutFile,
                                    RebaseActionInfoList.size());

    if (RebaseActionInfoList.empty()) {
        fputs("No Rebase-Info\n", Options.OutFile);
        return 0;
    }

    switch (RebaseActionInfoList.size()) {
        case 0:
            assert(0 && "Rebase-Action List shouldn't be empty at this point");
        case 1:
            fputs("1 Rebase Action:\n", Options.OutFile);
            break;
        default:
            fprintf(Options.OutFile,
                    "%" PRIuPTR " Rebase Actions:\n",
                    RebaseActionInfoList.size());
            break;
    }

    auto Counter = 1ull;
    const auto SizeDigitLength =
        PrintUtilsGetIntegerDigitLength(RebaseActionInfoList.size());

    for (const auto &Action : RebaseActionInfoList) {
        PrintRebaseAction(Counter,
                          SizeDigitLength,
                          Action,
                          SharedLibraryCollection,
                          SegmentCollection,
                          Is64Bit,
                          Options);
        Counter++;
    }

    OperationCommon::HandleRebaseOpcodeParseError(Options.ErrFile,
                                                  RebaseActionListError);

    return 0;
}

int
PrintRebaseActionListOperation::Run(const ConstDscImageMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    return PrintRebaseActionList(Object, Object.getDscMap(), Options);
}

int
PrintRebaseActionListOperation::Run(const ConstMachOMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    return PrintRebaseActionList(Object, Object.getMap(), Options);
}

struct PrintRebaseActionListOperation::Options
PrintRebaseActionListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                 int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort") == 0) {
            Options.Sort = true;
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
PrintRebaseActionListOperation::ParseOptions(const ArgvArray &Argv,
                                             int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintRebaseActionListOperation::Run(const MemoryObject &Object) const noexcept {
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
