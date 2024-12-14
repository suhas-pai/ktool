//
//  ADT/Mach-O/LoadCommandTemplates.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <string_view>
#include "LoadCommandsCommon.h"

namespace MachO {
    struct LoadCommand;
    struct SegmentCommand;
    struct SymTabCommand;
    struct SymbolSegmentCommand;
    struct ThreadCommand;
    struct FixedVMSharedLibraryCommand;
    struct IdentCommand;
    struct FixedVMFileCommand;
    struct DynamicSymTabCommand;
    struct DylibCommand;
    struct DylinkerCommand;
    struct EntryPointCommand;
    struct SourceVersionCommand;
    struct EncryptionInfoCommand64;
    struct LinkerOptionCommand;
    struct PreBoundDylibCommand;
    struct RoutinesCommand;
    struct SubFrameworkCommand;
    struct SubUmbrellaCommand;
    struct SubClientCommand;
    struct SubLibraryCommand;
    struct TwoLevelHintsCommand;
    struct PrebindChecksumCommand;
    struct SegmentCommand64;
    struct RoutinesCommand64;
    struct UuidCommand;
    struct RpathCommand;
    struct LinkeditDataCommand;
    struct EncryptionInfoCommand;
    struct DyldInfoCommand;
    struct VersionMinimumCommand;
    struct NoteCommand;
    struct BuildVersionCommand;
    struct FileSetEntryCommand;

    template <LoadCommandKind Kind>
    struct LoadCommandKindInfo {};

    template <LoadCommandKind Kind>
    using LoadCommandTypeFromKind = typename LoadCommandKindInfo<Kind>::Type;

    template <LoadCommandKind Kind>
    using LoadCommandPtrTypeFromKind =
        typename LoadCommandKindInfo<Kind>::PtrType;

    template <LoadCommandKind Kind>
    using LoadCommandConstPtrTypeFromKind =
        typename LoadCommandKindInfo<Kind>::ConstPtrType;

