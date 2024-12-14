//
//  ADT/Mach-O/Headers.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/BasicContiguousList.h"
#include "ADT/BasicMasksHandler.h"
#include "ADT/Range.h"

#include "ADT/Mach/Info.h"
#include "Utils/SwitchEndian.h"

#include "LoadCommandStorage.h"

namespace MachO {
    struct Header {
        enum class Magic : uint32_t {
            Default = 0xfeedface,
            Default64 = 0xfeedfacf,

            BigEndian = 0xcefaedfe,
            BigEndian64 = 0xcffaedfe,
        };

        [[nodiscard]]
        constexpr static auto MagicIsValid(const Magic Magic) noexcept {
            switch (Magic) {
                case Magic::Default:
                case Magic::Default64:
                case Magic::BigEndian:
                case Magic::BigEndian64:
                    return true;
            }

            return false;
        }

        [[nodiscard]]
        constexpr static auto MagicGetName(const Magic Magic) noexcept
            -> std::optional<std::string_view>
        {
            switch (Magic) {
                case Magic::Default:
                    return "MH_MAGIC";
                case Magic::Default64:
                    return "MH_MAGIC_64";
                case Magic::BigEndian:
                    return "MH_CIGAM";
                case Magic::BigEndian64:
                    return "MH_CIGAM_64";
            }

            return std::nullopt;
        }

        [[nodiscard]]
        constexpr static auto MagicGetDescription(const Magic Magic) noexcept
            -> std::optional<std::string_view>
        {
            switch (Magic) {
                case Magic::Default:
                    return "Default";
                case Magic::Default64:
                    return "Default (64-Bit)";
                case Magic::BigEndian:
                    return "Big Endian";
                case Magic::BigEndian64:
                    return "Big Endian (64-Bit)";
            }

            return std::nullopt;
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
            KextBundle,
            FileSet
        };

        [[nodiscard]] constexpr
        static auto FileKindIsRecognized(const FileKind FileKind) noexcept {
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
                case FileKind::FileSet:
                    return true;
            }

            return false;
        }

