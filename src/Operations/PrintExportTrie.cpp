//
//  src/Operations/PrintExportTrie.cpp
//  ktool
//
//  Created by Suhas Pai on 5/9/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "ADT/MachO.h"
#include "ADT/DscImage.h"

#include "Utils/MachOPrinter.h"
#include "Utils/MiscTemplates.h"
#include "Utils/PrintUtils.h"
#include "Utils/SwapRanges.h"

#include "Common.h"
#include "Operation.h"
#include "PrintExportTrie.h"

PrintExportTrieOperation::PrintExportTrieOperation() noexcept
: Operation(OpKind) {}

PrintExportTrieOperation::PrintExportTrieOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static void
FindSegmentAndSectionForAddr(const MachO::SegmentInfoCollection &Collection,
                             uint64_t Addr,
                             const MachO::SegmentInfo *&Segment,
                             const MachO::SectionInfo *&Section) noexcept
{
    Segment = Collection.FindSegmentContainingAddress(Addr);
    if (Segment != nullptr) {
        Section = Segment->FindSectionContainingAddress(Addr);
    }
}

static bool
ExportNodeMeetsRequirements(
    const MachO::ExportTrieExportChildNode *Node,
    const struct PrintExportTrieOperation::Options &Options)
{
    if (!Options.KindRequirements.empty()) {
        auto MeetsReq = false;
        for (const auto &Kind : Options.KindRequirements) {
            if (Node->Kind == Kind) {
                MeetsReq = true;
                break;
            }
        }

        if (!MeetsReq) {
            return false;
        }

        // Re-exports, if they're explicitly allowed, automatically pass all
        // section requirements.

        if (Node->Kind == MachO::ExportTrieExportKind::Reexport) {
            return true;
        }
    }

    if (!Options.SectionRequirements.empty()) {
        if (Node->Kind == MachO::ExportTrieExportKind::Reexport) {
            return false;
        }

        const auto Segment = Node->getSegment();
        const auto Section = Node->getSection();

        if (Segment == nullptr) {
            return false;
        }

        for (const auto &Requirement : Options.SectionRequirements) {
            if (Segment->Name != Requirement.SegmentName) {
                continue;
            }

            if (Requirement.SectionName.empty()) {
                return true;
            }

            if (Section == nullptr) {
                return false;
            }

            if (Section->Name == Requirement.SectionName) {
                return true;
            }
        }

        return false;
    }

    return true;
}

constexpr static auto TabLength = 8;

[[nodiscard]] static uint64_t
GetLineLengthForSymbolLength(uint64_t DepthLevel,
                             uint64_t SymbolLength) noexcept
{
    const auto DepthIndex = (DepthLevel - 1);
    const auto Length = (SymbolLength + (TabLength * DepthIndex));

    return Length;
}

static void
PrintTreeExportInfo(
    const MachO::ExportTrieExportChildNode &Export,
    const MachO::SharedLibraryInfoCollection &SharedLibraryCollection,
    int WrittenOut,
    uint64_t DepthLevel,
    int LongestLength,
    bool Is64Bit,
    const struct PrintExportTrieOperation::Options &Options) noexcept
{
    auto RightPad = LongestLength + LENGTH_OF("\"\"");
    auto KindDesc = ExportTrieExportKindGetDescription(Export.Kind).data();

    if (KindDesc == nullptr) {
        KindDesc = "<unrecognized>";
    }

    PrintUtilsRightPadSpaces(Options.OutFile, WrittenOut, RightPad);
    fprintf(Options.OutFile, " (Exported - %s", KindDesc);

    if (Options.Verbose) {
        if (Export.Kind != MachO::ExportTrieExportKind::Reexport) {
            PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                   Export.getSegment(),
                                                   Export.getSection(),
                                                   false,
                                                   "   - ",
                                                   " - ");

            const auto ImageOffset = Export.Info.getImageOffset();
            PrintUtilsWriteOffset(Options.OutFile, ImageOffset, Is64Bit);
        } else {
            fputs(" - ", Options.OutFile);
            if (!Export.Info.getReexportImportName().empty()) {
                fprintf(Options.OutFile,
                        "As \"%s\" - ",
                        Export.Info.getReexportImportName().data());
            }

            const auto DylibOrdinal = Export.Info.getReexportDylibOrdinal();
            OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                                   SharedLibraryCollection,
                                                   DylibOrdinal,
                                                   Options.Verbose);
            fputc(')', Options.OutFile);
        }
    }

    fputc(')', Options.OutFile);
}

