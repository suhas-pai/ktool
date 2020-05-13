//
//  include/ADT/Mach-O/LoadCommandTemplates.h
//  stool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string_view>

#include "ADT/Mach-O/LoadCommands.h"
#include "TypeTraits/DisableIfNotConst.h"
#include "TypeTraits/DisableIfNotPointer.h"

namespace MachO {
    template <enum LoadCommand::Kind Kind>
    struct LoadCommandKindInfo {};

    template <enum LoadCommand::Kind Kind>
    using LoadCommandTypeFromKind = typename LoadCommandKindInfo<Kind>::Type;

    template <enum LoadCommand::Kind Kind>
    using LoadCommandPtrTypeFromKind =
        typename LoadCommandKindInfo<Kind>::PtrType;

    template <enum LoadCommand::Kind Kind>
    using LoadCommandConstPtrTypeFromKind =
        typename LoadCommandKindInfo<Kind>::ConstPtrType;

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Segment> {
        static constexpr const auto Kind = LoadCommand::Kind::Segment;

        static constexpr const auto Name = std::string_view("LC_SEGMENT");
        static constexpr const auto Description = std::string_view("Segment");

        typedef MachO::SegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SymbolTable> {
        static constexpr const auto Kind = LoadCommand::Kind::SymbolTable;

        static constexpr const auto Name = std::string_view("LC_SYMTAB");
        static constexpr const auto Description =
            std::string_view("Symbol-Table");

        typedef MachO::SymtabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SymbolSegment> {
        static constexpr const auto Kind = LoadCommand::Kind::SymbolSegment;

        static constexpr const auto Name = std::string_view("LC_SYMSEG");
        static constexpr const auto Description =
            std::string_view("Symbol-Segment");

        typedef MachO::SymbolSegmentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Thread> {
        static constexpr const auto Kind = LoadCommand::Kind::Thread;

        static constexpr const auto Name = std::string_view("LC_THREAD");
        static constexpr const auto Description = std::string_view("Thread");

        typedef MachO::ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::UnixThread> {
        static constexpr const auto Kind = LoadCommand::Kind::UnixThread;

        static constexpr const auto Name = std::string_view("LC_UNIXTHREAD");
        static constexpr const auto Description =
            std::string_view("Unix-Thread");

        typedef MachO::ThreadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LoadFixedVMSharedLibrary> {
        static constexpr const auto Kind =
            LoadCommand::Kind::LoadFixedVMSharedLibrary;

        static constexpr const auto Name = std::string_view("LC_LOADFVMLIB");
        static constexpr const auto Description =
            std::string_view("Load Fixed VM Shared-Library");

        typedef MachO::FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::IdFixedVMSharedLibrary> {
        static constexpr const auto Kind =
            LoadCommand::Kind::IdFixedVMSharedLibrary;

        static constexpr const auto Name = std::string_view("LC_IDFVMLIB");
        static constexpr const auto Description =
            std::string_view("Id Fixed VM Shared-Library");

        typedef MachO::FixedVMSharedLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Ident> {
        static constexpr const auto Kind = LoadCommand::Kind::Ident;

        static constexpr const auto Name = std::string_view("LC_IDENT");
        static constexpr const auto Description = std::string_view("Identity");

        typedef MachO::IdentCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::FixedVMFile> {
        static constexpr const auto Kind = LoadCommand::Kind::FixedVMFile;

        static constexpr const auto Name = std::string_view("LC_FVMFILE");
        static constexpr const auto Description =
            std::string_view("Fixed VM File");

        typedef MachO::FixedVMFileCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::PrePage> {
        static constexpr const auto Kind = LoadCommand::Kind::PrePage;

        static constexpr const auto Name = std::string_view("LC_PREPAGE");
        static constexpr const auto Description = std::string_view("PrePage");

        typedef MachO::LoadCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DynamicSymbolTable> {
        static constexpr const auto Kind =
            LoadCommand::Kind::DynamicSymbolTable;

        static constexpr const auto Name = std::string_view("LC_DYSYMTAB");
        static constexpr const auto Description =
            std::string_view("Dynamic Symbol-Table");

        typedef MachO::DynamicSymtabCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LoadDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::LoadDylib;

        static constexpr const auto Name = std::string_view("LC_LOAD_DYLIB");
        static constexpr const auto Description =
            std::string_view("Load-Dylib");

        typedef MachO::DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::IdDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::IdDylib;

        static constexpr const auto Name = std::string_view("LC_ID_DYLIB");
        static constexpr const auto Description =
            std::string_view("Id-Dylib");

        typedef MachO::DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LoadDylinker> {
        static constexpr const auto Kind = LoadCommand::Kind::LoadDylinker;

        static constexpr const auto Name =
            std::string_view("LC_LOAD_DYLINKER");
        static constexpr const auto Description =
            std::string_view("Load-Dylinker");

        typedef MachO::DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::IdDylinker> {
        static constexpr const auto Kind = LoadCommand::Kind::IdDylinker;

        static constexpr const auto Name = std::string_view("LC_ID_DYLINKER");
        static constexpr const auto Description =
            std::string_view("Id-Dylinker");

        typedef MachO::DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::PreBoundDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::PreBoundDylib;

        static constexpr const auto Name =
            std::string_view("LC_PREBOUND_DYLIB");
        static constexpr const auto Description =
            std::string_view("Prebound-Dylib");

        typedef MachO::PreBoundDylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Routines> {
        static constexpr const auto Kind = LoadCommand::Kind::Routines;

        static constexpr const auto Name = std::string_view("LC_ROUTINES");
        static constexpr const auto Description = std::string_view("Routines");

        typedef MachO::RoutinesCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SubFramework> {
        static constexpr const auto Kind = LoadCommand::Kind::SubFramework;

        static constexpr const auto Name = std::string_view("LC_SUB_FRAMEWORK");
        static constexpr const auto Description =
            std::string_view("Sub-Framework");

        typedef MachO::SubFrameworkCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SubUmbrella> {
        static constexpr const auto Kind = LoadCommand::Kind::SubUmbrella;

        static constexpr const auto Name = std::string_view("LC_SUB_UMBRELLA");
        static constexpr const auto Description =
            std::string_view("Sub-Umbrella");

        typedef MachO::SubUmbrellaCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SubClient> {
        static constexpr const auto Kind = LoadCommand::Kind::SubClient;

        static constexpr const auto Name = std::string_view("LC_SUB_CLIENT");
        static constexpr const auto Description =
            std::string_view("Sub-Client");

        typedef MachO::SubClientCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SubLibrary> {
        static constexpr const auto Kind = LoadCommand::Kind::SubLibrary;

        static constexpr const auto Name = std::string_view("LC_SUB_LIBRARY");
        static constexpr const auto Description =
            std::string_view("Sub-Library");

        typedef MachO::SubLibraryCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::TwoLevelHints> {
        static constexpr const auto Kind = LoadCommand::Kind::TwoLevelHints;

        static constexpr const auto Name =
            std::string_view("LC_TWOLEVEL_HINTS");

        static constexpr const auto Description =
            std::string_view("TwoLevel-Hints");

        typedef MachO::TwoLevelHintsCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::PrebindChecksum> {
        static constexpr const auto Kind = LoadCommand::Kind::PrebindChecksum;

        static constexpr const auto Name = std::string_view("LC_PREBIND_CKSUM");
        static constexpr const auto Description =
            std::string_view("Prebind=Checksum");

        typedef MachO::PrebindChecksumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LoadWeakDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::LoadWeakDylib;

        static constexpr const auto Name =
            std::string_view("LC_LOAD_WEAK_DYLIB");

        static constexpr const auto Description =
            std::string_view("Load Weak-Dylib");

        typedef MachO::DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Segment64> {
        static constexpr const auto Kind = LoadCommand::Kind::Segment64;

        static constexpr const auto Name = std::string_view("LC_SEGMENT_64");
        static constexpr const auto Description =
            std::string_view("Segment (64-Bit)");

        typedef MachO::SegmentCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Routines64> {
        static constexpr const auto Kind = LoadCommand::Kind::Routines64;

        static constexpr const auto Name = std::string_view("LC_ROUTINES_64");
        static constexpr const auto Description =
            std::string_view("Routines (64-Bit)");

        typedef MachO::RoutinesCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Uuid> {
        static constexpr const auto Kind = LoadCommand::Kind::Uuid;

        static constexpr const auto Name = std::string_view("LC_UUID");
        static constexpr const auto Description = std::string_view("Uuid");

        typedef MachO::UuidCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Rpath> {
        static constexpr const auto Kind = LoadCommand::Kind::Rpath;

        static constexpr const auto Name = std::string_view("LC_RPATH");
        static constexpr const auto Description = std::string_view("Rpath");

        typedef MachO::RpathCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::CodeSignature> {
        static constexpr const auto Kind = LoadCommand::Kind::CodeSignature;

        static constexpr const auto Name =
            std::string_view("LC_CODE_SIGNATURE");
        static constexpr const auto Description =
            std::string_view("Code-Signature");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SegmentSplitInfo> {
        static constexpr const auto Kind = LoadCommand::Kind::SegmentSplitInfo;

        static constexpr const auto Name =
            std::string_view("LC_SEGMENT_SPLIT_INFO");
        static constexpr const auto Description =
            std::string_view("Segment Split-Info");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::ReexportDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::ReexportDylib;

        static constexpr const auto Name = std::string_view("LC_REEXPORT_DYLIB");
        static constexpr const auto Description =
            std::string_view("Re-export Dylib");

        typedef MachO::DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LazyLoadDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::LazyLoadDylib;

        static constexpr const auto Name =
            std::string_view("LC_LAZY_LOAD_DYLIB");
        static constexpr const auto Description =
            std::string_view("Lazy Load-Dylib");

        typedef MachO::DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::EncryptionInfo> {
        static constexpr const auto Kind = LoadCommand::Kind::EncryptionInfo;

        static constexpr const auto Name =
            std::string_view("LC_ENCRYTPION_INFO");
        static constexpr const auto Description =
            std::string_view("Encryption Info");

        typedef MachO::EncryptionInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DyldInfo> {
        static constexpr const auto Kind = LoadCommand::Kind::DyldInfo;

        static constexpr const auto Name = std::string_view("LC_DYLD_INFO");
        static constexpr const auto Description = std::string_view("Dyld Info");

        typedef MachO::DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DyldInfoOnly> {
        static constexpr const auto Kind = LoadCommand::Kind::DyldInfoOnly;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_INFO_ONLY");
        static constexpr const auto Description =
            std::string_view("Dyld-Info Only");

        typedef MachO::DyldInfoCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LoadUpwardDylib> {
        static constexpr const auto Kind = LoadCommand::Kind::LoadUpwardDylib;

        static constexpr const auto Name =
            std::string_view("LC_LOAD_UPWARD_DYLIB");
        static constexpr const auto Description =
            std::string_view("Load Upward Dylib");

        typedef MachO::DylibCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::VersionMinimumMacOSX> {
        static constexpr const auto Kind =
            LoadCommand::Kind::VersionMinimumMacOSX;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_MACOSX");
        static constexpr const auto Description =
            std::string_view("Version Minimum (macOS)");

        typedef MachO::VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::VersionMinimumIPhoneOS> {
        static constexpr const auto Kind =
            LoadCommand::Kind::VersionMinimumIPhoneOS;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_IPHONEOS");
        static constexpr const auto Description =
            std::string_view("Version Minimum (iOS)");

        typedef MachO::VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::FunctionStarts> {
        static constexpr const auto Kind = LoadCommand::Kind::FunctionStarts;

        static constexpr const auto Name =
            std::string_view("LC_FUNCTION_STARTS");
        static constexpr const auto Description =
            std::string_view("Function Starts");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DyldEnvironment> {
        static constexpr const auto Kind = LoadCommand::Kind::DyldEnvironment;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_ENVIRONMENT");
        static constexpr const auto Description =
            std::string_view("Dyld Environment");

        typedef MachO::DylinkerCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Main> {
        static constexpr const auto Kind = LoadCommand::Kind::Main;

        static constexpr const auto Name = std::string_view("LC_MAIN");
        static constexpr const auto Description = std::string_view("Main");

        typedef MachO::EntryPointCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DataInCode> {
        static constexpr const auto Kind = LoadCommand::Kind::DataInCode;

        static constexpr const auto Name = std::string_view("LC_DATA_IN_CODE");
        static constexpr const auto Description =
            std::string_view("Data In Code");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::SourceVersion> {
        static constexpr const auto Kind = LoadCommand::Kind::SourceVersion;

        static constexpr const auto Name =
            std::string_view("LC_SOURCE_VERSION");
        static constexpr const auto Description =
            std::string_view("Source Version");

        typedef MachO::SourceVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DylibCodeSignDRS> {
        static constexpr const auto Kind = LoadCommand::Kind::DylibCodeSignDRS;

        static constexpr const auto Name =
            std::string_view("LC_DYLIB_CODE_SIGN_DRS");
        static constexpr const auto Description =
            std::string_view("Dylib CodeSign DRS");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::EncryptionInfo64> {
        static constexpr const auto Kind = LoadCommand::Kind::EncryptionInfo64;

        static constexpr const auto Name =
            std::string_view("LC_ENCRYPTION_INFO_64");
        static constexpr const auto Description =
            std::string_view("Encryption Info (64-Bit)");

        typedef MachO::EncryptionInfoCommand64 Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LinkerOption> {
        static constexpr const auto Kind = LoadCommand::Kind::LinkerOption;

        static constexpr const auto Name =
            std::string_view("LC_LINKER_OPTION");
        static constexpr const auto Description =
            std::string_view("Linker Option");

        typedef MachO::LinkerOptionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::LinkerOptimizationHint>
    {
        static constexpr const auto Name =
            std::string_view("LC_LINKER_OPTIMIZATION_HINT");
        static constexpr const auto Description =
            std::string_view("Linker Optimization Hint");

        static constexpr const auto Kind =
            LoadCommand::Kind::LinkerOptimizationHint;

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::VersionMinimumTvOS> {
        static constexpr const auto Kind =
            LoadCommand::Kind::VersionMinimumTvOS;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_TVOS");
        static constexpr const auto Description =
            std::string_view("Version Minimum (tvOS)");

        typedef MachO::VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::VersionMinimumWatchOS> {
        static constexpr const auto Kind =
            LoadCommand::Kind::VersionMinimumWatchOS;

        static constexpr const auto Name =
            std::string_view("LC_VERSION_MIN_WATCHOS");
        static constexpr const auto Description =
            std::string_view("Version Minimum (watchOS)");

        typedef MachO::VersionMinimumCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::Note> {
        static constexpr const auto Kind = LoadCommand::Kind::Note;

        static constexpr const auto Name = std::string_view("LC_NOTE");
        static constexpr const auto Description = std::string_view("Note");

        typedef MachO::NoteCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::BuildVersion> {
        static constexpr const auto Kind = LoadCommand::Kind::BuildVersion;

        static constexpr const auto Name =
            std::string_view("LC_BUILD_VERSION");
        static constexpr const auto Description =
            std::string_view("Build Version");

        typedef MachO::BuildVersionCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DyldExportsTrie> {
        static constexpr const auto Kind = LoadCommand::Kind::DyldExportsTrie;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_EXPORTS_TRIE");
        static constexpr const auto Description =
            std::string_view("Dyld Exports-Trie");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <>
    struct LoadCommandKindInfo<LoadCommand::Kind::DyldChainedFixups> {
        static constexpr const auto Kind = LoadCommand::Kind::DyldChainedFixups;

        static constexpr const auto Name =
            std::string_view("LC_DYLD_CHAINED_FIXUPS");
        static constexpr const auto Description =
            std::string_view("Dyld Chained-Fixups");

        typedef MachO::LinkeditDataCommand Type;
        typedef Type *PtrType;
        typedef const Type *ConstPtrType;
    };

    template <typename LoadCommandType>
    using EnableIfLoadCommandSubtype =
        typename std::enable_if_t<
            std::is_base_of_v<LoadCommand, LoadCommandType>, LoadCommandType>;
}

// isa<T> templats
// isa<Kind>(const MachO::LoadCommand &, bool IsBigEndian)

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline bool isa(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) {
    return (LoadCmd.GetKind(IsBigEndian) == Kind);
}

// isa<Kind>(const MachO::LoadCommand *, bool IsBigEndian)

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline bool isa(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) {
    return (LoadCmd->GetKind(IsBigEndian) == Kind);
}

// isa<MachO::SegmentCommand>(const MachO::LoadCommand &, bool IsBigEndian)

template <typename LoadCommandType,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline
bool isa(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    return LoadCommandType::IsOfKind(LoadCmd.GetKind(IsBigEndian));
}

// isa<MachO::SegmentCommand>(const MachO::LoadCommand *, bool IsBigEndian)

template <typename LoadCommandType,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline
bool isa(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    return LoadCommandType::IsOfKind(LoadCmd->GetKind(IsBigEndian));
}

// isa<MachO::SegmentCommand *>(const MachO::LoadCommand *, bool IsBigEndian)

template <typename LoadCommandTypePtr,
          typename LoadCommandType =
              typename std::remove_pointer_t<LoadCommandTypePtr>,
          typename = TypeTraits::DisableIfNotPointer<LoadCommandTypePtr>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline
bool isa(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    return LoadCommandType::IsOfKind(LoadCmd->GetKind(IsBigEndian));
}

// cast<T> templates
// cast<Kind>(MachO::LoadCommand &, bool IsBigEndian) -> LoadCommandType &

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline
LoadCommandType &cast(MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<Kind>(LoadCmd, IsBigEndian));
    return static_cast<const LoadCommandType &>(LoadCmd);
}

// cast<Kind>(const MachO::LoadCommand &, bool IsBigEndian)
// -> const LoadCommandType &

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline const LoadCommandType &
cast(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<Kind>(LoadCmd, IsBigEndian));
    return static_cast<const LoadCommandType &>(LoadCmd);
}

// cast<Kind>(MachO::LoadCommand *, bool IsBigEndian) -> LoadCommandType *

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline
LoadCommandType *cast(MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<Kind>(LoadCmd, IsBigEndian));
    return static_cast<LoadCommandType *>(LoadCmd);
}

// cast<Kind>(const MachO::LoadCommand *) -> const LoadCommandType *

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline const LoadCommandType *
cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<Kind>(LoadCmd, IsBigEndian));
    return static_cast<LoadCommandType *>(LoadCmd);
}

// cast<const MachO::SegmentCommand>(const MachO::LoadCommand *)
// -> const LoadCommandType *

template <typename LoadCommandType,
          typename = TypeTraits::DisableIfNotConst<LoadCommandType>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline const LoadCommandType *
cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<LoadCommandType>(LoadCmd, IsBigEndian));
    return static_cast<const LoadCommandType *>(LoadCmd);
}

// cast<MachO::SegmentCommand &>(MachO::LoadCommand &)
// -> LoadCommandType *

template <typename LoadCommandTypeRef,
          typename LoadCommandType =
              typename std::remove_reference_t<LoadCommandTypeRef>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline
LoadCommandType &cast(MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<LoadCommandTypeRef>(LoadCmd, IsBigEndian));
    return static_cast<LoadCommandTypeRef>(LoadCmd);
}

// cast<const MachO::SegmentCommand &>(const MachO::LoadCommand &)
// -> const LoadCommandType *

template <typename LoadCommandTypeRef,
          typename LoadCommandType =
              typename std::remove_reference_t<LoadCommandTypeRef>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline const LoadCommandType &
cast(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<LoadCommandType>(LoadCmd, IsBigEndian));
    return static_cast<const LoadCommandType &>(LoadCmd);
}

// cast<MachO::SegmentCommand *>(MachO::LoadCommand *)
// -> LoadCommandType *

template <typename LoadCommandTypePtr,
          typename LoadCommandType =
              typename std::remove_pointer_t<LoadCommandTypePtr>,
          typename = TypeTraits::DisableIfNotPointer<LoadCommandTypePtr>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline
LoadCommandType *cast(MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<LoadCommandTypePtr>(LoadCmd, IsBigEndian));
    return static_cast<LoadCommandTypePtr>(LoadCmd);
}

// cast<const MachO::SegmentCommand *>(const MachO::LoadCommand *)
// -> const LoadCommandType *

template <typename LoadCommandTypePtr,
          typename LoadCommandType =
              typename std::remove_pointer_t<LoadCommandTypePtr>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline const LoadCommandType *
cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<LoadCommandTypePtr>(LoadCmd, IsBigEndian));
    return static_cast<const LoadCommandType *>(LoadCmd);
}

// dyn_cast<T> templates
// dyn_cast<Kind>(MachO::LoadCommand &) -> LoadCommandType &

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline
LoadCommandType &dyn_cast(MachO::LoadCommand &LoadCmd, bool IsBigEndian) {
    if (isa<Kind>(LoadCmd, IsBigEndian)) {
        return cast<LoadCommandType>(LoadCmd);
    }

    return nullptr;
}

// dyn_cast<Kind>(const MachO::LoadCommand &) -> const LoadCommandType &

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline const LoadCommandType *
dyn_cast(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) {
    if (isa<Kind>(LoadCmd, IsBigEndian)) {
        return &cast<LoadCommandType>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<Kind>(MachO::LoadCommand *) -> LoadCommandType *

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline LoadCommandType *
dyn_cast(MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return cast<LoadCommandType>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<Kind>(const MachO::LoadCommand *) -> const LoadCommandType *

template <MachO::LoadCommand::Kind Kind,
          typename LoadCommandType = MachO::LoadCommandTypeFromKind<Kind>>

static inline const LoadCommandType *
dyn_cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return cast<LoadCommandType>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<MachO::SegmentCommand>(MachO::LoadCommand *) -> LoadCommandType *

template <typename LoadCommandType,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline LoadCommandType &
dyn_cast(MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return cast<LoadCommandType>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<MachO::SegmentCommand>(MachO::LoadCommand *) -> LoadCommandType *

template <typename LoadCommandType,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline const LoadCommandType *
dyn_cast(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return &cast<LoadCommandType &>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<MachO::SegmentCommand>(MachO::LoadCommand *) -> LoadCommandType *

template <typename LoadCommandType,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline LoadCommandType *
dyn_cast(MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return &cast<LoadCommandType>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<const MachO::SegmentCommand>(const MachO::LoadCommand *)
// -> const LoadCommandType *

template <typename LoadCommandType,
          typename = TypeTraits::DisableIfNotConst<LoadCommandType>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline const LoadCommandType *
dyn_cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return cast<const LoadCommandType *>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<MachO::SegmentCommand *>(MachO::LoadCommand *)
// -> LoadCommandType *

template <typename LoadCommandTypePtr,
          typename LoadCommandType =
              typename std::remove_pointer_t<LoadCommandTypePtr>,
          typename = TypeTraits::DisableIfNotPointer<LoadCommandTypePtr>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline LoadCommandTypePtr
dyn_cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return cast<LoadCommandTypePtr>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}

// dyn_cast<const MachO::SegmentCommand *>(const MachO::LoadCommand *)
// -> const LoadCommandType *

template <typename LoadCommandTypePtr,
          typename LoadCommandType =
              typename std::remove_pointer_t<LoadCommandTypePtr>,
          typename = TypeTraits::DisableIfNotPointer<LoadCommandTypePtr>,
          typename = TypeTraits::DisableIfNotConst<LoadCommandTypePtr>,
          typename = MachO::EnableIfLoadCommandSubtype<LoadCommandType>>

static inline LoadCommandTypePtr
dyn_cast(const MachO::LoadCommand *LoadCmd, bool IsBigEndian) noexcept {
    if (isa<LoadCommandType>(LoadCmd, IsBigEndian)) {
        return cast<LoadCommandTypePtr>(LoadCmd, IsBigEndian);
    }

    return nullptr;
}
