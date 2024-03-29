//
//  include/ADT/Mach-O/LoadCommands.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <cstring>

#include "ADT/BasicContiguousList.h"
#include "ADT/Dyld3/PackedVersion.h"
#include "ADT/Dyld3/PlatformKind.h"

#include "BindInfo.h"
#include "ExportTrie.h"
#include "LoadCommandsCommon.h"
#include "LoadCommandTemplates.h"
#include "RebaseInfo.h"
#include "Utils/SwitchEndian.h"

namespace MachO {
    struct LoadCommand {
        using Kind = LoadCommandKind;

        template <Kind Kind>
        [[nodiscard]] inline bool isa(const bool IsBigEndian) const noexcept {
            return (this->getKind(IsBigEndian) == Kind);
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline bool isa(const bool IsBigEndian) const noexcept {
            if (isa<First>(IsBigEndian)) {
                return true;
            }

            return isa<Second, Rest...>(IsBigEndian);
        }

        template <Kind Kind>
        [[nodiscard]] inline
        LoadCommandTypeFromKind<Kind> &cast(const bool IsBigEndian) noexcept {
            assert(isa<Kind>(IsBigEndian));
            return reinterpret_cast<LoadCommandTypeFromKind<Kind> &>(*this);
        }

        template <Kind Kind>
        [[nodiscard]] inline const LoadCommandTypeFromKind<Kind> &
        cast(const bool IsBigEndian) const noexcept {
            using Type = LoadCommandTypeFromKind<Kind>;

            assert(isa<Kind>(IsBigEndian));
            const auto &Result = reinterpret_cast<const Type &>(*this);

            return Result;
        }

        template <Kind Kind>
        [[nodiscard]] inline LoadCommandPtrTypeFromKind<Kind>
        dynCast(const bool IsBigEndian) noexcept {
            if (isa<Kind>(IsBigEndian)) {
                return &cast<Kind>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind Kind>
        [[nodiscard]] inline const LoadCommandConstPtrTypeFromKind<Kind>
        dynCast(const bool IsBigEndian) const noexcept {
            if (isa<Kind>(IsBigEndian)) {
                return &cast<Kind>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline LoadCommandPtrTypeFromKind<First>
        dynCast(const bool IsBigEndian) noexcept {
            if (isa<First, Second, Rest...>(IsBigEndian)) {
                return &cast<First>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline const LoadCommandConstPtrTypeFromKind<First>
        dynCast(const bool IsBigEndian) const noexcept {
            if (isa<First, Second, Rest...>(IsBigEndian)) {
                return &cast<First>(IsBigEndian);
            }

            return nullptr;
        }

        [[nodiscard]]
        static const std::string_view &KindGetName(Kind Kind) noexcept;

        [[nodiscard]]
        static const std::string_view &KindGetDescription(Kind Kind) noexcept;

        [[nodiscard]]
        constexpr static bool KindIsRequiredByDyld(const Kind Kind) noexcept {
            return (static_cast<uint32_t>(Kind) & KindRequiredByDyld);
        }

        [[nodiscard]]
        constexpr static bool KindIsRecognized(const Kind Kind) noexcept {
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
                case Kind::FileSetEntry:
                    return true;
            }

            return false;
        }

        [[nodiscard]]
        constexpr static bool KindIsSharedLibrary(const Kind Kind) noexcept {
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
                case Kind::FileSetEntry:
                    return false;
            }
        }

        enum class CmdSizeInvalidKind {
            None,
            TooSmall,
            TooLarge
        };

        [[nodiscard]]
        LoadCommand::CmdSizeInvalidKind
        ValidateCmdsize(const LoadCommand *LC, bool IsBigEndian) noexcept;

        uint32_t Cmd;
        uint32_t CmdSize;

        [[nodiscard]]
        constexpr uint32_t getCmd(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cmd, IsBigEndian);
        }

        [[nodiscard]]
        constexpr Kind getKind(const bool IsBigEndian) const noexcept {
            return Kind(getCmd(IsBigEndian));
        }

        [[nodiscard]]
        constexpr uint32_t getCmdSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CmdSize, IsBigEndian);
        }

        constexpr
        LoadCommand &setKind(const Kind Kind, const bool IsBigEndian) noexcept {
            const auto Value = static_cast<uint32_t>(Kind);
            this->Cmd = SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        [[nodiscard]] constexpr LoadCommand &
        setCmdSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CmdSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        bool hasRecognizedKind(const bool IsBigEndian) const noexcept {
            return KindIsRecognized(getKind(IsBigEndian));
        }

        [[nodiscard]]
        constexpr bool isRequiredByDyld(const bool IsBigEndian) const noexcept {
            return KindIsRequiredByDyld(getKind(IsBigEndian));
        }

        [[nodiscard]] constexpr
        bool isSharedLibraryKind(const bool IsBigEndian) const noexcept {
            return KindIsSharedLibrary(getKind(IsBigEndian));
        }
    };

    [[nodiscard]] constexpr bool
    SegOrSectNameEquals(const char *const SegOrSectName,
                        const std::string_view Rhs) noexcept
    {
        if (Rhs.length() > 16) {
            return false;
        }

        return (strncmp(Rhs.data(), SegOrSectName, Rhs.length()) == 0);
    }

    struct SegmentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        struct Section {
            char Name[16];
            char SegmentName[16];
            uint32_t Addr;
            uint32_t Size;
            uint32_t Offset;
            uint32_t Align;
            uint32_t Reloff;
            uint32_t Nreloc;
            uint32_t Flags;
            uint32_t Reserved1;
            uint32_t Reserved2;

            [[nodiscard]] inline std::string_view getName() const noexcept {
                return std::string_view(Name, strnlen(Name, sizeof(Name)));
            }

            [[nodiscard]]
            inline std::string_view getSegmentName() const noexcept {
                const auto Length = strnlen(SegmentName, sizeof(SegmentName));
                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr
            inline uint32_t getAddress(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Addr, IsBigEndian);
            }

            [[nodiscard]]
            constexpr uint32_t getSize(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getFileOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getAlign(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]] constexpr uint32_t
            getRelocationsCount(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Nreloc, IsBigEndian);
            }

            [[nodiscard]] constexpr SegmentSectionFlags
            getFlags(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Flags, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getReserved1(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved1, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getReserved2(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved2, IsBigEndian);
            }

            [[nodiscard]] constexpr std::optional<LocationRange>
            getFileRange(const bool IsBigEndian) const noexcept {
                const auto Offset = getFileOffset(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Offset, Size);
            }

            [[nodiscard]] constexpr std::optional<LocationRange>
            getMemoryRange(const bool IsBigEndian) const noexcept {
                const auto Addr = getAddress(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Addr, Size);
            }

            [[nodiscard]] constexpr
            bool segmentNameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->SegmentName, Name);
            }

            [[nodiscard]] constexpr
            bool nameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->Name, Name);
            }

