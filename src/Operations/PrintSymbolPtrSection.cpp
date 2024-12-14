//
//  Operations/PrintSymbolPtrSection.cpp
//  ktool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "Operations/Common.h"
#include "Operations/Operation.h"
#include "Operations/PrintSymbolPtrSection.h"

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
            const auto LhsKind = Lhs.getSymbolInfo().getKind();
            const auto RhsKind = Rhs.getSymbolInfo().getKind();

            if (LhsKind == RhsKind) {
                return 0;
            } else if (LhsKind < RhsKind) {
                return -1;
            }

            return 1;
        }
        case PrintSymbolPtrSectionOperation::Options::SortKind::ByIndex: {
            if (Lhs.getIndex() == Rhs.getIndex()) {
                return 0;
            } else if (Lhs.getIndex() < Rhs.getIndex()) {
                return -1;
            }

            return 1;
        }
        case PrintSymbolPtrSectionOperation::Options::SortKind::BySymbol:
            return Lhs.getString()->compare(*Rhs.getString());
    }

    assert(0 && "Unrecognized (and invalid) Sort-Kind");
}

static void
PrintSymbolList(
    const struct PrintSymbolPtrSectionOperation::Options &Options,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const MachO::SharedLibraryInfoCollection &SharedLibraryCollection,
    const std::vector<MachO::SymbolTableEntryCollectionEntryInfo *> &List)
{
    auto LargestIndex = LargestIntHelper();
    auto LongestLength = LargestIntHelper();
    auto LongestKind = LargestIntHelper();
    auto LongestSegment = LargestIntHelper<uint8_t>();
    auto LongestSection = LargestIntHelper<uint8_t>();

    for (const auto &Info : List) {
        if (Info->isSectionDefined()) {
            const auto Section = Info->getSectionOrdinal() - 1;
            const auto SectionInfo =
                SegmentCollection.GetSectionWithIndex(Section);

            LongestSegment = SectionInfo->getSegment()->getName().length();
            LongestSection = SectionInfo->getName().length();
        }

        const auto SymbolKind = Info->getSymbolKind();
        const auto &SymbolKindDescriptionOpt =
            MachO::SymbolTableEntrySymbolKindGetDesc(SymbolKind);

        LargestIndex = Info->getIndex();
        LongestLength = Info->getString()->length();

        if (const auto SymbolKindDesc = SymbolKindDescriptionOpt) {
            LongestKind = SymbolKindDesc->length();
        }
    }

    Operation::PrintLineSpamWarning(Options.OutFile, List.size());
    fprintf(Options.OutFile,
            "Provided section has %" PRIuPTR " Indirect-Symbols:\n",
            List.size());

    auto Counter = static_cast<uint64_t>(1);
    auto ListSizeDigitLength = PrintUtilsGetIntegerDigitLength(List.size());
    auto MaxIndexDigitLength = PrintUtilsGetIntegerDigitLength(LargestIndex);

    const auto &Segment = SegmentCollection.front();
    for (const auto &Info : List) {
        fprintf(Options.OutFile,
                "Indirect-Symbol %0*" PRIu64 ": ",
                ListSizeDigitLength,
                Counter);

        const auto PrintLength =
            fprintf(Options.OutFile, "\"%s\"", Info->getString()->data());

        if (Options.Verbose) {
            const auto RightPad =
                static_cast<int>(LongestLength.value() + LENGTH_OF("\"\""));

            PrintUtilsRightPadSpaces(Options.OutFile, PrintLength, RightPad);

            const auto &SymbolInfo = Info->getSymbolInfo();
            const auto SymbolKind = SymbolInfo.getKind();
            const auto SymbolKindDescOpt =
                SymbolTableEntrySymbolKindGetDesc(SymbolKind);

            const auto SymbolKindDescription =
                SymbolKindDescOpt.value_or("Unrecognized");

            const auto KindRightPad =
                static_cast<int>(LongestKind + LENGTH_OF(" <Kind: , "));

            PrintUtilsRightPadSpaces(Options.OutFile,
                                     fprintf(Options.OutFile,
                                             " <Kind: %s, ",
                                             SymbolKindDescription.data()),
                                     KindRightPad);

            fprintf(Options.OutFile,
                    "Index: %0*" PRIu64,
                    MaxIndexDigitLength,
                    Info->getIndex());

            if (SymbolInfo.isPrivateExternal()) {
                fputs(", Private-External", Options.OutFile);
            }

            if (SymbolInfo.isDebugSymbol()) {
                fputs(", Debug-Symbol", Options.OutFile);
            }

            fputs(", Section: ", Options.OutFile);
            if (Info->isSectionDefined()) {
                const auto Ordinal = Info->getSectionOrdinal();
                const auto *Section = Segment.getSectionAtOrdinal(Ordinal);

                PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                       &Segment,
                                                       Section,
                                                       false,
                                                       "",
                                                       ", ");
            } else {
                fputs("No-Section, ", Options.OutFile);
            }

            OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                                   SharedLibraryCollection,
                                                   Info->getDylibOrdinal(),
                                                   PrintKind::Verbose);

            fputc('>', Options.OutFile);
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }
}

