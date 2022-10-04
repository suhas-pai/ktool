//
//  MachO/Header.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once

#include <string>
#include "ADT/Range.h"

#include "Mach/Machine.h"
#include "Mach/VmProt.h"

namespace MachO {
    enum class Magic : uint32_t {
        Default = 0xFEEDFACE,
        Swapped = 0xCEFADEFE,

        Default64 = 0xFEEDFACF,
        Swapped64 = 0xCEFADEFC
    };

    enum class FileKind {
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

    enum class Flags {
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

        Prebindabl = 1 << 11,
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

    struct Header {
        Magic Magic;
        Mach::CpuKind CpuKind;
        int32_t CpuSubKind;
        uint32_t Ncmds;
        uint32_t SizeOfCmds;
        uint32_t Flags;
    };

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

        uint32_t Nsects;

        struct FlagsStruct {
        protected:
            uint32_t Flags;
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

            constexpr auto setIsHighVm() noexcept {
                Flags |= static_cast<uint32_t>(Kind::HighVm);
                return *this;
            }

            constexpr auto setIsFixedVmLibrary() noexcept {
                Flags |= static_cast<uint32_t>(Kind::FixedVmLibrary);
                return *this;
            }

            constexpr auto setHasNoRelocations() noexcept {
                Flags |= static_cast<uint32_t>(Kind::NoRelocations);
                return *this;
            }

            constexpr auto setIsProtectedVersion1() noexcept {
                Flags |= static_cast<uint32_t>(Kind::ProtectedVersion1);
                return *this;
            }

            constexpr auto setIsReadOnly() noexcept {
                Flags |= static_cast<uint32_t>(Kind::ReadOnly);
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
            uint32_t NRelocs;

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

            struct FlagsStruct {
            protected:
                uint32_t Flags;

                static constexpr uint32_t KindMask = 0xFF;
                static constexpr auto AttributesMask = ~KindMask;
            public:
                [[nodiscard]] constexpr auto getKind() const noexcept {
                    return Kind(Flags & KindMask);
                }

                [[nodiscard]] constexpr auto getAttributes() const noexcept {
                    return (Flags & AttributesMask);
                }
            };
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

        uint32_t Nsects;

        using FlagsStruct = SegmentCommand::FlagsStruct;
        FlagsStruct Flags;
    };
}
