//
//  Operations/PrintLoadCommands.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include "ADT/FlagsIterator.h"

#include "Dyld3/Platform.h"
#include "MachO/LoadCommands.h"
#include "Operations/PrintLoadCommands.h"

#include "Utils/Print.h"

namespace Operations {
    PrintLoadCommands::PrintLoadCommands(FILE *const OutFile,
                                         const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintLoadCommands), OutFile(OutFile),
      Opt(Options) {}

    bool
    PrintLoadCommands::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintLoadCommands::supportsObjectKind()");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return true;
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintLoadCommands::supportsObjectKind()");
    }

    static auto
    PrintLoadCommand(FILE *const OutFile,
                     const MachO::LoadCommand &LC,
                     const bool IsBigEndian,
                     const bool Is64Bit,
                     uint32_t &DylibIndex,
                     const bool Verbose,
                     const char *Prefix = "") noexcept
    {
        const auto Kind = LC.kind(IsBigEndian);
        constexpr auto Malformed = std::string_view("<malformed>");

        switch (Kind) {
            using namespace  MachO;
            case LoadCommandKind::Segment: {
                const auto &Segment = cast<SegmentCommand>(LC, IsBigEndian);

                const auto VmAddr = Segment.vmAddr(IsBigEndian);
                const auto VmSize = Segment.vmSize(IsBigEndian);
                const auto FileOffset = Segment.fileOffset(IsBigEndian);
                const auto FileSize = Segment.fileSize(IsBigEndian);
                const auto SectionCount = Segment.sectionCount(IsBigEndian);
                const auto Flags = Segment.flags(IsBigEndian);

                std::print(OutFile,
                           "\t\"{}\"\t{}\n"
                           "{}File:          {}\n"
                           "{}Memory:        {}\n"
                           "{}File Size:     {}\n"
                           "{}Memory Size:   {}\n"
                           "{}Flags:         0x{:x}\n",
                           Segment.segmentName(),
                           Segment.initAndMaxProt(IsBigEndian),
                           Prefix, Utils::PrintRange(FileOffset, FileSize),
                           Prefix, Utils::PrintRange(VmAddr, VmSize),
                           Prefix, Utils::ByteSize(FileSize),
                           Prefix, Utils::ByteSize(VmSize),
                           Prefix, Flags.value());

                if (!Flags.empty()) {
                    using FlagsStruct = SegmentCommand::FlagsStruct;

                    auto Counter = uint32_t();
                    for (const auto Bit : ADT::FlagsIterator(Flags)) {
                        const auto Flag =
                            static_cast<FlagsStruct::Kind>(1ull << Bit);

                        std::println(OutFile,
                                     "\t{}{}. Bit {}: {}",
                                     Prefix,
                                     Counter + 1,
                                     Bit,
                                     FlagsStruct::KindIsValid(Flag) ?
                                         FlagsStruct::KindGetString(Flag) :
                                         "<unknown>");

                        Counter++;
                    }
                }

                std::println(OutFile,
                             "{}Section Count: {}",
                             Prefix,
                             SectionCount);

                if (SectionCount == 0) {
                    break;
                }

                const auto SectionCountDigitCount =
                    Utils::GetIntegerDigitCount(SectionCount);

                auto I = 0;
                for (const auto &Section : Segment.sectionList(IsBigEndian)) {
                    const auto Addr = Section.addr(IsBigEndian);
                    const auto Size = Section.size(IsBigEndian);
                    const auto FileOffset = Section.fileOffset(IsBigEndian);

                    const auto Align = Section.align(IsBigEndian);
                    const auto AlignDesc = Utils::ByteSize(1ull << Align);

                    constexpr auto LongestAlignDescLength =
                        std::string_view("512 Bytes").length();

                    if (Verbose) {
                        std::print(OutFile,
                                   "\t{}{:>{}}. ",
                                   Prefix,
                                   I + 1,
                                   SectionCountDigitCount);
                    } else {
                        std::print(OutFile,
                                   "\t{}{:>{}}. File: {:<}\t  Memory: {:<}"
                                   "\t  Align: {:<{}}",
                                   Prefix,
                                   I + 1,
                                   SectionCountDigitCount,
                                   Utils::PrintRange(FileOffset, Size),
                                   Utils::PrintRange(Addr, Size),
                                   AlignDesc, LongestAlignDescLength);
                    }

                    std::print(OutFile,
                               "{}",
                               Utils::SegmentSectionPair(
                                Section.segmentName(),
                                Section.sectionName(),
                                /*PadSegment=*/!Verbose,
                                /*PadSection=*/!Verbose));

                    using SectionT = SegmentCommand::Section;

                    const auto SectionKind = Section.kind(IsBigEndian);
                    const auto Flags = Section.flags(IsBigEndian);

                    if (SectionKind != SectionT::Kind::Regular) {
                        const auto SectionKindDesc =
                            SectionT::KindIsValid(SectionKind) ?
                                SectionT::KindGetDesc(SectionKind) :
                                "<unknown>";

                        std::print(OutFile, " ({}", SectionKindDesc);
                        if (!Flags.attributes().empty()) {
                            std::print(OutFile, ";");
                        } else {
                            std::print(OutFile, ")");
                        }
                    }

                    if (Verbose) {
                        std::println(OutFile);
                        std::print(OutFile,
                                   "\t\t{}File:              {}\n"
                                   "\t\t{}Memory:            {}\n"
                                   "\t\t{}Size:              {}\n"
                                   "\t\t{}Align:             {} ({})\n"
                                   "\t\t{}Reloc File Offset: {}\n"
                                   "\t\t{}Reloc Count:       {}\n"
                                   "\t\t{}Reserved 1:        {}\n"
                                   "\t\t{}Reserved 2:        {}\n"
                                   "\t\t{}Flags:             0x{:x}\n",
                                   Prefix, Utils::PrintRange(FileOffset, Size),
                                   Prefix, Utils::PrintRange(Addr, Size),
                                   Prefix, Utils::ByteSize(Size),
                                   Prefix, Align, AlignDesc,
                                   Prefix,
                                     Utils::Address(
                                         Section.relocFileOffset(IsBigEndian)),
                                   Prefix,
                                     Utils::FormattedNumber(
                                         Section.relocsCount(IsBigEndian)),
                                   Prefix, Section.reserved1(IsBigEndian),
                                   Prefix, Section.reserved2(IsBigEndian),
                                   Prefix, Flags.value());

                            auto FlagNumber = uint32_t();
                            for (const auto Bit :
                                    ADT::FlagsIterator(Flags.attributes()))
                            {
                                const auto Attr =
                                    SectionT::Attribute(1ull << Bit);
                                const auto AttrString =
                                    SectionT::AttributeIsValid(Attr) ?
                                        SectionT::AttributeGetString(Attr) :
                                        "<unknown>";

                                std::println(OutFile,
                                             "\t\t\t{}{}. Bit {}: {}",
                                             Prefix,
                                             FlagNumber + 1,
                                             Bit,
                                             AttrString);

                                FlagNumber++;
                            }
                    } else if (!Flags.attributes().empty()) {
                        if (SectionKind == SectionT::Kind::Regular) {
                            std::print(OutFile, " (Regular;");
                        }

                        auto Iterator = ADT::FlagsIterator(Flags.attributes());
                        for (auto Iter = Iterator.begin();;) {
                            const auto Bit = *Iter;
                            const auto Attr = SectionT::Attribute(1ull << Bit);

                            if (SectionT::AttributeIsValid(Attr)) {
                                const auto AttrDesc =
                                    SectionT::AttributeGetDesc(Attr);

                                std::print(OutFile, " {}", AttrDesc);
                            } else {
                                std::print(OutFile, " <unknown: Bit {}>", Bit);
                            }

                            Iter++;
                            if (Iter == Iterator.end()) {
                                break;
                            }

                            std::print(OutFile, ",");
                        }

                        std::println(OutFile, ")");
                    } else {
                        std::println(OutFile);
                    }

                    I++;
                }

                break;
            }
            case LoadCommandKind::Segment64: {
                const auto &Segment = cast<SegmentCommand64>(LC, IsBigEndian);

                const auto VmAddr = Segment.vmAddr(IsBigEndian);
                const auto VmSize = Segment.vmSize(IsBigEndian);
                const auto FileOffset = Segment.fileOffset(IsBigEndian);
                const auto FileSize = Segment.fileSize(IsBigEndian);
                const auto SectionCount = Segment.sectionCount(IsBigEndian);
                const auto Flags = Segment.flags(IsBigEndian);

                std::print(OutFile,
                           "\t\"{}\"\t{}\n"
                           "{}File:          {}\n"
                           "{}Memory:        {}\n"
                           "{}File Size:     {}\n"
                           "{}Memory Size:   {}\n"
                           "{}Flags:         0x{:x}\n",
                           Segment.segmentName(),
                           Segment.initAndMaxProt(IsBigEndian),
                           Prefix, Utils::PrintRange(FileOffset, FileSize),
                           Prefix, Utils::PrintRange(VmAddr, VmSize),
                           Prefix, Utils::ByteSize(FileSize),
                           Prefix, Utils::ByteSize(VmSize),
                           Prefix, Flags.value());

                if (!Flags.empty()) {
                    using FlagsStruct = SegmentCommand64::FlagsStruct;

                    auto Counter = uint32_t();
                    for (const auto Bit : ADT::FlagsIterator(Flags)) {
                        const auto Flag =
                            static_cast<FlagsStruct::Kind>(1ull << Bit);

                        std::println(OutFile,
                                     "\t{}{}. Bit {}: {}",
                                     Prefix,
                                     Counter + 1,
                                     Bit,
                                     FlagsStruct::KindIsValid(Flag) ?
                                         FlagsStruct::KindGetString(Flag) :
                                         "<unknown>");

                        Counter++;
                    }
                }

                std::println(OutFile,
                             "{}Section Count: {}",
                             Prefix,
                             SectionCount);

                if (SectionCount == 0) {
                    break;
                }

                const auto SectionCountDigitCount =
                    Utils::GetIntegerDigitCount(SectionCount);

                auto I = 0;
                for (const auto &Section : Segment.sectionList(IsBigEndian)) {
                    const auto Addr = Section.addr(IsBigEndian);
                    const auto Size = Section.size(IsBigEndian);
                    const auto FileOffset = Section.fileOffset(IsBigEndian);

                    const auto Align = Section.align(IsBigEndian);
                    const auto AlignDesc = Utils::ByteSize(1ull << Align);

                    constexpr auto LongestAlignDescLength =
                        std::string_view("512 Bytes").length();

                    if (Verbose) {
                        std::print(OutFile,
                                   "\t{}{:>{}}. ",
                                   Prefix,
                                   I + 1,
                                   SectionCountDigitCount);
                    } else {
                        std::print(OutFile,
                                   "\t{}{:>{}}. File: {:<}\t  Memory: {:<}"
                                   "\t  Align: {:>{}}",
                                   Prefix,
                                   I + 1,
                                   SectionCountDigitCount,
                                   Utils::PrintRange<uint64_t>(FileOffset,
                                                               Size),
                                   Utils::PrintRange(Addr, Size),
                                   AlignDesc, LongestAlignDescLength);
                    }

                    std::print(OutFile,
                               "{}",
                               Utils::SegmentSectionPair(
                                Section.segmentName(),
                                Section.sectionName(),
                                /*PadSegment=*/!Verbose,
                                /*PadSection=*/!Verbose));

                    using SectionT = SegmentCommand::Section;

                    const auto SectionKind = Section.kind(IsBigEndian);
                    const auto Flags = Section.flags(IsBigEndian);

                    if (SectionKind != SectionT::Kind::Regular) {
                        const auto SectionKindDesc =
                            SectionT::KindIsValid(SectionKind) ?
                                SectionT::KindGetDesc(SectionKind) :
                                "<unknown>";

                        std::print(OutFile, " ({}", SectionKindDesc);
                        if (!Flags.attributes().empty()) {
                            std::print(OutFile, ";");
                        } else {
                            std::print(OutFile, ")");
                        }
                    }

                    if (Verbose) {
                        const auto Align = Section.align(IsBigEndian);
                        const auto AlignDesc = Utils::ByteSize(1ull << Align);

                        std::println(OutFile);
                        std::print(OutFile,
                                   "\t\t{}File:              {}\n"
                                   "\t\t{}Memory:            {}\n"
                                   "\t\t{}Size:              {}\n"
                                   "\t\t{}Align:             {} ({})\n"
                                   "\t\t{}Reloc File Offset: {}\n"
                                   "\t\t{}Reloc Count:       {}\n"
                                   "\t\t{}Reserved 1:        {}\n"
                                   "\t\t{}Reserved 2:        {}\n"
                                   "\t\t{}Flags:             0x{:x}\n",
                                   Prefix,
                                    Utils::PrintRange<uint64_t>(FileOffset,
                                                                Size),
                                   Prefix, Utils::PrintRange(Addr, Size),
                                   Prefix, Utils::ByteSize(Size),
                                   Prefix, Align, AlignDesc,
                                   Prefix,
                                       Utils::Address(
                                           Section.relocFileOffset(IsBigEndian)),
                                   Prefix,
                                       Utils::FormattedNumber(
                                           Section.relocsCount(IsBigEndian)),
                                   Prefix, Section.reserved1(IsBigEndian),
                                   Prefix, Section.reserved2(IsBigEndian),
                                   Prefix, Flags.value());

                            auto FlagNumber = uint32_t();
                            for (const auto Bit :
                                    ADT::FlagsIterator(Flags.attributes()))
                            {
                                const auto Attr =
                                    SectionT::Attribute(1ull << Bit);
                                const auto AttrString =
                                    SectionT::AttributeIsValid(Attr) ?
                                        SectionT::AttributeGetString(Attr) :
                                        "<unknown>";

                                std::println(OutFile,
                                             "\t\t\t{}{}. Bit {}: {}",
                                             Prefix,
                                             FlagNumber + 1,
                                             Bit,
                                             AttrString);

                                FlagNumber++;
                            }
                    } else if (!Flags.attributes().empty()) {
                        if (SectionKind == SectionT::Kind::Regular) {
                            std::print(OutFile, " (Regular;");
                        }

                        auto Iterator = ADT::FlagsIterator(Flags.attributes());
                        for (auto Iter = Iterator.begin();;) {
                            const auto Bit = *Iter;
                            const auto Attr = SectionT::Attribute(1ull << Bit);

                            if (SectionT::AttributeIsValid(Attr)) {
                                const auto AttrDesc =
                                    SectionT::AttributeGetDesc(Attr);

                                std::print(OutFile, " {}", AttrDesc);
                            } else {
                                std::print(OutFile, " <unknown: Bit {}>", Bit);
                            }

                            Iter++;
                            if (Iter == Iterator.end()) {
                                break;
                            }

                            std::print(OutFile, ",");
                        }

                        std::println(OutFile, ")");
                    } else {
                        std::println(OutFile);
                    }

                    I++;
                }

                break;
            }
            case LoadCommandKind::Thread:
            case LoadCommandKind::UnixThread:
                std::println(OutFile);
                break;
            case LoadCommandKind::LoadFixedVMSharedLib:
            case LoadCommandKind::IdFixedVMSharedLib: {
                const auto &FvmLib = cast<FvmLibraryCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmLib.name(IsBigEndian);
                const auto MinorVersion =
                    FvmLib.Library.minorVersion(IsBigEndian);
                const auto HeaderAddress =
                    FvmLib.Library.headerAddress(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Name:           \"{}\"\n"
                           "{}Minor Version:  {}\n"
                           "{}Header Address: {}\n",
                           Prefix,
                            NameOpt.has_value() ? NameOpt.value() : Malformed,
                           Prefix, MinorVersion,
                           Prefix, HeaderAddress);

                break;
            }
            case LoadCommandKind::Identity:
                break;
            case LoadCommandKind::LoadDylib:
            case LoadCommandKind::IdDylib:
            case LoadCommandKind::ReexportDylib:
            case LoadCommandKind::LazyLoadDylib:
            case LoadCommandKind::LoadUpwardDylib:
            case LoadCommandKind::LoadWeakDylib: {
                const auto &DylibCmd = cast<DylibCommand>(LC, IsBigEndian);
                const auto NameOpt = DylibCmd.name(IsBigEndian);

                std::print(OutFile,
                           "\t\"{}\"",
                           NameOpt.has_value() ? NameOpt.value() : Malformed);

                if (Kind != MachO::LoadCommandKind::IdDylib) {
                    DylibIndex++;
                }

                if (!Verbose) {
                    std::println(OutFile);
                    break;
                }

                const auto Timestamp = DylibCmd.timestamp(IsBigEndian);
                const auto CompatVersion = DylibCmd.compatVersion(IsBigEndian);
                const auto CurrentVersion =
                    DylibCmd.currentVersion(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Current Version: {}\n"
                           "{}Compat Version:  {}\n"
                           "{}Timestamp:       {} (Value: {})\n",
                           Prefix, CurrentVersion,
                           Prefix, CompatVersion,
                           Prefix, Utils::Timestamp(Timestamp),
                            Utils::FormattedNumber(Timestamp));

                break;
            }
            case LoadCommandKind::SubFramework: {
                const auto &SubFramework =
                    cast<SubFrameworkCommand>(LC, IsBigEndian);

                const auto UmbrellaOpt = SubFramework.umbrella(IsBigEndian);
                std::println(OutFile,
                             "\t\"{}\"",
                              UmbrellaOpt.has_value() ?
                                  UmbrellaOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::SubClient: {
                const auto &SubClient = cast<SubClientCommand>(LC, IsBigEndian);
                const auto ClientOpt = SubClient.client(IsBigEndian);

                std::println(OutFile,
                             "\t\"{}\"",
                              ClientOpt.has_value() ?
                                  ClientOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::SubUmbrella: {
                const auto &SubUmbrella =
                    cast<SubUmbrellaCommand>(LC, IsBigEndian);
                const auto SubUmbrellaOpt =
                    SubUmbrella.subUmbrella(IsBigEndian);

                std::println(OutFile,
                             "\t\"{}\"",
                             SubUmbrellaOpt.has_value() ?
                                  SubUmbrellaOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::SubLibrary: {
                const auto &SubLibrary =
                    cast<SubLibraryCommand>(LC, IsBigEndian);

                const auto SubLibraryOpt = SubLibrary.subLibrary(IsBigEndian);
                std::println(OutFile,
                             "\t\"{}\"",
                              SubLibraryOpt.has_value() ?
                                SubLibraryOpt.value() : Malformed);

                break;
            }
            case MachO::LoadCommandKind::PreBoundDylib: {
                const auto &PreboundDylibCmd =
                    cast<PreboundDylibCommand>(LC, IsBigEndian);

                const auto NameOpt = PreboundDylibCmd.name(IsBigEndian);
                std::println(OutFile,
                             "\t\"{}\"",
                             NameOpt.has_value() ? NameOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::IdDylinker:
            case LoadCommandKind::LoadDylinker: {
                const auto &DylinkerCmd =
                    cast<DylinkerCommand>(LC, IsBigEndian);

                const auto NameOpt = DylinkerCmd.name(IsBigEndian);
                std::println(OutFile,
                             "\t\"{}\"",
                             NameOpt.has_value() ? NameOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::Routines: {
                const auto &RoutinesCmd =
                    cast<RoutinesCommand>(LC, IsBigEndian);

                const auto InitAddress = RoutinesCmd.initAddress(IsBigEndian);
                const auto InitModule = RoutinesCmd.initModule(IsBigEndian);

                const auto Reserved1 = RoutinesCmd.reserved1(IsBigEndian);
                const auto Reserved2 = RoutinesCmd.reserved2(IsBigEndian);
                const auto Reserved3 = RoutinesCmd.reserved3(IsBigEndian);
                const auto Reserved4 = RoutinesCmd.reserved4(IsBigEndian);
                const auto Reserved5 = RoutinesCmd.reserved5(IsBigEndian);
                const auto Reserved6 = RoutinesCmd.reserved6(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Init Address: {}\n"
                           "{}Init Module:  {}\n"
                           "{}Reserved 1:   {}\n"
                           "{}Reserved 2:   {}\n"
                           "{}Reserved 3:   {}\n"
                           "{}Reserved 4:   {}\n"
                           "{}Reserved 5:   {}\n"
                           "{}Reserved 6:   {}\n",
                           Prefix, Utils::Address(InitAddress),
                           Prefix, InitModule,
                           Prefix, Reserved1,
                           Prefix, Reserved2,
                           Prefix, Reserved3,
                           Prefix, Reserved4,
                           Prefix, Reserved5,
                           Prefix, Reserved6);
                break;
            }
            case LoadCommandKind::Routines64: {
                const auto &RoutinesCmd =
                    cast<RoutinesCommand64>(LC, IsBigEndian);

                const auto InitAddress = RoutinesCmd.initAddress(IsBigEndian);
                const auto InitModule = RoutinesCmd.initModule(IsBigEndian);

                const auto Reserved1 = RoutinesCmd.reserved1(IsBigEndian);
                const auto Reserved2 = RoutinesCmd.reserved2(IsBigEndian);
                const auto Reserved3 = RoutinesCmd.reserved3(IsBigEndian);
                const auto Reserved4 = RoutinesCmd.reserved4(IsBigEndian);
                const auto Reserved5 = RoutinesCmd.reserved5(IsBigEndian);
                const auto Reserved6 = RoutinesCmd.reserved6(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Init Address: {}\n"
                           "{}Init Module:  {}\n"
                           "{}Reserved 1:   {}\n"
                           "{}Reserved 2:   {}\n"
                           "{}Reserved 3:   {}\n"
                           "{}Reserved 4:   {}\n"
                           "{}Reserved 5:   {}\n"
                           "{}Reserved 6:   {}\n",
                           Prefix, Utils::Address(InitAddress),
                           Prefix, InitModule,
                           Prefix, Reserved1,
                           Prefix, Reserved2,
                           Prefix, Reserved3,
                           Prefix, Reserved4,
                           Prefix, Reserved5,
                           Prefix, Reserved6);
                break;
            }
            case LoadCommandKind::SymbolTable: {
                const auto &SymTabCmd = cast<SymTabCommand>(LC, IsBigEndian);

                const auto SymOff = SymTabCmd.symOffset(IsBigEndian);
                const auto SymCount = SymTabCmd.symCount(IsBigEndian);

                const auto StrOff = SymTabCmd.strOffset(IsBigEndian);
                const auto StrSize = SymTabCmd.strSize(IsBigEndian);

                const auto SymRange =
                    Utils::PrintRange<decltype(SymOff)>(
                        SymTabCmd.symRange(IsBigEndian, Is64Bit));

                std::println(OutFile);
                std::print(OutFile,
                           "{}Symbol Table Offset: {} ({})\n"
                           "{}Symbol Count:        {} ({})\n"
                           "{}String Table Offset: {} ({})\n"
                           "{}String Table Size:   {}\n",
                           Prefix, Utils::Address(SymOff), SymRange,
                           Prefix, Utils::FormattedNumber(SymCount),
                            Utils::ByteSize(SymRange.size()),
                           Prefix, Utils::Address(StrOff),
                            Utils::PrintRange(StrOff, StrSize),
                           Prefix, Utils::ByteSize(StrSize));

                break;
            }
            case LoadCommandKind::DynamicSymbolTable: {
                const auto &DySymTabCmd =
                    cast<DynamicSymTabCommand>(LC, IsBigEndian);

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
                    DySymTabCmd.indirectSymbolsCount(IsBigEndian);
                const auto ExternRelOffset =
                    DySymTabCmd.externRelOffset(IsBigEndian);
                const auto ExternRelCount =
                    DySymTabCmd.externRelCount(IsBigEndian);
                const auto LocalRelOffset =
                    DySymTabCmd.localRelOffset(IsBigEndian);
                const auto LocalRelCount =
                    DySymTabCmd.localRelCount(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Local Symbols Index:       {}\n"
                           "{}Local Symbols Count:       {}\n"
                           "{}Extern Def Symbols Index:  {}\n"
                           "{}Extern Def Symbols Count:  {}\n"
                           "{}UnDef Symbols Index:       {}\n"
                           "{}UnDef Symbols Count:       {}\n"
                           "{}Table Of Contents Offset:  {}\n"
                           "{}Table Of Contents Count:   {}\n"
                           "{}Modules Tab Offset:        {}\n"
                           "{}Modules Tab Count:         {}\n"
                           "{}Extern Ref Symbols Offset: {}\n"
                           "{}Extern Ref Symbols Count:  {}\n"
                           "{}Indirect Symbols Offset:   {}\n"
                           "{}Indirect Symbols Count:    {}\n"
                           "{}Extern Relocations Offset: {}\n"
                           "{}Extern Relocations Count:  {}\n"
                           "{}Local Relocations Offset:  {}\n"
                           "{}Local Relocations Count:   {}\n",
                           Prefix, Utils::FormattedNumber(LocalSymbolsIndex),
                           Prefix, Utils::FormattedNumber(LocalSymbolsCount),
                           Prefix,
                            Utils::FormattedNumber(ExternDefSymbolsIndex),
                           Prefix,
                            Utils::FormattedNumber(ExternDefSymbolsCount),
                           Prefix, Utils::FormattedNumber(UndefSymbolsIndex),
                           Prefix, Utils::FormattedNumber(UndefSymbolsCount),
                           Prefix, Utils::Address(TableOfContentsOffset),
                           Prefix,
                            Utils::FormattedNumber(TableOfContentsCount),
                           Prefix, Utils::Address(ModulesTabOffset),
                           Prefix, Utils::FormattedNumber(ModulesTabCount),
                           Prefix, Utils::Address(ExternRefSymbolsOffset),
                           Prefix,
                            Utils::FormattedNumber(ExternRefSymbolsCount),
                           Prefix, Utils::Address(IndirectSymbolsOffset),
                           Prefix,
                            Utils::FormattedNumber(IndirectSymbolsCount),
                           Prefix, Utils::Address(ExternRelOffset),
                           Prefix, Utils::FormattedNumber(ExternRelCount),
                           Prefix, Utils::Address(LocalRelOffset),
                           Prefix, Utils::FormattedNumber(LocalRelCount));
                break;
            }
            case LoadCommandKind::TwoLevelHints: {
                const auto &TwoLevelHintsCmd =
                    cast<TwoLevelHintsCommand>(LC, IsBigEndian);

                const auto Offset = TwoLevelHintsCmd.offset(IsBigEndian);
                const auto HintsCount =
                    TwoLevelHintsCmd.hintsCount(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Offset:      {}\n"
                           "{}Hints Count: {}\n",
                           Prefix, Utils::Address(Offset),
                           Prefix, Utils::FormattedNumber(HintsCount));
                break;
            }
            case LoadCommandKind::PreBindChecksum: {
                const auto PrebindChecksumCmd =
                    cast<PrebindChecksumCommand>(LC, IsBigEndian);

                const auto Checksum = PrebindChecksumCmd.checksum(IsBigEndian);
                std::println(OutFile, "\t{}", Checksum);

                break;
            }
            case LoadCommandKind::Uuid: {
                const auto UuidCmd = cast<UuidCommand>(LC, IsBigEndian);
                std::println(OutFile, "\t\"{}\"", Utils::Uuid(UuidCmd.Uuid));
                break;
            }
            case LoadCommandKind::Rpath: {
                const auto &RpathCmd = cast<RpathCommand>(LC, IsBigEndian);
                const auto PathOpt = RpathCmd.path(IsBigEndian);

                std::println(OutFile,
                             "\t\"{}\"",
                             PathOpt.has_value() ? PathOpt.value() : Malformed);
                break;
            }
            case LoadCommandKind::CodeSignature:
            case LoadCommandKind::SegmentSplitInfo:
            case LoadCommandKind::FunctionStarts:
            case LoadCommandKind::DataInCode:
            case LoadCommandKind::DylibCodeSignDRS:
            case LoadCommandKind::LinkerOptimizationHint:
            case LoadCommandKind::DyldExportsTrie:
            case LoadCommandKind::DyldChainedFixups: {
                const auto &LinkeditDataCmd =
                    cast<LinkeditDataCommand>(LC, IsBigEndian);

                const auto DataOff = LinkeditDataCmd.dataOff(IsBigEndian);
                const auto DataSize = LinkeditDataCmd.dataSize(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Data Offset: {} ({})\n"
                           "{}Data Size:   {}\n",
                           Prefix, Utils::Address(DataOff),
                            Utils::PrintRange(DataOff, DataSize),
                           Prefix, Utils::ByteSize(DataSize));
                break;
            }
            case LoadCommandKind::FileSetEntry: {
                const auto &FileSetEntryCmd =
                    cast<FileSetEntryCommand>(LC, IsBigEndian);

                const auto VmAddress = FileSetEntryCmd.vmAddress(IsBigEndian);
                const auto FileOffset = FileSetEntryCmd.fileOffset(IsBigEndian);
                const auto EntryIdOpt = FileSetEntryCmd.entryId(IsBigEndian);
                const auto Reserved = FileSetEntryCmd.reserved(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Vm Address:  {}\n"
                           "{}File Offset: {}\n"
                           "{}Entry Id:    {}\n"
                           "{}Reserved:    {}\n",
                           Prefix, Utils::Address(VmAddress),
                           Prefix, Utils::Address(FileOffset),
                           Prefix,
                                EntryIdOpt.has_value() ?
                                    EntryIdOpt.value() : Malformed,
                           Prefix, Reserved);
                break;
            }
            case LoadCommandKind::EncryptionInfo: {
                const auto &EncryptionInfoCmd =
                    cast<EncryptionInfoCommand>(LC, IsBigEndian);

                const auto CryptSize = EncryptionInfoCmd.cryptSize(IsBigEndian);
                const auto CryptId = EncryptionInfoCmd.cryptId(IsBigEndian);
                const auto CryptOffset =
                    EncryptionInfoCmd.cryptOffset(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Crypt Offset: {} ({})\n"
                           "{}Crypt Size:   {}\n"
                           "{}Crypt Id:     {}\n",
                           Prefix, CryptOffset,
                            Utils::PrintRange(CryptOffset, CryptSize),
                           Prefix, Utils::ByteSize(CryptSize),
                           Prefix, CryptId);
                break;
            }
            case LoadCommandKind::EncryptionInfo64: {
                const auto &EncryptionInfoCmd =
                    cast<EncryptionInfo64Command>(LC, IsBigEndian);

                const auto CryptSize = EncryptionInfoCmd.cryptSize(IsBigEndian);
                const auto CryptId = EncryptionInfoCmd.cryptId(IsBigEndian);
                const auto Pad = EncryptionInfoCmd.pad(IsBigEndian);
                const auto CryptOffset =
                    EncryptionInfoCmd.cryptOffset(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Crypt Offset: {} ({})\n"
                           "{}Crypt Size:   {}\n"
                           "{}Crypt Id:     {}\n"
                           "{}Pad:          {}\n",
                           Prefix, Utils::Address(CryptOffset),
                            Utils::PrintRange(CryptOffset, CryptSize),
                           Prefix, Utils::ByteSize(CryptSize),
                           Prefix, Utils::FormattedNumber(CryptId),
                           Prefix, Pad);
                break;
            }
            case LoadCommandKind::VersionMinMacOS:
            case LoadCommandKind::VersionMinIOS:
            case LoadCommandKind::VersionMinTVOS:
            case LoadCommandKind::VersionMinWatchOS: {
                const auto &VersionMinCmd =
                    cast<VersionMinCommand>(LC, IsBigEndian);

                const auto Version = VersionMinCmd.version(IsBigEndian);
                const auto Sdk = VersionMinCmd.sdk(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Version: {}\n"
                           "{}SDK:     {}\n",
                           Prefix, Version,
                           Prefix, Sdk);
                break;
            }
            case LoadCommandKind::BuildVersion: {
                const auto &BuildVersionCmd =
                    cast<BuildVersionCommand>(LC, IsBigEndian);

                const auto Platform = BuildVersionCmd.platform(IsBigEndian);
                if (Dyld3::PlatformIsValid(Platform)) {
                    const auto PlatformString =
                        Verbose ?
                            Dyld3::PlatformGetString(Platform) :
                            Dyld3::PlatformGetDesc(Platform);

                    std::println(OutFile);
                    std::println(OutFile,
                                 "{}Platform:    {}",
                                 Prefix,
                                 PlatformString);
                } else {
                    std::println(OutFile);
                    std::println(OutFile,
                                 "{}Platform:    <Unknown> (Value: {})",
                                 Prefix,
                                 static_cast<uint32_t>(Platform));
                }

                const auto MinOS = BuildVersionCmd.minOS(IsBigEndian);
                const auto Sdk = BuildVersionCmd.sdk(IsBigEndian);
                const auto ToolsCount = BuildVersionCmd.toolsCount(IsBigEndian);

                std::print(OutFile,
                           "{}Minimum OS:  {}\n"
                           "{}SDK:         {}\n"
                           "{}Tools Count: {}\n",
                           Prefix, MinOS,
                           Prefix, Sdk,
                           Prefix, ToolsCount);

                if (ToolsCount == 0) {
                    break;
                }

                auto Counter = uint32_t();
                for (const auto &Tool : BuildVersionCmd.toolList(IsBigEndian)) {
                    const auto Version = Tool.version(IsBigEndian);
                    const auto ToolValue = Tool.tool(IsBigEndian);
                    const auto ToolValueString =
                        BuildToolIsValid(ToolValue) ?
                            Verbose ?
                                BuildToolGetString(ToolValue) :
                                BuildToolGetDesc(ToolValue) :
                            "<unknown>";

                    std::print(OutFile,
                               "{}\t{}. Tool: {}\n"
                               "{}\t\tVersion: {}\n",
                               Prefix, Counter + 1, ToolValueString,
                               Prefix, Version);

                    Counter++;
                }

                break;
            }
            case LoadCommandKind::DyldInfo:
            case LoadCommandKind::DyldInfoOnly:
            case LoadCommandKind::DyldEnvironment: {
                const auto &DyldInfoCmd =
                    cast<DyldInfoCommand>(LC, IsBigEndian);

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

                std::println(OutFile);
                std::print(OutFile,
                           "{}Rebase Offset:      {} ({})\n"
                           "{}Rebase Size:        {}\n"
                           "{}Bind Offset:        {} ({})\n"
                           "{}Bind Size:          {}\n"
                           "{}Weak Bind Offset:   {} ({})\n"
                           "{}Bind Size:          {}\n"
                           "{}Bind Offset:        {} ({})\n"
                           "{}Lazy Bind Size:     {}\n"
                           "{}Export Trie Offset: {} ({})\n"
                           "{}Export Trie Size:   {}\n",
                           Prefix, Utils::Address(RebaseOffset),
                            Utils::PrintRange(RebaseOffset, RebaseSize),
                           Prefix, Utils::ByteSize(RebaseSize),
                           Prefix, Utils::Address(BindOffset),
                            Utils::PrintRange(BindOffset, BindSize),
                           Prefix, Utils::ByteSize(BindSize),
                           Prefix, Utils::Address(WeakBindOffset),
                            Utils::PrintRange(WeakBindOffset, WeakBindSize),
                           Prefix, Utils::ByteSize(WeakBindSize),
                           Prefix, Utils::Address(LazyBindOffset),
                            Utils::PrintRange(LazyBindOffset, LazyBindSize),
                           Prefix, Utils::ByteSize(LazyBindSize),
                           Prefix, Utils::Address(ExportTrieOffset),
                            Utils::PrintRange(ExportTrieOffset, ExportTrieSize),
                           Prefix, Utils::ByteSize(ExportTrieSize));
                break;
            }
            case LoadCommandKind::LinkerOption: {
                const auto &LinkerOptionCmd =
                    cast<LinkerOptionCommand>(LC, IsBigEndian);

                const auto Count = LinkerOptionCmd.count(IsBigEndian);
                std::println(OutFile, "\n{}Count: {}", Prefix, Count);

                break;
            }
            case LoadCommandKind::SymbolSegment: {
                const auto &SymbolSegmentCmd =
                    cast<SymbolSegmentCommand>(LC, IsBigEndian);

                const auto Offset = SymbolSegmentCmd.offset(IsBigEndian);
                const auto Size = SymbolSegmentCmd.size(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Offset: {} ({})\n"
                           "{}Size:   {}\n",
                           Prefix, Offset, Utils::PrintRange(Offset, Size),
                           Prefix, Utils::ByteSize(Size));
                break;
            }
            case LoadCommandKind::FixedVMFile: {
                const auto &FvmFileCmd = cast<FvmFileCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmFileCmd.name(IsBigEndian);
                const auto HeaderAddress =
                    FvmFileCmd.headerAddress(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Name:           \"{}\"\n"
                           "{}Header Address: {}\n",
                           Prefix,
                            NameOpt.has_value() ? NameOpt.value() : Malformed,
                           Prefix, HeaderAddress);
                break;
            }
            case LoadCommandKind::Main: {
                const auto &EntryPointCmd =
                    cast<EntryPointCommand>(LC, IsBigEndian);

                const auto EntryOffset = EntryPointCmd.entryOffset(IsBigEndian);
                const auto StackSize = EntryPointCmd.stackSize(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Entry Offset: {}\n"
                           "{}Stack Size:   {}\n",
                           Prefix, EntryOffset,
                           Prefix, Utils::ByteSize(StackSize));
                break;
            }
            case LoadCommandKind::SourceVersion: {
                const auto &SourceVersionCmd =
                    cast<SourceVersionCommand>(LC, IsBigEndian);

                const auto Version = SourceVersionCmd.version(IsBigEndian);
                std::println(OutFile, "\t{}", Version);

                break;
            }
            case LoadCommandKind::PrePage:
                break;
            case LoadCommandKind::Note: {
                const auto &NoteCmd = cast<NoteCommand>(LC, IsBigEndian);
                const auto DataOwner = NoteCmd.dataOwner();
                const auto Offset = NoteCmd.offset(IsBigEndian);
                const auto Size = NoteCmd.size(IsBigEndian);

                std::println(OutFile);
                std::print(OutFile,
                           "{}Data Owner: \"{}\"\n"
                           "{}Offset: {} ({})\n"
                           "{}Size:   {}\n",
                           Prefix, DataOwner,
                           Prefix, Offset, Utils::PrintRange(Offset, Size),
                           Prefix, Utils::ByteSize(Size));

                break;
            }
        }
    }

    auto PrintLoadCommands::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        auto Counter = static_cast<uint32_t>(1);
        auto DylibIndex = uint32_t();

        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

        const auto LongestLCKindLength =
            MachO::LoadCommandKindGetString(
                MachO::LoadCommandKind::LinkerOptimizationHint).length();

        const auto NcmdsDigitCount =
            Utils::GetIntegerDigitCount(MachO.header().ncmds());

        for (const auto &LoadCommand : MachO.loadCommandsMap()) {
            const auto Kind = LoadCommand.kind(IsBigEndian);
            if (MachO::LoadCommandKindIsValid(Kind)) {
                std::print(OutFile,
                           "LC {:>{}}: {:<{}}",
                           Counter,
                           NcmdsDigitCount,
                           MachO::LoadCommandKindGetString(Kind),
                           LongestLCKindLength);
            } else {
                std::print(OutFile,
                           "LC {:>{}}: <unknown> (Value: {})\n"
                           "\tCmdSize: {}\n",
                           Counter,
                           NcmdsDigitCount,
                           static_cast<uint32_t>(Kind),
                           LoadCommand.cmdsize(IsBigEndian));
            }

            PrintLoadCommand(OutFile,
                             LoadCommand,
                             IsBigEndian,
                             Is64Bit,
                             DylibIndex,
                             Opt.Verbose,
                             "\t");
            Counter++;
        }

        return RunResult();
    }

    auto PrintLoadCommands::run(const Objects::Base &Base) const noexcept ->
        RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintLoadCommands::run() got Object with Kind::None");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false && "Got unrecognized Object-Kind in PrintHeader::run");
    }
}

