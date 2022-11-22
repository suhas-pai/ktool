//
//  Operations/PrintLoadCommands.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include "ADT/Misc.h"
#include "ADT/PrintUtils.h"

#include "MachO/LoadCommandsMap.h"
#include "Operations/PrintLoadCommands.h"

namespace Operations {
    PrintLoadCommands::PrintLoadCommands(FILE *OutFile,
                                         const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintLoadCommands::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintLoadCommands::supportsObjectKind");
            case Objects::Kind::MachO:
            case Objects::Kind::FatMachO:
                return true;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintLoadCommands::supportsObjectKind");
    }

    static auto
    PrintLoadCommand(FILE *const OutFile,
                     const MachO::LoadCommand &LC,
                     const bool IsBigEndian,
                     const char *Prefix = "") noexcept
    {
        const auto Kind = LC.kind(IsBigEndian);
        fprintf(OutFile,
                "%sCmd:     %s\n"
                "%sCmdSize: %" PRIu32 "\n\n",
                Prefix,
                MachO::LoadCommandKindGetString(Kind).data(),
                Prefix, LC.cmdsize(IsBigEndian));

        constexpr auto Malformed = std::string_view("<malformed>");
        switch (Kind) {
            case MachO::LoadCommandKind::Segment: {
                const auto &Segment =
                    MachO::cast<MachO::SegmentCommand>(LC, IsBigEndian);

                const auto VmAddr = Segment.vmAddr(IsBigEndian);
                const auto VmSize = Segment.vmSize(IsBigEndian);
                const auto FileOffset = Segment.fileOffset(IsBigEndian);
                const auto FileSize = Segment.fileSize(IsBigEndian);

                fprintf(OutFile,
                        "%sSegname:       \"" STRING_VIEW_FMT "\"\n"
                        "%sVm Addr:       " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sVm Size:       %" PRId32 "\n"
                        "%sFile Offset:   " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sFile Size:     %" PRId32 "\n"
                        "%sMax Prot:      " MACH_VMPROT_FMT "\n"
                        "%sInit Prot:     " MACH_VMPROT_FMT "\n"
                        "%sSection Count: %" PRIu32 "\n"
                        "%sFlags:         0x%" PRIx32 "\n",
                        Prefix, STRING_VIEW_FMT_ARGS(Segment.segmentName()),
                        Prefix, VmAddr,
                        ADDR_RANGE_FMT_ARGS(VmAddr, VmAddr + VmSize),
                        Prefix, VmSize,
                        Prefix, FileOffset,
                        ADDR_RANGE_FMT_ARGS(FileOffset, FileOffset + FileSize),
                        Prefix, FileSize,
                        Prefix,
                        MACH_VMPROT_FMT_ARGS(Segment.maxProt(IsBigEndian)),
                        Prefix,
                        MACH_VMPROT_FMT_ARGS(Segment.initProt(IsBigEndian)),
                        Prefix, Segment.sectionCount(IsBigEndian),
                        Prefix, Segment.flags(IsBigEndian).value());

                break;
            }
            case MachO::LoadCommandKind::Segment64: {
                const auto &Segment =
                    MachO::cast<MachO::SegmentCommand64>(LC, IsBigEndian);

                const auto VmAddr = Segment.vmAddr(IsBigEndian);
                const auto VmSize = Segment.vmSize(IsBigEndian);
                const auto FileOffset = Segment.fileOffset(IsBigEndian);
                const auto FileSize = Segment.fileSize(IsBigEndian);

                fprintf(OutFile,
                        "%sSegname:       \"" STRING_VIEW_FMT "\"\n"
                        "%sVm Addr:       " ADDRESS_64_FMT " ("
                            ADDR_RANGE_64_FMT ")\n"
                        "%sVm Size:       %" PRId64 "\n"
                        "%sFile Offset:   " ADDRESS_64_FMT " ("
                            ADDR_RANGE_64_FMT ")\n"
                        "%sFile Size:     %" PRId64 "\n"
                        "%sMax Prot:      " MACH_VMPROT_FMT "\n"
                        "%sInit Prot:     " MACH_VMPROT_FMT "\n"
                        "%sSection Count: %" PRIu32 "\n"
                        "%sFlags:         0x%" PRIx32 "\n",
                        Prefix, STRING_VIEW_FMT_ARGS(Segment.segmentName()),
                        Prefix, VmAddr,
                        ADDR_RANGE_FMT_ARGS(VmAddr, VmAddr + VmSize),
                        Prefix, VmSize,
                        Prefix, FileOffset,
                        ADDR_RANGE_FMT_ARGS(FileOffset, FileOffset + FileSize),
                        Prefix, FileSize,
                        Prefix,
                        MACH_VMPROT_FMT_ARGS(Segment.maxProt(IsBigEndian)),
                        Prefix,
                        MACH_VMPROT_FMT_ARGS(Segment.initProt(IsBigEndian)),
                        Prefix, Segment.sectionCount(IsBigEndian),
                        Prefix, Segment.flags(IsBigEndian).value());

                break;
            }
            case MachO::LoadCommandKind::Thread:
            case MachO::LoadCommandKind::UnixThread:
                break;
            case MachO::LoadCommandKind::LoadFixedVMSharedLib:
            case MachO::LoadCommandKind::IdFixedVMSharedLib: {
                const auto &FvmLib =
                    MachO::cast<MachO::FvmLibraryCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmLib.name(IsBigEndian);
                const auto MinorVersion =
                    FvmLib.Library.minorVersion(IsBigEndian);
                const auto HeaderAddress =
                    FvmLib.Library.headerAddress(IsBigEndian);

                fprintf(OutFile,
                        "%sName:           \"" STRING_VIEW_FMT "\"\n"
                        "%sMinor Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sHeader Address: " ADDRESS_32_FMT "\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            NameOpt.has_value() ?
                                NameOpt.value() : Malformed),
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(MinorVersion),
                        Prefix, HeaderAddress);

                break;
            }
            case MachO::LoadCommandKind::Identity:
                break;
            case MachO::LoadCommandKind::LoadDylib:
            case MachO::LoadCommandKind::IdDylib:
            case MachO::LoadCommandKind::ReexportDylib:
            case MachO::LoadCommandKind::LazyLoadDylib:
            case MachO::LoadCommandKind::LoadUpwardDylib:
            case MachO::LoadCommandKind::LoadWeakDylib: {
                const auto &DylibCmd =
                    MachO::cast<MachO::DylibCommand>(LC, IsBigEndian);

                const auto NameOpt = DylibCmd.name(IsBigEndian);
                const auto CurrentVersion =
                    DylibCmd.currentVersion(IsBigEndian);

                const auto CompatVersion = DylibCmd.compatVersion(IsBigEndian);
                const auto Timestamp = DylibCmd.timestamp(IsBigEndian);
                const auto TimestampString =
                    ADT::GetHumanReadableTimestamp(Timestamp);

                fprintf(OutFile,
                        "%sName:            \"" STRING_VIEW_FMT "\"\n"
                        "%sCurrent Version: " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sCompat Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sTimestamp:       %s (Value: %" PRIu32 ")\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            NameOpt.has_value() ? NameOpt.value() : Malformed),
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(CurrentVersion),
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(CompatVersion),
                        Prefix, TimestampString.c_str(), Timestamp);

                break;
            }
            case MachO::LoadCommandKind::SubFramework: {
                const auto &SubFramework =
                    MachO::cast<MachO::SubFrameworkCommand>(LC, IsBigEndian);

                const auto UmbrellaOpt = SubFramework.umbrella(IsBigEndian);
                fprintf(OutFile,
                        "%sUmbrella: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            UmbrellaOpt.has_value() ?
                                UmbrellaOpt.value() : Malformed));

