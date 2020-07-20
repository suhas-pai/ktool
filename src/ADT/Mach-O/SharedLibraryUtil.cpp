//
//  src/ADT/Mach-O/SharedLibraryUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LoadCommandStorage.h"
#include "SharedLibraryUtil.h"

namespace MachO {
    bool IsSharedLibraryLoadCommand(const LoadCommand::Kind &Kind) noexcept {
        switch (Kind) {
            case LoadCommand::Kind::LoadDylib:
            case LoadCommand::Kind::LoadWeakDylib:
            case LoadCommand::Kind::ReexportDylib:
            case LoadCommand::Kind::LazyLoadDylib:
            case LoadCommand::Kind::LoadUpwardDylib:
                return true;
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
