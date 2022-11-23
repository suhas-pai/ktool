//
//  MachO/LoadCommands.h
//  ktool
//
//  Created by suhaspai on 10/5/22.
//

#pragma once

#include <bitset>
#include <cstring>
#include <optional>
#include <string_view>
#include <type_traits>

#include "ADT/FlagsBase.h"
#include "ADT/Range.h"
#include "ADT/SwitchEndian.h"

#include "Dyld3/PackedVersion.h"
#include "Dyld3/Platform.h"

#include "Mach/VmProt.h"

namespace MachO {
    static constexpr auto LoadCommandReqByDyld = 1 << 31;
    enum class LoadCommandKind : uint32_t {
        Segment = 1,
        SymbolTable,
        SymbolSegment,
        Thread,
        UnixThread,
        LoadFixedVMSharedLib,
        IdFixedVMSharedLib,
        Identity,
        FixedVMFile,
        PrePage,
        DynamicSymbolTable,
        LoadDylib,
        IdDylib,
        LoadDylinker,
        IdDylinker,
        PreBoundDylib,
        Routines,
        SubFramework,
        SubUmbrella,
        SubClient,
        SubLibrary,
        TwoLevelHints,
        PreBindChecksum,
        LoadWeakDylib = static_cast<uint32_t>(LoadCommandReqByDyld | 0x18),
        Segment64 = 0x19,
        Routines64,
        Uuid,
        Rpath,
        CodeSignature,
        SegmentSplitInfo,
        ReexportDylib = static_cast<uint32_t>(LoadCommandReqByDyld | 0x1f),
        LazyLoadDylib = 0x20,
        EncryptionInfo,
        DyldInfo,
        DyldInfoOnly = static_cast<uint32_t>(LoadCommandReqByDyld | 0x22),
        LoadUpwardDylib = 0x23,
        VersionMinMacOS,
        VersionMinIOS,
        FunctionStarts,
        DyldEnvironment,
        Main = static_cast<uint32_t>(LoadCommandReqByDyld | 0x28),
        DataInCode = 0x29,
        SourceVersion,
        DylibCodeSignDRS,
        EncryptionInfo64,
        LinkerOption,
        LinkerOptimizationHint,
        VersionMinTVOS,
        VersionMinWatchOS,
        Note,
        BuildVersion,
        DyldExportsTrie =
            static_cast<uint32_t>(LoadCommandReqByDyld | 0x33),
        DyldChainedFixups =
            static_cast<uint32_t>(LoadCommandReqByDyld | 0x34),
        FileSetEntry = static_cast<uint32_t>(LoadCommandReqByDyld | 0x35)
    };

