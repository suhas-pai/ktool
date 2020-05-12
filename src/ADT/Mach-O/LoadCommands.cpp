//
//  src/ADT/MachO/LoadCommands.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <string_view>

#include "ADT/MachO.h"
#include "LoadCommandTemplates.h"

#include "Utils/SwitchEndian.h"
#include "Utils/Uleb128.h"

namespace MachO {
    LoadCommand::CmdSizeInvalidType
    ValidateCmdsize(const LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
        switch (LoadCmd->GetKind(IsBigEndian)) {
            case LoadCommand::Kind::Segment: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Segment>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SymbolTable: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SymbolTable>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SymbolSegment: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SymbolSegment>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Thread: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Thread>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::UnixThread: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::UnixThread>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadFixedVMSharedLibrary:
            case LoadCommand::Kind::IdFixedVMSharedLibrary: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadFixedVMSharedLibrary>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Ident: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Ident>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::FixedVMFile: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::FixedVMFile>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::PrePage:
                return LoadCommand::CmdSizeInvalidType::None;

            case LoadCommand::Kind::DynamicSymbolTable: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DynamicSymbolTable>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::IdDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::IdDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadDylinker: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadDylinker>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::IdDylinker: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::IdDylinker>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::PreBoundDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::PreBoundDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Routines: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::Routines>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubFramework: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubFramework>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubUmbrella: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubUmbrella>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubClient: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubClient>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubLibrary: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubLibrary>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::TwoLevelHints: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::TwoLevelHints>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::PrebindChecksum: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::PrebindChecksum>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadWeakDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadWeakDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Segment64: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::Segment64>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Routines64: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::Routines64>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Uuid: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Uuid>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Rpath: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Rpath>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::CodeSignature: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::CodeSignature>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SegmentSplitInfo: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SegmentSplitInfo>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::ReexportDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::ReexportDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LazyLoadDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LazyLoadDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::EncryptionInfo: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::EncryptionInfo>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldInfo: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldInfo>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldInfoOnly: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldInfoOnly>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadUpwardDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadUpwardDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumMacOSX: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumMacOSX>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumIPhoneOS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumIPhoneOS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::FunctionStarts: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::FunctionStarts>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldEnvironment: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldEnvironment>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Main: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Main>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DataInCode: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DataInCode>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SourceVersion: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SourceVersion>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DylibCodeSignDRS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DylibCodeSignDRS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::EncryptionInfo64: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::EncryptionInfo64>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LinkerOption: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LinkerOption>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LinkerOptimizationHint: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LinkerOptimizationHint>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumTvOS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumTvOS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumWatchOS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumWatchOS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Note: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Note>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::BuildVersion: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::BuildVersion>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldExportsTrie: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldExportsTrie>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldChainedFixups: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldChainedFixups>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->HasValidCmdSize(IsBigEndian);
            }
        }