            constexpr Section &
            setAddr(const uint32_t Value, const bool IsBigEndian) noexcept {
                this->Addr = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setSize(const uint32_t Value, const bool IsBigEndian) noexcept {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setFileOffset(const uint32_t Value,
                          const bool IsBigEndian) noexcept
            {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setAlign(const uint32_t Value, const bool IsBigEndian) noexcept {
                this->Align = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr Section &
            setRelocationsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
            {
                this->Nreloc = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setFlags(const SegmentSectionFlags &Flags,
                     const bool IsBigEndian) noexcept
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr Section &
            setReserved1(const uint32_t Value, const bool IsBigEndian) noexcept
            {
                this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr Section &
            setReserved2(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            {
                this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
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

        [[nodiscard]] inline std::string_view getName() const noexcept {
            return std::string_view(Name, strnlen(Name, sizeof(Name)));
        }

        [[nodiscard]]
        constexpr uint32_t getVmAddr(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getVmSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getFileOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getFileSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        MemoryProtections getInitProt(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InitProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        MemoryProtections getMaxProt(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MaxProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        SegmentFlags getFlags(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getSectionCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsects, IsBigEndian);
        }

        [[nodiscard]] constexpr std::optional<LocationRange>
        getVmRange(const bool IsBigEndian) const noexcept {
            const auto VmAddr = getVmAddr(IsBigEndian);
            const auto VmSize = getVmSize(IsBigEndian);

            return LocationRange::CreateWithSize(VmAddr, VmSize);
        }

        [[nodiscard]] constexpr std::optional<LocationRange>
        getFileRange(const bool IsBigEndian) const noexcept {
            const auto Offset = getFileOffset(IsBigEndian);
            const auto Size = getFileSize(IsBigEndian);

            return LocationRange::CreateWithSize(Offset, Size);
        }

        [[nodiscard]]
        constexpr bool nameEquals(const std::string_view Name) const noexcept {
            return SegOrSectNameEquals(this->Name, Name);
        }

        constexpr SegmentCommand &
        setInitProt(const MemoryProtections &InitProt,
                    const bool IsBigEndian) noexcept
        {
            this->InitProt = SwitchEndianIf(InitProt.value(), IsBigEndian);
            return *this;
        }

        constexpr SegmentCommand &
        setMaxProt(const MemoryProtections &MaxProt,
                   const bool IsBigEndian) noexcept
        {
            this->MaxProt = SwitchEndianIf(MaxProt.value(), IsBigEndian);
            return *this;
        }

        constexpr SegmentCommand &
        setFlags(const SegmentFlags &Flags, const bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        bool isSectionListValid(const bool IsBigEndian) const noexcept;

        [[nodiscard]] inline
        SectionList getSectionListUnsafe(const bool IsBigEndian) noexcept {
            const auto Ptr = reinterpret_cast<Section *>(this + 1);
            return SectionList(Ptr, getSectionCount(IsBigEndian));
        }

        [[nodiscard]] inline ConstSectionList
        getConstSectionListUnsafe(const bool IsBigEndian) const noexcept {
            const auto Ptr = reinterpret_cast<const Section *>(this + 1);
            return ConstSectionList(Ptr, getSectionCount(IsBigEndian));
        }
    };

    struct SegmentCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        struct Section {
            char Name[16];
            char SegmentName[16];
            uint64_t Addr;
            uint64_t Size;
            uint32_t Offset;
            uint32_t Align;
            uint32_t Reloff;
            uint32_t Nreloc;
            uint32_t Flags;
            uint32_t Reserved1;
            uint32_t Reserved2;

            [[nodiscard]] inline std::string_view getName() const noexcept {
                return std::string_view(Name, strnlen(Name, sizeof(Name)));
            }

            [[nodiscard]]
            inline std::string_view getSegmentName() const noexcept {
                const auto Length =
                    strnlen(SegmentName, sizeof(SegmentName));

                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr
            uint64_t getAddress(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Addr, IsBigEndian);
            }

            [[nodiscard]]
            constexpr uint64_t getSize(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getFileOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getAlign(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]] constexpr uint32_t
            getRelocationsCount(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Nreloc, IsBigEndian);
            }

            [[nodiscard]] constexpr SegmentSectionFlags
            getFlags(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Flags, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getReserved1(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved1, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getReserved2(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved2, IsBigEndian);
            }

            [[nodiscard]] constexpr std::optional<LocationRange>
            getFileRange(const bool IsBigEndian) const noexcept {
                const auto Offset = getFileOffset(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Offset, Size);
            }

            [[nodiscard]] constexpr std::optional<LocationRange>
            getMemoryRange(const bool IsBigEndian) const noexcept {
                const auto Addr = getAddress(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Addr, Size);
            }

            [[nodiscard]] constexpr
            bool segmentNameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->SegmentName, Name);
            }

            [[nodiscard]] constexpr
            bool nameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->Name, Name);
            }

            constexpr Section &
            setAddr(const uint64_t Value, const bool IsBigEndian) noexcept {
                this->Addr = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setSize(const uint64_t Value, const bool IsBigEndian) noexcept {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setFileOffset(const uint32_t Value,
                          const bool IsBigEndian) noexcept
            {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setAlign(const uint32_t Value, const bool IsBigEndian) noexcept {
                this->Align = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr Section &
            setRelocationsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
            {
                this->Nreloc = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Section &
            setFlags(const SegmentSectionFlags &Flags,
                     const bool IsBigEndian) noexcept
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr Section &
            setReserved1(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            {
                this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr Section &
            setReserved2(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            {
                this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
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

        [[nodiscard]] inline std::string_view getName() const noexcept {
            return std::string_view(Name, strnlen(Name, sizeof(Name)));
        }

        [[nodiscard]]
        constexpr uint64_t getVmAddr(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getVmSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getFileOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getFileSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        MemoryProtections getInitProt(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InitProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        MemoryProtections getMaxProt(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MaxProt, IsBigEndian);
        }

        [[nodiscard]] constexpr
        SegmentFlags getFlags(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getSectionCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsects, IsBigEndian);
        }

        [[nodiscard]] constexpr std::optional<LocationRange>
        getVmRange(const bool IsBigEndian) const noexcept {
            const auto VmAddr = getVmAddr(IsBigEndian);
            const auto VmSize = getVmSize(IsBigEndian);

            return LocationRange::CreateWithSize(VmAddr, VmSize);
        }

        [[nodiscard]] constexpr std::optional<LocationRange>
        getFileRange(const bool IsBigEndian) const noexcept {
            const auto Offset = getFileOffset(IsBigEndian);
            const auto Size = getFileSize(IsBigEndian);

            return LocationRange::CreateWithSize(Offset, Size);
        }

        [[nodiscard]]
        constexpr bool nameEquals(const std::string_view Name) const noexcept {
            return SegOrSectNameEquals(this->Name, Name);
        }

        constexpr SegmentCommand64 &
        setInitProt(const MemoryProtections &InitProt,
                    const bool IsBigEndian) noexcept
        {
            this->InitProt = SwitchEndianIf(InitProt.value(), IsBigEndian);
            return *this;
        }

        constexpr SegmentCommand64 &
        setMaxProt(const MemoryProtections &MaxProt,
                   const bool IsBigEndian) noexcept
        {
            this->MaxProt = SwitchEndianIf(MaxProt.value(), IsBigEndian);
            return *this;
        }

        constexpr SegmentCommand64 &
        setFlags(const SegmentFlags &Flags, const bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] bool isSectionListValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] inline
        SectionList getSectionListUnsafe(const bool IsBigEndian) noexcept {
            const auto Ptr = reinterpret_cast<Section *>(this + 1);
            return SectionList(Ptr, getSectionCount(IsBigEndian));
        }

        [[nodiscard]] ConstSectionList
        getConstSectionListUnsafe(const bool IsBigEndian) const noexcept {
            const auto Ptr = reinterpret_cast<const Section *>(this + 1);
            return ConstSectionList(Ptr, getSectionCount(IsBigEndian));
        }
    };

    template <typename Error>
    struct LoadCommandStringViewOrError {
    protected:
        std::string_view View;
        [[nodiscard]] inline uintptr_t getStorage() const noexcept {
            return reinterpret_cast<uintptr_t>(View.begin());
        }
    public:
        LoadCommandStringViewOrError(const std::string_view &View) noexcept
        : View(View) {}

        LoadCommandStringViewOrError(const Error Value) noexcept {
            View = std::string_view(reinterpret_cast<const char *>(Value), 1);
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            const auto HasError = PointerHasErrorValue(getStorage());
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

        [[nodiscard]]
        constexpr uint32_t getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        constexpr LoadCommandString &
        setOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Offset, IsBigEndian);
            return *this;
        }

        using GetValueResult = LoadCommandStringViewOrError<GetStringError>;

        [[nodiscard]] bool
        isOffsetValid(uint32_t MinSize,
                      uint32_t CmdSize,
                      bool IsBigEndian) const noexcept;

        [[nodiscard]]
        uint32_t GetLength(uint32_t CmdSize, bool IsBigEndian) const noexcept;
    };

    struct FixedVmSharedLibraryInfo {
        LoadCommandString Name;
        Dyld3::PackedVersion MinorVersion;
        uint32_t HeaderAddr;
    };

    struct FixedVMSharedLibraryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::IdFixedVMSharedLibrary) ||
                (Kind == LoadCommand::Kind::LoadFixedVMSharedLibrary);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMSharedLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        FixedVmSharedLibraryInfo Info;

        [[nodiscard]]
        inline bool isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto Offset = Info.Name.getOffset(IsBigEndian);
            return Info.Name.isOffsetValid(sizeof(*this), Offset, CmdSize);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct DylibCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::LoadDylib) ||
                (Kind == LoadCommand::Kind::IdDylib) ||
                (Kind == LoadCommand::Kind::LoadWeakDylib) ||
                (Kind == LoadCommand::Kind::ReexportDylib) ||
                (Kind == LoadCommand::Kind::LazyLoadDylib) ||
                (Kind == LoadCommand::Kind::LoadUpwardDylib);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylibCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        struct Info {
            LoadCommandString Name;

            uint32_t Timestamp;
            uint32_t CurrentVersion;
            uint32_t CompatibilityVersion;

            [[nodiscard]] constexpr
            uint32_t getNameOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Name.Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr Dyld3::PackedVersion
            getCurrentVersion(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CurrentVersion, IsBigEndian);
            }

            [[nodiscard]] constexpr Dyld3::PackedVersion
            getCompatVersion(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CompatibilityVersion, IsBigEndian);
            }

            [[nodiscard]] constexpr
            uint32_t getTimestamp(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Timestamp, IsBigEndian);
            }

            constexpr Info &
            setNameOffset(const uint32_t Value,
                          const bool IsBigEndian) noexcept
            {
                this->Name.Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr Info &
            setCurrentVersion(const Dyld3::PackedVersion &Version,
                              const bool IsBigEndian) noexcept
            {
                const auto Value = Version.value();
                this->CurrentVersion = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr Info &
            setCompatVersion(const Dyld3::PackedVersion &Version,
                             const bool IsBigEndian) noexcept
            {
                const auto Value = Version.value();
                this->CompatibilityVersion = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr Info &
            setTimestamp(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            {
                this->Timestamp = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }
        };

        Info Info;

        [[nodiscard]]
        inline bool isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Info.Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct SubFrameworkCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubFramework);
        }

        [[nodiscard]] constexpr LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubFrameworkCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Umbrella;

        [[nodiscard]] inline
        bool isUmbrellaOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Umbrella.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetUmbrella(bool IsBigEndian) const noexcept;
    };

    struct SubClientCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubClient);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubClientCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Client;

        [[nodiscard]]
        inline bool isClientOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Client.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetClient(bool IsBigEndian) const noexcept;
    };

    struct SubUmbrellaCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubUmbrella);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubUmbrellaCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        inline bool isUmbrellaOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Umbrella.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetUmbrella(bool IsBigEndian) const noexcept;
    };

    struct SubLibraryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubLibrary);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Library;

        [[nodiscard]] inline
        bool isLibraryOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Library.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLibrary(bool IsBigEndian) const noexcept;
    };

    struct PreBoundDylibCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PreBoundDylib);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PreBoundDylibCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Name;
        uint32_t NModules;
        LoadCommandString LinkedModules;

        [[nodiscard]] bool isNameOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] inline
        bool IsLinkedModulesOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLinkedModules(bool IsBigEndian) const noexcept;
    };

