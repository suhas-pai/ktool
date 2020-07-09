//
//  include/ADT/Mach-O/LoadCommandTemplates.h
//  stool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string_view>

#include "TypeTraits/DisableIfNotConst.h"
#include "TypeTraits/DisableIfNotPointer.h"

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

    template <enum LoadCommandKind Kind>
    struct LoadCommandKindInfo {};

    template <enum LoadCommandKind Kind>
    using LoadCommandTypeFromKind = typename LoadCommandKindInfo<Kind>::Type;

    template <enum LoadCommandKind Kind>
    using LoadCommandPtrTypeFromKind =
        typename LoadCommandKindInfo<Kind>::PtrType;

    template <enum LoadCommandKind Kind>
    using LoadCommandConstPtrTypeFromKind =
        typename LoadCommandKindInfo<Kind>::ConstPtrType;

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Segment> {
        static constexpr const auto Kind = LoadCommandKind::Segment;

        static constexpr const auto Name = std::string_view("LC_SEGMENT");
        static constexpr const auto Description = std::string_view("Segment");

        typedef SegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SymbolTable> {
        static constexpr const auto Kind = LoadCommandKind::SymbolTable;

        static constexpr const auto Name = std::string_view("LC_SYMTAB");
        static constexpr const auto Description =
            std::string_view("Symbol-Table");

        typedef SymTabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SymbolSegment> {
        static constexpr const auto Kind = LoadCommandKind::SymbolSegment;

        static constexpr const auto Name = std::string_view("LC_SYMSEG");
        static constexpr const auto Description =
            std::string_view("Symbol-Segment");

        typedef SymbolSegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Thread> {
        static constexpr const auto Kind = LoadCommandKind::Thread;

        static constexpr const auto Name = std::string_view("LC_THREAD");
        static constexpr const auto Description = std::string_view("Thread");

        typedef ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::UnixThread> {
        static constexpr const auto Kind = LoadCommandKind::UnixThread;

        static constexpr const auto Name = std::string_view("LC_UNIXTHREAD");
        static constexpr const auto Description =
            std::string_view("Unix-Thread");

        typedef ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadFixedVMSharedLibrary> {
        static constexpr const auto Kind =
            LoadCommandKind::LoadFixedVMSharedLibrary;

        static constexpr const auto Name = std::string_view("LC_LOADFVMLIB");
        static constexpr const auto Description =
            std::string_view("Load Fixed VM Shared-Library");

        typedef FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdFixedVMSharedLibrary> {
        static constexpr const auto Kind =
            LoadCommandKind::IdFixedVMSharedLibrary;

        static constexpr const auto Name = std::string_view("LC_IDFVMLIB");
        static constexpr const auto Description =
            std::string_view("Id Fixed VM Shared-Library");

        typedef FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Ident> {
        static constexpr const auto Kind = LoadCommandKind::Ident;

        static constexpr const auto Name = std::string_view("LC_IDENT");
        static constexpr const auto Description = std::string_view("Identity");

        typedef IdentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FixedVMFile> {
        static constexpr const auto Kind = LoadCommandKind::FixedVMFile;

        static constexpr const auto Name = std::string_view("LC_FVMFILE");
        static constexpr const auto Description =
            std::string_view("Fixed VM File");

        typedef FixedVMFileCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PrePage> {
        static constexpr const auto Kind = LoadCommandKind::PrePage;

        static constexpr const auto Name = std::string_view("LC_PREPAGE");
        static constexpr const auto Description = std::string_view("PrePage");

        typedef LoadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DynamicSymbolTable> {
        static constexpr const auto Kind =
            LoadCommandKind::DynamicSymbolTable;

        static constexpr const auto Name = std::string_view("LC_DYSYMTAB");
        static constexpr const auto Description =
            std::string_view("Dynamic Symbol-Table");

        typedef DynamicSymTabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadDylib> {
        static constexpr const auto Kind = LoadCommandKind::LoadDylib;

        static constexpr const auto Name = std::string_view("LC_LOAD_DYLIB");
        static constexpr const auto Description =
            std::string_view("Load-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdDylib> {
        static constexpr const auto Kind = LoadCommandKind::IdDylib;

        static constexpr const auto Name = std::string_view("LC_ID_DYLIB");
        static constexpr const auto Description =
            std::string_view("Id-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadDylinker> {
        static constexpr const auto Kind = LoadCommandKind::LoadDylinker;

        static constexpr const auto Name =
            std::string_view("LC_LOAD_DYLINKER");
        static constexpr const auto Description =
            std::string_view("Load-Dylinker");

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdDylinker> {
        static constexpr const auto Kind = LoadCommandKind::IdDylinker;

        static constexpr const auto Name = std::string_view("LC_ID_DYLINKER");
        static constexpr const auto Description =
            std::string_view("Id-Dylinker");

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PreBoundDylib> {
        static constexpr const auto Kind = LoadCommandKind::PreBoundDylib;

        static constexpr const auto Name =
            std::string_view("LC_PREBOUND_DYLIB");
        static constexpr const auto Description =
            std::string_view("Prebound-Dylib");

        typedef PreBoundDylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Routines> {
        static constexpr const auto Kind = LoadCommandKind::Routines;

        static constexpr const auto Name = std::string_view("LC_ROUTINES");
        static constexpr const auto Description = std::string_view("Routines");

        typedef RoutinesCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubFramework> {
        static constexpr const auto Kind = LoadCommandKind::SubFramework;

        static constexpr const auto Name = std::string_view("LC_SUB_FRAMEWORK");
        static constexpr const auto Description =
            std::string_view("Sub-Framework");

        typedef SubFrameworkCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubUmbrella> {
        static constexpr const auto Kind = LoadCommandKind::SubUmbrella;

        static constexpr const auto Name = std::string_view("LC_SUB_UMBRELLA");
        static constexpr const auto Description =
            std::string_view("Sub-Umbrella");

        typedef SubUmbrellaCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubClient> {
        static constexpr const auto Kind = LoadCommandKind::SubClient;

        static constexpr const auto Name = std::string_view("LC_SUB_CLIENT");
        static constexpr const auto Description =
            std::string_view("Sub-Client");

        typedef SubClientCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubLibrary> {
        static constexpr const auto Kind = LoadCommandKind::SubLibrary;

        static constexpr const auto Name = std::string_view("LC_SUB_LIBRARY");
        static constexpr const auto Description =
            std::string_view("Sub-Library");

        typedef SubLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::TwoLevelHints> {
        static constexpr const auto Kind = LoadCommandKind::TwoLevelHints;

        static constexpr const auto Name =
            std::string_view("LC_TWOLEVEL_HINTS");

        static constexpr const auto Description =
            std::string_view("TwoLevel-Hints");

        typedef TwoLevelHintsCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PrebindChecksum> {
        static constexpr const auto Kind = LoadCommandKind::PrebindChecksum;

        static constexpr const auto Name = std::string_view("LC_PREBIND_CKSUM");
        static constexpr const auto Description =
            std::string_view("Prebind=Checksum");

        typedef PrebindChecksumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadWeakDylib> {
        static constexpr const auto Kind = LoadCommandKind::LoadWeakDylib;

        static constexpr const auto Name =
            std::string_view("LC_LOAD_WEAK_DYLIB");

        static constexpr const auto Description =
            std::string_view("Load Weak-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Segment64> {
        static constexpr const auto Kind = LoadCommandKind::Segment64;

        static constexpr const auto Name = std::string_view("LC_SEGMENT_64");
        static constexpr const auto Description =
            std::string_view("Segment (64-Bit)");

        typedef SegmentCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Routines64> {
        static constexpr const auto Kind = LoadCommandKind::Routines64;

        static constexpr const auto Name = std::string_view("LC_ROUTINES_64");
        static constexpr const auto Description =
            std::string_view("Routines (64-Bit)");

        typedef RoutinesCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Uuid> {
        static constexpr const auto Kind = LoadCommandKind::Uuid;

        static constexpr const auto Name = std::string_view("LC_UUID");
        static constexpr const auto Description = std::string_view("Uuid");

        typedef UuidCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Rpath> {
        static constexpr const auto Kind = LoadCommandKind::Rpath;

        static constexpr const auto Name = std::string_view("LC_RPATH");
        static constexpr const auto Description = std::string_view("Rpath");

        typedef RpathCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::CodeSignature> {
        static constexpr const auto Kind = LoadCommandKind::CodeSignature;

        static constexpr const auto Name =
            std::string_view("LC_CODE_SIGNATURE");
        static constexpr const auto Description =
            std::string_view("Code-Signature");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SegmentSplitInfo> {
        static constexpr const auto Kind = LoadCommandKind::SegmentSplitInfo;

        static constexpr const auto Name =
            std::string_view("LC_SEGMENT_SPLIT_INFO");
        static constexpr const auto Description =
            std::string_view("Segment Split-Info");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::ReexportDylib> {
        static constexpr const auto Kind = LoadCommandKind::ReexportDylib;

        static constexpr const auto Name = std::string_view("LC_REEXPORT_DYLIB");
        static constexpr const auto Description =
            std::string_view("Re-export Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LazyLoadDylib> {
        static constexpr const auto Kind = LoadCommandKind::LazyLoadDylib;

        static constexpr const auto Name =
            std::string_view("LC_LAZY_LOAD_DYLIB");
        static constexpr const auto Description =
            std::string_view("Lazy Load-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::EncryptionInfo> {
        static constexpr const auto Kind = LoadCommandKind::EncryptionInfo;

        static constexpr const auto Name =
            std::string_view("LC_ENCRYTPION_INFO");
        static constexpr const auto Description =
            std::string_view("Encryption Info");

        typedef EncryptionInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldInfo> {
        static constexpr const auto Kind = LoadCommandKind::DyldInfo;

        static constexpr const auto Name = std::string_view("LC_DYLD_INFO");
        static constexpr const auto Description = std::string_view("Dyld Info");

        typedef DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldInfoOnly> {
        static constexpr const auto Kind = LoadCommandKind::DyldInfoOnly;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_INFO_ONLY");
        static constexpr const auto Description =
            std::string_view("Dyld-Info Only");

        typedef DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadUpwardDylib> {
        static constexpr const auto Kind = LoadCommandKind::LoadUpwardDylib;

        static constexpr const auto Name =
            std::string_view("LC_LOAD_UPWARD_DYLIB");
        static constexpr const auto Description =
            std::string_view("Load Upward Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumMacOSX> {
        static constexpr const auto Kind =
            LoadCommandKind::VersionMinimumMacOSX;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_MACOSX");
        static constexpr const auto Description =
            std::string_view("Version Minimum (macOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumIPhoneOS> {
        static constexpr const auto Kind =
            LoadCommandKind::VersionMinimumIPhoneOS;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_IPHONEOS");
        static constexpr const auto Description =
            std::string_view("Version Minimum (iOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FunctionStarts> {
        static constexpr const auto Kind = LoadCommandKind::FunctionStarts;

        static constexpr const auto Name =
            std::string_view("LC_FUNCTION_STARTS");
        static constexpr const auto Description =
            std::string_view("Function Starts");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldEnvironment> {
        static constexpr const auto Kind = LoadCommandKind::DyldEnvironment;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_ENVIRONMENT");
        static constexpr const auto Description =
            std::string_view("Dyld Environment");

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Main> {
        static constexpr const auto Kind = LoadCommandKind::Main;

        static constexpr const auto Name = std::string_view("LC_MAIN");
        static constexpr const auto Description = std::string_view("Main");

        typedef EntryPointCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DataInCode> {
        static constexpr const auto Kind = LoadCommandKind::DataInCode;

        static constexpr const auto Name = std::string_view("LC_DATA_IN_CODE");
        static constexpr const auto Description =
            std::string_view("Data In Code");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SourceVersion> {
        static constexpr const auto Kind = LoadCommandKind::SourceVersion;

        static constexpr const auto Name =
            std::string_view("LC_SOURCE_VERSION");
        static constexpr const auto Description =
            std::string_view("Source Version");

        typedef SourceVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DylibCodeSignDRS> {
        static constexpr const auto Kind = LoadCommandKind::DylibCodeSignDRS;

        static constexpr const auto Name =
            std::string_view("LC_DYLIB_CODE_SIGN_DRS");
        static constexpr const auto Description =
            std::string_view("Dylib CodeSign DRS");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::EncryptionInfo64> {
        static constexpr const auto Kind = LoadCommandKind::EncryptionInfo64;

        static constexpr const auto Name =
            std::string_view("LC_ENCRYPTION_INFO_64");
        static constexpr const auto Description =
            std::string_view("Encryption Info (64-Bit)");

        typedef EncryptionInfoCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LinkerOption> {
        static constexpr const auto Kind = LoadCommandKind::LinkerOption;

        static constexpr const auto Name =
            std::string_view("LC_LINKER_OPTION");
        static constexpr const auto Description =
            std::string_view("Linker Option");

        typedef LinkerOptionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LinkerOptimizationHint>
    {
        static constexpr const auto Name =
            std::string_view("LC_LINKER_OPTIMIZATION_HINT");
        static constexpr const auto Description =
            std::string_view("Linker Optimization Hint");

        static constexpr const auto Kind =
            LoadCommandKind::LinkerOptimizationHint;

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumTvOS> {
        static constexpr const auto Kind =
            LoadCommandKind::VersionMinimumTvOS;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_TVOS");
        static constexpr const auto Description =
            std::string_view("Version Minimum (tvOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumWatchOS> {
        static constexpr const auto Kind =
            LoadCommandKind::VersionMinimumWatchOS;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_WATCHOS");
        static constexpr const auto Description =
            std::string_view("Version Minimum (watchOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Note> {
        static constexpr const auto Kind = LoadCommandKind::Note;

        static constexpr const auto Name = std::string_view("LC_NOTE");
        static constexpr const auto Description = std::string_view("Note");

        typedef NoteCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::BuildVersion> {
        static constexpr const auto Kind = LoadCommandKind::BuildVersion;

        static constexpr const auto Name =
            std::string_view("LC_BUILD_VERSION");
        static constexpr const auto Description =
            std::string_view("Build Version");

        typedef BuildVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldExportsTrie> {
        static constexpr const auto Kind = LoadCommandKind::DyldExportsTrie;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_EXPORTS_TRIE");
        static constexpr const auto Description =
            std::string_view("Dyld Exports-Trie");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldChainedFixups> {
        static constexpr const auto Kind = LoadCommandKind::DyldChainedFixups;
        static constexpr const auto Name =
            std::string_view("LC_DYLD_CHAINED_FIXUPS");
        static constexpr const auto Description =
            std::string_view("Dyld Chained-Fixups");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <typename LoadCommandType>
    using EnableIfLoadCommandSubtype =
        typename std::enable_if_t<
            std::is_base_of_v<LoadCommand, LoadCommandType>, LoadCommandType>;
}