[[nodiscard]] static uint64_t
GetSymbolLengthForLongestPrintedLine(
    const MachO::ExportTrieEntryCollection &Collection) noexcept
{
    auto LongestLength = LargestIntHelper();

    const auto End = Collection.end();
    for (auto Iter = Collection.begin(); Iter != End; Iter++) {
        const auto Length =
            GetLineLengthForSymbolLength(Iter.getDepthLevel(),
                                         Iter->String.length());

        LongestLength = Length;
    }

    return LongestLength;
}

static int
HandleTreeOption(
    MachO::ExportTrieEntryCollection &EntryCollection,
    const MachO::SharedLibraryInfoCollection &SharedLibraryCollection,
    uint64_t Base,
    bool Is64Bit,
    const struct PrintExportTrieOperation::Options &Options) noexcept
{
    if (EntryCollection.empty()) {
        fputs("Provided file has an empty export-trie\n", Options.OutFile);
        return 1;
    }

    if (!Options.SectionRequirements.empty()) {
        auto CollectionEnd = EntryCollection.end();
        for (auto Iter = EntryCollection.begin(); Iter != CollectionEnd;) {
            if (!Iter->IsExport()) {
                Iter++;
                continue;
            }

            if (ExportNodeMeetsRequirements(Iter->getAsExportNode(), Options)) {
                Iter++;
                continue;
            }

            Iter = EntryCollection.RemoveNode(*Iter.getNode(), true);
        }

        if (EntryCollection.empty()) {
            fputs("Provided file has no export-trie after filtering with "
                  "provided requirements\n",
                  Options.OutFile);
            return 1;
        }
    }

    if (Options.Sort) {
        EntryCollection.Sort([](const auto &Lhs, const auto &Rhs) noexcept {
            const auto &Left =
                reinterpret_cast<const MachO::ExportTrieChildNode &>(Lhs);
            const auto &Right =
                reinterpret_cast<const MachO::ExportTrieChildNode &>(Rhs);

            return (Left.String <= Right.String);
        });
    }

    const auto Count = EntryCollection.GetCount();
    const auto LongestLength =
        GetSymbolLengthForLongestPrintedLine(EntryCollection);

    const auto PrintNode =
        [&](FILE *OutFile,
            int WrittenOut,
            uint64_t DepthLevel,
            const BasicTreeNode &Node) noexcept
    {
        const auto &Info =
            reinterpret_cast<const MachO::ExportTrieChildNode &>(Node);

        WrittenOut += fprintf(OutFile, "\"%s\"", Info.String.data());
        if (Info.IsExport()) {
            const auto &ExportInfo =
                reinterpret_cast<const MachO::ExportTrieExportChildNode &>(
                    Info);

            PrintTreeExportInfo(ExportInfo,
                                SharedLibraryCollection,
                                WrittenOut,
                                DepthLevel,
                                static_cast<int>(LongestLength),
                                Is64Bit,
                                Options);
        }

        return true;
    };

    Operation::PrintLineSpamWarning(Options.OutFile, Count);
    EntryCollection.PrintHorizontal(Options.OutFile, TabLength, PrintNode);

    return 0;
}

struct ExportInfo {
    MachO::ExportTrieExportKind Kind;
    MachO::ExportTrieExportInfo Info;

    std::string_view SegmentName;
    std::string_view SectionName;

    std::string String;
};

void PrintExtraExportTrieError(FILE *ErrFile) noexcept {
    fputs("Provided file has multiple export-tries\n", ErrFile);
}

using TrieListType =
    TypedAllocationOrError<MachO::ConstExportTrieList, MachO::SizeRangeError>;

