/*
 * MachO/LibraryList.cpp
 * © suhas pai
 */

#include "MachO/LibraryList.h"
#include "MachO/LoadCommands.h"

namespace MachO {
    LibraryList::LibraryList(const MachO::LoadCommandsMap &Map,
                             const bool IsBigEndian) noexcept
    {
        for (const auto &LC : Map) {
            switch (LC.kind(IsBigEndian)) {
                case LoadCommandKind::LoadDylib:
                case LoadCommandKind::LoadWeakDylib:
                case LoadCommandKind::ReexportDylib:
                case LoadCommandKind::LazyLoadDylib:
                case LoadCommandKind::LoadUpwardDylib:
                    addLibrary(cast<DylibCommand>(LC, IsBigEndian),
                               IsBigEndian);
                    break;
                case LoadCommandKind::Segment:
                case LoadCommandKind::SymbolTable:
                case LoadCommandKind::SymbolSegment:
                case LoadCommandKind::Thread:
                case LoadCommandKind::UnixThread:
                case LoadCommandKind::LoadFixedVMSharedLib:
                case LoadCommandKind::IdFixedVMSharedLib:
                case LoadCommandKind::Identity:
                case LoadCommandKind::FixedVMFile:
                case LoadCommandKind::PrePage:
                case LoadCommandKind::DynamicSymbolTable:
                case LoadCommandKind::IdDylib:
                case LoadCommandKind::LoadDylinker:
                case LoadCommandKind::IdDylinker:
                case LoadCommandKind::PreBoundDylib:
                case LoadCommandKind::Routines:
                case LoadCommandKind::SubFramework:
                case LoadCommandKind::SubUmbrella:
                case LoadCommandKind::SubClient:
                case LoadCommandKind::SubLibrary:
                case LoadCommandKind::TwoLevelHints:
                case LoadCommandKind::PreBindChecksum:
                case LoadCommandKind::Segment64:
                case LoadCommandKind::Routines64:
                case LoadCommandKind::Uuid:
                case LoadCommandKind::Rpath:
                case LoadCommandKind::CodeSignature:
                case LoadCommandKind::SegmentSplitInfo:
                case LoadCommandKind::EncryptionInfo:
                case LoadCommandKind::DyldInfo:
                case LoadCommandKind::DyldInfoOnly:
                case LoadCommandKind::VersionMinMacOS:
                case LoadCommandKind::VersionMinIOS:
                case LoadCommandKind::FunctionStarts:
                case LoadCommandKind::DyldEnvironment:
                case LoadCommandKind::Main:
                case LoadCommandKind::DataInCode:
                case LoadCommandKind::SourceVersion:
                case LoadCommandKind::DylibCodeSignDRS:
                case LoadCommandKind::EncryptionInfo64:
                case LoadCommandKind::LinkerOption:
                case LoadCommandKind::LinkerOptimizationHint:
                case LoadCommandKind::VersionMinTVOS:
                case LoadCommandKind::VersionMinWatchOS:
                case LoadCommandKind::Note:
                case LoadCommandKind::BuildVersion:
                case LoadCommandKind::DyldExportsTrie:
                case LoadCommandKind::DyldChainedFixups:
                case LoadCommandKind::FileSetEntry:
                    break;
            }
        }
    }
}