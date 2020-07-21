//
//  src/Operations/PrintLoadCommands.cpp
//  ktool
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
: Operation(OpKind) {}

PrintLoadCommandsOperation::PrintLoadCommandsOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static void
IterateLoadCommands(
    const ConstMachOMemoryObject &Object,
    const RelativeRange &Range,
    const MachO::ConstLoadCommandStorage &LoadCmdStorage,
    const struct PrintLoadCommandsOperation::Options &Options) noexcept
{
    const auto IsBigEndian = Object.IsBigEndian();
    const auto Is64Bit = Object.IsBigEndian();

    auto LoadCmdCounter = uint32_t();
    for (const auto &LoadCmd : LoadCmdStorage) {
        fprintf(Options.OutFile, "LC %02d: ", LoadCmdCounter);

        const auto Kind = LoadCmd.getKind(IsBigEndian);
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
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Segment>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SymbolTable:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SymbolTable>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SymbolTable>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SymbolSegment:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SymbolSegment>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SymbolSegment>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Thread:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Thread>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Thread>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::UnixThread:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::UnixThread>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::UnixThread>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Segment64:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Segment64>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Segment64>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary>::Print(
                        Options.OutFile, Range, LoadCmd.cast<
                            MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::IdFixedVMSharedLibrary:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>::Print(
                        Options.OutFile, Range, LoadCmd.cast<
                            MachO::LoadCommand::Kind::IdFixedVMSharedLibrary>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Ident:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Ident>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Ident>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::FixedVMFile:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::FixedVMFile>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::FixedVMFile>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::PrePage:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::PrePage>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::PrePage>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DynamicSymbolTable:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DynamicSymbolTable>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::DynamicSymbolTable>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadDylib>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::LoadDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::IdDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::IdDylib>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::IdDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadDylinker:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadDylinker>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::LoadDylinker>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::IdDylinker:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::IdDylinker>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::IdDylinker>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::PreBoundDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::PreBoundDylib>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::PreBoundDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Routines:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Routines>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Routines>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubFramework:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubFramework>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SubFramework>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubUmbrella:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubUmbrella>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SubUmbrella>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubClient:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubClient>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SubClient>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SubLibrary:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SubLibrary>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SubLibrary>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::TwoLevelHints:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::TwoLevelHints>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::TwoLevelHints>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::PrebindChecksum:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::PrebindChecksum>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::PrebindChecksum>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LoadWeakDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadWeakDylib>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::LoadWeakDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Routines64:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Routines64>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Routines64>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Uuid:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Uuid>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Uuid>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Rpath:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Rpath>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Rpath>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::CodeSignature:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::CodeSignature>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::CodeSignature>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SegmentSplitInfo:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SegmentSplitInfo>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::SegmentSplitInfo>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::ReexportDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::ReexportDylib>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::ReexportDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LazyLoadDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LazyLoadDylib>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::LazyLoadDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::EncryptionInfo:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::EncryptionInfo>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::EncryptionInfo>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldInfo:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldInfo>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::DyldInfo>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldInfoOnly:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldInfoOnly>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::DyldInfoOnly>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);

                break;
            case MachO::LoadCommand::Kind::LoadUpwardDylib:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LoadUpwardDylib>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::LoadUpwardDylib>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumMacOSX:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumMacOSX>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::VersionMinimumMacOSX>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumIPhoneOS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumIPhoneOS>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::VersionMinimumIPhoneOS>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::FunctionStarts:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::FunctionStarts>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::FunctionStarts>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldEnvironment:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldEnvironment>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::DyldEnvironment>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Main:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Main>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Main>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DataInCode:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DataInCode>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::DataInCode>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::SourceVersion:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::SourceVersion>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::SourceVersion>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DylibCodeSignDRS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DylibCodeSignDRS>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::DylibCodeSignDRS>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::EncryptionInfo64:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::EncryptionInfo64>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::EncryptionInfo64>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LinkerOption:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LinkerOption>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::LinkerOption>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::LinkerOptimizationHint:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::LinkerOptimizationHint>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::LinkerOptimizationHint>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumTvOS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumTvOS>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::VersionMinimumTvOS>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::VersionMinimumWatchOS:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::VersionMinimumWatchOS>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::VersionMinimumWatchOS>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::Note:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::Note>::Print(Options.OutFile,
                        Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::Note>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::BuildVersion:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::BuildVersion>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::BuildVersion>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldExportsTrie:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldExportsTrie>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<MachO::LoadCommand::Kind::DyldExportsTrie>(
                            IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
            case MachO::LoadCommand::Kind::DyldChainedFixups:
                MachOLoadCommandPrinter<
                    MachO::LoadCommand::Kind::DyldChainedFixups>::Print(
                        Options.OutFile, Range,
                        LoadCmd.cast<
                            MachO::LoadCommand::Kind::DyldChainedFixups>(
                                IsBigEndian),
                        IsBigEndian, Is64Bit, Options.Verbose);
                break;
        }

        LoadCmdCounter++;
    }
}

int
PrintLoadCommandsOperation::Run(const ConstMachOMemoryObject &Object,
                                const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    IterateLoadCommands(Object, Object.getRange(), LoadCmdStorage, Options);
    return 0;
}

int
PrintLoadCommandsOperation::Run(const ConstDscImageMemoryObject &Object,
                                const struct Options &Options) noexcept
{
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    IterateLoadCommands(Object, Object.getDscRange(), LoadCmdStorage, Options);
    return 0;
}

struct PrintLoadCommandsOperation::Options
PrintLoadCommandsOperation::ParseOptionsImpl(const ArgvArray &Argv,
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
PrintLoadCommandsOperation::ParseOptions(const ArgvArray &Argv,
                                         int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int PrintLoadCommandsOperation::Run(const MemoryObject &Object) const noexcept {
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