static int
FindExportTrieList(
    const ConstMemoryMap &Map,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const struct PrintExportTrieOperation::Options &Options,
    TrieListType &TrieList) noexcept
{
    auto FoundExportTrie = false;
    const auto IsBigEndian = LoadCmdStorage.IsBigEndian();

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto *DyldInfo =
            dyn_cast<MachO::DyldInfoCommand>(LoadCmd, IsBigEndian);

        if (DyldInfo != nullptr) {
            if (FoundExportTrie) {
                PrintExtraExportTrieError(Options.ErrFile);
                return 1;
            }

            FoundExportTrie = true;
            TrieList = DyldInfo->GetConstExportTrieList(Map, IsBigEndian);
        } else {
            const auto *DyldExportsTrie =
                LoadCmd.dyn_cast<MachO::LoadCommand::Kind::DyldExportsTrie>(
                    IsBigEndian);

            if (DyldExportsTrie != nullptr) {
                if (FoundExportTrie) {
                    PrintExtraExportTrieError(Options.ErrFile);
                    return 1;
                }

                TrieList =
                    DyldExportsTrie->GetConstExportTrieList(Map, IsBigEndian);
            }
        }
    }

    // Not having an export-trie is not an error.

    if (!FoundExportTrie) {
        fputs("Provided file does not have an export-trie\n", Options.OutFile);
        return 0;
    }

    switch (TrieList.getError()) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Empty:
            fputs("Provided file has an empty export-trie\n", Options.OutFile);
            return 1;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Provided file has an export-trie that extends past "
                  "end-of-file\n",
                  Options.ErrFile);
            return 1;
    }

    return 0;
}

int
PrintExportTrie(
    const ConstMachOMemoryObject &Object,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const MachO::SegmentInfoCollection &SegmentCollection,
    const MachO::SharedLibraryInfoCollection &LibraryCollection,
    const TrieListType &TrieList,
    uint64_t Base,
    const struct PrintExportTrieOperation::Options &Options) noexcept
{
    auto ExportList = std::vector<ExportInfo>();
    auto LongestExportLength = LargestIntHelper();

    // A very large allocation, but many dylibs seem to have anywhere between
    // 1000-3000 symbols in the export-trie.

    ExportList.reserve(2000);

    for (const auto &Info : TrieList.getRef()) {
        if (!Info.IsExport()) {
            continue;
        }

        const auto String = Info.String;
        const auto StringLength = String.length();

        LongestExportLength = StringLength;

        auto SegmentName = std::string_view();
        auto SectionName = std::string_view();

        if (!Info.Export.getFlags().IsReexport()) {
            const auto Addr = Base + Info.Export.getImageOffset();

            auto SegmentInfo = static_cast<const MachO::SegmentInfo *>(nullptr);
            auto SectionInfo = static_cast<const MachO::SectionInfo *>(nullptr);

            FindSegmentAndSectionForAddr(SegmentCollection,
                                         Addr,
                                         SegmentInfo,
                                         SectionInfo);

            if (SegmentInfo != nullptr) {
                SegmentName = SegmentInfo->Name;
                if (SectionInfo != nullptr) {
                    SectionName = SectionInfo->Name;
                }
            }
        }

        ExportList.emplace_back(ExportInfo {
            .Kind = Info.Kind,
            .Info = Info.Export,
            .SegmentName = SegmentName,
            .SectionName = SectionName,
            .String = Info.String
        });
    }

    if (ExportList.empty()) {
        fputs("Provided file has no exports in export-trie\n", Options.ErrFile);
        return 1;
    }

    if (Options.Sort) {
        const auto Comparator =
            [](const ExportInfo &Lhs, const ExportInfo &Rhs) noexcept
        {
            return (Lhs.String < Rhs.String);
        };

        std::sort(ExportList.begin(), ExportList.end(), Comparator);
    }

    Operation::PrintLineSpamWarning(Options.OutFile, ExportList.size());
    fprintf(Options.OutFile,
            "Provided file has %" PRIuPTR " exports:\n",
            ExportList.size());

    auto Counter = static_cast<uint32_t>(1);

    const auto Is64Bit = Object.Is64Bit();
    const auto SizeDigitLength =
        PrintUtilsGetIntegerDigitLength(ExportList.size());

    for (const auto &Export : ExportList) {
        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fprintf(Options.OutFile,
                                         "Export %0*" PRIu32 ": ",
                                         SizeDigitLength,
                                         Counter),
                                 LENGTH_OF("Export : ") + SizeDigitLength);

        constexpr const auto LongestDescLength =
            static_cast<int>(
                MachO::ExportTrieExportKindGetLongestDescriptionLength());

        const auto IsReexport = Export.Info.getFlags().IsReexport();
        if (!IsReexport) {
            PrintUtilsWriteMachOSegmentSectionPair(Options.OutFile,
                                                   Export.SegmentName.data(),
                                                   Export.SectionName.data(),
                                                   true);

            const auto ImageOffset = Export.Info.getImageOffset();
            PrintUtilsWriteOffset(Options.OutFile, ImageOffset, Is64Bit);
        } else {
            const auto OffsetLength = (Is64Bit) ? OFFSET_64_LEN : OFFSET_32_LEN;
            const auto PadLength =
                OperationCommon::SegmentSectionPairMaxLength + OffsetLength;

            PrintUtilsPadSpaces(Options.OutFile, static_cast<int>(PadLength));
        }

        const auto KindDesc = ExportTrieExportKindGetDescription(Export.Kind);
        fprintf(Options.OutFile,
                "\t%" PRINTF_RIGHTPAD_FMT "s",
                LongestDescLength,
                KindDesc.data());

        const auto RightPad =
            static_cast<int>(LongestExportLength + LENGTH_OF("\"\""));

        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fprintf(Options.OutFile,
                                         "\"%s\"",
                                         Export.String.data()),
                                 RightPad);

        if (IsReexport) {
            const auto DylibOrdinal = Export.Info.getReexportDylibOrdinal();
            const auto ImportName = Export.Info.getReexportImportName();

            if (!ImportName.empty()) {
                fprintf(Options.OutFile,
                        " (Re-exported as %s, ",
                        ImportName.data());
            } else {
                fputs(" (Re-exported from ", Options.OutFile);
            }

            OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                                   LibraryCollection,
                                                   DylibOrdinal,
                                                   Options.Verbose);
            fputc(')', Options.OutFile);
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }

    return 0;
}

