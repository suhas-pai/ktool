//
//  include/ADT/Mach-O/Headers.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string>

#include "ADT/Mach.h"
#include "ADT/BasicContiguousList.h"
#include "ADT/BasicMasksHandler.h"

#include "Utils/Casting.h"
#include "Utils/SwitchEndian.h"

#include "Types.h"

namespace MachO {
    using namespace std::literals;
    constexpr inline const auto EmptyStringValue = std::string_view();

    struct Header {
        enum class Magic : uint32_t {
            Default = 0xfeedface,
            Default64 = 0xfeedfacf,

            BigEndian = 0xcefaedfe,
            BigEndian64 = 0xcffaedfe,
        };

        template <Magic>
        struct MagicInfo {};

        template <>
        struct MagicInfo<Magic::Default> {
            constexpr static const auto Kind = Magic::Default;

            constexpr static const auto Name = std::string_view("MH_MAGIC");
            constexpr static const auto Description =
                std::string_view("Default");
        };

        template <>
        struct MagicInfo<Magic::Default64> {
            constexpr static const auto Kind = Magic::Default64;

            constexpr static const auto Name = std::string_view("MH_MAGIC_64");
            constexpr static const auto Description =
                std::string_view("Default (64-Bit)");
        };

        template <>
        struct MagicInfo<Magic::BigEndian> {
            constexpr static const auto Kind = Magic::BigEndian;

            constexpr static const auto Name = std::string_view("MH_CIGAM");
            constexpr static const auto Description =
                std::string_view("Big Endian");
        };

        template <>
        struct MagicInfo<Magic::BigEndian64> {
            constexpr static const auto Kind = Magic::BigEndian64;

            constexpr static const auto Name = std::string_view("MH_CIGAM_64");
            constexpr static const auto Description =
                std::string_view("Big Endian (64-Bit)");
        };

        [[nodiscard]] constexpr static bool MagicIsValid(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                case Magic::Default64:
                case Magic::BigEndian:
                case Magic::BigEndian64:
                    return true;
            }

            return false;
        }

        [[nodiscard]] constexpr
        static const std::string_view &MagicGetName(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                    return MagicInfo<Magic::Default>::Name;
                case Magic::Default64:
                    return MagicInfo<Magic::Default64>::Name;
                case Magic::BigEndian:
                    return MagicInfo<Magic::BigEndian>::Name;
                case Magic::BigEndian64:
                    return MagicInfo<Magic::BigEndian64>::Name;
            }

