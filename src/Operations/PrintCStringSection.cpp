//
//  Operation/PrintCStringSection.cpp
//  ktool
//
//  Created by Suhas Pai on 6/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "Operations/Common.h"
#include "Operations/Operation.h"
#include "Operations/PrintCStringSection.h"

PrintCStringSectionOperation::PrintCStringSectionOperation() noexcept
: Operation(OpKind) {}

PrintCStringSectionOperation::PrintCStringSectionOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

struct StringInfo {
    std::string_view String;

    uint64_t Offset;
    uint64_t Addr;

    [[nodiscard]] inline bool operator<(const StringInfo &Rhs) const noexcept {
        return (String < Rhs.String);
    }
};

[[nodiscard]]
static inline bool ShouldExcludeString(std::string_view String) noexcept {
    if (String.empty()) {
        return true;
    }

    for (const auto &Ch : String) {
        if (isprint(Ch) == 0) {
            return true;
        }
    }

    return false;
}

static void
GetCStringList(const uint8_t *const Map,
               const MachO::SectionInfo &Section,
               std::vector<StringInfo> &StringList,
               LargestIntHelper<uint64_t> &LongestStringLength) noexcept
{
    const auto Begin = Section.getData<const char>(Map);
    const auto End = Section.getDataEnd<const char>(Map);

    auto FileOffset = Section.getFileRange().getBegin();
    auto VmAddr = Section.getMemoryRange().getBegin();

    auto Length = uint64_t();
    for (auto String = Begin;
         String < End;
         String += (Length + 1),
         FileOffset += (Length + 1),
         VmAddr += (Length + 1))
    {
        Length = strnlen(String, (End - String));

        const auto StringView = std::string_view(String, Length);
        if (ShouldExcludeString(StringView)) {
            continue;
        }

        LongestStringLength = Length;
        StringList.emplace_back(StringInfo {
            .String = StringView,
            .Offset = FileOffset,
            .Addr = VmAddr
        });
    }
}

static int
PrintCStringList(
    const uint8_t *const MapBegin,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const bool Is64Bit,
    const struct PrintCStringSectionOperation::Options &Options) noexcept
{
    auto Error = MachO::SegmentInfoCollection::Error::None;
    auto Segment = std::unique_ptr<MachO::SegmentInfo>();

    const auto Section =
        MachO::SegmentInfoCollection::OpenSectionInfoWithName(
            LoadCmdStorage,
            Is64Bit,
            Options.SegmentName,
            Options.SectionName,
            &Segment,
            &Error);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile, Error);
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

    if (Section == nullptr) {
        fprintf(Options.ErrFile,
                "Provided file has no section with name \"" STRING_VIEW_FMT
                "\" in provided segment\n",
                STRING_VIEW_FMT_ARGS(Options.SectionName));
        return 1;
    }

    if (Section->getKind() != MachO::SegmentSectionKind::CStringLiterals) {
        fprintf(Options.OutFile,
                "Provided Segment-Section \"%s\",\"%s\" is not a C-String "
                "Literal Section\n",
                Segment->getName().data(),
                Section->getName().data());
        return 0;
    }

    if (Section->getFileRange().empty()) {
        fputs("C-String Section is empty\n", Options.OutFile);
        return 0;
    }

    auto InfoList = std::vector<StringInfo>();
    auto LongestStringLength = LargestIntHelper();

    GetCStringList(MapBegin,
                   *Section.get(),
                   InfoList,
                   LongestStringLength);

    const auto LongestStringLengthDigitLength =
        PrintUtilsGetIntegerDigitLength(LongestStringLength);

    if (Options.Sort) {
        std::sort(InfoList.begin(), InfoList.end());
    }

    const auto InfoListSize = InfoList.size();
    Operation::PrintLineSpamWarning(Options.OutFile, InfoListSize);

    fprintf(Options.OutFile,
            "Provided section has %" PRIuPTR " C-Strings:\n",
            InfoListSize);

    auto Counter = static_cast<uint64_t>(1);
    const auto StringListSizeDigitLength =
        PrintUtilsGetIntegerDigitLength(InfoListSize);

    for (const auto &Info : InfoList) {
        fprintf(Options.OutFile,
                "C-String %0*" PRIu64 ": ",
                StringListSizeDigitLength,
                Counter);

        PrintUtilsWriteOffset32Or64(Options.OutFile, Is64Bit, Info.Addr);
        const auto PrintLength =
            fprintf(Options.OutFile, " \"%s\"", Info.String.data());

        if (Options.Verbose) {
            const auto RightPad =
                static_cast<int>(LongestStringLength + LENGTH_OF(" \"\""));

            PrintUtilsRightPadSpaces(Options.OutFile, PrintLength, RightPad);
            fprintf(Options.OutFile,
                    " (Length: %0*" PRIuPTR ", File: ",
                    LongestStringLengthDigitLength,
                    Info.String.length());

            PrintUtilsWriteOffset32Or64(Options.OutFile,
                                        Is64Bit,
                                        Info.Offset,
                                        false,
                                        "",
                                        ")");
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }

    return 0;
}

int
PrintCStringSectionOperation::Run(const DscImageMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    const auto Result =
        PrintCStringList(Object.getDscMap().getBegin(),
                         LoadCmdStorage,
                         Object.is64Bit(),
                         Options);

    return Result;
}


int
PrintCStringSectionOperation::Run(const MachOMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    const auto Result =
        PrintCStringList(Object.getMap().getBegin(),
                         LoadCmdStorage,
                         Object.is64Bit(),
                         Options);

    return Result;
}

auto
PrintCStringSectionOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                               int *const IndexOut) noexcept
    -> struct PrintCStringSectionOperation::Options
{
    struct Options Options;

    auto DidGetInfo = false;
    auto Index = int();

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort") == 0) {
            Options.Sort = true;
        } else if (!Argument.isOption()) {
            if (DidGetInfo) {
                break;
            }

            OperationCommon::ParseSegmentSectionPair(Options.ErrFile,
                                                     Argument.GetStringView(),
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

int PrintCStringSectionOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintCStringSectionOperation::Run(const MemoryObject &Object) const noexcept {
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