int
PrintExportTrieOperation::Run(const ConstMachOMemoryObject &Object,
                              const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Object.Is64Bit(),
                                           &SegmentCollectionError);

    auto LibraryCollectionError =
        MachO::SharedLibraryInfoCollection::Error::None;

    const auto LibraryCollection =
        MachO::SharedLibraryInfoCollection::Open(LoadCmdStorage,
                                                 &LibraryCollectionError);

    switch (LibraryCollectionError) {
        case MachO::SharedLibraryInfoCollection::Error::None:
        case MachO::SharedLibraryInfoCollection::Error::InvalidPath:
            break;
    }

    auto TrieList = TrieListType();
    auto Result =
        FindExportTrieList(Object.getConstMap(),
                           LoadCmdStorage,
                           Options,
                           TrieList);

    if (Result != 0) {
        return Result;
    }

    if (Options.PrintTree) {
        auto Error = MachO::ExportTrieEntryCollection::Error();
        auto EntryCollection =
            MachO::ExportTrieEntryCollection::Open(TrieList.getRef(),
                                                   &SegmentCollection,
                                                   &Error);

        auto Result =
            OperationCommon::HandleExportTrieParseError(Options.ErrFile, Error);

        if (Result != 0) {
            return Result;
        }

        Result =
            HandleTreeOption(EntryCollection,
                             LibraryCollection,
                             0,
                             Object.Is64Bit(),
                             Options);

        return Result;
    }

    Result =
        PrintExportTrie(Object,
                        LoadCmdStorage,
                        SegmentCollection,
                        LibraryCollection,
                        TrieList,
                        0,
                        Options);

    return Result;
}

int
PrintExportTrieOperation::Run(const ConstDscImageMemoryObject &Object,
                              const struct Options &Options) noexcept
{
    const auto Base = Object.getAddress();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto SegmentCollectionError = DscImage::SegmentInfoCollection::Error::None;
    const auto SegmentCollection =
        DscImage::SegmentInfoCollection::Open(LoadCmdStorage,
                                              Base,
                                              Object.Is64Bit(),
                                              &SegmentCollectionError);

    auto LibraryCollectionError =
        MachO::SharedLibraryInfoCollection::Error::None;

    const auto LibraryCollection =
        MachO::SharedLibraryInfoCollection::Open(LoadCmdStorage,
                                                 &LibraryCollectionError);

    switch (LibraryCollectionError) {
        case MachO::SharedLibraryInfoCollection::Error::None:
        case MachO::SharedLibraryInfoCollection::Error::InvalidPath:
            break;
    }

    auto TrieList = TrieListType();
    auto Result =
        FindExportTrieList(Object.getDscMap(),
                           LoadCmdStorage,
                           Options,
                           TrieList);

    if (Result != 0) {
        return Result;
    }

    if (Options.PrintTree) {
        auto Error = DscImage::ExportTrieEntryCollection::Error();
        auto EntryCollection =
            DscImage::ExportTrieEntryCollection::Open(Base,
                                                      TrieList.getRef(),
                                                      &SegmentCollection,
                                                      &Error);

        auto Result =
            OperationCommon::HandleExportTrieParseError(Options.ErrFile, Error);

        if (Result != 0) {
            return Result;
        }

        Result =
            HandleTreeOption(EntryCollection,
                             LibraryCollection,
                             Base,
                             Object.Is64Bit(),
                             Options);

        return Result;
    }

    Result =
        PrintExportTrie(Object,
                        LoadCmdStorage,
                        SegmentCollection,
                        LibraryCollection,
                        TrieList,
                        Base,
                        Options);

    return Result;
}

