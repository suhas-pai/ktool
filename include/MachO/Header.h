//
//  MachO/Header.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once

#include <string_view>

#include "ADT/FlagsBase.h"
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

    struct Flags : ADT::FlagsBase<uint32_t> {
    public:
        enum class Kind : uint32_t {
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

        [[nodiscard]]
        constexpr static auto KindIsValid(const Kind Kind) noexcept {
            switch (Kind) {
                case Kind::NoUndefineds:
                case Kind::IncrementalLink:
                case Kind::DynamicLinkerLink:
                case Kind::BindAtLoad:
                case Kind::PreBound:
                case Kind::SplitSegments:
                case Kind::LazyInit:
                case Kind::TwoLevelNamespaces:
                case Kind::ForceFlatNamespaces:
                case Kind::NoMultipleDefinitions:
                case Kind::NoFixPrebinding:
                case Kind::Prebindable:
                case Kind::AllModulesBound:
                case Kind::SubsectionsViaSymbols:
                case Kind::Canonical:
                case Kind::WeakDefines:
                case Kind::BindsToWeak:
                case Kind::AllowStackExecution:
                case Kind::RootSafe:
                case Kind::SetuidSafe:
                case Kind::NoReexportedDylibs:
                case Kind::PositionIndependentExec:
                case Kind::DeadStrippableDylib:
                case Kind::HasTlvDescriptors:
                case Kind::NoHeapExecution:
                case Kind::AppExtensionSafe:
                case Kind::NlistOutOfSyncDyldInfo:
                case Kind::SimSupport:
                case Kind::DylibInCache:
                    return true;
            }

            return false;
        }

        [[nodiscard]] constexpr static
        auto KindGetString(const Kind Flag) noexcept -> std::string_view {
            switch (Flag) {
                case Kind::NoUndefineds:
                    return "MH_UNDEF";
                case Kind::IncrementalLink:
                    return "MH_INCRLINK";
                case Kind::DynamicLinkerLink:
                    return "MH_DYLDLINK";
                case Kind::BindAtLoad:
                    return "MH_BINDATLOAD";
                case Kind::PreBound:
                    return "MH_PREBOUND";
                case Kind::SplitSegments:
                    return "MH_SPLITSEGS";
                case Kind::LazyInit:
                    return "MH_LAZY_INIT";
                case Kind::TwoLevelNamespaces:
                    return "MH_TWOLEVEL";
                case Kind::ForceFlatNamespaces:
                    return "MH_FORCE_FLAT";
                case Kind::NoMultipleDefinitions:
                    return "MH_NOMULTIDEF";
                case Kind::NoFixPrebinding:
                    return "MH_NOFIXPREBINDING";
                case Kind::Prebindable:
                    return "MH_PREBINDABLE";
                case Kind::AllModulesBound:
                    return "MH_ALLMODSBOUND";
                case Kind::SubsectionsViaSymbols:
                    return "MH_SUBSECTIONS_VIA_SYMBOL";
                case Kind::Canonical:
                    return "MH_CANONICAL";
                case Kind::WeakDefines:
                    return "MH_WEAK_DEFS";
                case Kind::BindsToWeak:
                    return "MH_BINDS_TO_WEAK";
                case Kind::AllowStackExecution:
                    return "MH_ALLOW_STACK_EXECUTION";
                case Kind::RootSafe:
                    return "MH_ROOT_SAFE";
                case Kind::SetuidSafe:
                    return "MH_SETUID_SAFE";
                case Kind::NoReexportedDylibs:
                    return "MH_NO_REEXPORTED_DYLIBS";
                case Kind::PositionIndependentExec:
                    return "MH_PIE";
                case Kind::DeadStrippableDylib:
                    return "MH_DEAD_STRIPPABLE_DYLIB";
                case Kind::HasTlvDescriptors:
                    return "MH_HAS_TLV_DESCRIPTORS";
                case Kind::NoHeapExecution:
                    return "MH_NO_HEAP_EXECUTION";
                case Kind::AppExtensionSafe:
                    return "MH_APP_EXTENSION_SAFE";
                case Kind::NlistOutOfSyncDyldInfo:
                    return "MH_NLIST_OUTOFSYNC_WITH_DYLDINFO";
                case Kind::SimSupport:
                    return "MH_SIM_SUPPORT";
                case Kind::DylibInCache:
                    return "MH_DYLIB_IN_CACHE";
            }

            assert(false &&
                   "Called MachO::Header::Flags::KindGetString() with unknown "
                   "Kind");
        }

        using ADT::FlagsBase<uint32_t>::FlagsBase;

        [[nodiscard]] constexpr auto noUndefineds() const noexcept {
            return has(Kind::NoUndefineds);
        }

        [[nodiscard]] constexpr auto incrementalLink() const noexcept {
            return has(Kind::IncrementalLink);
        }

        [[nodiscard]] constexpr auto dynamicLinkerLink() const noexcept {
            return has(Kind::DynamicLinkerLink);
        }

        [[nodiscard]] constexpr auto bindAtLoad() const noexcept {
            return has(Kind::BindAtLoad);
        }

        [[nodiscard]] constexpr auto preBound() const noexcept {
            return has(Kind::PreBound);
        }

        [[nodiscard]] constexpr auto splitSegments() const noexcept {
            return has(Kind::SplitSegments);
        }

        [[nodiscard]] constexpr auto lazyInit() const noexcept {
            return has(Kind::LazyInit);
        }

        [[nodiscard]] constexpr auto twoLevelNamespaces() const noexcept {
            return has(Kind::TwoLevelNamespaces);
        }

        [[nodiscard]] constexpr auto forceFlatNamespaces() const noexcept {
            return has(Kind::ForceFlatNamespaces);
        }

        [[nodiscard]] constexpr auto noMultipleDefinitions() const noexcept {
            return has(Kind::NoMultipleDefinitions);
        }

        [[nodiscard]] constexpr auto noFixPrebinding() const noexcept {
            return has(Kind::NoFixPrebinding);
        }

        [[nodiscard]] constexpr auto prebindable() const noexcept {
            return has(Kind::Prebindable);
        }

        [[nodiscard]] constexpr auto allModulesBound() const noexcept {
            return has(Kind::AllModulesBound);
        }

        [[nodiscard]] constexpr auto subsectionsViaSymbols() const noexcept {
            return has(Kind::SubsectionsViaSymbols);
        }

        [[nodiscard]] constexpr auto canonical() const noexcept {
            return has(Kind::Canonical);
        }

        [[nodiscard]] constexpr auto weakDefines() const noexcept {
            return has(Kind::WeakDefines);
        }

        [[nodiscard]] constexpr auto bindsToWeak() const noexcept {
            return has(Kind::BindsToWeak);
        }

        [[nodiscard]] constexpr auto allowStackExecution() const noexcept {
            return has(Kind::AllowStackExecution);
        }

        [[nodiscard]] constexpr auto rootSafe() const noexcept {
            return has(Kind::RootSafe);
        }

        [[nodiscard]] constexpr auto setuidSafe() const noexcept {
            return has(Kind::SetuidSafe);
        }

        [[nodiscard]] constexpr auto noReexportedDylibs() const noexcept {
            return has(Kind::NoReexportedDylibs);
        }

        [[nodiscard]] constexpr auto positionIndependentExec() const noexcept {
            return has(Kind::PositionIndependentExec);
        }

        [[nodiscard]] constexpr auto deadStrippableDylib() const noexcept {
            return has(Kind::DeadStrippableDylib);
        }

        [[nodiscard]] constexpr auto hasTlvDescriptors() const noexcept {
            return has(Kind::HasTlvDescriptors);
        }

        [[nodiscard]] constexpr auto noHeapExecution() const noexcept {
            return has(Kind::NoHeapExecution);
        }

        [[nodiscard]] constexpr auto appExtensionSafe() const noexcept {
            return has(Kind::AppExtensionSafe);
        }

        [[nodiscard]] constexpr auto nlistOutOfSyncDyldInfo() const noexcept {
            return has(Kind::NlistOutOfSyncDyldInfo);
        }

        [[nodiscard]] constexpr auto simSupport() const noexcept {
            return has(Kind::SimSupport);
        }

        [[nodiscard]] constexpr auto dylibInCache() const noexcept {
            return has(Kind::DylibInCache);
        }

        [[nodiscard]]
        constexpr auto setNoUndefineds(const bool Value = true) noexcept {
            return setForValue(Kind::NoUndefineds, Value);
        }

        [[nodiscard]]
        constexpr auto setIncrementalLink(const bool Value = true) noexcept {
            return setForValue(Kind::IncrementalLink, Value);
        }

        [[nodiscard]]
        constexpr auto setDynamicLinkerLink(const bool Value = true) noexcept {
            return setForValue(Kind::DynamicLinkerLink, Value);
        }

        [[nodiscard]]
        constexpr auto setBindAtLoad(const bool Value = true) noexcept {
            return setForValue(Kind::BindAtLoad, Value);
        }

        [[nodiscard]]
        constexpr auto setPreBound(const bool Value = true) noexcept {
            return setForValue(Kind::PreBound, Value);
        }

        [[nodiscard]]
        constexpr auto setSplitSegments(const bool Value = true) noexcept {
            return setForValue(Kind::SplitSegments, Value);
        }

        [[nodiscard]]
        constexpr auto setLazyInit(const bool Value = true) noexcept {
            return setForValue(Kind::LazyInit, Value);
        }

        [[nodiscard]]
        constexpr auto setTwoLevelNamespaces(const bool Value = true) noexcept {
            return setForValue(Kind::TwoLevelNamespaces, Value);
        }

        [[nodiscard]] constexpr
        auto setForceFlatNamespaces(const bool Value = true) noexcept {
            return setForValue(Kind::ForceFlatNamespaces, Value);
        }

        [[nodiscard]] constexpr
        auto setNoMultipleDefinitions(const bool Value = true) noexcept {
            return setForValue(Kind::NoMultipleDefinitions, Value);
        }

        [[nodiscard]]
        constexpr auto setNoFixPrebinding(const bool Value = true) noexcept {
            return setForValue(Kind::NoFixPrebinding, Value);
        }

        [[nodiscard]]
        constexpr auto setPrebindable(const bool Value = true) noexcept {
            return setForValue(Kind::Prebindable, Value);
        }

        [[nodiscard]]
        constexpr auto setAllModulesBound(const bool Value = true) noexcept {
            return setForValue(Kind::AllModulesBound, Value);
        }

        [[nodiscard]] constexpr
        auto setSubsectionsViaSymbols(const bool Value = true) noexcept {
            return setForValue(Kind::SubsectionsViaSymbols, Value);
        }

        [[nodiscard]]
        constexpr auto setCanonical(const bool Value = true) noexcept {
            return setForValue(Kind::Canonical, Value);
        }

        [[nodiscard]]
        constexpr auto setWeakDefines(const bool Value = true) noexcept {
            return setForValue(Kind::WeakDefines, Value);
        }

        [[nodiscard]]
        constexpr auto setBindsToWeak(const bool Value = true) noexcept {
            return setForValue(Kind::BindsToWeak, Value);
        }

        [[nodiscard]] constexpr
        auto setAllowStackExecution(const bool Value = true) noexcept {
            return setForValue(Kind::AllowStackExecution, Value);
        }

        [[nodiscard]]
        constexpr auto setRootSafe(const bool Value = true) noexcept {
            return setForValue(Kind::RootSafe, Value);
        }

        [[nodiscard]]
        constexpr auto setSetuidSafe(const bool Value = true) noexcept {
            return setForValue(Kind::SetuidSafe, Value);
        }

        [[nodiscard]]
        constexpr auto setNoReexportedDylibs(const bool Value = true) noexcept {
            return setForValue(Kind::NoReexportedDylibs, Value);
        }

        [[nodiscard]] constexpr
        auto setPositionIndependentExec(const bool Value = true) noexcept {
            return setForValue(Kind::PositionIndependentExec, Value);
        }

        [[nodiscard]] constexpr
        auto setDeadStrippableDylib(const bool Value = true) noexcept {
            return setForValue(Kind::DeadStrippableDylib, Value);
        }

        [[nodiscard]]
        constexpr auto setHasTlvDescriptors(const bool Value = true) noexcept {
            return setForValue(Kind::HasTlvDescriptors, Value);
        }

        [[nodiscard]]
        constexpr auto setNoHeapExecution(const bool Value = true) noexcept {
            return setForValue(Kind::NoHeapExecution, Value);
        }

        [[nodiscard]]
        constexpr auto setAppExtensionSafe(const bool Value = true) noexcept {
            return setForValue(Kind::AppExtensionSafe, Value);
        }

        [[nodiscard]] constexpr
        auto setNlistOutOfSyncDyldInfo(const bool Value = true) noexcept {
            return setForValue(Kind::NlistOutOfSyncDyldInfo, Value);
        }

        [[nodiscard]]
        constexpr auto setSimSupport(const bool Value = true) noexcept {
            return setForValue(Kind::SimSupport, Value);
        }

        [[nodiscard]]
        constexpr auto setDylibInCache(const bool Value = true) noexcept {
            return setForValue(Kind::DylibInCache, Value);
        }
    };

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
