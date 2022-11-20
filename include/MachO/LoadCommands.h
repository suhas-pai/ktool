//
//  MachO/LoadCommands.h
//  ktool
//
//  Created by suhaspai on 10/5/22.
//

#pragma once

#include <bitset>
#include <optional>
#include <string_view>

#include "ADT/FlagsBase.h"
#include "ADT/Range.h"

#include "Dyld3/PackedVersion.h"
#include "Dyld3/Platform.h"

#include "Mach/VmProt.h"

namespace MachO {
    struct LoadCommand {
        static constexpr auto LoadCommandReqByDyld = 1 << 31;
        enum class KindEnum : uint32_t {
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
            LoadWeakDylib = static_cast<uint32_t>(LoadCommandReqByDyld | 0x22),
            Segment64 = 0x23,
            Routines64,
            Uuid,
            RPath,
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

        KindEnum Kind;
        uint32_t CmdSize;
    };

    static_assert(sizeof(LoadCommand) == 8,
                  "struct LoadCommand doesn't have CmdSize 8");

    struct SegmentCommand : public LoadCommand {
        char SegmentName[16];

        [[nodiscard]] constexpr auto getSegmentName() const noexcept {
            const auto Length = strnlen(SegmentName, sizeof(SegmentName));
            return std::string_view(SegmentName, Length);
        }

        uint32_t VmAddr;
        uint32_t VmSize;

        [[nodiscard]] constexpr auto getVmRange() const noexcept {
            return ADT::Range::FromSize(VmAddr, VmSize);
        }

        uint32_t FileOffset;
        uint32_t FileSize;

        [[nodiscard]] constexpr auto getFileRange() const noexcept {
            return ADT::Range::FromSize(FileOffset, FileSize);
        }

        Mach::VmProt MaxProt;
        Mach::VmProt InitProt;

        uint32_t SectionCount;

        struct FlagsStruct : public ADT::FlagsBase<uint32_t> {
        public:
            enum class Kind {
                HighVm = 1 << 0,
                FixedVmLibrary = 1 << 1,
                NoRelocations = 1 << 2,
                ProtectedVersion1 = 1 << 3,
                ReadOnly = 1 << 4
            };

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

        FlagsStruct Flags;

        struct Section {
            char SegmentName[16];
            char SectionName[16];

            [[nodiscard]] constexpr auto getSegmentName() const noexcept {
                const auto Length = strnlen(SegmentName, sizeof(SegmentName));
                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr auto getSectionName() const noexcept {
                const auto Length = strnlen(SectionName, sizeof(SectionName));
                return std::string_view(SectionName, Length);
            }

            uint32_t Addr;
            uint32_t Size;

            [[nodiscard]] constexpr auto getVirtualRange() const noexcept {
                return ADT::Range::FromSize(Addr, Size);
            }

            uint32_t FileOffset;

            [[nodiscard]] constexpr auto getFileRange() const noexcept {
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
                static constexpr uint32_t KindMask = 0xFF;
                static constexpr auto AttributesMask = ~KindMask;
            public:
                [[nodiscard]] constexpr auto getKind() const noexcept {
                    return Kind(Flags & KindMask);
                }

                [[nodiscard]] constexpr auto getAttributes() const noexcept {
                    return Attributes(Flags & AttributesMask);
                }
            };

            FlagsStruct Flags;

            uint32_t Reserved1;
            uint32_t Reserved2;
        };
    };

    struct SegmentCommand64 : public LoadCommand {
        char SegmentName[16];

        [[nodiscard]] constexpr auto getSegmentName() const noexcept {
            const auto Length = strnlen(SegmentName, sizeof(SegmentName));
            return std::string_view(SegmentName, Length);
        }

        uint64_t VmAddr;
        uint64_t VmSize;

        [[nodiscard]] constexpr auto getVmRange() const noexcept {
            return ADT::Range::FromSize(VmAddr, VmSize);
        }

