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

#include "Utils/MachOPrinter.h"
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

int
PrintSharedLibrariesOperation::Run(const ConstMachOMemoryObject &Object,
                                   const struct Options &Options) noexcept
{
    constexpr static auto LongestLCKindLength =
        MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadUpwardDylib>::Name.length();

    struct DylibInfo {
        uint32_t Index;
        MachO::LoadCommand::Kind Kind;
        std::string_view Name;

        struct MachO::DylibCommand::Info Info;
    };

    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto LCIndex = uint32_t();
    auto DylibList = std::vector<DylibInfo>();
    auto MaxDylibNameLength = std::string_view::size_type();

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto LCKind = LoadCmd.getKind(IsBigEndian);
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

                const auto &DylibCmd =
                    cast<MachO::DylibCommand>(LoadCmd, IsBigEndian);

                const auto GetNameResult = DylibCmd.GetName(IsBigEndian);
                const auto &Name =
                    OperationCommon::GetLoadCommandStringValue(GetNameResult);

                if (MaxDylibNameLength < Name.length()) {
                    MaxDylibNameLength = Name.length();
                }

                DylibList.push_back({
                    .Index = LCIndex,
                    .Kind = LCKind,
                    .Name = Name,
                    .Info = DylibCmd.Info
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
                break;
        }

        LCIndex++;
    }

    fprintf(Options.OutFile,
            "Provided file has %" PRIuPTR " Shared Libraries:\n",
            DylibList.size());

    auto Counter = static_cast<uint32_t>(1);
    for (const auto &DylibInfo : DylibList) {
        fprintf(Options.OutFile,
                "Lib %02" PRIu32 ": LC %02" PRIu32 ": %" PRINTF_RIGHTPAD_FMT "s"
                "\t",
                Counter,
                DylibInfo.Index,
                static_cast<int>(LongestLCKindLength),
                MachO::LoadCommand::KindGetName(DylibInfo.Kind).data());

        const auto WrittenOut =
            fprintf(Options.OutFile, "\"%s\"", DylibInfo.Name.data());

        if (Options.Verbose) {
            PrintUtilsRightPadSpaces(Options.OutFile,
                                     WrittenOut,
                                     static_cast<int>(MaxDylibNameLength) + 2);

            MachOTypePrinter<struct MachO::DylibCommand::Info>::PrintOnOneLine(
                Options.OutFile, DylibInfo.Info, IsBigEndian, true, " (",
                ")");
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }

    return 0;
}

struct PrintSharedLibrariesOperation::Options
PrintSharedLibrariesOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                                int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
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
PrintSharedLibrariesOperation::ParseOptions(const ArgvArray &Argv,
                                            int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintSharedLibrariesOperation::Run(const MemoryObject &Object) const noexcept {
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
