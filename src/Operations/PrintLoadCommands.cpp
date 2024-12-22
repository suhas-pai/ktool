//
//  Operations/PrintLoadCommands.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include "ADT/FlagsIterator.h"
#include "ADT/Maximizer.h"

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
                     uint32_t &DylibIndex,
                     uint32_t MaxDylibPathLength,
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
                           "{}Mem:           {}\n"
                           "{}File Size:     {}\n"
                           "{}Mem Size:      {}\n"
                           "{}Flags:         0x{:x}\n",
                           Segment.segmentName(),
                           Mach::VmProtInitMax(Segment.initProt(IsBigEndian),
                                               Segment.maxProt(IsBigEndian)),
                           Prefix, ADT::Range::FromSize(FileOffset, FileSize),
                           Prefix, ADT::Range::FromSize(VmAddr, VmSize),
                           Prefix, Utils::ByteSize(FileSize),
                           Prefix, Utils::ByteSize(VmSize),
                           Prefix, Flags.value());

                if (!Flags.empty()) {
                    using FlagsStruct = SegmentCommand::FlagsStruct;

                    auto Counter = uint32_t();
                    for (const auto Bit : ADT::FlagsIterator(Flags)) {
                        const auto Flag =
                            static_cast<FlagsStruct::Kind>(1ull << Bit);

                        std::print(OutFile,
                                   "\t{}{}. Bit {}: {}\n",
                                   Prefix,
                                   Counter + 1,
                                   Bit,
                                   FlagsStruct::KindIsValid(Flag) ?
                                       FlagsStruct::KindGetString(Flag).data() :
                                       "<unknown>");

                        Counter++;
                    }
                }

                std::print(OutFile,
                           "{}Section Count: {}\n",
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
                                   "\t{}{:>{}}. File: {:<}\tMem: {:<}\tAlign: ",
                                   Prefix,
                                   I + 1,
                                   SectionCountDigitCount,
                                   Utils::PrintRange(FileOffset, Size),
                                   Utils::PrintRange(Addr, Size));

                        std::print(OutFile,
                                   "{:<{}}",
                                   AlignDesc,
                                   LongestAlignDescLength);
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
                       std::print(OutFile,
                                  "\n"
                                  "\t\t{}File:              {}\n"
                                  "\t\t{}Mem:               {}\n"
                                  "\t\t{}Size:              {}\n"
                                  "\t\t{}Align:             {} ({})\n"
                                  "\t\t{}Reloc File Offset: {}\n"
                                  "\t\t{}Reloc Count:       {}\n"
                                  "\t\t{}Reserved 1:        {}\n"
                                  "\t\t{}Reserved 2:        {}\n"
                                  "\t\t{}Flags:             0x{:x}\n",
                                  Prefix,
                                    ADT::Range::FromSize(FileOffset, Size),
                                  Prefix, ADT::Range::FromSize(Addr, Size),
                                  Prefix, Utils::ByteSize(Size),
                                  Prefix, Align, AlignDesc,
                                  Prefix,
                                    Utils::Address(
                                        Section.relocFileOffset(IsBigEndian)),
                                  Prefix,
                                    Utils::NumberWithCommas(
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

                                std::print(OutFile,
                                           "\t\t\t{}{}. Bit {}: {}\n",
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

                        std::print(OutFile, ")\n");
                    } else {
                        std::print(OutFile, "\n");
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
                           "{}Mem:           {}\n"
                           "{}File Size:     {}\n"
                           "{}Mem Size:      {}\n"
                           "{}Flags:         0x{:x}\n",
                           Segment.segmentName(),
                           Mach::VmProtInitMax(Segment.initProt(IsBigEndian),
                                               Segment.maxProt(IsBigEndian)),
                           Prefix, ADT::Range::FromSize(FileOffset, FileSize),
                           Prefix, ADT::Range::FromSize(VmAddr, VmSize),
                           Prefix, Utils::ByteSize(FileSize),
                           Prefix, Utils::ByteSize(VmSize),
                           Prefix, Flags.value());

                if (!Flags.empty()) {
                    using FlagsStruct = SegmentCommand64::FlagsStruct;

                    auto Counter = uint32_t();
                    for (const auto Bit : ADT::FlagsIterator(Flags)) {
                        const auto Flag =
                            static_cast<FlagsStruct::Kind>(1ull << Bit);

                        std::print(OutFile,
                                   "\t{}{}. Bit {}: {}\n",
                                   Prefix,
                                   Counter + 1,
                                   Bit,
                                   FlagsStruct::KindIsValid(Flag) ?
                                       FlagsStruct::KindGetString(Flag).data() :
                                       "<unknown>");

                        Counter++;
                    }
                }

                std::print(OutFile,
                           "{}Section Count: {}\n",
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
                                   "\t{}{:>{}}. File: {:<}\tMem: {:<}"
                                   "\tAlign: {:>{}}",
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

                        std::print(OutFile,
                                   "\n"
                                   "\t\t{}File:              {}\n"
                                   "\t\t{}Mem:               {}\n"
                                   "\t\t{}Size:              {}\n"
                                   "\t\t{}Align:             {} ({})\n"
                                   "\t\t{}Reloc File Offset: {}\n"
                                   "\t\t{}Reloc Count:       {}\n"
                                   "\t\t{}Reserved 1:        {}\n"
                                   "\t\t{}Reserved 2:        {}\n"
                                   "\t\t{}Flags:             0x{:x}\n",
                                   Prefix,
                                    ADT::Range::FromSize(FileOffset, Size),
                                   Prefix, ADT::Range::FromSize(Addr, Size),
                                   Prefix, Utils::ByteSize(Size),
                                   Prefix, Align, AlignDesc,
                                   Prefix,
                                       Utils::Address(
                                           Section.relocFileOffset(IsBigEndian)),
                                   Prefix,
                                       Utils::NumberWithCommas(
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

                                std::print(OutFile,
                                           "\t\t\t{}{}. Bit {}: {}\n",
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

                                std::print(OutFile, " {}", AttrDesc.data());
                            } else {
                                std::print(OutFile, " <unknown: Bit {}>", Bit);
                            }

                            Iter++;
                            if (Iter == Iterator.end()) {
                                break;
                            }

                            std::print(OutFile, ",");
                        }

                        std::print(OutFile, ")\n");
                    } else {
                        std::print(OutFile, "\n");
                    }

                    I++;
                }

                break;
            }
            case LoadCommandKind::Thread:
            case LoadCommandKind::UnixThread:
                std::print(OutFile, "\n");
                break;
            case LoadCommandKind::LoadFixedVMSharedLib:
            case LoadCommandKind::IdFixedVMSharedLib: {
                const auto &FvmLib = cast<FvmLibraryCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmLib.name(IsBigEndian);
                const auto MinorVersion =
                    FvmLib.Library.minorVersion(IsBigEndian);
                const auto HeaderAddress =
                    FvmLib.Library.headerAddress(IsBigEndian);

                std::print(OutFile,
                           "\n"
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
                    const auto PadLength =
                        MaxDylibPathLength -
                            (NameOpt.has_value() ?
                                NameOpt->length() : Malformed.length());

                    Utils::PadSpaces(OutFile, static_cast<uint32_t>(PadLength));
                    DylibIndex++;
                }

                const auto CurrentVersion =
                    DylibCmd.currentVersion(IsBigEndian);
                const auto CompatVersion = DylibCmd.compatVersion(IsBigEndian);

                const auto Timestamp = DylibCmd.timestamp(IsBigEndian);
                const auto TimestampString =
                    Utils::GetHumanReadableTimestamp(Timestamp);

                std::print(OutFile,
                           "\n"
                           "{}Current Version: {}\n"
                           "{}Compat Version:  {}\n"
                           "{}Timestamp:       {} (Value: {})\n",
                           Prefix, CurrentVersion,
                           Prefix, CompatVersion,
                           Prefix, TimestampString, Timestamp);

                break;
            }
            case LoadCommandKind::SubFramework: {
                const auto &SubFramework =
                    cast<SubFrameworkCommand>(LC, IsBigEndian);

                const auto UmbrellaOpt = SubFramework.umbrella(IsBigEndian);
                std::print(OutFile,
                           "\t\"{}\"\n",
                            UmbrellaOpt.has_value() ?
                                UmbrellaOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::SubClient: {
                const auto &SubClient = cast<SubClientCommand>(LC, IsBigEndian);
                const auto ClientOpt = SubClient.client(IsBigEndian);

                std::print(OutFile,
                           "\t\"{}\"\n",
                            ClientOpt.has_value() ?
                                ClientOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::SubUmbrella: {
                const auto &SubUmbrella =
                    cast<SubUmbrellaCommand>(LC, IsBigEndian);
                const auto SubUmbrellaOpt =
                    SubUmbrella.subUmbrella(IsBigEndian);

                std::print(OutFile,
                           "\t\"{}\"\n",
                           SubUmbrellaOpt.has_value() ?
                                SubUmbrellaOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::SubLibrary: {
                const auto &SubLibrary =
                    cast<SubLibraryCommand>(LC, IsBigEndian);

                const auto SubLibraryOpt = SubLibrary.subLibrary(IsBigEndian);
                std::print(OutFile,
                           "\t\"{}\"\n",
                            SubLibraryOpt.has_value() ?
                                SubLibraryOpt.value() : Malformed);

                break;
            }
            case MachO::LoadCommandKind::PreBoundDylib: {
                const auto &PreboundDylibCmd =
                    cast<PreboundDylibCommand>(LC, IsBigEndian);

                const auto NameOpt = PreboundDylibCmd.name(IsBigEndian);
                std::print(OutFile,
                           "\t\"{}\"\n",
                           NameOpt.has_value() ? NameOpt.value() : Malformed);

                break;
            }
            case LoadCommandKind::IdDylinker:
            case LoadCommandKind::LoadDylinker: {
                const auto &DylinkerCmd =
                    cast<DylinkerCommand>(LC, IsBigEndian);

                const auto NameOpt = DylinkerCmd.name(IsBigEndian);
                std::print(OutFile,
                           "\t\"{}\"\n",
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

                std::print(OutFile,
                           "\n"
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

                std::print(OutFile,
                           "\n"
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

                std::print(OutFile,
                           "\n"
                           "{}Symbol Table Offset: {}\n"
                           "{}Symbol Count:        {}\n"
                           "{}String Table Offset: ({})\n"
                           "{}String Table Size:   {}\n",
                           Prefix, SymOff,
                           Prefix, SymCount,
                           Prefix, StrOff,
                           ADT::Range::FromSize(StrOff, StrSize),
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
                    DySymTabCmd.indirectSymbolsOffset(IsBigEndian);
                const auto ExternRelOffset =
                    DySymTabCmd.externRelOffset(IsBigEndian);
                const auto ExternRelCount =
                    DySymTabCmd.externRelCount(IsBigEndian);
                const auto LocalRelOffset =
                    DySymTabCmd.localRelOffset(IsBigEndian);
                const auto LocalRelCount =
                    DySymTabCmd.localRelCount(IsBigEndian);

                std::print(OutFile,
                           "\n"
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
                           Prefix, Utils::NumberWithCommas(LocalSymbolsIndex),
                           Prefix, Utils::NumberWithCommas(LocalSymbolsCount),
                           Prefix,
                            Utils::NumberWithCommas(ExternDefSymbolsIndex),
                           Prefix,
                            Utils::NumberWithCommas(ExternDefSymbolsCount),
                           Prefix, Utils::NumberWithCommas(UndefSymbolsIndex),
                           Prefix, Utils::NumberWithCommas(UndefSymbolsCount),
                           Prefix, Utils::Address(TableOfContentsOffset),
                           Prefix,
                            Utils::NumberWithCommas(TableOfContentsCount),
                           Prefix, Utils::Address(ModulesTabOffset),
                           Prefix, Utils::NumberWithCommas(ModulesTabCount),
                           Prefix, Utils::Address(ExternRefSymbolsOffset),
                           Prefix,
                            Utils::NumberWithCommas(ExternRefSymbolsCount),
                           Prefix, Utils::Address(IndirectSymbolsOffset),
                           Prefix,
                            Utils::NumberWithCommas(IndirectSymbolsCount),
                           Prefix, Utils::Address(ExternRelOffset),
                           Prefix, Utils::NumberWithCommas(ExternRelCount),
                           Prefix, Utils::Address(LocalRelOffset),
                           Prefix, Utils::NumberWithCommas(LocalRelCount));
                break;
            }
            case LoadCommandKind::TwoLevelHints: {
                const auto &TwoLevelHintsCmd =
                    cast<TwoLevelHintsCommand>(LC, IsBigEndian);

                const auto Offset = TwoLevelHintsCmd.offset(IsBigEndian);
                const auto HintsCount =
                    TwoLevelHintsCmd.hintsCount(IsBigEndian);

                std::print(OutFile,
                           "\n"
                           "{}Offset:      {}\n"
                           "{}Hints Count: {}\n",
                           Prefix, Utils::Address(Offset),
                           Prefix, Utils::NumberWithCommas(HintsCount));
                break;
            }
            case LoadCommandKind::PreBindChecksum: {
                const auto PrebindChecksumCmd =
                    cast<PrebindChecksumCommand>(LC, IsBigEndian);

                const auto Checksum = PrebindChecksumCmd.checksum(IsBigEndian);
                std::print(OutFile, "\t{}\n", Checksum);

                break;
            }
            case LoadCommandKind::Uuid: {
                const auto UuidCmd = cast<UuidCommand>(LC, IsBigEndian);
                std::print(OutFile,
                           "\t\"{}\"\n",
                           Utils::Uuid(UuidCmd.Uuid));
                break;
            }
            case LoadCommandKind::Rpath: {
                const auto &RpathCmd = cast<RpathCommand>(LC, IsBigEndian);
                const auto PathOpt = RpathCmd.path(IsBigEndian);

                std::print(OutFile,
                           "\t\"{}\"\n",
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

                std::print(OutFile,
                           "\n"
                           "{}Data Offset: {} ({})\n"
                           "{}Data Size:   {}\n",
                           Prefix, Utils::Address(DataOff),
                            ADT::Range::FromSize(DataOff, DataSize),
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

                std::print(OutFile,
                           "\n"
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

                std::print(OutFile,
                           "\n"
                           "{}Crypt Offset: {} ({})\n"
                           "{}Crypt Size:   {}\n"
                           "{}Crypt Id:     {}\n",
                           Prefix, CryptOffset,
                           ADT::Range::FromSize(CryptOffset, CryptSize),
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

                std::print(OutFile,
                           "\n"
                           "{}Crypt Offset: {} ({})\n"
                           "{}Crypt Size:   {}\n"
                           "{}Crypt Id:     {}\n"
                           "{}Pad:          {}\n",
                           Prefix, CryptOffset,
                            ADT::Range::FromSize(CryptOffset, CryptSize),
                           Prefix, Utils::ByteSize(CryptSize),
                           Prefix, CryptId,
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

                std::print(OutFile,
                           "\n"
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

                    std::print(OutFile,
                               "\n"
                               "{}Platform:    {}\n",
                               Prefix,
                               PlatformString);
                } else {
                    std::print(OutFile,
                               "\n"
                               "{}Platform:    <Unknown> (Value: {}\n",
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

                std::print(OutFile,
                           "\n"
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
                            ADT::Range::FromSize(RebaseOffset, RebaseSize),
                           Prefix,
                               Utils::ByteSize(RebaseSize),
                           Prefix, Utils::Address(BindOffset),
                            ADT::Range::FromSize(BindOffset, BindSize),
                           Prefix,
                               Utils::ByteSize(BindSize),
                           Prefix, Utils::Address(WeakBindOffset),
                            ADT::Range::FromSize(WeakBindOffset, WeakBindSize),
                           Prefix,
                               Utils::ByteSize(WeakBindSize),
                           Prefix, Utils::Address(LazyBindOffset),
                            ADT::Range::FromSize(LazyBindOffset, LazyBindSize),
                           Prefix,
                               Utils::ByteSize(LazyBindSize),
                           Prefix, Utils::Address(ExportTrieOffset),
                            ADT::Range::FromSize(ExportTrieOffset,
                                                 ExportTrieSize),
                           Prefix, Utils::ByteSize(ExportTrieSize));
                break;
            }
            case LoadCommandKind::LinkerOption: {
                const auto &LinkerOptionCmd =
                    cast<LinkerOptionCommand>(LC, IsBigEndian);

                const auto Count = LinkerOptionCmd.count(IsBigEndian);
                std::print(OutFile, "\n{}Count: {}\n", Prefix, Count);

                break;
            }
            case LoadCommandKind::SymbolSegment: {
                const auto &SymbolSegmentCmd =
                    cast<SymbolSegmentCommand>(LC, IsBigEndian);

                const auto Offset = SymbolSegmentCmd.offset(IsBigEndian);
                const auto Size = SymbolSegmentCmd.size(IsBigEndian);

                std::print(OutFile,
                           "\n"
                           "{}Offset: {} ({})\n"
                           "{}Size:   {}\n",
                           Prefix, Offset, ADT::Range::FromSize(Offset, Size),
                           Prefix, Utils::ByteSize(Size));
                break;
            }
            case LoadCommandKind::FixedVMFile: {
                const auto &FvmFileCmd = cast<FvmFileCommand>(LC, IsBigEndian);

                const auto NameOpt = FvmFileCmd.name(IsBigEndian);
                const auto HeaderAddress =
                    FvmFileCmd.headerAddress(IsBigEndian);

                std::print(OutFile,
                           "\n"
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

                std::print(OutFile,
                           "\n"
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
                std::print(OutFile, "\t{}\n", Version);

                break;
            }
            case LoadCommandKind::PrePage:
                break;
            case LoadCommandKind::Note: {
                const auto &NoteCmd = cast<NoteCommand>(LC, IsBigEndian);
                const auto DataOwner = NoteCmd.dataOwner();
                const auto Offset = NoteCmd.offset(IsBigEndian);
                const auto Size = NoteCmd.size(IsBigEndian);

                std::print(OutFile,
                           "\n"
                           "{}Data Owner: \"{}\"\n"
                           "{}Offset: {} ({})\n"
                           "{}Size:   {}\n",
                           Prefix, DataOwner,
                           Prefix, Offset, ADT::Range::FromSize(Offset, Size),
                           Prefix, Utils::ByteSize(Size));

                break;
            }
        }
    }

    auto
    PrintLoadCommands::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        auto Counter = static_cast<uint32_t>(1);
        auto DylibIndex = uint32_t();

        const auto IsBigEndian = MachO.isBigEndian();
        const auto LongestLCKindLength =
            MachO::LoadCommandKindGetString(
                MachO::LoadCommandKind::LinkerOptimizationHint).length();

        auto MaxDylibPathLength = ADT::Maximizer<uint32_t>();
        for (const auto &LC : MachO.loadCommandsMap()) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                const auto &DylibCmd =
                    MachO::cast<MachO::DylibCommand>(LC, IsBigEndian);

                if (const auto NameOpt = DylibCmd.name(IsBigEndian)) {
                    MaxDylibPathLength.set(
                        static_cast<uint32_t>(NameOpt->length()));
                }
            }
        }

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
                             DylibIndex,
                             MaxDylibPathLength.value(),
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