        uint64_t FileOffset;
        uint64_t FileSize;

        [[nodiscard]] constexpr auto getFileRange() const noexcept {
            return ADT::Range::FromSize(FileOffset, FileSize);
        }

        Mach::VmProt MaxProt;
        Mach::VmProt InitProt;

        uint32_t SectionCount;

        using FlagsStruct = SegmentCommand::FlagsStruct;
        struct Section {
            char SegmentName[16];
            char SectionName[16];

            [[nodiscard]] constexpr auto getSegmentName() const noexcept {
                const auto Length = strnlen(SegmentName, sizeof(SegmentName));
                return std::string_view(SegmentName, Length);
            }

            [[nodiscard]] constexpr auto getSectionName() const noexcept {
                const auto Length = strnlen(SectionName, sizeof(SectionName));
                return std::string_view(SectionName, Length);
            }

            uint64_t Addr;
            uint64_t Size;

            [[nodiscard]] constexpr auto getVirtualRange() const noexcept {
                return ADT::Range::FromSize(Addr, Size);
            }

            uint32_t FileOffset;

            [[nodiscard]] constexpr auto getFileRange() const noexcept {
                return ADT::Range::FromSize(FileOffset, Size);
            }

            uint32_t Align;
            uint32_t RelocFileOffset;
            uint32_t RelocsCount;

            using FlagsStruct = SegmentCommand::Section::FlagsStruct;
            using Attributes = SegmentCommand::Section::Attributes;
            using Kind = SegmentCommand::Section::Kind;

            FlagsStruct Flags;
        };
        
        FlagsStruct Flags;
    };

    struct LoadCommandString {
        uint32_t Offset;

        [[nodiscard]]
        constexpr auto getString(const LoadCommand *const Cmd) const noexcept {
            if (Offset >= Cmd->CmdSize) {
                return std::optional<std::string_view>();
            }

            const auto Ptr = reinterpret_cast<const char *>(Cmd);
            const auto Length = strnlen(Ptr + Offset, Cmd->CmdSize - Offset);

            return std::optional(std::string_view(Ptr + Offset, Length));
        }
    };

    struct FvmLibrary {
        LoadCommandString Name;
        Dyld3::PackedVersion MinorVersion;
        uint32_t HeaderAddress;
    };

    struct FvmLibraryCommand : public LoadCommand {
        FvmLibrary Library;
    };

    struct Dylib {
        LoadCommandString Name;
        uint32_t Timestamp;

        Dyld3::PackedVersion CurrentVersion;
        Dyld3::PackedVersion CompatVersion;
    };

    struct DylibCommand : public LoadCommand {
        Dylib Dylib;
    };

    struct SubFrameworkCommand : public LoadCommand {
        LoadCommandString Umbrella;

        [[nodiscard]] constexpr auto getUmbrella() const noexcept {
            return Umbrella.getString(this);
        }
    };

    struct SubClientCommand : public LoadCommand {
        LoadCommandString Client;

        [[nodiscard]] constexpr auto getClient() const noexcept {
            return Client.getString(this);
        }
    };

    struct SubUmbrellaCommand : public LoadCommand {
        LoadCommandString SubUmbrella;

        [[nodiscard]] constexpr auto getSubUmbrella() const noexcept {
            return SubUmbrella.getString(this);
        }
    };

    struct SubLibraryCommand : public LoadCommand {
        LoadCommandString SubLibrary;

        [[nodiscard]] constexpr auto getSubLibrary() const noexcept {
            return SubLibrary.getString(this);
        }
    };

    struct LoadCommandBitVector {
        uint32_t Offset;