    [[nodiscard]]
    constexpr auto LoadCommandKindIsValid(const LoadCommandKind Kind) noexcept {
        switch (Kind) {
            case LoadCommandKind::Segment:
            case LoadCommandKind::SymbolTable:
            case LoadCommandKind::SymbolSegment:
            case LoadCommandKind::Thread:
            case LoadCommandKind::UnixThread:
            case LoadCommandKind::LoadFixedVMSharedLib:
            case LoadCommandKind::IdFixedVMSharedLib:
            case LoadCommandKind::Identity:
            case LoadCommandKind::FixedVMFile:
            case LoadCommandKind::PrePage:
            case LoadCommandKind::DynamicSymbolTable:
            case LoadCommandKind::LoadDylib:
            case LoadCommandKind::IdDylib:
            case LoadCommandKind::LoadDylinker:
            case LoadCommandKind::IdDylinker:
            case LoadCommandKind::PreBoundDylib:
            case LoadCommandKind::Routines:
            case LoadCommandKind::SubFramework:
            case LoadCommandKind::SubUmbrella:
            case LoadCommandKind::SubClient:
            case LoadCommandKind::SubLibrary:
            case LoadCommandKind::TwoLevelHints:
            case LoadCommandKind::PreBindChecksum:
            case LoadCommandKind::LoadWeakDylib:
            case LoadCommandKind::Segment64:
            case LoadCommandKind::Routines64:
            case LoadCommandKind::Uuid:
            case LoadCommandKind::Rpath:
            case LoadCommandKind::CodeSignature:
            case LoadCommandKind::SegmentSplitInfo:
            case LoadCommandKind::ReexportDylib:
            case LoadCommandKind::LazyLoadDylib:
            case LoadCommandKind::EncryptionInfo:
            case LoadCommandKind::DyldInfo:
            case LoadCommandKind::DyldInfoOnly:
            case LoadCommandKind::LoadUpwardDylib:
            case LoadCommandKind::VersionMinMacOS:
            case LoadCommandKind::VersionMinIOS:
            case LoadCommandKind::FunctionStarts:
            case LoadCommandKind::DyldEnvironment:
            case LoadCommandKind::Main:
            case LoadCommandKind::DataInCode:
            case LoadCommandKind::SourceVersion:
            case LoadCommandKind::DylibCodeSignDRS:
            case LoadCommandKind::EncryptionInfo64:
            case LoadCommandKind::LinkerOption:
            case LoadCommandKind::LinkerOptimizationHint:
            case LoadCommandKind::VersionMinTVOS:
            case LoadCommandKind::VersionMinWatchOS:
            case LoadCommandKind::Note:
            case LoadCommandKind::BuildVersion:
            case LoadCommandKind::DyldExportsTrie:
            case LoadCommandKind::DyldChainedFixups:
            case LoadCommandKind::FileSetEntry:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr
    auto LoadCommandKindGetString(const LoadCommandKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case LoadCommandKind::Segment:
                return "LC_SEGMENT";
            case LoadCommandKind::SymbolTable:
                return "LC_SYMTAB";
            case LoadCommandKind::SymbolSegment:
                return "LC_SYMSEG";
            case LoadCommandKind::Thread:
                return "LC_THREAD";
            case LoadCommandKind::UnixThread:
                return "LC_UNIXTHREAD";
            case LoadCommandKind::LoadFixedVMSharedLib:
                return "LC_LOADFVMLIB";
            case LoadCommandKind::IdFixedVMSharedLib:
                return "LC_IDFVMLIB";
            case LoadCommandKind::Identity:
                return "LC_IDENT";
            case LoadCommandKind::FixedVMFile:
                return "LC_FVMFILE";
            case LoadCommandKind::PrePage:
                return "LC_PREPAGE";
            case LoadCommandKind::DynamicSymbolTable:
                return "LC_DYSYMTAB";
            case LoadCommandKind::LoadDylib:
                return "LC_LOAD_DYLIB";
            case LoadCommandKind::IdDylib:
                return "LC_ID_DYLIB";
            case LoadCommandKind::LoadDylinker:
                return "LC_LOAD_DYLINKER";
            case LoadCommandKind::IdDylinker:
                return "LC_ID_DYLINKER";
            case LoadCommandKind::PreBoundDylib:
                return "LC_PREBOUND_DYLIB";
            case LoadCommandKind::Routines:
                return "LC_ROUTINES";
            case LoadCommandKind::SubFramework:
                return "LC_SUB_FRAMEWORK";
            case LoadCommandKind::SubUmbrella:
                return "LC_SUB_UMBRELLA";
            case LoadCommandKind::SubClient:
                return "LC_SUB_CLIENT";
            case LoadCommandKind::SubLibrary:
                return "LC_SUB_LIBRARY";
            case LoadCommandKind::TwoLevelHints:
                return "LC_TWOLEVEL_HINTS";
            case LoadCommandKind::PreBindChecksum:
                return "LC_PREBIND_CKSUM";
            case LoadCommandKind::LoadWeakDylib:
                return "LC_LOAD_WEAK_DYLIB";
            case LoadCommandKind::Segment64:
                return "LC_SEGMENT_64";
            case LoadCommandKind::Routines64:
                return "LC_ROUTINES_64";
            case LoadCommandKind::Uuid:
                return "LC_UUID";
            case LoadCommandKind::Rpath:
                return "LC_RPATH";
            case LoadCommandKind::CodeSignature:
                return "LC_CODE_SIGNATURE";
            case LoadCommandKind::SegmentSplitInfo:
                return "LC_SEGMENT_SPLIT_INFO";
            case LoadCommandKind::ReexportDylib:
                return "LC_REEXPORT_DYLIB";
            case LoadCommandKind::LazyLoadDylib:
                return "LC_LAZY_LOAD_DYLIB";
            case LoadCommandKind::EncryptionInfo:
                return "LC_ENCRYPTION_INFO";
            case LoadCommandKind::DyldInfo:
                return "LC_DYLD_INFO";
            case LoadCommandKind::DyldInfoOnly:
                return "LC_DYLD_INFO_ONLY";
            case LoadCommandKind::LoadUpwardDylib:
                return "LC_LOAD_UPWARD_DYLIB";
            case LoadCommandKind::VersionMinMacOS:
                return "LC_VERSION_MIN_MACOSX";
            case LoadCommandKind::VersionMinIOS:
                return "LC_VERSION_MIN_IPHONEOS";
            case LoadCommandKind::FunctionStarts:
                return "LC_FUNCTION_STARTS";
            case LoadCommandKind::DyldEnvironment:
                return "LC_DYLD_ENVIRONMENT";
            case LoadCommandKind::Main:
                return "LC_MAIN";
            case LoadCommandKind::DataInCode:
                return "LC_DATA_IN_CODE";
            case LoadCommandKind::SourceVersion:
                return "LC_SOURCE_VERSION";
            case LoadCommandKind::DylibCodeSignDRS:
                return "LC_DYLIB_CODE_SIGN_DRS";
            case LoadCommandKind::EncryptionInfo64:
                return "LC_ENCRYPTION_INFO_64";
            case LoadCommandKind::LinkerOption:
                return "LC_LINKER_OPTION";
            case LoadCommandKind::LinkerOptimizationHint:
                return "LC_LINKER_OPTIMIZATION_HINT";
            case LoadCommandKind::VersionMinTVOS:
                return "LC_VERSION_MIN_TVOS";
            case LoadCommandKind::VersionMinWatchOS:
                return "LC_VERSION_MIN_WATCHOS";
            case LoadCommandKind::Note:
                return "LC_NOTE";
            case LoadCommandKind::BuildVersion:
                return "LC_BUILD_VERSION";
            case LoadCommandKind::DyldExportsTrie:
                return "LC_DYLD_EXPORTS_TRIE";
            case LoadCommandKind::DyldChainedFixups:
                return "LC_DYLD_CHAINED_FIXUPS";
            case LoadCommandKind::FileSetEntry:
                return "LC_FILESET_ENTRY";
        }

        assert(false &&
               "Called LoadCommandKindGetString() with unknown "
               "LoadCommandKind");
    }

    struct LoadCommand {
        uint32_t Kind;
        uint32_t CmdSize;

        [[nodiscard]]
        constexpr auto kind(const bool IsBigEndian) const noexcept {
            return LoadCommandKind(ADT::SwitchEndianIf(Kind, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto cmdsize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CmdSize, IsBigEndian);
        }
    };

    static_assert(sizeof(LoadCommand) == 8,
                  "struct LoadCommand doesn't have CmdSize 8");

    enum class CmdSizeInvalidKind {
        None,
        TooSmall,
        TooLarge
    };

    [[nodiscard]] auto
    ValidateCmdsize(const LoadCommand *LC, bool IsBigEndian) noexcept
        -> CmdSizeInvalidKind;

    struct SegmentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Segment;
        }

        char SegmentName[16];

        [[nodiscard]] constexpr auto segmentName() const noexcept {
            const auto Length = strnlen(SegmentName, sizeof(SegmentName));
            return std::string_view(SegmentName, Length);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        uint32_t VmAddr;
        uint32_t VmSize;

        [[nodiscard]]
        constexpr auto vmAddr(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto vmSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(VmSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto vmRange(const bool IsBigEndian) const noexcept {
            const auto VmAddr = ADT::SwitchEndianIf(this->VmAddr, IsBigEndian);
            const auto VmSize = ADT::SwitchEndianIf(this->VmSize, IsBigEndian);

            return ADT::Range::FromSize(VmAddr, VmSize);
        }

        uint32_t FileOffset;
        uint32_t FileSize;

        [[nodiscard]]
        constexpr auto fileOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(FileOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto fileSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(FileSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto fileRange(const bool IsBigEndian) const noexcept {
            const auto FileSize =
                ADT::SwitchEndianIf(this->FileSize, IsBigEndian);
            const auto FileOffset =
                ADT::SwitchEndianIf(this->FileOffset, IsBigEndian);

            return ADT::Range::FromSize(FileOffset, FileSize);
        }

        uint32_t MaxProt;
        uint32_t InitProt;

        [[nodiscard]]
        constexpr auto maxProt(const bool IsBigEndian) const noexcept {
            return Mach::VmProt(ADT::SwitchEndianIf(MaxProt, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto initProt(const bool IsBigEndian) const noexcept {
            return Mach::VmProt(ADT::SwitchEndianIf(InitProt, IsBigEndian));
        }

        uint32_t SectionCount;

        [[nodiscard]]
        constexpr auto sectionCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(SectionCount, IsBigEndian);
        }

        struct FlagsStruct : public ADT::FlagsBase<uint32_t> {
        public:
            enum class Kind {
                HighVm = 1 << 0,
                FixedVmLibrary = 1 << 1,
                NoRelocations = 1 << 2,
                ProtectedVersion1 = 1 << 3,
                ReadOnly = 1 << 4
            };

            using ADT::FlagsBase<uint32_t>::FlagsBase;

            [[nodiscard]] constexpr auto isHighVm() const noexcept -> bool {
                return (Flags & static_cast<uint32_t>(Kind::HighVm));
            }

            [[nodiscard]]
            constexpr auto isFixedVmLibrary() const noexcept -> bool {
                return (Flags & static_cast<uint32_t>(Kind::FixedVmLibrary));
            }

            [[nodiscard]]
            constexpr auto hasNoRelocations() const noexcept -> bool {
                return (Flags & static_cast<uint32_t>(Kind::NoRelocations));
            }

            [[nodiscard]]
            constexpr auto isProtectedVersion1() const noexcept -> bool {
                return (Flags & static_cast<uint32_t>(Kind::ProtectedVersion1));
            }

            [[nodiscard]] constexpr auto isReadOnly() const noexcept -> bool {
                return (Flags & static_cast<uint32_t>(Kind::ReadOnly));
            }

            constexpr auto setIsHighVm(const bool Value = true) noexcept {
                setForValue(Kind::HighVm, Value);
                return *this;
            }

            constexpr
            auto setIsFixedVmLibrary(const bool Value = true) noexcept {
                setForValue(Kind::FixedVmLibrary, Value);
                return *this;
            }

            constexpr
            auto setHasNoRelocations(const bool Value = true) noexcept {
                setForValue(Kind::NoRelocations, Value);
                return *this;
            }

            constexpr
            auto setIsProtectedVersion1(const bool Value = true) noexcept {
                setForValue(Kind::ProtectedVersion1, Value);
                return *this;
            }

            constexpr auto setIsReadOnly(const bool Value = true) noexcept {
                setForValue(Kind::ReadOnly, Value);
                return *this;
            }
        };

        uint32_t Flags;

        [[nodiscard]]
        constexpr auto flags(const bool IsBigEndian) const noexcept {
            return FlagsStruct(ADT::SwitchEndianIf(Flags, IsBigEndian));
        }

        struct Section {
            char SegmentName[16];
            char SectionName[16];

            [[nodiscard]] constexpr auto segmentName() const noexcept {
                const auto Length = strnlen(SegmentName, sizeof(SegmentName));
                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr auto sectionName() const noexcept {
                const auto Length = strnlen(SectionName, sizeof(SectionName));
                return std::string_view(SectionName, Length);
            }

            uint32_t Addr;
            uint32_t Size;

            [[nodiscard]]
            constexpr auto virtualRange(const bool IsBigEndian) const noexcept {
                const auto Addr = ADT::SwitchEndianIf(this->Addr, IsBigEndian);
                const auto Size = ADT::SwitchEndianIf(this->Size, IsBigEndian);

                return ADT::Range::FromSize(Addr, Size);
            }

            uint32_t FileOffset;

            [[nodiscard]]
            constexpr auto fileRange(const bool IsBigEndian) const noexcept {
                const auto Size = ADT::SwitchEndianIf(this->Size, IsBigEndian);
                const auto FileOffset =
                    ADT::SwitchEndianIf(this->FileOffset, IsBigEndian);

                return ADT::Range::FromSize(FileOffset, Size);
            }

            uint32_t Align;
            uint32_t RelocFileOffset;
            uint32_t RelocsCount;

            enum class Kind {
                Regular,
                ZeroFillOnDemand,
                CStringLiterals,
                FourByteLiterals,
                EightByteLiterals,
                LiteralPointers,

                NonLazySymbolPointers,
                LazySymbolPointers,
                SymbolStubs,
                ModInitFunctionPointers,
                ModTermFunctionPointers,
                Coalesced,
                ZeroFill4GBPlus,
                Interposing,
                SixteenByteLiterals,
                DtraceDof,
                LazyDylibSymbolPointers,

                ThreadLocalRegular,
                ThreadLocalZeroFill,
                ThreadLocalVariables,
                ThreadLocalVariablePointers,
                ThreadLocalInitFunctionPointers,
                InitFunctionOffsets
            };

            enum class Attributes : uint32_t {
                HasLocalReloc = 1 << 8,
                HasExternalReloc = 1 << 9,
                HasSomeInstructions = 1 << 10,

                DebugSection = 1 << 25,
                SelfModifyingCode = 1 << 26,
                HasLiveSupport = 1 << 27,
                NoDeadStrip = 1 << 28,
                StripStaticSymbols = 1 << 29,
                NoTableOfContents = 1 << 30,
                PureInstructions = static_cast<uint32_t>(1 << 31)
            };

            struct FlagsStruct : public ADT::FlagsBase<uint32_t> {
            protected:
                static constexpr auto KindMask = uint32_t(0xFF);
                static constexpr auto AttributesMask = ~KindMask;
            public:
                using ADT::FlagsBase<uint32_t>::FlagsBase;

                [[nodiscard]] constexpr auto kind() const noexcept {
                    return Kind(Flags & KindMask);
                }

                [[nodiscard]] constexpr auto attributes() const noexcept {
                    return Attributes(Flags & AttributesMask);
                }
            };

            uint32_t Flags;

            [[nodiscard]]
            constexpr auto flags(const bool IsBigEndian) const noexcept {
                return FlagsStruct(ADT::SwitchEndianIf(Flags, IsBigEndian));
            }

            uint32_t Reserved1;
            uint32_t Reserved2;
        };

        [[nodiscard]] constexpr auto
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            if (const auto Result = hasValidCmdSize(cmdsize(IsBigEndian));
                Result != CmdSizeInvalidKind::None)
            {
                return Result;
            }

            const auto SectionSize =
                sizeof(Section) * sectionCount(IsBigEndian);

            if (sizeof(*this) + SectionSize < cmdsize(IsBigEndian)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }
    };

    struct SegmentCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Segment64;
        }

        char SegmentName[16];

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand64)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]] constexpr auto segmentName() const noexcept {
            const auto Length = strnlen(SegmentName, sizeof(SegmentName));
            return std::string_view(SegmentName, Length);
        }

        uint64_t VmAddr;
        uint64_t VmSize;

        [[nodiscard]]
        constexpr auto vmAddr(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(VmAddr, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto vmSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(VmSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto vmRange(const bool IsBigEndian) const noexcept {
            const auto VmAddr = ADT::SwitchEndianIf(this->VmAddr, IsBigEndian);
            const auto VmSize = ADT::SwitchEndianIf(this->VmSize, IsBigEndian);

            return ADT::Range::FromSize(VmAddr, VmSize);
        }

        uint64_t FileOffset;
        uint64_t FileSize;

        [[nodiscard]]
        constexpr auto fileOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(FileOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto fileSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(FileSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto fileRange(const bool IsBigEndian) const noexcept {
            const auto FileSize =
                ADT::SwitchEndianIf(this->FileSize, IsBigEndian);
            const auto FileOffset =
                ADT::SwitchEndianIf(this->FileOffset, IsBigEndian);

            return ADT::Range::FromSize(FileOffset, FileSize);
        }

        uint32_t MaxProt;
        uint32_t InitProt;

        [[nodiscard]]
        constexpr auto maxProt(const bool IsBigEndian) const noexcept {
            return Mach::VmProt(ADT::SwitchEndianIf(MaxProt, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto initProt(const bool IsBigEndian) const noexcept {
            return Mach::VmProt(ADT::SwitchEndianIf(InitProt, IsBigEndian));
        }


        uint32_t SectionCount;

        [[nodiscard]]
        constexpr auto sectionCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(SectionCount, IsBigEndian);
        }

        using FlagsStruct = SegmentCommand::FlagsStruct;
        struct Section {
            char SegmentName[16];
            char SectionName[16];

            [[nodiscard]] constexpr auto segmentName() const noexcept {
                const auto Length = strnlen(SegmentName, sizeof(SegmentName));
                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr auto sectionName() const noexcept {
                const auto Length = strnlen(SectionName, sizeof(SectionName));
                return std::string_view(SectionName, Length);
            }

            uint64_t Addr;
            uint64_t Size;

            [[nodiscard]] constexpr
            auto virtualRange(const bool IsBigEndian) const noexcept {
                const auto Addr = ADT::SwitchEndianIf(this->Addr, IsBigEndian);
                const auto Size = ADT::SwitchEndianIf(this->Size, IsBigEndian);

                return ADT::Range::FromSize(Addr, Size);
            }

            uint32_t FileOffset;

            [[nodiscard]]
            constexpr auto fileRange(const bool IsBigEndian) const noexcept {
                const auto Size = ADT::SwitchEndianIf(this->Size, IsBigEndian);
                const auto FileOffset =
                    ADT::SwitchEndianIf(this->FileOffset, IsBigEndian);

                return ADT::Range::FromSize(FileOffset, Size);
            }

            uint32_t Align;
            uint32_t RelocFileOffset;
            uint32_t RelocsCount;

            using FlagsStruct = SegmentCommand::Section::FlagsStruct;
            using Attributes = SegmentCommand::Section::Attributes;
            using Kind = SegmentCommand::Section::Kind;

            uint32_t Flags;

            [[nodiscard]]
            constexpr auto flags(const bool IsBigEndian) const noexcept {
                return FlagsStruct(ADT::SwitchEndianIf(Flags, IsBigEndian));
            }
        };

        [[nodiscard]] constexpr auto
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            if (const auto Result = hasValidCmdSize(cmdsize(IsBigEndian));
                Result != CmdSizeInvalidKind::None)
            {
                return Result;
            }

            const auto SectionSize =
                sizeof(Section) * sectionCount(IsBigEndian);

            if (sizeof(*this) + SectionSize < cmdsize(IsBigEndian)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        uint32_t Flags;

        [[nodiscard]]
        constexpr auto flags(const bool IsBigEndian) const noexcept {
            return FlagsStruct(ADT::SwitchEndianIf(Flags, IsBigEndian));
        }
    };

    struct ThreadCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommandKind Kind) noexcept {
            const auto IsOfKind =
                Kind == LoadCommandKind::Thread ||
                Kind == LoadCommandKind::UnixThread;

            return IsOfKind;
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(ThreadCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct LoadCommandString {
        uint32_t Offset;

        [[nodiscard]] constexpr auto
        string(const LoadCommand *const Cmd,
               const bool IsBigEndian) const noexcept
            -> std::optional<std::string_view>
        {
            const auto Offset = ADT::SwitchEndianIf(this->Offset, IsBigEndian);
            const auto CmdSize = ADT::SwitchEndianIf(Cmd->CmdSize, IsBigEndian);

            if (Offset >= CmdSize) {
                return std::nullopt;
            }

            const auto Ptr = reinterpret_cast<const char *>(Cmd);
            const auto Length = strnlen(Ptr + Offset, Cmd->CmdSize - Offset);

            if (Length == 0) {
                return std::nullopt;
            }

            return std::optional(std::string_view(Ptr + Offset, Length));
        }
    };

    struct FvmLibrary {
        LoadCommandString Name;
        uint32_t MinorVersion;
        uint32_t HeaderAddress;

        [[nodiscard]]
        constexpr auto minorVersion(const bool IsBE) const noexcept {
            const auto Swapped = ADT::SwitchEndianIf(MinorVersion, IsBE);
            return Dyld3::PackedVersion(Swapped);
        }

        [[nodiscard]]
        constexpr auto headerAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(HeaderAddress, IsBigEndian);
        }
    };

    struct FvmLibraryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static auto IsOfKind(const LoadCommandKind Kind) noexcept {
            return
                Kind == LoadCommandKind::LoadFixedVMSharedLib ||
                Kind == LoadCommandKind::IdFixedVMSharedLib;
        }

        FvmLibrary Library;

        [[nodiscard]]
        constexpr auto name(const bool IsBigEndian) const noexcept {
            return Library.Name.string(this, IsBigEndian);
        }
    };

    struct IdentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Identity;
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(IdentCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]] constexpr auto
        hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct Dylib {
        LoadCommandString Name;
        uint32_t Timestamp;

        uint32_t CurrentVersion;
        uint32_t CompatVersion;

        [[nodiscard]]
        constexpr auto timestamp(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Timestamp, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto currentVersion(const bool IsBE) const noexcept {
            const auto Swapped = ADT::SwitchEndianIf(CurrentVersion, IsBE);
            return Dyld3::PackedVersion(Swapped);
        }

        [[nodiscard]]
        constexpr auto compatVersion(const bool IsBE) const noexcept {
            const auto Swapped = ADT::SwitchEndianIf(CompatVersion, IsBE);
            return Dyld3::PackedVersion(Swapped);
        }
    };

    struct DylibCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return
                Kind == LoadCommandKind::LoadDylib ||
                Kind == LoadCommandKind::IdDylib ||
                Kind == LoadCommandKind::ReexportDylib ||
                Kind == LoadCommandKind::LazyLoadDylib ||
                Kind == LoadCommandKind::LoadUpwardDylib;
        }

        MachO::Dylib Dylib;

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylibCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto name(const bool IsBigEndian) const noexcept {
            return Dylib.Name.string(this, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto timestamp(const bool IsBigEndian) const noexcept {
            return Dylib.timestamp(IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto currentVersion(const bool IsBigEndian) const noexcept {
            return Dylib.currentVersion(IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto compatVersion(const bool IsBigEndian) const noexcept {
            return Dylib.compatVersion(IsBigEndian);
        }
    };

    struct SubFrameworkCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SubFramework;
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        constexpr auto umbrella(const bool IsBigEndian) const noexcept {
            return Umbrella.string(this, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubFrameworkCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct SubClientCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SubClient;
        }

        LoadCommandString Client;

        [[nodiscard]]
        constexpr auto client(const bool IsBigEndian) const noexcept {
            return Client.string(this, IsBigEndian);
        }
    };

    struct SubUmbrellaCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SubUmbrella;
        }

        LoadCommandString SubUmbrella;

        [[nodiscard]]
        constexpr auto subUmbrella(const bool IsBigEndian) const noexcept {
            return SubUmbrella.string(this, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubUmbrellaCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct SubLibraryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SubLibrary;
        }

        LoadCommandString SubLibrary;

        [[nodiscard]]
        constexpr auto subLibrary(const bool IsBigEndian) const noexcept {
            return SubLibrary.string(this, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubLibraryCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct LoadCommandBitVector {
        uint32_t Offset;

        [[nodiscard]] constexpr auto
        GetBitset(const LoadCommand *const Cmd,
                  const bool IsBE) const noexcept
        {
            const auto Offset = ADT::SwitchEndianIf(this->Offset, IsBE);
            const auto CmdSize = ADT::SwitchEndianIf(Cmd->CmdSize, IsBE);

            if (Offset >= CmdSize) {
                return std::optional<std::bitset<64>>();
            }

            const auto Ptr =
                reinterpret_cast<const void *>(
                    reinterpret_cast<const char *>(Cmd) + Offset);

            const auto Length = Cmd->CmdSize - Offset;
            auto BitSet = uint64_t();

            if (Length >= 8) {
                BitSet = *static_cast<const uint64_t *>(Ptr);
            } else if (Length >= 4) {
                BitSet = *static_cast<const uint32_t *>(Ptr);
            } else if (Length >= 2) {
                BitSet = *static_cast<const uint16_t *>(Ptr);
            } else if (Length >= 1) {
                BitSet = *static_cast<const uint8_t *>(Ptr);
            }

            return std::optional(std::bitset<64>(BitSet));
        }
    };

    struct PreboundDylibCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::PreBoundDylib;
        }

        LoadCommandString Name;
        uint32_t ModulesCount;

        [[nodiscard]]
        constexpr auto name(const bool IsBigEndian) const noexcept {
            return Name.string(this, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PreboundDylibCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct DylinkerCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return
                Kind == LoadCommandKind::LoadDylinker ||
                Kind == LoadCommandKind::IdDylinker;
        }

        LoadCommandString Name;

        [[nodiscard]]
        constexpr auto name(const bool IsBigEndian) const noexcept {
            return Name.string(this, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylinkerCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct RoutinesCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Routines;
        }

        uint32_t InitAddress;
        uint32_t InitModule;

        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;
        uint32_t Reserved4;
        uint32_t Reserved5;
        uint32_t Reserved6;

        [[nodiscard]]
        constexpr auto initAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InitAddress, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto initModule(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InitModule, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved1(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved2(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved3(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved3, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved4(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved4, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved5(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved5, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved6(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved6, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(RoutinesCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct RoutinesCommand64 : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Routines64;
        }

        uint64_t InitAddress;
        uint64_t InitModule;

        uint64_t Reserved1;
        uint64_t Reserved2;
        uint64_t Reserved3;
        uint64_t Reserved4;
        uint64_t Reserved5;
        uint64_t Reserved6;

        [[nodiscard]]
        constexpr auto initAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InitAddress, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto initModule(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InitModule, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved1(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved2(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved3(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved3, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved4(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved4, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved5(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved5, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved6(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved6, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(RoutinesCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct SymTabCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SymbolTable;
        }

        uint32_t SymOffset;
        uint32_t SymCount;
        uint32_t StrOffset;
        uint32_t StrSize;

        [[nodiscard]]
        constexpr auto symOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(SymOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto symCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(SymCount, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto strOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(StrOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto strSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(StrSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto strRange(const bool IsBE) const noexcept {
            const auto StrOffset = ADT::SwitchEndianIf(this->StrOffset, IsBE);
            const auto StrSize = ADT::SwitchEndianIf(this->StrSize, IsBE);

            return ADT::Range::FromSize(StrOffset, StrSize);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PreboundDylibCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct DynamicSymTabCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::DynamicSymbolTable;
        }

        uint32_t LocalSymbolsIndex;
        uint32_t LocalSymbolsCount;

        [[nodiscard]] constexpr
        auto localSymbolsIndex(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(LocalSymbolsIndex, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto localSymbolsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(LocalSymbolsCount, IsBigEndian);
        }

        uint32_t ExternDefSymbolsIndex;
        uint32_t ExternDefSymbolsCount;

        [[nodiscard]] constexpr
        auto externDefSymbolsIndex(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExternDefSymbolsIndex, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto externDefSymbolsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExternDefSymbolsCount, IsBigEndian);
        }

        uint32_t UndefSymbolsIndex;
        uint32_t UndefSymbolsCount;

        [[nodiscard]] constexpr
        auto undefDefSymbolsIndex(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(UndefSymbolsIndex, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto undefDefSymbolsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(UndefSymbolsCount, IsBigEndian);
        }

        uint32_t TableOfContentsOffset;
        uint32_t TableOfContentsCount;

        [[nodiscard]] constexpr
        auto tableOfContentsOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(TableOfContentsOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto tableOfContentsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(TableOfContentsCount, IsBigEndian);
        }

        uint32_t ModulesTabOffset;
        uint32_t ModulesTabCount;

        [[nodiscard]] constexpr
        auto modulesTabOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ModulesTabOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto modulesTabCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ModulesTabCount, IsBigEndian);
        }

        uint32_t ExternRefSymbolsOffset;
        uint32_t ExternRefSymbolsCount;

        [[nodiscard]] constexpr
        auto externRefSymbolsOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExternRefSymbolsOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto externRefSymbolsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExternRefSymbolsCount, IsBigEndian);
        }

        uint32_t IndirectSymbolsOffset;
        uint32_t IndirectSymbolsCount;

        [[nodiscard]] constexpr
        auto indirectSymbolsOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(IndirectSymbolsOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto indirectSymbolsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(IndirectSymbolsCount, IsBigEndian);
        }

        uint32_t ExternRelOffset;
        uint32_t ExternRelCount;

        [[nodiscard]] constexpr
        auto externRelOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExternRelOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto externRelCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExternRelCount, IsBigEndian);
        }

        uint32_t LocalRelOffset;
        uint32_t LocalRelCount;

        [[nodiscard]] constexpr
        auto localRelOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(LocalRelOffset, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto localRelCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(LocalRelCount, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DynamicSymTabCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(DynamicSymTabCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct DylibTableOfContents {
        uint32_t SymbolIndex;
        uint32_t ModuleIndex;
    };

    struct DylibModule {
        uint32_t ModuleNameStringTabIndex;

        uint32_t ExternDefSymbolsIndex;
        uint32_t ExternDefSymbolsCount;

        uint32_t RefSymbolsIndex;
        uint32_t RefSymbolsCount;

        uint32_t LocalSymbolIndex;
        uint32_t LocalSymbolCount;

        uint32_t ExternRelIndex;
        uint32_t ExternRelCount;

        uint32_t InitTermIndices;
        uint32_t InitTermCounts;

        [[nodiscard]]
        constexpr auto initIndex(const bool IsBigEndian) const noexcept {
            const auto InitTermIndices =
                ADT::SwitchEndianIf(this->InitTermIndices, IsBigEndian);

            return uint16_t(InitTermIndices);
        }

        [[nodiscard]]
        constexpr auto termIndex(const bool IsBigEndian) const noexcept {
            const auto InitTermIndices =
                ADT::SwitchEndianIf(this->InitTermIndices, IsBigEndian);

            return uint16_t(InitTermIndices >> 16);
        }

        [[nodiscard]]
        constexpr auto initCount(const bool IsBigEndian) const noexcept {
            const auto InitTermCounts =
                ADT::SwitchEndianIf(this->InitTermCounts, IsBigEndian);

            return uint16_t(InitTermCounts);
        }

        [[nodiscard]]
        constexpr auto termCount(const bool IsBigEndian) const noexcept {
            const auto InitTermCounts =
                ADT::SwitchEndianIf(this->InitTermCounts, IsBigEndian);

            return uint16_t(InitTermCounts >> 16);
        }

        uint32_t ObjcModuleInfoAddress;
        uint32_t ObjcModuleInfoSize;
    };

    struct DylibModule64 {
        uint32_t ModuleNameStringTabIndex;

        uint32_t ExternDefSymbolsIndex;
        uint32_t ExternDefSymbolsCount;

        uint32_t RefSymbolsIndex;
        uint32_t RefSymbolsCount;

        uint32_t LocalSymbolIndex;
        uint32_t LocalSymbolCount;

        uint32_t ExternRelIndex;
        uint32_t ExternRelCount;

        uint32_t InitTermIndices;
        uint32_t InitTermCounts;

        [[nodiscard]]
        constexpr auto initIndex(const bool IsBigEndian) const noexcept {
            const auto InitTermIndices =
                ADT::SwitchEndianIf(this->InitTermIndices, IsBigEndian);

            return uint16_t(InitTermIndices);
        }

        [[nodiscard]]
        constexpr auto termIndex(const bool IsBigEndian) const noexcept {
            const auto InitTermIndices =
                ADT::SwitchEndianIf(this->InitTermIndices, IsBigEndian);

            return uint16_t(InitTermIndices >> 16);
        }

        [[nodiscard]]
        constexpr auto initCount(const bool IsBigEndian) const noexcept {
            const auto InitTermCounts =
                ADT::SwitchEndianIf(this->InitTermCounts, IsBigEndian);

            return uint16_t(InitTermCounts);
        }

        [[nodiscard]]
        constexpr auto termCount(const bool IsBigEndian) const noexcept {
            const auto InitTermCounts =
                ADT::SwitchEndianIf(this->InitTermCounts, IsBigEndian);

            return uint16_t(InitTermCounts >> 16);
        }

        uint32_t ObjcModuleInfoSize;
        uint64_t ObjcModuleInfoAddr;
    };

    struct DylibReference {
        uint32_t SymbolIndex:24, Flags:8;
    };

    struct TwoLevelHint {
        uint32_t SubImageIndex:8, TableOfContentsIndex:24;
    };

    struct TwoLevelHintsCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::TwoLevelHints;
        }

        uint32_t Offset;
        uint32_t HintsCount;

        [[nodiscard]]
        constexpr auto offset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto hintsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(HintsCount, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(TwoLevelHintsCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(TwoLevelHintsCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct PrebindChecksumCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::PreBindChecksum;
        }

        uint32_t Checksum;

        [[nodiscard]]
        constexpr auto checksum(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Checksum, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PrebindChecksumCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(PrebindChecksumCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct UuidCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Uuid;
        }

        uint8_t Uuid[16];

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(UuidCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(UuidCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct RpathCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Rpath;
        }

        LoadCommandString Path;

        [[nodiscard]]
        constexpr auto path(const bool IsBigEndian) const noexcept {
            return Path.string(this, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RpathCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct LinkeditDataCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return
                Kind == LoadCommandKind::CodeSignature ||
                Kind == LoadCommandKind::SegmentSplitInfo ||
                Kind == LoadCommandKind::FunctionStarts ||
                Kind == LoadCommandKind::DyldEnvironment ||
                Kind == LoadCommandKind::DataInCode ||
                Kind == LoadCommandKind::DylibCodeSignDRS ||
                Kind == LoadCommandKind::LinkerOptimizationHint ||
                Kind == LoadCommandKind::DyldExportsTrie ||
                Kind == LoadCommandKind::DyldChainedFixups;
        }

        uint32_t DataOff;
        uint32_t DataSize;

        [[nodiscard]]
        constexpr auto dataOff(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(DataOff, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto dataSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(DataSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto dataRange(const bool IsBE) const noexcept {
            const auto DataOff = ADT::SwitchEndianIf(this->DataOff, IsBE);
            const auto DataSize = ADT::SwitchEndianIf(this->DataSize, IsBE);

            return ADT::Range::FromSize(DataOff, DataSize);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkeditDataCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(LinkeditDataCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct FileSetEntryCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::FileSetEntry;
        }

        uint64_t VmAddress;
        uint64_t FileOffset;

        uint32_t EntryId;
        uint32_t Reserved;

        [[nodiscard]]
        constexpr auto vmAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(VmAddress, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto fileOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(FileOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto entryId(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(EntryId, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FileSetEntryCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(FileSetEntryCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct EncryptionInfoCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::EncryptionInfo;
        }

        uint32_t CryptOffset;
        uint32_t CryptSize;
        uint32_t CryptId;

        [[nodiscard]]
        constexpr auto cryptOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CryptOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cryptSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CryptSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cryptId(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CryptId, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cryptRange(const bool IsBigEndian) const noexcept {
            const auto CryptOffset =
                ADT::SwitchEndianIf(this->CryptOffset, IsBigEndian);
            const auto CryptSize =
                ADT::SwitchEndianIf(this->CryptSize, IsBigEndian);

            return ADT::Range::FromSize(CryptOffset, CryptSize);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(EncryptionInfoCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

   struct EncryptionInfo64Command : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::EncryptionInfo64;
        }

        uint32_t CryptOffset;
        uint32_t CryptSize;
        uint32_t CryptId;
        uint32_t Pad;

        [[nodiscard]]
        constexpr auto cryptOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CryptOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cryptSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CryptSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cryptId(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(CryptId, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto pad(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Pad, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cryptRange(const bool IsBigEndian) const noexcept {
            const auto CryptOffset =
                ADT::SwitchEndianIf(this->CryptOffset, IsBigEndian);
            const auto CryptSize =
                ADT::SwitchEndianIf(this->CryptSize, IsBigEndian);

            return ADT::Range::FromSize(CryptOffset, CryptSize);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfo64Command)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(EncryptionInfo64Command)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct VersionMinCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return
                Kind == LoadCommandKind::VersionMinMacOS ||
                Kind == LoadCommandKind::VersionMinIOS ||
                Kind == LoadCommandKind::VersionMinTVOS ||
                Kind == LoadCommandKind::VersionMinWatchOS;
        }

        uint32_t Version;
        uint32_t Sdk;

        [[nodiscard]]
        constexpr auto version(const bool IsBE) const noexcept {
            return Dyld3::PackedVersion(ADT::SwitchEndianIf(Version, IsBE));
        }

        [[nodiscard]]
        constexpr auto sdk(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion(ADT::SwitchEndianIf(Sdk, IsBigEndian));
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(VersionMinCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(VersionMinCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    enum class BuildTool : uint32_t {
        Clang = 1,
        Swift,
        Ld
    };

    struct BuildToolVersion {
        uint32_t Tool;
        uint32_t Version;

        [[nodiscard]]
        constexpr auto tool(const bool IsBigEndian) const noexcept {
            return BuildTool(ADT::SwitchEndianIf(Tool, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto version(const bool IsBE) const noexcept {
            return Dyld3::PackedVersion(ADT::SwitchEndianIf(Version, IsBE));
        }
    };

    struct BuildVersionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::BuildVersion;
        }

        uint32_t Platform;

        uint32_t MinOS;
        uint32_t Sdk;

        uint32_t ToolsCount;

        [[nodiscard]]
        constexpr auto platform(const bool IsBigEndian) const noexcept {
            return Dyld3::Platform(ADT::SwitchEndianIf(Platform, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto minOS(const bool IsBE) const noexcept {
            return Dyld3::PackedVersion(ADT::SwitchEndianIf(MinOS, IsBE));
        }

        [[nodiscard]]
        constexpr auto sdk(const bool IsBigEndian) const noexcept {
            return Dyld3::PackedVersion(ADT::SwitchEndianIf(Sdk, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto toolsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ToolsCount, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(BuildVersionCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            if (const auto Result = hasValidCmdSize(cmdsize(IsBigEndian));
                Result != CmdSizeInvalidKind::None)
            {
                return Result;
            }

            const auto ToolListSize = sizeof(BuildTool) * ToolsCount;
            if (sizeof(*this) + ToolListSize < cmdsize(IsBigEndian)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }
    };

    struct DyldInfoCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return
                Kind == LoadCommandKind::DyldInfo ||
                Kind == LoadCommandKind::DyldInfoOnly;
        }

        uint32_t RebaseOffset;
        uint32_t RebaseSize;

        [[nodiscard]]
        constexpr auto rebaseOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(RebaseOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto rebaseSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(RebaseSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto rebaseRange(const bool IsBigEndian) const noexcept {
            const auto RebaseOffset =
                ADT::SwitchEndianIf(this->RebaseOffset, IsBigEndian);
            const auto RebaseSize =
                ADT::SwitchEndianIf(this->RebaseSize, IsBigEndian);

            return ADT::Range::FromSize(RebaseOffset, RebaseSize);
        }

        uint32_t BindOffset;
        uint32_t BindSize;

        [[nodiscard]]
        constexpr auto bindOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BindOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto bindSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BindSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto bindRange(const bool IsBigEndian) const noexcept {
            const auto BindSize =
                ADT::SwitchEndianIf(this->BindSize, IsBigEndian);
            const auto BindOffset =
                ADT::SwitchEndianIf(this->BindOffset, IsBigEndian);

            return ADT::Range::FromSize(BindOffset, BindSize);
        }

        uint32_t WeakBindOffset;
        uint32_t WeakBindSize;

        [[nodiscard]]
        constexpr auto weakBindOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(WeakBindOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto weakBindSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(WeakBindSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto weakBindRange(const bool IsBigEndian) const noexcept {
            const auto WeakBindSize =
                ADT::SwitchEndianIf(this->WeakBindSize, IsBigEndian);
            const auto WeakBindOffset =
                ADT::SwitchEndianIf(this->WeakBindOffset, IsBigEndian);

            return ADT::Range::FromSize(WeakBindOffset, WeakBindSize);
        }

        uint32_t LazyBindOffset;
        uint32_t LazyBindSize;

        [[nodiscard]]
        constexpr auto lazyBindOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(WeakBindOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto lazyBindSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(WeakBindSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto lazyBindRange(const bool IsBigEndian) const noexcept {
            const auto LazyBindSize =
                ADT::SwitchEndianIf(this->LazyBindSize, IsBigEndian);
            const auto LazyBindOffset =
                ADT::SwitchEndianIf(this->LazyBindOffset, IsBigEndian);

            return ADT::Range::FromSize(LazyBindOffset, LazyBindSize);
        }

        uint32_t ExportTrieOffset;
        uint32_t ExportTrieSize;

        [[nodiscard]]
        constexpr auto exportTrieOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExportTrieOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto exportTrieSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ExportTrieSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto exportTrieRange(const bool IsBigEndian) const noexcept {
            const auto ExportTrieOffset =
                ADT::SwitchEndianIf(this->ExportTrieOffset, IsBigEndian);
            const auto ExportTrieSize =
                ADT::SwitchEndianIf(this->ExportTrieSize, IsBigEndian);

            return ADT::Range::FromSize(ExportTrieOffset, ExportTrieSize);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DyldInfoCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(DyldInfoCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct LinkerOptionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::LinkerOption;
        }

        uint32_t Count;

        [[nodiscard]]
        constexpr auto count(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Count, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkerOptionCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(LinkerOptionCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct SymbolSegmentCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SymbolSegment;
        }

        uint32_t Offset;
        uint32_t Size;

        [[nodiscard]]
        constexpr auto offset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto size(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Size, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto range(const bool IsBigEndian) const noexcept {
            const auto Offset = ADT::SwitchEndianIf(this->Offset, IsBigEndian);
            const auto Size = ADT::SwitchEndianIf(this->Size, IsBigEndian);

            return ADT::Range::FromSize(Offset, Size);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymbolSegmentCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(SymbolSegmentCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct FvmFileCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::FixedVMFile;
        }

        LoadCommandString Name;
        uint32_t HeaderAddress;

        [[nodiscard]]
        constexpr auto name(const bool IsBigEndian) const noexcept {
            return Name.string(this, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto headerAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(HeaderAddress, IsBigEndian);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FvmFileCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct EntryPointCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Main;
        }

        uint64_t EntryOffset;
        uint64_t StackSize;

        [[nodiscard]]
        constexpr auto entryOffset(const bool IsBigEndian) const noexcept {
            return EntryOffset;
        }

        [[nodiscard]]
        constexpr auto stackSize(const bool IsBigEndian) const noexcept {
            return StackSize;
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EntryPointCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(EntryPointCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    struct SourceVersionCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::SourceVersion;
        }

        uint64_t Version;

        [[nodiscard]]
        constexpr auto version(const bool IsBE) const noexcept {
            return Dyld3::PackedVersion64(ADT::SwitchEndianIf(Version, IsBE));
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SourceVersionCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    enum class DataInCodeEntryKind : uint16_t {
        Data = 1,
        JumpTables8,
        JumpTables16,
        JumpTables32,
        AbsJumpTable32
    };

    [[nodiscard]] constexpr
    auto DataInCodeEntryKindIsValid(const DataInCodeEntryKind Kind) noexcept {
        switch (Kind) {
            case DataInCodeEntryKind::Data:
            case DataInCodeEntryKind::JumpTables8:
            case DataInCodeEntryKind::JumpTables16:
            case DataInCodeEntryKind::JumpTables32:
            case DataInCodeEntryKind::AbsJumpTable32:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto
    DataInCodeEntryKindGetString(const DataInCodeEntryKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case DataInCodeEntryKind::Data:
                return "DICE_KIND_DATA";
            case DataInCodeEntryKind::JumpTables8:
                return "DICE_KIND_JUMP_TABLE8";
            case DataInCodeEntryKind::JumpTables16:
                return "DICE_KIND_JUMP_TABLE16";
            case DataInCodeEntryKind::JumpTables32:
                return "DICE_KIND_JUMP_TABLE32";
            case DataInCodeEntryKind::AbsJumpTable32:
                return "DICE_KIND_ABS_JUMP_TABLE32";
        }

        assert(false &&
               "MachO::DataInCodeEntryKindGetString() called with unknown "
               "DataInCodeEntryKind");
    }

    struct DataInCodeEntry {
        uint32_t Offset;
        uint16_t Length;

        [[nodiscard]]
        constexpr auto dataRange(const bool IsBigEndian) const noexcept {
            const auto Offset = ADT::SwitchEndianIf(this->Offset, IsBigEndian);
            const auto Length = ADT::SwitchEndianIf(this->Length, IsBigEndian);

            return ADT::Range::FromSize(Offset, Length);
        }

        DataInCodeEntryKind Kind;
    };

    struct NoteCommand : public LoadCommand {
        [[nodiscard]]
        constexpr static bool IsOfKind(const LoadCommandKind Kind) noexcept {
            return Kind == LoadCommandKind::Note;
        }

        char DataOwner[16];

        [[nodiscard]] constexpr auto dataOwner() const noexcept {
            const auto Length = strnlen(DataOwner, sizeof(DataOwner));
            return std::string_view(DataOwner, Length);
        }

        uint64_t Offset;
        uint64_t Size;

        [[nodiscard]]
        constexpr auto offset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Offset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto size(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Size, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto noteRange(const bool IsBigEndian) const noexcept {
            const auto Offset = ADT::SwitchEndianIf(this->Offset, IsBigEndian);
            const auto Size = ADT::SwitchEndianIf(this->Size, IsBigEndian);

            return ADT::Range::FromSize(Offset, Size);
        }

        [[nodiscard]] constexpr static auto
        hasValidCmdSize(const uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(NoteCommand)) {
                return CmdSizeInvalidKind::TooSmall;
            } else if (CmdSize > sizeof(NoteCommand)) {
                return CmdSizeInvalidKind::TooLarge;
            }

            return CmdSizeInvalidKind::None;
        }

        [[nodiscard]]
        constexpr auto hasValidCmdSize(const bool IsBigEndian) noexcept {
            return hasValidCmdSize(cmdsize(IsBigEndian));
        }
    };

    template <LoadCommandKind Kind>
    struct LoadCommandTypeFromKind {};

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Segment> {
        using type = SegmentCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SymbolTable> {
        using type = SymTabCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SymbolSegment> {
        using type = SymbolSegmentCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::UnixThread> {
        using type = ThreadCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LoadFixedVMSharedLib> {
        using type = FvmLibraryCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::IdFixedVMSharedLib> {
        using type = FvmLibraryCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Identity> {
        using type = IdentCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::FixedVMFile> {
        using type = FvmFileCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::PrePage> {
        using type = LoadCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DynamicSymbolTable> {
        using type = DynamicSymTabCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LoadDylib> {
        using type = DylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::IdDylib> {
        using type = DylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LoadDylinker> {
        using type = DylinkerCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::IdDylinker> {
        using type = DylinkerCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::PreBoundDylib> {
        using type = PreboundDylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Routines> {
        using type = RoutinesCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SubFramework> {
        using type = SubFrameworkCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SubUmbrella> {
        using type = SubUmbrellaCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SubClient> {
        using type = SubClientCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SubLibrary> {
        using type = SubLibraryCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::TwoLevelHints> {
        using type = TwoLevelHintsCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::PreBindChecksum> {
        using type = PrebindChecksumCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LoadWeakDylib> {
        using type = DylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Segment64> {
        using type = SegmentCommand64;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Routines64> {
        using type = RoutinesCommand64;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Uuid> {
        using type = UuidCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Rpath> {
        using type = RpathCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::CodeSignature> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SegmentSplitInfo> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::ReexportDylib> {
        using type = DylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LazyLoadDylib> {
        using type = DylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::EncryptionInfo> {
        using type = EncryptionInfoCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DyldInfo> {
        using type = EncryptionInfoCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DyldInfoOnly> {
        using type = EncryptionInfoCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LoadUpwardDylib> {
        using type = DylibCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::VersionMinMacOS> {
        using type = VersionMinCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::VersionMinIOS> {
        using type = VersionMinCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::FunctionStarts> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DyldEnvironment> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Main> {
        using type = EntryPointCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DataInCode> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::SourceVersion> {
        using type = SourceVersionCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DylibCodeSignDRS> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::EncryptionInfo64> {
        using type = EncryptionInfo64Command;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LinkerOption> {
        using type = LinkerOptionCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::LinkerOptimizationHint> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::VersionMinTVOS> {
        using type = VersionMinCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::VersionMinWatchOS> {
        using type = VersionMinCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::Note> {
        using type = NoteCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::BuildVersion> {
        using type = BuildVersionCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DyldExportsTrie> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::DyldChainedFixups> {
        using type = LinkeditDataCommand;
    };

    template <>
    struct LoadCommandTypeFromKind<LoadCommandKind::FileSetEntry> {
        using type = FileSetEntryCommand;
    };

    template <typename T>
    concept LoadCommandDerived = std::is_base_of_v<LoadCommand, T>;

    template <LoadCommandDerived T>
    [[nodiscard]]
    constexpr auto isa(const LoadCommand *const LC, bool IsBigEndian) noexcept {
        return T::IsOfKind(LC->kind(IsBigEndian));
    }

    template <LoadCommandKind Kind>
    [[nodiscard]]
    constexpr auto isa(const LoadCommand *const LC, bool IsBigEndian) noexcept {
        return LC->kind(IsBigEndian) == Kind;
    }

    template <LoadCommandDerived First,
              LoadCommandDerived Second,
              LoadCommandDerived... Rest>
    [[nodiscard]]
    constexpr auto isa(const LoadCommand *const LC, bool IsBE) noexcept {
        return isa<First>(LC, IsBE) || isa<Second, Rest...>(LC, IsBE);
    }

    template <LoadCommandKind First,
              LoadCommandKind Second,
              LoadCommandKind... Rest>
    [[nodiscard]]
    constexpr auto isa(const LoadCommand *const LC, bool IsBE) noexcept {
        return isa<First>(LC, IsBE) || isa<Second, Rest...>(LC, IsBE);
    }

    template <LoadCommandDerived T>
    [[nodiscard]]
    constexpr auto &cast(LoadCommand *const LC, bool IsBigEndian) noexcept {
        assert(isa<T>(LC, IsBigEndian));
        return static_cast<T *>(LC);
    }

    template <LoadCommandDerived T>
    [[nodiscard]] constexpr
    auto &cast(const LoadCommand *const LC, const bool IsBigEndian) noexcept {
        assert(isa<T>(LC, IsBigEndian));
        return static_cast<const T *>(LC);
    }

    template <LoadCommandKind Kind>
    [[nodiscard]]
    constexpr auto &cast(LoadCommand *const LC, bool IsBigEndian) noexcept {
        assert(isa<Kind>(LC, IsBigEndian));

        using T = typename LoadCommandTypeFromKind<Kind>::type;
        return static_cast<T *>(LC);
    }

    template <LoadCommandKind Kind>
    [[nodiscard]] constexpr
    auto &cast(const LoadCommand *const LC, const bool IsBigEndian) noexcept {
        assert(isa<Kind>(LC, IsBigEndian));

        using T = typename LoadCommandTypeFromKind<Kind>::type;
        return static_cast<const T *>(LC);
    }

    template <LoadCommandDerived T>
    [[nodiscard]]
    constexpr auto &cast(LoadCommand &LC, bool IsBigEndian) noexcept {
        assert(isa<T>(&LC, IsBigEndian));
        return static_cast<T &>(LC);
    }

    template <LoadCommandDerived T>
    [[nodiscard]] constexpr
    auto &cast(const LoadCommand &LC, const bool IsBigEndian) noexcept {
        assert(isa<T>(&LC, IsBigEndian));
        return static_cast<const T &>(LC);
    }

    template <LoadCommandKind Kind>
    [[nodiscard]]
    constexpr auto &cast(LoadCommand &LC, bool IsBigEndian) noexcept {
        assert(isa<Kind>(&LC, IsBigEndian));

        using T = typename LoadCommandTypeFromKind<Kind>::type;
        return static_cast<T &>(LC);
    }

    template <LoadCommandKind Kind>
    [[nodiscard]] constexpr
    auto &cast(const LoadCommand &LC, const bool IsBigEndian) noexcept {
        assert(isa<Kind>(&LC, IsBigEndian));

        using T = typename LoadCommandTypeFromKind<Kind>::type;
        return static_cast<const T &>(LC);
    }

    template <LoadCommandDerived T>
    [[nodiscard]] constexpr
    auto dyn_cast(LoadCommand *const LC, const bool IsBigEndian) noexcept
        -> T *
    {
        if (isa<T>(LC, IsBigEndian)) {
            return static_cast<T *>(LC);
        }

        return nullptr;
    }

    template <LoadCommandDerived T>
    [[nodiscard]] constexpr auto
    dyn_cast(const LoadCommand *const LC, const bool IsBigEndian) noexcept
        -> const T *
    {
        if (isa<T>(LC, IsBigEndian)) {
            return static_cast<const T *>(LC);
        }

        return nullptr;
    }

    template <LoadCommandKind Kind>
    [[nodiscard]]
    constexpr auto dyn_cast(LoadCommand *const LC, bool IsBigEndian) noexcept {
        using T = typename LoadCommandTypeFromKind<Kind>::type;
        if (isa<Kind>(LC, IsBigEndian)) {
            return static_cast<T *>(LC);
        }

        return static_cast<T *>(nullptr);
    }

    template <LoadCommandKind Kind>
    [[nodiscard]] constexpr auto
    dyn_cast(const LoadCommand *const LC, const bool IsBigEndian) noexcept {
        using T = typename LoadCommandTypeFromKind<Kind>::type;
        if (isa<Kind>(LC, IsBigEndian)) {
            return static_cast<const T *>(LC);
        }

        return static_cast<const T *>(nullptr);
    }

    template <LoadCommandKind First,
              LoadCommandKind Second,
              LoadCommandKind... Rest>

    [[nodiscard]] constexpr
    auto dyn_cast(LoadCommand *const LC, const bool IsBigEndian) noexcept {
        using T = typename LoadCommandTypeFromKind<First>::type;
        using U = typename LoadCommandTypeFromKind<Second>::type;

        static_assert(
            std::is_same_v<T, U> &&
            std::conjunction<
                std::is_same<
                    T,
                    typename LoadCommandTypeFromKind<Rest>::type>...>::value,
            "Types of the several LoadCommandKinds don't match!");

        if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
            return static_cast<T *>(LC);
        }

        return static_cast<T *>(nullptr);
    }

    template <LoadCommandKind First,
              LoadCommandKind Second,
              LoadCommandKind... Rest>

    [[nodiscard]] constexpr auto
    dyn_cast(const LoadCommand *const LC, const bool IsBigEndian) noexcept {
        using T = typename LoadCommandTypeFromKind<First>::type;
        using U = typename LoadCommandTypeFromKind<Second>::type;

        static_assert(
            std::is_same_v<T, U> &&
            std::conjunction<
                std::is_same<
                    T,
                    typename LoadCommandTypeFromKind<Rest>::type>...>::value,
            "Types of the several LoadCommandKinds don't match!");

        if (isa<First, Second, Rest...>(LC, IsBigEndian)) {
            return static_cast<const T *>(LC);
        }

        return static_cast<const T *>(nullptr);
    }

}