    struct DylinkerCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::IdDylinker) ||
                (Kind == LoadCommand::Kind::LoadDylinker) ||
                (Kind == LoadCommand::Kind::DyldEnvironment);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylinkerCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Name;

        [[nodiscard]]
        inline bool isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct ThreadCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::Thread) ||
                (Kind == LoadCommand::Kind::UnixThread);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(ThreadCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }
    };

    struct RoutinesCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
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
        [[nodiscard]]
        constexpr static bool IsOfKindc(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
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
        SectionDefined    = 14,
    };

    constexpr static std::string_view
    SymbolTableEntrySymbolKindGetName(
        const SymbolTableEntrySymbolKind Kind) noexcept
    {
        switch (Kind) {
            case SymbolTableEntrySymbolKind::Undefined:
                return "N_UNDF";
            case SymbolTableEntrySymbolKind::Absolute:
                return "N_ABS";
            case SymbolTableEntrySymbolKind::Indirect:
                return "N_INDR";
            case SymbolTableEntrySymbolKind::PreboundUndefined:
                return "N_PBUD";
            case SymbolTableEntrySymbolKind::SectionDefined:
                return "N_SECT";
        }

        return std::string_view();
    }

    constexpr static std::string_view
    SymbolTableEntrySymbolKindGetDescription(
        const SymbolTableEntrySymbolKind Kind) noexcept
    {
        switch (Kind) {
            case SymbolTableEntrySymbolKind::Undefined:
                return "Undefined";
            case SymbolTableEntrySymbolKind::Absolute:
                return "Absolute";
            case SymbolTableEntrySymbolKind::Indirect:
                return "Indirect";
            case SymbolTableEntrySymbolKind::PreboundUndefined:
                return "Prebound-Undefined";
            case SymbolTableEntrySymbolKind::SectionDefined:
                return "Section-defined";
        }

        return std::string_view();
    }

    [[nodiscard]]
    constexpr uint64_t SymbolTableEntrySymbolKindGetLongestName() noexcept {
        const auto Result =
            EnumHelper<SymbolTableEntrySymbolKind>::GetLongestAssocLength(
                SymbolTableEntrySymbolKindGetName);

        return Result;
    }

    [[nodiscard]] constexpr
    uint64_t SymbolTableEntrySymbolKindGetLongestDescription() noexcept {
        const auto Result =
            EnumHelper<SymbolTableEntrySymbolKind>::GetLongestAssocLength(
                SymbolTableEntrySymbolKindGetDescription);

        return Result;
    }

    struct SymbolTableEntryInfo : private SymbolTableEntryInfoMasksHandler {
        using Masks = SymbolTableEntryInfoMasks;
        using SymbolKind = SymbolTableEntrySymbolKind;

        [[nodiscard]] inline SymbolKind getKind() const noexcept {
            return SymbolKind(getValueForMask(Masks::SymbolKind));
        }

        [[nodiscard]] inline bool isUndefined() const noexcept {
            return (getKind() == SymbolKind::Undefined);
        }

        [[nodiscard]] inline bool isAbsolute() const noexcept {
            return (getKind() == SymbolKind::Absolute);
        }

        [[nodiscard]] inline bool isIndirect() const noexcept {
            return (getKind() == SymbolKind::Indirect);
        }

        [[nodiscard]] inline bool isPreboundUndefined() const noexcept {
            return (getKind() == SymbolKind::PreboundUndefined);
        }

        [[nodiscard]] inline bool isSectionDefined() const noexcept {
            return (getKind() == SymbolKind::SectionDefined);
        }

        [[nodiscard]] inline bool isExternal() const noexcept {
            return hasValueForMask(Masks::IsExternal);
        }

        [[nodiscard]] inline bool isPrivateExternal() const noexcept {
            return hasValueForMask(Masks::IsPrivateExternal);
        }

        [[nodiscard]] inline bool isDebugSymbol() const noexcept {
            return hasValueForMask(Masks::IsDebugSymbol);
        }

        inline SymbolTableEntryInfo &setKind(const SymbolKind Kind) noexcept {
            const auto Value = static_cast<uint8_t>(Kind);
            setValueForMask(Masks::SymbolKind, Value);

            return *this;
        }

        inline SymbolTableEntryInfo &setIsExternal(const bool Value) noexcept {
            setValueForMask(Masks::IsExternal, Value);
            return *this;
        }

        inline
        SymbolTableEntryInfo &setIsPrivateExternal(const bool Value) noexcept {
            setValueForMask(Masks::IsPrivateExternal, Value);
            return *this;
        }

        inline
        SymbolTableEntryInfo &setIsDebugSymbol(const bool Value) noexcept {
            setValueForMask(Masks::IsDebugSymbol, Value);
            return *this;
        }
    };

    [[nodiscard]]
    constexpr static uint16_t GetDylibOrdinal(const uint16_t Desc) noexcept {
        return ((Desc >> 8) & 0xff);
    }

    constexpr uint16_t
    SetDylibOrdinal(uint16_t &Desc, const uint16_t Ordinal) noexcept {
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
        constexpr uint16_t getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }

        inline SymbolTableEntry32 &
        setDylibOrdinal(const uint16_t DylibOrdinal,
                        const bool IsBigEndian) noexcept
        {
            auto Desc = static_cast<uint16_t>(getDescription(IsBigEndian));

            SetDylibOrdinal(Desc, DylibOrdinal);
            setDescription(Desc, IsBigEndian);

            return *this;
        }

        [[nodiscard]]
        inline uint32_t getIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Index, IsBigEndian);
        }

        [[nodiscard]] inline
        uint8_t getSectionOrdinal(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Section, IsBigEndian);
        }

        [[nodiscard]]
        inline int16_t getDescription(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Desc, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getValue(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Value, IsBigEndian);
        }

        inline SymbolTableEntry32 &
        setIndex(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Index = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry32 &
        setSectionOrdinal(const uint8_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->Section = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry32 &
        setDescription(const int16_t Value, const bool IsBigEndian) noexcept {
            this->Desc = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry32 &
        setValue(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Value = SwitchEndianIf(Value, IsBigEndian);
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
        constexpr uint16_t getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }

        constexpr SymbolTableEntry64 &
        setDylibOrdinal(const uint16_t DylibOrdinal,
                        const bool IsBigEndian) noexcept
        {
            auto Desc = getDescription(IsBigEndian);

            SetDylibOrdinal(Desc, DylibOrdinal);
            setDescription(Desc, IsBigEndian);

            return *this;
        }

        [[nodiscard]]
        inline uint32_t getIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Index, IsBigEndian);
        }

        [[nodiscard]] inline
        uint8_t getSectionOrdinal(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Section, IsBigEndian);
        }

        [[nodiscard]]
        inline uint16_t getDescription(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Desc, IsBigEndian);
        }

        [[nodiscard]]
        inline uint64_t getValue(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Value, IsBigEndian);
        }

        inline SymbolTableEntry64 &
        setIndex(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Index = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry64 &
        setSectionOrdinal(const uint8_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->Section = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry64 &
        setDescription(uint16_t Value, bool IsBigEndian) noexcept {
            this->Desc = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline SymbolTableEntry64 &
        setValue(uint64_t Value, bool IsBigEndian) noexcept {
            this->Value = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymTabCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolTable);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t SymOff;
        uint32_t Nsyms;
        uint32_t StrOff;
        uint32_t StrSize;

        [[nodiscard]] inline
        uint32_t getSymbolTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SymOff, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getSymbolCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsyms, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getStringTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StrOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getStringTableSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StrSize, IsBigEndian);
        }

        [[nodiscard]] inline SymTabCommand &
        setSymbolTableOffset(const uint32_t Value,
                             const bool IsBigEndian) noexcept
        {
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
        setStringTableSize(const uint32_t Value,
                           const bool IsBigEndian) noexcept
        {
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
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::DynamicSymbolTable);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DynamicSymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(DynamicSymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
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

        [[nodiscard]] inline
        uint32_t getLocalSymbolsIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ILocalSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getLocalSymbolsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NLocalSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalSymbolsIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IExternallyDefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalSymbolsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NExternallyDefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getUndefinedSymbolsIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IUndefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getUndefinedSymbolsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NUndefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getTableOfContentsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(TableOfContentsOff, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getTableOfContentsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NTableOfContentsEntries, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getModuleTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ModuleTableOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getModuleTableCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NModuleTableEntries, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalRefSymbolTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExternalReferenceSymbolTableoff, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalRefSymbolTableCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NExtReferencedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getIndirectSymbolTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IndirectSymbolTableOff, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getIndirectSymbolTableCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NIndirectSymbols, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getExternalRelocationsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExternalRelocationsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getExternalRelocationsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NExternalRelocations, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getLocalRelocationsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(LocalRelocationsOff, IsBigEndian);
        }

        [[nodiscard]] inline uint32_t
        getLocalRelocationsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NLocalRelocations, IsBigEndian);
        }

        inline DynamicSymTabCommand &
        setLocalSymbolsIndex(const uint32_t Value,
                             const bool IsBigEndian) noexcept
        {
            ILocalSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }


        inline DynamicSymTabCommand &
        setLocalSymbolsCount(const uint32_t Value,
                             const bool IsBigEndian) noexcept
        {
            NLocalSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalSymbolsIndex(const uint32_t Value,
                                const bool IsBigEndian) noexcept
        {
            IExternallyDefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalSymbolsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
        {
            NExternallyDefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setUndefinedSymbolsIndex(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
        {
            IUndefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setUndefinedSymbolsCount(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
        {
            NUndefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setTableOfContentsOffset(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
        {
            TableOfContentsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setTableOfContentsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
        {
            NTableOfContentsEntries = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        getModuleTableOffset(const uint32_t Value,
                             const bool IsBigEndian) noexcept
        {
            ModuleTableOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        getModuleTableCount(const uint32_t Value,
                            const bool IsBigEndian) noexcept
        {
            NModuleTableEntries = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        getExternslRefSymbolTableOffset(const uint32_t Value,
                                        const bool IsBigEndian) noexcept
        {
            ExternalReferenceSymbolTableoff =
                SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        inline DynamicSymTabCommand &
        getExternslRefSymbolTableCount(const uint32_t Value,
                                       const bool IsBigEndian) noexcept
        {
            NExtReferencedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setIndirectSymbolTableOffset(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
        {
            IndirectSymbolTableOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setIndirectSymbolTableCount(const uint32_t Value,
                                    const bool IsBigEndian) noexcept
        {
            NIndirectSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalRelocationsOffset(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
        {
            ExternalRelocationsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setExternalRelocationsCount(const uint32_t Value,
                                    const bool IsBigEndian) noexcept
        {
            NExternalRelocations = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setLocalRelocationsOffset(const uint32_t Value,
                                  const bool IsBigEndian) noexcept
        {
            LocalRelocationsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline DynamicSymTabCommand &
        setLocalRelocationsCount(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
        {
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
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::TwoLevelHints);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        struct Hint {
            uint32_t SubImagesIndex       : 8,
                     TableOfContentsIndex : 24;
        };

        using HintList = BasicContiguousList<Hint>;
        using ConstHintList = BasicContiguousList<const Hint>;

        uint32_t Offset;
        uint32_t NHints;

        [[nodiscard]] constexpr
        uint32_t getHintsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getHintsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NHints, IsBigEndian);
        }

        [[nodiscard]] TypedAllocationOrError<HintList, SizeRangeError>
        GetHintList(const MemoryMap &Map, bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstHintList, SizeRangeError>
        GetConstHintList(const ConstMemoryMap &Map,
                         bool IsBigEndian) const noexcept;

        constexpr TwoLevelHintsCommand &
        setHintsOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Offset =  SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr TwoLevelHintsCommand &
        setHintsCount(uint32_t Value, bool IsBigEndian) noexcept {
            this->NHints = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct PrebindChecksumCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PrebindChecksum);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t CheckSum;

        [[nodiscard]]
        constexpr uint32_t getCheckSum(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CheckSum, IsBigEndian);
        }

        [[nodiscard]] constexpr PrebindChecksumCommand &
        setCheckSum(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CheckSum = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct UuidCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Uuid);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint8_t Uuid[16];
    };

    struct RpathCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Rpath);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RpathCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Path;

        [[nodiscard]]
        inline bool isPathOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Path.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetPath(bool IsBigEndian) const noexcept;
    };

    struct LinkeditDataCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t DataOff;
        uint32_t DataSize;

        [[nodiscard]] constexpr
        uint32_t getDataOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(DataOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getDataSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(DataSize, IsBigEndian);
        }

        [[nodiscard]] constexpr LinkeditDataCommand &
        setDataOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->DataOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr LinkeditDataCommand &
        setDataSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->DataSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] TypedAllocationOrError<ExportTrieList, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map,
                          const bool IsBigEndian) noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstExportTrieList, SizeRangeError>
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               const bool IsBigEndian) const noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ExportTrieExportList, SizeRangeError>
        GetExportTrieExportList(const MemoryMap &Map,
                                const bool IsBigEndian) noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>

        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     const bool IsBigEndian) const noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]] SizeRangeError
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            const bool IsBigEndian,
            std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);
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
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;

        [[nodiscard]] constexpr
        uint32_t getCryptOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getCryptSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getCryptId(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptId, IsBigEndian);
        }

        [[nodiscard]] constexpr EncryptionInfoCommand &
        setCryptOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CryptOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr EncryptionInfoCommand &
        setCryptSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CryptSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr EncryptionInfoCommand &
        setCryptId(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CryptId = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct EncryptionInfoCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;
        uint32_t Pad;

        [[nodiscard]] constexpr
        uint32_t getCryptOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getCryptSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getCryptId(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptId, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getPad(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Pad, IsBigEndian);
        }

        [[nodiscard]] constexpr EncryptionInfoCommand64 &
        setCryptOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CryptOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr EncryptionInfoCommand64 &
        setCryptSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CryptSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr EncryptionInfoCommand64 &
        setCryptId(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->CryptId = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr EncryptionInfoCommand64 &
        setPad(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Pad = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct VersionMinimumCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::VersionMinimumMacOSX) ||
                (Kind == LoadCommand::Kind::VersionMinimumIPhoneOS) ||
                (Kind == LoadCommand::Kind::VersionMinimumTvOS) ||
                (Kind == LoadCommand::Kind::VersionMinimumWatchOS);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t Version;
        uint32_t Sdk;

        [[nodiscard]] constexpr
        Dyld3::PackedVersion getVersion(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Version, IsBigEndian);
        }

        [[nodiscard]] constexpr
        Dyld3::PackedVersion getSdk(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Sdk, IsBigEndian);
        }

        constexpr VersionMinimumCommand &
        setVersion(const Dyld3::PackedVersion &Version,
                   const bool IsBigEndian) noexcept
        {
            this->Version = SwitchEndianIf(Version.value(), IsBigEndian);
            return *this;
        }

        constexpr VersionMinimumCommand &
        setSdk(const Dyld3::PackedVersion &Sdk,
               const bool IsBigEndian) noexcept
        {
            this->Sdk = SwitchEndianIf(Sdk.value(), IsBigEndian);
            return *this;
        }
    };

    struct BuildVersionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::BuildVersion);
        }

        struct Tool {
            enum class Kind {
                Clang = 1,
                Swift,
                Ld,
            };

            [[nodiscard]] constexpr
            static std::string_view KindGetName(const enum Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::Clang:
                        return "TOOL_CLANG";
                    case Kind::Swift:
                        return "TOOL_SWIFT";
                    case Kind::Ld:
                        return "TOOL_LD";
                }

                return std::string_view();
            }

            [[nodiscard]] constexpr static
            std::string_view KindGetDescription(const enum Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::Clang:
                        return "Clang";
                    case Kind::Swift:
                        return "Swift";
                    case Kind::Ld:
                        return "ld";
                }

                return std::string_view();
            }

            uint32_t Kind;
            uint32_t Version;

            [[nodiscard]]
            constexpr enum Kind getKind(const bool IsBigEndian) const noexcept {
                const auto Integer = SwitchEndianIf(Kind, IsBigEndian);
                return static_cast<enum Kind>(Integer);
            }

            [[nodiscard]] constexpr Dyld3::PackedVersion
            getVersion(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Version, IsBigEndian);
            }

            constexpr Tool &
            setKind(const enum Kind Value, const bool IsBigEndian) noexcept
            {
                this->Kind = static_cast<uint32_t>(Value);
                return *this;
            }

            constexpr Tool &
            setVersion(const Dyld3::PackedVersion Value,
                       const bool IsBigEndian) noexcept
            {
                this->Version = Value.value();
                return *this;
            }
        };

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            const auto InvalidType =
                hasValidCmdSize(getCmdSize(IsBigEndian));

            if (InvalidType != LoadCommand::CmdSizeInvalidKind::None) {
                return InvalidType;
            }

            auto ExpectedSize = uint32_t();
            if (DoesMultiplyAndAddOverflow(sizeof(Tool), NTools, sizeof(*this),
                                           &ExpectedSize))
            {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > ExpectedSize) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            } else if (CmdSize < ExpectedSize) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(BuildVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t Platform;

        uint32_t MinOS;
        uint32_t Sdk;
        uint32_t NTools;

        [[nodiscard]] constexpr
        Dyld3::PlatformKind getPlatform(const bool IsBigEndian) const noexcept {
            return Dyld3::PlatformKind(SwitchEndianIf(Platform, IsBigEndian));
        }

        [[nodiscard]] constexpr
        Dyld3::PackedVersion getMinOS(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MinOS, IsBigEndian);
        }

        [[nodiscard]] constexpr
        Dyld3::PackedVersion getSdk(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Sdk, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getToolCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NTools, IsBigEndian);
        }

        constexpr BuildVersionCommand &
        setPlatform(Dyld3::PlatformKind Platform, bool IsBigEndian) noexcept {
            const auto Value = static_cast<uint32_t>(Platform);
            this->Platform = SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        constexpr BuildVersionCommand &
        setMinOS(const Dyld3::PackedVersion &MinOS,
                 const bool IsBigEndian) noexcept
        {
            this->MinOS = SwitchEndianIf(MinOS.value(), IsBigEndian);
            return *this;
        }

        constexpr BuildVersionCommand &
        setSdk(const Dyld3::PackedVersion &Sdk,
                 const bool IsBigEndian) noexcept
        {
            this->Sdk = SwitchEndianIf(Sdk.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr BuildVersionCommand &
        setToolCount(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->NTools = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        using ToolList = BasicContiguousList<Tool>;
        using ConstToolList = BasicContiguousList<const Tool>;

        [[nodiscard]] TypedAllocationOrError<ToolList, SizeRangeError>
        getToolList(bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstToolList, SizeRangeError>
        getConstToolList(bool IsBigEndian) const noexcept;
    };

    struct DyldInfoCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::DyldInfo) ||
                (Kind == LoadCommand::Kind::DyldInfoOnly);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
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

        [[nodiscard]] constexpr
        uint32_t getRebaseOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(RebaseOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getRebaseSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(RebaseSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getBindOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BindOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getBindSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getWeakBindOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakBindOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getWeakBindSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakBindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getLazyBindOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(LazyBindOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getLazyBindSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(LazyBindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getExportOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExportOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getExportSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExportSize, IsBigEndian);
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setRebaseOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->RebaseOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setRebaseSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->RebaseSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setBindOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->BindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setBindSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->BindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setWeakBindOffset(const uint32_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->WeakBindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setWeakBindSize(const uint32_t Value,
                        const bool IsBigEndian) noexcept
        {
            this->WeakBindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setLazyBindOffset(const uint32_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->LazyBindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setLazyBindSize(const uint32_t Value,
                        const bool IsBigEndian) noexcept
        {
            this->LazyBindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setExportOffset(const uint32_t Value,
                        const bool IsBigEndian) noexcept
        {
            this->ExportOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr DyldInfoCommand &
        setExportSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->ExportSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
        GetNakedBindOpcodeList(const MemoryMap &Map,
                               const bool IsBigEndian) const noexcept
        {
            const auto Offset = getBindOffset(IsBigEndian);
            const auto Size = getBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindOpcodeList, SizeRangeError>
        GetBindOpcodeList(const ConstMemoryMap &Map,
                          const bool IsBigEndian,
                          const bool Is64Bit) const noexcept
        {
            const auto Offset = getBindOffset(IsBigEndian);
            const auto Size = getBindSize(IsBigEndian);

            return ::MachO::GetBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<LazyBindOpcodeList, SizeRangeError>
        GetLazyBindOpcodeList(const ConstMemoryMap &Map,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = getLazyBindOffset(IsBigEndian);
            const auto Size = getLazyBindSize(IsBigEndian);

            return ::MachO::GetLazyBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<WeakBindOpcodeList, SizeRangeError>
        GetWeakBindOpcodeList(const ConstMemoryMap &Map,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = getWeakBindOffset(IsBigEndian);
            const auto Size = getWeakBindSize(IsBigEndian);

            return ::MachO::GetWeakBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindActionList, SizeRangeError>
        GetBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &Collection,
                          const bool IsBigEndian,
                          const bool Is64Bit) const noexcept
        {
            const auto Offset = getBindOffset(IsBigEndian);
            const auto Size = getBindSize(IsBigEndian);

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
                                   const bool IsBigEndian) const noexcept
        {
            const auto Offset = getLazyBindOffset(IsBigEndian);
            const auto Size = getLazyBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<LazyBindActionList, SizeRangeError>
        GetLazyBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = getLazyBindOffset(IsBigEndian);
            const auto Size = getLazyBindSize(IsBigEndian);

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
                                   const bool IsBigEndian) const noexcept
        {
            const auto Offset = getWeakBindOffset(IsBigEndian);
            const auto Size = getWeakBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<WeakBindActionList, SizeRangeError>
        GetWeakBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = getWeakBindOffset(IsBigEndian);
            const auto Size = getWeakBindSize(IsBigEndian);

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
                                 const bool IsBigEndian) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstRebaseNakedOpcodeList, SizeRangeError>
        GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                      const bool IsBigEndian) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetConstRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseActionList, SizeRangeError>
        GetRebaseActionList(const ConstMemoryMap &Map,
                            const bool IsBigEndian,
                            const bool Is64Bit) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseActionList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseOpcodeList, SizeRangeError>
        GetRebaseOpcodeList(const ConstMemoryMap &Map,
                            const bool IsBigEndian,
                            const bool Is64Bit) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseOpcodeList(Map, Offset, Size, Is64Bit);
        }

        TypedAllocationOrError<ExportTrieList, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map,
                          const bool IsBigEndian) noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        TypedAllocationOrError<ConstExportTrieList, SizeRangeError>
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               const bool IsBigEndian) const noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        TypedAllocationOrError<ExportTrieExportList, SizeRangeError>
        GetExportTrieExportList(const MemoryMap &Map,
                                const bool IsBigEndian) noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>
        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     const bool IsBigEndian) const noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        SizeRangeError
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            const bool IsBigEndian,
            std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);
            const auto Result =
                ::MachO::GetExportListFromExportTrie(Map,
                                                     Offset,
                                                     Size,
                                                     ExportListOut);

            return Result;
        }
    };

    struct LinkerOptionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::LinkerOption);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkerOptionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t Offset;
        uint32_t Size;

        [[nodiscard]]
        constexpr uint32_t getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr LinkerOptionCommand &
        getOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr LinkerOptionCommand &
        getSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymbolSegmentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolSegment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t Offset;
        uint32_t Size;

        [[nodiscard]]
        constexpr uint32_t getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr SymbolSegmentCommand &
        getOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr SymbolSegmentCommand &
        getSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct IdentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Ident);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(IdentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }
    };

    struct FixedVMFileCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::FixedVMFile);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]]
        constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMFileCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Name;
        uint32_t HeaderAddr;

        [[nodiscard]]
        inline bool isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getHeaderAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(HeaderAddr, IsBigEndian);
        }

        constexpr FixedVMFileCommand &
        setHeaderAddress(const uint32_t Value,
                         const bool IsBigEndian) noexcept
        {
            this->HeaderAddr = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct EntryPointCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Main);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint64_t EntryOffset;
        uint64_t StackSize;

        [[nodiscard]] constexpr
        uint64_t getEntryOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(EntryOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getStackSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StackSize, IsBigEndian);
        }

        constexpr EntryPointCommand &
        setEntryOffset(const uint32_t Value,
                       const bool IsBigEndian) noexcept
        {
            this->EntryOffset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr EntryPointCommand &
        setStackSize(const uint32_t Value,
                     const bool IsBigEndian) noexcept
        {
            this->StackSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SourceVersionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SourceVersion);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint64_t Version;

        [[nodiscard]] constexpr Dyld3::PackedVersion64
        getVersion(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Version, IsBigEndian);
        }

        constexpr SourceVersionCommand &
        setVersion(const Dyld3::PackedVersion64 &Version,
                   const bool IsBigEndian) noexcept
        {
            this->Version = SwitchEndianIf(Version.value(), IsBigEndian);
            return *this;
        }
    };

    struct NoteCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Note);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        char DataOwner[16];
        uint64_t Offset;
        uint64_t Size;

        [[nodiscard]]
        constexpr uint64_t getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr NoteCommand &
        setOffset(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr NoteCommand &
        setSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct FileSetEntryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::FileSetEntry);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FileSetEntryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint64_t VmAddr;
        uint64_t FileOffset;
        LoadCommandString EntryID;
        uint32_t Reserved;

        [[nodiscard]]
        inline uint64_t getVmAddr(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        inline uint64_t getFileOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileOffset, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getReserved(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved, IsBigEndian);
        }

        [[nodiscard]] inline
        bool isEntryIDOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return EntryID.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetEntryID(bool IsBigEndian) const noexcept;
    };
}

