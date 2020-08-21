//
//  src/ADT/Mach-O/LoadCommands.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <algorithm>
#include <cstring>
#include <string_view>

#include "ADT/MachO.h"

#include "LoadCommandsCommon.h"
#include "Utils/Leb128.h"
#include "Utils/MiscTemplates.h"
#include "Utils/PointerUtils.h"
#include "Utils/SwitchEndian.h"

#include "LoadCommandTemplates.h"

namespace MachO {
    LoadCommand::CmdSizeInvalidType
    ValidateCmdsize(const LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
        switch (LoadCmd->getKind(IsBigEndian)) {
            case LoadCommand::Kind::Segment: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Segment>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SymbolTable: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SymbolTable>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SymbolSegment: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SymbolSegment>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Thread: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Thread>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::UnixThread: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::UnixThread>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadFixedVMSharedLibrary:
            case LoadCommand::Kind::IdFixedVMSharedLibrary: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadFixedVMSharedLibrary>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Ident: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Ident>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::FixedVMFile: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::FixedVMFile>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::PrePage:
                return LoadCommand::CmdSizeInvalidType::None;

            case LoadCommand::Kind::DynamicSymbolTable: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DynamicSymbolTable>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::IdDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::IdDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadDylinker: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadDylinker>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::IdDylinker: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::IdDylinker>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::PreBoundDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::PreBoundDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Routines: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::Routines>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubFramework: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubFramework>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubUmbrella: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubUmbrella>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubClient: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubClient>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SubLibrary: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SubLibrary>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::TwoLevelHints: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::TwoLevelHints>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::PrebindChecksum: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::PrebindChecksum>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadWeakDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadWeakDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Segment64: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::Segment64>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Routines64: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::Routines64>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Uuid: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Uuid>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Rpath: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Rpath>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::CodeSignature: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::CodeSignature>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SegmentSplitInfo: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SegmentSplitInfo>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::ReexportDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::ReexportDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LazyLoadDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LazyLoadDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::EncryptionInfo: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::EncryptionInfo>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldInfo: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldInfo>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldInfoOnly: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldInfoOnly>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LoadUpwardDylib: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LoadUpwardDylib>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumMacOSX: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumMacOSX>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumIPhoneOS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumIPhoneOS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::FunctionStarts: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::FunctionStarts>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldEnvironment: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldEnvironment>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Main: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Main>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DataInCode: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DataInCode>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::SourceVersion: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::SourceVersion>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DylibCodeSignDRS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DylibCodeSignDRS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::EncryptionInfo64: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::EncryptionInfo64>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LinkerOption: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LinkerOption>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::LinkerOptimizationHint: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::LinkerOptimizationHint>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumTvOS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumTvOS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::VersionMinimumWatchOS: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::VersionMinimumWatchOS>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::Note: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<LoadCommand::Kind::Note>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::BuildVersion: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::BuildVersion>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldExportsTrie: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldExportsTrie>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
            }

            case LoadCommand::Kind::DyldChainedFixups: {
                using ConstPtrType =
                    LoadCommandConstPtrTypeFromKind<
                        LoadCommand::Kind::DyldChainedFixups>;

                const auto RealLoadCmd = static_cast<ConstPtrType>(LoadCmd);
                return RealLoadCmd->hasValidCmdSize(IsBigEndian);
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

    SegmentCommand::SectionList
    SegmentCommand::GetSectionList(bool IsBigEndian) noexcept {
        const auto Ptr = reinterpret_cast<Section *>(this + 1);
        return SegmentCommand::SectionList(Ptr, getSectionCount(IsBigEndian));
    }

    SegmentCommand::ConstSectionList
    SegmentCommand::GetConstSectionList(bool IsBigEndian) const noexcept {
        const auto Ptr = reinterpret_cast<const Section *>(this + 1);
        const auto Count = getSectionCount(IsBigEndian);

        return SegmentCommand::ConstSectionList(Ptr, Count);
    }

    SegmentCommand64::SectionList
    SegmentCommand64::GetSectionList(bool IsBigEndian) noexcept {
        const auto Ptr = reinterpret_cast<Section *>(this + 1);
        const auto Count = getSectionCount(IsBigEndian);

        return SegmentCommand64::SectionList(Ptr, Count);
    }

    SegmentCommand64::ConstSectionList
    SegmentCommand64::GetConstSectionList(bool IsBigEndian) const noexcept {
        const auto Ptr = reinterpret_cast<const Section *>(this + 1);
        const auto Count = getSectionCount(IsBigEndian);

        return SegmentCommand64::ConstSectionList(Ptr, Count);
    }

    [[nodiscard]] static inline bool
    IsLoadCommandStringOffsetValid(uint32_t MinSize,
                                   uint32_t CmdSize,
                                   uint32_t Offset)
    {
        const auto Range = LocationRange::CreateWithEnd(MinSize, CmdSize);
        return Range.containsLocation(Offset);
    }

    [[nodiscard]] static inline uint32_t
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
        const auto Offset = getOffset(IsBigEndian);
        return IsLoadCommandStringOffsetValid(MinSize, CmdSize, Offset);
    }

    uint32_t
    LoadCommandString::GetLength(uint32_t CmdSize,
                                 bool IsBigEndian) const noexcept
    {
        const auto Offset = getOffset(IsBigEndian);
        return LoadCommandStringGetLength(this, Offset, CmdSize);
    }

    [[nodiscard]] static LoadCommandString::GetValueResult
    LoadCmdGetStoredString(const LoadCommand *LC,
                           uint32_t MinSize,
                           uint32_t CmdSize,
                           uint32_t Offset)
    {
        if (!IsLoadCommandStringOffsetValid(MinSize, CmdSize, Offset)) {
            return LoadCommandString::GetStringError::OffsetOutOfBounds;
        }

        const auto Length = LoadCommandStringGetLength(LC, Offset, CmdSize);
        if (Length == 0) {
            return LoadCommandString::GetStringError::EmptyString;
        }

        const auto Data = reinterpret_cast<const char *>(LC) + Offset;
        if ((Offset + Length) == CmdSize || Data[Length] != '\0') {
            return LoadCommandString::GetStringError::NoNullTerminator;
        }

        const auto Result = std::string_view(Data, Length);
        return Result;
    }

    LoadCommandString::GetValueResult
    FixedVMSharedLibraryCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto NameOffset = Info.Name.getOffset(IsBigEndian);
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
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto NameOffset = Info.Name.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    SubFrameworkCommand::GetUmbrella(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto UmbrellaOffset = Umbrella.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, UmbrellaOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    SubClientCommand::GetClient(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto ClientOffset = Client.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, ClientOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    SubUmbrellaCommand::GetUmbrella(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto UmbrellaOffset = Umbrella.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, UmbrellaOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    SubLibraryCommand::GetLibrary(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto LibraryOffset = Library.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, LibraryOffset);

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
        "NakedPreBoundDylibCommand is not same size of PreBoundDylibCommand");

    bool
    PreBoundDylibCommand::IsNameOffsetValid(bool IsBigEndian) const noexcept {
        const auto CmdSize = offsetof(NakedPreBoundDylibCommand, NModules);
        return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
    }

    LoadCommandString::GetValueResult
    PreBoundDylibCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = offsetof(NakedPreBoundDylibCommand, NModules);
        const auto NameOffset = Name.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    PreBoundDylibCommand::GetLinkedModules(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto NameOffset = Name.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    LoadCommandString::GetValueResult
    DylinkerCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto NameOffset = Name.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }

    TypedAllocationOrError<SymTabCommand::Entry32List, SizeRangeError>
    SymTabCommand::GetEntry32List(const MemoryMap &Map,
                                  bool IsBigEndian) const noexcept
    {
        const auto SymCount = getSymbolCount(IsBigEndian);
        if (SymCount == 0) {
            return SizeRangeError::Empty;
        }

        const auto SymOff = getSymbolTableOffset(IsBigEndian);
        auto SymEnd = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(SymTabCommand::Entry32),
                                       SymCount, SymOff, &SymEnd))
        {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry32 *>(Map.getBegin() + SymOff);

        return new Entry32List(Entries, SymCount);
    }

    TypedAllocationOrError<SymTabCommand::Entry64List, SizeRangeError>
    SymTabCommand::GetEntry64List(const MemoryMap &Map,
                                  bool IsBigEndian) const noexcept
    {
        const auto SymCount = getSymbolCount(IsBigEndian);
        if (SymCount == 0) {
            return SizeRangeError::Empty;
        }

        const auto SymOff = getSymbolTableOffset(IsBigEndian);
        auto SymEnd = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(SymTabCommand::Entry64),
                                       SymCount, SymOff, &SymEnd))
        {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry64 *>(Map.getBegin() + SymOff);

        return new Entry64List(Entries, SymCount);
    }

    TypedAllocationOrError<SymTabCommand::ConstEntry32List, SizeRangeError>
    SymTabCommand::GetConstEntry32List(const ConstMemoryMap &Map,
                                       bool IsBigEndian) const noexcept
    {
        const auto SymCount = getSymbolCount(IsBigEndian);
        if (SymCount == 0) {
            return SizeRangeError::Empty;
        }

        const auto SymOff = getSymbolTableOffset(IsBigEndian);
        auto SymEnd = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(SymTabCommand::Entry32), SymCount,
                                       SymOff, &SymEnd))
        {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry32 *>(
                Map.getBegin() + SymOff);

        return new ConstEntry32List(Entries, SymCount);
    }