static void
AddKindRequirement(FILE *ErrFile,
                   std::vector<MachO::ExportTrieExportKind> &List,
                   ArgvArrayIterator &Argument)
{
    if (Argument.IsOption()) {
        if (List.empty()) {
            fputs("Please provide a list of export-kind requirements\n",
                  ErrFile);
            exit(1);
        }
    }

    for (; !Argument.IsOption(); Argument.advance()) {
        const auto String = Argument.GetStringView();
        const auto Kind = MachO::ExportTrieExportKindFromString(String);
        const auto ListEnd = List.cend();

        if (std::find(List.cbegin(), ListEnd, Kind) != ListEnd) {
            fprintf(ErrFile, "Note: Kind %s specified twice\n", String.data());
            continue;
        }

        List.emplace_back(Kind);
    }

    Argument.moveBack();
}

static void
AddSegmentRequirement(
    FILE *ErrFile,
    std::vector<PrintExportTrieOperation::Options::SegmentSectionPair> &List,
    ArgvArrayIterator &Argument)
{
    if (Argument.IsOption()) {
        if (List.empty()) {
            fputs("Please provide a segment-section pair requirement\n",
                  ErrFile);
            exit(1);
        }
    }

    const auto SegmentName = Argument.GetStringView();
    if (SegmentName.length() > 16) {
        fprintf(ErrFile,
                "Invalid Provided Segment \"%s\": Length too long\n",
                SegmentName.data());
        exit(1);
    }

    const auto Pair = PrintExportTrieOperation::Options::SegmentSectionPair {
        .SegmentName = SegmentName,
        .SectionName = ""
    };

    if (std::find(List.cbegin(), List.cend(), Pair) != List.cend()) {
        fprintf(ErrFile,
                "Note: Segment-Requirement \"%s\" provided twice",
                Argument.getString());
        return;
    }

    List.push_back(std::move(Pair));
}

static void
AddSectionRequirement(
    FILE *ErrFile,
    std::vector<PrintExportTrieOperation::Options::SegmentSectionPair> &List,
    ArgvArrayIterator &Argument)
{
    if (Argument.IsOption()) {
        if (List.empty()) {
            fputs("Please provide a segment requirement\n", ErrFile);
            exit(1);
        }
    }

    auto SegmentName = std::string_view();
    auto SectionName = std::string_view();

    OperationCommon::ParseSegmentSectionPair(ErrFile,
                                             Argument.GetStringView(),
                                             SegmentName,
                                             SectionName);

    const auto Pair = PrintExportTrieOperation::Options::SegmentSectionPair {
        .SegmentName = SegmentName,
        .SectionName = SectionName
    };

    if (std::find(List.cbegin(), List.cend(), Pair) != List.cend()) {
        fprintf(ErrFile,
                "Note: Segment-Section Pair \"%s\" provided twice",
                Argument.getString());
        return;
    }

    List.push_back(std::move(Pair));
}

struct PrintExportTrieOperation::Options
PrintExportTrieOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                           int *IndexOut) noexcept
{
    struct Options Options;
    for (auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--require-kind") == 0) {
            AddKindRequirement(Options.ErrFile,
                               Options.KindRequirements,
                               Argument.advance());
        } else if (strcmp(Argument, "--require-segment") == 0) {
            AddSegmentRequirement(Options.ErrFile,
                                  Options.SectionRequirements,
                                  Argument.advance());
        } else if (strcmp(Argument, "--require-section") == 0) {
            AddSectionRequirement(Options.ErrFile,
                                  Options.SectionRequirements,
                                  Argument.advance());
        } else if (strcmp(Argument, "--sort") == 0) {
            Options.Sort = true;
        } else if (strcmp(Argument, "--tree") == 0) {
            Options.PrintTree = true;
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
PrintExportTrieOperation::ParseOptions(const ArgvArray &Argv,
                                       int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int PrintExportTrieOperation::Run(const MemoryObject &Object) const noexcept {
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