        return LoadCommand::CmdSizeInvalidType::None;
    }

    constexpr static const auto EmptyStringView = std::string_view();

    const std::string_view &LoadCommand::KindGetName(Kind Kind) noexcept {
        switch (Kind) {
            case Kind::Segment:
                return LoadCommandKindInfo<Kind::Segment>::Name;
            case Kind::SymbolTable:
                return LoadCommandKindInfo<Kind::SymbolTable>::Name;
            case Kind::SymbolSegment:
                return LoadCommandKindInfo<Kind::SymbolSegment>::Name;
            case Kind::Thread:
                return LoadCommandKindInfo<Kind::Thread>::Name;
            case Kind::UnixThread:
                return LoadCommandKindInfo<Kind::UnixThread>::Name;
            case Kind::LoadFixedVMSharedLibrary:
                return LoadCommandKindInfo<
                    Kind::LoadFixedVMSharedLibrary>::Name;
            case Kind::IdFixedVMSharedLibrary:
                return LoadCommandKindInfo<Kind::IdFixedVMSharedLibrary>::Name;
            case Kind::Ident:
                return LoadCommandKindInfo<Kind::Ident>::Name;
            case Kind::FixedVMFile:
                return LoadCommandKindInfo<Kind::FixedVMFile>::Name;
            case Kind::PrePage:
                return LoadCommandKindInfo<Kind::PrePage>::Name;
            case Kind::DynamicSymbolTable:
                return LoadCommandKindInfo<Kind::DynamicSymbolTable>::Name;
            case Kind::LoadDylib:
                return LoadCommandKindInfo<Kind::LoadDylib>::Name;
            case Kind::IdDylib:
                return LoadCommandKindInfo<Kind::IdDylib>::Name;
            case Kind::LoadDylinker:
                return LoadCommandKindInfo<Kind::LoadDylinker>::Name;
            case Kind::IdDylinker:
                return LoadCommandKindInfo<Kind::IdDylinker>::Name;
            case Kind::PreBoundDylib:
                return LoadCommandKindInfo<Kind::PreBoundDylib>::Name;
            case Kind::Routines:
                return LoadCommandKindInfo<Kind::Routines>::Name;
            case Kind::SubFramework:
                return LoadCommandKindInfo<Kind::SubFramework>::Name;
            case Kind::SubUmbrella:
                return LoadCommandKindInfo<Kind::SubUmbrella>::Name;
            case Kind::SubClient:
                return LoadCommandKindInfo<Kind::SubClient>::Name;
            case Kind::SubLibrary:
                return LoadCommandKindInfo<Kind::SubLibrary>::Name;
            case Kind::TwoLevelHints:
                return LoadCommandKindInfo<Kind::TwoLevelHints>::Name;
            case Kind::PrebindChecksum:
                return LoadCommandKindInfo<Kind::PrebindChecksum>::Name;
            case Kind::LoadWeakDylib:
                return LoadCommandKindInfo<Kind::LoadWeakDylib>::Name;
            case Kind::Segment64:
                return LoadCommandKindInfo<Kind::Segment64>::Name;
            case Kind::Routines64:
                return LoadCommandKindInfo<Kind::Routines64>::Name;
            case Kind::Uuid:
                return LoadCommandKindInfo<Kind::Uuid>::Name;
            case Kind::Rpath:
                return LoadCommandKindInfo<Kind::Rpath>::Name;
            case Kind::CodeSignature:
                return LoadCommandKindInfo<Kind::CodeSignature>::Name;
            case Kind::SegmentSplitInfo:
                return LoadCommandKindInfo<Kind::SegmentSplitInfo>::Name;
            case Kind::ReexportDylib:
                return LoadCommandKindInfo<Kind::ReexportDylib>::Name;
            case Kind::LazyLoadDylib:
                return LoadCommandKindInfo<Kind::LazyLoadDylib>::Name;
            case Kind::EncryptionInfo:
                return LoadCommandKindInfo<Kind::EncryptionInfo>::Name;
            case Kind::DyldInfo:
                return LoadCommandKindInfo<Kind::DyldInfo>::Name;
            case Kind::DyldInfoOnly:
                return LoadCommandKindInfo<Kind::DyldInfoOnly>::Name;
            case Kind::LoadUpwardDylib:
                return LoadCommandKindInfo<Kind::LoadUpwardDylib>::Name;
            case Kind::VersionMinimumMacOSX:
                return LoadCommandKindInfo<Kind::VersionMinimumMacOSX>::Name;
            case Kind::VersionMinimumIPhoneOS:
                return LoadCommandKindInfo<Kind::VersionMinimumIPhoneOS>::Name;
            case Kind::FunctionStarts:
                return LoadCommandKindInfo<Kind::FunctionStarts>::Name;
            case Kind::DyldEnvironment:
                return LoadCommandKindInfo<Kind::DyldEnvironment>::Name;
            case Kind::Main:
                return LoadCommandKindInfo<Kind::Main>::Name;
            case Kind::DataInCode:
                return LoadCommandKindInfo<Kind::DataInCode>::Name;
            case Kind::SourceVersion:
                return LoadCommandKindInfo<Kind::SourceVersion>::Name;
            case Kind::DylibCodeSignDRS:
                return LoadCommandKindInfo<Kind::DylibCodeSignDRS>::Name;
            case Kind::EncryptionInfo64:
                return LoadCommandKindInfo<Kind::EncryptionInfo64>::Name;
            case Kind::LinkerOption:
                return LoadCommandKindInfo<Kind::LinkerOption>::Name;
            case Kind::LinkerOptimizationHint:
                return LoadCommandKindInfo<
                    Kind::LinkerOptimizationHint>::Name;
            case Kind::VersionMinimumTvOS:
                return LoadCommandKindInfo<Kind::VersionMinimumTvOS>::Name;
            case Kind::VersionMinimumWatchOS:
                return LoadCommandKindInfo<Kind::VersionMinimumWatchOS>::Name;
            case Kind::Note:
                return LoadCommandKindInfo<Kind::Note>::Name;
            case Kind::BuildVersion:
                return LoadCommandKindInfo<Kind::BuildVersion>::Name;
            case Kind::DyldExportsTrie:
                return LoadCommandKindInfo<Kind::DyldExportsTrie>::Name;
            case Kind::DyldChainedFixups:
                return LoadCommandKindInfo<Kind::DyldChainedFixups>::Name;
        }

        return EmptyStringView;
    }

    const std::string_view &
    LoadCommand::KindGetDescription(Kind Kind) noexcept {
        switch (Kind) {
            case Kind::Segment:
                return LoadCommandKindInfo<Kind::Segment>::Description;
            case Kind::SymbolTable:
                return LoadCommandKindInfo<Kind::SymbolTable>::Description;
            case Kind::SymbolSegment:
                return LoadCommandKindInfo<Kind::SymbolSegment>::Description;
            case Kind::Thread:
                return LoadCommandKindInfo<Kind::Thread>::Description;
            case Kind::UnixThread:
                return LoadCommandKindInfo<Kind::UnixThread>::Description;
            case Kind::LoadFixedVMSharedLibrary:
                return LoadCommandKindInfo<
                    Kind::LoadFixedVMSharedLibrary>::Description;
            case Kind::IdFixedVMSharedLibrary:
                return LoadCommandKindInfo<
                    Kind::IdFixedVMSharedLibrary>::Description;
            case Kind::Ident:
                return LoadCommandKindInfo<Kind::Ident>::Description;
            case Kind::FixedVMFile:
                return LoadCommandKindInfo<Kind::FixedVMFile>::Description;
            case Kind::PrePage:
                return LoadCommandKindInfo<Kind::PrePage>::Description;
            case Kind::DynamicSymbolTable:
                return LoadCommandKindInfo<
                    Kind::DynamicSymbolTable>::Description;
            case Kind::LoadDylib:
                return LoadCommandKindInfo<Kind::LoadDylib>::Description;
            case Kind::IdDylib:
                return LoadCommandKindInfo<Kind::IdDylib>::Description;
            case Kind::LoadDylinker:
                return LoadCommandKindInfo<Kind::LoadDylinker>::Description;
            case Kind::IdDylinker:
                return LoadCommandKindInfo<Kind::IdDylinker>::Description;
            case Kind::PreBoundDylib:
                return LoadCommandKindInfo<Kind::PreBoundDylib>::Description;
            case Kind::Routines:
                return LoadCommandKindInfo<Kind::Routines>::Description;
            case Kind::SubFramework:
                return LoadCommandKindInfo<Kind::SubFramework>::Description;
            case Kind::SubUmbrella:
                return LoadCommandKindInfo<Kind::SubUmbrella>::Description;
            case Kind::SubClient:
                return LoadCommandKindInfo<Kind::SubClient>::Description;
            case Kind::SubLibrary:
                return LoadCommandKindInfo<Kind::SubLibrary>::Description;
            case Kind::TwoLevelHints:
                return LoadCommandKindInfo<Kind::TwoLevelHints>::Description;
            case Kind::PrebindChecksum:
                return LoadCommandKindInfo<Kind::PrebindChecksum>::Description;
            case Kind::LoadWeakDylib:
                return LoadCommandKindInfo<Kind::LoadWeakDylib>::Description;
            case Kind::Segment64:
                return LoadCommandKindInfo<Kind::Segment64>::Description;
            case Kind::Routines64:
                return LoadCommandKindInfo<Kind::Routines64>::Description;
            case Kind::Uuid:
                return LoadCommandKindInfo<Kind::Uuid>::Description;
            case Kind::Rpath:
                return LoadCommandKindInfo<Kind::Rpath>::Description;
            case Kind::CodeSignature:
                return LoadCommandKindInfo<Kind::CodeSignature>::Description;
            case Kind::SegmentSplitInfo:
                return LoadCommandKindInfo<Kind::SegmentSplitInfo>::Description;
            case Kind::ReexportDylib:
                return LoadCommandKindInfo<Kind::ReexportDylib>::Description;
            case Kind::LazyLoadDylib:
                return LoadCommandKindInfo<Kind::LazyLoadDylib>::Description;
            case Kind::EncryptionInfo:
                return LoadCommandKindInfo<Kind::EncryptionInfo>::Description;
            case Kind::DyldInfo:
                return LoadCommandKindInfo<Kind::DyldInfo>::Description;
            case Kind::DyldInfoOnly:
                return LoadCommandKindInfo<Kind::DyldInfoOnly>::Description;
            case Kind::LoadUpwardDylib:
                return LoadCommandKindInfo<Kind::LoadUpwardDylib>::Description;
            case Kind::VersionMinimumMacOSX:
                return LoadCommandKindInfo<
                    Kind::VersionMinimumMacOSX>::Description;
            case Kind::VersionMinimumIPhoneOS:
                return LoadCommandKindInfo<
                    Kind::VersionMinimumIPhoneOS>::Description;
            case Kind::FunctionStarts:
                return LoadCommandKindInfo<Kind::FunctionStarts>::Description;
            case Kind::DyldEnvironment:
                return LoadCommandKindInfo<Kind::DyldEnvironment>::Description;
            case Kind::Main:
                return LoadCommandKindInfo<Kind::Main>::Description;
            case Kind::DataInCode:
                return LoadCommandKindInfo<Kind::DataInCode>::Description;
            case Kind::SourceVersion:
                return LoadCommandKindInfo<Kind::SourceVersion>::Description;
            case Kind::DylibCodeSignDRS:
                return LoadCommandKindInfo<Kind::DylibCodeSignDRS>::Description;
            case Kind::EncryptionInfo64:
                return LoadCommandKindInfo<Kind::EncryptionInfo64>::Description;
            case Kind::LinkerOption:
                return LoadCommandKindInfo<Kind::LinkerOption>::Description;
            case Kind::LinkerOptimizationHint:
                return LoadCommandKindInfo<
                    Kind::LinkerOptimizationHint>::Description;
            case Kind::VersionMinimumTvOS:
                return LoadCommandKindInfo<
                    Kind::VersionMinimumTvOS>::Description;
            case Kind::VersionMinimumWatchOS:
                return LoadCommandKindInfo<
                    Kind::VersionMinimumWatchOS>::Description;
            case Kind::Note:
                return LoadCommandKindInfo<Kind::Note>::Description;
            case Kind::BuildVersion:
                return LoadCommandKindInfo<Kind::BuildVersion>::Description;
            case Kind::DyldExportsTrie:
                return LoadCommandKindInfo<Kind::DyldExportsTrie>::Description;
            case Kind::DyldChainedFixups:
                return LoadCommandKindInfo<
                    Kind::DyldChainedFixups>::Description;
        }

        return EmptyStringView;
    }

    SegmentSectionAttributes::SegmentSectionAttributes(uint32_t Flags) noexcept
    : Flags(Flags) {}

    SegmentSectionFlags::Type
    SegmentSectionFlags::GetType(bool IsBigEndian) const noexcept {
        return Type(GetValueForMask(SegmentSectionMasks::Type, IsBigEndian));
    }

    SegmentSectionFlags::Attributes
    SegmentSectionFlags::GetAttributes(bool IsBigEndian) const noexcept {
        const auto Value =
            GetValueForMask(SegmentSectionMasks::Attributes, IsBigEndian);

        return Attributes(Value);
    }

    SegmentCommand::SectionList
    SegmentCommand::GetSectionList(bool IsBigEndian) noexcept {
        const auto Ptr = reinterpret_cast<Section *>(this + 1);
        const auto Nsects = SwitchEndianIf(this->Nsects, IsBigEndian);

        return SegmentCommand::SectionList(Ptr, Nsects);
    }

    SegmentCommand::ConstSectionList
    SegmentCommand::GetConstSectionList(bool IsBigEndian) const noexcept {
        const auto Ptr = reinterpret_cast<const Section *>(this + 1);
        const auto Nsects = SwitchEndianIf(this->Nsects, IsBigEndian);

        return SegmentCommand::ConstSectionList(Ptr, Nsects);
    }

    SegmentCommand64::SectionList
    SegmentCommand64::GetSectionList(bool IsBigEndian) noexcept {
        const auto Ptr = reinterpret_cast<Section *>(this + 1);
        const auto Nsects = SwitchEndianIf(this->Nsects, IsBigEndian);

        return SegmentCommand64::SectionList(Ptr, Nsects);
    }

    SegmentCommand64::ConstSectionList
    SegmentCommand64::GetConstSectionList(bool IsBigEndian) const noexcept {
        const auto Ptr = reinterpret_cast<const Section *>(this + 1);
        const auto Nsects = SwitchEndianIf(this->Nsects, IsBigEndian);

        return SegmentCommand64::ConstSectionList(Ptr, Nsects);
    }

    static inline bool
    IsLoadCommandStringOffsetValid(uint32_t MinSize,
                                   uint32_t CmdSize,
                                   uint32_t Offset)
    {
        return (Offset >= MinSize && Offset < CmdSize);
    }

    static inline uint32_t
    LoadCommandStringGetLength(const void *Begin,
                               uint32_t Offset,
                               uint32_t CmdSize)
    {
        const auto Ptr = reinterpret_cast<const char *>(Begin) + Offset;
        const auto MaxLength = (CmdSize - 1 - Offset);
        const auto Length = static_cast<uint32_t>(strnlen(Ptr, MaxLength));

        return Length;
    }

    bool
    LoadCommandString::IsOffsetValid(uint32_t MinSize,
                                     uint32_t CmdSize,
                                     bool IsBigEndian) const noexcept
    {
        const auto Offset = SwitchEndianIf(this->Offset, IsBigEndian);
        return IsLoadCommandStringOffsetValid(MinSize, CmdSize, Offset);
    }

    uint32_t
    LoadCommandString::GetLength(uint32_t CmdSize,
                                 bool IsBigEndian) const noexcept
    {
        const auto Offset = SwitchEndianIf(this->Offset, IsBigEndian);
        return LoadCommandStringGetLength(this, Offset, CmdSize);
    }

    bool DylibCommand::IsNameOffsetValid(bool IsBigEndian) const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Info.Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    static LoadCommandString::GetValueResult
    LoadCmdGetStoredString(const LoadCommand *LC,
                           uint32_t MinSize,
                           uint32_t CmdSize,
                           uint32_t Offset)
    {
        if (!IsLoadCommandStringOffsetValid(MinSize, CmdSize, Offset)) {
            return LoadCommandString::GetStringError::OffsetOutOfBounds;
        }

        const auto Length = LoadCommandStringGetLength(LC, Offset, CmdSize);
        const auto Data = reinterpret_cast<const char *>(LC) + Offset;

        if (Length == 0) {
            return LoadCommandString::GetStringError::EmptyString;
        }

        if ((Offset + Length) == CmdSize || Data[Length] != '\0') {
            return LoadCommandString::GetStringError::NoNullTerminator;
        }

        const auto Result = std::string_view(Data, Length);
        return Result;
    }

    bool
    FixedVMSharedLibraryCommand::IsNameOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto Offset = SwitchEndianIf(Info.Name.Offset, IsBigEndian);
        return Info.Name.IsOffsetValid(sizeof(*this), Offset, CmdSize);
    }

    LoadCommandString::GetValueResult
    FixedVMSharedLibraryCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Info.Name.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this,
                                   MinSize,
                                   CmdSize,
                                   NameOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    DylibCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Info.Name.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    bool SubFrameworkCommand::IsUmbrellaOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Umbrella.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    SubFrameworkCommand::GetUmbrella(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Umbrella.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    bool SubClientCommand::IsClientOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Client.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    SubClientCommand::GetClient(bool IsBigEndian)
    const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Client.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    bool SubUmbrellaCommand::IsUmbrellaOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Umbrella.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    SubUmbrellaCommand::GetUmbrella(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Umbrella.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    bool SubLibraryCommand::IsLibraryOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Library.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    SubLibraryCommand::GetLibrary(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Library.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    struct NakedPreBoundDylibCommand {
        LoadCommand LoadCmd;
        LoadCommandString Name;
        uint32_t NModules;
        LoadCommandString LinkedModules;
    };

    static_assert(
        sizeof(NakedPreBoundDylibCommand) == sizeof(PreBoundDylibCommand),
        "NakedPreBoundDylibCommand is not same as PreBoundDylibCommand");

    bool PreBoundDylibCommand::IsNameOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = offsetof(NakedPreBoundDylibCommand, NModules);
        return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    PreBoundDylibCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = offsetof(NakedPreBoundDylibCommand, NModules);
        const auto NameOffset = SwitchEndianIf(Name.Offset, IsBigEndian);

        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    bool PreBoundDylibCommand::IsLinkedModulesOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    PreBoundDylibCommand::GetLinkedModules(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Name.Offset, IsBigEndian);

        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    bool DylinkerCommand::IsNameOffsetValid(bool IsBigEndian)
    const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    DylinkerCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Name.Offset, IsBigEndian);

        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    SymbolTableEntryInfo::SymbolType
    SymbolTableEntryInfo::GetSymbolType() const noexcept {
        return SymbolType(GetValueForMask(Masks::SymbolTypeMask));
    }

    TypedAllocationOrError<SymtabCommand::Entry32List *, SizeRangeError>
    SymtabCommand::GetEntry32List(const MemoryMap &Map,
                                  bool IsBigEndian) noexcept
    {
        const auto Nsyms = SwitchEndianIf(this->Nsyms, IsBigEndian);
        const auto SymOff = SwitchEndianIf(this->SymOff, IsBigEndian);

        auto SymEnd = uint64_t();
        if (DoesMultiplyAndAddOverflow(sizeof(SymtabCommand::Entry32),
                                       Nsyms, SymOff, &SymEnd))
        {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry32 *>(Map.GetBegin() + SymOff);

        return new Entry32List(Entries, Nsyms);
    }

    TypedAllocationOrError<SymtabCommand::Entry64List *, SizeRangeError>
    SymtabCommand::GetEntry64List(const MemoryMap &Map, bool IsBigEndian)
    noexcept {
        const auto Nsyms = SwitchEndianIf(this->Nsyms, IsBigEndian);
        const auto SymOff = SwitchEndianIf(this->SymOff, IsBigEndian);

        auto SymEnd = uint64_t();
        if (DoesMultiplyAndAddOverflow(sizeof(SymtabCommand::Entry64),
                                       Nsyms, SymOff, &SymEnd))
        {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry64 *>(Map.GetBegin() + SymOff);

        return new Entry64List(Entries, Nsyms);
    }

    TypedAllocationOrError<SymtabCommand::ConstEntry32List *, SizeRangeError>
    SymtabCommand::GetConstEntry32List(const ConstMemoryMap &Map,
                                       bool IsBigEndian) const noexcept
    {
        const auto Nsyms = SwitchEndianIf(this->Nsyms, IsBigEndian);
        const auto SymOff = SwitchEndianIf(this->SymOff, IsBigEndian);

        auto SymEnd = uint64_t();
        if (DoesMultiplyAndAddOverflow(sizeof(SymtabCommand::Entry32), Nsyms,
                                       SymOff, &SymEnd))
        {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry32 *>(
                Map.GetBegin() + SymOff);

        return new ConstEntry32List(Entries, Nsyms);
    }

    TypedAllocationOrError<SymtabCommand::ConstEntry64List *, SizeRangeError>
    SymtabCommand::GetConstEntry64List(const ConstMemoryMap &Map,
                                       bool IsBigEndian) const noexcept
    {
        const auto Nsyms = SwitchEndianIf(this->Nsyms, IsBigEndian);
        const auto SymOff = SwitchEndianIf(this->SymOff, IsBigEndian);

        auto SymEnd = uint64_t();
        if (DoesMultiplyAndAddOverflow(sizeof(SymtabCommand::Entry64), Nsyms,
                                       SymOff, &SymEnd))
        {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry64 *>(
                Map.GetBegin() + SymOff);

        return new ConstEntry64List(Entries, Nsyms);
    }

    TypedAllocationOrError<DynamicSymtabCommand::Entry32List *, SizeRangeError>
    DynamicSymtabCommand::GetExport32List(const MemoryMap &Map,
                                          uint32_t SymOff,
                                          bool IsBigEndian) noexcept
    {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry32 *>(Map.GetBegin() + SymOff);

        return new Entry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::Entry64List *, SizeRangeError>
    DynamicSymtabCommand::GetExport64List(const MemoryMap &Map,
                                          uint32_t SymOff,
                                          bool IsBigEndian) noexcept
    {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry64 *>(Map.GetBegin() + SymOff);

        return new Entry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::ConstEntry32List *, SizeRangeError>
    DynamicSymtabCommand::GetConstExport32List(const ConstMemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) const noexcept
    {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry32 *>(
                Map.GetBegin() + SymOff);

        return new ConstEntry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::ConstEntry64List *, SizeRangeError>
    DynamicSymtabCommand::GetConstExport64List(const ConstMemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) const noexcept
    {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry64 *>(
                Map.GetBegin() + SymOff);

        return new ConstEntry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::Entry32List *, SizeRangeError>
    DynamicSymtabCommand::GetLocalSymbol32List(const MemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) noexcept
    {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry32 *>(Map.GetBegin() + SymOff);

        return new Entry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::Entry64List *, SizeRangeError>
    DynamicSymtabCommand::GetLocalSymbol64List(const MemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) noexcept
    {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry64 *>(Map.GetBegin() + SymOff);

        return new Entry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::ConstEntry32List *, SizeRangeError>
    DynamicSymtabCommand::GetConstLocalSymbol32List(const ConstMemoryMap &Map,
                                                    uint32_t SymOff,
                                                    bool IsBigEndian)
    const noexcept {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry32 *>(
                Map.GetBegin() + SymOff);

        return new ConstEntry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymtabCommand::ConstEntry64List *, SizeRangeError>
    DynamicSymtabCommand::GetConstLocalSymbol64List(const ConstMemoryMap &Map,
                                                    uint32_t SymOff,
                                                    bool IsBigEndian)
    const noexcept {
        const auto Index =
            SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        const auto Count =
            SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);

        auto ByteIndex = uint64_t();
        if (DoesAddOverflow(sizeof(DynamicSymtabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry64 *>(
                Map.GetBegin() + SymOff);

        return new ConstEntry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<TwoLevelHintsCommand::HintList *, SizeRangeError>
    TwoLevelHintsCommand::GetHintList(const MemoryMap &Map, bool IsBigEndian)
    noexcept {
        const auto Offset = SwitchEndianIf(this->Offset, IsBigEndian);
        const auto Count = SwitchEndianIf(NHints, IsBigEndian);

        auto End = uint64_t();
        if (DoesMultiplyAndAddOverflow(sizeof(Hint), Count, Offset, &End)) {
            return SizeRangeError::PastEnd;
        }

        const auto LocRange = LocationRange::CreateWithEnd(Offset, End);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries = reinterpret_cast<Hint *>(Map.GetBegin() + Offset);
        return new HintList(Entries, Count);
    }

    TypedAllocationOrError<TwoLevelHintsCommand::ConstHintList *, SizeRangeError>
    TwoLevelHintsCommand::GetConstHintList(const ConstMemoryMap &Map,
                                           bool IsBigEndian) const noexcept
    {
        const auto Offset = SwitchEndianIf(this->Offset, IsBigEndian);
        const auto Count = SwitchEndianIf(NHints, IsBigEndian);

        auto End = uint64_t();
        if (DoesMultiplyAndAddOverflow(sizeof(Hint), Count, Offset, &End)) {
            return SizeRangeError::PastEnd;
        }

        const auto LocRange = LocationRange::CreateWithEnd(Offset, End);
        if (!Map.GetRange().Contains(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const Hint *>(Map.GetBegin() + Offset);

        return new ConstHintList(Entries, Count);
    }

    bool RpathCommand::IsPathOffsetValid(bool IsBigEndian) const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Path.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    RpathCommand::GetPath(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Path.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    enum BuildVersionCommand::Tool::Kind
    BuildVersionCommand::Tool::GetKind(bool IsBigEndian) const noexcept {
        return static_cast<enum Kind>(SwitchEndianIf(this->Kind, IsBigEndian));
    }

    BuildVersionCommand::PlatformKind
    BuildVersionCommand::GetPlatform(bool IsBigEndian) const noexcept {
        return PlatformKind(SwitchEndianIf(Platform, IsBigEndian));
    }

    TypedAllocationOrError<BuildVersionCommand::ToolList *, SizeRangeError>
    BuildVersionCommand::GetToolList(bool IsBigEndian) noexcept {
        auto End = uint64_t();

        const auto Count = SwitchEndianIf(NTools, IsBigEndian);
        const auto Entries = reinterpret_cast<Tool *>(this + 1);

        if (DoesMultiplyAndAddOverflow(sizeof(Tool), Count, sizeof(*this),
                                       &End))
        {
            return SizeRangeError::Overflows;
        }

        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        if (End > CmdSize) {
            return SizeRangeError::PastEnd;
        }

        return new ToolList(Entries, Count);
    }

    TypedAllocationOrError<BuildVersionCommand::ConstToolList *, SizeRangeError>
    BuildVersionCommand::GetConstToolList(bool IsBigEndian) const noexcept {
        auto End = uint64_t();

        const auto Count = SwitchEndianIf(NTools, IsBigEndian);
        const auto Entries = reinterpret_cast<const Tool *>(this + 1);

        if (DoesMultiplyAndAddOverflow(sizeof(Tool), Count, sizeof(*this),
                                       &End))
        {
            return SizeRangeError::Overflows;
        }

        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        if (End > CmdSize) {
            return SizeRangeError::PastEnd;
        }

        return new ConstToolList(Entries, Count);
    }

    BindByte::BindByte(uint8_t Byte) noexcept
    : DyldInfoByteMasksHandler(Byte) {}
    
    BindByte::Opcode BindByte::GetOpcode() const noexcept {
        return BindByte::Opcode(GetValueForMask(Masks::Opcode));
    }

    uint8_t BindByte::GetImmediate() const noexcept {
        return GetValueForMask(Masks::Immediate);
    }

    RebaseByte::RebaseByte(uint8_t Byte) noexcept
    : DyldInfoByteMasksHandler(Byte) {}
    
    RebaseByte::Opcode RebaseByte::GetOpcode() const noexcept {
        return RebaseByte::Opcode(GetValueForMask(Masks::Opcode));
    }

    uint8_t RebaseByte::GetImmediate() const noexcept {
        return GetValueForMask(Masks::Immediate);
    }

    DyldInfoCommand::BindOpcodeList
    DyldInfoCommand::GetBindOpcodeList(const MemoryMap &Map,
                                       bool IsBigEndian) const noexcept
    {
        const auto BindSize = SwitchEndianIf(this->BindSize, IsBigEndian);
        const auto Begin =
            Map.GetBegin() + SwitchEndianIf(BindOff, IsBigEndian);

        return BindOpcodeList(Begin, BindSize);
    }

    DyldInfoCommand::ConstBindOpcodeList
    DyldInfoCommand::GetConstBindOpcodeList(const ConstMemoryMap &Map,
                                            bool IsBigEndian) const noexcept
    {
        const auto BindSize = SwitchEndianIf(this->BindSize, IsBigEndian);
        const auto Begin =
            Map.GetBegin() + SwitchEndianIf(BindOff, IsBigEndian);

        return ConstBindOpcodeList(Begin, BindSize);
    }

    DyldInfoCommand::RebaseOpcodeList
    DyldInfoCommand::GetRebaseOpcodeList(const MemoryMap &Map,
                                         bool IsBigEndian) const noexcept
    {
        const auto RebaseSize = SwitchEndianIf(this->RebaseSize, IsBigEndian);
        const auto Begin =
            Map.GetBegin() + SwitchEndianIf(RebaseOff, IsBigEndian);

        return RebaseOpcodeList(Begin, RebaseSize);
    }

    DyldInfoCommand::ConstRebaseOpcodeList
    DyldInfoCommand::GetConstRebaseOpcodeList(const ConstMemoryMap &Map,
                                              bool IsBigEndian) const noexcept
    {
        const auto RebaseSize = SwitchEndianIf(this->RebaseSize, IsBigEndian);
        const auto Begin =
            Map.GetBegin() + SwitchEndianIf(RebaseOff, IsBigEndian);

        return ConstRebaseOpcodeList(Begin, RebaseSize);
    }

    ExportTrieIterator::ExportTrieIterator(const uint8_t *Begin,
                                           const uint8_t *End) noexcept
    : Begin(Begin), End(End)
    {
        auto Node = NodeInfo();
        auto ParseError = ParseNode(Begin, &Node);

        RangeList.reserve(128);
        StackList.reserve(16);
        Symbol.String.reserve(64);

        if (ParseError != Error::None) {
            this->ParseError = ParseError;
            return;
        }

        const auto Stack = StackInfo(Node);
        StackList.emplace_back(Stack);

        ParseError = Advance();
        if (ParseError != Error::None) {
            this->ParseError = ParseError;
        }
    }

    void ExportTrieIterator::SetupInfoForTopNode() noexcept {
        // Before advancing to the parent-node, restore the string and
        // range-list to the stack's spec.

        const auto &Top = StackList.back();
        Symbol.String.erase(Top.StringSize);

        if (Top.RangeListSize != 0) {
            RangeList.erase(RangeList.cbegin() + Top.RangeListSize,
                            RangeList.cend());
        }
    }

    void ExportTrieIterator::MoveUptoParentNode() noexcept {
        StackList.pop_back();
        StackList.back().ChildIndex += 1;
    }

    ExportTrieIterator::Error
    ExportTrieIterator::ParseNode(const uint8_t *Ptr, NodeInfo *InfoOut)
    noexcept {
        auto NodeSize = uint32_t();
        if ((Ptr = ReadUleb128(Ptr, End, &NodeSize)) == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        const auto Offset = (Ptr - Begin);
        auto OffsetEnd = uint32_t();

        if (DoesAddOverflow(Offset, NodeSize, &OffsetEnd)) {
            return Error::InvalidFormat;
        }

        const auto ExpectedEnd = Ptr + NodeSize;
        if (ExpectedEnd >= End) {
            return Error::InvalidFormat;
        }

        const auto Range = LocationRange::CreateWithEnd(Offset, OffsetEnd);
        const auto RangeListEnd = RangeList.cend();

        const auto Predicate = [&Range](const LocationRange &RhsRange) {
            return Range.Overlaps(RhsRange);
        };

        if (std::find_if(RangeList.cbegin(), RangeListEnd, Predicate) !=
            RangeListEnd)
        {
            return Error::OverlappingRanges;
        }

        const auto ChildCount = *ExpectedEnd;
        RangeList.emplace_back(Range);

        InfoOut->Ptr = Ptr;
        InfoOut->Size = NodeSize;
        InfoOut->ChildCount = ChildCount;

        return Error::None;
    }

    ExportTrieIterator::Error ExportTrieIterator::Advance() noexcept {
        auto StackTopIndex = StackList.size();

        // Do some initial setup.
        Symbol.ClearExportExclusiveInfo();

        do {
            if (StackList.empty()) {
                return Error::ReachedEnd;
            }

            SetupInfoForTopNode();

            auto &Stack = StackList.back();
            auto &Node = Stack.Node;
            auto &Ptr = Node.Ptr;

            const auto NodeSize = Node.Size;
            const auto ExpectedEnd = Ptr + NodeSize;
            const auto IsExportInfo = (NodeSize != 0);

            if (Stack.ChildIndex == 0) {
                if (IsExportInfo) {
                    if (Symbol.String.empty()) {
                        return Error::EmptyExport;
                    }

                    auto Flags = uint64_t();
                    if ((Ptr = ReadUleb128(Ptr, End, &Flags)) == nullptr) {
                        return Error::InvalidUleb128;
                    }

                    if (Ptr == End) {
                        return Error::InvalidFormat;
                    }

                    Symbol.Flags = ExportTrieFlagsByte(Flags);
                    if (Symbol.Flags.IsReexport()) {
                        auto DylibOrdinal = uint32_t();
                        Ptr = ReadUleb128(Ptr, End, &DylibOrdinal);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        if (Ptr == End) {
                            return Error::InvalidFormat;
                        }

                        Symbol.ReexportDylibOrdinal = DylibOrdinal;
                        if (*Ptr != '\0') {
                            const auto MaxLength = (End - Ptr);
                            const auto Length =
                                strnlen(reinterpret_cast<const char *>(Ptr),
                                        MaxLength);

                            if (Length == MaxLength) {
                                return Error::InvalidFormat;
                            }

                            const auto String =
                                reinterpret_cast<const char *>(Ptr);

                            Symbol.ReexportImportName.append(String, Length);
                            Ptr += (Length + 1);
                        } else {
                            Ptr += 1;
                        }
                    } else {
                        auto ImageOffset = uint64_t();
                        Ptr = ReadUleb128(Ptr, End, &ImageOffset);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        if (Symbol.Flags.IsStubAndResolver()) {
                            if (Ptr == End) {
                                return Error::InvalidFormat;
                            }

                            auto ResolverStubAddress = uint64_t();
                            Ptr = ReadUleb128(Ptr, End, &ResolverStubAddress);

                            if (Ptr == nullptr) {
                                return Error::InvalidUleb128;
                            }

                            Symbol.ResolverStubAddress = ResolverStubAddress;
                        }

                        Symbol.ImageOffset = ImageOffset;
                    }

                    if (Ptr != ExpectedEnd) {
                        return Error::InvalidUleb128;
                    }

                    switch (Symbol.Flags.GetKind()) {
                        case ExportSymbolKind::Regular:
                            if (Symbol.Flags.IsStubAndResolver()) {
                                Symbol.sType =
                                    ExportTrieSymbolType::StubAndResolver;
                            } else if (Symbol.Flags.IsReexport()) {
                                Symbol.sType = ExportTrieSymbol::Type::Reexport;
                            } else {
                                Symbol.sType = ExportTrieSymbol::Type::Regular;
                            }

                            break;
                        case ExportSymbolKind::ThreadLocal:
                            Symbol.sType = ExportTrieSymbol::Type::ThreadLocal;
                            break;
                        case ExportSymbolKind::Absolute:
                            Symbol.sType = ExportTrieSymbol::Type::Absolute;
                            break;
                    }

                    // Return later, after checking the child-count, so we can
                    // update the stack-list if necessary
                }

                // If this node has no children, we're done with this entire
                // stack. To finish, we simply pop the stack and move to the
                // parent-stack. If we don't have a parent-stack, we've reached
                // the end of the export-trie.

                if (Node.ChildCount == 0) {
                    if (StackList.empty()) {
                        if (IsExportInfo) {
                            break;
                        }

                        return Error::ReachedEnd;
                    }

                    MoveUptoParentNode();
                    if (IsExportInfo) {
                        break;
                    }

                    StackTopIndex -= 1;
                    continue;
                }

                // Skip the byte storing the child-count.
                Ptr++;
                if (IsExportInfo) {
                    // Since we do have children as an export-node, we have to
                    // set ChildIndex to a marker-value to avoid an infinite
                    // loop when calling Advance().
                    //
                    // Without a marker-value, Child-Index would be 0, and
                    // Advance() would enter and re-return this export-node.

                    Stack.ChildIndex = std::numeric_limits<uint16_t>::max();
                    break;
                }
            }
            else if (Stack.ChildIndex == std::numeric_limits<uint16_t>::max()) {
                Stack.ChildIndex = 0;
            } else {
                // If our index equals child-count, we've finished with this
                // node, and should move up to the parent-node.

                if (Stack.ChildIndex == Node.ChildCount) {
                    if (StackList.empty()) {
                        return Error::ReachedEnd;
                    }

                    MoveUptoParentNode();
                    StackTopIndex -= 1;
                    
                    continue;
                }
            }

            const auto MaxLength = (End - Ptr);
            const auto Prefix = reinterpret_cast<const char *>(Ptr);
            const auto Length = strnlen(Prefix, MaxLength);

            if (Length == MaxLength) {
                return Error::InvalidFormat;
            }

            Symbol.String.append(Prefix, Length);
            Ptr += (Length + 1);

            auto Next = uint64_t();
            if ((Ptr = ReadUleb128<uint32_t>(Ptr, End, &Next)) == nullptr) {
                return Error::InvalidUleb128;
            }

            if (Ptr == End) {
                if (Stack.ChildIndex != (Node.ChildCount - 1)) {
                    return Error::InvalidFormat;
                }
            }

            if (Next >= (End - Begin)) {
                return Error::InvalidFormat;
            }

            auto NewStack = StackInfo();
            if (const auto Error = ParseNode(Begin + Next, &NewStack.Node);
                Error != Error::None)
            {
                return Error;
            }

            NewStack.StringSize = Symbol.String.length();
            NewStack.RangeListSize = RangeList.size();

            StackList.emplace_back(NewStack);
            StackTopIndex += 1;
        } while (true);

        return Error::None;
    }

    TypedAllocationOrError<ExportTrieList *, SizeRangeError>
    DyldInfoCommand::GetExportTrieList(const MemoryMap &Map,
                                       bool IsBigEndian) noexcept
    {
        const auto Offset = SwitchEndianIf(this->ExportOff, IsBigEndian);
        const auto Size = SwitchEndianIf(this->ExportSize, IsBigEndian);

        auto End = static_cast<uint64_t>(Offset + Size);
        auto Range = LocationRange::CreateWithEnd(Offset, End);

        if (!Map.GetRange().Contains(Range)) {
            return SizeRangeError::PastEnd;
        }

        const auto MapBegin = Map.GetBegin();
        return new ExportTrieList(MapBegin + Offset, MapBegin + End);
    }

    TypedAllocationOrError<ConstExportTrieList *, SizeRangeError>
    DyldInfoCommand::GetConstExportTrieList(const ConstMemoryMap &Map,
                                            bool IsBigEndian) const noexcept
    {
        const auto Offset = SwitchEndianIf(this->ExportOff, IsBigEndian);
        const auto Size = SwitchEndianIf(this->ExportSize, IsBigEndian);

        auto End = static_cast<uint64_t>(Offset + Size);
        auto Range = LocationRange::CreateWithEnd(Offset, End);

        if (!Map.GetRange().Contains(Range)) {
            return SizeRangeError::PastEnd;
        }

        const auto MapBegin = Map.GetBegin();
        return new ConstExportTrieList(MapBegin + Offset, MapBegin + End);
    }

    TypedAllocationOrError<std::vector<ExportTrieSymbol> *, SizeRangeError>
    DyldInfoCommand::GetExportList(const ConstMemoryMap &Map,
                                   bool IsBigEndian) noexcept
    {
        // TODO: Get faster impl?
        const auto TrieList = GetConstExportTrieList(Map, IsBigEndian);
        if (TrieList.HasError()) {
            return TrieList.GetError();
        }

        auto ExportList = new std::vector<ExportTrieSymbol>();

        // We have to create a copy because each export's string is only
        // temporary.

        for (auto Symbol : TrieList.GetRef()) {
            ExportList->emplace_back(std::move(Symbol));
        }

        return ExportList;
    }

    bool
    FixedVMFileCommand::IsNameOffsetValid(bool IsBigEndian) const noexcept {
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    FixedVMFileCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
        const auto NameOffset = SwitchEndianIf(Name.Offset, IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);
        
        return Result;
    }
}
