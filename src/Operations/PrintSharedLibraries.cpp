//
//  src/Operations/PrintSharedLibraries.cpp
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <ctime>
#include <vector>

#include "ADT/MachO.h"

#include "Utils/MachOTypePrinter.h"
#include "Utils/PrintUtils.h"
#include "Utils/Timestamp.h"

#include "Common.h"
#include "Operation.h"
#include "PrintSharedLibraries.h"

PrintSharedLibrariesOperation::PrintSharedLibrariesOperation() noexcept
: Operation(OpKind) {}

PrintSharedLibrariesOperation::PrintSharedLibrariesOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

struct DylibInfo {
    MachO::LoadCommand::Kind Kind;

    uint32_t Index;
    uint32_t NameOffset;

    std::string_view Name;

    Dyld3::PackedVersion CurrentVersion;
    Dyld3::PackedVersion CompatVersion;
    uint32_t Timestamp;
};

static int
CompareEntriesBySortKind(
    const DylibInfo &Lhs,
    const DylibInfo &Rhs,
    const PrintSharedLibrariesOperation::Options::SortKind SortKind) noexcept
{
    switch (SortKind) {
        case PrintSharedLibrariesOperation::Options::SortKind::ByCurrentVersion:
            if (Lhs.CurrentVersion == Rhs.CurrentVersion) {
                return 0;
            }

            if (Lhs.CurrentVersion < Rhs.CurrentVersion) {
                return -1;
            }

            return 1;
        case PrintSharedLibrariesOperation::Options::SortKind::ByCompatVersion:
        {
            if (Lhs.CompatVersion == Rhs.CompatVersion) {
                return 0;
            } else if (Lhs.CompatVersion < Rhs.CompatVersion) {
                return -1;
            }

            return 1;
        }
        case PrintSharedLibrariesOperation::Options::SortKind::ByIndex: {
            if (Lhs.Index == Rhs.Index) {
                return 0;
            } else if (Lhs.Index < Rhs.Index) {
                return -1;
            }

            return 1;
        }
        case PrintSharedLibrariesOperation::Options::SortKind::ByTimeStamp: {
            if (Lhs.Timestamp == Rhs.Timestamp) {
                return 0;
            } else if (Lhs.Timestamp < Rhs.Timestamp) {
                return -1;
            }

            return 1;
        }
        case PrintSharedLibrariesOperation::Options::SortKind::ByName:
            return Lhs.Name.compare(Rhs.Name);
    }

    assert(0 && "Unrecognized (and invalid) Sort-Kind");
}

