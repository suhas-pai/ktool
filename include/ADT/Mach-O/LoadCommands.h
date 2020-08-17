//
//  include/ADT/Mach-O/LoadCommands.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "ADT/BasicContiguousList.h"
#include "ADT/BasicMasksHandler.h"
#include "ADT/LargestIntHelper.h"
#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerErrorStorage.h"
#include "ADT/PointerOrError.h"

#include "TypeTraits/IsEnumClass.h"

#include "Utils/DoesOverflow.h"
#include "Utils/Leb128.h"
#include "Utils/MiscTemplates.h"
#include "Utils/SwitchEndian.h"

#include "BindInfo.h"
#include "ExportTrie.h"
#include "LoadCommandsCommon.h"
#include "LoadCommandTemplates.h"
#include "RebaseInfo.h"
#include "SegmentUtil.h"

namespace MachO {
    struct LoadCommand {
        using Kind = LoadCommandKind;

        template <Kind Kind>
        [[nodiscard]] inline bool isa(bool IsBigEndian) const noexcept {
            return (this->getKind(IsBigEndian) == Kind);
        }

        template <Kind Kind>
        [[nodiscard]] inline LoadCommandTypeFromKind<Kind> &
        cast(bool IsBigEndian) noexcept {
            assert(isa<Kind>(IsBigEndian));
            return reinterpret_cast<LoadCommandTypeFromKind<Kind> &>(*this);
        }

        template <Kind Kind>
        [[nodiscard]] inline const LoadCommandTypeFromKind<Kind> &
        cast(bool IsBigEndian) const noexcept {
            using Type = LoadCommandTypeFromKind<Kind>;

            assert(isa<Kind>(IsBigEndian));
            const auto &Result = reinterpret_cast<const Type &>(*this);

            return Result;
        }

        template <Kind Kind>
        [[nodiscard]] inline LoadCommandPtrTypeFromKind<Kind>
        dynCast(bool IsBigEndian) noexcept {
            if (isa<Kind>(IsBigEndian)) {
                return &cast<Kind>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind Kind>
        [[nodiscard]] inline const LoadCommandConstPtrTypeFromKind<Kind>
        dynCast(bool IsBigEndian) const noexcept {
            if (isa<Kind>(IsBigEndian)) {
                return &cast<Kind>(IsBigEndian);
            }

            return nullptr;
        }

        [[nodiscard]]
        static const std::string_view &KindGetName(Kind Kind) noexcept;

        [[nodiscard]]
        static const std::string_view &KindGetDescription(Kind Kind) noexcept;

        [[nodiscard]]
        constexpr inline static bool KindIsRequiredByDyld(Kind Kind) noexcept {
            return (static_cast<uint32_t>(Kind) & KindRequiredByDyld);
        }

        [[nodiscard]]
        constexpr static inline bool KindIsValid(Kind Kind) noexcept {
            switch (Kind) {
                case Kind::Segment:
                case Kind::SymbolTable:
                case Kind::SymbolSegment:
                case Kind::Thread:
                case Kind::UnixThread:
                case Kind::LoadFixedVMSharedLibrary:
                case Kind::IdFixedVMSharedLibrary:
                case Kind::Ident:
                case Kind::FixedVMFile:
                case Kind::PrePage:
                case Kind::DynamicSymbolTable:
                case Kind::LoadDylib:
                case Kind::IdDylib:
                case Kind::LoadDylinker:
                case Kind::IdDylinker:
                case Kind::PreBoundDylib:
                case Kind::Routines:
                case Kind::SubFramework:
                case Kind::SubUmbrella:
                case Kind::SubClient:
                case Kind::SubLibrary:
                case Kind::TwoLevelHints:
                case Kind::PrebindChecksum:
                case Kind::LoadWeakDylib:
                case Kind::Segment64:
                case Kind::Routines64:
                case Kind::Uuid:
                case Kind::Rpath:
                case Kind::CodeSignature:
                case Kind::SegmentSplitInfo:
                case Kind::ReexportDylib:
                case Kind::LazyLoadDylib:
                case Kind::EncryptionInfo:
                case Kind::DyldInfo:
                case Kind::DyldInfoOnly:
                case Kind::LoadUpwardDylib:
                case Kind::VersionMinimumMacOSX:
                case Kind::VersionMinimumIPhoneOS:
                case Kind::FunctionStarts:
                case Kind::DyldEnvironment:
                case Kind::Main:
                case Kind::DataInCode:
                case Kind::SourceVersion:
                case Kind::DylibCodeSignDRS:
                case Kind::EncryptionInfo64:
                case Kind::LinkerOption:
                case Kind::LinkerOptimizationHint:
                case Kind::VersionMinimumTvOS:
                case Kind::VersionMinimumWatchOS:
                case Kind::Note:
                case Kind::BuildVersion:
                case Kind::DyldExportsTrie:
                case Kind::DyldChainedFixups:
                    return true;
            }

            return false;
        }

        [[nodiscard]]
        constexpr static inline bool KindIsSharedLibrary(Kind Kind) noexcept {
            switch (Kind) {
                case Kind::LoadDylib:
                case Kind::LoadWeakDylib:
                case Kind::ReexportDylib:
                case Kind::LazyLoadDylib:
                case Kind::LoadUpwardDylib:
                    return true;
                case Kind::Segment:
                case Kind::SymbolTable:
                case Kind::SymbolSegment:
                case Kind::Thread:
                case Kind::UnixThread:
                case Kind::LoadFixedVMSharedLibrary:
                case Kind::IdFixedVMSharedLibrary:
                case Kind::Ident:
                case Kind::FixedVMFile:
                case Kind::PrePage:
                case Kind::DynamicSymbolTable:
                case Kind::IdDylib:
                case Kind::LoadDylinker:
                case Kind::IdDylinker:
                case Kind::PreBoundDylib:
                case Kind::Routines:
                case Kind::SubFramework:
                case Kind::SubUmbrella:
                case Kind::SubClient:
                case Kind::SubLibrary:
                case Kind::TwoLevelHints:
                case Kind::PrebindChecksum:
                case Kind::Segment64:
                case Kind::Routines64:
                case Kind::Uuid:
                case Kind::Rpath:
                case Kind::CodeSignature:
                case Kind::SegmentSplitInfo:
                case Kind::EncryptionInfo:
                case Kind::DyldInfo:
                case Kind::DyldInfoOnly:
                case Kind::VersionMinimumMacOSX:
                case Kind::VersionMinimumIPhoneOS:
                case Kind::FunctionStarts:
                case Kind::DyldEnvironment:
                case Kind::Main:
                case Kind::DataInCode:
                case Kind::SourceVersion:
                case Kind::DylibCodeSignDRS:
                case Kind::EncryptionInfo64:
                case Kind::LinkerOption:
                case Kind::LinkerOptimizationHint:
                case Kind::VersionMinimumTvOS:
                case Kind::VersionMinimumWatchOS:
                case Kind::Note:
                case Kind::BuildVersion:
                case Kind::DyldExportsTrie:
                case Kind::DyldChainedFixups:
                    return false;
            }
        }

        enum class CmdSizeInvalidType {
            None,
            TooSmall,
            TooLarge
        };

        [[nodiscard]]
        LoadCommand::CmdSizeInvalidType
        ValidateCmdsize(const LoadCommand *LoadCmd, bool IsBigEndian) noexcept;

        uint32_t Cmd;
        uint32_t CmdSize;

        [[nodiscard]]
        constexpr inline Kind getKind(bool IsBigEndian) const noexcept {
            return Kind(SwitchEndianIf(Cmd, IsBigEndian));
        }

        constexpr
        inline LoadCommand &setKind(Kind Kind, bool IsBigEndian) noexcept {
            const auto Cmd = static_cast<uint32_t>(Kind);
            this->Cmd = SwitchEndianIf(Cmd, IsBigEndian);

            return *this;
        }
    };

    struct SegmentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Section {
            char Name[16];
            char SegName[16];
            uint32_t Addr;
            uint32_t Size;
            uint32_t Offset;
            uint32_t Align;
            uint32_t Reloff;
            uint32_t Nreloc;
            uint32_t Flags;
            uint32_t Reserved1;
            uint32_t Reserved2;

            [[nodiscard]] constexpr inline
            SegmentSectionFlags getFlags(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Flags, IsBigEndian);
            }

            constexpr inline Section &
            setFlags(const SegmentSectionFlags &Flags,
                     bool IsBigEndian) noexcept
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }
        };

