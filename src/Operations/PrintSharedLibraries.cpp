//
//  include/Operations/PrintSharedLibraries.h
//  stool
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

void PrintSharedLibrariesOperation::run(const ConstMemoryObject &Object)
noexcept {
    run(Object, Options);
}

void
PrintSharedLibrariesOperation::run(const ConstMachOMemoryObject &Object,
                                   const struct Options &Options) noexcept
{
    constexpr static const auto LongestLCKindLength =
        MachO::LoadCommandKindInfo<
            MachO::LoadCommand::Kind::LoadUpwardDylib>::Name.length();

    struct DylibInfo {
        uint32_t Index;
        MachO::LoadCommand::Kind Kind;
        std::string_view Name;

        struct MachO::DylibCommand::Info Info;
    };

    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCommandStorage =
        OperationCommon::GetConstLoadCommandStorage(Object);

    auto LCIndex = uint32_t();
    auto DylibList = std::vector<DylibInfo>();
    auto MaxDylibNameLength = std::string_view::size_type();

    for (const auto &LoadCmd : LoadCommandStorage) {
        const auto LCKind = LoadCmd.GetKind(IsBigEndian);
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
                    cast<const MachO::DylibCommand &>(LoadCmd, IsBigEndian);

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

    fprintf(stdout, "%" PRIuPTR " Shared Libraries:\n", DylibList.size());
    auto Counter = static_cast<uint32_t>(1);

    for (const auto &DylibInfo : DylibList) {
        fprintf(stdout,
                "Lib %02" PRIu32 ": LC %02" PRIu32 ": %" PRINTF_RIGHTPAD_FMT "s"
                "\t",
                Counter,
                DylibInfo.Index,
                static_cast<int>(LongestLCKindLength),
                MachO::LoadCommand::KindGetName(DylibInfo.Kind).data());

        fprintf(stdout,
                "%" PRINTF_RIGHTPAD_FMT "s",
                static_cast<int>(MaxDylibNameLength),
                DylibInfo.Name.data());

        MachOTypePrinter<struct MachO::DylibCommand::Info>::PrintOnOneLine(
            stdout, DylibInfo.Info, IsBigEndian, Options.Verbose, " (", ")\n");

        Counter++;
    }
}

struct PrintSharedLibrariesOperation::Options
PrintSharedLibrariesOperation::ParseOptionsImpl(int Argc,
                                                const char *Argv[],
                                                int *IndexOut) noexcept
{
    struct Options Options;
    for (auto I = int(); I != Argc; I++) {
        const auto Argument = Argv[I];
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
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

struct PrintSharedLibrariesOperation::Options *
PrintSharedLibrariesOperation::ParseOptions(int Argc,
                                            const char *Argv[],
                                            int *IndexOut) noexcept
{
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

void
PrintSharedLibrariesOperation::run(const ConstMemoryObject &Object,
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
PrintSharedLibrariesOperation::run(const ConstMemoryObject &Object,
                                   int Argc,
                                   const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
