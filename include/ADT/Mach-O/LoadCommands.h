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

#include "BindInfo.h"
#include "ExportTrie.h"
#include "LoadCommandsCommon.h"
#include "LoadCommandTemplates.h"
#include "RebaseInfo.h"

namespace MachO {
    struct LoadCommand {
        using Kind = LoadCommandKind;

        template <Kind Kind>
        [[nodiscard]] inline bool isa(bool IsBigEndian) const noexcept {
            return (this->getKind(IsBigEndian) == Kind);
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline bool isa(bool IsBigEndian) const noexcept {
            if (isa<First>(IsBigEndian)) {
                return true;
            }

            return isa<Second, Rest...>(IsBigEndian);
        }

        template <Kind Kind>
        [[nodiscard]] inline
        LoadCommandTypeFromKind<Kind> &cast(bool IsBigEndian) noexcept {
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

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline LoadCommandPtrTypeFromKind<First>
        dynCast(bool IsBigEndian) noexcept {
            if (isa<First, Second, Rest...>(IsBigEndian)) {
                return &cast<First>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline const LoadCommandConstPtrTypeFromKind<First>
        dynCast(bool IsBigEndian) const noexcept {
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
        constexpr inline Kind getKind(bool IsBigEndian) const noexcept {
            return Kind(SwitchEndianIf(Cmd, IsBigEndian));
        }

        [[nodiscard]]
        constexpr inline uint32_t getCmdSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CmdSize, IsBigEndian);
        }

        constexpr
        inline LoadCommand &setKind(Kind Kind, bool IsBigEndian) noexcept {
            const auto Value = static_cast<uint32_t>(Kind);
            this->Cmd = SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        [[nodiscard]] constexpr inline
        LoadCommand &setCmdSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->CmdSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        constexpr inline bool hasValidKind(bool IsBigEndian) const noexcept {
            return KindIsValid(getKind(IsBigEndian));
        }

        [[nodiscard]] constexpr
        inline bool isRequiredByDyld(bool IsBigEndian) const noexcept {
            return KindIsRequiredByDyld(getKind(IsBigEndian));
        }

        [[nodiscard]] constexpr
        inline bool isSharedLibraryKind(bool IsBigEndian) const noexcept {
            return KindIsSharedLibrary(getKind(IsBigEndian));
        }
    };

    [[nodiscard]] constexpr inline bool
    SegOrSectNameEquals(const char *SegOrSectName,
                        std::string_view Rhs) noexcept
    {
        if (Rhs.length() > 16) {
            return false;
        }

        return (strncmp(Rhs.data(), SegOrSectName, Rhs.length()) == 0);
    }

    struct SegmentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
                const auto Length =
                    strnlen(SegmentName, sizeof(SegmentName));

                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr
            inline uint32_t getAddress(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Addr, IsBigEndian);
            }

            [[nodiscard]]
            constexpr inline uint32_t getSize(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getFileOffset(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getAlign(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            uint32_t getRelocationsCount(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Nreloc, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            SegmentSectionFlags getFlags(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Flags, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getReserved1(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved1, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getReserved2(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved2, IsBigEndian);
            }

            [[nodiscard]] constexpr inline std::optional<LocationRange>
            getFileRange(bool IsBigEndian) const noexcept {
                const auto Offset = getFileOffset(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Offset, Size);
            }

            [[nodiscard]] constexpr inline std::optional<LocationRange>
            getMemoryRange(bool IsBigEndian) const noexcept {
                const auto Addr = getAddress(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Addr, Size);
            }

            [[nodiscard]] constexpr inline
            bool segmentNameEquals(std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->SegmentName, Name);
            }

            [[nodiscard]] constexpr
            inline bool nameEquals(std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->Name, Name);
            }

            constexpr
            inline Section &setAddr(uint32_t Value, bool IsBigEndian) noexcept {
                this->Addr = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr
            inline Section &setSize(uint32_t Value, bool IsBigEndian) noexcept {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Section &setFileOffset(uint32_t Value, bool IsBigEndian) noexcept {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Section &setAlign(uint32_t Value, bool IsBigEndian) noexcept {
                this->Align = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr inline Section &
            setRelocationsCount(uint32_t Value, bool IsBigEndian) noexcept {
                this->Nreloc = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline Section &
            setFlags(const SegmentSectionFlags &Flags,
                     bool IsBigEndian) noexcept
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr inline
            Section &setReserved1(uint32_t Value, bool IsBigEndian) noexcept {
                this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr
            Section &setReserved2(uint32_t Value, bool IsBigEndian) noexcept {
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
        constexpr inline uint32_t getVmAddr(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getVmSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getFileOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getFileSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileSize, IsBigEndian);
        }

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

        [[nodiscard]] constexpr
        inline uint32_t getSectionCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsects, IsBigEndian);
        }

        [[nodiscard]] constexpr inline std::optional<LocationRange>
        getVmRange(bool IsBigEndian) const noexcept {
            const auto VmAddr = getVmAddr(IsBigEndian);
            const auto VmSize = getVmSize(IsBigEndian);

            return LocationRange::CreateWithSize(VmAddr, VmSize);
        }

        [[nodiscard]] constexpr inline std::optional<LocationRange>
        getFileRange(bool IsBigEndian) const noexcept {
            const auto Offset = getFileOffset(IsBigEndian);
            const auto Size = getFileSize(IsBigEndian);

            return LocationRange::CreateWithSize(Offset, Size);
        }

        [[nodiscard]]
        constexpr inline bool nameEquals(std::string_view Name) const noexcept {
            return SegOrSectNameEquals(this->Name, Name);
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

        [[nodiscard]] bool isSectionListValid(bool IsBigEndian) const noexcept;

        [[nodiscard]]
        inline SectionList getSectionListUnsafe(bool IsBigEndian) noexcept {
            const auto Ptr = reinterpret_cast<Section *>(this + 1);
            return SectionList(Ptr, getSectionCount(IsBigEndian));
        }

        [[nodiscard]] ConstSectionList
        getConstSectionListUnsafe(bool IsBigEndian) const noexcept {
            const auto Ptr = reinterpret_cast<const Section *>(this + 1);
            return ConstSectionList(Ptr, getSectionCount(IsBigEndian));
        }
    };

    struct SegmentCommand64 : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
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
            inline uint64_t getAddress(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Addr, IsBigEndian);
            }

            [[nodiscard]]
            constexpr inline uint64_t getSize(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getFileOffset(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getAlign(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Align, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            uint32_t getRelocationsCount(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Nreloc, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            SegmentSectionFlags getFlags(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Flags, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getReserved1(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved1, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getReserved2(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved2, IsBigEndian);
            }

            [[nodiscard]] constexpr inline std::optional<LocationRange>
            getFileRange(bool IsBigEndian) const noexcept {
                const auto Offset = getFileOffset(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Offset, Size);
            }

            [[nodiscard]] constexpr inline std::optional<LocationRange>
            getMemoryRange(bool IsBigEndian) const noexcept {
                const auto Addr = getAddress(IsBigEndian);
                const auto Size = getSize(IsBigEndian);

                return LocationRange::CreateWithSize(Addr, Size);
            }

            [[nodiscard]] constexpr inline
            bool segmentNameEquals(std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->SegmentName, Name);
            }

            [[nodiscard]] constexpr
            inline bool nameEquals(std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->Name, Name);
            }

            constexpr
            inline Section &setAddr(uint64_t Value, bool IsBigEndian) noexcept {
                this->Addr = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr
            inline Section &setSize(uint64_t Value, bool IsBigEndian) noexcept {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Section &setFileOffset(uint32_t Value, bool IsBigEndian) noexcept {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline
            Section &setAlign(uint32_t Value, bool IsBigEndian) noexcept {
                this->Align = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr inline Section &
            setRelocationsCount(uint32_t Value, bool IsBigEndian) noexcept {
                this->Nreloc = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr inline Section &
            setFlags(const SegmentSectionFlags &Flags,
                     bool IsBigEndian) noexcept
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr inline
            Section &setReserved1(uint32_t Value, bool IsBigEndian) noexcept {
                this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr
            Section &setReserved2(uint32_t Value, bool IsBigEndian) noexcept {
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
        constexpr inline uint64_t getVmAddr(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint64_t getVmSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(VmSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getFileOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint64_t getFileSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(FileSize, IsBigEndian);
        }

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

        [[nodiscard]] constexpr
        inline uint32_t getSectionCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsects, IsBigEndian);
        }

        [[nodiscard]] constexpr inline std::optional<LocationRange>
        getVmRange(bool IsBigEndian) const noexcept {
            const auto VmAddr = getVmAddr(IsBigEndian);
            const auto VmSize = getVmSize(IsBigEndian);

            return LocationRange::CreateWithSize(VmAddr, VmSize);
        }

        [[nodiscard]] constexpr inline std::optional<LocationRange>
        getFileRange(bool IsBigEndian) const noexcept {
            const auto Offset = getFileOffset(IsBigEndian);
            const auto Size = getFileSize(IsBigEndian);

            return LocationRange::CreateWithSize(Offset, Size);
        }

        [[nodiscard]]
        constexpr inline bool nameEquals(std::string_view Name) const noexcept {
            return SegOrSectNameEquals(this->Name, Name);
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

        [[nodiscard]] bool isSectionListValid(bool IsBigEndian) const noexcept;

        [[nodiscard]]
        inline SectionList getSectionListUnsafe(bool IsBigEndian) noexcept {
            const auto Ptr = reinterpret_cast<Section *>(this + 1);
            return SectionList(Ptr, getSectionCount(IsBigEndian));
        }

        [[nodiscard]] ConstSectionList
        getConstSectionListUnsafe(bool IsBigEndian) const noexcept {
            const auto Ptr = reinterpret_cast<const Section *>(this + 1);
            return ConstSectionList(Ptr, getSectionCount(IsBigEndian));
        }
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
        constexpr inline uint32_t getOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        constexpr inline LoadCommandString &
        setOffset(uint32_t Value, bool IsBigEndian) noexcept {
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMSharedLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        FixedVmSharedLibraryInfo Info;

        [[nodiscard]]
        inline bool isNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto Offset = Info.Name.getOffset(IsBigEndian);
            return Info.Name.isOffsetValid(sizeof(*this), Offset, CmdSize);
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
            inline uint32_t getNameOffset(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Name.Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            PackedVersion getCurrentVersion(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CurrentVersion, IsBigEndian);
            }

            [[nodiscard]] constexpr inline
            PackedVersion getCompatVersion(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(CompatibilityVersion, IsBigEndian);
            }

            [[nodiscard]] constexpr
            inline uint32_t getTimestamp(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Timestamp, IsBigEndian);
            }

            constexpr inline
            Info &setNameOffset(uint32_t Value, bool IsBigEndian) noexcept {
                this->Name.Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
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

            constexpr inline
            Info &setTimestamp(uint32_t Value, bool IsBigEndian) noexcept {
                this->Timestamp = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }
        };

        Info Info;

        [[nodiscard]]
        inline bool isNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Info.Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct SubFrameworkCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubFramework);
        }

        [[nodiscard]] constexpr inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubFrameworkCommand)) {
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

    struct SubClientCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubClient);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubClientCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Client;

        [[nodiscard]]
        inline bool isClientOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Client.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetClient(bool IsBigEndian) const noexcept;
    };

    struct SubUmbrellaCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubUmbrella);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubLibrary);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Library;

        [[nodiscard]]
        inline bool isLibraryOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Library.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLibrary(bool IsBigEndian) const noexcept;
    };

    struct PreBoundDylibCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PreBoundDylib);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        bool IsLinkedModulesOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylinkerCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Name;

        [[nodiscard]]
        inline bool isNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(ThreadCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }
    };

    struct RoutinesCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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

        [[nodiscard]] inline bool isExternal() const noexcept {
            return hasValueForMask(Masks::IsExternal);
        }

        [[nodiscard]] inline bool isPrivateExternal() const noexcept {
            return hasValueForMask(Masks::IsPrivateExternal);
        }

        [[nodiscard]] inline bool isDebugSymbol() const noexcept {
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]]
        constexpr static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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

        [[nodiscard]]
        inline uint32_t getSymbolTableOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SymOff, IsBigEndian);
        }

        [[nodiscard]]
        inline uint32_t getSymbolCount(bool IsBigEndian) const noexcept {
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline uint32_t getHintsOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getHintsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(NHints, IsBigEndian);
        }

        [[nodiscard]] TypedAllocationOrError<HintList, SizeRangeError>
        GetHintList(const MemoryMap &Map, bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstHintList, SizeRangeError>
        GetConstHintList(const ConstMemoryMap &Map,
                         bool IsBigEndian) const noexcept;

        constexpr inline TwoLevelHintsCommand &
        setHintsOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->Offset =  SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline TwoLevelHintsCommand &
        setHintsCount(uint32_t Value, bool IsBigEndian) noexcept {
            this->NHints = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct PrebindChecksumCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PrebindChecksum);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t CheckSum;

        [[nodiscard]] constexpr
        inline uint32_t getCheckSum(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CheckSum, IsBigEndian);
        }

        [[nodiscard]] constexpr inline PrebindChecksumCommand &
        setCheckSum(uint32_t Value, bool IsBigEndian) noexcept {
            this->CheckSum = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct UuidCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Uuid);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Rpath);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RpathCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Path;

        [[nodiscard]]
        inline bool isPathOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Path.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline uint32_t getDataOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(DataOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getDataSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(DataSize, IsBigEndian);
        }

        [[nodiscard]] constexpr inline LinkeditDataCommand &
        setDataOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->DataOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline LinkeditDataCommand &
        setDataSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->DataSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] TypedAllocationOrError<ExportTrieList, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map, bool IsBigEndian) noexcept {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstExportTrieList, SizeRangeError>

        GetConstExportTrieList(const ConstMemoryMap &Map,
                               bool IsBigEndian) const noexcept
        {
            assert(isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = getDataOffset(IsBigEndian);
            const auto Size = getDataSize(IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ExportTrieExportList, SizeRangeError>

        GetExportTrieExportList(const MemoryMap &Map, bool IsBigEndian) noexcept
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
                                     bool IsBigEndian) const noexcept
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
            bool IsBigEndian,
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
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline uint32_t getCryptOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getCryptSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getCryptId(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptId, IsBigEndian);
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand &
        setCryptOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->CryptOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand &
        setCryptSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->CryptSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand &
        setCryptId(uint32_t Value, bool IsBigEndian) noexcept {
            this->CryptId = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct EncryptionInfoCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline uint32_t getCryptOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getCryptSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getCryptId(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(CryptId, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getPad(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Pad, IsBigEndian);
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand64 &
        setCryptOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->CryptOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand64 &
        setCryptSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->CryptSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand64 &
        setCryptId(uint32_t Value, bool IsBigEndian) noexcept {
            this->CryptId = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline EncryptionInfoCommand64 &
        setPad(uint32_t Value, bool IsBigEndian) noexcept {
            this->Pad = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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

            uint32_t Kind;
            uint32_t Version;

            [[nodiscard]] constexpr
            inline enum Kind getKind(bool IsBigEndian) const noexcept {
                const auto Integer = SwitchEndianIf(Kind, IsBigEndian);
                return static_cast<enum Kind>(Integer);
            }

            [[nodiscard]] constexpr
            PackedVersion getVersion(bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Version, IsBigEndian);
            }

            constexpr
            inline Tool &setKind(enum Kind Value, bool IsBigEndian) noexcept {
                this->Kind = static_cast<uint32_t>(Value);
                return *this;
            }

            constexpr inline Tool &
            setVersion(PackedVersion Value, bool IsBigEndian) noexcept {
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

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline PlatformKind getPlatform(bool IsBigEndian) const noexcept {
            return PlatformKind(SwitchEndianIf(Platform, IsBigEndian));
        }

        [[nodiscard]] constexpr
        inline PackedVersion getMinOS(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(MinOS, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline PackedVersion getSdk(bool IsBigEndian) const noexcept {
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
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::DyldInfo) ||
                (Kind == LoadCommand::Kind::DyldInfoOnly);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline uint32_t getRebaseOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(RebaseOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getRebaseSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(RebaseSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getBindOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BindOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getBindSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getWeakBindOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakBindOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getWeakBindSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakBindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getLazyBindOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(LazyBindOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getLazyBindSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(LazyBindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getExportOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExportOff, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getExportSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ExportSize, IsBigEndian);
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setRebaseOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->RebaseOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setRebaseSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->RebaseSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setBindOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->BindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setBindSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->BindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setWeakBindOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->WeakBindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setWeakBindSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->WeakBindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setLazyBindOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->LazyBindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setLazyBindSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->LazyBindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setExportOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->ExportOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr inline DyldInfoCommand &
        setExportSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->ExportSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
        GetNakedBindOpcodeList(const MemoryMap &Map,
                               bool IsBigEndian) const noexcept
        {
            const auto Offset = getBindOffset(IsBigEndian);
            const auto Size = getBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindOpcodeList, SizeRangeError>
        GetBindOpcodeList(const ConstMemoryMap &Map,
                          bool IsBigEndian,
                          bool Is64Bit) const noexcept
        {
            const auto Offset = getBindOffset(IsBigEndian);
            const auto Size = getBindSize(IsBigEndian);

            return ::MachO::GetBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<LazyBindOpcodeList, SizeRangeError>
        GetLazyBindOpcodeList(const ConstMemoryMap &Map,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
        {
            const auto Offset = getLazyBindOffset(IsBigEndian);
            const auto Size = getLazyBindSize(IsBigEndian);

            return ::MachO::GetLazyBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<WeakBindOpcodeList, SizeRangeError>
        GetWeakBindOpcodeList(const ConstMemoryMap &Map,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
        {
            const auto Offset = getWeakBindOffset(IsBigEndian);
            const auto Size = getWeakBindSize(IsBigEndian);

            return ::MachO::GetWeakBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<BindActionList, SizeRangeError>
        GetBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &Collection,
                          bool IsBigEndian,
                          bool Is64Bit) const noexcept
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
                                   bool IsBigEndian) const noexcept
        {
            const auto Offset = getLazyBindOffset(IsBigEndian);
            const auto Size = getLazyBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<LazyBindActionList, SizeRangeError>
        GetLazyBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
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
                                   bool IsBigEndian) const noexcept
        {
            const auto Offset = getWeakBindOffset(IsBigEndian);
            const auto Size = getWeakBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline
        TypedAllocationOrError<WeakBindActionList, SizeRangeError>
        GetWeakBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              bool IsBigEndian,
                              bool Is64Bit) const noexcept
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
                                 bool IsBigEndian) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<ConstRebaseNakedOpcodeList, SizeRangeError>
        GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                      bool IsBigEndian) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetConstRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseActionList, SizeRangeError>
        GetRebaseActionList(const ConstMemoryMap &Map,
                            bool IsBigEndian,
                            bool Is64Bit) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseActionList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]]
        TypedAllocationOrError<RebaseOpcodeList, SizeRangeError>
        GetRebaseOpcodeList(const ConstMemoryMap &Map,
                            bool IsBigEndian,
                            bool Is64Bit) const noexcept
        {
            const auto Offset = getRebaseOffset(IsBigEndian);
            const auto Size = getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseOpcodeList(Map, Offset, Size, Is64Bit);
        }

        TypedAllocationOrError<ExportTrieList, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map, bool IsBigEndian) noexcept {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        TypedAllocationOrError<ConstExportTrieList, SizeRangeError>
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               bool IsBigEndian) const noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        TypedAllocationOrError<ExportTrieExportList, SizeRangeError>
        GetExportTrieExportList(const MemoryMap &Map,
                                bool IsBigEndian) noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>
        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     bool IsBigEndian) const noexcept
        {
            const auto Offset = getExportOffset(IsBigEndian);
            const auto Size = getExportSize(IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        SizeRangeError
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            bool IsBigEndian,
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
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::LinkerOption);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkerOptionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t Offset;
	    uint32_t Size;

        [[nodiscard]]
        constexpr inline uint32_t getOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr inline LinkerOptionCommand &
        getOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline LinkerOptionCommand &
        getSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymbolSegmentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolSegment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        constexpr inline uint32_t getOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr inline SymbolSegmentCommand &
        getOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline SymbolSegmentCommand &
        getSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct IdentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Ident);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(IdentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }
    };

    struct FixedVMFileCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::FixedVMFile);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMFileCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        LoadCommandString Name;
        uint32_t HeaderAddr;

        [[nodiscard]]
        inline bool isNameOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = getCmdSize(IsBigEndian);
            return Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getHeaderAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(HeaderAddr, IsBigEndian);
        }

        constexpr inline FixedVMFileCommand &
        setHeaderAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->HeaderAddr = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct EntryPointCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Main);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        inline uint64_t getEntryOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(EntryOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getStackSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StackSize, IsBigEndian);
        }

        constexpr inline EntryPointCommand &
        setEntryOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->EntryOffset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline EntryPointCommand &
        setStackSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->StackSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
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

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(bool IsBigEndian) noexcept {
            return hasValidCmdSize(getCmdSize(IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidKind
        hasValidCmdSize(uint32_t CmdSize) noexcept {
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
        constexpr inline uint64_t getOffset(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint64_t getSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr inline
        NoteCommand &setOffset(uint32_t Value, bool IsBigEndian) noexcept {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr
        inline NoteCommand &setSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };
}

template <MachO::LoadCommand::Kind Kind>
[[nodiscard]]
inline bool isa(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    return LC.getKind(IsBigEndian) == Kind;
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest>

[[nodiscard]]
inline bool isa(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    return isa<First>(LC, IsBigEndian) || isa<Second, Rest...>(LC, IsBigEndian);
}

template <typename T, typename = MachO::EnableIfLoadCommandSubtype<T>>
inline bool isa(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    return T::IsOfKind(LC.getKind(IsBigEndian));
}

template <typename First,
          typename Second,
          typename... Rest,
          typename = MachO::EnableIfLoadCommandSubtype<First, Second, Rest...>>

[[nodiscard]]
inline bool isa(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    return isa<First>(LC, IsBigEndian) || isa<Second, Rest...>(LC, IsBigEndian);
}

template <typename T, typename = MachO::EnableIfLoadCommandSubtype<T>>
[[nodiscard]]
inline T &cast(MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    assert(isa<T>(LC, IsBigEndian));
    return reinterpret_cast<T &>(LC);
}

template <typename T, typename = MachO::EnableIfLoadCommandSubtype<T>>
[[nodiscard]] inline
const T &cast(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    assert(isa<T>(LC, IsBigEndian));
    return reinterpret_cast<const T &>(LC);
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]]
inline Ret &cast(MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    assert(isa<Kind>(LC, IsBigEndian));
    return reinterpret_cast<Ret &>(LC);
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]] inline
const Ret &cast(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    assert(isa<Kind>(LC, IsBigEndian));
    return reinterpret_cast<const Ret &>(LC);
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]]
inline Ret *dyn_cast(MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    if (isa<Kind>(LC, IsBigEndian)) {
        return reinterpret_cast<Ret *>(&LC);
    }

    return nullptr;
}

template <MachO::LoadCommand::Kind Kind,
          typename Ret = typename MachO::LoadCommandKindInfo<Kind>::Type>

[[nodiscard]] inline
const Ret *dyn_cast(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
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
inline Ret *dyn_cast(MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
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
dyn_cast(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
        return reinterpret_cast<const Ret *>(&LC);
    }

    return nullptr;
}

template <typename T>
[[nodiscard]]
inline T *dyn_cast(MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    if (isa<T>(LC, IsBigEndian)) {
        return reinterpret_cast<T *>(&LC);
    }

    return nullptr;
}

template <typename T>
[[nodiscard]] inline const T *
dyn_cast(const MachO::LoadCommand &LC, bool IsBigEndian) noexcept {
    if (isa<T>(LC, IsBigEndian)) {
        return reinterpret_cast<const T *>(&LC);
    }

    return nullptr;
}
