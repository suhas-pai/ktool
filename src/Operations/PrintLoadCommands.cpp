//
//  Operations/PrintLoadCommands.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include "ADT/FlagsIterator.h"
#include "Dyld3/Platform.h"

#include "MachO/LoadCommands.h"
#include "MachO/LoadCommandsMap.h"

#include "Operations/PrintLoadCommands.h"

#include "Utils/Misc.h"
#include "Utils/Print.h"

namespace Operations {
    PrintLoadCommands::PrintLoadCommands(FILE *const OutFile,
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
                     uint32_t &DylibIndex,
                     const bool Verbose,
                     const char *Prefix = "") noexcept
    {
        const auto Kind = LC.kind(IsBigEndian);
        constexpr auto Malformed = std::string_view("<malformed>");

        switch (Kind) {
            case MachO::LoadCommandKind::Segment: {
                const auto &Segment =
                    MachO::cast<MachO::SegmentCommand>(LC, IsBigEndian);

                const auto VmAddr = Segment.vmAddr(IsBigEndian);
                const auto VmSize = Segment.vmSize(IsBigEndian);
                const auto FileOffset = Segment.fileOffset(IsBigEndian);
                const auto FileSize = Segment.fileSize(IsBigEndian);
                const auto SectionCount = Segment.sectionCount(IsBigEndian);
                const auto Flags = Segment.flags(IsBigEndian);

                fprintf(OutFile,
                        "\t\"" STRING_VIEW_FMT "\""
                        "\t" MACH_VMPROT_FMT "/" MACH_VMPROT_FMT "\n"
                        "%sFile:          " ADDR_RANGE_32_FMT "\n"
                        "%sMem:           " ADDR_RANGE_32_FMT "\n"
                        "%sFile Size:     %" PRId32 " (%s)\n"
                        "%sMem Size:      %" PRId32 " (%s)\n"
                        "%sFlags:         0x%" PRIx32 "\n"
                        "%sSection Count: %" PRIu32 "\n",
                        STRING_VIEW_FMT_ARGS(Segment.segmentName()),
                        MACH_VMPROT_FMT_ARGS(Segment.initProt(IsBigEndian)),
                        MACH_VMPROT_FMT_ARGS(Segment.maxProt(IsBigEndian)),
                        Prefix,
                        ADDR_RANGE_FMT_ARGS(FileOffset, FileOffset + FileSize),
                        Prefix,
                        ADDR_RANGE_FMT_ARGS(VmAddr, VmAddr + VmSize),
                        Prefix, FileSize, Utils::FormattedSize(FileSize).data(),
                        Prefix, VmSize, Utils::FormattedSize(VmSize).data(),
                        Prefix, Flags.value(),
                        Prefix, SectionCount);

                if (!Flags.empty()) {
                    for (const auto &Flag : ADT::FlagsIterator(Flags.value())) {
                        printf("Index: %" PRIu32, Flag);
                    }
                }

                if (SectionCount == 0) {
                    break;
                }

                const auto SectionList = Segment.sections();
                const auto SectionCountDigitCount =
                    Utils::GetIntegerDigitCount(SectionCount);

                for (auto I = uint32_t(); I != SectionCount; I++) {
                    const auto &Section = SectionList[I];

                    const auto Addr = Section.addr(IsBigEndian);
                    const auto Size = Section.size(IsBigEndian);
                    const auto FileOffset = Section.fileOffset(IsBigEndian);
                    const auto FormattedSize = Utils::FormattedSize(Size);

                    fprintf(OutFile,
                            "\t%s%*" PRIu32 ". "
                            "File: " ADDR_RANGE_32_FMT
                            "\tMem: " ADDR_RANGE_32_FMT,
                            Prefix, SectionCountDigitCount, I + 1,
                            ADDR_RANGE_FMT_ARGS(FileOffset, FileOffset + Size),
                            ADDR_RANGE_FMT_ARGS(Addr, Addr + Size));

                    Utils::PrintSegmentSectionPair(OutFile,
                                                   Section.segmentName(),
                                                   Section.sectionName(),
                                                   /*PadSegment=*/true,
                                                   /*PadSection=*/true,
                                                   " ");

                    using SectionSpace = MachO::SegmentCommand::Section;

                    const auto SectionKind = Section.kind(IsBigEndian);
                    if (SectionKind != SectionSpace::Kind::Regular) {
                        auto SectionKindDesc =
                            SectionSpace::KindIsValid(SectionKind) ?
                                SectionSpace::KindGetDesc(SectionKind) :
                                std::string_view("<unknown>");

                        fprintf(OutFile, " (%s)\n", SectionKindDesc.data());
                    } else {
                        fputc('\n', OutFile);
                    }

                    if (Verbose) {
                        fprintf(OutFile,
                                "%s\t\tSize:              %" PRIu32 " (%s)\n"
                                "%s\t\tAlign:             %" PRIu32 "\n"
                                "%s\t\tReloc File Offset: " ADDRESS_32_FMT "\n"
                                "%s\t\tReloc Count:       %" PRIu32 "\n"
                                "%s\t\tFlags:             0x%" PRIx32 "\n"
                                "%s\t\tReserved 1:        %" PRIu32 "\n"
                                "%s\t\tReserved 2:        %" PRIu32 "\n",
                                Prefix, Size, FormattedSize.data(),
                                Prefix, Section.align(IsBigEndian),
                                Prefix, Section.relocFileOffset(IsBigEndian),
                                Prefix, Section.relocsCount(IsBigEndian),
                                Prefix, Section.flags(IsBigEndian).value(),
                                Prefix, Section.reserved1(IsBigEndian),
                                Prefix, Section.reserved2(IsBigEndian));
                    }
                }

                break;
            }
            case MachO::LoadCommandKind::Segment64: {
                const auto &Segment =
                    MachO::cast<MachO::SegmentCommand64>(LC, IsBigEndian);

                const auto VmAddr = Segment.vmAddr(IsBigEndian);
                const auto VmSize = Segment.vmSize(IsBigEndian);
                const auto FileOffset = Segment.fileOffset(IsBigEndian);
                const auto FileSize = Segment.fileSize(IsBigEndian);
                const auto SectionCount = Segment.sectionCount(IsBigEndian);
                const auto Flags = Segment.flags(IsBigEndian);

                fprintf(OutFile,
                        "\t\"" STRING_VIEW_FMT "\""
                        "\t" MACH_VMPROT_FMT "/" MACH_VMPROT_FMT "\n"
                        "%sFile:          " ADDR_RANGE_64_FMT "\n"
                        "%sMem:           " ADDR_RANGE_64_FMT "\n"
                        "%sFile Size:     %" PRId64 " (%s)\n"
                        "%sMem Size:      %" PRId64 " (%s)\n"
                        "%sFlags:         0x%" PRIx32 "\n"
                        "%sSection Count: %" PRIu32 "\n",
                        STRING_VIEW_FMT_ARGS(Segment.segmentName()),
                        MACH_VMPROT_FMT_ARGS(Segment.initProt(IsBigEndian)),
                        MACH_VMPROT_FMT_ARGS(Segment.maxProt(IsBigEndian)),
                        Prefix,
                        ADDR_RANGE_FMT_ARGS(FileOffset, FileOffset + FileSize),
                        Prefix,
                        ADDR_RANGE_FMT_ARGS(VmAddr, VmAddr + VmSize),
                        Prefix, FileSize, Utils::FormattedSize(FileSize).data(),
                        Prefix, VmSize, Utils::FormattedSize(VmSize).data(),
                        Prefix, Flags.value(),
                        Prefix, SectionCount);

                if (!Flags.empty()) {
                    for (const auto &Flag : ADT::FlagsIterator(Flags.value())) {
                        printf("Index: %" PRIu32, Flag);
                    }
                }

                if (SectionCount == 0) {
                    break;
                }

                const auto SectionList = Segment.sections();
                const auto SectionCountDigitCount =
                    Utils::GetIntegerDigitCount(SectionCount);

                for (auto I = uint32_t(); I != SectionCount; I++) {
                    const auto &Section = SectionList[I];

                    const auto Addr = Section.addr(IsBigEndian);
                    const auto Size = Section.size(IsBigEndian);
                    const auto FileOffset = Section.fileOffset(IsBigEndian);
                    const auto FormattedSize = Utils::FormattedSize(Size);

                    fprintf(OutFile,
                            "\t%s%*" PRIu32 ". "
                            "File: " ADDR_RANGE_32_64_FMT
                            "\tMem: " ADDR_RANGE_64_FMT,
                            Prefix, SectionCountDigitCount, I + 1,
                            ADDR_RANGE_FMT_ARGS(FileOffset, FileOffset + Size),
                            ADDR_RANGE_FMT_ARGS(Addr, Addr + Size));

                    Utils::PrintSegmentSectionPair(OutFile,
                                                   Section.segmentName(),
                                                   Section.sectionName(),
                                                   /*PadSegment=*/true,
                                                   /*PadSection=*/true,
                                                   " ");

                    using SectionSpace = MachO::SegmentCommand::Section;

                    const auto SectionKind = Section.kind(IsBigEndian);
                    if (SectionKind != SectionSpace::Kind::Regular) {
                        auto SectionKindDesc =
                            SectionSpace::KindIsValid(SectionKind) ?
                                SectionSpace::KindGetDesc(SectionKind) :
                                std::string_view("<unknown>");

                        fprintf(OutFile, " (%s)\n", SectionKindDesc.data());
                    } else {
                        fputc('\n', OutFile);
                    }

                    if (Verbose) {
                        fprintf(OutFile,
                                "%s\t\tSize:              %" PRIu64 " (%s)\n"
                                "%s\t\tAlign:             %" PRIu32 "\n"
                                "%s\t\tReloc File Offset: " ADDRESS_32_FMT "\n"
                                "%s\t\tReloc Count:       %" PRIu32 "\n"
                                "%s\t\tFlags:             0x%" PRIx32 "\n"
                                "%s\t\tReserved 1:        %" PRIu32 "\n"
                                "%s\t\tReserved 2:        %" PRIu32 "\n",
                                Prefix, Size, FormattedSize.data(),
                                Prefix, Section.align(IsBigEndian),
                                Prefix, Section.relocFileOffset(IsBigEndian),
                                Prefix, Section.relocsCount(IsBigEndian),
                                Prefix, Section.flags(IsBigEndian).value(),
                                Prefix, Section.reserved1(IsBigEndian),
                                Prefix, Section.reserved2(IsBigEndian));
                    }
                }

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
                        "\n"
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
                    Utils::GetHumanReadableTimestamp(Timestamp);

                fprintf(OutFile,
                        "\t\"" STRING_VIEW_FMT "\"",
                        STRING_VIEW_FMT_ARGS(
                            NameOpt.has_value() ? NameOpt.value() : Malformed));

                if (Kind != MachO::LoadCommandKind::IdDylib) {
                    fprintf(OutFile,
                            "\t\t(Dylib-Ordinal %" PRIu32 ")",
                            DylibIndex + 1);

                    DylibIndex++;
                }

                fprintf(OutFile,
                        "\n"
                        "%sCurrent Version: " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sCompat Version:  " DYLD3_PACKED_VERSION_FMT "\n"
                        "%sTimestamp:       %s (Value: %" PRIu32 ")\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\n"
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
                        "\n"
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
                        "\n"
                        "%sSym Offset:          " ADDRESS_32_FMT "\n"
                        "%sSymbol Count:        %" PRIu32 "\n"
                        "%sString Table Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sString Table Size:   %" PRIu32 " (%s)\n",
                        Prefix, SymOff,
                        Prefix, SymCount,
                        Prefix, StrOff,
                        ADDR_RANGE_FMT_ARGS(StrOff, StrOff + StrSize),
                        Prefix, StrSize, Utils::FormattedSize(StrSize).data());

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
                        "\n"
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
                        "\n"
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
                fprintf(OutFile, "\t%" PRIu32 "\n", Checksum);

                break;
            }
            case MachO::LoadCommandKind::Uuid: {
                const auto UuidCmd =
                    MachO::cast<MachO::UuidCommand>(LC, IsBigEndian);

                fprintf(OutFile,
                        "\t\"%.2X%.2X%.2X%.2X-%.2X%.2X-%.2X%.2X-%.2X%.2X-"
                        "%.2X%.2x%.2x%.2x%.2X%.2X\"\n",
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
                        "\t\"" STRING_VIEW_FMT "\"\n",
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
                        "\n"
                        "%sData Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sData Size:   %" PRIu32 " (%s)\n",
                        Prefix, DataOff,
                        ADDR_RANGE_FMT_ARGS(DataOff, DataOff + DataSize),
                        Prefix, DataSize,
                        Utils::FormattedSize(DataSize).data());
                break;
            }
            case MachO::LoadCommandKind::FileSetEntry: {
                const auto &FileSetEntryCmd =
                    MachO::cast<MachO::FileSetEntryCommand>(LC, IsBigEndian);

                const auto VmAddress = FileSetEntryCmd.vmAddress(IsBigEndian);
                const auto FileOffset = FileSetEntryCmd.fileOffset(IsBigEndian);
                const auto EntryId = FileSetEntryCmd.entryId(IsBigEndian);
                const auto Reserved = FileSetEntryCmd.reserved(IsBigEndian);

                fprintf(OutFile,
                        "\n"
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
                        "\n"
                        "%sCrypt Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sCrypt Size:   %" PRIu32 " (%s)\n"
                        "%sCrypt Id:     %" PRIu32 "\n",
                        Prefix, CryptOffset,
                        ADDR_RANGE_FMT_ARGS(CryptOffset, CryptSize),
                        Prefix, CryptSize,
                        Utils::FormattedSize(CryptSize).data(),
                        Prefix, CryptId);
                break;
            }
            case MachO::LoadCommandKind::EncryptionInfo64: {
                const auto &EncryptionInfoCmd =
                    MachO::cast<MachO::EncryptionInfo64Command>(LC,
                                                                IsBigEndian);

                const auto CryptSize = EncryptionInfoCmd.cryptSize(IsBigEndian);
                const auto CryptId = EncryptionInfoCmd.cryptId(IsBigEndian);
                const auto Pad = EncryptionInfoCmd.pad(IsBigEndian);
                const auto CryptOffset =
                    EncryptionInfoCmd.cryptOffset(IsBigEndian);

                fprintf(OutFile,
                        "\n"
                        "%sCrypt Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sCrypt Size:   %" PRIu32 " (%s)\n"
                        "%sCrypt Id:     %" PRIu32 "\n"
                        "%sPad:          %" PRIu32 "\n",
                        Prefix, CryptOffset,
                        ADDR_RANGE_FMT_ARGS(CryptOffset, CryptSize),
                        Prefix, CryptSize,
                        Utils::FormattedSize(CryptSize).data(),
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
                        "\n"
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
                        Verbose ?
                            Dyld3::PlatformGetString(Platform) :
                            Dyld3::PlatformGetDesc(Platform);

                    fprintf(OutFile,
                            "\n"
                            "%sPlatform:    %s\n",
                            Prefix,
                            PlatformString.data());
                } else {
                    fprintf(OutFile,
                            "\n"
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
                        "\n"
                        "%sRebase Offset:      " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sRebase Size:        %" PRIu32 " (%s)\n"
                        "%sBind Offset:        " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sBind Size:          %" PRIu32 " (%s)\n"
                        "%sWeak Bind Offset:   " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sWeak Bind Size:     %" PRIu32 " (%s)\n"
                        "%sLazy Bind Offset:   " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sLazy Bind Size:     %" PRIu32 " (%s)\n"
                        "%sExport Trie Offset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sExport Trie Size:   %" PRIu32 " (%s)\n",
                        Prefix, RebaseOffset,
                        ADDR_RANGE_FMT_ARGS(RebaseOffset,
                                            RebaseOffset + RebaseSize),
                        Prefix, RebaseSize,
                        Utils::FormattedSize(RebaseSize).data(),
                        Prefix, BindOffset,
                        ADDR_RANGE_FMT_ARGS(BindOffset, BindOffset + BindSize),
                        Prefix, BindSize, Utils::FormattedSize(BindSize).data(),
                        Prefix, WeakBindOffset,
                        ADDR_RANGE_FMT_ARGS(WeakBindOffset,
                                            WeakBindOffset + WeakBindSize),
                        Prefix, WeakBindSize,
                        Utils::FormattedSize(WeakBindSize).data(),
                        Prefix, LazyBindOffset,
                        ADDR_RANGE_FMT_ARGS(LazyBindOffset,
                                            LazyBindOffset + LazyBindSize),
                        Prefix, LazyBindSize,
                        Utils::FormattedSize(LazyBindSize).data(),
                        Prefix, ExportTrieOffset,
                        ADDR_RANGE_FMT_ARGS(ExportTrieOffset,
                                            ExportTrieOffset + ExportTrieSize),
                        Prefix, ExportTrieSize,
                        Utils::FormattedSize(ExportTrieSize).data());
                break;
            }
            case MachO::LoadCommandKind::LinkerOption: {
                const auto &LinkerOptionCmd =
                    MachO::cast<MachO::LinkerOptionCommand>(LC, IsBigEndian);

                const auto Count = LinkerOptionCmd.count(IsBigEndian);
                fprintf(OutFile, "\n%sCount: %" PRIu32 "\n", Prefix, Count);

                break;
            }
            case MachO::LoadCommandKind::SymbolSegment: {
                const auto &SymbolSegmentCmd =
                    MachO::cast<MachO::SymbolSegmentCommand>(LC, IsBigEndian);

                const auto Offset = SymbolSegmentCmd.offset(IsBigEndian);
                const auto Size = SymbolSegmentCmd.size(IsBigEndian);

                fprintf(OutFile,
                        "\n"
                        "%sOffset: " ADDRESS_32_FMT " ("
                            ADDR_RANGE_32_FMT ")\n"
                        "%sSize:   %" PRIu32 " (%s)\n",
                        Prefix, Offset,
                        ADDR_RANGE_FMT_ARGS(Offset, Size),
                        Prefix, Size, Utils::FormattedSize(Size).data());
                break;
            }
            case MachO::LoadCommandKind::FixedVMFile: {
                const auto &FvmFileCmd =
                    MachO::cast<MachO::FvmFileCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmFileCmd.name(IsBigEndian);
                const auto HeaderAddress =
                    FvmFileCmd.headerAddress(IsBigEndian);

                fprintf(OutFile,
                        "\n"
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
                        "\n"
                        "%sEntry Offset: " ADDRESS_64_FMT "\n"
                        "%sStack Size:   %" PRIu64 " (%s)\n",
                        Prefix, EntryOffset,
                        Prefix, StackSize,
                        Utils::FormattedSize(StackSize).data());
                break;
            }
            case MachO::LoadCommandKind::SourceVersion: {
                const auto &SourceVersionCmd =
                    MachO::cast<MachO::SourceVersionCommand>(LC, IsBigEndian);

                const auto Version = SourceVersionCmd.version(IsBigEndian);
                fprintf(OutFile,
                        "\t" DYLD3_PACKED_VERSION_64_FMT "\n",
                        DYLD3_PACKED_VERSION_64_FMT_ARGS(Version));

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
                        "\n"
                        "%sData Owner: \"" STRING_VIEW_FMT "\"\n"
                        "%sOffset: " ADDRESS_64_FMT " ("
                            ADDR_RANGE_64_FMT ")\n"
                        "%sSize:   %" PRIu64 " (%s)\n",
                        Prefix, STRING_VIEW_FMT_ARGS(DataOwner),
                        Prefix, Offset,
                        ADDR_RANGE_FMT_ARGS(Offset, Size),
                        Prefix, Size, Utils::FormattedSize(Size).data());

                break;
            }
        }
    }

    auto
    PrintLoadCommands::run(const Objects::MachO &MachO) const noexcept ->
        RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);
        auto Counter = static_cast<uint32_t>(1);
        auto DylibIndex = uint32_t();

        const auto IsBigEndian = MachO.isBigEndian();
        const auto LongestLCKindLength =
            MachO::LoadCommandKindGetString(
                MachO::LoadCommandKind::LinkerOptimizationHint).length();

        for (const auto &LoadCommand : MachO.loadCommandsMap()) {
            const auto Kind = LoadCommand.kind(IsBigEndian);
            if (MachO::LoadCommandKindIsValid(Kind)) {
                fprintf(OutFile,
                        "LC %" PRIu32 ": %-*s",
                        Counter,
                        (int)LongestLCKindLength,
                        MachO::LoadCommandKindGetString(Kind).data());
            } else {
                fprintf(OutFile,
                        "LC %" PRIu32 ": <unknown> (Value: %" PRIu32 ")\n",
                        Counter,
                        static_cast<uint32_t>(Kind));
            }

            PrintLoadCommand(OutFile,
                             LoadCommand,
                             IsBigEndian,
                             DylibIndex,
                             Opt.Verbose,
                             "\t");
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

