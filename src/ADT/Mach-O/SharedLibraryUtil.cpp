//
//  src/ADT/Mach-O/SharedLibraryUtil.cpp
//  stool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LoadCommandStorage.h"
#include "SharedLibraryUtil.h"

namespace MachO {
    bool IsSharedLibraryLoadCommand(const LoadCommand::Kind &Kind) noexcept {
        switch (Kind) {
            case MachO::LoadCommand::Kind::LoadDylib:
            case MachO::LoadCommand::Kind::LoadWeakDylib:
            case MachO::LoadCommand::Kind::ReexportDylib:
            case MachO::LoadCommand::Kind::LazyLoadDylib:
            case MachO::LoadCommand::Kind::LoadUpwardDylib:
                return true;
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
                return false;
        }
    }
    
    SharedLibraryInfoCollection
    SharedLibraryInfoCollection::Open(
        const ConstLoadCommandStorage &LoadCmdStorage,
        Error *ErrorOut) noexcept
    {
        const auto IsBigEndian = LoadCmdStorage.IsBigEndian();

        auto Error = Error::None;
        auto LCIndex = uint32_t();
        auto Result = SharedLibraryInfoCollection();

        for (const auto &LoadCmd : LoadCmdStorage) {
            const auto LCKind = LoadCmd.getKind(IsBigEndian);
            switch (LCKind) {
                case LoadCommand::Kind::LoadDylib:
                case LoadCommand::Kind::LoadWeakDylib:
                case LoadCommand::Kind::ReexportDylib:
                case LoadCommand::Kind::LazyLoadDylib:
                case LoadCommand::Kind::LoadUpwardDylib: {
                    const auto &DylibCmd =
                        cast<DylibCommand>(LoadCmd, IsBigEndian);
                    
                    const auto GetNameResult = DylibCmd.GetName(IsBigEndian);
                    const auto &Name =
                        (GetNameResult.hasError()) ?
                            EmptyStringValue :
                            GetNameResult.getString();

                    if (GetNameResult.hasError()) {
                        Error = Error::InvalidPath;
                    }

                    const auto &Info = DylibCmd.Info;
                    const auto Timestamp =
                        SwitchEndianIf(Info.Timestamp, IsBigEndian);

                    Result.List.emplace_back(SharedLibraryInfo {
                        .Kind = LCKind,
                        .Path = std::string(Name),

                        .Index = LCIndex,
                        .Timestamp = Timestamp,

                        .CurrentVersion = Info.getCurrentVersion(IsBigEndian),
                        .CompatibilityVersion =
                            Info.getCompatVersion(IsBigEndian)
                    });

                    break;
                }

                case LoadCommand::Kind::Segment:
                case LoadCommand::Kind::SymbolTable:
                case LoadCommand::Kind::SymbolSegment:
                case LoadCommand::Kind::Thread:
                case LoadCommand::Kind::UnixThread:
                case LoadCommand::Kind::LoadFixedVMSharedLibrary:
                case LoadCommand::Kind::IdFixedVMSharedLibrary:
                case LoadCommand::Kind::Ident:
                case LoadCommand::Kind::FixedVMFile:
                case LoadCommand::Kind::PrePage:
                case LoadCommand::Kind::DynamicSymbolTable:
                case LoadCommand::Kind::IdDylib:
                case LoadCommand::Kind::LoadDylinker:
                case LoadCommand::Kind::IdDylinker:
                case LoadCommand::Kind::PreBoundDylib:
                case LoadCommand::Kind::Routines:
                case LoadCommand::Kind::SubFramework:
                case LoadCommand::Kind::SubUmbrella:
                case LoadCommand::Kind::SubClient:
                case LoadCommand::Kind::SubLibrary:
                case LoadCommand::Kind::TwoLevelHints:
                case LoadCommand::Kind::PrebindChecksum:
                case LoadCommand::Kind::Segment64:
                case LoadCommand::Kind::Routines64:
                case LoadCommand::Kind::Uuid:
                case LoadCommand::Kind::Rpath:
                case LoadCommand::Kind::CodeSignature:
                case LoadCommand::Kind::SegmentSplitInfo:
                case LoadCommand::Kind::EncryptionInfo:
                case LoadCommand::Kind::DyldInfo:
                case LoadCommand::Kind::DyldInfoOnly:
                case LoadCommand::Kind::VersionMinimumMacOSX:
                case LoadCommand::Kind::VersionMinimumIPhoneOS:
                case LoadCommand::Kind::FunctionStarts:
                case LoadCommand::Kind::DyldEnvironment:
                case LoadCommand::Kind::Main:
                case LoadCommand::Kind::DataInCode:
                case LoadCommand::Kind::SourceVersion:
                case LoadCommand::Kind::DylibCodeSignDRS:
                case LoadCommand::Kind::EncryptionInfo64:
                case LoadCommand::Kind::LinkerOption:
                case LoadCommand::Kind::LinkerOptimizationHint:
                case LoadCommand::Kind::VersionMinimumTvOS:
                case LoadCommand::Kind::VersionMinimumWatchOS:
                case LoadCommand::Kind::Note:
                case LoadCommand::Kind::BuildVersion:
                case LoadCommand::Kind::DyldExportsTrie:
                case LoadCommand::Kind::DyldChainedFixups:
                    break;
            }

            LCIndex++;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error;
        }

        return Result;
    }
}
