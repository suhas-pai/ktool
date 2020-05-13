//
//  src/Operations/PrintLoadCommands.cpp
//  stool
//
//  Created by Suhas Pai on 4/10/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstdio>
#include <cstring>

#include "ADT/Mach-O/LoadCommandStorage.h"
#include "ADT/Mach-O/LoadCommands.h"

#include "Objects/FatMachOMemory.h"

#include "Utils/MachOLoadCommandPrinter.h"
#include "Utils/SwitchEndian.h"

#include "Common.h"
#include "Operation.h"
#include "PrintLoadCommands.h"

PrintLoadCommandsOperation::PrintLoadCommandsOperation() noexcept
: PrintOperation(OpKind) {}

PrintLoadCommandsOperation::PrintLoadCommandsOperation(
    const struct Options &Options) noexcept
: PrintOperation(OpKind), Options(Options) {}

void PrintLoadCommandsOperation::run(const ConstMemoryObject &Object) noexcept {
    run(Object, Options);
}

void
PrintLoadCommandsOperation::run(const ConstMachOMemoryObject &Object,
                                const struct Options &Options) noexcept
{
    const auto IsBigEndian = Object.IsBigEndian();
    const auto Is64Bit = Object.Is64Bit();

    const auto LoadCommandStorage =
        OperationCommon::GetConstLoadCommandStorage(Object);

    auto LoadCmdCounter = uint32_t();
    for (const auto &LoadCmd : LoadCommandStorage) {
        fprintf(Options.OutFile, "LC %02d: ", LoadCmdCounter);

        const auto Kind = LoadCmd.GetKind(IsBigEndian);
        if (!MachO::LoadCommand::KindIsValid(Kind)) {
            fprintf(Options.OutFile,
                    "Unrecognized LoadCmd (%" PRIu32 ")\n",
                    static_cast<uint32_t>(Kind));
            continue;
        }

        switch (Kind) {
            case MachO::LoadCommand::Kind::Segment:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Segment>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Segment>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SymbolTable:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SymbolTable>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SymbolTable>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SymbolSegment:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SymbolSegment>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SymbolSegment>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Thread:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Thread>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Thread>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::UnixThread:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::UnixThread>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::UnixThread>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Segment64:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Segment64>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Segment64>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary>::Print(
                        Options.OutFile, Object.GetRange(), cast<
                            MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary>(
                                LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::IdFixedVMSharedLibrary:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>::Print(
                        Options.OutFile, Object.GetRange(), cast<
                            MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>(
                                LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Ident:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Ident>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Ident>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::FixedVMFile:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::FixedVMFile>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::FixedVMFile>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::PrePage:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::PrePage>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::PrePage>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DynamicSymbolTable:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DynamicSymbolTable>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DynamicSymbolTable>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadDylib>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LoadDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::IdDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::IdDylib>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::IdDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadDylinker:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadDylinker>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LoadDylinker>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::IdDylinker:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::IdDylinker>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::IdDylinker>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::PreBoundDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::PreBoundDylib>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::PreBoundDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Routines:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Routines>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Routines>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubFramework:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubFramework>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SubFramework>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubUmbrella:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubUmbrella>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SubUmbrella>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubClient:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubClient>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SubClient>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubLibrary:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubLibrary>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SubLibrary>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::TwoLevelHints:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::TwoLevelHints>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::TwoLevelHints>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::PrebindChecksum:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::PrebindChecksum>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::PrebindChecksum>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadWeakDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadWeakDylib>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LoadWeakDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Routines64:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Routines64>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Routines64>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Uuid:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Uuid>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Uuid>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Rpath:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Rpath>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Rpath>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::CodeSignature:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::CodeSignature>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::CodeSignature>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SegmentSplitInfo:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SegmentSplitInfo>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SegmentSplitInfo>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::ReexportDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::ReexportDylib>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::ReexportDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LazyLoadDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LazyLoadDylib>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LazyLoadDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::EncryptionInfo:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::EncryptionInfo>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::EncryptionInfo>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldInfo:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldInfo>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DyldInfo>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldInfoOnly:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldInfoOnly>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DyldInfoOnly>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);

                break;
            case MachO::LoadCommand::Kind::LoadUpwardDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadUpwardDylib>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LoadUpwardDylib>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumMacOSX:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumMacOSX>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::VersionMinimumMacOSX>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumIPhoneOS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumIPhoneOS>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::VersionMinimumIPhoneOS>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::FunctionStarts:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::FunctionStarts>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::FunctionStarts>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldEnvironment:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldEnvironment>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DyldEnvironment>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Main:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Main>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Main>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DataInCode:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DataInCode>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DataInCode>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SourceVersion:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SourceVersion>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::SourceVersion>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DylibCodeSignDRS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DylibCodeSignDRS>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DylibCodeSignDRS>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::EncryptionInfo64:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::EncryptionInfo64>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::EncryptionInfo64>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LinkerOption:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LinkerOption>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LinkerOption>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LinkerOptimizationHint:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LinkerOptimizationHint>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::LinkerOptimizationHint>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumTvOS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumTvOS>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::VersionMinimumTvOS>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumWatchOS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumWatchOS>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::VersionMinimumWatchOS>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Note:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Note>::Print(Options.OutFile,
                        Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::Note>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::BuildVersion:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::BuildVersion>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::BuildVersion>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldExportsTrie:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldExportsTrie>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DyldExportsTrie>(LoadCmd,
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldChainedFixups:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldChainedFixups>::Print(
                        Options.OutFile, Object.GetRange(),
                        cast<MachO::LoadCommand::Kind::DyldChainedFixups>(
                            LoadCmd, IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
        }

        LoadCmdCounter++;
    }
}

struct PrintLoadCommandsOperation::Options
PrintLoadCommandsOperation::ParseOptionsImpl(int Argc,
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

struct PrintLoadCommandsOperation::Options *
PrintLoadCommandsOperation::ParseOptions(int Argc,
                                         const char *Argv[],
                                         int *IndexOut) noexcept
{
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

void
PrintLoadCommandsOperation::run(const ConstMemoryObject &Object,
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
PrintLoadCommandsOperation::run(const ConstMemoryObject &Object,
                                int Argc,
                                const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