            return EmptyStringValue;
        }

        [[nodiscard]] constexpr static
        const std::string_view &MagicGetDescription(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                    return MagicInfo<Magic::Default>::Description;
                case Magic::Default64:
                    return MagicInfo<Magic::Default64>::Description;
                case Magic::BigEndian:
                    return MagicInfo<Magic::BigEndian>::Description;
                case Magic::BigEndian64:
                    return MagicInfo<Magic::BigEndian64>::Description;
            }

            return EmptyStringValue;
        }

        enum class FileKind : uint32_t {
            Object = 1,
            Executable,
            FixedVMSharedLibrary,
            CoreFile,
            PreloadedExecutable,
            Dylib,
            Dylinker,
            Bundle,
            DylibStub,
            Dsym,
            KextBundle
        };

        template <FileKind>
        struct FileKindInfo {};

        template <>
        struct FileKindInfo<FileKind::Object> {
            constexpr static const auto Kind = FileKind::Object;

            constexpr static const auto Name = std::string_view("MH_OBJECT");
            constexpr static const auto Description =
                std::string_view("Object File");
        };

        template <>
        struct FileKindInfo<FileKind::Executable> {
            constexpr static const auto Kind = FileKind::Executable;

            constexpr static const auto Name = std::string_view("MH_EXECUTE");
            constexpr static const auto Description =
                std::string_view("Executable");
        };

        template <>
        struct FileKindInfo<FileKind::FixedVMSharedLibrary> {
            constexpr static const auto Kind = FileKind::FixedVMSharedLibrary;

            constexpr static const auto Name = std::string_view("MH_FVMLIB");
            constexpr static const auto Description =
                std::string_view("Fixed-VM Shared Library");
        };

        template <>
        struct FileKindInfo<FileKind::CoreFile> {
            constexpr static const auto Kind = FileKind::CoreFile;

            constexpr static const auto Name = std::string_view("MH_CORE");
            constexpr static const auto Description =
                std::string_view("Core File");
        };

        template <>
        struct FileKindInfo<FileKind::PreloadedExecutable> {
            constexpr static const auto Kind = FileKind::PreloadedExecutable;

            constexpr static const auto Name = std::string_view("MH_PRELOAD");
            constexpr static const auto Description =
                std::string_view("Preloaded Executable");
        };

        template <>
        struct FileKindInfo<FileKind::Dylib> {
            constexpr static const auto Kind = FileKind::Dylib;

            constexpr static const auto Name = std::string_view("MH_DYLIB");
            constexpr static const auto Description =
                std::string_view("Dynamic Library (Dylib)");
        };

        template <>
        struct FileKindInfo<FileKind::Dylinker> {
            constexpr static const auto Kind = FileKind::Dylinker;

            constexpr static const auto Name = std::string_view("MH_DYLINKER");
            constexpr static const auto Description =
                std::string_view("Dynamic Linker (Dylinker)");
        };

        template <>
        struct FileKindInfo<FileKind::Bundle> {
            constexpr static const auto Kind = FileKind::Bundle;

            constexpr static const auto Name = std::string_view("MH_BUNDLE");
            constexpr static const auto Description =
                std::string_view("Bundle");
        };

        template <>
        struct FileKindInfo<FileKind::DylibStub> {
            constexpr static const auto Kind = FileKind::DylibStub;

            constexpr static const auto Name =
                std::string_view("MH_DYLIB_STUB");
            constexpr static const auto Description =
                std::string_view("Dylib Stub");
        };

        template <>
        struct FileKindInfo<FileKind::Dsym> {
            constexpr static const auto Kind = FileKind::Dsym;

            constexpr static const auto Name = std::string_view("MH_DSYM");
            constexpr static const auto Description =
                std::string_view("Debug Symbols File (.dSYM)");
        };

        template <>
        struct FileKindInfo<FileKind::KextBundle> {
            constexpr static const auto Kind = FileKind::KextBundle;

            constexpr static const auto Name =
                std::string_view("MH_KEXT_BUNDLE");
            constexpr static const auto Description =
                std::string_view("Kernel Extension Bundle");
        };

        [[nodiscard]] static bool FileKindIsValid(FileKind FileKind) noexcept {
            switch (FileKind) {
                case FileKind::Object:
                case FileKind::Executable:
                case FileKind::FixedVMSharedLibrary:
                case FileKind::CoreFile:
                case FileKind::PreloadedExecutable:
                case FileKind::Dylib:
                case FileKind::Dylinker:
                case FileKind::Bundle:
                case FileKind::DylibStub:
                case FileKind::Dsym:
                case FileKind::KextBundle:
                    return true;
            }

            return false;
        }

        [[nodiscard]] static const std::string_view &
        FileKindGetName(FileKind FileKind) noexcept {
            switch (FileKind) {
                case FileKind::Object:
                    return FileKindInfo<FileKind::Object>::Name;
                case FileKind::Executable:
                    return FileKindInfo<FileKind::Executable>::Name;
                case FileKind::FixedVMSharedLibrary:
                    return FileKindInfo<FileKind::FixedVMSharedLibrary>::Name;
                case FileKind::CoreFile:
                    return FileKindInfo<FileKind::CoreFile>::Name;
                case FileKind::PreloadedExecutable:
                    return FileKindInfo<FileKind::PreloadedExecutable>::Name;
                case FileKind::Dylib:
                    return FileKindInfo<FileKind::Dylib>::Name;
                case FileKind::Dylinker:
                    return FileKindInfo<FileKind::Dylinker>::Name;
                case FileKind::Bundle:
                    return FileKindInfo<FileKind::Bundle>::Name;
                case FileKind::DylibStub:
                    return FileKindInfo<FileKind::DylibStub>::Name;
                case FileKind::Dsym:
                    return FileKindInfo<FileKind::Dsym>::Name;
                case FileKind::KextBundle:
                    return FileKindInfo<FileKind::KextBundle>::Name;
            }

            return EmptyStringValue;
        }

        [[nodiscard]] static const std::string_view &
        FileKindGetDescription(FileKind FileKind) noexcept {
            switch (FileKind) {
                case FileKind::Object:
                    return FileKindInfo<FileKind::Object>::Description;
                case FileKind::Executable:
                    return FileKindInfo<FileKind::Executable>::Description;
                case FileKind::FixedVMSharedLibrary:
                    return
                        FileKindInfo<FileKind::FixedVMSharedLibrary>
                            ::Description;
                case FileKind::CoreFile:
                    return FileKindInfo<FileKind::CoreFile>::Description;
                case FileKind::PreloadedExecutable:
                    return
                        FileKindInfo<
                            FileKind::PreloadedExecutable>::Description;
                case FileKind::Dylib:
                    return FileKindInfo<FileKind::Dylib>::Description;
                case FileKind::Dylinker:
                    return FileKindInfo<FileKind::Dylinker>::Description;
                case FileKind::Bundle:
                    return FileKindInfo<FileKind::Bundle>::Description;
                case FileKind::DylibStub:
                    return FileKindInfo<FileKind::DylibStub>::Description;
                case FileKind::Dsym:
                    return FileKindInfo<FileKind::Dsym>::Description;
                case FileKind::KextBundle:
                    return FileKindInfo<FileKind::KextBundle>::Description;
            }

            return EmptyStringValue;
        }

        enum class FlagsEnum : uint32_t {
            NoUndefinedReferences         = 1 << 0,
            IncrementalLink               = 1 << 1,
            DyldLink                      = 1 << 2,
            BindAtLoad                    = 1 << 3,
            PreBound                      = 1 << 4,
            SplitSegments                 = 1 << 5,
            LazyInitialization            = 1 << 6,
            TwoLevelNamespaceBindings     = 1 << 7,
            ForceFlatNamespaces           = 1 << 8,
            NoMultipleDefinitions         = 1 << 9,
            NoFixPrebinding               = 1 << 10,
            Prebindable                   = 1 << 11,
            AllModulesBound               = 1 << 12,
            SubsectionsViaSymbols         = 1 << 13,
            Canonical                     = 1 << 14,
            WeakDefinitions               = 1 << 15,
            BindsToWeak                   = 1 << 16,
            AllowStackExecution           = 1 << 17,
            RootSafe                      = 1 << 18,
            SetuidSafe                    = 1 << 19,
            NoReexportedDylibs            = 1 << 20,
            PositionIndependentExecutable = 1 << 21,
            DeadStrippableDylib           = 1 << 22,
            HasTlvDescriptors             = 1 << 23,
            NoHeapExecution               = 1 << 24,
            AppExtensionSafe              = 1 << 25,
            NlistOutOfSyncWithDyldInfo    = 1 << 26,
            SimulatorSupport              = 1 << 27,
            DylibInCache                  = static_cast<uint32_t>(1 << 31)
        };

        template <FlagsEnum>
        struct FlagInfo {};

        template <>
        struct FlagInfo<FlagsEnum::NoUndefinedReferences> {
            constexpr static const auto Kind = FlagsEnum::NoUndefinedReferences;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_NOUNDEF");
            constexpr static const auto Description =
                std::string_view("No Undefined References");
        };

        template <>
        struct FlagInfo<FlagsEnum::IncrementalLink> {
            constexpr static const auto Kind = FlagsEnum::IncrementalLink;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_INCRLINK");
            constexpr static const auto Description =
                std::string_view("Incrementally Linked");
        };

        template <>
        struct FlagInfo<FlagsEnum::DyldLink> {
            constexpr static const auto Kind = FlagsEnum::DyldLink;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_DYLDLINK");
            constexpr static const auto Description =
                std::string_view("Dyanmic Linker Input");
        };

        template <>
        struct FlagInfo<FlagsEnum::BindAtLoad> {
            constexpr static const auto Kind = FlagsEnum::BindAtLoad;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_BINDATLOAD");
            constexpr static const auto Description =
                std::string_view("No Undefined References");
        };

        template <>
        struct FlagInfo<FlagsEnum::SplitSegments> {
            constexpr static const auto Kind = FlagsEnum::SplitSegments;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_SPLIT_SEGS");
            constexpr static const auto Description =
                std::string_view("Bind Undefined References at Launch");
        };

        template <>
        struct FlagInfo<FlagsEnum::PreBound> {
            constexpr static const auto Kind = FlagsEnum::PreBound;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_PREBOUND");
            constexpr static const auto Description =
                std::string_view("Dynamic Undefined Reference PreBound");
        };

        template <>
        struct FlagInfo<FlagsEnum::LazyInitialization> {
            constexpr static const auto Kind = FlagsEnum::LazyInitialization;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_LAZY_INIT");
            constexpr static const auto Description =
                std::string_view("Lazy Initialization");
        };

        template <>
        struct FlagInfo<FlagsEnum::TwoLevelNamespaceBindings> {
            constexpr static const auto Kind =
                FlagsEnum::TwoLevelNamespaceBindings;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_TWOLEVEL");
            constexpr static const auto Description =
                std::string_view("Has Two-Level Namespace Bindings");
        };

        template <>
        struct FlagInfo<FlagsEnum::ForceFlatNamespaces> {
            constexpr static const auto Kind = FlagsEnum::ForceFlatNamespaces;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_FORCE_FLAT");
            constexpr static const auto Description =
                std::string_view("Has Two-Level Namespace Bindings");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoMultipleDefinitions> {
            constexpr static const auto Kind = FlagsEnum::NoMultipleDefinitions;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_NOMULTIDEFS");
            constexpr static const auto Description =
                std::string_view("No Multiple Symbol Definitions");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoFixPrebinding> {
            constexpr static const auto Kind = FlagsEnum::NoFixPrebinding;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_NOFIXPREBINDING");
            constexpr static const auto Description =
                std::string_view("Don't Fix PreBinding");
        };

        template <>
        struct FlagInfo<FlagsEnum::Prebindable> {
            constexpr static const auto Kind = FlagsEnum::Prebindable;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_PREBINDABLE");
            constexpr static const auto Description =
                std::string_view("PreBinded - But can be Prebinded Again");
        };

        template <>
        struct FlagInfo<FlagsEnum::AllModulesBound> {
            constexpr static const auto Kind = FlagsEnum::AllModulesBound;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_ALLMODSBOUND");
            constexpr static const auto Description =
                std::string_view("Binds to all Two-Level Namespace Modules");
        };

        template <>
        struct FlagInfo<FlagsEnum::SubsectionsViaSymbols> {
            constexpr static const auto Kind = FlagsEnum::SubsectionsViaSymbols;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_SUBSECTIONS_VIA_SYMBOLS");
            constexpr static const auto Description =
                std::string_view("Safe to Divide Sections into Sub-Sections");
        };

        template <>
        struct FlagInfo<FlagsEnum::Canonical> {
            constexpr static const auto Kind = FlagsEnum::Canonical;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_CANONICAL");
            constexpr static const auto Description =
                std::string_view("Canocalized via UnPreBind");
        };

        template <>
        struct FlagInfo<FlagsEnum::WeakDefinitions> {
            constexpr static const auto Kind = FlagsEnum::WeakDefinitions;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_WEAK_DEFINES");
            constexpr static const auto Description =
                std::string_view("Has External Weak Symbols");
        };

        template <>
        struct FlagInfo<FlagsEnum::BindsToWeak> {
            constexpr static const auto Kind = FlagsEnum::BindsToWeak;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_BINDS_TO_WEAK");
            constexpr static const auto Description =
                std::string_view("Binds to Weak Symbols");
        };

        template <>
        struct FlagInfo<FlagsEnum::AllowStackExecution> {
            constexpr static const auto Kind = FlagsEnum::AllowStackExecution;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_ALLOW_STACK_EXECUTION");
            constexpr static const auto Description =
                std::string_view("Stacks have Execution Priviledge");
        };

        template <>
        struct FlagInfo<FlagsEnum::RootSafe> {
            constexpr static const auto Kind = FlagsEnum::RootSafe;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_ROOT_SAFE");
            constexpr static const auto Description =
                std::string_view("Safe for root (uid 0) priviledges");
        };

        template <>
        struct FlagInfo<FlagsEnum::SetuidSafe> {
            constexpr static const auto Kind = FlagsEnum::SetuidSafe;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_SETUID_SAFE");
            constexpr static const auto Description =
                std::string_view("Safe for issetuid()");
        };

        template <>
        struct FlagInfo<FlagsEnum::PositionIndependentExecutable> {
            constexpr static const auto Kind =
                FlagsEnum::PositionIndependentExecutable;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name = std::string_view("MH_PIE");
            constexpr static const auto Description =
                std::string_view("Position-Independant Executable");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoReexportedDylibs> {
            constexpr static const auto Kind =
                FlagsEnum::NoReexportedDylibs;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_NO_REEXPORTED_DYLIBS");
            constexpr static const auto Description =
                std::string_view("No Re-Exported Dylibs");
        };

        template <>
        struct FlagInfo<FlagsEnum::DeadStrippableDylib> {
            constexpr static const auto Kind = FlagsEnum::DeadStrippableDylib;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_DEAD_STRIPPABLE_DYLIB");
            constexpr static const auto Description =
                std::string_view("Dead Strippable Dylib");
        };

        template <>
        struct FlagInfo<FlagsEnum::HasTlvDescriptors> {
            constexpr static const auto Kind = FlagsEnum::HasTlvDescriptors;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_HAS_TLV_DESCRIPTORS");
            constexpr static const auto Description =
                std::string_view("Has thread-local variables section");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoHeapExecution> {
            constexpr static const auto Kind = FlagsEnum::NoHeapExecution;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_NO_HEAP_EXECUTION");
            constexpr static const auto Description =
                std::string_view("No Heap Execution");
        };

        template <>
        struct FlagInfo<FlagsEnum::AppExtensionSafe> {
            constexpr static const auto Kind = FlagsEnum::AppExtensionSafe;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_APP_EXTENSION_SAFE");
            constexpr static const auto Description =
                std::string_view("App Extension Safe");
        };

        template <>
        struct FlagInfo<FlagsEnum::NlistOutOfSyncWithDyldInfo> {
            constexpr static const auto Kind =
                FlagsEnum::NlistOutOfSyncWithDyldInfo;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_NLIST_OUTOFSYNC_WITH_DYLDINFO");
            constexpr static const auto Description =
                std::string_view("Nlist Out-Of-Sync with DyldInfo");
        };

        template <>
        struct FlagInfo<FlagsEnum::SimulatorSupport> {
            constexpr static const auto Kind = FlagsEnum::SimulatorSupport;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_SIM_SUPPORT");
            constexpr static const auto Description =
                std::string_view("Has Simulator Support");
        };

        template <>
        struct FlagInfo<FlagsEnum::DylibInCache> {
            constexpr static const auto Kind = FlagsEnum::DylibInCache;
            constexpr static const auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static const auto Name =
                std::string_view("MH_DYLIB_IN_CACHE");
            constexpr static const auto Description =
                std::string_view("Dylib in Shared-Cache");
        };

        [[nodiscard]] constexpr static
        const std::string_view &FlagsEnumGetName(FlagsEnum Flag) noexcept {
            switch (Flag) {
                case FlagsEnum::NoUndefinedReferences:
                    return FlagInfo<FlagsEnum::NoUndefinedReferences>::Name;
                case FlagsEnum::IncrementalLink:
                    return FlagInfo<FlagsEnum::IncrementalLink>::Name;
                case FlagsEnum::DyldLink:
                    return FlagInfo<FlagsEnum::DyldLink>::Name;
                case FlagsEnum::BindAtLoad:
                    return FlagInfo<FlagsEnum::BindAtLoad>::Name;
                case FlagsEnum::PreBound:
                    return FlagInfo<FlagsEnum::PreBound>::Name;
                case FlagsEnum::SplitSegments:
                    return FlagInfo<FlagsEnum::SplitSegments>::Name;
                case FlagsEnum::LazyInitialization:
                    return FlagInfo<FlagsEnum::LazyInitialization>::Name;
                case FlagsEnum::TwoLevelNamespaceBindings:
                    return FlagInfo<FlagsEnum::TwoLevelNamespaceBindings>::Name;
                case FlagsEnum::ForceFlatNamespaces:
                    return FlagInfo<FlagsEnum::ForceFlatNamespaces>::Name;
                case FlagsEnum::NoMultipleDefinitions:
                    return FlagInfo<FlagsEnum::NoMultipleDefinitions>::Name;
                case FlagsEnum::NoFixPrebinding:
                    return FlagInfo<FlagsEnum::NoFixPrebinding>::Name;
                case FlagsEnum::Prebindable:
                    return FlagInfo<FlagsEnum::Prebindable>::Name;
                case FlagsEnum::AllModulesBound:
                    return FlagInfo<FlagsEnum::AllModulesBound>::Name;
                case FlagsEnum::SubsectionsViaSymbols:
                    return FlagInfo<FlagsEnum::SubsectionsViaSymbols>::Name;
                case FlagsEnum::Canonical:
                    return FlagInfo<FlagsEnum::Canonical>::Name;
                case FlagsEnum::WeakDefinitions:
                    return FlagInfo<FlagsEnum::WeakDefinitions>::Name;
                case FlagsEnum::BindsToWeak:
                    return FlagInfo<FlagsEnum::BindsToWeak>::Name;
                case FlagsEnum::AllowStackExecution:
                    return FlagInfo<FlagsEnum::AllowStackExecution>::Name;
                case FlagsEnum::RootSafe:
                    return FlagInfo<FlagsEnum::RootSafe>::Name;
                case FlagsEnum::SetuidSafe:
                    return FlagInfo<FlagsEnum::SetuidSafe>::Name;
                case FlagsEnum::NoReexportedDylibs:
                    return FlagInfo<FlagsEnum::NoReexportedDylibs>::Name;
                case FlagsEnum::PositionIndependentExecutable:
                    return
                        FlagInfo<FlagsEnum::PositionIndependentExecutable>
                            ::Name;
                case FlagsEnum::DeadStrippableDylib:
                    return FlagInfo<FlagsEnum::DeadStrippableDylib>::Name;
                case FlagsEnum::HasTlvDescriptors:
                    return FlagInfo<FlagsEnum::HasTlvDescriptors>::Name;
                case FlagsEnum::NoHeapExecution:
                    return FlagInfo<FlagsEnum::NoHeapExecution>::Name;
                case FlagsEnum::AppExtensionSafe:
                    return FlagInfo<FlagsEnum::AppExtensionSafe>::Name;
                case FlagsEnum::NlistOutOfSyncWithDyldInfo:
                    return
                        FlagInfo<FlagsEnum::NlistOutOfSyncWithDyldInfo>::Name;
                case FlagsEnum::SimulatorSupport:
                    return FlagInfo<FlagsEnum::SimulatorSupport>::Name;
                case FlagsEnum::DylibInCache:
                    return FlagInfo<FlagsEnum::DylibInCache>::Name;
            }

            return EmptyStringValue;
        }

        [[nodiscard]] constexpr static const std::string_view &
        FlagsEnumGetDescription(FlagsEnum Flag) noexcept {
            switch (Flag) {
                case FlagsEnum::NoUndefinedReferences:
                    return
                        FlagInfo<FlagsEnum::NoUndefinedReferences>::Description;
                case FlagsEnum::IncrementalLink:
                    return FlagInfo<FlagsEnum::IncrementalLink>::Description;
                case FlagsEnum::DyldLink:
                    return FlagInfo<FlagsEnum::DyldLink>::Description;
                case FlagsEnum::BindAtLoad:
                    return FlagInfo<FlagsEnum::BindAtLoad>::Description;
                case FlagsEnum::PreBound:
                    return FlagInfo<FlagsEnum::PreBound>::Description;
                case FlagsEnum::SplitSegments:
                    return FlagInfo<FlagsEnum::SplitSegments>::Description;
                case FlagsEnum::LazyInitialization:
                    return FlagInfo<FlagsEnum::LazyInitialization>::Description;
                case FlagsEnum::TwoLevelNamespaceBindings:
                    return
                        FlagInfo<FlagsEnum::TwoLevelNamespaceBindings>
                            ::Description;
                case FlagsEnum::ForceFlatNamespaces:
                    return
                        FlagInfo<FlagsEnum::ForceFlatNamespaces>::Description;
                case FlagsEnum::NoMultipleDefinitions:
                    return
                        FlagInfo<FlagsEnum::NoMultipleDefinitions>::Description;
                case FlagsEnum::NoFixPrebinding:
                    return FlagInfo<FlagsEnum::NoFixPrebinding>::Description;
                case FlagsEnum::Prebindable:
                    return FlagInfo<FlagsEnum::Prebindable>::Description;
                case FlagsEnum::AllModulesBound:
                    return FlagInfo<FlagsEnum::AllModulesBound>::Description;
                case FlagsEnum::SubsectionsViaSymbols:
                    return
                        FlagInfo<FlagsEnum::SubsectionsViaSymbols>::Description;
                case FlagsEnum::Canonical:
                    return FlagInfo<FlagsEnum::Canonical>::Description;
                case FlagsEnum::WeakDefinitions:
                    return FlagInfo<FlagsEnum::WeakDefinitions>::Description;
                case FlagsEnum::BindsToWeak:
                    return FlagInfo<FlagsEnum::BindsToWeak>::Description;
                case FlagsEnum::AllowStackExecution:
                    return FlagInfo<FlagsEnum::AllowStackExecution>::Description;
                case FlagsEnum::RootSafe:
                    return FlagInfo<FlagsEnum::RootSafe>::Description;
                case FlagsEnum::SetuidSafe:
                    return FlagInfo<FlagsEnum::SetuidSafe>::Description;
                case FlagsEnum::NoReexportedDylibs:
                    return FlagInfo<FlagsEnum::NoReexportedDylibs>::Description;
                case FlagsEnum::PositionIndependentExecutable:
                    return
                        FlagInfo<FlagsEnum::PositionIndependentExecutable>
                            ::Name;
                case FlagsEnum::DeadStrippableDylib:
                    return FlagInfo<FlagsEnum::DeadStrippableDylib>::Description;
                case FlagsEnum::HasTlvDescriptors:
                    return FlagInfo<FlagsEnum::HasTlvDescriptors>::Description;
                case FlagsEnum::NoHeapExecution:
                    return FlagInfo<FlagsEnum::NoHeapExecution>::Description;
                case FlagsEnum::AppExtensionSafe:
                    return FlagInfo<FlagsEnum::AppExtensionSafe>::Description;
                case FlagsEnum::NlistOutOfSyncWithDyldInfo:
                    return
                        FlagInfo<FlagsEnum::NlistOutOfSyncWithDyldInfo>
                            ::Description;
                case FlagsEnum::SimulatorSupport:
                    return FlagInfo<FlagsEnum::SimulatorSupport>::Description;
                case FlagsEnum::DylibInCache:
                    return FlagInfo<FlagsEnum::DylibInCache>::Description;
            }

            return EmptyStringValue;
        }

        using FlagsType = BasicMasksHandler<FlagsEnum>;

        Magic Magic;
        int32_t CpuKind;
        int32_t CpuSubKind;
        uint32_t FileKind;
        uint32_t Ncmds;
        uint32_t SizeOfCmds;
        uint32_t Flags;

        [[nodiscard]] inline bool hasValidMagic() const noexcept {
            return MagicIsValid(Magic);
        }

        [[nodiscard]] inline bool hasValidFileKind() const noexcept {
            const auto Value = SwitchEndianIf(FileKind, this->IsBigEndian());
            return FileKindIsValid(static_cast<enum FileKind>(Value));
        }

        [[nodiscard]] inline bool IsBigEndian() const noexcept {
            switch (this->Magic) {
                case Magic::BigEndian:
                case Magic::BigEndian64:
                    return true;
                case Magic::Default:
                case Magic::Default64:
                    return false;
            }

            return false;
        }

        [[nodiscard]] inline bool Is64Bit() const noexcept {
            switch (this->Magic) {
                case Magic::Default64:
                case Magic::BigEndian64:
                    return true;
                case Magic::Default:
                case Magic::BigEndian:
                    return false;
            }

            return false;
        }

        [[nodiscard]] Mach::CpuKind getCpuKind() const noexcept {
            return Mach::CpuKind(SwitchEndianIf(CpuKind, this->IsBigEndian()));
        }

        [[nodiscard]] enum FileKind getFileKind() const noexcept {
            const auto Value = SwitchEndianIf(FileKind, this->IsBigEndian());
            return static_cast<enum FileKind>(Value);
        }

        [[nodiscard]] FlagsType getFlags() const noexcept {
            const auto Flags = SwitchEndianIf(this->Flags, this->IsBigEndian());
            return Flags;
        }

        [[nodiscard]] uint64_t size() const noexcept {
            if (this->Is64Bit()) {
                return (sizeof(*this) + sizeof(uint32_t));
            }

            return sizeof(*this);
        }

        [[nodiscard]] uint8_t *getLoadCmdBuffer() noexcept {
            const auto LoadCmdBuffer =
                reinterpret_cast<uint8_t *>(this) + size();

            return LoadCmdBuffer;
        }

        [[nodiscard]]
        inline const uint8_t *getConstLoadCmdBuffer() const noexcept {
            const auto LoadCmdBuffer =
                reinterpret_cast<const uint8_t *>(this) + size();

            return LoadCmdBuffer;
        }
    };

    struct FatHeader {
        enum class Magic : uint32_t {
            Default = 0xcafebabe,
            Default64 = 0xcafebabf,

            BigEndian = 0xbebafeca,
            BigEndian64 = 0xbfbafeca
        };

        template <Magic>
        struct MagicInfo {};

        template <>
        struct MagicInfo<Magic::Default> {
            constexpr static const auto Kind = Magic::Default;

            constexpr static const auto Name = std::string_view("FAT_MAGIC");
            constexpr static const auto Description =
                std::string_view("Default");
        };

        template <>
        struct MagicInfo<Magic::Default64> {
            constexpr static const auto Kind = Magic::Default64;

            constexpr static const auto Name = std::string_view("FAT_MAGIC_64");
            constexpr static const auto Description =
                std::string_view("Default (64-Bit)");
        };

        template <>
        struct MagicInfo<Magic::BigEndian> {
            constexpr static const auto Kind = Magic::BigEndian;

            constexpr static const auto Name = std::string_view("FAT_CIGAM");
            constexpr static const auto Description =
                std::string_view("Big Endian");
        };

        template <>
        struct MagicInfo<Magic::BigEndian64> {
            constexpr static const auto Kind = Magic::BigEndian64;

            constexpr static const auto Name = std::string_view("FAT_CIGAM_64");
            constexpr static const auto Description =
                std::string_view("Big Endian (64-Bit)");
        };

        [[nodiscard]] constexpr static bool MagicIsValid(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                case Magic::Default64:
                case Magic::BigEndian:
                case Magic::BigEndian64:
                    return true;
            }

            return false;
        }

        [[nodiscard]] constexpr
        static const std::string_view &MagicGetName(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                    return MagicInfo<Magic::Default>::Name;
                case Magic::Default64:
                    return MagicInfo<Magic::Default64>::Name;
                case Magic::BigEndian:
                    return MagicInfo<Magic::BigEndian>::Name;
                case Magic::BigEndian64:
                    return MagicInfo<Magic::BigEndian64>::Name;
            }

            return EmptyStringValue;
        }

        [[nodiscard]] constexpr static
        const std::string_view &MagicGetDescription(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                    return MagicInfo<Magic::Default>::Description;
                case Magic::Default64:
                    return MagicInfo<Magic::Default64>::Description;
                case Magic::BigEndian:
                    return MagicInfo<Magic::BigEndian>::Description;
                case Magic::BigEndian64:
                    return MagicInfo<Magic::BigEndian64>::Description;
            }

            return EmptyStringValue;
        }

        struct Arch32 {
            int32_t CpuKind;
            int32_t CpuSubKind;
            uint32_t Offset;
            uint32_t Size;
            uint32_t Align;

            [[nodiscard]]
            inline Mach::CpuKind getCpuKind(bool IsBigEndian) const noexcept {
                return Mach::CpuKind(SwitchEndianIf(CpuKind, IsBigEndian));
            }
        };

        struct Arch64 {
            int32_t CpuKind;
            int32_t CpuSubKind;
            uint64_t Offset;
            uint64_t Size;
            uint32_t Align;
            uint32_t Reserved;

            [[nodiscard]]
            inline Mach::CpuKind getCpuKind(bool IsBigEndian) const noexcept {
                return Mach::CpuKind(SwitchEndianIf(CpuKind, IsBigEndian));
            }
        };

        Magic Magic;
        uint32_t NFatArch;

        using Arch32List = BasicContiguousList<Arch32>;
        using Arch64List = BasicContiguousList<Arch64>;

        using ConstArch32List = BasicContiguousList<const Arch32>;
        using ConstArch64List = BasicContiguousList<const Arch64>;

        [[nodiscard]] Arch32List getArch32List() noexcept;
        [[nodiscard]] Arch64List getArch64List() noexcept;

        [[nodiscard]] ConstArch32List getConstArch32List() const noexcept;
        [[nodiscard]] ConstArch64List getConstArch64List() const noexcept;

        [[nodiscard]] inline bool hasValidMagic() const noexcept {
            return MagicIsValid(Magic);
        }

        [[nodiscard]] inline bool IsBigEndian() const noexcept {
            switch (this->Magic) {
                case Magic::BigEndian:
                case Magic::BigEndian64:
                    return true;
                case Magic::Default:
                case Magic::Default64:
                    return false;
            }

            return false;
        }

        [[nodiscard]] inline bool Is64Bit() const noexcept {
            switch (this->Magic) {
                case Magic::Default64:
                case Magic::BigEndian64:
                    return true;
                case Magic::Default:
                case Magic::BigEndian:
                    return false;
            }

            return false;
        }

        [[nodiscard]] inline uint32_t getArchCount() const noexcept {
            return SwitchEndianIf(NFatArch, this->IsBigEndian());
        }
    };
}
