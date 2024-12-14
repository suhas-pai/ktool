//
//  ADT/Mach-O/LoadCommands.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include <cstring>

#include "ADT/BasicContiguousList.h"
#include "ADT/Dyld3/PackedVersion.h"
#include "ADT/Dyld3/PlatformKind.h"

#include "Utils/SwitchEndian.h"

#include "BindInfo.h"
#include "ExportTrie.h"
#include "LoadCommandsCommon.h"
#include "LoadCommandTemplates.h"
#include "RebaseInfo.h"

namespace MachO {
    struct LoadCommand {
        using Kind = LoadCommandKind;

        [[nodiscard]]
        static auto KindGetName(Kind Kind) noexcept
            -> std::optional<std::string_view>;

        [[nodiscard]]
        static auto KindGetDescription(Kind Kind) noexcept
            -> std::optional<std::string_view>;

        [[nodiscard]]
        constexpr static auto KindIsRequiredByDyld(const Kind Kind) noexcept {
            return static_cast<uint32_t>(Kind) & KindRequiredByDyld;
        }

        [[nodiscard]]
        constexpr static auto KindIsRecognized(const Kind Kind) noexcept {
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
        constexpr static auto KindIsSharedLibrary(const Kind Kind) noexcept {
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
        auto
        ValidateCmdsize(const LoadCommand *LC, bool IsBigEndian) noexcept
            -> LoadCommand::CmdSizeInvalidKind;

        uint32_t Cmd;
        uint32_t CmdSize;

        [[nodiscard]]
        constexpr auto getCmd(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Cmd, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getKind(const bool IsBigEndian) const noexcept {
            return Kind(this->getCmd(IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getCmdSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CmdSize, IsBigEndian);
        }

        constexpr auto
        setKind(const Kind Kind, const bool IsBigEndian) noexcept {
            const auto Value = static_cast<uint32_t>(Kind);
            this->Cmd = SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        [[nodiscard]] constexpr auto
        setCmdSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CmdSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto hasRecognizedKind(const bool IsBigEndian) const noexcept {
            return KindIsRecognized(this->getKind(IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto isRequiredByDyld(const bool IsBigEndian) const noexcept {
            return KindIsRequiredByDyld(this->getKind(IsBigEndian));
        }

        [[nodiscard]] constexpr
        auto isSharedLibraryKind(const bool IsBigEndian) const noexcept {
            return KindIsSharedLibrary(this->getKind(IsBigEndian));
        }

        template <Kind Kind>
        [[nodiscard]] inline auto isa(const bool IsBigEndian) const noexcept {
            return this->getKind(IsBigEndian) == Kind;
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline auto isa(const bool IsBigEndian) const noexcept {
            if (this->isa<First>(IsBigEndian)) {
                return true;
            }

            return this->isa<Second, Rest...>(IsBigEndian);
        }

        template <Kind Kind>
        [[nodiscard]] inline auto cast(const bool IsBigEndian) noexcept
            -> LoadCommandTypeFromKind<Kind> &
        {
            assert(this->isa<Kind>(IsBigEndian));
            return reinterpret_cast<LoadCommandTypeFromKind<Kind> &>(*this);
        }

        template <Kind Kind>
        [[nodiscard]] inline auto cast(const bool IsBigEndian) const noexcept
            -> const LoadCommandTypeFromKind<Kind> &
        {
            using Type = LoadCommandTypeFromKind<Kind>;

            assert(this->isa<Kind>(IsBigEndian));
            const auto &Result = reinterpret_cast<const Type &>(*this);

            return Result;
        }

        template <Kind Kind>
        [[nodiscard]] inline auto dynCast(const bool IsBigEndian) noexcept
            -> LoadCommandPtrTypeFromKind<Kind>
        {
            if (this->isa<Kind>(IsBigEndian)) {
                return &this->cast<Kind>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind Kind>
        [[nodiscard]] inline auto dynCast(const bool IsBigEndian) const noexcept
            -> const LoadCommandConstPtrTypeFromKind<Kind>
        {
            if (this->isa<Kind>(IsBigEndian)) {
                return &this->cast<Kind>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline auto dynCast(const bool IsBigEndian) noexcept
            -> LoadCommandPtrTypeFromKind<First>
        {
            if (this->isa<First, Second, Rest...>(IsBigEndian)) {
                return &this->cast<First>(IsBigEndian);
            }

            return nullptr;
        }

        template <Kind First, Kind Second, Kind... Rest>
        [[nodiscard]] inline auto dynCast(const bool IsBigEndian) const noexcept
            -> const LoadCommandConstPtrTypeFromKind<First>
        {
            if (this->isa<First, Second, Rest...>(IsBigEndian)) {
                return &this->cast<First>(IsBigEndian);
            }

            return nullptr;
        }
    };

    [[nodiscard]] constexpr static auto
    SegOrSectNameEquals(const char *const SegOrSectName,
                        const std::string_view Rhs) noexcept
    {
        if (Rhs.length() > 16) {
            return false;
        }

        return strncmp(Rhs.data(), SegOrSectName, Rhs.length()) == 0;
    }

    struct SegmentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Segment;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
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

            [[nodiscard]] inline auto getName() const noexcept {
                const auto Length = strnlen(this->Name, sizeof(this->Name));
                return std::string_view(this->Name, Length);
            }

            [[nodiscard]] inline auto getSegmentName() const noexcept {
                const auto Length = strnlen(SegmentName, sizeof(SegmentName));
                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]]
            constexpr auto getAddress(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Addr, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getSize(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            auto getFileOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Offset, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getAlign(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Align, IsBigEndian);
            }

            [[nodiscard]] constexpr
            auto getRelocationsCount(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Nreloc, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getFlags(const bool IsBigEndian) const noexcept {
                return SegmentSectionFlags(SwitchEndianIf(Flags, IsBigEndian));
            }

            [[nodiscard]]
            constexpr auto getReserved1(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved1, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getReserved2(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Reserved2, IsBigEndian);
            }

            [[nodiscard]] constexpr auto
            getFileRange(const bool IsBigEndian) const noexcept
                -> std::optional<Range>
            {
                const auto Offset = this->getFileOffset(IsBigEndian);
                const auto Size = this->getSize(IsBigEndian);

                return Range::CreateWithSize(Offset, Size);
            }

            [[nodiscard]] constexpr auto
            getMemoryRange(const bool IsBigEndian) const noexcept
                -> std::optional<Range>
            {
                const auto Addr = this->getAddress(IsBigEndian);
                const auto Size = this->getSize(IsBigEndian);

                return Range::CreateWithSize(Addr, Size);
            }

            [[nodiscard]] constexpr
            auto segmentNameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->SegmentName, Name);
            }

            [[nodiscard]] constexpr
            auto nameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->Name, Name);
            }

            constexpr auto
            setAddr(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Addr = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setSize(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setFileOffset(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setAlign(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Align = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr auto
            setRelocationsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Nreloc = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setFlags(const SegmentSectionFlags &Flags,
                     const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr auto
            setReserved1(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr auto
            setReserved2(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
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

        [[nodiscard]] inline auto getName() const noexcept {
            const auto Length = strnlen(this->Name, sizeof(this->Name));
            return std::string_view(this->Name, Length);
        }

        [[nodiscard]]
        constexpr auto getVmAddr(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getVmSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->VmSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getFileOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->FileOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getFileSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->FileSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getInitProt(const bool IsBigEndian) const noexcept {
            const auto Value = SwitchEndianIf(this->InitProt, IsBigEndian);
            return MemoryProtections(Value);
        }

        [[nodiscard]]
        constexpr auto getMaxProt(const bool IsBigEndian) const noexcept {
            return MemoryProtections(SwitchEndianIf(this->MaxProt, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getFlags(const bool IsBigEndian) const noexcept {
            return SegmentFlags(SwitchEndianIf(this->Flags, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getSectionCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Nsects, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getVmRange(const bool IsBigEndian) const noexcept
            -> std::optional<Range>
        {
            const auto VmAddr = this->getVmAddr(IsBigEndian);
            const auto VmSize = this->getVmSize(IsBigEndian);

            return Range::CreateWithSize(VmAddr, VmSize);
        }

        [[nodiscard]]
        constexpr auto getFileRange(const bool IsBigEndian) const noexcept
            -> std::optional<Range>
        {
            const auto Offset = this->getFileOffset(IsBigEndian);
            const auto Size = this->getFileSize(IsBigEndian);

            return Range::CreateWithSize(Offset, Size);
        }

        [[nodiscard]]
        constexpr auto nameEquals(const std::string_view Name) const noexcept {
            return SegOrSectNameEquals(this->Name, Name);
        }

        constexpr auto
        setInitProt(const MemoryProtections &InitProt,
                    const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InitProt = SwitchEndianIf(InitProt.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setMaxProt(const MemoryProtections &MaxProt,
                   const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->MaxProt = SwitchEndianIf(MaxProt.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setFlags(const SegmentFlags &Flags, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        bool isSectionListValid(const bool IsBigEndian) const noexcept;

        [[nodiscard]]
        inline auto getSectionListUnsafe(const bool IsBigEndian) noexcept {
            const auto Ptr = reinterpret_cast<Section *>(this + 1);
            return SectionList(Ptr, getSectionCount(IsBigEndian));
        }

        [[nodiscard]] inline auto
        getConstSectionListUnsafe(const bool IsBigEndian) const noexcept {
            const auto Ptr = reinterpret_cast<const Section *>(this + 1);
            return ConstSectionList(Ptr, getSectionCount(IsBigEndian));
        }
    };

    struct SegmentCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Segment64;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
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

            [[nodiscard]] inline auto getName() const noexcept {
                const auto Length = strnlen(this->Name, sizeof(this->Name));
                return std::string_view(this->Name, Length);
            }

            [[nodiscard]] inline auto getSegmentName() const noexcept {
                const auto Length =
                    strnlen(this->SegmentName, sizeof(this->SegmentName));

                return std::string_view(this->SegmentName, Length);
            }

            [[nodiscard]]
            constexpr auto getAddress(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Addr, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getSize(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Size, IsBigEndian);
            }

            [[nodiscard]] constexpr
            auto getFileOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(Offset, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getAlign(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Align, IsBigEndian);
            }

            [[nodiscard]] constexpr auto
            getRelocationsCount(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Nreloc, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getFlags(const bool IsBigEndian) const noexcept {
                return SegmentSectionFlags(SwitchEndianIf(Flags, IsBigEndian));
            }

            [[nodiscard]]
            constexpr auto getReserved1(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Reserved1, IsBigEndian);
            }

            [[nodiscard]]
            constexpr auto getReserved2(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Reserved2, IsBigEndian);
            }

            [[nodiscard]] constexpr
            auto getFileRange(const bool IsBigEndian) const noexcept
                -> std::optional<Range>
            {
                const auto Offset = this->getFileOffset(IsBigEndian);
                const auto Size = this->getSize(IsBigEndian);

                return Range::CreateWithSize(Offset, Size);
            }

            [[nodiscard]] constexpr
            auto getMemoryRange(const bool IsBigEndian) const noexcept
                -> std::optional<Range>
            {
                const auto Addr = this->getAddress(IsBigEndian);
                const auto Size = this->getSize(IsBigEndian);

                return Range::CreateWithSize(Addr, Size);
            }

            [[nodiscard]] constexpr
            auto segmentNameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->SegmentName, Name);
            }

            [[nodiscard]] constexpr
            auto nameEquals(const std::string_view Name) const noexcept {
                return SegOrSectNameEquals(this->Name, Name);
            }

            constexpr auto
            setAddr(const uint64_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Addr = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setSize(const uint64_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Size = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setFileOffset(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setAlign(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Align = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr auto
            setRelocationsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Nreloc = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setFlags(const SegmentSectionFlags &Flags,
                     const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr auto
            setReserved1(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            [[nodiscard]] constexpr auto
            setReserved2(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
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

        [[nodiscard]] inline auto getName() const noexcept {
            const auto Length = strnlen(this->Name, sizeof(this->Name));
            return std::string_view(this->Name, Length);
        }

        [[nodiscard]]
        constexpr auto getVmAddr(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getVmSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->VmSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getFileOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->FileOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getFileSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->FileSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getInitProt(const bool IsBigEndian) const noexcept {
            const auto Value = SwitchEndianIf(this->InitProt, IsBigEndian);
            return MemoryProtections(Value);
        }

        [[nodiscard]]
        constexpr auto getMaxProt(const bool IsBigEndian) const noexcept {
            const auto Value = SwitchEndianIf(this->MaxProt, IsBigEndian);
            return MemoryProtections(Value);
        }

        [[nodiscard]]
        constexpr auto getFlags(const bool IsBigEndian) const noexcept {
            return SegmentFlags(SwitchEndianIf(this->Flags, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getSectionCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Nsects, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getVmRange(const bool IsBigEndian) const noexcept
            -> std::optional<Range>
        {
            const auto VmAddr = this->getVmAddr(IsBigEndian);
            const auto VmSize = this->getVmSize(IsBigEndian);

            return Range::CreateWithSize(VmAddr, VmSize);
        }

        [[nodiscard]]
        constexpr auto getFileRange(const bool IsBigEndian) const noexcept
            -> std::optional<Range>
        {
            const auto Offset = this->getFileOffset(IsBigEndian);
            const auto Size = this->getFileSize(IsBigEndian);

            return Range::CreateWithSize(Offset, Size);
        }

        [[nodiscard]]
        constexpr auto nameEquals(const std::string_view Name) const noexcept {
            return SegOrSectNameEquals(this->Name, Name);
        }

        constexpr auto
        setInitProt(const MemoryProtections &InitProt,
                    const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InitProt = SwitchEndianIf(InitProt.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setMaxProt(const MemoryProtections &MaxProt,
                   const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->MaxProt = SwitchEndianIf(MaxProt.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setFlags(const SegmentFlags &Flags, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] bool isSectionListValid(bool IsBigEndian) const noexcept;

        [[nodiscard]]
        inline auto getSectionListUnsafe(const bool IsBigEndian) noexcept {
            const auto Ptr = reinterpret_cast<Section *>(this + 1);
            return SectionList(Ptr, this->getSectionCount(IsBigEndian));
        }

        [[nodiscard]] inline auto
        getConstSectionListUnsafe(const bool IsBigEndian) const noexcept {
            const auto Ptr = reinterpret_cast<const Section *>(this + 1);
            return ConstSectionList(Ptr, this->getSectionCount(IsBigEndian));
        }
    };

    template <typename Error>
    struct LoadCommandStringViewOrError {
    protected:
        std::string_view View;
        [[nodiscard]] inline auto getStorage() const noexcept {
            return reinterpret_cast<uintptr_t>(this->View.begin());
        }
    public:
        LoadCommandStringViewOrError(const std::string_view View) noexcept
        : View(View) {}

        LoadCommandStringViewOrError(const Error Value) noexcept {
            this->View =
                std::string_view(reinterpret_cast<const char *>(Value), 1);
        }

        [[nodiscard]] inline auto hasError() const noexcept {
            const auto HasError = PointerHasErrorValue(this->getStorage());
            return HasError;
        }

        [[nodiscard]] inline auto getError() const noexcept {
            if (!this->hasError()) {
                return Error::None;
            }

            return Error(this->getStorage());
        }

        [[nodiscard]] inline auto getString() const noexcept {
            assert(!this->hasError());
            return this->View;
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
        constexpr auto getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        constexpr auto
        setOffset(const uint32_t Offset, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Offset = SwitchEndianIf(Offset, IsBigEndian);
            return *this;
        }

        using GetValueResult = LoadCommandStringViewOrError<GetStringError>;

        [[nodiscard]] bool
        isOffsetValid(uint32_t MinSize,
                      uint32_t CmdSize,
                      bool IsBigEndian) const noexcept;

        [[nodiscard]]
        auto GetLength(uint32_t CmdSize, bool IsBigEndian) const noexcept
            -> uint32_t;
    };

    struct FixedVmSharedLibraryInfo {
        LoadCommandString Name;
        Dyld3::PackedVersion MinorVersion;
        uint32_t HeaderAddr;
    };

    struct FixedVMSharedLibraryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::IdFixedVMSharedLibrary ||
                Kind == LoadCommand::Kind::LoadFixedVMSharedLibrary;

            return IsOfKind;
        }

        [[nodiscard]] constexpr
        static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMSharedLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        FixedVmSharedLibraryInfo Info;

        [[nodiscard]]
        inline auto isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto Offset = this->Info.Name.getOffset(IsBigEndian);
            return this->Info.Name.isOffsetValid(sizeof(*this),
                                                 Offset,
                                                 this->CmdSize);
        }

        [[nodiscard]] auto GetName(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct DylibCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::LoadDylib ||
                Kind == LoadCommand::Kind::IdDylib ||
                Kind == LoadCommand::Kind::LoadWeakDylib ||
                Kind == LoadCommand::Kind::ReexportDylib ||
                Kind == LoadCommand::Kind::LazyLoadDylib ||
                Kind == LoadCommand::Kind::LoadUpwardDylib;

            return IsOfKind;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylibCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        struct Info {
            LoadCommandString Name;

            uint32_t Timestamp;
            uint32_t CurrentVersion;
            uint32_t CompatibilityVersion;

            [[nodiscard]] constexpr
            auto getNameOffset(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Name.Offset, IsBigEndian);
            }

            [[nodiscard]] constexpr auto
            getCurrentVersion(const bool IsBigEndian) const noexcept {
                const auto Version =
                    SwitchEndianIf(this->CurrentVersion, IsBigEndian);

                return Dyld3::PackedVersion(Version);
            }

            [[nodiscard]] constexpr
            auto getCompatVersion(const bool IsBigEndian) const noexcept {
                const auto Version =
                    SwitchEndianIf(this->CompatibilityVersion, IsBigEndian);

                return Dyld3::PackedVersion(Version);
            }

            [[nodiscard]]
            constexpr auto getTimestamp(const bool IsBigEndian) const noexcept {
                return SwitchEndianIf(this->Timestamp, IsBigEndian);
            }

            constexpr auto
            setNameOffset(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Name.Offset = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }

            constexpr auto
            setCurrentVersion(const Dyld3::PackedVersion &Version,
                              const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                const auto Value = Version.value();
                this->CurrentVersion = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr auto
            setCompatVersion(const Dyld3::PackedVersion &Version,
                             const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                const auto Value = Version.value();
                this->CompatibilityVersion = SwitchEndianIf(Value, IsBigEndian);

                return *this;
            }

            constexpr auto
            setTimestamp(const uint32_t Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Timestamp = SwitchEndianIf(Value, IsBigEndian);
                return *this;
            }
        };

        Info Info;

        [[nodiscard]]
        inline auto isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Info.Name.isOffsetValid(sizeof(*this),
                                                 CmdSize,
                                                 IsBigEndian);
        }

        [[nodiscard]] auto GetName(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct SubFrameworkCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SubFramework;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubFrameworkCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Umbrella;

        [[nodiscard]] inline
        auto isUmbrellaOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Umbrella.isOffsetValid(sizeof(*this),
                                                CmdSize,
                                                IsBigEndian);
        }

        [[nodiscard]] auto GetUmbrella(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct SubClientCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SubClient;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubClientCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Client;

        [[nodiscard]]
        inline auto isClientOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Client.isOffsetValid(sizeof(*this),
                                              CmdSize,
                                              IsBigEndian);
        }

        [[nodiscard]] auto GetClient(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct SubUmbrellaCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SubUmbrella;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubUmbrellaCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        inline auto isUmbrellaOffsetValid(bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Umbrella.isOffsetValid(sizeof(*this),
                                                CmdSize,
                                                IsBigEndian);
        }

        [[nodiscard]] auto
        GetUmbrella(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct SubLibraryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SubLibrary;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Library;

        [[nodiscard]] inline
        auto isLibraryOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Library.isOffsetValid(sizeof(*this),
                                               CmdSize,
                                               IsBigEndian);
        }

        [[nodiscard]] auto GetLibrary(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct PreBoundDylibCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::PreBoundDylib;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PreBoundDylibCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Name;
        uint32_t NModules;
        LoadCommandString LinkedModules;

        [[nodiscard]] bool isNameOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] inline
        auto IsLinkedModulesOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] auto GetName(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;

        [[nodiscard]] auto GetLinkedModules(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct DylinkerCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::IdDylinker ||
                Kind == LoadCommand::Kind::LoadDylinker ||
                Kind == LoadCommand::Kind::DyldEnvironment;

            return IsOfKind;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylinkerCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Name;

        [[nodiscard]]
        inline auto isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] auto GetName(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct ThreadCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::Thread ||
                Kind == LoadCommand::Kind::UnixThread;

            return IsOfKind;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(ThreadCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }
    };

    struct RoutinesCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Routines;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
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
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Routines64;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
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

    constexpr static auto
    SymbolTableEntrySymbolKindGetName(
        const SymbolTableEntrySymbolKind Kind) noexcept
        -> std::optional<std::string_view>
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

        return std::nullopt;
    }

    constexpr static auto
    SymbolTableEntrySymbolKindGetDesc(
        const SymbolTableEntrySymbolKind Kind) noexcept
        -> std::optional<std::string_view>
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

        return std::nullopt;
    }

    [[nodiscard]]
    constexpr auto SymbolTableEntrySymbolKindGetLongestName() noexcept {
        const auto Result =
            EnumHelper<SymbolTableEntrySymbolKind>::GetLongestAssocLength(
                [](const SymbolTableEntrySymbolKind Kind) noexcept {
                    return SymbolTableEntrySymbolKindGetName(Kind).value();
                });

        return Result;
    }

    [[nodiscard]] constexpr
    auto SymbolTableEntrySymbolKindGetLongestDescription() noexcept {
        const auto Result =
            EnumHelper<SymbolTableEntrySymbolKind>::GetLongestAssocLength(
                [](const SymbolTableEntrySymbolKind Kind) noexcept {
                   return SymbolTableEntrySymbolKindGetDesc(Kind).value();
                });

        return Result;
    }

    struct SymbolTableEntryInfo : private SymbolTableEntryInfoMasksHandler {
        using Masks = SymbolTableEntryInfoMasks;
        using SymbolKind = SymbolTableEntrySymbolKind;

        [[nodiscard]] inline auto getKind() const noexcept {
            return SymbolKind(this->getValueForMask(Masks::SymbolKind));
        }

        [[nodiscard]] inline auto isUndefined() const noexcept {
            return this->getKind() == SymbolKind::Undefined;
        }

        [[nodiscard]] inline auto isAbsolute() const noexcept {
            return this->getKind() == SymbolKind::Absolute;
        }

        [[nodiscard]] inline auto isIndirect() const noexcept {
            return this->getKind() == SymbolKind::Indirect;
        }

        [[nodiscard]] inline auto isPreboundUndefined() const noexcept {
            return this->getKind() == SymbolKind::PreboundUndefined;
        }

        [[nodiscard]] inline auto isSectionDefined() const noexcept {
            return this->getKind() == SymbolKind::SectionDefined;
        }

        [[nodiscard]] inline auto isExternal() const noexcept {
            return this->hasValueForMask(Masks::IsExternal);
        }

        [[nodiscard]] inline auto isPrivateExternal() const noexcept {
            return this->hasValueForMask(Masks::IsPrivateExternal);
        }

        [[nodiscard]] inline auto isDebugSymbol() const noexcept {
            return this->hasValueForMask(Masks::IsDebugSymbol);
        }

        inline auto setKind(const SymbolKind Kind) noexcept
            -> decltype(*this)
        {
            const auto Value = static_cast<uint8_t>(Kind);
            this->setValueForMask(Masks::SymbolKind, Value);

            return *this;
        }

        inline auto setIsExternal(const bool Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::IsExternal, Value);
            return *this;
        }

        inline auto setIsPrivateExternal(const bool Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::IsPrivateExternal, Value);
            return *this;
        }

        inline auto setIsDebugSymbol(const bool Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::IsDebugSymbol, Value);
            return *this;
        }
    };

    [[nodiscard]]
    constexpr static auto GetDylibOrdinal(const uint16_t Desc) noexcept {
        return static_cast<uint64_t>((Desc >> 8) & 0xff);
    }

    constexpr auto
    SetDylibOrdinalOfDesc(uint16_t &Desc, const uint16_t Ordinal) noexcept {
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
        constexpr auto getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }

        inline auto
        setDylibOrdinal(const uint16_t DylibOrdinal,
                        const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            auto Desc =
                static_cast<uint16_t>(this->getDescription(IsBigEndian));

            SetDylibOrdinalOfDesc(Desc, DylibOrdinal);

            this->setDescription(Desc, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        inline auto getIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Index, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getSectionOrdinal(const bool IsBigEndian) const noexcept {
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

        inline auto
        setIndex(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Index = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setSectionOrdinal(const uint8_t Value,
                          const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Section = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setDescription(const int16_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Desc = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setValue(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
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

        [[nodiscard]] constexpr auto getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }

        [[nodiscard]]
        inline auto getIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Index, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getSectionOrdinal(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Section, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getDescription(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Desc, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getValue(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Value, IsBigEndian);
        }

        constexpr auto
        setDylibOrdinal(const uint16_t DylibOrdinal,
                        const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            auto Desc = this->getDescription(IsBigEndian);

            SetDylibOrdinalOfDesc(Desc, DylibOrdinal);
            setDescription(Desc, IsBigEndian);

            return *this;
        }

        inline auto
        setIndex(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Index = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setSectionOrdinal(const uint8_t Value,
                          const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Section = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setDescription(uint16_t Value, bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Desc = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setValue(uint64_t Value, bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Value = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymTabCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SymbolTable;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t SymOff;
        uint32_t Nsyms;
        uint32_t StrOff;
        uint32_t StrSize;

        [[nodiscard]] inline
        auto getSymbolTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SymOff, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getSymbolCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Nsyms, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getStringTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StrOff, IsBigEndian);
        }

        [[nodiscard]] inline
        uint32_t getStringTableSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(StrSize, IsBigEndian);
        }

        [[nodiscard]] inline auto
        setSymbolTableOffset(const uint32_t Value,
                             const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            SymOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        inline auto setSymbolsCount(uint32_t Value, bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            Nsyms = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline
        auto setStringTableOffset(uint32_t Value, bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            StrOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline auto
        setStringTableSize(const uint32_t Value,
                           const bool IsBigEndian) noexcept
            -> decltype(*this)
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

        [[nodiscard]] auto
        GetEntry32List(const MemoryMap &Map, bool IsBigEndian) const noexcept
            -> ExpectedAlloc<Entry32List, SizeRangeError>;

        [[nodiscard]] auto
        GetEntry64List(const MemoryMap &Map, bool IsBigEndian) const noexcept
            -> ExpectedAlloc<Entry64List, SizeRangeError>;

        [[nodiscard]] auto
        GetConstEntry32List(const ConstMemoryMap &Map,
                            bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstEntry32List, SizeRangeError>;

        [[nodiscard]] auto
        GetConstEntry64List(const ConstMemoryMap &Map,
                            bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstEntry64List, SizeRangeError>;
    };

    struct DynamicSymTabCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::DynamicSymbolTable;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DynamicSymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(DynamicSymTabCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
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
        auto getLocalSymbolsIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->ILocalSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getLocalSymbolsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NLocalSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getExternalSymbolsIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->IExternallyDefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getExternalSymbolsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NExternallyDefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getUndefinedSymbolsIndex(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->IUndefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getUndefinedSymbolsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NUndefinedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getTableOfContentsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->TableOfContentsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getTableOfContentsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NTableOfContentsEntries, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getModuleTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->ModuleTableOff, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getModuleTableCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NModuleTableEntries, IsBigEndian);
        }

        [[nodiscard]] inline auto
        getExternalRefSymbolTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(
                this->ExternalReferenceSymbolTableoff, IsBigEndian);
        }

        [[nodiscard]] inline auto
        getExternalRefSymbolTableCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NExtReferencedSymbols, IsBigEndian);
        }

        [[nodiscard]] inline auto
        getIndirectSymbolTableOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->IndirectSymbolTableOff, IsBigEndian);
        }

        [[nodiscard]] inline auto
        getIndirectSymbolTableCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NIndirectSymbols, IsBigEndian);
        }

        [[nodiscard]] inline auto
        getExternalRelocationsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->ExternalRelocationsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getExternalRelocationsCount(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NExternalRelocations, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getLocalRelocationsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->LocalRelocationsOff, IsBigEndian);
        }

        [[nodiscard]] inline
        auto getLocalRelocationsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NLocalRelocations, IsBigEndian);
        }

        inline auto
        setLocalSymbolsIndex(const uint32_t Value,
                             const bool IsBigEndian) noexcept -> decltype(*this)
        {
            this->ILocalSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }


        inline auto
        setLocalSymbolsCount(const uint32_t Value,
                             const bool IsBigEndian) noexcept -> decltype(*this)
        {
            this->NLocalSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setExternalSymbolsIndex(const uint32_t Value,
                                const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->IExternallyDefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setExternalSymbolsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NExternallyDefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setUndefinedSymbolsIndex(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->IUndefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setUndefinedSymbolsCount(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NUndefinedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setTableOfContentsOffset(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->TableOfContentsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setTableOfContentsCount(const uint32_t Value,
                                const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NTableOfContentsEntries = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        getModuleTableOffset(const uint32_t Value,
                             const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->ModuleTableOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        getModuleTableCount(const uint32_t Value,
                            const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NModuleTableEntries = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        getExternalRefSymbolTableOffset(const uint32_t Value,
                                        const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->ExternalReferenceSymbolTableoff =
                SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        inline auto
        getExternalRefSymbolTableCount(const uint32_t Value,
                                       const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NExtReferencedSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setIndirectSymbolTableOffset(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->IndirectSymbolTableOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setIndirectSymbolTableCount(const uint32_t Value,
                                    const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NIndirectSymbols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setExternalRelocationsOffset(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
        {
            this->ExternalRelocationsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setExternalRelocationsCount(const uint32_t Value,
                                    const bool IsBigEndian) noexcept
        {
            this->NExternalRelocations = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setLocalRelocationsOffset(const uint32_t Value,
                                  const bool IsBigEndian) noexcept
        {
            this->LocalRelocationsOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        inline auto
        setLocalRelocationsCount(const uint32_t Value,
                                 const bool IsBigEndian) noexcept
        {
            this->NLocalRelocations = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        using Entry32 = SymbolTableEntry32;
        using Entry64 = SymbolTableEntry64;

        using Entry32List = BasicContiguousList<Entry32>;
        using Entry64List = BasicContiguousList<Entry64>;

        using ConstEntry32List = BasicContiguousList<const Entry32>;
        using ConstEntry64List = BasicContiguousList<const Entry64>;

        [[nodiscard]] auto
        GetExport32List(const MemoryMap &Map,
                        uint32_t SymOff,
                        bool IsBigEndian) noexcept
            -> ExpectedAlloc<Entry32List, SizeRangeError>;

        [[nodiscard]] auto
        GetExport64List(const MemoryMap &Map,
                        uint32_t SymOff,
                        bool IsBigEndian) noexcept
            -> ExpectedAlloc<Entry64List, SizeRangeError>;

        [[nodiscard]] auto
        GetLocalSymbol32List(const MemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) noexcept
            -> ExpectedAlloc<Entry32List, SizeRangeError>;

        [[nodiscard]] auto
        GetLocalSymbol64List(const MemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) noexcept
            -> ExpectedAlloc<Entry64List, SizeRangeError>;

        [[nodiscard]] auto
        GetConstExport32List(const ConstMemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstEntry32List, SizeRangeError>;

        [[nodiscard]] auto
        GetConstExport64List(const ConstMemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstEntry64List, SizeRangeError>;

        [[nodiscard]] auto
        GetConstLocalSymbol32List(const ConstMemoryMap &Map,
                                  uint32_t SymOff,
                                  bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstEntry32List, SizeRangeError>;

        [[nodiscard]] auto
        GetConstLocalSymbol64List(const ConstMemoryMap &Map,
                                  uint32_t SymOff,
                                  bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstEntry64List, SizeRangeError>;

        [[nodiscard]] auto
        GetIndirectSymbolIndexTable(const MemoryMap &Map,
                                    bool IsBigEndian) noexcept
            -> ExpectedAlloc<
                BasicContiguousList<uint32_t>, SizeRangeError>;

        [[nodiscard]]
        auto
        GetConstIndirectSymbolIndexTable(const ConstMemoryMap &Map,
                                         bool IsBigEndian) noexcept
            -> ExpectedAlloc<
                BasicContiguousList<const uint32_t>, SizeRangeError>;
    };

    constexpr static auto IndirectSymbolLocal = 0x80000000;
    constexpr static auto IndirectSymbolAbsolute = 0x40000000;

    struct TwoLevelHintsCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::TwoLevelHints;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        struct Hint {
            uint32_t SubImagesIndex       : 8,
                     TableOfContentsIndex : 24;
        };

        using HintList = BasicContiguousList<Hint>;
        using ConstHintList = BasicContiguousList<const Hint>;

        uint32_t Offset;
        uint32_t NHints;

        [[nodiscard]]
        constexpr auto getHintsOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getHintsCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NHints, IsBigEndian);
        }

        [[nodiscard]]
        auto GetHintList(const MemoryMap &Map, bool IsBigEndian) noexcept
            -> ExpectedAlloc<HintList, SizeRangeError>;

        [[nodiscard]]
        auto
        GetConstHintList(const ConstMemoryMap &Map,
                         bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstHintList, SizeRangeError>;

        constexpr auto
        setHintsOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Offset =  SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto setHintsCount(uint32_t Value, bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NHints = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct PrebindChecksumCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::PrebindChecksum;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t CheckSum;

        [[nodiscard]]
        constexpr auto getCheckSum(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CheckSum, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto setCheckSum(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CheckSum = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct UuidCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Uuid;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint8_t Uuid[16];
    };

    struct RpathCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Rpath;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RpathCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Path;

        [[nodiscard]]
        inline auto isPathOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Path.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] auto GetPath(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct LinkeditDataCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::CodeSignature ||
                Kind == LoadCommand::Kind::SegmentSplitInfo ||
                Kind == LoadCommand::Kind::DataInCode ||
                Kind == LoadCommand::Kind::FunctionStarts ||
                Kind == LoadCommand::Kind::DylibCodeSignDRS ||
                Kind == LoadCommand::Kind::LinkerOptimizationHint ||
                Kind == LoadCommand::Kind::DyldExportsTrie ||
                Kind == LoadCommand::Kind::DyldChainedFixups;

            return IsOfKind;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t DataOff;
        uint32_t DataSize;

        [[nodiscard]]
        constexpr auto getDataOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->DataOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getDataSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->DataSize, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        setDataOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->DataOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setDataSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->DataSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] auto
        GetExportTrieList(const MemoryMap &Map, const bool IsBigEndian) noexcept
            -> ExpectedAlloc<ExportTrieList, SizeRangeError>
        {
            assert(this->isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = this->getDataOffset(IsBigEndian);
            const auto Size = this->getDataSize(IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        auto
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               const bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstExportTrieList, SizeRangeError>
        {
            assert(this->isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = this->getDataOffset(IsBigEndian);
            const auto Size = this->getDataSize(IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]]
        auto
        GetExportTrieExportList(const MemoryMap &Map,
                                const bool IsBigEndian) noexcept
            -> ExpectedAlloc<ExportTrieExportList, SizeRangeError>
        {
            assert(this->isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = this->getDataOffset(IsBigEndian);
            const auto Size = this->getDataSize(IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]] auto
        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     const bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstExportTrieExportList, SizeRangeError>
        {
            assert(this->isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = this->getDataOffset(IsBigEndian);
            const auto Size = this->getDataSize(IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]] auto
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            const bool IsBigEndian,
            std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
                -> SizeRangeError
        {
            assert(this->isa<LoadCommand::Kind::DyldExportsTrie>(IsBigEndian) &&
                   "Load Command is not a Dyld Export-Trie Load Command");

            const auto Offset = this->getDataOffset(IsBigEndian);
            const auto Size = this->getDataSize(IsBigEndian);
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
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::EncryptionInfo;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;

        [[nodiscard]]
        constexpr auto getCryptOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CryptOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getCryptSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CryptSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getCryptId(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CryptId, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        setCryptOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CryptOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setCryptSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CryptSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setCryptId(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CryptId = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct EncryptionInfoCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::EncryptionInfo64;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;
        uint32_t Pad;

        [[nodiscard]]
        constexpr auto getCryptOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CryptOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getCryptSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CryptSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getCryptId(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->CryptId, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getPad(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Pad, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        setCryptOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CryptOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setCryptSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CryptSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setCryptId(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->CryptId = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setPad(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Pad = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct VersionMinimumCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::VersionMinimumMacOSX ||
                Kind == LoadCommand::Kind::VersionMinimumIPhoneOS ||
                Kind == LoadCommand::Kind::VersionMinimumTvOS ||
                Kind == LoadCommand::Kind::VersionMinimumWatchOS;

            return IsOfKind;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }


        uint32_t Version;
        uint32_t Sdk;

        [[nodiscard]]
        constexpr auto getVersion(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion(SwitchEndianIf(Version, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getSdk(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion(SwitchEndianIf(Sdk, IsBigEndian));
        }

        constexpr auto
        setVersion(const Dyld3::PackedVersion &Version,
                   const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Version = SwitchEndianIf(Version.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setSdk(const Dyld3::PackedVersion &Sdk, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Sdk = SwitchEndianIf(Sdk.value(), IsBigEndian);
            return *this;
        }
    };

    struct BuildVersionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::BuildVersion;
        }

        struct Tool {
            enum class Kind {
                Clang = 1,
                Swift,
                Ld,
            };

            [[nodiscard]]
            constexpr static auto KindGetName(const enum Kind Kind) noexcept
                -> std::optional<std::string_view>
            {
                switch (Kind) {
                    case Kind::Clang:
                        return "TOOL_CLANG";
                    case Kind::Swift:
                        return "TOOL_SWIFT";
                    case Kind::Ld:
                        return "TOOL_LD";
                }

                return std::nullopt;
            }

            [[nodiscard]] constexpr
            static auto KindGetDescription(const enum Kind Kind) noexcept
                -> std::optional<std::string_view>
            {
                switch (Kind) {
                    case Kind::Clang:
                        return "Clang";
                    case Kind::Swift:
                        return "Swift";
                    case Kind::Ld:
                        return "ld";
                }

                return std::nullopt;
            }

            uint32_t Kind;
            uint32_t Version;

            [[nodiscard]]
            constexpr auto getKind(const bool IsBigEndian) const noexcept {
                const auto Integer = SwitchEndianIf(this->Kind, IsBigEndian);
                return static_cast<enum Kind>(Integer);
            }

            [[nodiscard]]
            constexpr auto getVersion(const bool IsBigEndian) const noexcept {
                const auto Version = SwitchEndianIf(this->Version, IsBigEndian);
                return Dyld3::PackedVersion(Version);
            }

            constexpr
            auto setKind(const enum Kind Value, const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Kind =
                    SwitchEndianIf(static_cast<uint32_t>(Value), IsBigEndian);

                return *this;
            }

            constexpr auto
            setVersion(const Dyld3::PackedVersion Value,
                       const bool IsBigEndian) noexcept
                -> decltype(*this)
            {
                this->Version = SwitchEndianIf(Value.value(), IsBigEndian);
                return *this;
            }
        };

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(BuildVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        uint32_t Platform;

        uint32_t MinOS;
        uint32_t Sdk;
        uint32_t NTools;

        [[nodiscard]]
        constexpr auto getPlatform(const bool IsBigEndian) const noexcept {
            return Dyld3::PlatformKind(SwitchEndianIf(Platform, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getMinOS(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion(SwitchEndianIf(MinOS, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getSdk(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion(SwitchEndianIf(this->Sdk, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getToolCount(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->NTools, IsBigEndian);
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            const auto InvalidType =
                this->hasValidCmdSize(this->getCmdSize(IsBigEndian));

            if (InvalidType != LoadCommand::CmdSizeInvalidKind::None) {
                return InvalidType;
            }

            const auto NTools = this->getToolCount(IsBigEndian);
            const auto CmdSize = this->getCmdSize(IsBigEndian);

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

        constexpr auto
        setPlatform(Dyld3::PlatformKind Platform, bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            const auto Value = static_cast<uint32_t>(Platform);
            this->Platform = SwitchEndianIf(Value, IsBigEndian);

            return *this;
        }

        constexpr auto
        setMinOS(const Dyld3::PackedVersion &MinOS,
                 const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->MinOS = SwitchEndianIf(MinOS.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setSdk(const Dyld3::PackedVersion &Sdk, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Sdk = SwitchEndianIf(Sdk.value(), IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setToolCount(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->NTools = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        using ToolList = BasicContiguousList<Tool>;
        using ConstToolList = BasicContiguousList<const Tool>;

        [[nodiscard]] auto getToolList(bool IsBigEndian) noexcept
            -> ExpectedAlloc<ToolList, SizeRangeError>;

        [[nodiscard]] auto getConstToolList(bool IsBigEndian) const noexcept
            -> ExpectedAlloc<ConstToolList, SizeRangeError>;
    };

    struct DyldInfoCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommand::Kind::DyldInfo ||
                Kind == LoadCommand::Kind::DyldInfoOnly;

            return IsOfKind;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
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

        [[nodiscard]]
        constexpr auto getRebaseOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->RebaseOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getRebaseSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->RebaseSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getBindOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->BindOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getBindSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->BindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getWeakBindOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->WeakBindOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getWeakBindSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->WeakBindSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getLazyBindOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->LazyBindOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getLazyBindSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->LazyBindSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getExportOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->ExportOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getExportSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->ExportSize, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        setRebaseOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->RebaseOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setRebaseSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->RebaseSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setBindOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr
        auto setBindSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setWeakBindOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->WeakBindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setWeakBindSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->WeakBindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setLazyBindOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->LazyBindOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setLazyBindSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->LazyBindSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setExportOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->ExportOff = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] constexpr auto
        setExportSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->ExportSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline auto
        GetNakedBindOpcodeList(const MemoryMap &Map,
                               const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getBindOffset(IsBigEndian);
            const auto Size = this->getBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetBindOpcodeList(const ConstMemoryMap &Map,
                          const bool IsBigEndian,
                          const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getBindOffset(IsBigEndian);
            const auto Size = this->getBindSize(IsBigEndian);

            return ::MachO::GetBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline auto
        GetLazyBindOpcodeList(const ConstMemoryMap &Map,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getLazyBindOffset(IsBigEndian);
            const auto Size = this->getLazyBindSize(IsBigEndian);

            return ::MachO::GetLazyBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline auto
        GetWeakBindOpcodeList(const ConstMemoryMap &Map,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getWeakBindOffset(IsBigEndian);
            const auto Size = this->getWeakBindSize(IsBigEndian);

            return ::MachO::GetWeakBindOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline auto
        GetBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &Collection,
                          const bool IsBigEndian,
                          const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getBindOffset(IsBigEndian);
            const auto Size = this->getBindSize(IsBigEndian);

            auto Result =
                ::MachO::GetBindActionList(Map,
                                           Collection,
                                           Offset,
                                           Size,
                                           Is64Bit);

            return Result;
        }

        [[nodiscard]] inline auto
        GetLazyBindNakedOpcodeList(const MemoryMap &Map,
                                   const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getLazyBindOffset(IsBigEndian);
            const auto Size = this->getLazyBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetLazyBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getLazyBindOffset(IsBigEndian);
            const auto Size = this->getLazyBindSize(IsBigEndian);

            auto Result =
                ::MachO::GetLazyBindActionList(Map,
                                               Collection,
                                               Offset,
                                               Size,
                                               Is64Bit);

            return Result;
        }

        [[nodiscard]] inline auto
        GetWeakBindNakedOpcodeList(const MemoryMap &Map,
                                   const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getWeakBindOffset(IsBigEndian);
            const auto Size = this->getWeakBindSize(IsBigEndian);

            return ::MachO::GetBindNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetWeakBindActionList(const ConstMemoryMap &Map,
                              const SegmentInfoCollection &Collection,
                              const bool IsBigEndian,
                              const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getWeakBindOffset(IsBigEndian);
            const auto Size = this->getWeakBindSize(IsBigEndian);

            auto Result =
                ::MachO::GetWeakBindActionList(Map,
                                               Collection,
                                               Offset,
                                               Size,
                                               Is64Bit);

            return Result;
        }

        [[nodiscard]] inline auto
        GetRebaseNakedOpcodeList(const MemoryMap &Map,
                                 const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getRebaseOffset(IsBigEndian);
            const auto Size = this->getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                      const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getRebaseOffset(IsBigEndian);
            const auto Size = this->getRebaseSize(IsBigEndian);

            return ::MachO::GetConstRebaseNakedOpcodeList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetRebaseActionList(const ConstMemoryMap &Map,
                            const bool IsBigEndian,
                            const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getRebaseOffset(IsBigEndian);
            const auto Size = this->getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseActionList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline auto
        GetRebaseOpcodeList(const ConstMemoryMap &Map,
                            const bool IsBigEndian,
                            const bool Is64Bit) const noexcept
        {
            const auto Offset = this->getRebaseOffset(IsBigEndian);
            const auto Size = this->getRebaseSize(IsBigEndian);

            return ::MachO::GetRebaseOpcodeList(Map, Offset, Size, Is64Bit);
        }

        [[nodiscard]] inline auto
        GetExportTrieList(const MemoryMap &Map,
                          const bool IsBigEndian) noexcept
        {
            const auto Offset = this->getExportOffset(IsBigEndian);
            const auto Size = this->getExportSize(IsBigEndian);

            return ::MachO::GetExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getExportOffset(IsBigEndian);
            const auto Size = this->getExportSize(IsBigEndian);

            return ::MachO::GetConstExportTrieList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetExportTrieExportList(const MemoryMap &Map,
                                const bool IsBigEndian) noexcept
        {
            const auto Offset = this->getExportOffset(IsBigEndian);
            const auto Size = this->getExportSize(IsBigEndian);

            return ::MachO::GetExportTrieExportList(Map, Offset, Size);
        }

        [[nodiscard]] inline auto
        GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                     const bool IsBigEndian) const noexcept
        {
            const auto Offset = this->getExportOffset(IsBigEndian);
            const auto Size = this->getExportSize(IsBigEndian);

            return ::MachO::GetConstExportTrieExportList(Map, Offset, Size);
        }

        auto
        GetExportListFromExportTrie(
            const ConstMemoryMap &Map,
            const bool IsBigEndian,
            std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
                -> SizeRangeError
        {
            const auto Offset = this->getExportOffset(IsBigEndian);
            const auto Size = this->getExportSize(IsBigEndian);
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
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::LinkerOption;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkerOptionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t Offset;
        uint32_t Size;

        [[nodiscard]]
        constexpr auto getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Size, IsBigEndian);
        }

        constexpr auto
        getOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        getSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SymbolSegmentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SymbolSegment;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint32_t Offset;
        uint32_t Size;

        [[nodiscard]]
        constexpr auto getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Size, IsBigEndian);
        }

        constexpr auto
        setOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct IdentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Ident;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(IdentCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }
    };

    struct FixedVMFileCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::FixedVMFile;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMFileCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        LoadCommandString Name;
        uint32_t HeaderAddr;

        [[nodiscard]]
        inline auto isNameOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return this->Name.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getHeaderAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->HeaderAddr, IsBigEndian);
        }

        constexpr auto
        setHeaderAddress(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->HeaderAddr = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] auto GetName(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };

    struct EntryPointCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Main;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint64_t EntryOffset;
        uint64_t StackSize;

        [[nodiscard]]
        constexpr auto getEntryOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->EntryOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getStackSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->StackSize, IsBigEndian);
        }

        constexpr auto
        setEntryOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->EntryOffset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setStackSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->StackSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct SourceVersionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::SourceVersion;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint64_t Version;

        [[nodiscard]]
        constexpr auto getVersion(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion64(SwitchEndianIf(Version, IsBigEndian));
        }

        constexpr auto
        setVersion(const Dyld3::PackedVersion64 &Version,
                   const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Version = SwitchEndianIf(Version.value(), IsBigEndian);
            return *this;
        }
    };

    struct NoteCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::Note;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            if (CmdSize > sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return this->hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        char DataOwner[16];
        uint64_t Offset;
        uint64_t Size;

        [[nodiscard]]
        constexpr auto getOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Size, IsBigEndian);
        }

        constexpr auto
        setOffset(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Offset = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Size = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct FileSetEntryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommand::Kind Kind) noexcept {
            return Kind == LoadCommand::Kind::FileSetEntry;
        }

        [[nodiscard]]
        constexpr static auto hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FileSetEntryCommand)) {
                return LoadCommand::CmdSizeInvalidKind::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        inline auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(this->getCmdSize(IsBigEndian));
        }

        uint64_t VmAddr;
        uint64_t FileOffset;
        LoadCommandString EntryID;
        uint32_t Reserved;

        [[nodiscard]]
        inline auto getVmAddr(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getFileOffset(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->FileOffset, IsBigEndian);
        }

        [[nodiscard]]
        inline auto getReserved(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(this->Reserved, IsBigEndian);
        }

        [[nodiscard]] inline
        auto isEntryIDOffsetValid(const bool IsBigEndian) const noexcept {
            const auto CmdSize = this->getCmdSize(IsBigEndian);
            return EntryID.isOffsetValid(sizeof(*this), CmdSize, IsBigEndian);
        }

        [[nodiscard]] auto GetEntryID(bool IsBigEndian) const noexcept
            -> LoadCommandString::GetValueResult;
    };
}

template <MachO::LoadCommand::Kind Kind>
[[nodiscard]]
inline auto isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return LC.getKind(IsBigEndian) == Kind;
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest>

[[nodiscard]] inline auto
isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return isa<First>(LC, IsBigEndian) || isa<Second, Rest...>(LC, IsBigEndian);
}

template <MachO::LoadCommandSubtype T>
[[nodiscard]] inline auto
isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return T::IsOfKind(LC.getKind(IsBigEndian));
}

template <MachO::LoadCommandSubtype First,
          MachO::LoadCommandSubtype Second,
          MachO::LoadCommandSubtype... Rest>

[[nodiscard]] inline auto
isa(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    return isa<First>(LC, IsBigEndian) || isa<Second, Rest...>(LC, IsBigEndian);
}

template <MachO::LoadCommandSubtype T>
[[nodiscard]] inline auto
cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept -> T& {
    assert(isa<T>(LC, IsBigEndian));
    return reinterpret_cast<T &>(LC);
}

template <MachO::LoadCommandSubtype T>
[[nodiscard]]
inline auto cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept
    -> const T&
{
    assert(isa<T>(LC, IsBigEndian));
    return reinterpret_cast<const T &>(LC);
}

template <MachO::LoadCommand::Kind Kind>
[[nodiscard]]
inline auto cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    assert(isa<Kind>(LC, IsBigEndian));

    using Ret = typename MachO::LoadCommandKindInfo<Kind>::Type;
    return reinterpret_cast<Ret &>(LC);
}

template <MachO::LoadCommand::Kind Kind>
[[nodiscard]] inline auto
cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    assert(isa<Kind>(LC, IsBigEndian));

    using Ret = typename MachO::LoadCommandKindInfo<Kind>::Type;
    return reinterpret_cast<const Ret &>(LC);
}

template <MachO::LoadCommand::Kind Kind>

[[nodiscard]]
inline auto dyn_cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    using Ret = typename MachO::LoadCommandKindInfo<Kind>::Type;
    if (isa<Kind>(LC, IsBigEndian)) {
        return reinterpret_cast<Ret *>(&LC);
    }

    return static_cast<Ret *>(nullptr);
}

template <MachO::LoadCommand::Kind Kind>
[[nodiscard]] inline auto
dyn_cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    using Ret = typename MachO::LoadCommandKindInfo<Kind>::Type;
    if (isa<Kind>(LC, IsBigEndian)) {
        return reinterpret_cast<const Ret *>(&LC);
    }

    return static_cast<const Ret *>(nullptr);
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest>

[[nodiscard]]
inline auto dyn_cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    using Ret = typename MachO::LoadCommandKindInfo<First>::Type;
    if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
        return reinterpret_cast<Ret *>(&LC);
    }

    return static_cast<Ret *>(nullptr);
}

template <MachO::LoadCommand::Kind First,
          MachO::LoadCommand::Kind Second,
          MachO::LoadCommand::Kind... Rest>

[[nodiscard]]
inline
auto dyn_cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept {
    using Ret = typename MachO::LoadCommandKindInfo<First>::Type;
    if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
        return reinterpret_cast<const Ret *>(&LC);
    }

    return static_cast<const Ret *>(nullptr);
}

template <typename T>
[[nodiscard]] inline auto
dyn_cast(MachO::LoadCommand &LC, const bool IsBigEndian) noexcept -> T* {
    if (isa<T>(LC, IsBigEndian)) {
        return reinterpret_cast<T *>(&LC);
    }

    return nullptr;
}

template <typename T>
[[nodiscard]] inline auto
dyn_cast(const MachO::LoadCommand &LC, const bool IsBigEndian) noexcept
    -> const T*
{
    if (isa<T>(LC, IsBigEndian)) {
        return reinterpret_cast<const T *>(&LC);
    }

    return nullptr;
}
