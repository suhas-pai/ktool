//
//  include/ADT/Mach-O/Headers.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string>

#include "ADT/BasicContiguousList.h"
#include "ADT/BasicMasksHandler.h"
#include "ADT/LocationRange.h"
#include "ADT/Mach.h"

#include "Utils/Casting.h"
#include "Utils/SwitchEndian.h"

#include "LoadCommandStorage.h"

namespace MachO {
    using namespace std::literals;
    constexpr inline auto EmptyStringValue = std::string_view();

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
            constexpr static auto Kind = Magic::Default;

            constexpr static auto Name = std::string_view("MH_MAGIC");
            constexpr static auto Description =
                std::string_view("Default");
        };

        template <>
        struct MagicInfo<Magic::Default64> {
            constexpr static auto Kind = Magic::Default64;

            constexpr static auto Name = std::string_view("MH_MAGIC_64");
            constexpr static auto Description =
                std::string_view("Default (64-Bit)");
        };

        template <>
        struct MagicInfo<Magic::BigEndian> {
            constexpr static auto Kind = Magic::BigEndian;

            constexpr static auto Name = std::string_view("MH_CIGAM");
            constexpr static auto Description =
                std::string_view("Big Endian");
        };

        template <>
        struct MagicInfo<Magic::BigEndian64> {
            constexpr static auto Kind = Magic::BigEndian64;

            constexpr static auto Name = std::string_view("MH_CIGAM_64");
            constexpr static auto Description =
                std::string_view("Big Endian (64-Bit)");
        };

        [[nodiscard]]
        constexpr static inline bool MagicIsValid(Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                case Magic::Default64:
                case Magic::BigEndian:
                case Magic::BigEndian64:
                    return true;
            }

            return false;
        }

        [[nodiscard]] constexpr static
        inline const std::string_view &MagicGetName(Magic Magic) noexcept {
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

        [[nodiscard]] constexpr static inline
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
            constexpr static auto Kind = FileKind::Object;

            constexpr static auto Name = std::string_view("MH_OBJECT");
            constexpr static auto Description =
                std::string_view("Object File");
        };

        template <>
        struct FileKindInfo<FileKind::Executable> {
            constexpr static auto Kind = FileKind::Executable;

            constexpr static auto Name = std::string_view("MH_EXECUTE");
            constexpr static auto Description =
                std::string_view("Executable");
        };

        template <>
        struct FileKindInfo<FileKind::FixedVMSharedLibrary> {
            constexpr static auto Kind = FileKind::FixedVMSharedLibrary;

            constexpr static auto Name = std::string_view("MH_FVMLIB");
            constexpr static auto Description =
                std::string_view("Fixed-VM Shared Library");
        };

        template <>
        struct FileKindInfo<FileKind::CoreFile> {
            constexpr static auto Kind = FileKind::CoreFile;

            constexpr static auto Name = std::string_view("MH_CORE");
            constexpr static auto Description =
                std::string_view("Core File");
        };

        template <>
        struct FileKindInfo<FileKind::PreloadedExecutable> {
            constexpr static auto Kind = FileKind::PreloadedExecutable;

            constexpr static auto Name = std::string_view("MH_PRELOAD");
            constexpr static auto Description =
                std::string_view("Preloaded Executable");
        };

        template <>
        struct FileKindInfo<FileKind::Dylib> {
            constexpr static auto Kind = FileKind::Dylib;

            constexpr static auto Name = std::string_view("MH_DYLIB");
            constexpr static auto Description =
                std::string_view("Dynamic Library (Dylib)");
        };

        template <>
        struct FileKindInfo<FileKind::Dylinker> {
            constexpr static auto Kind = FileKind::Dylinker;

            constexpr static auto Name = std::string_view("MH_DYLINKER");
            constexpr static auto Description =
                std::string_view("Dynamic Linker (Dylinker)");
        };

        template <>
        struct FileKindInfo<FileKind::Bundle> {
            constexpr static auto Kind = FileKind::Bundle;

            constexpr static auto Name = std::string_view("MH_BUNDLE");
            constexpr static auto Description =
                std::string_view("Bundle");
        };

        template <>
        struct FileKindInfo<FileKind::DylibStub> {
            constexpr static auto Kind = FileKind::DylibStub;

            constexpr static auto Name =
                std::string_view("MH_DYLIB_STUB");
            constexpr static auto Description =
                std::string_view("Dylib Stub");
        };

        template <>
        struct FileKindInfo<FileKind::Dsym> {
            constexpr static auto Kind = FileKind::Dsym;

            constexpr static auto Name = std::string_view("MH_DSYM");
            constexpr static auto Description =
                std::string_view("Debug Symbols File (.dSYM)");
        };

        template <>
        struct FileKindInfo<FileKind::KextBundle> {
            constexpr static auto Kind = FileKind::KextBundle;

            constexpr static auto Name =
                std::string_view("MH_KEXT_BUNDLE");
            constexpr static auto Description =
                std::string_view("Kernel Extension Bundle");
        };

        [[nodiscard]] constexpr
        static inline bool FileKindIsValid(FileKind FileKind) noexcept {
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

        [[nodiscard]] constexpr static std::string_view
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
            constexpr static auto Kind = FlagsEnum::NoUndefinedReferences;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_NOUNDEF");
            constexpr static auto Description =
                std::string_view("No Undefined References");
        };

        template <>
        struct FlagInfo<FlagsEnum::IncrementalLink> {
            constexpr static auto Kind = FlagsEnum::IncrementalLink;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_INCRLINK");
            constexpr static auto Description =
                std::string_view("Incrementally Linked");
        };

        template <>
        struct FlagInfo<FlagsEnum::DyldLink> {
            constexpr static auto Kind = FlagsEnum::DyldLink;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_DYLDLINK");
            constexpr static auto Description =
                std::string_view("Dyanmic Linker Input");
        };

        template <>
        struct FlagInfo<FlagsEnum::BindAtLoad> {
            constexpr static auto Kind = FlagsEnum::BindAtLoad;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_BINDATLOAD");
            constexpr static auto Description =
                std::string_view("No Undefined References");
        };

        template <>
        struct FlagInfo<FlagsEnum::SplitSegments> {
            constexpr static auto Kind = FlagsEnum::SplitSegments;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_SPLIT_SEGS");
            constexpr static auto Description =
                std::string_view("Bind Undefined References at Launch");
        };

        template <>
        struct FlagInfo<FlagsEnum::PreBound> {
            constexpr static auto Kind = FlagsEnum::PreBound;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_PREBOUND");
            constexpr static auto Description =
                std::string_view("Dynamic Undefined Reference PreBound");
        };

        template <>
        struct FlagInfo<FlagsEnum::LazyInitialization> {
            constexpr static auto Kind = FlagsEnum::LazyInitialization;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_LAZY_INIT");
            constexpr static auto Description =
                std::string_view("Lazy Initialization");
        };

        template <>
        struct FlagInfo<FlagsEnum::TwoLevelNamespaceBindings> {
            constexpr static auto Kind =
                FlagsEnum::TwoLevelNamespaceBindings;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_TWOLEVEL");
            constexpr static auto Description =
                std::string_view("Has Two-Level Namespace Bindings");
        };

        template <>
        struct FlagInfo<FlagsEnum::ForceFlatNamespaces> {
            constexpr static auto Kind = FlagsEnum::ForceFlatNamespaces;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_FORCE_FLAT");
            constexpr static auto Description =
                std::string_view("Has Two-Level Namespace Bindings");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoMultipleDefinitions> {
            constexpr static auto Kind = FlagsEnum::NoMultipleDefinitions;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_NOMULTIDEFS");
            constexpr static auto Description =
                std::string_view("No Multiple Symbol Definitions");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoFixPrebinding> {
            constexpr static auto Kind = FlagsEnum::NoFixPrebinding;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_NOFIXPREBINDING");
            constexpr static auto Description =
                std::string_view("Don't Fix PreBinding");
        };

        template <>
        struct FlagInfo<FlagsEnum::Prebindable> {
            constexpr static auto Kind = FlagsEnum::Prebindable;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_PREBINDABLE");
            constexpr static auto Description =
                std::string_view("PreBinded - But can be Prebinded Again");
        };

        template <>
        struct FlagInfo<FlagsEnum::AllModulesBound> {
            constexpr static auto Kind = FlagsEnum::AllModulesBound;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_ALLMODSBOUND");
            constexpr static auto Description =
                std::string_view("Binds to all Two-Level Namespace Modules");
        };

        template <>
        struct FlagInfo<FlagsEnum::SubsectionsViaSymbols> {
            constexpr static auto Kind = FlagsEnum::SubsectionsViaSymbols;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_SUBSECTIONS_VIA_SYMBOLS");
            constexpr static auto Description =
                std::string_view("Safe to Divide Sections into Sub-Sections");
        };

        template <>
        struct FlagInfo<FlagsEnum::Canonical> {
            constexpr static auto Kind = FlagsEnum::Canonical;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_CANONICAL");
            constexpr static auto Description =
                std::string_view("Canocalized via UnPreBind");
        };

        template <>
        struct FlagInfo<FlagsEnum::WeakDefinitions> {
            constexpr static auto Kind = FlagsEnum::WeakDefinitions;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_WEAK_DEFINES");
            constexpr static auto Description =
                std::string_view("Has External Weak Symbols");
        };

        template <>
        struct FlagInfo<FlagsEnum::BindsToWeak> {
            constexpr static auto Kind = FlagsEnum::BindsToWeak;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_BINDS_TO_WEAK");
            constexpr static auto Description =
                std::string_view("Binds to Weak Symbols");
        };

        template <>
        struct FlagInfo<FlagsEnum::AllowStackExecution> {
            constexpr static auto Kind = FlagsEnum::AllowStackExecution;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_ALLOW_STACK_EXECUTION");
            constexpr static auto Description =
                std::string_view("Stacks have Execution Priviledge");
        };

        template <>
        struct FlagInfo<FlagsEnum::RootSafe> {
            constexpr static auto Kind = FlagsEnum::RootSafe;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_ROOT_SAFE");
            constexpr static auto Description =
                std::string_view("Safe for root (uid 0) priviledges");
        };

        template <>
        struct FlagInfo<FlagsEnum::SetuidSafe> {
            constexpr static auto Kind = FlagsEnum::SetuidSafe;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_SETUID_SAFE");
            constexpr static auto Description =
                std::string_view("Safe for issetuid()");
        };

        template <>
        struct FlagInfo<FlagsEnum::PositionIndependentExecutable> {
            constexpr static auto Kind =
                FlagsEnum::PositionIndependentExecutable;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name = std::string_view("MH_PIE");
            constexpr static auto Description =
                std::string_view("Position-Independant Executable");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoReexportedDylibs> {
            constexpr static auto Kind =
                FlagsEnum::NoReexportedDylibs;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_NO_REEXPORTED_DYLIBS");
            constexpr static auto Description =
                std::string_view("No Re-Exported Dylibs");
        };

        template <>
        struct FlagInfo<FlagsEnum::DeadStrippableDylib> {
            constexpr static auto Kind = FlagsEnum::DeadStrippableDylib;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_DEAD_STRIPPABLE_DYLIB");
            constexpr static auto Description =
                std::string_view("Dead Strippable Dylib");
        };

        template <>
        struct FlagInfo<FlagsEnum::HasTlvDescriptors> {
            constexpr static auto Kind = FlagsEnum::HasTlvDescriptors;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_HAS_TLV_DESCRIPTORS");
            constexpr static auto Description =
                std::string_view("Has thread-local variables section");
        };

        template <>
        struct FlagInfo<FlagsEnum::NoHeapExecution> {
            constexpr static auto Kind = FlagsEnum::NoHeapExecution;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_NO_HEAP_EXECUTION");
            constexpr static auto Description =
                std::string_view("No Heap Execution");
        };

        template <>
        struct FlagInfo<FlagsEnum::AppExtensionSafe> {
            constexpr static auto Kind = FlagsEnum::AppExtensionSafe;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_APP_EXTENSION_SAFE");
            constexpr static auto Description =
                std::string_view("App Extension Safe");
        };

        template <>
        struct FlagInfo<FlagsEnum::NlistOutOfSyncWithDyldInfo> {
            constexpr static auto Kind =
                FlagsEnum::NlistOutOfSyncWithDyldInfo;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_NLIST_OUTOFSYNC_WITH_DYLDINFO");
            constexpr static auto Description =
                std::string_view("Nlist Out-Of-Sync with DyldInfo");
        };

        template <>
        struct FlagInfo<FlagsEnum::SimulatorSupport> {
            constexpr static auto Kind = FlagsEnum::SimulatorSupport;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_SIM_SUPPORT");
            constexpr static auto Description =
                std::string_view("Has Simulator Support");
        };

        template <>
        struct FlagInfo<FlagsEnum::DylibInCache> {
            constexpr static auto Kind = FlagsEnum::DylibInCache;
            constexpr static auto Mask =
                static_cast<std::underlying_type_t<FlagsEnum>>(Kind);

            constexpr static auto Name =
                std::string_view("MH_DYLIB_IN_CACHE");
            constexpr static auto Description =
                std::string_view("Dylib in Shared-Cache");
        };

        [[nodiscard]] constexpr static
        const std::string_view &FlagsEnumGetName(FlagsEnum Flag) noexcept {
            using Enum = FlagsEnum;
            switch (Flag) {
                case Enum::NoUndefinedReferences:
                    return FlagInfo<Enum::NoUndefinedReferences>::Name;
                case Enum::IncrementalLink:
                    return FlagInfo<Enum::IncrementalLink>::Name;
                case Enum::DyldLink:
                    return FlagInfo<Enum::DyldLink>::Name;
                case Enum::BindAtLoad:
                    return FlagInfo<Enum::BindAtLoad>::Name;
                case Enum::PreBound:
                    return FlagInfo<Enum::PreBound>::Name;
                case Enum::SplitSegments:
                    return FlagInfo<Enum::SplitSegments>::Name;
                case Enum::LazyInitialization:
                    return FlagInfo<Enum::LazyInitialization>::Name;
                case Enum::TwoLevelNamespaceBindings:
                    return FlagInfo<Enum::TwoLevelNamespaceBindings>::Name;
                case Enum::ForceFlatNamespaces:
                    return FlagInfo<Enum::ForceFlatNamespaces>::Name;
                case Enum::NoMultipleDefinitions:
                    return FlagInfo<Enum::NoMultipleDefinitions>::Name;
                case Enum::NoFixPrebinding:
                    return FlagInfo<Enum::NoFixPrebinding>::Name;
                case Enum::Prebindable:
                    return FlagInfo<Enum::Prebindable>::Name;
                case Enum::AllModulesBound:
                    return FlagInfo<Enum::AllModulesBound>::Name;
                case Enum::SubsectionsViaSymbols:
                    return FlagInfo<Enum::SubsectionsViaSymbols>::Name;
                case Enum::Canonical:
                    return FlagInfo<Enum::Canonical>::Name;
                case Enum::WeakDefinitions:
                    return FlagInfo<Enum::WeakDefinitions>::Name;
                case Enum::BindsToWeak:
                    return FlagInfo<Enum::BindsToWeak>::Name;
                case Enum::AllowStackExecution:
                    return FlagInfo<Enum::AllowStackExecution>::Name;
                case Enum::RootSafe:
                    return FlagInfo<Enum::RootSafe>::Name;
                case Enum::SetuidSafe:
                    return FlagInfo<Enum::SetuidSafe>::Name;
                case Enum::NoReexportedDylibs:
                    return FlagInfo<Enum::NoReexportedDylibs>::Name;
                case Enum::PositionIndependentExecutable:
                    return FlagInfo<Enum::PositionIndependentExecutable>::Name;
                case Enum::DeadStrippableDylib:
                    return FlagInfo<Enum::DeadStrippableDylib>::Name;
                case Enum::HasTlvDescriptors:
                    return FlagInfo<Enum::HasTlvDescriptors>::Name;
                case Enum::NoHeapExecution:
                    return FlagInfo<Enum::NoHeapExecution>::Name;
                case Enum::AppExtensionSafe:
                    return FlagInfo<Enum::AppExtensionSafe>::Name;
                case Enum::NlistOutOfSyncWithDyldInfo:
                    return FlagInfo<Enum::NlistOutOfSyncWithDyldInfo>::Name;
                case Enum::SimulatorSupport:
                    return FlagInfo<Enum::SimulatorSupport>::Name;
                case Enum::DylibInCache:
                    return FlagInfo<Enum::DylibInCache>::Name;
            }

            return EmptyStringValue;
        }

        [[nodiscard]] constexpr static std::string_view
        FlagsEnumGetDescription(FlagsEnum Flag) noexcept {
            using Enum = FlagsEnum;
            switch (Flag) {
                case Enum::NoUndefinedReferences:
                    return FlagInfo<Enum::NoUndefinedReferences>::Description;
                case Enum::IncrementalLink:
                    return FlagInfo<Enum::IncrementalLink>::Description;
                case Enum::DyldLink:
                    return FlagInfo<Enum::DyldLink>::Description;
                case Enum::BindAtLoad:
                    return FlagInfo<Enum::BindAtLoad>::Description;
                case Enum::PreBound:
                    return FlagInfo<Enum::PreBound>::Description;
                case Enum::SplitSegments:
                    return FlagInfo<Enum::SplitSegments>::Description;
                case Enum::LazyInitialization:
                    return FlagInfo<Enum::LazyInitialization>::Description;
                case Enum::TwoLevelNamespaceBindings:
                    return
                        FlagInfo<Enum::TwoLevelNamespaceBindings>::Description;
                case Enum::ForceFlatNamespaces:
                    return FlagInfo<Enum::ForceFlatNamespaces>::Description;
                case Enum::NoMultipleDefinitions:
                    return FlagInfo<Enum::NoMultipleDefinitions>::Description;
                case Enum::NoFixPrebinding:
                    return FlagInfo<Enum::NoFixPrebinding>::Description;
                case Enum::Prebindable:
                    return FlagInfo<Enum::Prebindable>::Description;
                case Enum::AllModulesBound:
                    return FlagInfo<Enum::AllModulesBound>::Description;
                case Enum::SubsectionsViaSymbols:
                    return FlagInfo<Enum::SubsectionsViaSymbols>::Description;
                case Enum::Canonical:
                    return FlagInfo<Enum::Canonical>::Description;
                case Enum::WeakDefinitions:
                    return FlagInfo<Enum::WeakDefinitions>::Description;
                case Enum::BindsToWeak:
                    return FlagInfo<Enum::BindsToWeak>::Description;
                case Enum::AllowStackExecution:
                    return FlagInfo<Enum::AllowStackExecution>::Description;
                case Enum::RootSafe:
                    return FlagInfo<Enum::RootSafe>::Description;
                case Enum::SetuidSafe:
                    return FlagInfo<Enum::SetuidSafe>::Description;
                case Enum::NoReexportedDylibs:
                    return FlagInfo<Enum::NoReexportedDylibs>::Description;
                case Enum::PositionIndependentExecutable:
                    return
                        FlagInfo<Enum::PositionIndependentExecutable>
                            ::Description;
                case Enum::DeadStrippableDylib:
                    return FlagInfo<Enum::DeadStrippableDylib>::Description;
                case Enum::HasTlvDescriptors:
                    return FlagInfo<Enum::HasTlvDescriptors>::Description;
                case Enum::NoHeapExecution:
                    return FlagInfo<Enum::NoHeapExecution>::Description;
                case Enum::AppExtensionSafe:
                    return FlagInfo<Enum::AppExtensionSafe>::Description;
                case Enum::NlistOutOfSyncWithDyldInfo:
                    return
                        FlagInfo<Enum::NlistOutOfSyncWithDyldInfo>::Description;
                case Enum::SimulatorSupport:
                    return FlagInfo<Enum::SimulatorSupport>::Description;
                case Enum::DylibInCache:
                    return FlagInfo<Enum::DylibInCache>::Description;
            }

            return EmptyStringValue;
        }

        using FlagsType = BasicFlags<FlagsEnum>;

        Magic Magic;
        int32_t CpuKind;
        int32_t CpuSubKind;
        uint32_t FileKind;
        uint32_t Ncmds;
        uint32_t SizeOfCmds;
        uint32_t Flags;

        [[nodiscard]] constexpr inline bool hasValidMagic() const noexcept {
            return MagicIsValid(Magic);
        }

        [[nodiscard]] constexpr inline bool hasValidFileKind() const noexcept {
            const auto Value = SwitchEndianIf(FileKind, this->isBigEndian());
            return FileKindIsValid(static_cast<enum FileKind>(Value));
        }

        [[nodiscard]] constexpr inline bool isBigEndian() const noexcept {
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

        [[nodiscard]] constexpr inline bool is64Bit() const noexcept {
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

        [[nodiscard]]
        constexpr inline Mach::CpuKind getCpuKind() const noexcept {
            return Mach::CpuKind(SwitchEndianIf(CpuKind, this->isBigEndian()));
        }

        [[nodiscard]]
        constexpr inline int32_t getCpuSubKind() const noexcept {
            return SwitchEndianIf(CpuSubKind, this->isBigEndian());
        }

        [[nodiscard]]
        constexpr inline enum FileKind getFileKind() const noexcept {
            const auto Value = SwitchEndianIf(FileKind, this->isBigEndian());
            return static_cast<enum FileKind>(Value);
        }

        [[nodiscard]] inline uint32_t getLoadCommandsCount() const noexcept {
            return SwitchEndianIf(Ncmds, this->isBigEndian());
        }

        [[nodiscard]] inline uint32_t getLoadCommandsSize() const noexcept {
            return SwitchEndianIf(SizeOfCmds, this->isBigEndian());
        }

        [[nodiscard]] constexpr inline FlagsType getFlags() const noexcept {
            return SwitchEndianIf(Flags, this->isBigEndian());
        }

        constexpr inline Header &setCpuKind(Mach::CpuKind Kind) noexcept {
            const auto Value = static_cast<int32_t>(Kind);
            this->CpuKind = SwitchEndianIf(Value, this->isBigEndian());

            return *this;
        }

        constexpr inline Header &setLoadCommandsCount(uint32_t Value) noexcept {
            this->Ncmds = SwitchEndianIf(Value, this->isBigEndian());
            return *this;
        }

        constexpr inline Header &setLoadCommandsSize(uint32_t Value) noexcept {
            this->SizeOfCmds = SwitchEndianIf(Value, this->isBigEndian());
            return *this;
        }

        constexpr inline Header &setFlags(const FlagsType &Flags) noexcept {
            const auto FlagsValue = Flags.value();
            this->Flags = SwitchEndianIf(FlagsValue, this->isBigEndian());

            return *this;
        }

        [[nodiscard]] constexpr inline uint64_t size() const noexcept {
            if (this->is64Bit()) {
                return (sizeof(*this) + sizeof(uint32_t));
            }

            return sizeof(*this);
        }

        [[nodiscard]] inline uint8_t *getLoadCmdBuffer() noexcept {
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

        [[nodiscard]] inline
        LoadCommandStorage GetLoadCmdStorage(bool Verify = true) noexcept {
            const auto Result =
                LoadCommandStorage::Open(this->getLoadCmdBuffer(),
                                         this->getLoadCommandsCount(),
                                         this->getLoadCommandsSize(),
                                         this->isBigEndian(),
                                         this->is64Bit(),
                                         Verify);

            return Result;
        }

        [[nodiscard]] inline ConstLoadCommandStorage
        GetConstLoadCmdStorage(bool Verify = true) const noexcept {
            const auto Result =
                ConstLoadCommandStorage::Open(this->getConstLoadCmdBuffer(),
                                              this->getLoadCommandsCount(),
                                              this->getLoadCommandsSize(),
                                              this->isBigEndian(),
                                              this->is64Bit(),
                                              Verify);

            return Result;
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
            constexpr static auto Kind = Magic::Default;

            constexpr static auto Name = std::string_view("FAT_MAGIC");
            constexpr static auto Description =
                std::string_view("Default");
        };

        template <>
        struct MagicInfo<Magic::Default64> {
            constexpr static auto Kind = Magic::Default64;

            constexpr static auto Name = std::string_view("FAT_MAGIC_64");
            constexpr static auto Description =
                std::string_view("Default (64-Bit)");
        };

        template <>
        struct MagicInfo<Magic::BigEndian> {
            constexpr static auto Kind = Magic::BigEndian;

            constexpr static auto Name = std::string_view("FAT_CIGAM");
            constexpr static auto Description =
                std::string_view("Big Endian");
        };

        template <>
        struct MagicInfo<Magic::BigEndian64> {
            constexpr static auto Kind = Magic::BigEndian64;

            constexpr static auto Name = std::string_view("FAT_CIGAM_64");
            constexpr static auto Description =
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

            [[nodiscard]] constexpr
            inline Mach::CpuKind getCpuKind(bool IsBigEndian) const noexcept {
                return Mach::CpuKind(SwitchEndianIf(CpuKind, IsBigEndian));
            }

            [[nodiscard]] constexpr
            inline int32_t getCpuSubKind(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CpuSubKind, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getFileOffset(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getFileSize(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getAlign(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]] constexpr inline std::optional<LocationRange>
            getFileRange(bool IsBigEndian) const noexcept {
                const auto Offset = getFileOffset(IsBigEndian);
                const auto Size = getFileSize(IsBigEndian);

                return LocationRange::CreateWithSize(Offset, Size);
            }

            constexpr inline
            Arch32 &setCpuKind(Mach::CpuKind Kind, bool IsBigEndian) noexcept {
                const auto Value = static_cast<int32_t>(Kind);
                this->CpuKind = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr inline Arch32 &
            setCpuSubKind(int32_t Value, bool IsBigEndian) noexcept {
                this->CpuSubKind = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Arch32 &getFileOffset(uint32_t Value, bool IsBigEndian) noexcept {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Arch32 &setFileSize(uint32_t Value, bool IsBigEndian) noexcept {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }
        };

        struct Arch64 {
            int32_t CpuKind;
            int32_t CpuSubKind;
            uint64_t Offset;
            uint64_t Size;
            uint32_t Align;
            uint32_t Reserved;

            [[nodiscard]] constexpr
            inline Mach::CpuKind getCpuKind(bool IsBigEndian) const noexcept {
                return Mach::CpuKind(SwitchEndianIf(CpuKind, IsBigEndian));
            }

            [[nodiscard]] constexpr
            inline int32_t getCpuSubKind(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CpuSubKind, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint64_t getFileOffset(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint64_t getFileSize(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getAlign(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getReserved(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved, IsBigEndian);
            }

            [[nodiscard]] constexpr inline std::optional<LocationRange>
            getFileRange(bool IsBigEndian) const noexcept {
                const auto Offset = getFileOffset(IsBigEndian);
                const auto Size = getFileSize(IsBigEndian);

                return LocationRange::CreateWithSize(Offset, Size);
            }

            constexpr inline
            Arch64 &setCpuKind(Mach::CpuKind Kind, bool IsBigEndian) noexcept {
                const auto Value = static_cast<int32_t>(Kind);
                this->CpuKind = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr inline Arch64 &
            setCpuSubKind(int32_t Value, bool IsBigEndian) noexcept {
                this->CpuSubKind = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Arch64 &getFileOffset(uint32_t Value, bool IsBigEndian) noexcept {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Arch64 &setFileSize(uint32_t Value, bool IsBigEndian) noexcept {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
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

        [[nodiscard]] inline bool isBigEndian() const noexcept {
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

        [[nodiscard]] inline bool is64Bit() const noexcept {
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
            return SwitchEndianIf(NFatArch, this->isBigEndian());
        }

        [[nodiscard]] inline uint32_t hasZeroArchs() const noexcept {
            return (NFatArch == 0);
        }

        inline FatHeader &setArchCount(uint32_t Value) noexcept {
            this->NFatArch = SwitchEndianIf(Value, this->isBigEndian());
            return *this;
        }
    };
}
