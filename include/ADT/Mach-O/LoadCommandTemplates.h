//
//  include/ADT/Mach-O/LoadCommandTemplates.h
//  ktool
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

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Segment> {
        constexpr static auto Kind = LoadCommandKind::Segment;
        constexpr static auto Name = std::string_view("LC_SEGMENT");
        constexpr static auto Description = std::string_view("Segment");

        typedef SegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SymbolTable> {
        constexpr static auto Kind = LoadCommandKind::SymbolTable;
        constexpr static auto Name = std::string_view("LC_SYMTAB");
        constexpr static auto Description = std::string_view("Symbol-Table");

        typedef SymTabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SymbolSegment> {
        constexpr static auto Kind = LoadCommandKind::SymbolSegment;
        constexpr static auto Name = std::string_view("LC_SYMSEG");
        constexpr static auto Description = std::string_view("Symbol-Segment");

        typedef SymbolSegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Thread> {
        constexpr static auto Kind = LoadCommandKind::Thread;
        constexpr static auto Name = std::string_view("LC_THREAD");
        constexpr static auto Description = std::string_view("Thread");

        typedef ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::UnixThread> {
        constexpr static auto Kind = LoadCommandKind::UnixThread;
        constexpr static auto Name = std::string_view("LC_UNIXTHREAD");
        constexpr static auto Description = std::string_view("Unix-Thread");

        typedef ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadFixedVMSharedLibrary> {
        constexpr static auto Kind =
            LoadCommandKind::LoadFixedVMSharedLibrary;

        constexpr static auto Name = std::string_view("LC_LOADFVMLIB");
        constexpr static auto Description =
            std::string_view("Load Fixed VM Shared-Library");

        typedef FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdFixedVMSharedLibrary> {
        constexpr static auto Kind =
            LoadCommandKind::IdFixedVMSharedLibrary;

        constexpr static auto Name = std::string_view("LC_IDFVMLIB");
        constexpr static auto Description =
            std::string_view("Id Fixed VM Shared-Library");

        typedef FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Ident> {
        constexpr static auto Kind = LoadCommandKind::Ident;
        constexpr static auto Name = std::string_view("LC_IDENT");
        constexpr static auto Description = std::string_view("Identity");

        typedef IdentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FixedVMFile> {
        constexpr static auto Kind = LoadCommandKind::FixedVMFile;
        constexpr static auto Name = std::string_view("LC_FVMFILE");
        constexpr static auto Description = std::string_view("Fixed VM File");

        typedef FixedVMFileCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PrePage> {
        constexpr static auto Kind = LoadCommandKind::PrePage;
        constexpr static auto Name = std::string_view("LC_PREPAGE");
        constexpr static auto Description = std::string_view("PrePage");

        typedef LoadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DynamicSymbolTable> {
        constexpr static auto Kind =
            LoadCommandKind::DynamicSymbolTable;

        constexpr static auto Name = std::string_view("LC_DYSYMTAB");
        constexpr static auto Description =
            std::string_view("Dynamic Symbol-Table");

        typedef DynamicSymTabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadDylib> {
        constexpr static auto Kind = LoadCommandKind::LoadDylib;
        constexpr static auto Name = std::string_view("LC_LOAD_DYLIB");
        constexpr static auto Description = std::string_view("Load-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdDylib> {
        constexpr static auto Kind = LoadCommandKind::IdDylib;
        constexpr static auto Name = std::string_view("LC_ID_DYLIB");
        constexpr static auto Description = std::string_view("Id-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadDylinker> {
        constexpr static auto Kind = LoadCommandKind::LoadDylinker;
        constexpr static auto Name = std::string_view("LC_LOAD_DYLINKER");
        constexpr static auto Description = std::string_view("Load-Dylinker");

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::IdDylinker> {
        constexpr static auto Kind = LoadCommandKind::IdDylinker;
        constexpr static auto Name = std::string_view("LC_ID_DYLINKER");
        constexpr static auto Description = std::string_view("Id-Dylinker");

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PreBoundDylib> {
        constexpr static auto Kind = LoadCommandKind::PreBoundDylib;
        constexpr static auto Name = std::string_view("LC_PREBOUND_DYLIB");
        constexpr static auto Description = std::string_view("Prebound-Dylib");

        typedef PreBoundDylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Routines> {
        constexpr static auto Kind = LoadCommandKind::Routines;
        constexpr static auto Name = std::string_view("LC_ROUTINES");
        constexpr static auto Description = std::string_view("Routines");

        typedef RoutinesCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubFramework> {
        constexpr static auto Kind = LoadCommandKind::SubFramework;
        constexpr static auto Name = std::string_view("LC_SUB_FRAMEWORK");
        constexpr static auto Description = std::string_view("Sub-Framework");

        typedef SubFrameworkCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubUmbrella> {
        constexpr static auto Kind = LoadCommandKind::SubUmbrella;
        constexpr static auto Name = std::string_view("LC_SUB_UMBRELLA");
        constexpr static auto Description = std::string_view("Sub-Umbrella");

        typedef SubUmbrellaCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubClient> {
        constexpr static auto Kind = LoadCommandKind::SubClient;
        constexpr static auto Name = std::string_view("LC_SUB_CLIENT");
        constexpr static auto Description = std::string_view("Sub-Client");

        typedef SubClientCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SubLibrary> {
        constexpr static auto Kind = LoadCommandKind::SubLibrary;
        constexpr static auto Name = std::string_view("LC_SUB_LIBRARY");
        constexpr static auto Description = std::string_view("Sub-Library");

        typedef SubLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::TwoLevelHints> {
        constexpr static auto Kind = LoadCommandKind::TwoLevelHints;
        constexpr static auto Name = std::string_view("LC_TWOLEVEL_HINTS");
        constexpr static auto Description = std::string_view("TwoLevel-Hints");

        typedef TwoLevelHintsCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::PrebindChecksum> {
        constexpr static auto Kind = LoadCommandKind::PrebindChecksum;

        constexpr static auto Name = std::string_view("LC_PREBIND_CKSUM");
        constexpr static auto Description =
            std::string_view("Prebind=Checksum");

        typedef PrebindChecksumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadWeakDylib> {
        constexpr static auto Kind = LoadCommandKind::LoadWeakDylib;
        constexpr static auto Name = std::string_view("LC_LOAD_WEAK_DYLIB");
        constexpr static auto Description = std::string_view("Load Weak-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Segment64> {
        constexpr static auto Kind = LoadCommandKind::Segment64;
        constexpr static auto Name = std::string_view("LC_SEGMENT_64");
        constexpr static auto Description =
            std::string_view("Segment (64-Bit)");

        typedef SegmentCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Routines64> {
        constexpr static auto Kind = LoadCommandKind::Routines64;
        constexpr static auto Name = std::string_view("LC_ROUTINES_64");
        constexpr static auto Description =
            std::string_view("Routines (64-Bit)");

        typedef RoutinesCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Uuid> {
        constexpr static auto Kind = LoadCommandKind::Uuid;
        constexpr static auto Name = std::string_view("LC_UUID");
        constexpr static auto Description = std::string_view("Uuid");

        typedef UuidCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Rpath> {
        constexpr static auto Kind = LoadCommandKind::Rpath;
        constexpr static auto Name = std::string_view("LC_RPATH");
        constexpr static auto Description = std::string_view("Rpath");

        typedef RpathCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::CodeSignature> {
        constexpr static auto Kind = LoadCommandKind::CodeSignature;
        constexpr static auto Name = std::string_view("LC_CODE_SIGNATURE");
        constexpr static auto Description = std::string_view("Code-Signature");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SegmentSplitInfo> {
        constexpr static auto Kind = LoadCommandKind::SegmentSplitInfo;
        constexpr static auto Name = std::string_view("LC_SEGMENT_SPLIT_INFO");
        constexpr static auto Description =
            std::string_view("Segment Split-Info");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::ReexportDylib> {
        constexpr static auto Kind = LoadCommandKind::ReexportDylib;
        constexpr static auto Name = std::string_view("LC_REEXPORT_DYLIB");
        constexpr static auto Description = std::string_view("Re-export Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LazyLoadDylib> {
        constexpr static auto Kind = LoadCommandKind::LazyLoadDylib;
        constexpr static auto Name = std::string_view("LC_LAZY_LOAD_DYLIB");
        constexpr static auto Description =
            std::string_view("Lazy Load-Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::EncryptionInfo> {
        constexpr static auto Kind = LoadCommandKind::EncryptionInfo;
        constexpr static auto Name = std::string_view("LC_ENCRYTPION_INFO");
        constexpr static auto Description =
            std::string_view("Encryption Info");

        typedef EncryptionInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldInfo> {
        constexpr static auto Kind = LoadCommandKind::DyldInfo;
        constexpr static auto Name = std::string_view("LC_DYLD_INFO");
        constexpr static auto Description = std::string_view("Dyld Info");

        typedef DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldInfoOnly> {
        constexpr static auto Kind = LoadCommandKind::DyldInfoOnly;
        constexpr static auto Name = std::string_view("LC_DYLD_INFO_ONLY");
        constexpr static auto Description =
            std::string_view("Dyld-Info Only");

        typedef DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LoadUpwardDylib> {
        constexpr static auto Kind = LoadCommandKind::LoadUpwardDylib;
        constexpr static auto Name = std::string_view("LC_LOAD_UPWARD_DYLIB");
        constexpr static auto Description =
            std::string_view("Load Upward Dylib");

        typedef DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumMacOSX> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumMacOSX;

        constexpr static auto Name = std::string_view("LC_VERSION_MIN_MACOSX");
        constexpr static auto Description =
            std::string_view("Version Minimum (macOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumIPhoneOS> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumIPhoneOS;

        constexpr static auto Name =
            std::string_view("LC_VERSION_MIN_IPHONEOS");
        constexpr static auto Description =
            std::string_view("Version Minimum (iOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FunctionStarts> {
        constexpr static auto Kind = LoadCommandKind::FunctionStarts;
        constexpr static auto Name = std::string_view("LC_FUNCTION_STARTS");
        constexpr static auto Description = std::string_view("Function Starts");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldEnvironment> {
        constexpr static auto Kind = LoadCommandKind::DyldEnvironment;

        constexpr static auto Name = std::string_view("LC_DYLD_ENVIRONMENT");
        constexpr static auto Description =
            std::string_view("Dyld Environment");

        typedef DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Main> {
        constexpr static auto Kind = LoadCommandKind::Main;

        constexpr static auto Name = std::string_view("LC_MAIN");
        constexpr static auto Description = std::string_view("Main");

        typedef EntryPointCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DataInCode> {
        constexpr static auto Kind = LoadCommandKind::DataInCode;

        constexpr static auto Name = std::string_view("LC_DATA_IN_CODE");
        constexpr static auto Description = std::string_view("Data In Code");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::SourceVersion> {
        constexpr static auto Kind = LoadCommandKind::SourceVersion;
        constexpr static auto Name = std::string_view("LC_SOURCE_VERSION");
        constexpr static auto Description = std::string_view("Source Version");

        typedef SourceVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DylibCodeSignDRS> {
        constexpr static auto Kind = LoadCommandKind::DylibCodeSignDRS;

        constexpr static auto Name =
            std::string_view("LC_DYLIB_CODE_SIGN_DRS");
        constexpr static auto Description =
            std::string_view("Dylib CodeSign DRS");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::EncryptionInfo64> {
        constexpr static auto Kind = LoadCommandKind::EncryptionInfo64;
        constexpr static auto Name = std::string_view("LC_ENCRYPTION_INFO_64");
        constexpr static auto Description =
            std::string_view("Encryption Info (64-Bit)");

        typedef EncryptionInfoCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LinkerOption> {
        constexpr static auto Kind = LoadCommandKind::LinkerOption;
        constexpr static auto Name = std::string_view("LC_LINKER_OPTION");
        constexpr static auto Description = std::string_view("Linker Option");

        typedef LinkerOptionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::LinkerOptimizationHint>
    {
        constexpr static auto Name =
            std::string_view("LC_LINKER_OPTIMIZATION_HINT");
        constexpr static auto Description =
            std::string_view("Linker Optimization Hint");

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

        constexpr static auto Name = std::string_view("LC_VERSION_MIN_TVOS");
        constexpr static auto Description =
            std::string_view("Version Minimum (tvOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::VersionMinimumWatchOS> {
        constexpr static auto Kind =
            LoadCommandKind::VersionMinimumWatchOS;

        constexpr static auto Name = std::string_view("LC_VERSION_MIN_WATCHOS");
        constexpr static auto Description =
            std::string_view("Version Minimum (watchOS)");

        typedef VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::Note> {
        constexpr static auto Kind = LoadCommandKind::Note;
        constexpr static auto Name = std::string_view("LC_NOTE");
        constexpr static auto Description = std::string_view("Note");

        typedef NoteCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::BuildVersion> {
        constexpr static auto Kind = LoadCommandKind::BuildVersion;
        constexpr static auto Name = std::string_view("LC_BUILD_VERSION");
        constexpr static auto Description = std::string_view("Build Version");

        typedef BuildVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldExportsTrie> {
        constexpr static auto Kind = LoadCommandKind::DyldExportsTrie;
        constexpr static auto Name = std::string_view("LC_DYLD_EXPORTS_TRIE");
        constexpr static auto Description =
            std::string_view("Dyld Exports-Trie");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::DyldChainedFixups> {
        constexpr static auto Kind = LoadCommandKind::DyldChainedFixups;
        constexpr static auto Name = std::string_view("LC_DYLD_CHAINED_FIXUPS");
        constexpr static auto Description =
            std::string_view("Dyld Chained-Fixups");

        typedef LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommandKind::FileSetEntry> {
        constexpr static auto Kind = LoadCommandKind::FileSetEntry;
        constexpr static auto Name = std::string_view("LC_FILESET_ENTRY");
        constexpr static auto Description = std::string_view("File-Set Entry");

        typedef FileSetEntryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <typename... T>
    struct IsLoadCommandSubType {};

    template <typename T>
    struct IsLoadCommandSubType<T> {
        constexpr static auto value = std::is_base_of_v<LoadCommand, T>;
    };

    template <typename T, typename U, typename... Rest>
    struct IsLoadCommandSubType<T, U, Rest...> {
        constexpr static auto value =
            IsLoadCommandSubType<T>::value &&
            IsLoadCommandSubType<U, Rest...>::value;
    };

    template <typename T, typename... Rest>
    using EnableIfLoadCommandSubtype =
        typename std::enable_if_t<
            IsLoadCommandSubType<T, Rest...>::value, bool>;
}
