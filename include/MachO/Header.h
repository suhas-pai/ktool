//
//  MachO/Header.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once

#include <string_view>

#include "ADT/Range.h"
#include "ADT/SwitchEndian.h"

#include "Mach/Machine.h"
#include "Magic.h"

namespace MachO {
    enum class FileKind : uint32_t {
        Object = 1,
        Executable,
        FixedVMSharedLib,
        Core,
        PreLoadedExecutable,
        DynamicLibrary,
        DynamicLinker,
        Bundle,
        DylibStub,
        Dsym,
        KextBundle,
        FileSet
    };

    constexpr auto FileKindIsValid(const FileKind FileKind) noexcept {
        switch (FileKind) {
            case FileKind::Object:
            case FileKind::Executable:
            case FileKind::FixedVMSharedLib:
            case FileKind::Core:
            case FileKind::PreLoadedExecutable:
            case FileKind::DynamicLibrary:
            case FileKind::DynamicLinker:
            case FileKind::Bundle:
            case FileKind::DylibStub:
            case FileKind::Dsym:
            case FileKind::KextBundle:
            case FileKind::FileSet:
                return true;
        }

        return false;
    }

    constexpr auto FileKindGetString(const FileKind FileKind) noexcept
        -> std::string_view
    {
        switch (FileKind) {
            case FileKind::Object:
                return "MH_OBJECT";
            case FileKind::Executable:
                return "MH_EXECUTE";
            case FileKind::FixedVMSharedLib:
                return "MH_FVMLIB";
            case FileKind::Core:
                return "MH_CORE";
            case FileKind::PreLoadedExecutable:
                return "MH_PRELOAD";
            case FileKind::DynamicLibrary:
                return "MH_DYLIB";
            case FileKind::DynamicLinker:
                return "MH_DYLINKER";
            case FileKind::Bundle:
                return "MH_BUNDLE";
            case FileKind::DylibStub:
                return "MH_DYLIB_STUB";
            case FileKind::Dsym:
                return "MH_DSYM";
            case FileKind::KextBundle:
                return "MH_KEXT";
            case FileKind::FileSet:
                return "MH_FILESET";
        }

        assert(false && "Called FileKindGetString() with unknown FileKind");
    }

    constexpr auto FileKindGetDesc(const FileKind FileKind) noexcept
        -> std::string_view
    {
        switch (FileKind) {
            case FileKind::Object:
                return "Object";
            case FileKind::Executable:
                return "Executable";
            case FileKind::FixedVMSharedLib:
                return "Fixed VM Shared-Library";
            case FileKind::Core:
                return "Core";
            case FileKind::PreLoadedExecutable:
                return "Preloaded Executable";
            case FileKind::DynamicLibrary:
                return "Dylib";
            case FileKind::DynamicLinker:
                return "Dylinker";
            case FileKind::Bundle:
                return "Bundle";
            case FileKind::DylibStub:
                return "Dylib Stub";
            case FileKind::Dsym:
                return "Dsym";
            case FileKind::KextBundle:
                return "Kext Bundle";
            case FileKind::FileSet:
                return "Fileset";
        }

        assert(false && "Called FileKindGetDesc() with unknown FileKind");
    }

    enum class Flags : uint32_t {
        NoUndefineds = 1 << 0,
        IncrementalLink = 1 << 1,
        DynamicLinkerLink = 1 << 2,

        BindAtLoad    = 1 << 3,
        PreBound      = 1 << 4,
        SplitSegments = 1 << 5,
        LazyInit      = 1 << 6,

        TwoLevelNamespaces  = 1 << 7,
        ForceFlatNamespaces = 1 << 8,

        NoMultipleDefinitions = 1 << 9,
        NoFixPrebinding = 1 << 10,

        Prebindable = 1 << 11,
        AllModulesBound = 1 << 12,
        SubsectionsViaSymbols = 1 << 13,

        Canonical = 1 << 14,
        WeakDefines = 1 << 15,

        BindsToWeak = 1 << 16,
        AllowStackExecution = 1 << 17,

        RootSafe = 1 << 18,
        SetuidSafe = 1 << 19,

        NoReexportedDylibs = 1 << 20,
        PositionIndependentExec = 1 << 21,
        DeadStrippableDylib = 1 << 22,
        HasTlvDescriptors = 1 << 23,
        NoHeapExecution = 1 << 24,
        AppExtensionSafe = 1 << 25,

        NlistOutOfSyncDyldInfo = 1 << 26,

        SimSupport = 1 << 27,
        DylibInCache = static_cast<uint32_t>(1 << 31),
    };

