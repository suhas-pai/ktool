//
//  src/Operations/PrintSymbolPtrSection.cpp
//  ktool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Common.h"
#include "Operation.h"
#include "PrintSymbolPtrSection.h"

PrintSymbolPtrSectionOperation::PrintSymbolPtrSectionOperation() noexcept
: Operation(OpKind) {}

PrintSymbolPtrSectionOperation::PrintSymbolPtrSectionOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static int
CompareEntriesBySortKind(
    const MachO::SymbolTableEntryCollectionEntryInfo &Lhs,
    const MachO::SymbolTableEntryCollectionEntryInfo &Rhs,
    const PrintSymbolPtrSectionOperation::Options::SortKind SortKind) noexcept
{
    switch (SortKind) {
        case PrintSymbolPtrSectionOperation::Options::SortKind::ByDylibOrdinal:
            if (Lhs.getDylibOrdinal() == Rhs.getDylibOrdinal()) {
                return 0;
            } else if (Lhs.getDylibOrdinal() < Rhs.getDylibOrdinal()) {
                return -1;
            }

            return 1;
        case PrintSymbolPtrSectionOperation::Options::SortKind::ByKind: {
            const auto LhsKind = Lhs.SymbolInfo.getKind();
            const auto RhsKind = Rhs.SymbolInfo.getKind();

            if (LhsKind == RhsKind) {
                return 0;
            } else if (LhsKind < RhsKind) {
                return -1;
            }

            return 1;
        }
        case PrintSymbolPtrSectionOperation::Options::SortKind::ByIndex: {
            if (Lhs.Index == Rhs.Index) {
                return 0;
            } else if (Lhs.Index < Rhs.Index) {
                return -1;
            }

            return 1;
        }
        case PrintSymbolPtrSectionOperation::Options::SortKind::BySymbol:
            return Lhs.String->compare(*Rhs.String);
    }

    assert(0 && "Unrecognized (and invalid) Sort-Type");
}

static void
PrintSymbolList(
    const struct PrintSymbolPtrSectionOperation::Options &Options,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const MachO::SharedLibraryInfoCollection &SharedLibraryCollection,
    const std::vector<MachO::SymbolTableEntryCollectionEntryInfo *> &List,
    const MachO::SectionInfo &Section) noexcept
{
    auto LargestIndex = LargestIntHelper<uint64_t>();
    auto LongestLength = LargestIntHelper<uint64_t>();
    auto LongestKind = LargestIntHelper<uint64_t>();
    auto LongestSegment = LargestIntHelper<uint8_t>();
    auto LongestSection = LargestIntHelper<uint8_t>();

    for (const auto &Info : List) {
        const auto Section = Info->Section + 1;
        const auto SectionInfo = SegmentCollection.GetSectionWithIndex(Section);

        LargestIndex = Info->Index;
        LongestLength = Info->String->length();
        LongestSegment = SectionInfo->Segment->Name.length();
        LongestSection = SectionInfo->Name.length();
        LongestKind =
            MachO::SymbolTableEntrySymbolKindGetName(
                Info->SymbolInfo.getKind()).length();
    }

    PrintArchListOperation::PrintLineSpamWarning(Options.OutFile, List.size());
    fprintf(Options.OutFile,
            "Provided section has %" PRIuPTR " Indirect-Symbols:\n",
            List.size());

    auto Counter = static_cast<uint64_t>(1);
    auto ListSizeDigitLength = PrintUtilsGetIntegerDigitLength(List.size());
    auto MaxIndexDigitLength = PrintUtilsGetIntegerDigitLength(LargestIndex);

    const auto Segment = SegmentCollection.at(0);
    for (const auto &Info : List) {
        fprintf(Options.OutFile,
                "Indirect-Symbol %0*" PRIu64 ": ",
                ListSizeDigitLength,
                Counter);

        const auto PrintLength =
            fprintf(Options.OutFile, "\"%s\"", Info->String->data());

        if (Options.Verbose) {
            PrintUtilsRightPadSpaces(
                Options.OutFile,
                PrintLength,
                static_cast<int>(LongestLength.value() + 2));

            const auto &SymbolInfo = Info->SymbolInfo;
            const auto SymbolKind = SymbolInfo.getKind();
            const auto SymbolKindString =
                SymbolTableEntrySymbolKindGetName(SymbolKind).data();

            const auto KindRightPad =
                static_cast<int>(LongestKind + LENGTH_OF(" <Kind: , "));

            PrintUtilsRightPadSpaces(Options.OutFile,
                                     fprintf(Options.OutFile,
                                             " <Kind: %s, ",
                                             SymbolKindString),
                                     KindRightPad);

            fprintf(Options.OutFile,
                    "Index: %0*" PRIu64,
                    MaxIndexDigitLength,
                    Info->Index);

            if (SymbolInfo.IsPrivateExternal()) {
                fputs(", Private-External", Options.OutFile);
            }

            if (SymbolInfo.IsDebugSymbol()) {
                fputs(", Debug-Symbol", Options.OutFile);
            }

            const auto Section = Segment->SectionList.at(Info->Section + 1);
            fputs(", Section: ", Options.OutFile);

            PrintUtilsRightPadSpaces(Options.OutFile,
                                     fprintf(Options.OutFile,
                                             "%s,",
                                             Segment->Name.data()),
                                     LongestSegment + LENGTH_OF(","));

            PrintUtilsRightPadSpaces(Options.OutFile,
                                     fprintf(Options.OutFile,
                                             "%s, ",
                                             Section.Name.data()),
                                     LongestSection + LENGTH_OF(", "));

            const auto DylibOrdinal = Info->getDylibOrdinal();
            OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                                   SharedLibraryCollection,
                                                   DylibOrdinal,
                                                   true);

            fputc('>', Options.OutFile);
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }
}