                break;
            }
            case MachO::LoadCommandKind::SubClient: {
                const auto &SubClient =
                    MachO::cast<MachO::SubClientCommand>(LC, IsBigEndian);

                const auto ClientOpt = SubClient.client(IsBigEndian);
                fprintf(OutFile,
                        "%sClient: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            ClientOpt.has_value() ?
                                ClientOpt.value() : Malformed));

                break;
            }
            case MachO::LoadCommandKind::SubUmbrella: {
                const auto &SubUmbrella =
                    MachO::cast<MachO::SubUmbrellaCommand>(LC, IsBigEndian);

                const auto SubUmbrellaOpt =
                    SubUmbrella.subUmbrella(IsBigEndian);

                fprintf(OutFile,
                        "%sClient: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            SubUmbrellaOpt.has_value() ?
                                SubUmbrellaOpt.value() : Malformed));

                break;
            }
            case MachO::LoadCommandKind::SubLibrary: {
                const auto &SubLibrary =
                    MachO::cast<MachO::SubLibraryCommand>(LC, IsBigEndian);

                const auto SubLibraryOpt = SubLibrary.subLibrary(IsBigEndian);
                fprintf(OutFile,
                        "%sClient: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            SubLibraryOpt.has_value() ?
                                SubLibraryOpt.value() : Malformed));

                break;
            }
            case MachO::LoadCommandKind::PreBoundDylib: {
                const auto &PreboundDylibCmd =
                    MachO::cast<MachO::PreboundDylibCommand>(LC, IsBigEndian);

                const auto NameOpt = PreboundDylibCmd.name(IsBigEndian);
                fprintf(OutFile,
                        "%sName: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            NameOpt.has_value() ?
                                NameOpt.value() : Malformed));

                break;
            }
            case MachO::LoadCommandKind::IdDylinker:
            case MachO::LoadCommandKind::LoadDylinker: {
                const auto &DylinkerCmd =
                    MachO::cast<MachO::DylinkerCommand>(LC, IsBigEndian);

                const auto NameOpt = DylinkerCmd.name(IsBigEndian);
                fprintf(OutFile,
                        "%sName: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            NameOpt.has_value() ?
                                NameOpt.value() : Malformed));

                break;
            }
            case MachO::LoadCommandKind::Routines: {
                const auto &RoutinesCmd =
                    MachO::cast<MachO::RoutinesCommand>(LC, IsBigEndian);

                const auto InitAddress = RoutinesCmd.initAddress(IsBigEndian);
                const auto InitModule = RoutinesCmd.initModule(IsBigEndian);

                const auto Reserved1 = RoutinesCmd.reserved1(IsBigEndian);
                const auto Reserved2 = RoutinesCmd.reserved2(IsBigEndian);
                const auto Reserved3 = RoutinesCmd.reserved3(IsBigEndian);
                const auto Reserved4 = RoutinesCmd.reserved4(IsBigEndian);
                const auto Reserved5 = RoutinesCmd.reserved5(IsBigEndian);
                const auto Reserved6 = RoutinesCmd.reserved6(IsBigEndian);

                fprintf(OutFile,
                        "%sInit Address: " ADDRESS_32_FMT "\n"
                        "%sInit Module:  %" PRIu32 "\n"
                        "%sReserved 1:   %" PRIu32 "\n"
                        "%sReserved 2:   %" PRIu32 "\n"
                        "%sReserved 3:   %" PRIu32 "\n"
                        "%sReserved 4:   %" PRIu32 "\n"
                        "%sReserved 5:   %" PRIu32 "\n"
                        "%sReserved 6:   %" PRIu32 "\n",
                        Prefix, InitAddress,
                        Prefix, InitModule,
                        Prefix, Reserved1,
                        Prefix, Reserved2,
                        Prefix, Reserved3,
                        Prefix, Reserved4,
                        Prefix, Reserved5,
                        Prefix, Reserved6);
                break;
            }
            case MachO::LoadCommandKind::Routines64: {
                const auto &RoutinesCmd =
                    MachO::cast<MachO::RoutinesCommand64>(LC, IsBigEndian);

                const auto InitAddress = RoutinesCmd.initAddress(IsBigEndian);
                const auto InitModule = RoutinesCmd.initModule(IsBigEndian);

                const auto Reserved1 = RoutinesCmd.reserved1(IsBigEndian);
                const auto Reserved2 = RoutinesCmd.reserved2(IsBigEndian);
                const auto Reserved3 = RoutinesCmd.reserved3(IsBigEndian);
                const auto Reserved4 = RoutinesCmd.reserved4(IsBigEndian);
                const auto Reserved5 = RoutinesCmd.reserved5(IsBigEndian);
                const auto Reserved6 = RoutinesCmd.reserved6(IsBigEndian);

                fprintf(OutFile,
                        "%sInit Address: " ADDRESS_64_FMT "\n"
                        "%sInit Module:  %" PRIu64 "\n"
                        "%sReserved 1:   %" PRIu64 "\n"
                        "%sReserved 2:   %" PRIu64 "\n"
                        "%sReserved 3:   %" PRIu64 "\n"
                        "%sReserved 4:   %" PRIu64 "\n"
                        "%sReserved 5:   %" PRIu64 "\n"
                        "%sReserved 6:   %" PRIu64 "\n",
                        Prefix, InitAddress,
                        Prefix, InitModule,
                        Prefix, Reserved1,
                        Prefix, Reserved2,
                        Prefix, Reserved3,
                        Prefix, Reserved4,
                        Prefix, Reserved5,
                        Prefix, Reserved6);
                break;
            }
            case MachO::LoadCommandKind::SymbolTable: {
                const auto &SymTabCmd =
                    MachO::cast<MachO::SymTabCommand>(LC, IsBigEndian);

                const auto SymOff = SymTabCmd.symOffset(IsBigEndian);
                const auto SymCount = SymTabCmd.symCount(IsBigEndian);

                const auto StrOff = SymTabCmd.strOffset(IsBigEndian);
                const auto StrSize = SymTabCmd.strSize(IsBigEndian);

                fprintf(OutFile,
                        "%sSym Offset:          " ADDRESS_32_FMT "\n"
                        "%sSymbol Count         %" PRIu32 "\n"
                        "%sString Table Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sString Table Size:   %" PRIu32 "\n",
                        Prefix, SymOff,
                        Prefix, SymCount,
                        Prefix, StrOff,
                        ADDR_RANGE_FMT_ARGS(StrOff, StrSize),
                        Prefix, StrSize);

                break;
            }
            case MachO::LoadCommandKind::DynamicSymbolTable: {
                const auto &DySymTabCmd =
                MachO::cast<MachO::DynamicSymTabCommand>(LC, IsBigEndian);

                const auto LocalSymbolsIndex =
                    DySymTabCmd.localSymbolsIndex(IsBigEndian);
                const auto LocalSymbolsCount =
                    DySymTabCmd.localSymbolsCount(IsBigEndian);
                const auto ExternDefSymbolsIndex =
                    DySymTabCmd.externDefSymbolsIndex(IsBigEndian);
                const auto ExternDefSymbolsCount =
                    DySymTabCmd.externDefSymbolsCount(IsBigEndian);
                const auto UndefSymbolsIndex =
                    DySymTabCmd.undefDefSymbolsIndex(IsBigEndian);
                const auto UndefSymbolsCount =
                    DySymTabCmd.undefDefSymbolsCount(IsBigEndian);
                const auto TableOfContentsOffset =
                    DySymTabCmd.tableOfContentsOffset(IsBigEndian);
                const auto TableOfContentsCount =
                    DySymTabCmd.tableOfContentsCount(IsBigEndian);
                const auto ModulesTabOffset =
                    DySymTabCmd.modulesTabOffset(IsBigEndian);
                const auto ModulesTabCount =
                    DySymTabCmd.modulesTabCount(IsBigEndian);
                const auto ExternRefSymbolsOffset =
                    DySymTabCmd.externRefSymbolsOffset(IsBigEndian);
                const auto ExternRefSymbolsCount =
                    DySymTabCmd.externRefSymbolsCount(IsBigEndian);
                const auto IndirectSymbolsOffset =
                    DySymTabCmd.indirectSymbolsOffset(IsBigEndian);
                const auto IndirectSymbolsCount =
                    DySymTabCmd.indirectSymbolsOffset(IsBigEndian);
                const auto ExternRelOffset =
                    DySymTabCmd.externRelOffset(IsBigEndian);
                const auto ExternRelCount =
                    DySymTabCmd.externRelCount(IsBigEndian);
                const auto LocalRelOffset =
                    DySymTabCmd.localRelOffset(IsBigEndian);
                const auto LocalRelCount =
                    DySymTabCmd.localRelCount(IsBigEndian);

                fprintf(OutFile,
                        "%sLocal Symbols Index:       %" PRIu32 "\n"
                        "%sLocal Symbols Count:       %" PRIu32 "\n"
                        "%sExtern Def Symbols Index:  %" PRIu32 "\n"
                        "%sExtern Def Symbols Count:  %" PRIu32 "\n"
                        "%sUnDef Symbols Index:       %" PRIu32 "\n"
                        "%sUnDef Symbols Count:       %" PRIu32 "\n"
                        "%sTable Of Contents Offset:  " ADDRESS_32_FMT "\n"
                        "%sTable Of Contents Count:   %" PRIu32 "\n"
                        "%sModules Tab Offset:        " ADDRESS_32_FMT "\n"
                        "%sModules Tab Count:         %" PRIu32 "\n"
                        "%sExtern Ref Symbols Offset: " ADDRESS_32_FMT "\n"
                        "%sExtern Ref Symbols Count:  %" PRIu32 "\n"
                        "%sIndirect Symbols Offset:   " ADDRESS_32_FMT "\n"
                        "%sIndirect Symbols Count:    %" PRIu32 "\n"
                        "%sExtern Relocations Offset: " ADDRESS_32_FMT "\n"
                        "%sExtern Relocations Count:  %" PRIu32 "\n"
                        "%sLocal Relocations Offset:  " ADDRESS_32_FMT "\n"
                        "%sLocal Relocations Count:   %" PRIu32 "\n",
                        Prefix, LocalSymbolsIndex,
                        Prefix, LocalSymbolsCount,
                        Prefix, ExternDefSymbolsIndex,
                        Prefix, ExternDefSymbolsCount,
                        Prefix, UndefSymbolsIndex,
                        Prefix, UndefSymbolsCount,
                        Prefix, TableOfContentsOffset,
                        Prefix, TableOfContentsCount,
                        Prefix, ModulesTabOffset,
                        Prefix, ModulesTabCount,
                        Prefix, ExternRefSymbolsOffset,
                        Prefix, ExternRefSymbolsCount,
                        Prefix, IndirectSymbolsOffset,
                        Prefix, IndirectSymbolsCount,
                        Prefix, ExternRelOffset,
                        Prefix, ExternRelCount,
                        Prefix, LocalRelOffset,
                        Prefix, LocalRelCount);
                break;
            }
            case MachO::LoadCommandKind::TwoLevelHints: {
                const auto &TwoLevelHintsCmd =
                    MachO::cast<MachO::TwoLevelHintsCommand>(LC, IsBigEndian);

                const auto Offset = TwoLevelHintsCmd.offset(IsBigEndian);
                const auto HintsCount =
                    TwoLevelHintsCmd.hintsCount(IsBigEndian);

                fprintf(OutFile,
                        "%sOffset:      " ADDRESS_32_FMT
                        "%sHints Count: %" PRIu32 "\n",
                        Prefix, Offset,
                        Prefix, HintsCount);
                break;
            }
            case MachO::LoadCommandKind::PreBindChecksum: {
                const auto PrebindChecksumCmd =
                    MachO::cast<MachO::PrebindChecksumCommand>(LC, IsBigEndian);

                const auto Checksum = PrebindChecksumCmd.checksum(IsBigEndian);
                fprintf(OutFile,
                        "%sChecksum: %" PRIu32 "\n",
                        Prefix, Checksum);

                break;
            }
            case MachO::LoadCommandKind::Uuid: {
                const auto UuidCmd =
                    MachO::cast<MachO::UuidCommand>(LC, IsBigEndian);

                fprintf(OutFile,
                        "%sUuid: %.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-"
                        "%.2X%.2x%.2x%.2x%.2X%.2X\n",
                        Prefix,
                        UuidCmd.Uuid[0],
                        UuidCmd.Uuid[1],
                        UuidCmd.Uuid[2],
                        UuidCmd.Uuid[3],
                        UuidCmd.Uuid[4],
                        UuidCmd.Uuid[5],
                        UuidCmd.Uuid[6],
                        UuidCmd.Uuid[7],
                        UuidCmd.Uuid[8],
                        UuidCmd.Uuid[9],
                        UuidCmd.Uuid[10],
                        UuidCmd.Uuid[11],
                        UuidCmd.Uuid[12],
                        UuidCmd.Uuid[13],
                        UuidCmd.Uuid[14],
                        UuidCmd.Uuid[15]);
                break;
            }
            case MachO::LoadCommandKind::Rpath: {
                const auto &RpathCmd =
                    MachO::cast<MachO::RpathCommand>(LC, IsBigEndian);

                const auto PathOpt = RpathCmd.path(IsBigEndian);
                fprintf(OutFile,
                        "%sPath: \"" STRING_VIEW_FMT "\"\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            PathOpt.has_value() ?
                                PathOpt.value() : Malformed));
                break;
            }
            case MachO::LoadCommandKind::CodeSignature:
            case MachO::LoadCommandKind::SegmentSplitInfo:
            case MachO::LoadCommandKind::FunctionStarts:
            case MachO::LoadCommandKind::DyldEnvironment:
            case MachO::LoadCommandKind::DataInCode:
            case MachO::LoadCommandKind::DylibCodeSignDRS:
            case MachO::LoadCommandKind::LinkerOptimizationHint:
            case MachO::LoadCommandKind::DyldExportsTrie:
            case MachO::LoadCommandKind::DyldChainedFixups: {
                const auto &LinkeditDataCmd =
                    MachO::cast<MachO::LinkeditDataCommand>(LC, IsBigEndian);

                const auto DataOff = LinkeditDataCmd.dataOff(IsBigEndian);
                const auto DataSize = LinkeditDataCmd.dataSize(IsBigEndian);

                fprintf(OutFile,
                        "%sData Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sData Size:   %" PRIu32 "\n",
                        Prefix, DataOff,
                        ADDR_RANGE_FMT_ARGS(DataOff, DataSize),
                        Prefix, DataSize);
                break;
            }
            case MachO::LoadCommandKind::FileSetEntry: {
                const auto &FilesetEntryCmd =
                    MachO::cast<MachO::FileSetEntryCommand>(LC, IsBigEndian);

                const auto VmAddress = FilesetEntryCmd.vmAddress(IsBigEndian);
                const auto FileOffset = FilesetEntryCmd.fileOffset(IsBigEndian);
                const auto EntryId = FilesetEntryCmd.entryId(IsBigEndian);
                const auto Reserved = FilesetEntryCmd.reserved(IsBigEndian);

                fprintf(OutFile,
                        "%sVm Address:  " ADDRESS_64_FMT "\n"
                        "%sFile Offset: %" PRIu64 "\n"
                        "%sEntry Id:    %" PRIu32 "\n"
                        "%sReserved:    %" PRIu32 "\n",
                        Prefix, VmAddress,
                        Prefix, FileOffset,
                        Prefix, EntryId,
                        Prefix, Reserved);
                break;
            }
            case MachO::LoadCommandKind::EncryptionInfo: {
                const auto &EncryptionInfoCmd =
                    MachO::cast<MachO::EncryptionInfoCommand>(LC, IsBigEndian);

                const auto CryptSize = EncryptionInfoCmd.cryptSize(IsBigEndian);
                const auto CryptId = EncryptionInfoCmd.cryptId(IsBigEndian);
                const auto CryptOffset =
                    EncryptionInfoCmd.cryptOffset(IsBigEndian);

                fprintf(OutFile,
                        "%sCrypt Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sCrypt Size:   %" PRIu32 "\n"
                        "%sCrypt Id:     %" PRIu32 "\n",
                        Prefix, CryptOffset,
                        ADDR_RANGE_FMT_ARGS(CryptOffset, CryptSize),
                        Prefix, CryptSize,
                        Prefix, CryptId);
                break;
            }
            case MachO::LoadCommandKind::EncryptionInfo64: {
                const auto &EncryptionInfoCmd =
                    MachO::cast<MachO::EncryptionInfo64Command>(LC, IsBigEndian);

                const auto CryptSize = EncryptionInfoCmd.cryptSize(IsBigEndian);
                const auto CryptId = EncryptionInfoCmd.cryptId(IsBigEndian);
                const auto Pad = EncryptionInfoCmd.pad(IsBigEndian);
                const auto CryptOffset =
                    EncryptionInfoCmd.cryptOffset(IsBigEndian);

                fprintf(OutFile,
                        "%sCrypt Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sCrypt Size:   %" PRIu32 "\n"
                        "%sCrypt Id:     %" PRIu32 "\n"
                        "%sPad:          %" PRIu32 "\n",
                        Prefix, CryptOffset,
                        ADDR_RANGE_FMT_ARGS(CryptOffset, CryptSize),
                        Prefix, CryptSize,
                        Prefix, CryptId,
                        Prefix, Pad);
                break;
            }
            case MachO::LoadCommandKind::VersionMinMacOS:
            case MachO::LoadCommandKind::VersionMinIOS:
            case MachO::LoadCommandKind::VersionMinTVOS:
            case MachO::LoadCommandKind::VersionMinWatchOS: {
                const auto &VersionMinCmd =
                    MachO::cast<MachO::VersionMinCommand>(LC, IsBigEndian);

                const auto Version = VersionMinCmd.version(IsBigEndian);
                const auto Sdk = VersionMinCmd.sdk(IsBigEndian);

                fprintf(OutFile,
                        "%sVersion: " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sSDK:     " DYLD3_PACKED_VERSION_FMT "\n",
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(Version),
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(Sdk));
                break;
            }
            case MachO::LoadCommandKind::BuildVersion: {
                const auto &BuildVersionCmd =
                    MachO::cast<MachO::BuildVersionCommand>(LC, IsBigEndian);

                const auto Platform = BuildVersionCmd.platform(IsBigEndian);
                if (Dyld3::PlatformIsValid(Platform)) {
                    const auto PlatformString =
                        Dyld3::PlatformGetString(Platform);

                    fprintf(OutFile,
                            "%sPlatform:    %s\n",
                            Prefix,
                            PlatformString.data());
                } else {
                    fprintf(OutFile,
                            "%sPlatform:    <Unknown> (Value: %" PRIu32 "\n",
                            Prefix,
                            static_cast<uint32_t>(Platform));
                }

                const auto MinOS = BuildVersionCmd.minOS(IsBigEndian);
                const auto Sdk = BuildVersionCmd.sdk(IsBigEndian);
                const auto ToolsCount = BuildVersionCmd.toolsCount(IsBigEndian);

                fprintf(OutFile,
                        "%sMinimum OS:  " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sSDK:         " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sTools Count: %" PRIu32 "\n",
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(MinOS),
                        Prefix, DYLD3_PACKED_VERSION_FMT_ARGS(Sdk),
                        Prefix, ToolsCount);
                break;
            }
            case MachO::LoadCommandKind::DyldInfo:
            case MachO::LoadCommandKind::DyldInfoOnly: {
                const auto &DyldInfoCmd =
                    MachO::cast<MachO::DyldInfoCommand>(LC, IsBigEndian);

                const auto RebaseOffset = DyldInfoCmd.rebaseOffset(IsBigEndian);
                const auto RebaseSize = DyldInfoCmd.rebaseSize(IsBigEndian);
                const auto BindOffset = DyldInfoCmd.bindOffset(IsBigEndian);
                const auto BindSize = DyldInfoCmd.bindSize(IsBigEndian);
                const auto WeakBindOffset =
                    DyldInfoCmd.weakBindOffset(IsBigEndian);
                const auto WeakBindSize = DyldInfoCmd.weakBindSize(IsBigEndian);
                const auto LazyBindOffset =
                    DyldInfoCmd.lazyBindOffset(IsBigEndian);
                const auto LazyBindSize = DyldInfoCmd.lazyBindSize(IsBigEndian);
                const auto ExportTrieOffset =
                    DyldInfoCmd.exportTrieOffset(IsBigEndian);
                const auto ExportTrieSize =
                    DyldInfoCmd.exportTrieSize(IsBigEndian);

                fprintf(OutFile,
                        "%sRebase Offset:      " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sRebase Size:        %" PRIu32 "\n"
                        "%sBind Offset:        " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sBind Size:          %" PRIu32 "\n"
                        "%sWeak Bind Offset:   " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sWeak Bind Size:     %" PRIu32 "\n"
                        "%sLazy Bind Offset:   " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sLazy Bind Size:     %" PRIu32 "\n"
                        "%sExport Trie Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sExport Trie Size:   %" PRIu32 "\n",
                        Prefix, RebaseOffset,
                        ADDR_RANGE_FMT_ARGS(RebaseOffset, RebaseSize),
                        Prefix, RebaseSize,
                        Prefix, BindOffset,
                        ADDR_RANGE_FMT_ARGS(BindOffset, BindSize),
                        Prefix, BindSize,
                        Prefix, WeakBindOffset,
                        ADDR_RANGE_FMT_ARGS(WeakBindOffset, WeakBindSize),
                        Prefix, WeakBindSize,
                        Prefix, LazyBindOffset,
                        ADDR_RANGE_FMT_ARGS(LazyBindOffset, LazyBindSize),
                        Prefix, LazyBindSize,
                        Prefix, ExportTrieOffset,
                        ADDR_RANGE_FMT_ARGS(ExportTrieOffset, ExportTrieSize),
                        Prefix, ExportTrieSize);
                break;
            }
            case MachO::LoadCommandKind::LinkerOption: {
                const auto &LinkerOptionCmd =
                    MachO::cast<MachO::LinkerOptionCommand>(LC, IsBigEndian);

                const auto Count = LinkerOptionCmd.count(IsBigEndian);
                fprintf(OutFile, "%sCount: %" PRIu32 "\n", Prefix, Count);

                break;
            }
            case MachO::LoadCommandKind::SymbolSegment: {
                const auto &SymbolSegmentCmd =
                    MachO::cast<MachO::SymbolSegmentCommand>(LC, IsBigEndian);

                const auto Offset = SymbolSegmentCmd.offset(IsBigEndian);
                const auto Size = SymbolSegmentCmd.size(IsBigEndian);

                fprintf(OutFile,
                        "%sOffset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sSize:   %" PRIu32 "\n",
                        Prefix, Offset,
                        ADDR_RANGE_FMT_ARGS(Offset, Size),
                        Prefix, Size);
                break;
            }
            case MachO::LoadCommandKind::FixedVMFile: {
                const auto &FvmFileCmd =
                    MachO::cast<MachO::FvmFileCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmFileCmd.name(IsBigEndian);
                const auto HeaderAddress =
                    FvmFileCmd.headerAddress(IsBigEndian);

                fprintf(OutFile,
                        "%sName:           \"" STRING_VIEW_FMT "\"\n"
                        "%sHeader Address: " ADDRESS_32_FMT "\n",
                        Prefix,
                        STRING_VIEW_FMT_ARGS(
                            NameOpt.has_value() ?
                                NameOpt.value() : Malformed),
                        Prefix, HeaderAddress);
                break;
            }
            case MachO::LoadCommandKind::Main: {
                const auto &EntryPointCmd =
                    MachO::cast<MachO::EntryPointCommand>(LC, IsBigEndian);

                const auto EntryOffset = EntryPointCmd.entryOffset(IsBigEndian);
                const auto StackSize = EntryPointCmd.stackSize(IsBigEndian);

                fprintf(OutFile,
                        "%sEntry Offset: " ADDRESS_64_FMT "\n"
                        "%sStack Size:   %" PRIu64 "\n",
                        Prefix, EntryOffset,
                        Prefix, StackSize);
                break;
            }
            case MachO::LoadCommandKind::SourceVersion: {
                const auto &SourceVersionCmd =
                    MachO::cast<MachO::SourceVersionCommand>(LC, IsBigEndian);

                const auto Version = SourceVersionCmd.version(IsBigEndian);
                fprintf(OutFile,
                        "%sVersion: " DYLD3_PACKED_VERSION_64_FMT "\n",
                        Prefix, DYLD3_PACKED_VERSION_64_FMT_ARGS(Version));
                
                break;
            }
            case MachO::LoadCommandKind::PrePage:
                break;
            case MachO::LoadCommandKind::Note: {
                const auto &NoteCmd =
                    MachO::cast<MachO::NoteCommand>(LC, IsBigEndian);

                const auto DataOwner = NoteCmd.dataOwner();
                const auto Offset = NoteCmd.offset(IsBigEndian);
                const auto Size = NoteCmd.size(IsBigEndian);

                fprintf(OutFile,
                        "%sData Owner: \"" STRING_VIEW_FMT "\"\n"
                        "%sOffset: " ADDRESS_64_FMT " ("
                            ADDR_RANGE_64_FMT ")\n"
                        "%sSize:   %" PRIu64 "\n",
                        Prefix, STRING_VIEW_FMT_ARGS(DataOwner),
                        Prefix, Offset,
                        ADDR_RANGE_FMT_ARGS(Offset, Size),
                        Prefix, Size);

                break;
            }
        }
    }

    auto
    PrintLoadCommands::run(const Objects::MachO &MachO) const noexcept ->
        RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);
        const auto LoadCommandsMemoryMap =
            ADT::MemoryMap(MachO.map(), MachO.header().loadCommandsRange());

        const auto IsBigEndian = MachO.isBigEndian();
        const auto LoadCommandsMap =
            ::MachO::LoadCommandsMap(LoadCommandsMemoryMap, IsBigEndian);

        auto Counter = static_cast<uint32_t>(1);
        for (const auto &LoadCommand : LoadCommandsMap) {
            fprintf(OutFile, "Load-Command %" PRIu32 ":\n", Counter);
            PrintLoadCommand(OutFile, LoadCommand, IsBigEndian, "  ");
            fputc('\n', OutFile);

            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto PrintLoadCommands::run(const Objects::Base &Base) const noexcept ->
        RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false && "Got unrecognized Object-Kind in PrintHeader::run");
    }
}