        [[nodiscard]]
        constexpr auto GetBitset(const LoadCommand *const Cmd) const noexcept {
            if (Offset >= Cmd->CmdSize) {
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
        LoadCommandString Name;
        uint32_t ModulesCount;
    };

    struct DylinkerCommand : public LoadCommand {
        LoadCommandString Name;
    };

    struct RoutinesCommand : public LoadCommand {
        uint32_t InitAddress;
        uint32_t InitModule;

        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;
        uint32_t Reserved4;
        uint32_t Reserved5;
        uint32_t Reserved6;
    };

    struct RoutinesCommand64 : public LoadCommand {
        uint64_t InitAddress;
        uint64_t InitModule;

        uint64_t Reserved1;
        uint64_t Reserved2;
        uint64_t Reserved3;
        uint64_t Reserved4;
        uint64_t Reserved5;
        uint64_t Reserved6;
    };

    struct SymTabCommand : public LoadCommand {
        uint32_t SymOffset;
        uint32_t SymCount;
        uint32_t StrOffset;
        uint32_t StrSize;

        [[nodiscard]] constexpr auto getStrRange() const noexcept {
            return ADT::Range::FromSize(StrOffset, StrSize);
        }
    };

    struct DynamicSymTab : public LoadCommand {
        uint32_t LocalSymbolsIndex;
        uint32_t LocalSymbolsCount;

        uint32_t ExternDefSymbolsIndex;
        uint32_t ExternDefSymbolsCount;

        uint32_t UndefSymbolsIndex;
        uint32_t UndefSymbolsCount;

        uint32_t TableOfContentsOffset;
        uint32_t TableOfContentsCount;

        uint32_t ModulesTabOffset;
        uint32_t ModulesTabCount;

        uint32_t ExternRefSymbolsOffset;
        uint32_t ExternRefSymbolsCount;

        uint32_t IndirectSymbolsOffset;
        uint32_t IndirectSymbolsCount;

        uint32_t ExternRelOffset;
        uint32_t ExternRelCount;

        uint32_t LocalRelOffset;
        uint32_t LocalRelCount;
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

        [[nodiscard]] constexpr auto getInitIndex() const noexcept {
            return uint16_t(InitTermIndices);
        }

        [[nodiscard]] constexpr auto getTermIndex() const noexcept {
            return uint16_t(InitTermIndices >> 16);
        }

        [[nodiscard]] constexpr auto getInitCount() const noexcept {
            return uint16_t(InitTermCounts);
        }

        [[nodiscard]] constexpr auto getTermCount() const noexcept {
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

        [[nodiscard]] constexpr auto getInitIndex() const noexcept {
            return uint16_t(InitTermIndices);
        }

        [[nodiscard]] constexpr auto getTermIndex() const noexcept {
            return uint16_t(InitTermIndices >> 16);
        }

        [[nodiscard]] constexpr auto getInitCount() const noexcept {
            return uint16_t(InitTermCounts);
        }

        [[nodiscard]] constexpr auto getTermCount() const noexcept {
            return uint16_t(InitTermCounts >> 16);
        }

        uint32_t ObjcModuleInfoSize;
        uint64_t ObjcModuleInfoAddr;
    };

    struct DylibReference {
        uint32_t SymbolIndex:24, Flags:8;
    };

    struct TwoLevelHintsCommand : public LoadCommand {
        uint32_t Offset;
        uint32_t HintsCount;
    };

    struct TwoLevelHint {
        uint32_t SubImageIndex:8, TableOfContentsIndex:24;
    };

    struct PrebindChecksumCommand : public LoadCommand {
        uint32_t Checksum;
    };

    struct UuidCommand : public LoadCommand {
        uint8_t Uuid[16];
    };

    struct RpathCommand : public LoadCommand {
        LoadCommandString Path;
    };

    struct LinkeditDataCommand : public LoadCommand {
        uint32_t DataOff;
        uint32_t DataSize;

        [[nodiscard]] constexpr auto getDataRange() const noexcept {
            return ADT::Range::FromSize(DataOff, DataSize);
        }
    };

    struct FilesetEntryCommand : public LoadCommand {
        uint64_t VmAddress;
        uint64_t FileOffset;

        uint32_t EntryId;
        uint32_t Reserved;
    };

    struct EncryptionInfoCommand : public LoadCommand {
        uint32_t CryptOffset;
        uint32_t CryptSize;
        uint32_t CryptId;
    };

    struct EncryptionInfo64Command : public LoadCommand {
        uint32_t CryptOffset;
        uint32_t CryptSize;
        uint32_t CryptId;
        uint32_t Pad;
    };

    struct VersionMinCommand : public LoadCommand {
        Dyld3::PackedVersion Version;
        Dyld3::PackedVersion Sdk;
    };

    struct BuildVersionCommand : public LoadCommand {
        Dyld3::Platform Platform;

        Dyld3::PackedVersion MinimumOS;
        Dyld3::PackedVersion Sdk;

        uint32_t ToolsCount;
    };

    enum class BuildTool : uint32_t {
        Clang = 1,
        Swift,
        Ld
    };

    struct BuildToolVersion {
        BuildTool Tool;
        Dyld3::PackedVersion Version;
    };

    struct DyldInfoCommand : public LoadCommand {
        uint32_t RebaseOffset;
        uint32_t RebaseSize;

        [[nodiscard]] constexpr auto getRebaseRange() const noexcept {
            return ADT::Range::FromSize(RebaseOffset, RebaseSize);
        }

        uint32_t BindOffset;
        uint32_t BindSize;

        [[nodiscard]] constexpr auto getBindRange() const noexcept {
            return ADT::Range::FromSize(BindOffset, BindSize);
        }

        uint32_t WeakBindOffset;
        uint32_t WeakBindSize;

        [[nodiscard]] constexpr auto getWeakBindRange() const noexcept {
            return ADT::Range::FromSize(WeakBindOffset, WeakBindSize);
        }

        uint32_t LazyBindOffset;
        uint32_t LazyBindSize;

        [[nodiscard]] constexpr auto getLazyBindRange() const noexcept {
            return ADT::Range::FromSize(LazyBindOffset, LazyBindSize);
        }

        uint32_t ExportTrieOffset;
        uint32_t ExportTrieSize;

        [[nodiscard]] constexpr auto getExportTrieRange() const noexcept {
            return ADT::Range::FromSize(ExportTrieOffset, ExportTrieSize);
        }
    };

    struct LinkerOptionCommand : public LoadCommand {
        uint32_t Count;
    };

    struct SymbolSegmentCommand : public LoadCommand {
        uint32_t Offset;
        uint32_t Size;

        [[nodiscard]] constexpr auto getRange() const noexcept {
            return ADT::Range::FromSize(Offset, Size);
        }
    };

    struct FvmFileCommand : public LoadCommand {
        LoadCommandString Name;
        uint32_t HeaderAddress;
    };

    struct EntryPointCommand : public LoadCommand {
        uint64_t EntryOffset;
        uint64_t StackSize;
    };

    struct SourceVersionCommand : public LoadCommand {
        Dyld3::PackedVersion64 Version;
    };

    struct DataInCodeEntry {
        enum class KindEnum : uint16_t {
            Data = 1,
            JumpTables8,
            JumpTables16,
            JumpTables32,
            AbsJumpTable32
        };

        uint32_t Offset;
        uint16_t Length;

        [[nodiscard]] constexpr auto getDataRange() const noexcept {
            return ADT::Range::FromSize(Offset, Length);
        }

        KindEnum Kind;
    };

    struct NoteCommand : public LoadCommand {
        char DataOwner[16];

        [[nodiscard]] constexpr auto getDataOwner() const noexcept {
            const auto Length = strnlen(DataOwner, sizeof(DataOwner));
            return std::string_view(DataOwner, Length);
        }

        uint64_t Offset;
        uint64_t Size;

        [[nodiscard]] constexpr auto getNoteRange() const noexcept {
            return ADT::Range::FromSize(Offset, Size);
        }
    };
}
