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

    struct Header {
        enum class FlagsEnum {
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

            AllowStackExecution = 1 << 16,

            RootSafe = 1 << 17,
            SetuidSafe = 1 << 18,

            NoReexportedDylibs = 1 << 19,
            PositionIndependentExec = 1 << 20,
            DeadStrippableDylib = 1 << 21,
            HasTlvDescriptors = 1 << 22,
            NoHeapExecution = 1 << 23,
            AppExtensionSafe = 1 << 24,

            NlistOutOfSyncDyldInfo = 1 << 25,

            SimSupport = 1 << 26,
            DylibInCache = 1 << 31,
        };

        Magic Magic;
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
            return ADT::SwitchEndianIf(Flags, isBigEndian());
        }
    };
}