        using SectionList = BasicContiguousList<Section>;
        using ConstSectionList = BasicContiguousList<const Section>;

        char Name[16];
        uint32_t VmAddr;
        uint32_t VmSize;
        uint32_t FileOff;
        uint32_t FileSize;
        int32_t MaxProt;
        int32_t InitProt;
        uint32_t Nsects;
        uint32_t Flags;

        [[nodiscard]] constexpr
        inline MemoryProtections getInitProt(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InitProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline MemoryProtections getMaxProt(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MaxProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline SegmentFlags getFlags(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        constexpr inline SegmentCommand &
        setInitProt(const MemoryProtections &InitProt,
                    bool IsBigEndian) noexcept
        {
            this->InitProt = SwitchEndianIf(InitProt.value(), IsBigEndian);
            return *this;
        }

        constexpr inline SegmentCommand &
        setMaxProt(const MemoryProtections &MaxProt,
                   bool IsBigEndian) noexcept
        {
            this->MaxProt = SwitchEndianIf(MaxProt.value(), IsBigEndian);
            return *this;
        }

        constexpr inline SegmentCommand &
        setFlags(const SegmentFlags &Flags, bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] SectionList GetSectionList(bool IsBigEndian) noexcept;

        [[nodiscard]]
        ConstSectionList GetConstSectionList(bool IsBigEndian) const noexcept;
    };

    struct SegmentCommand64 : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Section {
            char Name[16];
            char SegName[16];
            uint64_t Addr;
            uint64_t Size;
            uint32_t Offset;
            uint32_t Align;
            uint32_t Reloff;
            uint32_t Nreloc;
            uint32_t Flags;
            uint32_t Reserved1;
            uint32_t Reserved2;

            [[nodiscard]] constexpr inline
            SegmentSectionFlags getFlags(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Flags, IsBigEndian);
            }

            constexpr inline Section &
            setFlags(const SegmentSectionFlags &Flags,
                     bool IsBigEndian) noexcept
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }
        };

        using SectionList = BasicContiguousList<Section>;
        using ConstSectionList = BasicContiguousList<const Section>;

        char Name[16];
        uint64_t VmAddr;
        uint64_t VmSize;
        uint64_t FileOff;
        uint64_t FileSize;
        int32_t MaxProt;
        int32_t InitProt;
        uint32_t Nsects;
        uint32_t Flags;

        [[nodiscard]] constexpr
        inline MemoryProtections getInitProt(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InitProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline MemoryProtections getMaxProt(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MaxProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline SegmentFlags getFlags(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        constexpr inline SegmentCommand64 &
        setInitProt(const MemoryProtections &InitProt,
                    bool IsBigEndian) noexcept
        {
            this->InitProt = SwitchEndianIf(InitProt.value(), IsBigEndian);
            return *this;
        }

        constexpr inline SegmentCommand64 &
        setMaxProt(const MemoryProtections &MaxProt,
                   bool IsBigEndian) noexcept
        {
            this->MaxProt = SwitchEndianIf(MaxProt.value(), IsBigEndian);
            return *this;
        }

        constexpr inline SegmentCommand64 &
        setFlags(const SegmentFlags &Flags, bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] SectionList GetSectionList(bool IsBigEndian) noexcept;

        [[nodiscard]]
        ConstSectionList GetConstSectionList(bool IsBigEndian) const noexcept;
    };

    template <typename Error>
    struct LoadCommandStringViewOrError {
        static_assert(TypeTraits::IsEnumClassValue<Error>,
                      "Error-Type is not an enum class");
    protected:
        std::string_view View;
        [[nodiscard]] inline uintptr_t getStorage() const noexcept {
            return reinterpret_cast<uintptr_t>(View.begin());
        }
    public:
        LoadCommandStringViewOrError(const std::string_view &View) noexcept
        : View(View) {}

        LoadCommandStringViewOrError(Error Value) noexcept {
            View = std::string_view(reinterpret_cast<const char *>(Value), 1);
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            const auto HasError =
                PointerErrorStorage<Error>::PointerHasErrorValue(getStorage());

            return HasError;
        }

        [[nodiscard]] inline Error getError() const noexcept {
            if (!hasError()) {
                return Error::None;
            }

            return Error(getStorage());
        }

        [[nodiscard]]
        inline const std::string_view &getString() const noexcept {
            assert(!hasError());
            return View;
        }
    };

    union LoadCommandString {
        uint32_t Offset;
    #ifndef __LP64__
        char *Ptr;
    #endif

        enum class GetStringError {
            None,
            OffsetOutOfBounds,
            NoNullTerminator,
            EmptyString,
        };

        using GetValueResult = LoadCommandStringViewOrError<GetStringError>;

        [[nodiscard]] bool
        IsOffsetValid(uint32_t MinSize,
                      uint32_t CmdSize,
                      bool IsBigEndian) const noexcept;

        [[nodiscard]]
        uint32_t GetLength(uint32_t CmdSize, bool IsBigEndian) const noexcept;
    };

    enum class PackedVersionMasks : uint32_t {
        Revision = 0xff,
        Minor = 0xff00,
        Major = 0xffff0000
    };

    enum class PackedVersionShifts : uint32_t {
        Revision,
        Minor = 8,
        Major = 16
    };

    struct PackedVersion :
        public
            BasicMasksAndShiftsHandler<PackedVersionMasks, PackedVersionShifts>
    {
    private:
        using Base =
            BasicMasksAndShiftsHandler<PackedVersionMasks, PackedVersionShifts>;
    public:
        constexpr PackedVersion() noexcept = default;
        constexpr PackedVersion(uint32_t Integer) noexcept : Base(Integer) {}

        using Masks = PackedVersionMasks;
        using Shifts = PackedVersionShifts;

        [[nodiscard]] constexpr inline uint8_t getRevision() const noexcept {
            return getValueForMaskAndShift(Masks::Revision, Shifts::Revision);
        }

        [[nodiscard]] constexpr inline uint8_t getMinor() const noexcept {
            return getValueForMaskAndShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr inline uint16_t getMajor() const noexcept {
            return getValueForMaskAndShift(Masks::Major, Shifts::Major);
        }

        constexpr inline PackedVersion &setRevision(uint8_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision, Shifts::Revision, Value);
            return *this;
        }

        constexpr inline PackedVersion &setMinor(uint8_t Value) noexcept {
            setValueForMaskAndShift(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr inline PackedVersion &setMajor(uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Major, Shifts::Major, Value);
            return *this;
        }
    };

    struct FixedVmSharedLibraryInfo {
        LoadCommandString Name;
        PackedVersion MinorVersion;
        uint32_t HeaderAddr;
    };

    struct FixedVMSharedLibraryCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::IdFixedVMSharedLibrary) ||
                (Kind == LoadCommand::Kind::LoadFixedVMSharedLibrary);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMSharedLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        FixedVmSharedLibraryInfo Info;

        [[nodiscard]]
        inline bool IsNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto Offset = SwitchEndianIf(Info.Name.Offset, IsBigEndian);
            return Info.Name.IsOffsetValid(sizeof(*this), Offset, CmdSize);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct DylibCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::LoadDylib) ||
                (Kind == LoadCommand::Kind::IdDylib) ||
                (Kind == LoadCommand::Kind::LoadWeakDylib) ||
                (Kind == LoadCommand::Kind::ReexportDylib) ||
                (Kind == LoadCommand::Kind::LazyLoadDylib) ||
                (Kind == LoadCommand::Kind::LoadUpwardDylib);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylibCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Info {
            LoadCommandString Name;

            uint32_t Timestamp;
            uint32_t CurrentVersion;
            uint32_t CompatibilityVersion;

            [[nodiscard]] constexpr inline
            PackedVersion getCurrentVersion(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CurrentVersion, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            PackedVersion getCompatVersion(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CompatibilityVersion, IsBigEndian);
            }

            constexpr inline Info &
            setCurrentVersion(const PackedVersion &Version,
                              bool IsBigEndian) noexcept
            {
                const auto Value = Version.value();
                this->CurrentVersion = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr inline Info &
            setCompatVersion(const PackedVersion &Version,
                             bool IsBigEndian) noexcept
            {
                const auto Value = Version.value();
                this->CompatibilityVersion = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }
        };

        Info Info;

        [[nodiscard]]
        inline bool IsNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Info.Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct SubFrameworkCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubFramework);
        }