    [[nodiscard]] constexpr auto FlagsIsValid(const Flags Flag) noexcept {
        switch (Flag) {
            case Flags::NoUndefineds:
            case Flags::IncrementalLink:
            case Flags::DynamicLinkerLink:
            case Flags::BindAtLoad:
            case Flags::PreBound:
            case Flags::SplitSegments:
            case Flags::LazyInit:
            case Flags::TwoLevelNamespaces:
            case Flags::ForceFlatNamespaces:
            case Flags::NoMultipleDefinitions:
            case Flags::NoFixPrebinding:
            case Flags::Prebindable:
            case Flags::AllModulesBound:
            case Flags::SubsectionsViaSymbols:
            case Flags::Canonical:
            case Flags::WeakDefines:
            case Flags::BindsToWeak:
            case Flags::AllowStackExecution:
            case Flags::RootSafe:
            case Flags::SetuidSafe:
            case Flags::NoReexportedDylibs:
            case Flags::PositionIndependentExec:
            case Flags::DeadStrippableDylib:
            case Flags::HasTlvDescriptors:
            case Flags::NoHeapExecution:
            case Flags::AppExtensionSafe:
            case Flags::NlistOutOfSyncDyldInfo:
            case Flags::SimSupport:
            case Flags::DylibInCache:
                return true;
        }

        return false;

        assert(false && "Called FlagsGetString() with unknown Flag");
    }

    [[nodiscard]]
    constexpr auto FlagsGetString(const Flags Flag) noexcept -> std::string_view
    {
        switch (Flag) {
            case Flags::NoUndefineds:
                return "MH_UNDEF";
            case Flags::IncrementalLink:
                return "MH_INCRLINK";
            case Flags::DynamicLinkerLink:
                return "MH_DYLDLINK";
            case Flags::BindAtLoad:
                return "MH_BINDATLOAD";
            case Flags::PreBound:
                return "MH_PREBOUND";
            case Flags::SplitSegments:
                return "MH_SPLTTSEGS";
            case Flags::LazyInit:
                return "MH_LAZY_INIT";
            case Flags::TwoLevelNamespaces:
                return "MH_TWOLEVEL";
            case Flags::ForceFlatNamespaces:
                return "MH_FORCE_FLAT";
            case Flags::NoMultipleDefinitions:
                return "MH_NOMULTIDEF";
            case Flags::NoFixPrebinding:
                return "MH_NOFIXPREBINDING";
            case Flags::Prebindable:
                return "MH_PREBINDABLE";
            case Flags::AllModulesBound:
                return "MH_ALLMODSBOUND";
            case Flags::SubsectionsViaSymbols:
                return "MH_SUBSECTIONS_VIA_SYMBOL";
            case Flags::Canonical:
                return "MH_CANOICAL";
            case Flags::WeakDefines:
                return "MH_WEAK_DEFS";
            case Flags::BindsToWeak:
                return "MH_BINDS_TO_WEAK";
            case Flags::AllowStackExecution:
                return "MH_ALLOW_STACK_EXECUTION";
            case Flags::RootSafe:
                return "MH_ROOT_SAFE";
            case Flags::SetuidSafe:
                return "MH_SETUID_SAFE";
            case Flags::NoReexportedDylibs:
                return "MH_NO_REEXPORTED_DYLIBS";
            case Flags::PositionIndependentExec:
                return "MH_PIE";
            case Flags::DeadStrippableDylib:
                return "MH_DEAD_STRIPPABLE_DYLIB";
            case Flags::HasTlvDescriptors:
                return "MH_HAS_TLV_DESCRIPTORS";
            case Flags::NoHeapExecution:
                return "MH_NO_HEAP_EXECUTION";
            case Flags::AppExtensionSafe:
                return "MH_APP_EXTENSION_SAFE";
            case Flags::NlistOutOfSyncDyldInfo:
                return "MH_NLIST_OUTOFSYNC_WITH_DYLDINFO";
            case Flags::SimSupport:
                return "MH_SIM_SUPPORT";
            case Flags::DylibInCache:
                return "MH_DYLIB_IN_CACHE";
        }

        assert(false && "Called FlagsGetString() with unknown Flag");
    }

    struct Header {
        MachO::Magic Magic;
        int32_t CpuKind;
        int32_t CpuSubKind;
        uint32_t FileKind;
        uint32_t Ncmds;
        uint32_t SizeOfCmds;
        uint32_t Flags;

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
            return MagicIsBigEndian(Magic);
        }

        [[nodiscard]] constexpr auto is64Bit() const noexcept {
            return MagicIs64Bit(Magic);
        }

        [[nodiscard]] constexpr auto cpuKind() const noexcept {
            return Mach::CpuKind(ADT::SwitchEndianIf(CpuKind, isBigEndian()));
        }

        [[nodiscard]] constexpr auto cpuSubKind() const noexcept {
            return ADT::SwitchEndianIf(CpuSubKind, isBigEndian());
        }

        [[nodiscard]] constexpr auto fileKind() const noexcept {
            const auto Value = ADT::SwitchEndianIf(FileKind, isBigEndian());
            return ::MachO::FileKind(Value);
        }

        [[nodiscard]] constexpr auto ncmds() const noexcept {
            return ADT::SwitchEndianIf(Ncmds, isBigEndian());
        }

        [[nodiscard]] constexpr auto sizeOfCmds() const noexcept {
            return ADT::SwitchEndianIf(SizeOfCmds, isBigEndian());
        }

        [[nodiscard]] constexpr auto flags() const noexcept {
            return ::MachO::Flags(ADT::SwitchEndianIf(Flags, isBigEndian()));
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return sizeof(*this) + (is64Bit() ? sizeof(uint32_t) : 0);
        }

        [[nodiscard]] constexpr auto loadCommandsRange() const noexcept {
            return ADT::Range::FromSize(size(), sizeOfCmds());
        }
    };
}