template <MachO::LoadCommand::Kind Kind>
[[nodiscard]]
inline bool isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return LC.getKind(IsBigEndian) == Kind;
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest>

[[nodiscard]] inline
bool isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return isa<First>(LC, IsBigEndian) || isa<Second, Rest...>(LC, IsBigEndian);
}

template <MachO::LoadCommandSubtype T>
inline
bool isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return T::IsOfKind(LC.getKind(IsBigEndian));
}

template <MachO::LoadCommandSubtype First,
          MachO::LoadCommandSubtype Second,
          MachO::LoadCommandSubtype... Rest>

[[nodiscard]]
inline bool isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return isa<First>(LC, IsBigEndian) || isa<Second, Rest...>(LC, IsBigEndian);
}

template <MachO::LoadCommandSubtype T>
[[nodiscard]]
inline T &cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    assert(isa<T>(LC, IsBigEndian));
    return reinterpret_cast<T &>(LC);
}

template <MachO::LoadCommandSubtype T>
[[nodiscard]] inline
const T &cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    assert(isa<T>(LC, IsBigEndian));
    return reinterpret_cast<const T &>(LC);
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]]
inline Ret &cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    assert(isa<Kind>(LC, IsBigEndian));
    return reinterpret_cast<Ret &>(LC);
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]] inline
const Ret &cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    assert(isa<Kind>(LC, IsBigEndian));
    return reinterpret_cast<const Ret &>(LC);
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]]
inline Ret *dyn_cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    if (isa<Kind>(LC, IsBigEndian)) {
        return reinterpret_cast<Ret *>(&LC);
    }

    return nullptr;
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]] inline const Ret *
dyn_cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    if (isa<Kind>(LC, IsBigEndian)) {
        return reinterpret_cast<const Ret *>(&LC);
    }

    return nullptr;
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest,
          typename Ret = typename MachO::LoadCommandKindInfo<First>::Type>

[[nodiscard]]
inline Ret *dyn_cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
        return reinterpret_cast<Ret *>(&LC);
    }

    return nullptr;
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest,
          typename Ret = typename MachO::LoadCommandKindInfo<First>::Type>

[[nodiscard]]
inline const Ret *
dyn_cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
        return reinterpret_cast<const Ret *>(&LC);
    }

    return nullptr;
}

template <typename T>
[[nodiscard]]
inline T *dyn_cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    if (isa<T>(LC, IsBigEndian)) {
        return reinterpret_cast<T *>(&LC);
    }

    return nullptr;
}

template <typename T>
[[nodiscard]] inline const T *
dyn_cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    if (isa<T>(LC, IsBigEndian)) {
        return reinterpret_cast<const T *>(&LC);
    }

    return nullptr;
}
