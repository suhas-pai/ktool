/*
 * MachO/LoadCommands.cpp
 * © suhas pai
 */

#include "MachO/LoadCommands.h"

namespace  MachO {
    template <MachO::LoadCommandKind Kind>
    using A = LoadCommandTypeFromKindType<Kind>;

    [[nodiscard]] auto
    ValidateCmdsize(const LoadCommand *const LC,
                    const bool IsBigEndian) noexcept
        -> CmdSizeInvalidKind
    {
        const auto CmdSize = LC->cmdsize(IsBigEndian);
        using B = LoadCommandKind;

        switch (LC->kind(IsBigEndian)) {
            case LoadCommandKind::Segment:
                return A<B::Segment>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SymbolTable:
                return A<B::SymbolTable>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SymbolSegment:
                return A<B::SymbolSegment>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Thread:
                return A<B::Thread>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::UnixThread:
                return A<B::UnixThread>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LoadFixedVMSharedLib:
                return A<B::LoadFixedVMSharedLib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::IdFixedVMSharedLib:
                return A<B::IdFixedVMSharedLib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Identity:
                return A<B::Identity>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::FixedVMFile:
                return A<B::FixedVMFile>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::PrePage:
                return A<B::PrePage>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DynamicSymbolTable:
                return A<B::DynamicSymbolTable>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LoadDylib:
                return A<B::LoadDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::IdDylib:
                return A<B::IdDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LoadDylinker:
                return A<B::LoadDylinker>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::IdDylinker:
                return A<B::IdDylinker>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::PreBoundDylib:
                return A<B::PreBoundDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Routines:
                return A<B::Routines>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SubFramework:
                return A<B::SubFramework>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SubUmbrella:
                return A<B::SubUmbrella>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SubClient:
                return A<B::SubClient>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SubLibrary:
                return A<B::SubLibrary>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::TwoLevelHints:
                return A<B::TwoLevelHints>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::PreBindChecksum:
                return A<B::PreBindChecksum>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LoadWeakDylib:
                return A<B::LoadWeakDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Segment64:
                return A<B::Segment64>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Routines64:
                return A<B::Routines64>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Uuid:
                return A<B::Uuid>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Rpath:
                return A<B::Rpath>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::CodeSignature:
                return A<B::CodeSignature>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SegmentSplitInfo:
                return A<B::SegmentSplitInfo>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::ReexportDylib:
                return A<B::ReexportDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LazyLoadDylib:
                return A<B::LazyLoadDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::EncryptionInfo:
                return A<B::EncryptionInfo>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DyldInfo:
                return A<B::DyldInfo>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DyldInfoOnly:
                return A<B::DyldInfoOnly>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LoadUpwardDylib:
                return A<B::LoadUpwardDylib>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::VersionMinMacOS:
                return A<B::VersionMinMacOS>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::VersionMinIOS:
                return A<B::VersionMinIOS>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::FunctionStarts:
                return A<B::FunctionStarts>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DyldEnvironment:
                return A<B::DyldEnvironment>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Main:
                return A<B::Main>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DataInCode:
                return A<B::DataInCode>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::SourceVersion:
                return A<B::SourceVersion>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DylibCodeSignDRS:
                return A<B::DylibCodeSignDRS>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::EncryptionInfo64:
                return A<B::EncryptionInfo64>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LinkerOption:
                return A<B::LinkerOption>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::LinkerOptimizationHint:
                return A<B::LinkerOptimizationHint>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::VersionMinTVOS:
                return A<B::VersionMinTVOS>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::VersionMinWatchOS:
                return A<B::VersionMinWatchOS>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::Note:
                return A<B::Note>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::BuildVersion:
                return A<B::BuildVersion>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DyldExportsTrie:
                return A<B::DyldExportsTrie>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::DyldChainedFixups:
                return A<B::DyldChainedFixups>::hasValidCmdSize(CmdSize);
            case LoadCommandKind::FileSetEntry:
                return A<B::FileSetEntry>::hasValidCmdSize(CmdSize);
        }

        assert(false && "MachO::ValidateCmdSize() got unknown LoadCommand");
    }
}