    TypedAllocationOrError<SymTabCommand::ConstEntry64List, SizeRangeError>
    SymTabCommand::GetConstEntry64List(const ConstMemoryMap &Map,
                                       bool IsBigEndian) const noexcept
    {
        const auto SymCount = getSymbolCount(IsBigEndian);
        if (SymCount == 0) {
            return SizeRangeError::Empty;
        }

        const auto SymOff = getSymbolTableOffset(IsBigEndian);
        auto SymEnd = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(SymTabCommand::Entry64), SymCount,
                                       SymOff, &SymEnd))
        {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry64 *>(
                Map.getBegin() + SymOff);

        return new ConstEntry64List(Entries, SymCount);
    }

    TypedAllocationOrError<DynamicSymTabCommand::Entry32List, SizeRangeError>
    DynamicSymTabCommand::GetExport32List(const MemoryMap &Map,
                                          uint32_t SymOff,
                                          bool IsBigEndian) noexcept
    {
        const auto Count = getExternalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getExternalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry32 *>(Map.getBegin() + SymOff);

        return new Entry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::Entry64List, SizeRangeError>
    DynamicSymTabCommand::GetExport64List(const MemoryMap &Map,
                                          uint32_t SymOff,
                                          bool IsBigEndian) noexcept
    {
        const auto Count = getExternalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getExternalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry64 *>(Map.getBegin() + SymOff);

        return new Entry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::ConstEntry32List,
                           SizeRangeError>

    DynamicSymTabCommand::GetConstExport32List(const ConstMemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) const noexcept
    {
        const auto Count = getExternalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getExternalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry32 *>(
                Map.getBegin() + SymOff);

        return new ConstEntry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::ConstEntry64List,
                           SizeRangeError>
    DynamicSymTabCommand::GetConstExport64List(const ConstMemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) const noexcept
    {
        const auto Count = getExternalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getExternalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry64 *>(
                Map.getBegin() + SymOff);

        return new ConstEntry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::Entry32List, SizeRangeError>
    DynamicSymTabCommand::GetLocalSymbol32List(const MemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) noexcept
    {
        const auto Count = getLocalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getLocalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry32 *>(Map.getBegin() + SymOff);

        return new Entry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::Entry64List, SizeRangeError>
    DynamicSymTabCommand::GetLocalSymbol64List(const MemoryMap &Map,
                                               uint32_t SymOff,
                                               bool IsBigEndian) noexcept
    {
        const auto Count = getLocalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getLocalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<SymbolTableEntry64 *>(Map.getBegin() + SymOff);

        return new Entry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::ConstEntry32List,
                           SizeRangeError>

    DynamicSymTabCommand::GetConstLocalSymbol32List(
        const ConstMemoryMap &Map,
        uint32_t SymOff,
        bool IsBigEndian) const noexcept
    {
        const auto Count = getLocalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getLocalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry32), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry32 *>(
                Map.getBegin() + SymOff);

        return new ConstEntry32List(Entries + Index, Count);
    }

    TypedAllocationOrError<DynamicSymTabCommand::ConstEntry64List,
                           SizeRangeError>

    DynamicSymTabCommand::GetConstLocalSymbol64List(
        const ConstMemoryMap &Map,
        uint32_t SymOff,
        bool IsBigEndian) const noexcept
    {
        const auto Count = getLocalSymbolsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Index = getLocalSymbolsIndex(IsBigEndian);
        auto ByteIndex = uint64_t();

        if (DoesAddOverflow(sizeof(DynamicSymTabCommand::Entry64), Index,
                            &ByteIndex))
        {
            return SizeRangeError::Overflows;
        }

        auto SymEnd = uint64_t();
        if (DoesAddOverflow(SymOff, ByteIndex, &SymEnd)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(SymOff, SymEnd);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const SymbolTableEntry64 *>(
                Map.getBegin() + SymOff);

        return new ConstEntry64List(Entries + Index, Count);
    }

    TypedAllocationOrError<BasicContiguousList<uint32_t>, SizeRangeError>
    DynamicSymTabCommand::GetIndirectSymbolIndexTable(
        const MemoryMap &Map,
        bool IsBigEndian) noexcept
    {
        const auto Count = getIndirectSymbolTableCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Offset = getIndirectSymbolTableOffset(IsBigEndian);
        auto End = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(uint32_t), Count, Offset, &End)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(Offset, End);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<uint32_t *>(Map.getBegin() + Offset);

        return new BasicContiguousList<uint32_t>(Entries, Count);
    }

    TypedAllocationOrError<BasicContiguousList<const uint32_t>,
                           SizeRangeError>

    DynamicSymTabCommand::GetConstIndirectSymbolIndexTable(
        const ConstMemoryMap &Map,
        bool IsBigEndian) noexcept
    {
        const auto Count = getIndirectSymbolTableCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Offset = getIndirectSymbolTableOffset(IsBigEndian);
        auto End = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(uint32_t), Count, Offset, &End)) {
            return SizeRangeError::Overflows;
        }

        const auto LocRange = LocationRange::CreateWithEnd(Offset, End);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const uint32_t *>(Map.getBegin() + Offset);

        return new BasicContiguousList<const uint32_t>(Entries, Count);
    }

    TypedAllocationOrError<TwoLevelHintsCommand::HintList, SizeRangeError>
    TwoLevelHintsCommand::GetHintList(
        const MemoryMap &Map, bool IsBigEndian) noexcept
    {
        const auto Count = getHintsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Offset = getHintsOffset(IsBigEndian);
        auto End = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(Hint), Count, Offset, &End)) {
            return SizeRangeError::PastEnd;
        }

        const auto LocRange = LocationRange::CreateWithEnd(Offset, End);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries = reinterpret_cast<Hint *>(Map.getBegin() + Offset);
        return new HintList(Entries, Count);
    }

    TypedAllocationOrError<TwoLevelHintsCommand::ConstHintList,
                           SizeRangeError>

    TwoLevelHintsCommand::GetConstHintList(const ConstMemoryMap &Map,
                                           bool IsBigEndian) const noexcept
    {
        const auto Count = getHintsCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        const auto Offset = getHintsOffset(IsBigEndian);
        auto End = uint64_t();

        if (DoesMultiplyAndAddOverflow(sizeof(Hint), Count, Offset, &End)) {
            return SizeRangeError::PastEnd;
        }

        const auto LocRange = LocationRange::CreateWithEnd(Offset, End);
        if (!Map.getRange().containsRange(LocRange)) {
            return SizeRangeError::PastEnd;
        }

        const auto Entries =
            reinterpret_cast<const Hint *>(Map.getBegin() + Offset);

        return new ConstHintList(Entries, Count);
    }

    LoadCommandString::GetValueResult
    RpathCommand::GetPath(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto PathOffset = Path.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, PathOffset);

        return Result;
    }

    TypedAllocationOrError<BuildVersionCommand::ToolList, SizeRangeError>
    BuildVersionCommand::getToolList(bool IsBigEndian) noexcept {
        const auto Count = getToolCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        auto End = uint64_t();
        const auto Entries = reinterpret_cast<Tool *>(this + 1);

        if (DoesMultiplyAndAddOverflow(sizeof(Tool), Count, sizeof(*this),
                                       &End))
        {
            return SizeRangeError::Overflows;
        }

        const auto CmdSize = getCmdSize(IsBigEndian);
        if (End > CmdSize) {
            return SizeRangeError::PastEnd;
        }

        return new ToolList(Entries, Count);
    }

    TypedAllocationOrError<BuildVersionCommand::ConstToolList, SizeRangeError>
    BuildVersionCommand::getConstToolList(bool IsBigEndian) const noexcept {
        const auto Count = getToolCount(IsBigEndian);
        if (Count == 0) {
            return SizeRangeError::Empty;
        }

        auto End = uint64_t();
        const auto Entries = reinterpret_cast<const Tool *>(this + 1);

        if (DoesMultiplyAndAddOverflow(sizeof(Tool), Count, sizeof(*this),
                                       &End))
        {
            return SizeRangeError::Overflows;
        }

        const auto CmdSize = getCmdSize(IsBigEndian);
        if (End > CmdSize) {
            return SizeRangeError::PastEnd;
        }

        return new ConstToolList(Entries, Count);
    }

    LoadCommandString::GetValueResult
    FixedVMFileCommand::GetName(bool IsBigEndian) const noexcept {
        const auto MinSize = sizeof(*this);
        const auto CmdSize = getCmdSize(IsBigEndian);
        const auto NameOffset = Name.getOffset(IsBigEndian);
        const auto Result =
            LoadCmdGetStoredString(this, MinSize, CmdSize, NameOffset);

        return Result;
    }
}