    using namespace std::literals;

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Segment> {
        constexpr static auto Kind = LoadCommandKind::Segment;
        constexpr static auto Name = "LC_SEGMENT"sv;
        constexpr static auto Description = "Segment"sv;

        typedef SegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SymbolTable> {
        constexpr static auto Kind = LoadCommandKind::SymbolTable;
        constexpr static auto Name = "LC_SYMTAB"sv;
        constexpr static auto Description = "Symbol-Table"sv;

        typedef SymTabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SymbolSegment> {
        constexpr static auto Kind = LoadCommandKind::SymbolSegment;
        constexpr static auto Name = "LC_SYMSEG"sv;
        constexpr static auto Description = "Symbol-Segment"sv;

        typedef SymbolSegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Thread> {
        constexpr static auto Kind = LoadCommandKind::Thread;
        constexpr static auto Name = "LC_THREAD"sv;
        constexpr static auto Description = "Thread"sv;

        typedef ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::UnixThread> {
        constexpr static auto Kind = LoadCommandKind::UnixThread;
        constexpr static auto Name = "LC_UNIXTHREAD"sv;
        constexpr static auto Description = "Unix-Thread"sv;

        typedef ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadFixedVMSharedLibrary> {
        constexpr static auto Kind =
            LoadCommandKind::LoadFixedVMSharedLibrary;

        constexpr static auto Name = "LC_LOADFVMLIB"sv;
        constexpr static auto Description =
            "Load Fixed VM Shared-Library"sv;

        typedef FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdFixedVMSharedLibrary> {
        constexpr static auto Kind =
            LoadCommandKind::IdFixedVMSharedLibrary;

        constexpr static auto Name = "LC_IDFVMLIB"sv;
        constexpr static auto Description =
            "Id Fixed VM Shared-Library"sv;

        typedef FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Ident> {
        constexpr static auto Kind = LoadCommandKind::Ident;
        constexpr static auto Name = "LC_IDENT"sv;
        constexpr static auto Description = "Identity"sv;

        typedef IdentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FixedVMFile> {
        constexpr static auto Kind = LoadCommandKind::FixedVMFile;
        constexpr static auto Name = "LC_FVMFILE"sv;
        constexpr static auto Description = "Fixed VM File"sv;

        typedef FixedVMFileCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PrePage> {
        constexpr static auto Kind = LoadCommandKind::PrePage;
        constexpr static auto Name = "LC_PREPAGE"sv;
        constexpr static auto Description = "PrePage"sv;

        typedef LoadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DynamicSymbolTable> {
        constexpr static auto Kind =
            LoadCommandKind::DynamicSymbolTable;

        constexpr static auto Name = "LC_DYSYMTAB"sv;
        constexpr static auto Description =
            "Dynamic Symbol-Table"sv;

        typedef DynamicSymTabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadDylib> {
        constexpr static auto Kind = LoadCommandKind::LoadDylib;
        constexpr static auto Name = "LC_LOAD_DYLIB"sv;
        constexpr static auto Description = "Load-Dylib"sv;

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdDylib> {
        constexpr static auto Kind = LoadCommandKind::IdDylib;
        constexpr static auto Name = "LC_ID_DYLIB"sv;
        constexpr static auto Description = "Id-Dylib"sv;

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadDylinker> {
        constexpr static auto Kind = LoadCommandKind::LoadDylinker;
        constexpr static auto Name = "LC_LOAD_DYLINKER"sv;
        constexpr static auto Description = "Load-Dylinker"sv;

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdDylinker> {
        constexpr static auto Kind = LoadCommandKind::IdDylinker;
        constexpr static auto Name = "LC_ID_DYLINKER"sv;
        constexpr static auto Description = "Id-Dylinker"sv;

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PreBoundDylib> {
        constexpr static auto Kind = LoadCommandKind::PreBoundDylib;
        constexpr static auto Name = "LC_PREBOUND_DYLIB"sv;
        constexpr static auto Description = "Prebound-Dylib"sv;

        typedef PreBoundDylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Routines> {
        constexpr static auto Kind = LoadCommandKind::Routines;
        constexpr static auto Name = "LC_ROUTINES"sv;
        constexpr static auto Description = "Routines"sv;

        typedef RoutinesCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubFramework> {
        constexpr static auto Kind = LoadCommandKind::SubFramework;
        constexpr static auto Name = "LC_SUB_FRAMEWORK"sv;
        constexpr static auto Description = "Sub-Framework"sv;

        typedef SubFrameworkCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubUmbrella> {
        constexpr static auto Kind = LoadCommandKind::SubUmbrella;
        constexpr static auto Name = "LC_SUB_UMBRELLA"sv;
        constexpr static auto Description = "Sub-Umbrella"sv;

        typedef SubUmbrellaCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubClient> {
        constexpr static auto Kind = LoadCommandKind::SubClient;
        constexpr static auto Name = "LC_SUB_CLIENT"sv;
        constexpr static auto Description = "Sub-Client"sv;

        typedef SubClientCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubLibrary> {
        constexpr static auto Kind = LoadCommandKind::SubLibrary;
        constexpr static auto Name = "LC_SUB_LIBRARY"sv;
        constexpr static auto Description = "Sub-Library"sv;

        typedef SubLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::TwoLevelHints> {
        constexpr static auto Kind = LoadCommandKind::TwoLevelHints;
        constexpr static auto Name = "LC_TWOLEVEL_HINTS"sv;
        constexpr static auto Description = "TwoLevel-Hints"sv;

        typedef TwoLevelHintsCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PrebindChecksum> {
        constexpr static auto Kind = LoadCommandKind::PrebindChecksum;

        constexpr static auto Name = "LC_PREBIND_CKSUM"sv;
        constexpr static auto Description =
            "Prebind=Checksum"sv;

        typedef PrebindChecksumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadWeakDylib> {
        constexpr static auto Kind = LoadCommandKind::LoadWeakDylib;
        constexpr static auto Name = "LC_LOAD_WEAK_DYLIB"sv;
        constexpr static auto Description = "Load Weak-Dylib"sv;

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Segment64> {
        constexpr static auto Kind = LoadCommandKind::Segment64;
        constexpr static auto Name = "LC_SEGMENT_64"sv;
        constexpr static auto Description =
            "Segment (64-Bit)"sv;

        typedef SegmentCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Routines64> {
        constexpr static auto Kind = LoadCommandKind::Routines64;
        constexpr static auto Name = "LC_ROUTINES_64"sv;
        constexpr static auto Description =
            "Routines (64-Bit)"sv;

        typedef RoutinesCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Uuid> {
        constexpr static auto Kind = LoadCommandKind::Uuid;
        constexpr static auto Name = "LC_UUID"sv;
        constexpr static auto Description = "Uuid"sv;

        typedef UuidCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Rpath> {
        constexpr static auto Kind = LoadCommandKind::Rpath;
        constexpr static auto Name = "LC_RPATH"sv;
        constexpr static auto Description = "Rpath"sv;

        typedef RpathCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::CodeSignature> {
        constexpr static auto Kind = LoadCommandKind::CodeSignature;
        constexpr static auto Name = "LC_CODE_SIGNATURE"sv;
        constexpr static auto Description = "Code-Signature"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SegmentSplitInfo> {
        constexpr static auto Kind = LoadCommandKind::SegmentSplitInfo;
        constexpr static auto Name = "LC_SEGMENT_SPLIT_INFO"sv;
        constexpr static auto Description =
            "Segment Split-Info"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::ReexportDylib> {
        constexpr static auto Kind = LoadCommandKind::ReexportDylib;
        constexpr static auto Name = "LC_REEXPORT_DYLIB"sv;
        constexpr static auto Description = "Re-export Dylib"sv;

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LazyLoadDylib> {
        constexpr static auto Kind = LoadCommandKind::LazyLoadDylib;
        constexpr static auto Name = "LC_LAZY_LOAD_DYLIB"sv;
        constexpr static auto Description =
            "Lazy Load-Dylib"sv;

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::EncryptionInfo> {
        constexpr static auto Kind = LoadCommandKind::EncryptionInfo;
        constexpr static auto Name = "LC_ENCRYPTION_INFO"sv;
        constexpr static auto Description =
            "Encryption Info"sv;

        typedef EncryptionInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldInfo> {
        constexpr static auto Kind = LoadCommandKind::DyldInfo;
        constexpr static auto Name = "LC_DYLD_INFO"sv;
        constexpr static auto Description = "Dyld Info"sv;

        typedef DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldInfoOnly> {
        constexpr static auto Kind = LoadCommandKind::DyldInfoOnly;
        constexpr static auto Name = "LC_DYLD_INFO_ONLY"sv;
        constexpr static auto Description =
            "Dyld-Info Only"sv;

        typedef DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadUpwardDylib> {
        constexpr static auto Kind = LoadCommandKind::LoadUpwardDylib;
        constexpr static auto Name = "LC_LOAD_UPWARD_DYLIB"sv;
        constexpr static auto Description =
            "Load Upward Dylib"sv;

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumMacOSX> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumMacOSX;

        constexpr static auto Name = "LC_VERSION_MIN_MACOSX"sv;
        constexpr static auto Description =
            "Version Minimum (macOS)"sv;

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumIPhoneOS> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumIPhoneOS;

        constexpr static auto Name =
            "LC_VERSION_MIN_IPHONEOS"sv;
        constexpr static auto Description =
            "Version Minimum (iOS)"sv;

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FunctionStarts> {
        constexpr static auto Kind = LoadCommandKind::FunctionStarts;
        constexpr static auto Name = "LC_FUNCTION_STARTS"sv;
        constexpr static auto Description = "Function Starts"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldEnvironment> {
        constexpr static auto Kind = LoadCommandKind::DyldEnvironment;

        constexpr static auto Name = "LC_DYLD_ENVIRONMENT"sv;
        constexpr static auto Description =
            "Dyld Environment"sv;

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Main> {
        constexpr static auto Kind = LoadCommandKind::Main;

        constexpr static auto Name = "LC_MAIN"sv;
        constexpr static auto Description = "Main"sv;

        typedef EntryPointCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DataInCode> {
        constexpr static auto Kind = LoadCommandKind::DataInCode;

        constexpr static auto Name = "LC_DATA_IN_CODE"sv;
        constexpr static auto Description = "Data In Code"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SourceVersion> {
        constexpr static auto Kind = LoadCommandKind::SourceVersion;
        constexpr static auto Name = "LC_SOURCE_VERSION"sv;
        constexpr static auto Description = "Source Version"sv;

        typedef SourceVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DylibCodeSignDRS> {
        constexpr static auto Kind = LoadCommandKind::DylibCodeSignDRS;

        constexpr static auto Name =
            "LC_DYLIB_CODE_SIGN_DRS"sv;
        constexpr static auto Description =
            "Dylib CodeSign DRS"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::EncryptionInfo64> {
        constexpr static auto Kind = LoadCommandKind::EncryptionInfo64;
        constexpr static auto Name = "LC_ENCRYPTION_INFO_64"sv;
        constexpr static auto Description =
            "Encryption Info (64-Bit)"sv;

        typedef EncryptionInfoCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LinkerOption> {
        constexpr static auto Kind = LoadCommandKind::LinkerOption;
        constexpr static auto Name = "LC_LINKER_OPTION"sv;
        constexpr static auto Description = "Linker Option"sv;

        typedef LinkerOptionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LinkerOptimizationHint>
    {
        constexpr static auto Name =
            "LC_LINKER_OPTIMIZATION_HINT"sv;
        constexpr static auto Description =
            "Linker Optimization Hint"sv;

        constexpr static auto Kind =
            LoadCommandKind::LinkerOptimizationHint;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumTvOS> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumTvOS;

        constexpr static auto Name = "LC_VERSION_MIN_TVOS"sv;
        constexpr static auto Description =
            "Version Minimum (tvOS)"sv;

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumWatchOS> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumWatchOS;

        constexpr static auto Name = "LC_VERSION_MIN_WATCHOS"sv;
        constexpr static auto Description =
            "Version Minimum (watchOS)"sv;

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Note> {
        constexpr static auto Kind = LoadCommandKind::Note;
        constexpr static auto Name = "LC_NOTE"sv;
        constexpr static auto Description = "Note"sv;

        typedef NoteCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::BuildVersion> {
        constexpr static auto Kind = LoadCommandKind::BuildVersion;
        constexpr static auto Name = "LC_BUILD_VERSION"sv;
        constexpr static auto Description = "Build Version"sv;

        typedef BuildVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldExportsTrie> {
        constexpr static auto Kind = LoadCommandKind::DyldExportsTrie;
        constexpr static auto Name = "LC_DYLD_EXPORTS_TRIE"sv;
        constexpr static auto Description =
            "Dyld Exports-Trie"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldChainedFixups> {
        constexpr static auto Kind = LoadCommandKind::DyldChainedFixups;
        constexpr static auto Name = "LC_DYLD_CHAINED_FIXUPS"sv;
        constexpr static auto Description =
            "Dyld Chained-Fixups"sv;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FileSetEntry> {
        constexpr static auto Kind = LoadCommandKind::FileSetEntry;
        constexpr static auto Name = "LC_FILESET_ENTRY"sv;
        constexpr static auto Description = "File-Set Entry"sv;

        typedef FileSetEntryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <typename T>
    concept LoadCommandSubtype = std::is_base_of_v<LoadCommand, T>;
}