        [[nodiscard]] constexpr
        static auto FileKindGetName(const FileKind FileKind) noexcept
            -> std::optional<std::string_view>
        {
            switch (FileKind) {
                case FileKind::Object:
                    return "MH_OBJECT";
                case FileKind::Executable:
                    return "MH_EXECUTE";
                case FileKind::FixedVMSharedLibrary:
                    return "MH_FVMLIB";
                case FileKind::CoreFile:
                    return "MH_CORE";
                case FileKind::PreloadedExecutable:
                    return "MH_PRELOAD";
                case FileKind::Dylib:
                    return "MH_DYLIB";
                case FileKind::Dylinker:
                    return "MH_DYLINKER";
                case FileKind::Bundle:
                    return "MH_BUNDLE";
                case FileKind::DylibStub:
                    return "MH_DYLIB_STUB";
                case FileKind::Dsym:
                    return "MH_DSYM";
                case FileKind::KextBundle:
                    return "MH_KEXT_BUNDLE";
                case FileKind::FileSet:
                    return "MH_FILESET";
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr static auto
        FileKindGetDescription(const FileKind FileKind) noexcept
            -> std::optional<std::string_view>
        {
            switch (FileKind) {
                case FileKind::Object:
                    return "Object File";
                case FileKind::Executable:
                    return "Executable";
                case FileKind::FixedVMSharedLibrary:
                    return "Fixed-VM Shared Library";
                case FileKind::CoreFile:
                    return "Core File";
                case FileKind::PreloadedExecutable:
                    return "Pre-loaded Executable";
                case FileKind::Dylib:
                    return "Dynamic Library (Dylib)";
                case FileKind::Dylinker:
                    return "Dynamic Linker (Dylinker)";
                case FileKind::Bundle:
                    return "Bundle";
                case FileKind::DylibStub:
                    return "Dynamic-Library Stub";
                case FileKind::Dsym:
                    return "Debug Symbols File (.dSYM)";
                case FileKind::KextBundle:
                    return "Kernel Extension Bundle";
                case FileKind::FileSet:
                    return "FileSet";
            }

            return std::nullopt;
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

        [[nodiscard]] constexpr static
        auto FlagsEnumGetName(const FlagsEnum Flag) noexcept
            -> std::optional<std::string_view>
        {
            using Enum = FlagsEnum;
            switch (Flag) {
                case Enum::NoUndefinedReferences:
                    return "MH_NOUNDEF";
                case Enum::IncrementalLink:
                    return "MH_INCRLINK";
                case Enum::DyldLink:
                    return "MH_DYLDLINK";
                case Enum::BindAtLoad:
                    return "MH_BINDATLOAD";
                case Enum::PreBound:
                    return "MH_PREBOUND";
                case Enum::SplitSegments:
                    return "MH_SPLIT_SEGS";
                case Enum::LazyInitialization:
                    return "MH_LAZY_INIT";
                case Enum::TwoLevelNamespaceBindings:
                    return "MH_TWOLEVEL";
                case Enum::ForceFlatNamespaces:
                    return "MH_FORCE_FLAT";
                case Enum::NoMultipleDefinitions:
                    return "MH_NOMULTIDEFS";
                case Enum::NoFixPrebinding:
                    return "MH_NOFIXPREBINDING";
                case Enum::Prebindable:
                    return "MH_PREBINDABLE";
                case Enum::AllModulesBound:
                    return "MH_ALLMODSBOUND";
                case Enum::SubsectionsViaSymbols:
                    return "MH_SUBSECTIONS_VIA_SYMBOLS";
                case Enum::Canonical:
                    return "MH_CANONICAL";
                case Enum::WeakDefinitions:
                    return "MH_WEAK_DEFINES";
                case Enum::BindsToWeak:
                    return "MH_BINDS_TO_WEAK";
                case Enum::AllowStackExecution:
                    return "MH_ALLOW_STACK_EXECUTION";
                case Enum::RootSafe:
                    return "MH_ROOT_SAFE";
                case Enum::SetuidSafe:
                    return "MH_SETUID_SAFE";
                case Enum::NoReexportedDylibs:
                    return "MH_NO_REEXPORTED_DYLIBS";
                case Enum::PositionIndependentExecutable:
                    return "MH_PIE";
                case Enum::DeadStrippableDylib:
                    return "MH_DEAD_STRIPPABLE_DYLIB";
                case Enum::HasTlvDescriptors:
                    return "MH_HAS_TLV_DESCRIPTORS";
                case Enum::NoHeapExecution:
                    return "MH_NO_HEAP_EXECUTION";
                case Enum::AppExtensionSafe:
                    return "MH_APP_EXTENSION_SAFE";
                case Enum::NlistOutOfSyncWithDyldInfo:
                    return "MH_NLIST_OUT_OF_SYNC_WITH_DYLDINFO";
                case Enum::SimulatorSupport:
                    return "MH_SIM_SUPPORT";
                case Enum::DylibInCache:
                    return "MH_DYLIB_IN_CACHE";
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr static auto
        FlagsEnumGetDescription(const FlagsEnum Flag) noexcept
            -> std::optional<std::string_view>
        {
            using Enum = FlagsEnum;
            switch (Flag) {
                case Enum::NoUndefinedReferences:
                    return "No Undefined References";
                case Enum::IncrementalLink:
                    return "Incrementally Linked";
                case Enum::DyldLink:
                    return "Dynamic Linker Input";
                case Enum::BindAtLoad:
                    return "Bind Undefined References at Launch";
                case Enum::PreBound:
                    return "Dynamic Undefined Reference PreBound";
                case Enum::SplitSegments:
                    return "Read-only and Write-only Segments Split";
                case Enum::LazyInitialization:
                    return "Lazy Initialization";
                case Enum::TwoLevelNamespaceBindings:
                    return "Has Two-Level Namespace Bindings";
                case Enum::ForceFlatNamespaces:
                    return "Has Two-Level Namespace Bindings";
                case Enum::NoMultipleDefinitions:
                    return "No Multiple Symbol Definitions";
                case Enum::NoFixPrebinding:
                    return "Don't Fix PreBinding";
                case Enum::Prebindable:
                    return "Prebound - But can be Prebindable Again";
                case Enum::AllModulesBound:
                    return "Binds to all Two-Level Namespace Modules";
                case Enum::SubsectionsViaSymbols:
                    return "Safe to Divide Sections into Sub-Sections";
                case Enum::Canonical:
                    return "Canonicalized via UnPreBind";
                case Enum::WeakDefinitions:
                    return "Has External Weak Symbols";
                case Enum::BindsToWeak:
                    return "Binds to Weak Symbols";
                case Enum::AllowStackExecution:
                    return "Stacks have Execution Privilege";
                case Enum::RootSafe:
                    return "Safe for root (uid 0) privileges";
                case Enum::SetuidSafe:
                    return "Safe for issetuid()";
                case Enum::NoReexportedDylibs:
                    return "Position-Independent Executable";
                case Enum::PositionIndependentExecutable:
                    return "No Re-Exported Dylibs";
                case Enum::DeadStrippableDylib:
                    return "Dead Strippable Dylib";
                case Enum::HasTlvDescriptors:
                    return "Has thread-local variables section";
                case Enum::NoHeapExecution:
                    return "No Heap Execution";
                case Enum::AppExtensionSafe:
                    return "App Extension Safe";
                case Enum::NlistOutOfSyncWithDyldInfo:
                    return "Nlist Out-Of-Sync with DyldInfo";
                case Enum::SimulatorSupport:
                    return "Has Simulator Support";
                case Enum::DylibInCache:
                    return "Dylib in Shared-Cache";
            }

            return std::nullopt;
        }

        using FlagsType = BasicFlags<FlagsEnum>;

        Magic Magic;
        int32_t CpuKind;
        int32_t CpuSubKind;
        uint32_t FileKind;
        uint32_t Ncmds;
        uint32_t SizeOfCmds;
        uint32_t Flags;

        [[nodiscard]] constexpr auto hasValidMagic() const noexcept {
            return MagicIsValid(this->Magic);
        }

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
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

        [[nodiscard]] constexpr auto hasValidFileKind() const noexcept {
            const auto Value =
                SwitchEndianIf(this->FileKind, this->isBigEndian());

            return FileKindIsRecognized(static_cast<enum FileKind>(Value));
        }

        [[nodiscard]] constexpr auto is64Bit() const noexcept {
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

        [[nodiscard]] constexpr auto getCpuKind() const noexcept {
            const auto CpuKind =
                SwitchEndianIf(this->CpuKind, this->isBigEndian());

            return Mach::CpuKind(CpuKind);
        }

        [[nodiscard]]
        constexpr auto getCpuSubKind() const noexcept {
            return SwitchEndianIf(this->CpuSubKind, this->isBigEndian());
        }

        [[nodiscard]]
        constexpr auto getFileKind() const noexcept {
            const auto Value =
                SwitchEndianIf(this->FileKind, this->isBigEndian());

            return static_cast<enum FileKind>(Value);
        }

        [[nodiscard]] inline auto getLoadCommandsCount() const noexcept {
            return SwitchEndianIf(this->Ncmds, this->isBigEndian());
        }

        [[nodiscard]] inline auto getLoadCommandsSize() const noexcept {
            return SwitchEndianIf(this->SizeOfCmds, this->isBigEndian());
        }

        [[nodiscard]] constexpr auto getFlags() const noexcept {
            const auto Flags = SwitchEndianIf(this->Flags, this->isBigEndian());
            return FlagsType(Flags);
        }

        constexpr auto setCpuKind(const Mach::CpuKind Kind) noexcept {
            const auto Value = static_cast<int32_t>(Kind);
            this->CpuKind = SwitchEndianIf(Value, this->isBigEndian());

            return *this;
        }

        constexpr auto setLoadCommandsCount(const uint32_t Value) noexcept {
            this->Ncmds = SwitchEndianIf(Value, this->isBigEndian());
            return *this;
        }

        constexpr auto setLoadCommandsSize(const uint32_t Value) noexcept {
            this->SizeOfCmds = SwitchEndianIf(Value, this->isBigEndian());
            return *this;
        }

        constexpr auto setFlags(const FlagsType &Flags) noexcept
            -> decltype(*this)
        {
            const auto FlagsValue = Flags.value();
            this->Flags = SwitchEndianIf(FlagsValue, this->isBigEndian());

            return *this;
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            if (this->is64Bit()) {
                return sizeof(*this) + sizeof(uint32_t);
            }

            return sizeof(*this);
        }

        [[nodiscard]] inline auto getLoadCmdBuffer() noexcept {
            const auto LoadCmdBuffer =
                reinterpret_cast<uint8_t *>(this) + size();

            return LoadCmdBuffer;
        }

        [[nodiscard]]
        inline auto getConstLoadCmdBuffer() const noexcept {
            const auto LoadCmdBuffer =
                reinterpret_cast<const uint8_t *>(this) + size();

            return LoadCmdBuffer;
        }

        [[nodiscard]] inline auto
        GetLoadCmdStorage(const bool Verify = true) noexcept {
            const auto Result =
                LoadCommandStorage::Open(this->getLoadCmdBuffer(),
                                         this->getLoadCommandsCount(),
                                         this->getLoadCommandsSize(),
                                         this->isBigEndian(),
                                         this->is64Bit(),
                                         Verify);

            return Result;
        }

        [[nodiscard]] inline auto
        GetConstLoadCmdStorage(const bool Verify = true) const noexcept {
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

        [[nodiscard]]
        constexpr static auto MagicIsValid(const Magic Magic) noexcept {
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
        static auto MagicGetName(const Magic Magic) noexcept
            -> std::optional<std::string_view>
        {
            switch (Magic) {
                case Magic::Default:
                    return "FAT_MAGIC";
                case Magic::Default64:
                    return "FAT_MAGIC_64";
                case Magic::BigEndian:
                    return "FAT_CIGAM";
                case Magic::BigEndian64:
                    return "FAT_CIGAM_64";
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr static
        auto MagicGetDescription(const Magic Magic) noexcept
            -> std::optional<std::string_view>
        {
            switch (Magic) {
                case Magic::Default:
                    return "Default";
                case Magic::Default64:
                    return "Default (64-Bit)";
                case Magic::BigEndian:
                    return "Big Endian";
                case Magic::BigEndian64:
                    return "Big Endian (64-Bit)";
            }

            return std::nullopt;
        }

        struct Arch32 {
            int32_t CpuKind;
            int32_t CpuSubKind;
            uint32_t Offset;
            uint32_t Size;
            uint32_t Align;

            [[nodiscard]]
            constexpr auto getCpuKind(const bool IsBigEndian) const noexcept {
                return Mach::CpuKind(SwitchEndianIf(CpuKind, IsBigEndian));
            }

            [[nodiscard]] constexpr
            auto getCpuSubKind(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CpuSubKind, IsBigEndian);
            }

            [[nodiscard]] constexpr
            auto getFileOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getFileSize(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getAlign(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getFileRange(const bool IsBigEndian) const noexcept
                -> std::optional<Range>
            {
                const auto Offset = this->getFileOffset(IsBigEndian);
                const auto Size = this->getFileSize(IsBigEndian);

                return Range::CreateWithSize(Offset, Size);
            }

            constexpr auto
            setCpuKind(const Mach::CpuKind Kind,
                       const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                const auto Value = static_cast<int32_t>(Kind);
                this->CpuKind = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr auto
            setCpuSubKind(const int32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->CpuSubKind = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            getFileOffset(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setFileSize(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
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

            [[nodiscard]]
            constexpr auto getCpuKind(const bool IsBigEndian) const noexcept {
                return Mach::CpuKind(SwitchEndianIf(CpuKind, IsBigEndian));
            }

            [[nodiscard]] constexpr
            auto getCpuSubKind(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CpuSubKind, IsBigEndian);
            }

            [[nodiscard]] constexpr
            auto getFileOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getFileSize(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getAlign(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getReserved(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getFileRange(const bool IsBigEndian) const noexcept
                -> std::optional<Range>
            {
                const auto Offset = this->getFileOffset(IsBigEndian);
                const auto Size = this->getFileSize(IsBigEndian);

                return Range::CreateWithSize(Offset, Size);
            }

            constexpr auto
            setCpuKind(const Mach::CpuKind Kind,
                       const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                const auto Value = static_cast<int32_t>(Kind);
                this->CpuKind = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr auto
            setCpuSubKind(const int32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->CpuSubKind = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            getFileOffset(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setFileSize(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
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

        [[nodiscard]] inline auto is64Bit() const noexcept {
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

        [[nodiscard]] inline auto isBigEndian() const noexcept {
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

        [[nodiscard]] inline auto getArchCount() const noexcept {
            return SwitchEndianIf(NFatArch, this->isBigEndian());
        }

        [[nodiscard]] inline auto getArch32List() noexcept {
            assert(!this->is64Bit());

            const auto Entries = reinterpret_cast<Arch32 *>(this + 1);
            return Arch32List(Entries, this->getArchCount());
        }

        [[nodiscard]] inline auto getArch64List() noexcept {
            assert(this->is64Bit());

            const auto Entries = reinterpret_cast<Arch64 *>(this + 1);
            return Arch64List(Entries, this->getArchCount());
        }

        [[nodiscard]] inline auto getConstArch32List() const noexcept {
            assert(!this->is64Bit());

            const auto Entries = reinterpret_cast<const Arch32 *>(this + 1);
            return ConstArch32List(Entries, this->getArchCount());
        }

        [[nodiscard]] inline auto getConstArch64List() const noexcept {
            assert(this->is64Bit());

            const auto Entries = reinterpret_cast<const Arch64 *>(this + 1);
            return ConstArch64List(Entries, this->getArchCount());
        }

        [[nodiscard]] inline auto hasValidMagic() const noexcept {
            return MagicIsValid(this->Magic);
        }

        [[nodiscard]] inline auto hasZeroArchs() const noexcept {
            return this->NFatArch == 0;
        }

        inline auto setArchCount(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            this->NFatArch = SwitchEndianIf(Value, this->isBigEndian());
            return *this;
        }
    };
}
