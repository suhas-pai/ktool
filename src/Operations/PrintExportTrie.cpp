//
//  src/Operations/PrintExportTrie.cpp
//  stool
//
//  Created by Suhas Pai on 5/9/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "ADT/MachO.h"

#include "Utils/MachOPrinter.h"
#include "Utils/PrintUtils.h"
#include "Utils/MiscTemplates.h"

#include "Common.h"
#include "Operation.h"
#include "PrintExportTrie.h"

PrintExportTrieOperation::PrintExportTrieOperation() noexcept
: PrintOperation(OpKind) {}

PrintExportTrieOperation::PrintExportTrieOperation(
    const struct Options &Options) noexcept
: PrintOperation(OpKind), Options(Options) {}

void PrintExportTrieOperation::run(const ConstMemoryObject &Object) noexcept {
    run(Object, Options);
}

static void
PrintReexportDylibPath(const std::vector<std::string_view> &DylibList,
                       uint32_t DylibOrdinal,
                       const struct PrintExportTrieOperation::Options &Options)
noexcept {
    const auto DylibIndex = DylibOrdinal - 1;
    if (DylibOrdinal == 0 || !IndexOutOfBounds(DylibIndex, DylibList.size())) {
        fprintf(Options.OutFile, "%s", DylibList.at(DylibIndex).data());
    } else {
        fputs("(Ordinal Out-Of-Bounds!)", Options.OutFile);
    }
}

static void
PrintExport(const MachO::ExportTrieSymbol &Export,
            const std::vector<std::string_view> &DylibList,
            const struct PrintExportTrieOperation::Options &Options,
            uint64_t LongestExportLength,
            bool Is64Bit) noexcept
{
    using namespace MachO;

    const auto TypeDesc = ExportTrieSymbolTypeGetDescription(Export.GetType());
    const auto IsReexport =
        (Export.GetType() == MachO::ExportTrieSymbolType::Reexport);

    constexpr const auto LongestDescLength =
        static_cast<int>(ExportTrieSymbolTypeGetLongestDescriptionLength());

    if (!IsReexport) {
        if (Is64Bit) {
            fprintf(Options.OutFile, OFFSET_64_FMT, Export.GetImageOffset());
        } else {
            fprintf(Options.OutFile,
                    OFFSET_32_FMT,
                    static_cast<uint32_t>(Export.GetImageOffset()));
        }
    } else {
        if (Is64Bit) {
            PrintUtilsPadSpaces(Options.OutFile, OFFSET_64_LEN);
        } else {
            PrintUtilsPadSpaces(Options.OutFile, OFFSET_32_LEN);
        }
    }

    fprintf(Options.OutFile,
            "\t%" PRINTF_RIGHTPAD_FMT "s",
            static_cast<int>(LongestDescLength),
            TypeDesc.data());

    if (Options.PrintReexportDylibPaths) {
        fprintf(Options.OutFile,
                "%" PRINTF_RIGHTPAD_FMT "s",
                static_cast<int>(LongestExportLength),
                Export.GetString().data());

        if (IsReexport) {
            PrintReexportDylibPath(DylibList,
                                   Export.GetReexportDylibOrdinal(),
                                   Options);
        }

        fputc('\n', Options.OutFile);
    } else {
        fprintf(Options.OutFile, "%s\n", Export.GetString().data());
    }
}

static void
PrintExportVerbose(const MachO::ExportTrieSymbol &Export,
                   const std::vector<std::string_view> &DylibList,
                   const struct PrintExportTrieOperation::Options &Options,
                   uint64_t LongestExportLength,
                   bool Is64Bit) noexcept
{
    using namespace MachO;
    constexpr const auto LongestDescLength =
        static_cast<int>(ExportTrieSymbolTypeGetLongestDescriptionLength());

    const auto TypeDesc = ExportTrieSymbolTypeGetDescription(Export.GetType());
    const auto TypeName = ExportTrieSymbolTypeGetName(Export.GetType());
    const auto IsReexport =
        (Export.GetType() == MachO::ExportTrieSymbolType::Reexport);

    constexpr const auto LongestNameLength =
        static_cast<int>(ExportTrieSymbolTypeGetLongestNameLength());

    if (!IsReexport) {
        if (Is64Bit) {
            fprintf(Options.OutFile, OFFSET_64_FMT, Export.GetImageOffset());
        } else {
            fprintf(Options.OutFile,
                    OFFSET_32_FMT,
                    static_cast<uint32_t>(Export.GetImageOffset()));
        }
    } else {
        if (Is64Bit) {
            PrintUtilsPadSpaces(Options.OutFile, OFFSET_64_LEN);
        } else {
            PrintUtilsPadSpaces(Options.OutFile, OFFSET_32_LEN);
        }
    }

    fprintf(Options.OutFile,
            "\t%" PRINTF_RIGHTPAD_FMT "s",
            LongestDescLength,
            TypeDesc.data());

    fprintf(Options.OutFile,
            "%" PRINTF_RIGHTPAD_FMT "s",
            static_cast<int>(LongestExportLength),
            Export.GetString().data());

    fprintf(Options.OutFile,
            " %" PRINTF_RIGHTPAD_FMT "s",
            LongestNameLength,
            TypeName.data());

    if (Export.GetType() == MachO::ExportTrieSymbolType::Reexport) {
        if (!Export.GetReexportImportName().empty()) {
            fprintf(Options.OutFile,
                    " (Re-exported from %s, Ordinal: %" PRIu32 ")",
                    Export.GetReexportImportName().data(),
                    Export.GetReexportDylibOrdinal());
        } else {
            const auto DylibOrdinal = Export.GetReexportDylibOrdinal();
            fprintf(Options.OutFile,
                    " (Re-exported from Dylib-Ordinal %02" PRIu32,
                    DylibOrdinal);

            if (Options.PrintReexportDylibPaths) {
                fputs(" - ", Options.OutFile);
                PrintReexportDylibPath(DylibList, DylibOrdinal, Options);
            }

            fputc(')', Options.OutFile);
        }
    }

    fputc('\n', Options.OutFile);
}