int
PrintSharedLibrariesOperation::Run(const ConstMachOMemoryObject &Object,
                                   const struct Options &Options) noexcept
{
    constexpr static auto LongestLCKindLength =
        MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadUpwardDylib>::Name.length();

    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto LCIndex = uint32_t();
    auto DylibList = std::vector<DylibInfo>();
    auto MaxDylibNameLength = LargestIntHelper();

    for (const auto &LC : LoadCmdStorage) {
        const auto LCKind = LC.getKind(IsBigEndian);
        switch (LCKind) {
            case MachO::LoadCommand::Kind::LoadDylib:
                static_assert(MachO::LoadCommandKindInfo<
                                MachO::LoadCommand::Kind::LoadDylib>
                                    ::Name.length() <= LongestLCKindLength,
                              "LoadCommand has a longer Kind-Name than current "
                              "record-holder");
            case MachO::LoadCommand::Kind::LoadWeakDylib:
                static_assert(MachO::LoadCommandKindInfo<
                                MachO::LoadCommand::Kind::LoadWeakDylib>
                                    ::Name.length() <= LongestLCKindLength,
                              "LoadCommand has a longer Kind-Name than current "
                              "record-holder");
            case MachO::LoadCommand::Kind::ReexportDylib:
                static_assert(MachO::LoadCommandKindInfo<
                                MachO::LoadCommand::Kind::ReexportDylib>
                                    ::Name.length() <= LongestLCKindLength,
                              "LoadCommand has a longer Kind-Name than current "
                              "record-holder");
            case MachO::LoadCommand::Kind::LazyLoadDylib:
                static_assert(MachO::LoadCommandKindInfo<
                                MachO::LoadCommand::Kind::LazyLoadDylib>
                                    ::Name.length() <= LongestLCKindLength,
                              "LoadCommand has a longer Kind-Name than current "
                              "record-holder");
            case MachO::LoadCommand::Kind::LoadUpwardDylib: {
                static_assert(MachO::LoadCommandKindInfo<
                                MachO::LoadCommand::Kind::LoadUpwardDylib>
                                    ::Name.length() <= LongestLCKindLength,
                              "LoadCommand has a longer Kind-Name than current "
                              "record-holder");

                const auto &Dylib = cast<MachO::DylibCommand>(LC, IsBigEndian);
                const auto GetNameResult = Dylib.GetName(IsBigEndian);
                const auto &Name =
                    OperationCommon::GetLoadCommandStringValue(GetNameResult);

                MaxDylibNameLength = Name.length();
                DylibList.push_back({
                    .Kind = LCKind,
                    .Index = LCIndex,
                    .Name = Name,
                    .CurrentVersion = Dylib.Info.getCurrentVersion(IsBigEndian),
                    .CompatVersion = Dylib.Info.getCompatVersion(IsBigEndian),
                    .Timestamp = Dylib.Info.getTimestamp(IsBigEndian)
                });

                break;
            }

            case MachO::LoadCommand::Kind::Segment:
            case MachO::LoadCommand::Kind::SymbolTable:
            case MachO::LoadCommand::Kind::SymbolSegment:
            case MachO::LoadCommand::Kind::Thread:
            case MachO::LoadCommand::Kind::UnixThread:
            case MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary:
            case MachO::LoadCommand::Kind::IdFixedVMSharedLibrary:
            case MachO::LoadCommand::Kind::Ident:
            case MachO::LoadCommand::Kind::FixedVMFile:
            case MachO::LoadCommand::Kind::PrePage:
            case MachO::LoadCommand::Kind::DynamicSymbolTable:
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
            case MachO::LoadCommand::Kind::FileSetEntry:
                break;
        }

        LCIndex++;
    }

    if (!Options.SortKindList.empty()) {
        const auto Comparator = [&](const auto &Lhs, const auto &Rhs) noexcept {
            for (const auto &Sort : Options.SortKindList) {
                const auto Compare = CompareEntriesBySortKind(Lhs, Rhs, Sort);
                if (Compare != 0) {
                    return (Compare < 0);
                }
            }

            return false;
        };

        std::sort(DylibList.begin(), DylibList.end(), Comparator);
    }

    const auto DylibListSize = DylibList.size();
    fprintf(Options.OutFile,
            "Provided file has %" PRIuPTR " Shared Libraries:\n",
            DylibListSize);

    const auto LCCountDigitLength = LoadCmdStorage.size();
    const auto DylibSizeDigitLength =
        PrintUtilsGetIntegerDigitLength(DylibListSize);

    auto Counter = static_cast<uint32_t>(1);
    for (const auto &DylibInfo : DylibList) {
        fprintf(Options.OutFile,
                "Library %0*" PRIu32 ": LC %0*" PRIu32 ": "
                "%" PRINTF_RIGHTPAD_FMT "s"
                "\t",
                DylibSizeDigitLength,
                Counter,
                LCCountDigitLength,
                DylibInfo.Index,
                static_cast<int>(LongestLCKindLength),
                MachO::LoadCommand::KindGetName(DylibInfo.Kind).data());

        const auto WrittenOut =
            fprintf(Options.OutFile, "\"%s\"", DylibInfo.Name.data());

        if (Options.Verbose) {
            const auto RightPad =
                static_cast<int>(MaxDylibNameLength + LENGTH_OF("\"\""));

            PrintUtilsRightPadSpaces(Options.OutFile, WrittenOut, RightPad);
            const struct MachO::DylibCommand::Info Info = {
                .Name = { .Offset = DylibInfo.NameOffset },
                .Timestamp = DylibInfo.Timestamp,
                .CurrentVersion = DylibInfo.CurrentVersion.value(),
                .CompatibilityVersion = DylibInfo.CompatVersion.value()
            };

            MachOTypePrinter<struct MachO::DylibCommand::Info>::PrintOnOneLine(
                Options.OutFile, Info, IsBigEndian, PrintKind::Verbose, " (",
                ")");
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }

    return 0;
}

[[nodiscard]] static inline bool
ListHasSortKind(
    const std::vector<PrintSharedLibrariesOperation::Options::SortKind> &List,
    const PrintSharedLibrariesOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(PrintSharedLibrariesOperation::Options::SortKind SortKind,
            const char *Argument,
            struct PrintSharedLibrariesOperation::Options &Options) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Argument);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

struct PrintSharedLibrariesOperation::Options
PrintSharedLibrariesOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                int *const IndexOut) noexcept
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort-by-current-version") == 0) {
            AddSortKind(Options::SortKind::ByCurrentVersion, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-compat-version") == 0) {
            AddSortKind(Options::SortKind::ByCompatVersion, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-index") == 0) {
            AddSortKind(Options::SortKind::ByIndex, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-name") == 0) {
            AddSortKind(Options::SortKind::ByName, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-timestamp") == 0) {
            AddSortKind(Options::SortKind::ByTimeStamp, Argument, Options);
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

int
PrintSharedLibrariesOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintSharedLibrariesOperation::Run(const MemoryObject &Object) const noexcept {
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
