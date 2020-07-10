//
//  src/Operation/PrintCStringSection.cpp
//  ktool
//
//  Created by Suhas Pai on 6/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Common.h"
#include "Operation.h"
#include "PrintCStringSection.h"

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

[[nodiscard]] static
inline bool ShouldExcludeString(const std::string_view &String) noexcept {
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
GetCStringList(FILE *OutFile,
               const uint8_t *Map,
               const MachO::SectionInfo *Section,
               std::vector<StringInfo> &StringList,
               LargestIntHelper<uint64_t> &LongestStringLength) noexcept
{
    if (Section->Memory.empty()) {
        fputs("C-String Section is empty\n", OutFile);
        return;
    }

    const auto Begin = Section->getData<const char>(Map);
    const auto End = Section->getDataEnd<const char>(Map);

    auto FileOffset = Section->File.getBegin();
    auto VmAddr = Section->Memory.getBegin();

    auto Length = uint64_t();
    for (auto String = Begin;
         String <= End;
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

int
PrintCStringSectionOperation::Run(const ConstMachOMemoryObject &Object,
                                  const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.Is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    OperationCommon::HandleSegmentCollectionError(Options.ErrFile,
                                                  SegmentCollectionError);

    const auto Segment =
        SegmentCollection.GetInfoForName(Options.SegmentName);

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

    if (Section->getKind() != MachO::SegmentSectionKind::CStringLiterals) {
        fprintf(Options.OutFile,
                "Provided Segment-Section \"%s\",\"%s\" is not a C-String "
                "Literal Section\n",
                Segment->Name.data(),
                Section->Name.data());
        return 0;
    }

    const auto MapBegin = Object.getMap().getBegin();

    auto InfoList = std::vector<StringInfo>();
    auto LongestStringLength = LargestIntHelper();

    GetCStringList(Options.OutFile,
                   MapBegin,
                   Section,
                   InfoList,
                   LongestStringLength);

    const auto LongestStringLengthDigitLength =
        PrintUtilsGetIntegerDigitLength(LongestStringLength);

    if (Options.Sort) {
        std::sort(InfoList.begin(), InfoList.end());
    }

    PrintLineSpamWarning(Options.OutFile, InfoList.size());
    fprintf(Options.OutFile,
            "Provided section has %" PRIuPTR " C-Strings:\n",
            InfoList.size());

    auto Counter = static_cast<uint64_t>(1);
    const auto StringListSizeDigithLength =
        PrintUtilsGetIntegerDigitLength(InfoList.size());

    for (const auto &Info : InfoList) {
        const auto &String = Info.String;
        fprintf(Options.OutFile,
                "C-String %0*" PRIu64 ": ",
                StringListSizeDigithLength,
                Counter);

        PrintUtilsPrintOffset(Options.OutFile, Info.Addr, Is64Bit);
        const auto PrintLength =
            fprintf(Options.OutFile, " \"%s\"", String.data());

        if (Options.Verbose) {
            const auto RightPad =
                static_cast<int>(LongestStringLength + LENGTH_OF(" \"\""));

            PrintUtilsRightPadSpaces(Options.OutFile, PrintLength, RightPad);
            fprintf(Options.OutFile,
                    " (Length: %0*" PRIuPTR ", File: ",
                    LongestStringLengthDigitLength,
                    String.length());

            PrintUtilsPrintOffset(Options.OutFile, Info.Offset, Is64Bit);
            fputc(')', Options.OutFile);
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }

    return 0;
}

struct PrintCStringSectionOperation::Options
PrintCStringSectionOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                               int *IndexOut) noexcept
{
    auto DidGetInfo = false;
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort") == 0) {
            Options.Sort = true;
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
PrintCStringSectionOperation::ParseOptions(const ArgvArray &Argv,
                                           int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintCStringSectionOperation::Run(const MemoryObject &Object) const noexcept {
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