int
PrintSymbolPtrSectionOperation::Run(const ConstMachOMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    auto SymtabCommand = static_cast<const MachO::SymTabCommand *>(nullptr);
    auto DySymtabCommand =
        static_cast<const MachO::DynamicSymTabCommand *>(nullptr);

    for (const auto &LoadCmd : LoadCmdStorage) {
        switch (LoadCmd.getKind(IsBigEndian)) {
            case MachO::LoadCommand::Kind::SymbolTable:
                if (SymtabCommand != nullptr) {
                    const auto Compare =
                        memcmp(SymtabCommand, &LoadCmd, sizeof(*SymtabCommand));

                    if (Compare != 0) {
                        fputs("Provided file has multiple (differing) "
                              "symbol-table load-commands\n",
                              Options.ErrFile);
                        return 1;
                    }
                } else {
                    SymtabCommand =
                        &cast<MachO::SymTabCommand>(LoadCmd, IsBigEndian);
                }

                break;

            case MachO::LoadCommand::Kind::DynamicSymbolTable:
                if (DySymtabCommand != nullptr) {
                    const auto Compare =
                        memcmp(DySymtabCommand,
                               &LoadCmd,
                               sizeof(*DySymtabCommand));

                    if (Compare != 0) {
                        fputs("Provided file has multiple (differing) "
                              "dynamic-symbol-table load-commands\n",
                              Options.ErrFile);
                        return 1;
                    }
                } else {
                    DySymtabCommand =
                        &cast<MachO::DynamicSymTabCommand>(LoadCmd,
                                                           IsBigEndian);
                }

                break;

            case MachO::LoadCommand::Kind::LoadDylib:
            case MachO::LoadCommand::Kind::LoadWeakDylib:
            case MachO::LoadCommand::Kind::ReexportDylib:
            case MachO::LoadCommand::Kind::LazyLoadDylib:
            case MachO::LoadCommand::Kind::LoadUpwardDylib:
            case MachO::LoadCommand::Kind::Segment:
            case MachO::LoadCommand::Kind::SymbolSegment:
            case MachO::LoadCommand::Kind::Thread:
            case MachO::LoadCommand::Kind::UnixThread:
            case MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary:
            case MachO::LoadCommand::Kind::IdFixedVMSharedLibrary:
            case MachO::LoadCommand::Kind::Ident:
            case MachO::LoadCommand::Kind::FixedVMFile:
            case MachO::LoadCommand::Kind::PrePage:
            case MachO::LoadCommand::Kind::IdDylib:
            case MachO::LoadCommand::Kind::LoadDylinker:
            case MachO::LoadCommand::Kind::IdDylinker:
            case MachO::LoadCommand::Kind::PreBoundDylib:
            case MachO::LoadCommand::Kind::Routines:
            case MachO::LoadCommand::Kind::SubFramework:
            case MachO::LoadCommand::Kind::SubUmbrella:
            case MachO::LoadCommand::Kind::SubClient:
            case MachO::LoadCommand::Kind::SubLibrary:
            case MachO::LoadCommand::Kind::TwoLevelHints:
            case MachO::LoadCommand::Kind::PrebindChecksum:
            case MachO::LoadCommand::Kind::Segment64:
            case MachO::LoadCommand::Kind::Routines64:
            case MachO::LoadCommand::Kind::Uuid:
            case MachO::LoadCommand::Kind::Rpath:
            case MachO::LoadCommand::Kind::CodeSignature:
            case MachO::LoadCommand::Kind::SegmentSplitInfo:
            case MachO::LoadCommand::Kind::EncryptionInfo:
            case MachO::LoadCommand::Kind::DyldInfo:
            case MachO::LoadCommand::Kind::DyldInfoOnly:
            case MachO::LoadCommand::Kind::VersionMinimumMacOSX:
            case MachO::LoadCommand::Kind::VersionMinimumIPhoneOS:
            case MachO::LoadCommand::Kind::FunctionStarts:
            case MachO::LoadCommand::Kind::DyldEnvironment:
            case MachO::LoadCommand::Kind::Main:
            case MachO::LoadCommand::Kind::DataInCode:
            case MachO::LoadCommand::Kind::SourceVersion:
            case MachO::LoadCommand::Kind::DylibCodeSignDRS:
            case MachO::LoadCommand::Kind::EncryptionInfo64:
            case MachO::LoadCommand::Kind::LinkerOption:
            case MachO::LoadCommand::Kind::LinkerOptimizationHint:
            case MachO::LoadCommand::Kind::VersionMinimumTvOS:
            case MachO::LoadCommand::Kind::VersionMinimumWatchOS:
            case MachO::LoadCommand::Kind::Note:
            case MachO::LoadCommand::Kind::BuildVersion:
            case MachO::LoadCommand::Kind::DyldExportsTrie:
            case MachO::LoadCommand::Kind::DyldChainedFixups:
                break;
        }
    }

    if (SymtabCommand == nullptr) {
        fputs("Provided file is missing its symbol-table load-command\n",
              Options.ErrFile);
        return 1;
    }

    if (DySymtabCommand == nullptr) {
        fputs("Provided file is missing its dynamic-symbol-table "
              "load-command\n",
              Options.ErrFile);
        return 1;
    }

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.Is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    const auto Segment = SegmentCollection.GetInfoForName(Options.SegmentName);
    if (Segment == nullptr) {
        fprintf(Options.ErrFile,
                "Provided file has no segment with name \"%s\"",
                Options.SegmentName.data());
        return 1;
    }

    const auto Section = Segment->FindSectionWithName(Options.SectionName);
    if (Section == nullptr) {
        fprintf(Options.ErrFile,
                "Provided file has no section with name \"%s\" in provided "
                "segment\n",
                Options.SectionName.data());
        return 1;
    }

    const auto SectionKind = Section->getKind();
    if (SectionKind != MachO::SegmentSectionKind::LazySymbolPointers &&
        SectionKind != MachO::SegmentSectionKind::NonLazySymbolPointers)
    {
        fprintf(Options.OutFile,
                "Provided Segment-Section \"%s\",\"%s\" is neither a Non-Lazy "
                "nor a Lazy Symbol-Pointer Section\n",
                Segment->Name.data(),
                Section->Name.data());
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

    auto SymbolTableParseOptions =
        MachO::SymbolTableEntryCollection::ParseOptions();

    auto SymbolTableParseError = MachO::SymbolTableEntryCollection::Error::None;
    auto SymbolTableKeyKind =
        MachO::SymbolTableEntryCollection::KeyKindEnum::Index;

    const auto SymbolCollection =
        MachO::SymbolTableEntryCollection::OpenForIndirectSymbolsPtrSection(
            Object.getMap().getBegin(),
            *SymtabCommand,
            *DySymtabCommand,
            *Section,
            Is64Bit,
            IsBigEndian,
            SymbolTableKeyKind,
            SymbolTableParseOptions,
            &SymbolTableParseError);

    switch (SymbolTableParseError) {
        case MachO::SymbolTableParseError::None:
            break;
        case MachO::SymbolTableParseError::InvalidStringOffset:
            fputs("Provided file has an invalid symbol-table entry "
                  "(String-Index is out-of-bounds)\n",
                  Options.ErrFile);
            return 1;
        case MachO::SymbolTableParseError::OutOfBoundsIndirectIndex:
            fputs("Provided Section points to an invalid indirect-symbol\n",
                  Options.ErrFile);
            return 1;
        case MachO::SymbolTableParseError::InvalidSection:
            fputs("Provided section is invalid\n", Options.ErrFile);
            return 1;
    }

    auto List = SymbolCollection.GetAsList();
    if (List.empty()) {
        fputs("Provided section has no Indirect-Symbols\n", Options.ErrFile);
        return 1;
    }

    std::sort(List.begin(), List.end(), [&](const auto &Lhs, const auto &Rhs) {
        if (!Options.SortKindList.empty()) {
            for (const auto &Sort : Options.SortKindList) {
                const auto Compare = CompareEntriesBySortKind(*Lhs, *Rhs, Sort);
                if (Compare != 0) {
                    return (Compare < 0);
                }
            }
        } else if (Lhs->Index < Rhs->Index) {
            return true;
        }

        return false;
    });

    PrintSymbolList(Options,
                    SegmentCollection,
                    SharedLibraryCollection,
                    List,
                    *Section);

    return 0;
}

static inline bool
ListHasSortKind(
    std::vector<PrintSymbolPtrSectionOperation::Options::SortKind> &List,
    const PrintSymbolPtrSectionOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(PrintSymbolPtrSectionOperation::Options::SortKind SortKind,
            const char *Option,
            struct PrintSymbolPtrSectionOperation::Options &Options) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Option);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

struct PrintSymbolPtrSectionOperation::Options
PrintSymbolPtrSectionOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                 int *IndexOut) noexcept
{
    auto DidGetInfo = false;
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort-by-dylib-ordinal") == 0) {
            AddSortKind(Options::SortKind::ByDylibOrdinal, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-index") == 0) {
            AddSortKind(Options::SortKind::ByIndex, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-kind") == 0) {
            AddSortKind(Options::SortKind::ByKind, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-symbol") == 0) {
            AddSortKind(Options::SortKind::BySymbol, Argument, Options);
        } else if (!Argument.IsOption()) {
            if (DidGetInfo) {
                if (IndexOut != nullptr) {
                    *IndexOut = Argv.indexOf(Argument);
                }

                break;
            }

            const auto ArgumentString = Argument.GetStringView();
            OperationCommon::ParseSegmentSectionPair(Options.ErrFile,
                                                     ArgumentString,
                                                     Options.SegmentName,
                                                     Options.SectionName);

            DidGetInfo = true;
            continue;
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
PrintSymbolPtrSectionOperation::ParseOptions(const ArgvArray &Argv,
                                             int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintSymbolPtrSectionOperation::Run(const MemoryObject &Object) const noexcept {
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