static int
PrintSymbolPtrList(
    const MachOMemoryObject &Object,
    const uint8_t *const MapBegin,
    const struct PrintSymbolPtrSectionOperation::Options &Options) noexcept
{
    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    const auto Segment = SegmentCollection.GetInfoForName(Options.SegmentName);
    if (Segment == nullptr) {
        fprintf(Options.ErrFile,
                "Provided file has no segment with name \"" STRING_VIEW_FMT
                "\"\n",
                STRING_VIEW_FMT_ARGS(Options.SegmentName));
        return 1;
    }

    if (Segment->getFlags().isProtected()) {
        fprintf(Options.ErrFile,
                "Provided segment \"%s\" is protected (encrypted)\n",
                Segment->getName().data());
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
                Segment->getName().data(),
                Section->getName().data());
        return 0;
    }

    auto SymtabCommand = static_cast<const MachO::SymTabCommand *>(nullptr);
    auto DySymtabCommand =
        static_cast<const MachO::DynamicSymTabCommand *>(nullptr);

    for (const auto &LC : LoadCmdStorage) {
        switch (LC.getKind(IsBigEndian)) {
            case MachO::LoadCommand::Kind::SymbolTable:
                if (SymtabCommand != nullptr) {
                    const auto Compare =
                        memcmp(SymtabCommand, &LC, sizeof(*SymtabCommand));

                    if (Compare != 0) {
                        fputs("Provided file has multiple (differing) "
                              "symbol-table load-commands\n",
                              Options.ErrFile);
                        return 1;
                    }
                } else {
                    SymtabCommand =
                        &cast<MachO::SymTabCommand>(LC, IsBigEndian);
                }

                break;

            case MachO::LoadCommand::Kind::DynamicSymbolTable:
                if (DySymtabCommand != nullptr) {
                    const auto Compare =
                        memcmp(DySymtabCommand, &LC, sizeof(*DySymtabCommand));

                    if (Compare != 0) {
                        fputs("Provided file has multiple (differing) "
                              "dynamic-symbol-table load-commands\n",
                              Options.ErrFile);
                        return 1;
                    }
                } else {
                    DySymtabCommand =
                        &cast<MachO::DynamicSymTabCommand>(LC, IsBigEndian);
                }

                break;

            default:
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
            MapBegin,
            *SymtabCommand,
            *DySymtabCommand,
            *Section,
            IsBigEndian,
            Is64Bit,
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
        case MachO::SymbolTableParseError::NoNullTerminator:
            fputs("Provided file has an invalid symbol-table entry "
                  "(String has no null-terminator)\n",
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

    if (!Options.SortKindList.empty()) {
        std::sort(List.begin(),
                  List.end(),
                  [&](const auto &Lhs, const auto &Rhs) noexcept
        {
            for (const auto &Sort : Options.SortKindList) {
                const auto Compare = CompareEntriesBySortKind(*Lhs, *Rhs, Sort);
                if (Compare != 0) {
                    return (Compare < 0);
                }
            }

            return false;
        });
    } else {
        std::sort(List.begin(),
                  List.end(),
                  [&](const auto &Lhs, const auto &Rhs) noexcept
        {
            return Lhs->getIndex() < Rhs->getIndex();
        });
    }

    PrintSymbolList(Options,
                    SegmentCollection,
                    SharedLibraryCollection,
                    List);
    return 0;
}

int
PrintSymbolPtrSectionOperation::Run(const DscImageMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    return PrintSymbolPtrList(Object, Object.getDscMap().getBegin(), Options);
}

int
PrintSymbolPtrSectionOperation::Run(const MachOMemoryObject &Object,
                                    const struct Options &Options) noexcept
{
    return PrintSymbolPtrList(Object, Object.getMap().getBegin(), Options);
}

static inline bool
ListHasSortKind(
    const std::vector<PrintSymbolPtrSectionOperation::Options::SortKind> &List,
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

auto
PrintSymbolPtrSectionOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                 int *const IndexOut) noexcept
    -> struct PrintSymbolPtrSectionOperation::Options
{
    struct Options Options;

    auto DidGetInfo = false;
    auto Index = int();

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
        } else if (!Argument.isOption()) {
            if (DidGetInfo) {
                break;
            }

            const auto ArgumentString = Argument.GetStringView();
            OperationCommon::ParseSegmentSectionPair(Options.ErrFile,
                                                     ArgumentString,
                                                     Options.SegmentName,
                                                     Options.SectionName);

            DidGetInfo = true;
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
PrintSymbolPtrSectionOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintSymbolPtrSectionOperation::Run(const MemoryObject &Object) const noexcept {
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
