//
//  Operations/PrintHeader.cpp
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#include <cstdio>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "ADT/FlagsIterator.h"
#include "Objects/DyldSharedCache.h"

#include "Operations/Base.h"
#include "Operations/PrintArchs.h"
#include "Operations/PrintHeader.h"

#include "Utils/Print.h"

namespace Operations {
    PrintHeader::PrintHeader(FILE *const OutFile,
                             const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintHeader::supportsObjectKind(const Objects::Kind Kind) const noexcept {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintHeader::supportsObjectKind()");
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::DscImage:
            case Objects::Kind::MachO:
            case Objects::Kind::FatMachO:
                return true;
        }

        assert(false &&
               "Got unknown Object-Kind in PrintHeader::supportsObjectKind()");
    }

    auto
    PrintHeader::run(const Objects::MachO &MachO) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::MachO);

        const auto Header = MachO.header();
        const auto CpuKind = Header.cpuKind();
        const auto SubKind = Header.cpuSubKind();
        const auto FileKind = Header.fileKind();
        const auto Ncmds = Header.ncmds();
        const auto SizeOfCmds = Header.sizeOfCmds();
        const auto Flags = Header.flags();

        const auto SubKindString =
            Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind) ?
                Opt.Verbose ?
                    Mach::CpuKindAndSubKindGetString(CpuKind, SubKind).data() :
                    Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind).data()
                : "Unrecognized";

        fprintf(OutFile,
                "Apple %s Mach-O File\n"
                "\tMagic:      %s\n"
                "\tCputype:    %s\n"
                "\tCpuSubtype: %s\n"
                "\tFiletype:   %s\n"
                "\tNcmds:      %" PRIu32 "\n"
                "\tSizeOfCmds: %" PRIu32 "\n"
                "\tFlags:      0x%" PRIx32 "\n",
                MachO.is64Bit() ? "64-Bit" : "32-Bit",
                Opt.Verbose ?
                    MachO::MagicGetString(Header.Magic).data() :
                    MachO::MagicGetDesc(Header.Magic).data(),
                Mach::CpuKindIsValid(CpuKind) ?
                    Opt.Verbose ?
                        Mach::CpuKindGetString(CpuKind).data() :
                        Mach::CpuKindGetDesc(CpuKind).data()
                    : "Unrecognized",
                SubKindString,
                MachO::FileKindIsValid(FileKind) ?
                    Opt.Verbose ?
                        MachO::FileKindGetString(FileKind).data() :
                        MachO::FileKindGetDesc(FileKind).data()
                    : "Unrecognized",
                Ncmds,
                SizeOfCmds,
                Flags.value());

        auto Counter = static_cast<uint8_t>(1);
        for (const auto Bit : ADT::FlagsIterator(Flags)) {
            const auto Flag = MachO::Flags::Kind(1 << Bit);
            fprintf(OutFile,
                    "\t\t %" ZEROPAD_FMT_C(2) PRIu8 ". Bit %" PRIu32 ": %s\n",
                    Counter,
                    Bit,
                    MachO::Flags::KindIsValid(Flag) ?
                        MachO::Flags::KindGetString(Flag).data() :
                        "<unknown>");

            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto
    PrintHeader::run(const Objects::FatMachO &Fat) const noexcept -> RunResult {
        auto Result = RunResult(Objects::Kind::FatMachO);

        const auto Header = Fat.header();
        const auto ArchCount = Header.archCount();

        fprintf(OutFile,
                "Apple %s Fat Mach-O File\n"
                "\tMagic: %s\n"
                "\tArch Count: %" PRIu32 "\n",
                Fat.is64Bit() ? "64-Bit" : "32-Bit",
                Opt.Verbose ?
                    MachO::MagicGetString(Header.Magic).data() :
                    MachO::MagicGetDesc(Header.Magic).data(),
                ArchCount);

        if (ArchCount < 6) {
            auto I = uint32_t();
            const auto IsBigEndian = Fat.isBigEndian();

            if (Fat.is64Bit()) {
                for (const auto &Arch : Fat.arch64List()) {
                    const auto Object =
                        std::unique_ptr<Objects::Base>(
                            Fat.getArchObjectAtIndex(I).ptr());

                    Operations::PrintArchs::PrintArch64(OutFile,
                                                        Arch,
                                                        Object.get(),
                                                        I + 1,
                                                        Opt.Verbose,
                                                        IsBigEndian,
                                                        "\t\t");
                    I++;
                }
            } else {
                for (const auto &Arch : Fat.archList()) {
                    const auto Object =
                        std::unique_ptr<Objects::Base>(
                            Fat.getArchObjectAtIndex(I).ptr());

                    Operations::PrintArchs::PrintArch(OutFile,
                                                      Arch,
                                                      Object.get(),
                                                      I + 1,
                                                      Opt.Verbose,
                                                      IsBigEndian,
                                                      "\t\t");
                    I++;
                }
            }
        }

        return Result.set(RunError::None);
    }

    enum class DscRangeKind {
        None,

        // HeaderV0
        MappingInfoList,
        ImageInfoList,

        // HeaderV1
        CodeSignature,
        SlideInfo,

        // HeaderV2
        LocalSymbolInfo,

        // HeaderV4
        ImageTextInfoList,

        // HeaderV5
        DylibsImageGroup,
        OtherImageGroup,
        ProgClosures,
        ProgClosuresTrie,

        // HeaderV6
        DylibsImageArray,
        DylibsTrie,
        OtherImageArray,
        OtherTrie,

        // HeaderV7

        // HeaderV8
        ProgramsPBLSetPool,
        ProgramTrie,
        SwiftOpts,
        RosettaReadOnly,
        RosettaReadWrite,

        // HeaderV9
        ObjcOpts,
        CacheAtlas,
        DynamicDataMax,

        End,
    };

    struct DscRange {
        DscRangeKind Kind;
        ADT::Range Range;
        std::bitset<static_cast<uint32_t>(DscRangeKind::End) - 1>
            OverlapKindSet;
    };

    using DscRangeList = std::unordered_map<DscRangeKind, DscRange>;
    static
    void AddRangeToList(DscRangeList &List, DscRange &&DscRange) noexcept {
        for (const auto &Iter : List) {
            const auto &IterKind = Iter.second.Kind;
            if (DscRange.Kind < IterKind) {
                continue;
            }

            if (DscRange.Range.overlaps(Iter.second.Range)) {
                DscRange.OverlapKindSet.set(
                    static_cast<uint32_t>(IterKind) - 1);
            }
        };

        List.insert({ DscRange.Kind, std::move(DscRange) });
    }

    [[nodiscard]] static DscRangeList
    CollectDscRangeList(const DyldSharedCache::Header &Header,
                        const DyldSharedCache::HeaderVersion &Version) noexcept
    {
        auto Kind = DscRangeKind::None;
        auto Result = DscRangeList();

        switch (Kind) {
            case DscRangeKind::None:
            case DscRangeKind::MappingInfoList: {
                if (const auto Range = Header.getMappingInfoListRange()) {
                    AddRangeToList(Result, DscRange {
                        .Kind = DscRangeKind::MappingInfoList,
                        .Range = Range.value(),
                        .OverlapKindSet = {}
                    });
                }

                [[fallthrough]];
            }

            case DscRangeKind::ImageInfoList: {
                if (const auto Range = Header.getImageInfoListRange()) {
                    AddRangeToList(Result, DscRange {
                        .Kind = DscRangeKind::ImageInfoList,
                        .Range = Range.value(),
                        .OverlapKindSet = {}
                    });
                }
            }

            if (Version < DyldSharedCache::HeaderVersion::V1) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::CodeSignature:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::CodeSignature,
                    .Range = Header.codeSignatureRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::SlideInfo:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::SlideInfo,
                    .Range = Header.slideInfoRange(),
                    .OverlapKindSet = {}
                });

            if (Version < DyldSharedCache::HeaderVersion::V2) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::LocalSymbolInfo:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::LocalSymbolInfo,
                    .Range = Header.localSymbolInfoRange(),
                    .OverlapKindSet = {}
                });

            if (Version < DyldSharedCache::HeaderVersion::V4) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::ImageTextInfoList: {
                if (const auto Range = Header.getImageTextInfoListRange()) {
                    AddRangeToList(Result, DscRange {
                        .Kind = DscRangeKind::ImageTextInfoList,
                        .Range = Range.value(),
                        .OverlapKindSet = {}
                    });
                }
            }

            if (Version < DyldSharedCache::HeaderVersion::V5) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::DylibsImageGroup:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsImageGroup,
                    .Range = Header.dylibsImageGroupRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::OtherImageGroup:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherImageGroup,
                    .Range = Header.otherImageGroupRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::ProgClosures:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgClosures,
                    .Range = Header.progClosuresRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];

            case DscRangeKind::ProgClosuresTrie:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgClosuresTrie,
                    .Range = Header.progClosuresTrieRange(),
                    .OverlapKindSet = {}
                });

            if (Version < DyldSharedCache::HeaderVersion::V6) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::DylibsImageArray:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsImageArray,
                    .Range = Header.dylibsImageArrayRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::DylibsTrie:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DylibsTrie,
                    .Range = Header.dylibsTrieRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::OtherImageArray:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherImageArray,
                    .Range = Header.otherImageArrayRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];

            case DscRangeKind::OtherTrie:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::OtherTrie,
                    .Range = Header.otherTrieRange(),
                    .OverlapKindSet = {}
                });

            // Nothing in V7
            if (Version < DyldSharedCache::HeaderVersion::V8) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::ProgramsPBLSetPool:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgramsPBLSetPool,
                    .Range = Header.programsPBLSetPoolRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];

            case DscRangeKind::ProgramTrie:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ProgramTrie,
                    .Range = Header.programTrieRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::SwiftOpts:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::SwiftOpts,
                    .Range = Header.swiftOptsRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::RosettaReadOnly:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::RosettaReadOnly,
                    .Range = Header.rosettaReadOnlyRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::RosettaReadWrite:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::RosettaReadWrite,
                    .Range = Header.rosettaReadWriteRange(),
                    .OverlapKindSet = {}
                });

            if (Version < DyldSharedCache::HeaderVersion::V9) {
                return Result;
            }

            [[fallthrough]];
            case DscRangeKind::ObjcOpts:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::ObjcOpts,
                    .Range = Header.objcOptsRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::CacheAtlas:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::CacheAtlas,
                    .Range = Header.cacheAtlasRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::DynamicDataMax:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::DynamicDataMax,
                    .Range = Header.dynamicDataMaxRange(),
                    .OverlapKindSet = {}
                });

                [[fallthrough]];
            case DscRangeKind::End:
                break;
        }

        return Result;
    }

    static void
    PrintDscRangeOverlapsErrorOrNewline(
        FILE *const OutFile,
        const struct PrintHeader::Options &Options,
        const DscRangeList &List,
        const DscRangeKind Kind) noexcept
    {
        if (Options.Verbose) {
            fputc('\n', OutFile);
            return;
        }

        const auto End = List.cend();
        const auto Iter = List.find(Kind);

        if (Iter == End) {
            fputc('\n', OutFile);
            return;
        }

        const auto &OverlapKindSet = Iter->second.OverlapKindSet;
        if (OverlapKindSet.none()) {
            fputc('\n', OutFile);
            return;
        }

        fputs(" (Overlaps with ", OutFile);

        auto Count = uint8_t();
        auto OverlapKindSetCount = OverlapKindSet.count();

        for (auto KindInt = static_cast<uint32_t>(DscRangeKind::None) + 1;;
             KindInt++)
        {
            const auto Kind = static_cast<DscRangeKind>(KindInt);
            if (!OverlapKindSet[KindInt - 1]) {
                continue;
            }

            switch (Kind) {
                case DscRangeKind::None:
                    assert(0 && "Overlap-Kind None is set");
                case DscRangeKind::MappingInfoList:
                    fputs("Mapping-Info Range", OutFile);
                    break;
                case DscRangeKind::ImageInfoList:
                    fputs("Image-Info Range", OutFile);
                    break;
                case DscRangeKind::CodeSignature:
                    fputs("Code-Signature Range", OutFile);
                    break;
                case DscRangeKind::SlideInfo:
                    fputs("Slide-Info Range", OutFile);
                    break;
                case DscRangeKind::LocalSymbolInfo:
                    fputs("Local-Symbol Info Range", OutFile);
                    break;
                case DscRangeKind::ImageTextInfoList:
                    fputs("Image-Text Info Range", OutFile);
                    break;
                case DscRangeKind::DylibsImageGroup:
                    fputs("Dylibs Image-Group Range", OutFile);
                    break;
                case DscRangeKind::OtherImageGroup:
                    fputs("Other-Image Group Range", OutFile);
                    break;
                case DscRangeKind::ProgClosures:
                    fputs("Prog-Closures Range", OutFile);
                    break;
                case DscRangeKind::ProgClosuresTrie:
                    fputs("Prog-Closures Trie Range", OutFile);
                    break;
                case DscRangeKind::DylibsImageArray:
                    fputs("Dylibs-Image Range", OutFile);
                    break;
                case DscRangeKind::DylibsTrie:
                    fputs("Dylibs-Trie Range", OutFile);
                    break;
                case DscRangeKind::OtherImageArray:
                    fputs("Other-Image Array Range", OutFile);
                    break;
                case DscRangeKind::OtherTrie:
                    fputs("Other-Trie Range", OutFile);
                    break;
                case DscRangeKind::ProgramsPBLSetPool:
                    fputs("Programs Prebuilt Loader Set Pool Range",
                        OutFile);
                    break;
                case DscRangeKind::ProgramTrie:
                    fputs("Program Trie Range", OutFile);
                    break;
                case DscRangeKind::SwiftOpts:
                    fputs("Swift Optimizations Range", OutFile);
                    break;
                case DscRangeKind::RosettaReadOnly:
                    fputs("Rosetta Read Only Range", OutFile);
                    break;
                case DscRangeKind::RosettaReadWrite:
                    fputs("Rosetta Read Write Range", OutFile);
                    break;
                case DscRangeKind::ObjcOpts:
                    fputs("Objc Opts Range", OutFile);
                    break;
                case DscRangeKind::CacheAtlas:
                    fputs("Cache Atlas Range", OutFile);
                    break;
                case DscRangeKind::DynamicDataMax:
                    fputs("Dynamic Data Max", OutFile);
                    break;
                case DscRangeKind::End:
                    assert(0 && "Overlap-Kind End is set");
            }

            Count++;
            if (Count == OverlapKindSetCount) {
                fputs(")\n", OutFile);
                break;
            } else {
                fputs(", ", OutFile);
            }
        }
    }

    constexpr static auto LongestDscKey =
        STR_LENGTH("Programs Prebuilt Loader Set Pool Address");

    static void
    PrintDscKey(FILE *const OutFile,
                const std::string_view Key,
                const std::string_view Prefix = "") noexcept
    {
        fprintf(OutFile, STRING_VIEW_FMT, STRING_VIEW_FMT_ARGS(Prefix));
        fflush(OutFile);

        Utils::RightPadSpaces(OutFile,
                              fprintf(OutFile,
                                      STRING_VIEW_FMT ": ",
                                      STRING_VIEW_FMT_ARGS(Key)),
                              LongestDscKey + STR_LENGTH(": "));
    }

    static void
    WarnIfOutOfRange(FILE *const OutFile,
                     const ADT::Range &DscRange,
                     const uint64_t Offset,
                     const uint64_t Size,
                     const bool PrintNewLine = true) noexcept
    {
        const auto Range = ADT::Range::FromSize(Offset, Size);
        if (!DscRange.contains(Range)) {
            fputs(" (Past EOF!)", OutFile);
        }

        if (PrintNewLine) {
            fputc('\n', OutFile);
        }
    }

    template <
        void (*PrintKeyFunc)(FILE *, std::string_view, std::string_view) =
            PrintDscKey,
        std::unsigned_integral AddrType,
        std::unsigned_integral SizeType>

    static inline void
    PrintDscSizeRange(FILE *const OutFile,
                      const ADT::Range &DscRange,
                      const char *const AddressName,
                      const char *const SizeName,
                      const AddrType Address,
                      const SizeType Size,
                      const bool Verbose,
                      const std::string_view Prefix = "",
                      const std::string_view Suffix = "",
                      const bool IsOffset = false,
                      const bool PrintNewLine = false) noexcept
    {
        PrintKeyFunc(OutFile, AddressName, Prefix);
        Utils::PrintAddress(OutFile,
                            Address,
                            std::is_same_v<AddrType, uint64_t>);

        constexpr auto SizeIs64Bit = std::is_same_v<SizeType, uint64_t>;
        if (Verbose && Size != 0) {
            Utils::PrintOffsetSizeRange(OutFile,
                                        ADT::Range::FromSize(Address, Size),
                                        std::is_same_v<AddrType, uint64_t>,
                                        SizeIs64Bit,
                                        /*Prefix=*/" (",
                                        /*Suffix=*/")");
        }

        if (IsOffset) {
            WarnIfOutOfRange(OutFile,
                             DscRange,
                             Address,
                             Size,
                             /*PrintNewLine=*/false);
        }

        fprintf(OutFile, STRING_VIEW_FMT "\n", STRING_VIEW_FMT_ARGS(Suffix));
        PrintKeyFunc(OutFile, SizeName, Prefix);
        fprintf(OutFile, "%s", Utils::FormattedSizeForOutput(Size).c_str());

        if (PrintNewLine) {
            fputc('\n', OutFile);
        }
    }

    static void
    PrintMappingInfoList(FILE *const OutFile,
                         const struct Operations::PrintHeader::Options &Options,
                         const Objects::DyldSharedCache &Dsc)
    {
        const auto MappingCountDigitLength =
            Utils::GetIntegerDigitCount(Dsc.mappingCount());

        constexpr auto LongestKeyLength = STR_LENGTH("File-Offset: ");
        auto Index = uint32_t(1);

        for (const auto &Mapping : Dsc.mappingInfoList()) {
            const auto InitProt = Mapping.initProt();
            const auto MaxProt = Mapping.maxProt();

            fprintf(OutFile,
                    "\tMapping %" ZEROPAD_FMT PRIu32 ": "
                    MACH_VMPROT_INIT_MAX_FMT " \n",
                    ZEROPAD_FMT_ARGS(MappingCountDigitLength),
                    Index,
                    MACH_VMPROT_INIT_MAX_FMT_ARGS(InitProt, MaxProt));

            Utils::RightPadSpaces(OutFile,
                                  fputs("\t\tFile-Offset: ", OutFile),
                                  STR_LENGTH("\t\t") + LongestKeyLength);

            Utils::PrintAddress(OutFile,
                                Mapping.FileOffset,
                                /*Is64Bit=*/true,
                                /*Prefix=*/"");

            const auto PrintRange = !Mapping.empty() && Options.Verbose;
            if (PrintRange) {
                Utils::PrintOffsetSizeRange(OutFile,
                                            Mapping.fileRange(),
                                            /*Is64Bit=*/true,
                                            /*SizeIs64Bit=*/true,
                                            " (",
                                            ")");
            }

            Utils::RightPadSpaces(OutFile,
                                  fputs("\n\t\tAddress: ", OutFile),
                                  STR_LENGTH("\n\t\t") + LongestKeyLength);

            Utils::PrintAddress(OutFile,
                                Mapping.Address,
                                /*Pad=*/true,
                                /*Prefix=*/"");

            if (PrintRange) {
                Utils::PrintOffsetSizeRange(OutFile,
                                            Mapping.addressRange(),
                                            /*Is64Bit=*/true,
                                            /*SizeIs64Bit=*/true,
                                            " (",
                                            ")");
            }

            fprintf(OutFile,
                    "\n\t\tSize:        %s\n",
                    Utils::FormattedSizeForOutput(Mapping.Size).c_str()),

            Index++;
        }
    }

    static void
    PrintSubCacheEntryInfo(FILE *const OutFile,
                           const Objects::DyldSharedCache &Dsc)
    {
        const auto MappingCountDigitLength =
            Utils::GetIntegerDigitCount(Dsc.mappingCount());

        constexpr auto LongestKeyLength = STR_LENGTH("Cache Vm-Offset: ");
        auto Index = uint32_t(1);

        if (const auto ListOpt = Dsc.subCacheEntryInfoList()) {
            for (const auto &Info : ListOpt.value()) {
                fprintf(OutFile,
                        "\tSub-Cache Entry %" ZEROPAD_FMT PRIu32 ": "
                        UUID_FMT " \n",
                        ZEROPAD_FMT_ARGS(MappingCountDigitLength),
                        Index,
                        UUID_FMT_ARGS(Info.Uuid));

                Utils::RightPadSpaces(OutFile,
                                      fputs("\t\tCache Vm-Offset: ", OutFile),
                                      STR_LENGTH("\t\t") + LongestKeyLength);

                Utils::PrintAddress(OutFile,
                                    Info.CacheVMOffset,
                                    /*Is64Bit=*/true,
                                    /*Prefix=*/"",
                                    /*Suffix=*/"\n");

                Utils::RightPadSpaces(OutFile,
                                      fputs("\t\tFile-Suffix: ", OutFile),
                                      STR_LENGTH("\t\t") + LongestKeyLength);

                fprintf(OutFile,
                        "\"" CHAR_ARR_FMT(32) "\"\n",
                        CHAR_ARR_FMT_ARGS(Info.FileSuffix));

                Index++;
            }

            return;
        }

        const auto ListOpt = Dsc.subCacheEntryV1InfoList();
        if (!ListOpt.has_value()) {
            return;
        }

        for (const auto &Info : ListOpt.value()) {
            fprintf(OutFile,
                    "\tSub-Cache V1 Entry %" ZEROPAD_FMT PRIu32 ": "
                    UUID_FMT " \n",
                    ZEROPAD_FMT_ARGS(MappingCountDigitLength),
                    Index,
                    UUID_FMT_ARGS(Info.Uuid));

            Utils::RightPadSpaces(OutFile,
                                  fputs("\t\tCache Vm-Offset: ", OutFile),
                                  STR_LENGTH("\t\t") + LongestKeyLength);

            Utils::PrintAddress(OutFile,
                                Info.CacheVMOffset,
                                /*Is64Bit=*/true,
                                /*Prefix=*/"",
                                /*Suffix=*/"\n");

            Index++;
        }
    }

    static void
    PrintDscHeaderV0Info(FILE *const OutFile,
                         const struct PrintHeader::Options &Options,
                         const Objects::DyldSharedCache &Dsc) noexcept
    {
        const auto Header = Dsc.headerV0();

        PrintDscKey(OutFile, "Magic");
        fprintf(OutFile, "\"" CHAR_ARR_FMT(16) "\"", Header.Magic);

        if (Options.Verbose) {
            const auto [CpuKind, CpuSubKind] = Dsc.getMachCpuKindAndSubKind();
            fprintf(OutFile,
                    " (Cpu-Kind: %s)\n",
                    Mach::CpuKindAndSubKindGetString(CpuKind,
                                                     CpuSubKind).data());
        } else {
            fputc('\n', OutFile);
        }

        const auto Version = Dsc.getVersion();

        PrintDscKey(OutFile, "Version");
        fprintf(OutFile, "%d\n", static_cast<int>(Version));

        PrintDscKey(OutFile, "Mapping Offset");
        Utils::PrintAddress(OutFile,
                            Header.MappingOffset,
                            false,
                            "",
                            "\n");

        PrintDscKey(OutFile, "Mapping Count");
        if (Options.Verbose) {
            fprintf(OutFile,
                    "%s\n",
                    Utils::GetFormattedNumber(Header.MappingCount).c_str());
        } else {
            fprintf(OutFile, "%" PRIu32 "\n", Header.MappingCount);
        }

        if (Dsc.mappingCount() <= 10) {
            fputs("Mappings:\n", OutFile);
            PrintMappingInfoList(OutFile, Options, Dsc);
        }

        PrintDscKey(OutFile, "Images Offset (Old)");
        Utils::PrintAddress(OutFile,
                            Header.ImagesOffsetOld,
                            false,
                            "",
                            "\n");

        PrintDscKey(OutFile, "Images Count (Old)");
        if (Options.Verbose) {
           fprintf(OutFile,
                   "%s\n",
                   Utils::GetFormattedNumber(Header.ImagesCountOld).c_str());
        } else {
            fprintf(OutFile, "%" PRIu32 "\n", Header.ImagesCountOld);
        }

        PrintDscKey(OutFile, "Dyld Base-Address");
        Utils::PrintAddress(OutFile,
                            Header.DyldBaseAddress,
                            /*Is64Bit=*/true,
                            "",
                            "\n");
    }

    static void
    PrintDscHeaderV1Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV1();
        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Code-Signature Offset",
                          "Code-Signature Size",
                          Header.CodeSignatureOffset,
                          Header.CodeSignatureSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::CodeSignature);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Slide-Info Offset",
                          "Slide-Info Size",
                          Header.SlideInfoOffset,
                          Header.SlideInfoSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::SlideInfo);
        fputc('\n', OutFile);
    }

    static void
    PrintDscHeaderV2Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV2();
        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Local-Symbols Offset",
                          "Local-Symbols Size",
                          Header.LocalSymbolsOffset,
                          Header.LocalSymbolsSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::LocalSymbolInfo);

        PrintDscKey(OutFile, "Uuid");
        Utils::PrintUuid(OutFile, Header.Uuid, "", "\n\n");
    }

    template <std::unsigned_integral OffsetType,
              std::unsigned_integral SizeType>

    static void
    PrintOffsetCountPair(FILE *const OutFile,
                         const ADT::Range &Range,
                         const char *const OffsetKey,
                         const char *const CountKey,
                         const OffsetType Offset,
                         const SizeType Count,
                         const std::string_view Suffix = "")
    {
        PrintDscKey(OutFile, OffsetKey);
        Utils::PrintAddress(OutFile,
                            Offset,
                            std::is_same_v<OffsetType, uint64_t>);

        WarnIfOutOfRange(OutFile, Range, Offset, 1);

        PrintDscKey(OutFile, CountKey);
        fprintf(OutFile,
                "%s" STRING_VIEW_FMT,
                Utils::GetFormattedNumber(Count).c_str(),
                STRING_VIEW_FMT_ARGS(Suffix));
    }

    static void
    PrintCacheKind(FILE *const OutFile,
                   const std::string_view Key,
                   const DyldSharedCache::CacheKind CacheKind) noexcept
    {
        PrintDscKey(OutFile, Key);
        switch (CacheKind) {
            case DyldSharedCache::CacheKind::Development:
                fputs("Development\n", OutFile);
                return;
            case DyldSharedCache::CacheKind::Production:
                fputs("Production\n", OutFile);
                return;
        }

        fputs("<Unrecognized>\n", OutFile);
    }

    static void
    PrintDscHeaderV3Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Object) noexcept
    {
        const auto &Header = Object.headerV3();
        PrintCacheKind(OutFile, "Cache Kind", Header.Kind);
    }

    static void
    PrintDscHeaderV4Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV4();
        PrintOffsetCountPair(OutFile,
                             Dsc.range(),
                             "Branch-Pools Offset",
                             "Branch-Pools Count",
                             Header.BranchPoolsOffset,
                             Header.BranchPoolsCount,
                             /*Suffix=*/"\n");

        PrintOffsetCountPair(OutFile,
                             Dsc.range(),
                             "Images-Text Offset",
                             "Images-Text Count",
                             Header.ImagesTextOffset,
                             Header.ImagesTextCount);

        if (Header.ImagesTextCount != Dsc.imageCount()) {
            fprintf(OutFile,
                    " (Invalid: %" PRIu64 " Image-Texts vs %" PRIu32 " Images",
                    Header.ImagesTextCount,
                    Dsc.imageCount());
        }

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ImageTextInfoList);

        PrintDscKey(OutFile, "Dyld In-Cache Mach-Header Address");
        Utils::PrintAddress(OutFile,
                            Header.DyldInCacheMachHeaderAddr,
                            /*Is64Bit=*/true,
                            /*Prefix=*/"",
                            /*Suffix=*/"\n");

        PrintDscKey(OutFile, "Dyld In-Cache Entry-Point Address");
        Utils::PrintAddress(OutFile,
                            Header.DyldInCacheEntryPointAddr,
                            /*Is64Bit=*/true,
                            /*Prefix=*/"",
                            /*Suffix=*/"\n");
    }

    static inline void
    PrintBoolValue(FILE *const OutFile,
                   const char *const Key,
                   const bool Value) noexcept
    {
        PrintDscKey(OutFile, Key);

        fputs((Value) ? "true" : "false", OutFile);
        fputc('\n', OutFile);
    }

    static inline void
    PrintPlatformValue(FILE *const OutFile,
                       const char *const Key,
                       const Dyld3::Platform Platform)
    {
        PrintDscKey(OutFile, Key);
        fprintf(OutFile,
                "%s\n",
                Dyld3::PlatformGetDesc(Platform).data());
    }

    static void
    PrintDscHeaderV5Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV5();
        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Dylibs Image-Group Address",
                          "Dylibs Image-Group Size",
                          Header.DylibsImageGroupAddr,
                          Header.DylibsImageGroupSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::DylibsImageGroup);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Other Image-Group Address",
                          "Other Image-Group Size",
                          Header.OtherImageGroupAddr,
                          Header.OtherImageGroupSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::OtherImageGroup);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Program Closures Address",
                          "Program Closures Size",
                          Header.ProgClosuresAddr,
                          Header.ProgClosuresSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ProgClosures);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Program Closures Trie Address",
                          "Program Closures Trie Size",
                          Header.ProgClosuresTrieAddr,
                          Header.ProgClosuresTrieSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ProgClosuresTrie);

        PrintPlatformValue(OutFile, "Platform", Header.platform());

        PrintDscKey(OutFile, "Closure-Format Version");
        fprintf(OutFile, "%" PRIu32 "\n", Header.FormatVersion);

        PrintBoolValue(OutFile,
                       "Dylibs Expected On Disk",
                       Header.DylibsImageGroupSize);

        PrintBoolValue(OutFile, "Simulator", Header.Simulator);
        PrintBoolValue(OutFile,
                       "Locally-Built Cache",
                       Header.isLocallyBuiltCache() ? true : false);

        PrintBoolValue(OutFile,
                       "Built From Chained-Fixups",
                       Header.BuiltFromChainedFixups);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Shared-Region Start",
                          "Shared-Region Size",
                          Header.SharedRegionStart,
                          Header.SharedRegionSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/false,
                          /*PrintNewLine=*/true);

        PrintDscKey(OutFile, "Max Slide");
        if (Options.Verbose) {
            fprintf(OutFile,
                    "%s\n",
                    Utils::FormattedSizeForOutput(Header.MaxSlide).c_str());
        } else {
            fprintf(OutFile, "%s\n", std::to_string(Header.MaxSlide).c_str());
        }
    }

    static void
    PrintDscHeaderV6Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV6();
        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Dylibs Image-Array Address",
                          "Dylibs Image-Array Size",
                          Header.DylibsImageArrayAddr,
                          Header.DylibsImageArraySize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::DylibsImageGroup);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Dylibs Trie Address",
                          "Dylibs Trie Size",
                          Header.DylibsTrieAddr,
                          Header.DylibsTrieSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::DylibsTrie);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Other Image Array Address",
                          "Other Image Array Size",
                          Header.OtherImageArrayAddr,
                          Header.OtherImageArraySize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::OtherImageArray);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Other Trie Address",
                          "Other Trie Size",
                          Header.OtherTrieAddr,
                          Header.OtherTrieSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::OtherTrie);
    }

    static void
    PrintMappingWithSlideInfoList(FILE *const OutFile,
                                  const struct PrintHeader::Options &Options,
                                  const Objects::DyldSharedCache &Dsc)
    {
        const auto MappingCountDigitLength =
            Utils::GetIntegerDigitCount(Dsc.mappingCount());

        const auto LongestKeyLength = STR_LENGTH("Slide-Info File Offset: ");
        auto Index = static_cast<int>(1);

        const auto &Header = Dsc.headerV7();
        for (const auto &Mapping : Header.mappingWithSlideInfoList()) {
            const auto InitProt = Mapping.initProt();
            const auto MaxProt = Mapping.maxProt();

            fprintf(OutFile,
                    "\tMapping %" ZEROPAD_FMT PRIu32 ": "
                    MACH_VMPROT_INIT_MAX_FMT " \n",
                    ZEROPAD_FMT_ARGS(MappingCountDigitLength),
                    Index,
                    MACH_VMPROT_INIT_MAX_FMT_ARGS(InitProt, MaxProt));

            Utils::RightPadSpaces(OutFile,
                                  fputs("\t\tFile-Offset: ", OutFile),
                                  STR_LENGTH("\t\t") + LongestKeyLength);

            Utils::PrintAddress(OutFile,
                                Mapping.FileOffset,
                                /*Is64Bit=*/true,
                                /*Prefix=*/"");

            const auto PrintRange = !Mapping.empty() && Options.Verbose;
            if (PrintRange) {
                Utils::PrintOffsetSizeRange(OutFile,
                                            Mapping.fileRange(),
                                            /*Is64Bit=*/true,
                                            /*SizeIs64Bit=*/true,
                                            " (",
                                            ")");
            }

            Utils::RightPadSpaces(OutFile,
                                  fputs("\n\t\tAddress: ", OutFile),
                                  STR_LENGTH("\n\t\t") + LongestKeyLength);

            Utils::PrintAddress(OutFile,
                                Mapping.Address,
                                /*Is64Bit=*/true,
                                /*Prefix=*/"");

            if (PrintRange) {
                Utils::PrintOffsetSizeRange(OutFile,
                                            Mapping.addressRange(),
                                            /*Is64Bit=*/true,
                                            /*SizeIs64Bit=*/true,
                                            " (",
                                            ")");
            }

            Utils::RightPadSpaces(OutFile,
                                  fputs("\n\t\tSize: ", OutFile),
                                  STR_LENGTH("\n\t\t") + LongestKeyLength);

            if (Options.Verbose) {
                fprintf(OutFile,
                        "%s\n",
                        Utils::FormattedSizeForOutput(Mapping.Size).c_str());
            } else {
                fprintf(OutFile, "%" PRIu64 "\n", Mapping.Size);
            }

            Utils::RightPadSpaces(
                OutFile,
                fputs("\t\tSlide-Info File Offset: ", OutFile),
                STR_LENGTH("\t\t") + LongestKeyLength);

            Utils::PrintAddress(OutFile,
                                Mapping.SlideInfoFileOffset,
                                /*Is64Bit=*/true,
                                /*Prefix=*/"");

            if (PrintRange) {
                Utils::PrintOffsetSizeRange(OutFile,
                                            Mapping.slideInfoFileRange(),
                                            /*Is64Bit=*/true,
                                            /*SizeIs64Bit=*/true,
                                            " (",
                                            ")");
            }

            Utils::RightPadSpaces(
                OutFile,
                fputs("\n\t\tSlide-Info File Size: ", OutFile),
                STR_LENGTH("\n\t\t") + LongestKeyLength);

            if (Options.Verbose) {
                fprintf(OutFile,
                        "%s\n",
                        Utils::FormattedSizeForOutput(
                            Mapping.SlideInfoFileSize).c_str());
            } else {
                fprintf(OutFile, "%" PRIu64 "\n", Mapping.SlideInfoFileSize);
            }

            Index++;
        }
    }

    static void
    PrintDscHeaderV7Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV7();
        PrintOffsetCountPair(OutFile,
                             Dsc.range(),
                             "Mapping With Slide Info Address",
                             "Mapping With Slide Info Count",
                             Header.MappingWithSlideOffset,
                             Header.MappingWithSlideCount,
                             /*Suffix=*/"\n");

        if (Header.MappingWithSlideCount <= 10) {
            fputs("Mappings With Slide Info:\n", OutFile);
            PrintMappingWithSlideInfoList(OutFile, Options, Dsc);
        }
    }

    static void
    PrintPackedVersion(FILE *const OutFile,
                       const std::string_view Key,
                       const Dyld3::PackedVersion &Version) noexcept
    {
        PrintDscKey(OutFile, Key);
        fprintf(OutFile,
                DYLD3_PACKED_VERSION_FMT "\n",
                DYLD3_PACKED_VERSION_FMT_ARGS(Version));
    }

    static void
    PrintDscHeaderV8Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV8();

        PrintDscKey(OutFile, "Dylibs Prebuilt Loader Set Address");
        Utils::PrintAddress(OutFile,
                            Header.DylibsPBLSetAddr,
                            /*Is64Bit=*/true,
                            "",
                            "\n");

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Programs Prebuilt Loader Set Pool Address",
                          "Programs Prebuilt Loader Set Pool Size",
                          Header.ProgramsPBLSetPoolAddr,
                          Header.ProgramsPBLSetPoolSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ProgramsPBLSetPool);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Program Trie Address",
                          "Program Trie Size",
                          Header.ProgramTrieAddr,
                          Header.ProgramTrieSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ProgramTrie);

        PrintPackedVersion(OutFile, "OS Version", Header.osVersion());
        PrintPlatformValue(OutFile,
                               "Alternate Platform",
                               Header.altPlatform());

        PrintPackedVersion(OutFile,
                           "Alternate OS Version",
                           Header.altOsVersion());

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Swift Optimizations Offset",
                          "Swift Optimizations Size",
                          Header.SwiftOptsOffset,
                          Header.SwiftOptsSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::SwiftOpts);

        PrintOffsetCountPair(OutFile,
                             Dsc.range(),
                             "SubCache Array Offset",
                             "SubCache Array Count",
                             Header.SubCacheArrayOffset,
                             Header.SubCacheArrayCount,
                             /*Suffix=*/"\n");

        if (Header.SubCacheArrayCount < 10) {
            PrintSubCacheEntryInfo(OutFile, Dsc);
        }

        PrintDscKey(OutFile, "Symbol File UUID");
        Utils::PrintUuid(OutFile, Header.SymbolFileUUID, "", "\n");

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Rosetta Read-Only Address",
                          "Rosetta Read-Only Size",
                          Header.RosettaReadOnlyAddr,
                          Header.RosettaReadOnlySize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::RosettaReadOnly);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Rosetta Read-Write Address",
                          "Rosetta Read-Write Size",
                          Header.RosettaReadWriteAddr,
                          Header.RosettaReadWriteSize,
                          Options.Verbose);

        // HACK: Temp fix to bad output
        fflush(OutFile);
        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::RosettaReadWrite);
    }

    static void
    PrintDscHeaderV9Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        fputc('\n', OutFile);

        const auto &Header = Dsc.headerV9();
        PrintCacheKind(OutFile, "Cache Sub-Kind", Header.Kind);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Objc Optimizations Offset",
                          "Objc Optimizations Size",
                          Header.ObjcOptsOffset,
                          Header.ObjcOptsSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/false);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ObjcOpts);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Cache Atlas Offset",
                          "Cache Atlas Size",
                          Header.CacheAtlasOffset,
                          Header.CacheAtlasSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/false);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::CacheAtlas);

        PrintDscSizeRange(OutFile,
                          Dsc.range(),
                          "Dynamic Data Offset",
                          "Dynamic Data Max Size",
                          Header.DynamicDataOffset,
                          Header.DynamicDataMaxSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/false);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::DynamicDataMax);
    }

    auto
    PrintHeader::run(const Objects::DyldSharedCache &Dsc) const noexcept
        -> RunResult
    {
        auto Result = RunResult(Objects::Kind::DyldSharedCache);
        fputs("Apple Dyld Shared-Cache File\n", OutFile);

        const auto Version = Dsc.getVersion();
        const auto List = CollectDscRangeList(Dsc.header(), Version);

        PrintDscHeaderV0Info(OutFile, Opt, Dsc);
        if (Version < DyldSharedCache::HeaderVersion::V1) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV1Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V2) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV2Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V3) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV3Info(OutFile, Dsc);
        if (Version < DyldSharedCache::HeaderVersion::V4) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV4Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V5) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV5Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V6) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV6Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V7) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV7Info(OutFile, Dsc, Opt);
        if (Version < DyldSharedCache::HeaderVersion::V8) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV8Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V9) {
            return Result.set(RunError::None);
        }

        PrintDscHeaderV9Info(OutFile, Dsc, Opt, List);
        return Result.set(RunError::None);
    }

    auto PrintHeader::run(const Objects::Base &Base) const noexcept -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintHeader::run() got Object with Kind::None");
            case Objects::Kind::DyldSharedCache:
                return run(static_cast<const Objects::DyldSharedCache &>(Base));
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return run(static_cast<const Objects::FatMachO &>(Base));
        }

        assert(false && "Got unrecognized Object-Kind in PrintHeader::run()");
    }
}