void
PrintExportTrieOperation::run(const ConstMachOMemoryObject &Object,
                              const struct Options &Options) noexcept
{
    using namespace MachO;

    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object);

    auto FoundTrieCount = uint32_t();
    auto DylibList = std::vector<std::string_view>();
    auto TrieList =
        TypedAllocationOrError<ConstExportTrieList *, SizeRangeError>();

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto Kind = LoadCmd.GetKind(IsBigEndian);
        if (LoadCommand::KindIsSharedLibrary(Kind)) {
            if (!Options.PrintReexportDylibPaths) {
                continue;
            }

            const auto &Dylib = cast<DylibCommand>(LoadCmd, IsBigEndian);
            const auto GetNameResult = Dylib.GetName(IsBigEndian);
            const auto &Name =
                OperationCommon::GetLoadCommandStringValue(GetNameResult);

            DylibList.emplace_back(Name);
            continue;
        }

        auto LCExportOff = uint32_t();
        auto LCExportSize = uint32_t();

        const auto *DyldInfo = dyn_cast<DyldInfoCommand>(LoadCmd, IsBigEndian);
        if (DyldInfo != nullptr) {
            TrieList = DyldInfo->GetConstExportTrieList(Object.GetMap(),
                                                        IsBigEndian);
            FoundTrieCount++;
        } else {
            const auto *DyldExportsTrie =
                dyn_cast<LoadCommand::Kind::DyldExportsTrie>(LoadCmd,
                                                             IsBigEndian);

            if (DyldExportsTrie != nullptr) {
                LCExportOff =
                    SwitchEndianIf(DyldExportsTrie->DataOff, IsBigEndian);
                LCExportSize =
                    SwitchEndianIf(DyldExportsTrie->DataSize, IsBigEndian);

                FoundTrieCount++;
            }
        }

        if (FoundTrieCount == 2) {
            fputs("Provided file has multiple export-tries\n", Options.ErrFile);
            exit(1);
        }
    }

    // Not having an export-trie is not an error.

    if (FoundTrieCount == 0) {
        fputs("Provided file does not have an export-trie\n", Options.OutFile);
        return;
    }

    switch (TrieList.GetError()) {
        case MachO::SizeRangeError::None:
            break;
        case MachO::SizeRangeError::Overflows:
        case MachO::SizeRangeError::PastEnd:
            fputs("Provided file has an export-trie that extends past "
                  "end-of-file",
                  Options.ErrFile);
            exit(1);
    }

    auto ExportList = std::vector<ExportTrieSymbol>();
    auto LongestExportLength = uint64_t();

    // A very large allocation, but many dylibs seem to have anywhere between
    // 1000-3000 symbols in the export-trie.

    ExportList.reserve(1000);

    for (auto Symbol : TrieList.GetRef()) {
        const auto String = Symbol.GetString();
        const auto StringLength = String.length();

        if (LongestExportLength < StringLength) {
            LongestExportLength = StringLength;
        }

        ExportList.emplace_back(std::move(Symbol));
    }

    if (ExportList.empty()) {
        fputs("Provided file has no exports in export-trie\n", Options.ErrFile);
        exit(1);
    }

    fprintf(Options.OutFile,
            "Provided file has %ld exports:\n",
            ExportList.size());

    auto Counter = static_cast<uint32_t>(1);
    const auto Is64Bit = Object.Is64Bit();

    for (const auto &Export : ExportList) {
        PrintUtilsRightPadSpaces(Options.OutFile,
                                 fprintf(Options.OutFile,
                                         "Export %02" PRIu32 ":",
                                         Counter),
                                 LENGTH_OF("Export 0000:"));

        fputc('\t', Options.OutFile);

        if (Options.Verbose) {
            PrintExportVerbose(Export,
                               DylibList,
                               Options,
                               LongestExportLength,
                               Is64Bit);
        } else {
            PrintExport(Export,
                        DylibList,
                        Options,
                        LongestExportLength,
                        Is64Bit);
        }

        Counter++;
    }
}

struct PrintExportTrieOperation::Options
PrintExportTrieOperation::ParseOptionsImpl(int Argc,
                                           const char *Argv[],
                                           int *IndexOut) noexcept
{
    struct Options Options;
    for (auto I = int(); I != Argc; I++) {
        const auto Argument = Argv[I];
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--print-reexport-dylibs") == 0) {
            Options.PrintReexportDylibPaths = true;
        } else if (Argument[0] != '-') {
            if (IndexOut != nullptr) {
                *IndexOut = I;
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument);
            exit(1);
        }
    }

    return Options;
}

struct PrintExportTrieOperation::Options *
PrintExportTrieOperation::ParseOptions(int Argc,
                                       const char *Argv[],
                                       int *IndexOut) noexcept
{
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

void
PrintExportTrieOperation::run(const ConstMemoryObject &Object,
                              const struct Options &Options) noexcept
{
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object Type is None");
        case ObjectKind::MachO:
            run(cast<ObjectKind::MachO>(Object), Options);
            break;
        case ObjectKind::FatMachO:
            PrintObjectKindNotSupportedError(OpKind, Object);
            exit(1);
    }
}

void
PrintExportTrieOperation::run(const ConstMemoryObject &Object,
                              int Argc,
                              const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