        [[nodiscard]] constexpr inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubFrameworkCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        inline bool IsUmbrellaOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Umbrella.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetUmbrella(bool IsBigEndian) const noexcept;
    };

    struct SubClientCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubClient);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubClientCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Client;

        [[nodiscard]]
        inline bool IsClientOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Client.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetClient(bool IsBigEndian) const noexcept;
    };

    struct SubUmbrellaCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubUmbrella);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubUmbrellaCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        inline bool IsUmbrellaOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Umbrella.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetUmbrella(bool IsBigEndian) const noexcept;
    };

    struct SubLibraryCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubLibrary);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Library;

        [[nodiscard]]
        inline bool IsLibraryOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Library.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLibrary(bool IsBigEndian) const noexcept;
    };

    struct PreBoundDylibCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PreBoundDylib);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PreBoundDylibCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Name;
        uint32_t NModules;
        LoadCommandString LinkedModules;

        [[nodiscard]] bool IsNameOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] inline
        bool IsLinkedModulesOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLinkedModules(bool IsBigEndian) const noexcept;
    };

    struct DylinkerCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::IdDylinker) ||
                (Kind == LoadCommand::Kind::LoadDylinker) ||
                (Kind == LoadCommand::Kind::DyldEnvironment);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylinkerCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Name;

        [[nodiscard]]
        inline bool IsNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct ThreadCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::Thread) ||
                (Kind == LoadCommand::Kind::UnixThread);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(ThreadCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }
    };

    struct RoutinesCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t InitAddress;
        uint32_t InitRoutine;

        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;
        uint32_t Reserved4;
        uint32_t Reserved5;
        uint32_t Reserved6;
    };

    struct RoutinesCommand64 : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint64_t InitAddress;
        uint64_t InitRoutine;

        uint64_t Reserved1;
        uint64_t Reserved2;
        uint64_t Reserved3;
        uint64_t Reserved4;
        uint64_t Reserved5;
        uint64_t Reserved6;
    };

    enum class SymbolTableEntryInfoMasks : uint8_t {
        IsExternal = 0x1,
        SymbolKind = 0xe,
        IsPrivateExternal = 0x10,
        IsDebugSymbol     = 0xe0
    };

    using SymbolTableEntryInfoMasksHandler =
        BasicMasksHandler<SymbolTableEntryInfoMasks>;

    enum class SymbolTableEntrySymbolKind : uint8_t {
        Undefined,
        Absolute          = 2,
        Indirect          = 10,
        PreboundUndefined = 12,
        Section           = 14,
    };

    constexpr std::string_view
    SymbolTableEntrySymbolKindGetName(
        SymbolTableEntrySymbolKind Kind) noexcept
    {
        switch (Kind) {
            case SymbolTableEntrySymbolKind::Undefined:
                return "Undefined"sv;
            case SymbolTableEntrySymbolKind::Absolute:
                return "Absolute"sv;
            case SymbolTableEntrySymbolKind::Indirect:
                return "Indirect"sv;
            case SymbolTableEntrySymbolKind::PreboundUndefined:
                return "Prebound-Undefined"sv;
            case SymbolTableEntrySymbolKind::Section:
                return "Section"sv;
        }

        return EmptyStringValue;
    }

    [[nodiscard]]
    constexpr uint64_t SymbolTableEntrySymbolKindGetLongestName() noexcept {
        const auto Result =
            EnumHelper<SymbolTableEntrySymbolKind>::GetLongestAssocLength(
                SymbolTableEntrySymbolKindGetName);

        return Result;
    }

    struct SymbolTableEntryInfo : private SymbolTableEntryInfoMasksHandler {
        using Masks = SymbolTableEntryInfoMasks;
        using SymbolKind = SymbolTableEntrySymbolKind;

        [[nodiscard]] inline SymbolKind getKind() const noexcept {
            return SymbolKind(getValueForMask(Masks::SymbolKind));
        }

        [[nodiscard]] inline bool IsExternal() const noexcept {
            return hasValueForMask(Masks::IsExternal);
        }

        [[nodiscard]] inline bool IsPrivateExternal() const noexcept {
            return hasValueForMask(Masks::IsPrivateExternal);
        }

        [[nodiscard]] inline bool IsDebugSymbol() const noexcept {
            return hasValueForMask(Masks::IsDebugSymbol);
        }

        inline SymbolTableEntryInfo &setKind(SymbolKind Kind) noexcept {
            const auto Value = static_cast<uint8_t>(Kind);
            setValueForMask(Masks::SymbolKind, Value);

            return *this;
        }

        inline SymbolTableEntryInfo &setIsExternal(bool Value) noexcept {
            setValueForMask(Masks::IsExternal, Value);
            return *this;
        }

        inline SymbolTableEntryInfo &setIsPrivateExternal(bool Value) noexcept {
            setValueForMask(Masks::IsPrivateExternal, Value);
            return *this;
        }

        inline SymbolTableEntryInfo &setIsDebugSymbol(bool Value) noexcept {
            setValueForMask(Masks::IsDebugSymbol, Value);
            return *this;
        }
    };

    [[nodiscard]] constexpr
    static inline uint16_t GetDylibOrdinal(uint16_t Desc) noexcept {
        return ((Desc >> 8) & 0xff);
    }

    constexpr static inline
    uint16_t SetDylibOrdinal(uint16_t &Desc, uint16_t Ordinal) noexcept {
        Desc |= ((Ordinal << 8) & 0xff00);
        return Desc;
    }

    struct SymbolTableEntry32 {
        union {
    #ifndef __LP64__
            char *Name;
    #endif
            uint32_t Index;
        };

        SymbolTableEntryInfo Info;
        uint8_t Section;
        int16_t Desc;
        uint32_t Value;

        [[nodiscard]]
        constexpr inline uint16_t getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }

        inline SymbolTableEntry32 &
        setDylibOrdinal(uint16_t DylibOrdinal, bool IsBigEndian) noexcept {
            auto Desc = static_cast<uint16_t>(getDesc(IsBigEndian));

            SetDylibOrdinal(Desc, DylibOrdinal);
            setDesc(Desc, IsBigEndian);

            return *this;
        }

        [[nodiscard]]
        inline uint32_t getIndex(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Index, IsBigEndian);
        }

        [[nodiscard]]
        inline uint8_t getSection(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Section, IsBigEndian);
        }

        [[nodiscard]] inline int16_t getDesc(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Desc, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getValue(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Value, IsBigEndian);
        }

        inline SymbolTableEntry32 &
        setIndex(uint32_t Value, bool IsBigEndian) noexcept {
            Index = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry32 &
        setSection(uint8_t Value, bool IsBigEndian) noexcept {
            Section = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline
        SymbolTableEntry32 &setDesc(int16_t Value, bool IsBigEndian) noexcept {
            Desc = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry32 &
        setValue(uint32_t Value, bool IsBigEndian) noexcept {
            Value = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymbolTableEntry64 {
        uint32_t Index;
        SymbolTableEntryInfo Info;
        uint8_t Section;
        uint16_t Desc;
        uint64_t Value;

        [[nodiscard]]
        constexpr inline uint16_t getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }

        constexpr inline SymbolTableEntry64 &
        setDylibOrdinal(uint16_t DylibOrdinal, bool IsBigEndian) noexcept {
            auto Desc = getDesc(IsBigEndian);

            SetDylibOrdinal(Desc, DylibOrdinal);
            setDesc(Desc, IsBigEndian);

            return *this;
        }

        [[nodiscard]]
        inline uint32_t getIndex(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Index, IsBigEndian);
        }

        [[nodiscard]]
        inline uint8_t getSection(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Section, IsBigEndian);
        }

        [[nodiscard]] inline uint16_t getDesc(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Desc, IsBigEndian);
        }

        [[nodiscard]]
        inline uint64_t getValue(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Value, IsBigEndian);
        }

        inline SymbolTableEntry64 &
        setIndex(uint32_t Value, bool IsBigEndian) noexcept {
            Index = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry64 &
        setSection(uint8_t Value, bool IsBigEndian) noexcept {
            Section = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline
        SymbolTableEntry64 &setDesc(uint16_t Value, bool IsBigEndian) noexcept {
            Desc = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry64 &
        setValue(uint64_t Value, bool IsBigEndian) noexcept {
            Value = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymTabCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolTable);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]]
        constexpr static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymTabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(SymTabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t SymOff;
        uint32_t Nsyms;
        uint32_t StrOff;
        uint32_t StrSize;

        [[nodiscard]]
        inline uint32_t getSymbolTableOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SymOff, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getSymbolsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsyms, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getStringTableOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StrOff, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getStringTableSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StrSize, IsBigEndian);
        }

        [[nodiscard]] inline SymTabCommand &
        setSymbolTableOffset(uint32_t Value, bool IsBigEndian) noexcept {
            SymOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline SymTabCommand &
        setSymbolsCount(uint32_t Value, bool IsBigEndian) noexcept {
            Nsyms = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline SymTabCommand &
        setStringTableOffset(uint32_t Value, bool IsBigEndian) noexcept {
            StrOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline SymTabCommand &
        setStringTableSize(uint32_t Value, bool IsBigEndian) noexcept {
            StrSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        using Entry32 = SymbolTableEntry32;
        using Entry64 = SymbolTableEntry64;

        using Entry32List = BasicContiguousList<Entry32>;
        using Entry64List = BasicContiguousList<Entry64>;

        using ConstEntry32List = BasicContiguousList<const Entry32>;
        using ConstEntry64List = BasicContiguousList<const Entry64>;

        [[nodiscard]] TypedAllocationOrError<Entry32List, SizeRangeError>
        GetEntry32List(const MemoryMap &Map, bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry64List, SizeRangeError>
        GetEntry64List(const MemoryMap &Map, bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry32List, SizeRangeError>
        GetConstEntry32List(const ConstMemoryMap &Map,
                            bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry64List, SizeRangeError>
        GetConstEntry64List(const ConstMemoryMap &Map,
                            bool IsBigEndian) const noexcept;
    };

    struct DynamicSymTabCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::DynamicSymbolTable);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DynamicSymTabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(DynamicSymTabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t ILocalSymbols;
        uint32_t NLocalSymbols;

        uint32_t IExternallyDefinedSymbols;
        uint32_t NExternallyDefinedSymbols;

        uint32_t IUndefinedSymbols;
        uint32_t NUndefinedSymbols;

        uint32_t TableOfContentsOff;
        uint32_t NTableOfContentsEntries;

        uint32_t ModuleTableOff;
        uint32_t NModuleTableEntries;

        uint32_t ExternalReferenceSymbolTableoff;
        uint32_t NExtReferencedSymbols;

        uint32_t IndirectSymbolTableOff;
        uint32_t NIndirectSymbols;

        uint32_t ExternalRelocationsOff;
        uint32_t NExternalRelocations;

        uint32_t LocalRelocationsOff;
        uint32_t NLocalRelocations;

        [[nodiscard]]
        inline uint32_t getLocalSymbolsIndex(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ILocalSymbols, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getLocalSymbolsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NLocalSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getExternalSymbolsIndex(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getExternalSymbolsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getUndefinedSymbolsIndex(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IUndefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getUndefinedSymbolsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NUndefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getTableOfContentsOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(TableOfContentsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getTableOfContentsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NTableOfContentsEntries, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getModuleTableOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ModuleTableOff, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getModuleTableCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NModuleTableEntries, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalRefSymbolTableOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExternalReferenceSymbolTableoff, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalRefSymbolTableCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NExtReferencedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getIndirectSymbolTableOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IndirectSymbolTableOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getIndirectSymbolTableCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NIndirectSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getExternalRelocationsOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExternalRelocationsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getExternalRelocationsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NExternalRelocations, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getLocalRelocationsOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(LocalRelocationsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getLocalRelocationsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NLocalRelocations, IsBigEndian);
        }

        inline DynamicSymTabCommand &
        setLocalSymbolsIndex(uint32_t Value, bool IsBigEndian) noexcept {
            ILocalSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }


        inline DynamicSymTabCommand &
        setLocalSymbolsCount(uint32_t Value, bool IsBigEndian) noexcept {
            NLocalSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalSymbolsIndex(uint32_t Value, bool IsBigEndian) noexcept {
            IExternallyDefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalSymbolsCount(uint32_t Value, bool IsBigEndian) noexcept {
            NExternallyDefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setUndefinedSymbolsIndex(uint32_t Value, bool IsBigEndian) noexcept {
            IUndefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setUndefinedSymbolsCount(uint32_t Value, bool IsBigEndian) noexcept {
            NUndefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setTableOfContentsOffset(uint32_t Value, bool IsBigEndian) noexcept {
            TableOfContentsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setTableOfContentsCount(uint32_t Value, bool IsBigEndian) noexcept {
            NTableOfContentsEntries = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }


        inline DynamicSymTabCommand &
        getModuleTableOffset(uint32_t Value, bool IsBigEndian) noexcept {
            ModuleTableOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }


        inline DynamicSymTabCommand &
        getModuleTableCount(uint32_t Value, bool IsBigEndian) noexcept {
            NModuleTableEntries = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        getExternslRefSymbolTableOffset(uint32_t Value,
                                        bool IsBigEndian) noexcept
        {
            ExternalReferenceSymbolTableoff =
                SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        inline DynamicSymTabCommand &
        getExternslRefSymbolTableCount(uint32_t Value,
                                       bool IsBigEndian) noexcept
        {
            NExtReferencedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setIndirectSymbolTableOffset(uint32_t Value,
                                     bool IsBigEndian) noexcept
        {
            IndirectSymbolTableOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setIndirectSymbolTableCount(uint32_t Value, bool IsBigEndian) noexcept {
            NIndirectSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalRelocationsOffset(uint32_t Value,
                                     bool IsBigEndian) noexcept
        {
            ExternalRelocationsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalRelocationsCount(uint32_t Value, bool IsBigEndian) noexcept {
            NExternalRelocations = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setLocalRelocationsOffset(uint32_t Value, bool IsBigEndian) noexcept {
            LocalRelocationsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setLocalRelocationsCount(uint32_t Value, bool IsBigEndian) noexcept {
            NLocalRelocations = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }


        using Entry32 = SymbolTableEntry32;
        using Entry64 = SymbolTableEntry64;

        using Entry32List = BasicContiguousList<Entry32>;
        using Entry64List = BasicContiguousList<Entry64>;

        using ConstEntry32List = BasicContiguousList<const Entry32>;
        using ConstEntry64List = BasicContiguousList<const Entry64>;

        [[nodiscard]] TypedAllocationOrError<Entry32List, SizeRangeError>
        GetExport32List(const MemoryMap &Map,
                        uint32_t SymOff,
                        bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry64List, SizeRangeError>
        GetExport64List(const MemoryMap &Map,
                        uint32_t SymOff,
                        bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry32List, SizeRangeError>
        GetLocalSymbol32List(const MemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry64List, SizeRangeError>
        GetLocalSymbol64List(const MemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry32List, SizeRangeError>
        GetConstExport32List(const ConstMemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry64List, SizeRangeError>
        GetConstExport64List(const ConstMemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry32List, SizeRangeError>
        GetConstLocalSymbol32List(const ConstMemoryMap &Map,
                                  uint32_t SymOff,
                                  bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry64List, SizeRangeError>
        GetConstLocalSymbol64List(const ConstMemoryMap &Map,
                                  uint32_t SymOff,
                                  bool IsBigEndian) const noexcept;

        [[nodiscard]]
        TypedAllocationOrError<BasicContiguousList<uint32_t>, SizeRangeError>
        GetIndirectSymbolIndexTable(const MemoryMap &Map,
                                    bool IsBigEndian) noexcept;

        [[nodiscard]]
        TypedAllocationOrError<BasicContiguousList<const uint32_t>,
                               SizeRangeError>

        GetConstIndirectSymbolIndexTable(const ConstMemoryMap &Map,
                                         bool IsBigEndian) noexcept;

    };

    constexpr static auto IndirectSymbolLocal = 0x80000000;
    constexpr static auto IndirectSymbolAbsolute = 0x40000000;

    struct TwoLevelHintsCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::TwoLevelHints);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Hint {
            uint32_t SubImagesIndex       : 8,
                     TableOfContentsIndex : 24;
        };

        using HintList = BasicContiguousList<Hint>;
        using ConstHintList = BasicContiguousList<const Hint>;

        uint32_t Offset;
        uint32_t NHints;

        [[nodiscard]] TypedAllocationOrError<HintList, SizeRangeError>
        GetHintList(const MemoryMap &Map, bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstHintList, SizeRangeError>
        GetConstHintList(const ConstMemoryMap &Map,
                         bool IsBigEndian) const noexcept;
    };

    struct PrebindChecksumCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PrebindChecksum);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t CheckSum;
    };

    struct UuidCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Uuid);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint8_t Uuid[16];
    };

    struct RpathCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Rpath);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RpathCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Path;

        [[nodiscard]]
        inline bool IsPathOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Path.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetPath(bool IsBigEndian) const noexcept;
    };

    struct LinkeditDataCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::CodeSignature) ||
                (Kind == LoadCommand::Kind::SegmentSplitInfo) ||
                (Kind == LoadCommand::Kind::DataInCode) ||
                (Kind == LoadCommand::Kind::FunctionStarts) ||
                (Kind == LoadCommand::Kind::DylibCodeSignDRS) ||
                (Kind == LoadCommand::Kind::LinkerOptimizationHint) ||
                (Kind == LoadCommand::Kind::DyldExportsTrie) ||
                (Kind == LoadCommand::Kind::DyldChainedFixups);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t DataOff;
        uint32_t DataSize;

        [[nodiscard]] TypedAllocationOrError<ExportTrieList, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map, bool IsBigEndian) noexcept {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = SwitchEndianIf(DataOff, IsBigEndian);
            const auto Size = SwitchEndianIf(DataSize, IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstExportTrieList, SizeRangeError>

        GetConstExportTrieList(const ConstMemoryMap &Map,
                               bool IsBigEndian) const noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = SwitchEndianIf(DataOff, IsBigEndian);
            const auto Size = SwitchEndianIf(DataSize, IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ExportTrieExportList, SizeRangeError>

        GetExportTrieExportList(const MemoryMap &Map,
                                bool IsBigEndian) noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = SwitchEndianIf(DataOff, IsBigEndian);
            const auto Size = SwitchEndianIf(DataSize, IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>

        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     bool IsBigEndian) const noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = SwitchEndianIf(DataOff, IsBigEndian);
            const auto Size = SwitchEndianIf(DataSize, IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]] SizeRangeError
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            bool IsBigEndian,
            std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = SwitchEndianIf(DataOff, IsBigEndian);
            const auto Size = SwitchEndianIf(DataSize, IsBigEndian);
            const auto Result =
                ::MachO::GetExportListFromExportTrie(Map,
                                                     Offset,
                                                     Size,
                                                     ExportListOut);

            return Result;
        }
    };

    struct EncryptionInfoCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;
    };

    struct EncryptionInfoCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;
        uint32_t Pad;
    };

    struct VersionMinimumCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::VersionMinimumMacOSX) ||
                (Kind == LoadCommand::Kind::VersionMinimumIPhoneOS) ||
                (Kind == LoadCommand::Kind::VersionMinimumTvOS) ||
                (Kind == LoadCommand::Kind::VersionMinimumWatchOS);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t Version;
        uint32_t Sdk;

        [[nodiscard]] constexpr
        inline PackedVersion getVersion(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Version, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline PackedVersion getSdk(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Sdk, IsBigEndian);
        }

        constexpr inline VersionMinimumCommand &
        setVersion(const PackedVersion &Version, bool IsBigEndian) noexcept {
            this->Version = SwitchEndianIf(Version.value(), IsBigEndian);
            return *this;
        }

        constexpr inline VersionMinimumCommand &
        setSdk(const PackedVersion &Sdk, bool IsBigEndian) noexcept {
            this->Sdk = SwitchEndianIf(Sdk.value(), IsBigEndian);
            return *this;
        }
    };

    enum class PlatformKind : uint32_t {
        macOS = 1,
        iOS,
        tvOS,
        watchOS,
        bridgeOS,
        iOSMac,
        macCatalyst = iOSMac,
        iOSSimulator,
        tvOSSimulator,
        watchOSSimulator,
        DriverKit
    };

    template <PlatformKind>
    struct PlatformKindInfo {};

    template <>
    struct PlatformKindInfo<PlatformKind::macOS> {
        constexpr static const auto Kind = PlatformKind::macOS;

        constexpr static const auto Name = std::string_view("PLATFORM_MACOS");
        constexpr static const auto Description = std::string_view("macOS");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::iOS> {
        constexpr static const auto Kind = PlatformKind::iOS;

        constexpr static const auto Name = std::string_view("PLATFORM_IOS");
        constexpr static const auto Description = std::string_view("iOS");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::tvOS> {
        constexpr static const auto Kind = PlatformKind::tvOS;

        constexpr static const auto Name =
            std::string_view("PLATFORM_TVOS");
        constexpr static const auto Description = std::string_view("tvOS");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::watchOS> {
        constexpr static const auto Kind = PlatformKind::watchOS;

        constexpr static const auto Name =
            std::string_view("PLATFORM_WATCHOS");
        constexpr static const auto Description =
            std::string_view("watchOS");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::bridgeOS> {
        constexpr static const auto Kind = PlatformKind::bridgeOS;

        constexpr static const auto Name =
            std::string_view("PLATFORM_BRDIGEOS");
        constexpr static const auto Description =
            std::string_view("bridgeOS");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::iOSMac> {
        constexpr static const auto Kind = PlatformKind::iOSMac;

        constexpr static const auto Name =
            std::string_view("PLATFORM_IOSMAC");
        constexpr static const auto Description =
            std::string_view("iOSMac");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::iOSSimulator> {
        constexpr static const auto Kind = PlatformKind::iOSSimulator;

        constexpr static const auto Name =
            std::string_view("PLATFORM_IOSSIMULATOR");
        constexpr static const auto Description =
            std::string_view("iOS Simulator");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::tvOSSimulator> {
        constexpr static const auto Kind = PlatformKind::tvOSSimulator;

        constexpr static const auto Name =
            std::string_view("PLATFORM_TVOSSIMULATOR");
        constexpr static const auto Description =
            std::string_view("tvOS Simulator");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::watchOSSimulator> {
        constexpr static const auto Kind = PlatformKind::watchOSSimulator;

        constexpr static const auto Name =
            std::string_view("PLATFORM_WATCHOSSIMULATOR");
        constexpr static const auto Description =
            std::string_view("watchOS Simulator");
    };

    template <>
    struct PlatformKindInfo<PlatformKind::DriverKit> {
        constexpr static const auto Kind = PlatformKind::DriverKit;

        constexpr static const auto Name =
            std::string_view("PLATFORM_DRIVERKIT");
        constexpr static const auto Description =
            std::string_view("DriverKit");
    };

    [[nodiscard]] constexpr const std::string_view &
    PlatformKindGetName(PlatformKind Kind) noexcept {
        using Enum = PlatformKind;
        switch (Kind) {
            case PlatformKind::macOS:
                return PlatformKindInfo<Enum::macOS>::Name;
            case PlatformKind::iOS:
                return PlatformKindInfo<Enum::iOS>::Name;
            case PlatformKind::tvOS:
                return PlatformKindInfo<Enum::tvOS>::Name;
            case PlatformKind::watchOS:
                return PlatformKindInfo<Enum::watchOS>::Name;
            case PlatformKind::bridgeOS:
                return PlatformKindInfo<Enum::bridgeOS>::Name;
            case PlatformKind::iOSMac:
                return PlatformKindInfo<Enum::iOSMac>::Name;
            case PlatformKind::iOSSimulator:
                return PlatformKindInfo<Enum::iOSSimulator>::Name;
            case PlatformKind::tvOSSimulator:
                return PlatformKindInfo<Enum::tvOSSimulator>::Name;
            case PlatformKind::watchOSSimulator:
                return PlatformKindInfo<Enum::watchOSSimulator>::Name;
            case PlatformKind::DriverKit:
                return PlatformKindInfo<Enum::DriverKit>::Name;
        }

        return EmptyStringValue;
    }

    [[nodiscard]] constexpr const std::string_view &
    PlatformKindGetDescription(PlatformKind Kind) noexcept {
        using Enum = PlatformKind;
        switch (Kind) {
            case PlatformKind::macOS:
                return PlatformKindInfo<Enum::macOS>::Description;
            case PlatformKind::iOS:
                return PlatformKindInfo<Enum::iOS>::Description;
            case PlatformKind::tvOS:
                return PlatformKindInfo<Enum::tvOS>::Description;
            case PlatformKind::watchOS:
                return PlatformKindInfo<Enum::watchOS>::Description;
            case PlatformKind::bridgeOS:
                return PlatformKindInfo<Enum::bridgeOS>::Description;
            case PlatformKind::iOSMac:
                return PlatformKindInfo<Enum::iOSMac>::Description;
            case PlatformKind::iOSSimulator:
                return PlatformKindInfo<Enum::iOSSimulator>::Description;
            case PlatformKind::tvOSSimulator:
                return PlatformKindInfo<Enum::tvOSSimulator>::Description;
            case PlatformKind::watchOSSimulator:
                return PlatformKindInfo<Enum::watchOSSimulator>::Description;
            case PlatformKind::DriverKit:
                return PlatformKindInfo<Enum::DriverKit>::Description;
        }

        return EmptyStringValue;
    }

    struct BuildVersionCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::BuildVersion);
        }

        struct Tool {
            enum class Kind {
                Clang = 1,
                Swift,
                Ld,
            };

            template <Kind>
            struct KindInfo {};

            template <>
            struct KindInfo<Kind::Clang> {
                constexpr static const auto Kind = Kind::Clang;

                constexpr static const auto Name =
                    std::string_view("TOOL_CLANG");
                constexpr static const auto Description =
                    std::string_view("Clang");
            };

            template <>
            struct KindInfo<Kind::Swift> {
                constexpr static const auto Kind = Kind::Swift;

                constexpr static const auto Name =
                    std::string_view("TOOL_SWIFT");
                constexpr static const auto Description =
                    std::string_view("Swift");
            };

            template <>
            struct KindInfo<Kind::Ld> {
                constexpr static const auto Kind = Kind::Ld;
                constexpr static const auto Name =
                    std::string_view("TOOL_LD");
                constexpr static const auto Description =
                    std::string_view("ld");
            };

            uint32_t Kind;
            uint32_t Version;

            [[nodiscard]] constexpr
            inline enum Kind getKind(bool IsBigEndian) const noexcept {
                const auto Integer = SwitchEndianIf(Kind, IsBigEndian);
                return static_cast<enum Kind>(Integer);
            }

            [[nodiscard]] constexpr static const std::string_view &
            KindGetName(enum Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::Clang:
                        return KindInfo<Kind::Clang>::Name;
                    case Kind::Swift:
                        return KindInfo<Kind::Swift>::Name;
                    case Kind::Ld:
                        return KindInfo<Kind::Ld>::Name;
                }

                return EmptyStringValue;
            }

            [[nodiscard]] constexpr static const std::string_view &
            KindGetDescription(enum Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::Clang:
                        return KindInfo<Kind::Clang>::Description;
                    case Kind::Swift:
                        return KindInfo<Kind::Swift>::Description;
                    case Kind::Ld:
                        return KindInfo<Kind::Ld>::Description;
                }

                return EmptyStringValue;
            }

            [[nodiscard]] constexpr
            PackedVersion getVersion(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Version, IsBigEndian);
            }
        };

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            const auto InvalidType =
                HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));

            if (InvalidType != LoadCommand::CmdSizeInvalidType::None) {
                return InvalidType;
            }

            auto ExpectedSize = uint32_t();
            if (DoesMultiplyAndAddOverflow(sizeof(Tool), NTools, sizeof(*this),
                                           &ExpectedSize))
            {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > ExpectedSize) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            } else if (CmdSize < ExpectedSize) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(BuildVersionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t Platform;

        uint32_t MinOS;
        uint32_t Sdk;
        uint32_t NTools;

        [[nodiscard]] constexpr
        inline PlatformKind getPlatform(bool IsBigEndian) const noexcept {
            return PlatformKind(SwitchEndianIf(Platform, IsBigEndian));
        }

        [[nodiscard]] constexpr
        inline PackedVersion getMinOS(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MinOS, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline PackedVersion getSdk(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Sdk, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getToolCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NTools, IsBigEndian);
        }

        constexpr inline BuildVersionCommand &
        setPlatform(PlatformKind Platform, bool IsBigEndian) noexcept {
            const auto Value = static_cast<uint32_t>(Platform);
            this->Platform = SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        constexpr inline BuildVersionCommand &
        setMinOS(const PackedVersion &MinOS, bool IsBigEndian) noexcept {
            this->MinOS = SwitchEndianIf(MinOS.value(), IsBigEndian);
            return *this;
        }

        constexpr inline BuildVersionCommand &
        setSdk(const PackedVersion &Sdk, bool IsBigEndian) noexcept {
            this->Sdk = SwitchEndianIf(Sdk.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline BuildVersionCommand &
        setToolCount(uint32_t Value, bool IsBigEndian) noexcept {
            NTools = SwitchEndianIf(NTools, IsBigEndian);
            return *this;
        }

        using ToolList = BasicContiguousList<Tool>;
        using ConstToolList = BasicContiguousList<const Tool>;

        [[nodiscard]] TypedAllocationOrError<ToolList, SizeRangeError>
        GetToolList(bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstToolList, SizeRangeError>
        GetConstToolList(bool IsBigEndian) const noexcept;
    };

    struct DyldInfoCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::DyldInfo) ||
                (Kind == LoadCommand::Kind::DyldInfoOnly);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t RebaseOff;
        uint32_t RebaseSize;

        uint32_t BindOff;
        uint32_t BindSize;

        uint32_t WeakBindOff;
        uint32_t WeakBindSize;

        uint32_t LazyBindOff;
        uint32_t LazyBindSize;

        uint32_t ExportOff;
        uint32_t ExportSize;

        [[nodiscard]] inline
        TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
        GetNakedBindOpcodeList(const MemoryMap &Map,
                               bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(BindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(BindSize, IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindOpcodeList, SizeRangeError>
        GetBindOpcodeList(const ConstMemoryMap &Map,
                          bool IsBigEndian,
                          bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(BindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(BindSize, IsBigEndian);

            return ::MachO::GetBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<LazyBindOpcodeList, SizeRangeError>
        GetLazyBindOpcodeList(const ConstMemoryMap &Map,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(LazyBindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(LazyBindSize, IsBigEndian);

            return ::MachO::GetLazyBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<WeakBindOpcodeList, SizeRangeError>
        GetWeakBindOpcodeList(const ConstMemoryMap &Map,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(WeakBindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(WeakBindSize, IsBigEndian);

            return ::MachO::GetWeakBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindActionList, SizeRangeError>
        GetBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &Collection,
                          bool IsBigEndian,
                          bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(BindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(BindSize, IsBigEndian);

            auto Result =
                ::MachO::GetBindActionList(Map,
                                           Collection,
                                           Offset,
                                           Size,
                                           Is64Bit);

            return Result;
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
        GetLazyBindNakedOpcodeList(const MemoryMap &Map,
                                   bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(LazyBindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(LazyBindSize, IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<LazyBindActionList, SizeRangeError>
        GetLazyBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(LazyBindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(LazyBindSize, IsBigEndian);

            auto Result =
                ::MachO::GetLazyBindActionList(Map,
                                               Collection,
                                               Offset,
                                               Size,
                                               Is64Bit);

            return Result;
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
        GetWeakBindNakedOpcodeList(const MemoryMap &Map,
                                   bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(WeakBindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(WeakBindSize, IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<WeakBindActionList, SizeRangeError>
        GetWeakBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(WeakBindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(WeakBindSize, IsBigEndian);

            auto Result =
                ::MachO::GetWeakBindActionList(Map,
                                               Collection,
                                               Offset,
                                               Size,
                                               Is64Bit);

            return Result;
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseNakedOpcodeList, SizeRangeError>
        GetRebaseNakedOpcodeList(const MemoryMap &Map,
                                 bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(BindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(BindSize, IsBigEndian);

            return ::MachO::GetRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstRebaseNakedOpcodeList, SizeRangeError>
        GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                      bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(BindOff, IsBigEndian);
            const auto Size = SwitchEndianIf(BindSize, IsBigEndian);

            return ::MachO::GetConstRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseActionList, SizeRangeError>
        GetRebaseActionList(const ConstMemoryMap &Map,
                            bool IsBigEndian,
                            bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(RebaseOff, IsBigEndian);
            const auto Size = SwitchEndianIf(RebaseSize, IsBigEndian);

            return ::MachO::GetRebaseActionList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseOpcodeList, SizeRangeError>
        GetRebaseOpcodeList(const ConstMemoryMap &Map,
                            bool IsBigEndian,
                            bool Is64Bit) const noexcept
        {
            const auto Offset = SwitchEndianIf(RebaseOff, IsBigEndian);
            const auto Size = SwitchEndianIf(RebaseSize, IsBigEndian);

            return ::MachO::GetRebaseOpcodeList(Map, Offset, Size, Is64Bit);
        }

        TypedAllocationOrError<ExportTrieList, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map, bool IsBigEndian) noexcept {
            const auto Offset = SwitchEndianIf(ExportOff, IsBigEndian);
            const auto Size = SwitchEndianIf(ExportSize, IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        TypedAllocationOrError<ConstExportTrieList, SizeRangeError>
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(ExportOff, IsBigEndian);
            const auto Size = SwitchEndianIf(ExportSize, IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        TypedAllocationOrError<ExportTrieExportList, SizeRangeError>
        GetExportTrieExportList(const MemoryMap &Map,
                                bool IsBigEndian) noexcept
        {
            const auto Offset = SwitchEndianIf(ExportOff, IsBigEndian);
            const auto Size = SwitchEndianIf(ExportSize, IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>
        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     bool IsBigEndian) const noexcept
        {
            const auto Offset = SwitchEndianIf(ExportOff, IsBigEndian);
            const auto Size = SwitchEndianIf(ExportSize, IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        SizeRangeError
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            bool IsBigEndian,
            std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
        {
            const auto Offset = SwitchEndianIf(ExportOff, IsBigEndian);
            const auto Size = SwitchEndianIf(ExportSize, IsBigEndian);
            const auto Result =
                ::MachO::GetExportListFromExportTrie(Map,
                                                     Offset,
                                                     Size,
                                                     ExportListOut);

            return Result;
        }
    };

    struct LinkerOptionCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::LinkerOption);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkerOptionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t Offset;
	    uint32_t Size;
    };

    struct SymbolSegmentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolSegment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t Offset;
        uint32_t Size;
    };

    struct IdentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Ident);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(IdentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }
    };

    struct FixedVMFileCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::FixedVMFile);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMFileCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Name;
        uint32_t HeaderAddr;

        [[nodiscard]]
        inline bool IsNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = SwitchEndianIf(this->CmdSize, IsBigEndian);
            return Name.IsOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct EntryPointCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Main);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint64_t EntryOffset;
        uint64_t StackSize;
    };

    enum class PackedVersion64Masks : uint64_t {
        Revision3 = 0x3ff,
        Revision2 = 0xffc00,
        Revision1 = 0x3ff00000,
        Minor = 0xffc0000000,
        Major = 0xffffff0000000000
    };

    enum class PackedVersion64Shifts : uint64_t {
        Revision3,
        Revision2 = 10,
        Revision1 = 20,
        Minor = 30,
        Major = 40
    };

    struct PackedVersion64 :
        public
            ::BasicMasksAndShiftsHandler<
                PackedVersion64Masks, PackedVersion64Shifts>
    {
    private:
        using Base =
            ::BasicMasksAndShiftsHandler<
                PackedVersion64Masks, PackedVersion64Shifts>;
    public:
        using Masks = PackedVersion64Masks;
        using Shifts = PackedVersion64Shifts;

        constexpr PackedVersion64() noexcept = default;
        constexpr PackedVersion64(uint64_t Integer) noexcept : Base(Integer) {}

        [[nodiscard]] constexpr inline uint16_t getRevision3() const noexcept {
            return getValueForMaskAndShift(Masks::Revision3, Shifts::Revision3);
        }

        [[nodiscard]] constexpr inline uint16_t getRevision2() const noexcept {
            return getValueForMaskAndShift(Masks::Revision2, Shifts::Revision2);
        }

        [[nodiscard]] constexpr inline uint16_t getRevision1() const noexcept {
            return getValueForMaskAndShift(Masks::Revision1, Shifts::Revision1);
        }

        [[nodiscard]] constexpr inline uint16_t getMinor() const noexcept {
            return getValueForMaskAndShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr inline uint32_t getMajor() const noexcept {
            const auto Value =
                getValueForMaskAndShift(Masks::Major, Shifts::Major);

            return static_cast<uint32_t>(Value);
        }

        constexpr
        inline PackedVersion64 &setRevision3(uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision3, Shifts::Revision3, Value);
            return *this;
        }

        constexpr
        inline PackedVersion64 &setRevision2(uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision2, Shifts::Revision2, Value);
            return *this;
        }

        constexpr
        inline PackedVersion64 &setRevision1(uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision1, Shifts::Revision1, Value);
            return *this;
        }

        constexpr inline PackedVersion64 &setMinor(uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr inline PackedVersion64 &setMajor(uint32_t Value) noexcept {
            setValueForMaskAndShift(Masks::Major, Shifts::Major, Value);
            return *this;
        }
    };

    struct SourceVersionCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SourceVersion);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint64_t Version;

        [[nodiscard]] constexpr
        inline PackedVersion64 getVersion(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Version, IsBigEndian);
        }

        constexpr inline SourceVersionCommand &
        setVersion(const PackedVersion64 &Version, bool IsBigEndian) noexcept {
            this->Version = SwitchEndianIf(Version.value(), IsBigEndian);
            return *this;
        }
    };

    struct NoteCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Note);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        char DataOwner[16];
        uint64_t Offset;
        uint64_t Size;
    };
}

template <typename T>
[[nodiscard]]
inline bool isa(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    return T::IsOfKind(LoadCmd.getKind(IsBigEndian));
}

template <typename T>
[[nodiscard]]
inline T &cast(MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<T>(LoadCmd, IsBigEndian));
    return reinterpret_cast<T &>(LoadCmd);
}

template <typename T>
[[nodiscard]] inline
const T &cast(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    assert(isa<T>(LoadCmd, IsBigEndian));
    return reinterpret_cast<const T &>(LoadCmd);
}

template <typename T>
[[nodiscard]]
inline T *dyn_cast(MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    if (isa<T>(LoadCmd, IsBigEndian)) {
        return reinterpret_cast<T *>(&LoadCmd);
    }

    return nullptr;
}

template <typename T>
[[nodiscard]] inline const T *
dyn_cast(const MachO::LoadCommand &LoadCmd, bool IsBigEndian) noexcept {
    if (isa<T>(LoadCmd, IsBigEndian)) {
        return reinterpret_cast<const T *>(&LoadCmd);
    }

    return nullptr;
}
