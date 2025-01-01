//
//  Operations/PrintHeader.cpp
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#include <format>
#include <print>
#include <unordered_map>

#include "ADT/FlagsIterator.h"

#include "Objects/DyldSharedCache.h"
#include "Objects/Open.h"

#include "Operations/PrintArchs.h"
#include "Operations/PrintHeader.h"

#include "Utils/Print.h"

constexpr static auto LongestDscKeyLength =
    STR_LENGTH("Programs Prebuilt Loader Set Pool Address");

struct DscKey {
protected:
    std::string_view Key;
    uint32_t PadLength;
public:
    constexpr explicit
    DscKey(const std::string_view Key,
           const uint32_t LongestKey = LongestDscKeyLength) noexcept
    : Key(Key), PadLength(LongestKey - Key.length()) {
        assert(LongestKey >= Key.length());
    }

    [[nodiscard]] constexpr auto key() const noexcept {
        return this->Key;
    }

    [[nodiscard]] constexpr auto padLength() const noexcept {
        return this->PadLength;
    }
};

template <>
struct std::formatter<DscKey> : std::formatter<std::string_view> {
    auto format(const DscKey &DscKey, auto &Ctx) const noexcept {
        auto Result = std::string();
        std::format_to(std::back_inserter(Result),
                       "{}: {:<{}}",
                       DscKey.key(), "", DscKey.padLength());

        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

namespace Operations {
    PrintHeader::PrintHeader(FILE *const OutFile,
                             const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintHeader), OutFile(OutFile), Opt(Options) {}

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

    static auto
    StringForCpuKind(const Mach::CpuKind CpuKind,
                     const bool Verbose) noexcept -> std::string
    {
        return Mach::CpuKindIsValid(CpuKind) ?
            Verbose ?
                std::string(Mach::CpuKindGetString(CpuKind)) :
                std::string(Mach::CpuKindGetDesc(CpuKind))
            : std::format("<Unknown: 0x{:02x}>",
                          static_cast<int32_t>(CpuKind));
    }

    static auto
    StringForSubKind(const Mach::CpuKind CpuKind,
                     const int32_t SubKind,
                     const bool Verbose) noexcept -> std::string
    {
        return Mach::CpuKindAndSubKindIsValid(CpuKind, SubKind) ?
            Verbose ?
                std::string(
                        Mach::CpuKindAndSubKindGetString(CpuKind, SubKind)) :
                    std::string(
                        Mach::CpuKindAndSubKindGetDesc(CpuKind, SubKind))
            : std::format("<Unknown: 0x{:02x}>",
                          static_cast<uint32_t>(SubKind));
    }

    auto
    PrintHeader::run(const Objects::MachO &MachO) const noexcept -> RunResult {
        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

        const auto &Header = MachO.header();
        const auto CpuKind = Header.cpuKind();
        const auto SubKind = Header.cpuSubKind();
        const auto FileKind = Header.fileKind();
        const auto Ncmds = Header.ncmds();
        const auto SizeOfCmds = Header.sizeOfCmds();
        const auto Flags = Header.flags();

        const auto CpuKindString = StringForCpuKind(CpuKind, Opt.Verbose);
        const auto SubKindString =
            StringForSubKind(CpuKind, SubKind, Opt.Verbose);

        const auto FileKindString =
            MachO::FileKindIsValid(FileKind) ?
                Opt.Verbose ?
                    std::string(MachO::FileKindGetString(FileKind)) :
                    std::string(MachO::FileKindGetDesc(FileKind))
                : std::format("<Unknown: 0x{:02x}>",
                              static_cast<uint32_t>(FileKind));

        std::print(OutFile,
                   "Apple {} Mach-O File\n"
                   "\tMagic:      {}\n"
                   "\tCputype:    {}\n",
                   MachO.is64Bit() ? "64-Bit" : "32-Bit",
                   Opt.Verbose ?
                    MachO::MagicGetString(Header.Magic) :
                    MachO::MagicGetDesc(Header.Magic),
                   CpuKindString);

        if (Opt.Verbose) {
            const auto RawCpuType = static_cast<uint32_t>(Header.rawCpuType());
            std::print(OutFile,
                       "\t\tIs 64-bit: {}\n"
                       "\t\tIs 64-bit with 32-bit pointers: {}\n",
                       RawCpuType & Mach::CpuABI64 ? "yes" : "no",
                       RawCpuType & Mach::CpuABI64_32 ? "yes" : "no");
        }

        std::println(OutFile, "\tCpuSubtype: {}", SubKindString);
        if (Opt.Verbose) {
            const auto RawCpuSubType =
                static_cast<uint32_t>(Header.rawCpuSubType());

            std::print(OutFile,
                       "\t\tIs 64-bit: {}\n"
                       "\t\tSupports Pointer Authentication: {}\n",
                       RawCpuSubType & Mach::CpuSubtypeLib64 ?
                        "yes" : "no",
                       RawCpuSubType & Mach::CpuSubtypePtrauthABI ?
                        "yes" : "no");
        }

        std::print(OutFile,
                   "\tFiletype:   {}\n"
                   "\tNcmds:      {}\n"
                   "\tSizeOfCmds: {}\n"
                   "\tFlags:      0x{:x}\n",
                   FileKindString,
                   Utils::FormattedNumber(Ncmds),
                   Utils::FormattedNumber(SizeOfCmds),
                   Flags.value());

        auto Counter = static_cast<uint8_t>(1);
        auto DigitCount =
            Utils::GetIntegerDigitCount(
                static_cast<uint32_t>(std::popcount(Flags.value())));

        for (const auto Bit : ADT::FlagsIterator(Flags)) {
            const auto Flag = MachO::Flags::Kind(1 << Bit);
            std::println(OutFile,
                         "\t\t{:0{}}. Bit {:02}: {}",
                         Counter,
                         DigitCount,
                         Bit,
                         MachO::Flags::KindIsValid(Flag) ?
                          MachO::Flags::KindGetString(Flag) :
                          "<Unknown>");

            Counter++;
        }

        return RunResult();
    }

    auto
    PrintHeader::run(const Objects::FatMachO &Fat) const noexcept -> RunResult {
        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

        const auto Header = Fat.header();
        const auto ArchCount = Header.archCount();

        std::print(OutFile,
                   "Apple {} Fat Mach-O File\n"
                   "\tMagic: {}\n"
                   "\tArch Count: {}\n",
                   Fat.is64Bit() ? "64-Bit" : "32-Bit",
                   Opt.Verbose ?
                    MachO::MagicGetString(Header.Magic) :
                    MachO::MagicGetDesc(Header.Magic),
                   ArchCount);

        if (ArchCount < 6) {
            auto I = uint32_t();
            const auto IsBigEndian = Fat.isBigEndian();

            if (Fat.is64Bit()) {
                for (const auto &Arch : Fat.arch64List()) {
                    const auto Object =
                        std::unique_ptr<Objects::Base>(
                            Objects::OpenArch(Fat, I).value());

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
                            Objects::OpenArch(Fat, I).value());

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

        return RunResult();
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
        PatchInfo,
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
                if (const auto Range = Header.mappingInfoListRange()) {
                    AddRangeToList(Result, DscRange {
                        .Kind = DscRangeKind::MappingInfoList,
                        .Range = Range.value(),
                        .OverlapKindSet = {}
                    });
                }

                [[fallthrough]];
            }

            case DscRangeKind::ImageInfoList: {
                if (const auto Range = Header.imageInfoListRange()) {
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
                if (const auto Range = Header.imageTextInfoListRange()) {
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
            case DscRangeKind::PatchInfo:
                AddRangeToList(Result, DscRange {
                    .Kind = DscRangeKind::PatchInfo,
                    .Range = Header.patchInfoRange(),
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
            std::println(OutFile);
            return;
        }

        const auto End = List.cend();
        const auto Iter = List.find(Kind);

        if (Iter == End) {
            std::println(OutFile);
            return;
        }

        const auto &OverlapKindSet = Iter->second.OverlapKindSet;
        if (OverlapKindSet.none()) {
            std::println(OutFile);
            return;
        }

        std::print(OutFile, " (Overlaps with ");

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
                    assert(false && "Overlap-Kind None is set");
                case DscRangeKind::MappingInfoList:
                    std::print(OutFile, "Mapping-Info Range");
                    break;
                case DscRangeKind::ImageInfoList:
                    std::print(OutFile, "Image-Info Range");
                    break;
                case DscRangeKind::CodeSignature:
                    std::print(OutFile, "Code-Signature Range");
                    break;
                case DscRangeKind::SlideInfo:
                    std::print(OutFile, "Slide-Info Range");
                    break;
                case DscRangeKind::LocalSymbolInfo:
                    std::print(OutFile, "Local-Symbol Info Range");
                    break;
                case DscRangeKind::ImageTextInfoList:
                    std::print(OutFile, "Image-Text Info Range");
                    break;
                case DscRangeKind::PatchInfo:
                    std::print(OutFile, "Dylibs Image-Group Range");
                    break;
                case DscRangeKind::OtherImageGroup:
                    std::print(OutFile, "Other-Image Group Range");
                    break;
                case DscRangeKind::ProgClosures:
                    std::print(OutFile, "Prog-Closures Range");
                    break;
                case DscRangeKind::ProgClosuresTrie:
                    std::print(OutFile, "Prog-Closures Trie Range");
                    break;
                case DscRangeKind::DylibsImageArray:
                    std::print(OutFile, "Dylibs-Image Range");
                    break;
                case DscRangeKind::DylibsTrie:
                    std::print(OutFile, "Dylibs-Trie Range");
                    break;
                case DscRangeKind::OtherImageArray:
                    std::print(OutFile, "Other-Image Array Range");
                    break;
                case DscRangeKind::OtherTrie:
                    std::print(OutFile, "Other-Trie Range");
                    break;
                case DscRangeKind::ProgramsPBLSetPool:
                    std::print(OutFile,
                               "Programs Prebuilt Loader Set Pool Range");
                    break;
                case DscRangeKind::ProgramTrie:
                    std::print(OutFile, "Program Trie Range");
                    break;
                case DscRangeKind::SwiftOpts:
                    std::print(OutFile, "Swift Optimizations Range");
                    break;
                case DscRangeKind::RosettaReadOnly:
                    std::print(OutFile, "Rosetta Read Only Range");
                    break;
                case DscRangeKind::RosettaReadWrite:
                    std::print(OutFile, "Rosetta Read Write Range");
                    break;
                case DscRangeKind::ObjcOpts:
                    std::print(OutFile, "Objc Opts Range");
                    break;
                case DscRangeKind::CacheAtlas:
                    std::print(OutFile, "Cache Atlas Range");
                    break;
                case DscRangeKind::DynamicDataMax:
                    std::print(OutFile, "Dynamic Data Max");
                    break;
                case DscRangeKind::End:
                    assert(false && "Overlap-Kind End is set");
            }

            Count++;
            if (Count == OverlapKindSetCount) {
                std::println(OutFile, ")");
                break;
            } else {
                std::print(OutFile, ", ");
            }
        }
    }

    static void
    WarnIfOutOfRange(FILE *const OutFile,
                     const Objects::DyldSharedCache &Dsc,
                     const uint64_t Offset,
                     const uint64_t Size,
                     const bool PrintNewLine = true) noexcept
    {
        const auto &DscRange = Dsc.range();
        const auto Range = ADT::Range::FromSize(Offset, Size);

        if (!DscRange.contains(Range)) {
            const auto ResultOpt = Dsc.getMapForAddrRange(Range);
            if (!ResultOpt.has_value()) {
                std::print(OutFile,
                           " (Out of range, Not entirely found in any "
                           "single, available sub-cache)");
                return;
            }

            const auto &[SubCache, Map] = ResultOpt.value();
            std::print(OutFile,
                       " (Past EOF, Found in sub-cache with "
                       "file-suffix \"{}\")",
                       SubCache.fileSuffix());
        }

        if (PrintNewLine) {
            std::println(OutFile);
        }
    }

    template <std::unsigned_integral AddrType, std::unsigned_integral SizeType>
    static inline void
    PrintDscSizeRange(FILE *const OutFile,
                      const Objects::DyldSharedCache &Dsc,
                      const std::string_view AddressName,
                      const std::string_view SizeName,
                      const AddrType Address,
                      const SizeType Size,
                      const bool Verbose,
                      const std::string_view Prefix = "",
                      const std::string_view Suffix = "",
                      const bool IsOffset = false,
                      const bool PrintNewLine = false) noexcept
    {
        std::print(OutFile,
                   "{}{}{}",
                   Prefix, DscKey(AddressName), Utils::Address(Address));

        if (Verbose && Size != 0) {
            std::print(OutFile,
                       " ({})",
                       Utils::PrintRange<AddrType>(Address, Size));
        }

        if (IsOffset) {
            WarnIfOutOfRange(OutFile,
                             Dsc,
                             Address,
                             Size,
                             /*PrintNewLine=*/false);
        }

        std::println(OutFile);
        std::print(OutFile,
                   "{}{}{}",
                   Suffix, DscKey(SizeName), Utils::ByteSize(Size));

        if (PrintNewLine) {
            std::println(OutFile);
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
        auto Index = static_cast<uint32_t>(1);

        for (const auto &Mapping : Dsc.mappingInfoList()) {
            std::print(OutFile,
                       "\tMapping {:0{}}: {}\n"
                       "\t\t{}{}",
                       Index,
                       MappingCountDigitLength,
                       Mapping.initAndMaxProt(),
                       DscKey("File-Offset", LongestKeyLength),
                        Utils::Address(Mapping.FileOffset));

            const auto PrintRange = !Mapping.empty() && Options.Verbose;
            if (PrintRange) {
                std::print(OutFile, " ({})", Mapping.fileRange());
            }

            std::println(OutFile);
            std::println(OutFile,
                         "\t\t{}{}",
                         DscKey("Address", LongestKeyLength),
                            Utils::Address(Mapping.Address));

            if (PrintRange) {
                std::print(OutFile,
                           " ({})",
                           Utils::PrintRange(Mapping.addressRange()));
            }

            std::println(OutFile);
            std::println(OutFile,
                         "\t\t{}{}",
                         DscKey("Size", LongestKeyLength),
                            Utils::ByteSize(Mapping.Size));

            Index++;
        }
    }

    static void
    PrintSubCacheEntryInfo(FILE *const OutFile,
                           const Objects::DyldSharedCache &Dsc)
    {
        constexpr auto LongestKeyLength = STR_LENGTH("Cache Vm-Offset: ");
        const auto MappingCountDigitLength =
            Utils::GetIntegerDigitCount(Dsc.mappingCount());

        auto Index = static_cast<uint32_t>(1);
        if (const auto ListOpt = Dsc.subCacheEntryInfoList()) {
            for (const auto &Info : ListOpt.value()) {
                std::print(OutFile,
                           "\tSubCache Entry {:0{}}: {}\n"
                           "\t\t{}{}\n"
                           "\t\t{}\"{}\"\n",
                           Index,
                           MappingCountDigitLength,
                           Utils::Uuid(Info.Uuid),
                           DscKey("Cache Vm-Offset", LongestKeyLength),
                            Utils::Address(Info.CacheVMOffset),
                           DscKey("File Suffix", LongestKeyLength),
                           Info.fileSuffix());

                Index++;
            }

            return;
        }

        const auto ListOpt = Dsc.subCacheEntryV1InfoList();
        if (!ListOpt.has_value()) {
            return;
        }

        for (const auto &Info : ListOpt.value()) {
            std::print(OutFile,
                       "\tSubCache V1 Entry {:0{}}: {}\n"
                       "\t\t{}{}\n",
                       Index,
                       MappingCountDigitLength,
                       Utils::Uuid(Info.Uuid),
                       DscKey("Cache Vm-Offset", LongestKeyLength),
                        Info.CacheVMOffset);

            Index++;
        }
    }

    static void
    PrintDscHeaderV0Info(FILE *const OutFile,
                         const struct PrintHeader::Options &Options,
                         const Objects::DyldSharedCache &Dsc) noexcept
    {
        const auto Header = Dsc.headerV0();
        std::print(OutFile,
                   "{}\"{}\"",
                   DscKey("Magic"), Header.Magic);

        if (Options.Verbose) {
            const auto [CpuKind, CpuSubKind] = Dsc.getMachCpuKindAndSubKind();
            std::println(OutFile,
                         " (Cpu-Kind: {})",
                         Mach::CpuKindAndSubKindGetString(CpuKind, CpuSubKind));
        } else {
            std::println(OutFile);
        }

        const auto Version = Dsc.getVersion();
        std::print(OutFile,
                   "{}{}\n"
                   "{}{}\n"
                   "{}{}\n",
                   DscKey("Version"), static_cast<int>(Version),
                   DscKey("Mapping Offset"),
                    Utils::Address(Header.MappingOffset),
                   DscKey("Mapping Count"),
                    Utils::FormattedNumber(Header.MappingCount));

        if (Dsc.mappingCount() <= 10) {
            std::println(OutFile, "Mappings:");
            PrintMappingInfoList(OutFile, Options, Dsc);
        }

        if (Header.isAtleastV8()) {
            std::print(OutFile,
                       "{}{}\n"
                       "{}{}\n",
                       DscKey("Images Offset (Old)"),
                        Utils::Address(Header.ImagesOffsetOld),
                       DscKey("Images Count (Old)"),
                        Utils::FormattedNumber(Header.ImagesCountOld));
        } else {
            std::print(OutFile,
                       "{}{}\n"
                       "{}{}\n",
                       DscKey("Images Offset"),
                        Utils::Address(Header.ImagesOffsetOld),
                       DscKey("Images Count"),
                        Utils::FormattedNumber(Header.ImagesCountOld));
        }

        std::println(OutFile,
                     "{}{}",
                     DscKey("Dyld Base-Address"),
                        Utils::Address(Header.DyldBaseAddress));
    }

    static void
    PrintDscHeaderV1Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        std::println(OutFile);

        const auto &Header = Dsc.headerV1();
        PrintDscSizeRange(OutFile,
                          Dsc,
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
                          Dsc,
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
    }

    static void
    PrintDscHeaderV2Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        std::println(OutFile);

        const auto &Header = Dsc.headerV2();
        PrintDscSizeRange(OutFile,
                          Dsc,
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

        std::println(OutFile,
                     "{}\"{}\"",
                     DscKey("Uuid"), Utils::Uuid(Header.Uuid));
    }

    template <std::unsigned_integral OffsetType,
              std::unsigned_integral SizeType>

    static void
    PrintOffsetCountPair(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const std::string_view OffsetKey,
                         const std::string_view CountKey,
                         const OffsetType Offset,
                         const SizeType Count,
                         const std::string_view Suffix = "")
    {
        std::print(OutFile,
                   "{}{}",
                   DscKey(OffsetKey), Utils::Address(Offset));

        WarnIfOutOfRange(OutFile, Dsc, Offset, /*Size=*/1);
        std::print(OutFile,
                   "{}{}{}",
                   DscKey(CountKey), Utils::FormattedNumber(Count), Suffix);
    }

    static void
    PrintCacheKind(FILE *const OutFile,
                   const std::string_view Key,
                   const DyldSharedCache::CacheKind CacheKind) noexcept
    {
        switch (CacheKind) {
            case DyldSharedCache::CacheKind::Development:
                std::println(OutFile, "{}Development", DscKey(Key));
                return;
            case DyldSharedCache::CacheKind::Production:
                std::println(OutFile, "{}Production", DscKey(Key));
                return;
        }

        std::println(OutFile, "{}<Unrecognized>", DscKey(Key));
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
        std::println(OutFile);

        const auto &Header = Dsc.headerV4();
        PrintOffsetCountPair(OutFile,
                             Dsc,
                             "Branch-Pools Offset",
                             "Branch-Pools Count",
                             Header.BranchPoolsOffset,
                             Header.BranchPoolsCount,
                             /*Suffix=*/"\n");

        PrintOffsetCountPair(OutFile,
                             Dsc,
                             "Images-Text Offset",
                             "Images-Text Count",
                             Header.ImagesTextOffset,
                             Header.ImagesTextCount);

        if (Header.ImagesTextCount != Dsc.imageCount()) {
            std::print(OutFile,
                       " (Invalid: {} Image-Texts vs {} Images",
                       Header.ImagesTextCount,
                       Dsc.imageCount());
        }

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ImageTextInfoList);

        std::print(OutFile,
                   "{}{}\n"
                   "{}{}\n",
                   DscKey("Dyld In-Cache Mach-Header Address"),
                    Utils::Address(Header.DyldInCacheMachHeaderAddr),
                   DscKey("Dyld In-Cache Entry-Point Address"),
                    Utils::Address(Header.DyldInCacheEntryPointAddr));
    }

    static inline void
    PrintBoolValue(FILE *const OutFile,
                   const std::string_view Key,
                   const bool Value) noexcept
    {
        std::println(OutFile,
                     "{}{}",
                     DscKey(Key), (Value) ? "true" : "false");
    }

    static inline void
    PrintPlatformValue(FILE *const OutFile,
                       const std::string_view Key,
                       const Dyld3::Platform Platform) noexcept
    {
        if (Dyld3::PlatformIsValid(Platform)) {
            std::println(OutFile,
                         "{}{}",
                         DscKey(Key), Dyld3::PlatformGetDesc(Platform));
        } else {
            std::println(OutFile,
                         "{}<unknown, value={}>",
                         DscKey(Key), static_cast<uint32_t>(Platform));
        }
    }

    static void
    PrintDscHeaderV5Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        std::println(OutFile);

        const auto &Header = Dsc.headerV5();
        PrintDscSizeRange(OutFile,
                          Dsc,
                          "Patch Info Address",
                          "Patch Info Size",
                          Header.PatchInfoAddr,
                          Header.PatchInfoSize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::PatchInfo);

        PrintDscSizeRange(OutFile,
                          Dsc,
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
                          Dsc,
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
                          Dsc,
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
        std::println(OutFile,
                     "{}{}",
                     DscKey("Closure-Format Version"), Header.FormatVersion);

        PrintBoolValue(OutFile,
                       "Dylibs Expected On Disk",
                       Header.DylibsExpectedOnDisk);

        PrintBoolValue(OutFile, "Simulator", Header.Simulator);
        PrintBoolValue(OutFile,
                       "Locally-Built Cache",
                       Header.isLocallyBuiltCache().value_or(false));

        PrintBoolValue(OutFile,
                       "Built From Chained-Fixups",
                       Header.BuiltFromChainedFixups);

        PrintDscSizeRange(OutFile,
                          Dsc,
                          "Shared-Region Start",
                          "Shared-Region Size",
                          Header.SharedRegionStart,
                          Header.SharedRegionSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/false,
                          /*PrintNewLine=*/true);

        std::println(OutFile,
                     "{}{}",
                     DscKey("Max Slide"), Utils::ByteSize(Header.MaxSlide));
    }

    static void
    PrintDscHeaderV6Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        std::println(OutFile);

        const auto &Header = Dsc.headerV6();
        PrintDscSizeRange(OutFile,
                          Dsc,
                          "Dylibs Image-Array Address",
                          "Dylibs Image-Array Size",
                          Header.DylibsImageArrayAddr,
                          Header.DylibsImageArraySize,
                          Options.Verbose);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::PatchInfo);

        PrintDscSizeRange(OutFile,
                          Dsc,
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
                          Dsc,
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
                          Dsc,
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
        const auto LongestKeyLength = STR_LENGTH("Slide-Info File Offset: ");
        const auto MappingCountDigitLength =
            Utils::GetIntegerDigitCount(Dsc.mappingCount());

        const auto &Header = Dsc.headerV7();
        auto Index = static_cast<int>(1);

        for (const auto &Mapping : Header.mappingWithSlideInfoList()) {
            std::print(OutFile,
                       "\tMapping {:0{}}: {}\n"
                       "\t\t{}{}",
                       Index,
                       MappingCountDigitLength,
                       Mapping.initAndMaxProt(),
                       DscKey("File-Offset", LongestKeyLength),
                        Utils::Address(Mapping.FileOffset));

            const auto PrintRange = !Mapping.empty() && Options.Verbose;
            if (PrintRange) {
                std::print(OutFile,
                           " ({})",
                           Utils::PrintRange(Mapping.fileRange()));
            }

            std::println(OutFile);
            std::print(OutFile,
                       "\t\t{}{}",
                       DscKey("Address", LongestKeyLength),
                        Utils::Address(Mapping.Address));

            if (PrintRange) {
                std::print(OutFile,
                           " ({})",
                           Utils::PrintRange(Mapping.addressRange()));
            }

            std::println(OutFile);
            std::print(OutFile,
                       "\t\t{}{}\n"
                       "\t\t{}{}",
                       DscKey("Size", LongestKeyLength),
                        Utils::ByteSize(Mapping.Size),
                       DscKey("Slide-Info File Offset", LongestKeyLength),
                        Utils::Address(Mapping.SlideInfoFileOffset));

            if (!Mapping.slideInfoFileRange().empty() && Options.Verbose) {
                std::print(OutFile,
                           " ({})",
                           Utils::PrintRange(Mapping.slideInfoFileRange()));
            }

            std::println(OutFile);
            std::println(OutFile,
                         "\t\t{}{}",
                         DscKey("Slide-Info File Size", LongestKeyLength),
                            Utils::ByteSize(Mapping.SlideInfoFileSize));

            Index++;
        }
    }

    static void
    PrintDscHeaderV7Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options) noexcept
    {
        std::println(OutFile);

        const auto &Header = Dsc.headerV7();
        PrintOffsetCountPair(OutFile,
                             Dsc,
                             "Mapping With Slide Info Offset",
                             "Mapping With Slide Info Count",
                             Header.MappingWithSlideOffset,
                             Header.MappingWithSlideCount,
                             /*Suffix=*/"\n");

        if (Header.MappingWithSlideCount <= 10) {
            std::println(OutFile, "Mapping With Slide Info List:");
            PrintMappingWithSlideInfoList(OutFile, Options, Dsc);
        }
    }

    static void
    PrintPackedVersion(FILE *const OutFile,
                       const std::string_view Key,
                       const Dyld3::PackedVersion &Version) noexcept
    {
        std::println(OutFile, "{}{}", DscKey(Key), Version);
    }

    static void
    PrintDscHeaderV8Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        std::println(OutFile);
        const auto &Header = Dsc.headerV8();

        std::println(OutFile,
                     "{}{}",
                     DscKey("Dylibs Prebuilt Loader Set Address"),
                        Utils::Address(Header.DylibsPBLSetAddr));

        PrintDscSizeRange(OutFile,
                          Dsc,
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
                          Dsc,
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
                          Dsc,
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
                             Dsc,
                             "SubCache Array Offset",
                             "SubCache Array Count",
                             Header.SubCacheArrayOffset,
                             Header.SubCacheArrayCount,
                             /*Suffix=*/"\n");

        if (Header.SubCacheArrayCount < 10) {
            PrintSubCacheEntryInfo(OutFile, Dsc);
        }

        std::println(OutFile,
                     "{}\"{}\"",
                     DscKey("Symbol File Uuid"),
                        Utils::Uuid(Header.SymbolFileUUID));

        PrintDscSizeRange(OutFile,
                          Dsc,
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
                          Dsc,
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

        std::print(OutFile,
                   "{}{}\n"
                   "{}{}\n",
                   DscKey("Images Offset"),
                    Utils::CustomAddress(Header.ImagesOffset),
                   DscKey("Images Count"),
                    Utils::FormattedNumber(Header.ImagesCount));
    }

    static void
    PrintObjcOptsInfo(FILE *const OutFile,
                      const Objects::DyldSharedCache &Dsc,
                      const DyldSharedCache::HeaderV9 &Header) noexcept
    {
        const auto ObjcOptsRange =
            ADT::Range::FromSize(Header.DynamicDataOffset,
                                 sizeof(DyldSharedCache::DynamicDataHeader));

        if (const auto Header =
                Dsc.getForFileRange<
                    DyldSharedCache::ObjcOptimizationHeader>(ObjcOptsRange))
        {
            std::print(OutFile,
                       "\tVersion: {}\n"
                       "\tFlags:   {}\n",
                       Header->Version,
                       Header->Flags);

            auto Counter = uint32_t();
            for (const auto Bit :
                    ADT::FlagsIterator<uint32_t>(Header->flags()))
            {
                using FlagsStruct =
                    DyldSharedCache::ObjcOptimizationHeader::FlagsStruct;

                const auto Flag = static_cast<FlagsStruct::Kind>(1ull << Bit);
                std::println(OutFile,
                             "\t\t{}. Bit {}: {}",
                             Counter + 1,
                             Bit,
                             FlagsStruct::KindIsValid(Flag) ?
                                 FlagsStruct::KindGetString(Flag) :
                                 "<unknown>");

                Counter++;
            }

            std::print(OutFile,
                       "\tHeader-Info Read-Only Cache Offset: {}\n"
                       "\tHeader-Info Read-Write Cache Offset: {}\n"
                       "\tSelector Hash-Table Cache Offset: {}\n"
                       "\tClass Hash-Table Cache Offset: {}\n"
                       "\tProtocol Hash-Table Cache Offset: {}\n"
                       "\tRelative Method Selector Base Address Offset: {}\n",
                       Utils::Address(Header->HeaderInfoReadOnlyCacheOffset),
                       Utils::Address(Header->HeaderInfoReadWriteCacheOffset),
                       Utils::Address(Header->SelectorHashTableCacheOffset),
                       Utils::Address(Header->ClassHashTableCacheOffset),
                       Utils::Address(Header->ProtocolHashTableCacheOffset),
                       Utils::Address(
                        Header->RelativeMethodSelectorBaseAddressOffset));
        }
    }

    static void
    PrintDynamicDataHeader(FILE *const OutFile,
                           const Objects::DyldSharedCache &Dsc,
                           const DyldSharedCache::HeaderV9 &Header) noexcept
    {
        const auto DynamicDataHeaderRange =
            ADT::Range::FromSize(Header.DynamicDataOffset,
                                 sizeof(DyldSharedCache::DynamicDataHeader));

        if (const auto Header =
                Dsc.getForFileRange<
                    DyldSharedCache::DynamicDataHeader>(DynamicDataHeaderRange))
        {
            std::print(OutFile,
                       "\tMagic:   {}\n"
                       "\tFsId:    {}\n"
                       "\tFsObjId: {}\n",
                       Header->magic(),
                       Header->FsId,
                       Header->FsObjectId);
        }
    }

    static void
    PrintDscHeaderV9Info(FILE *const OutFile,
                         const Objects::DyldSharedCache &Dsc,
                         const struct PrintHeader::Options &Options,
                         const DscRangeList &List) noexcept
    {
        std::println(OutFile);
        const auto &Header = Dsc.headerV9();

        PrintCacheKind(OutFile, "Cache Sub-Kind", Header.cacheSubKind());
        PrintDscSizeRange(OutFile,
                          Dsc,
                          "Objc Optimizations Offset",
                          "Objc Optimizations Size",
                          Header.ObjcOptsOffset,
                          Header.ObjcOptsSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::ObjcOpts);

        PrintObjcOptsInfo(OutFile, Dsc, Header);
        PrintDscSizeRange(OutFile,
                          Dsc,
                          "Cache Atlas Offset",
                          "Cache Atlas Size",
                          Header.CacheAtlasOffset,
                          Header.CacheAtlasSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::CacheAtlas);

        PrintDscSizeRange(OutFile,
                          Dsc,
                          "Dynamic Data Offset",
                          "Dynamic Data Max Size",
                          Header.DynamicDataOffset,
                          Header.DynamicDataMaxSize,
                          Options.Verbose,
                          /*Prefix=*/"",
                          /*Suffix=*/"",
                          /*IsOffset=*/true);

        PrintDscRangeOverlapsErrorOrNewline(OutFile,
                                            Options,
                                            List,
                                            DscRangeKind::DynamicDataMax);

        PrintDynamicDataHeader(OutFile, Dsc, Header);
    }

    auto
    PrintHeader::run(const Objects::DyldSharedCache &Dsc) const noexcept
        -> RunResult
    {
        std::println(OutFile, "Apple Dyld Shared-Cache File");

        const auto &Opt = this->Opt;
        const auto OutFile = this->OutFile;

        const auto Version = Dsc.getVersion();
        const auto List = CollectDscRangeList(Dsc.header(), Version);

        PrintDscHeaderV0Info(OutFile, Opt, Dsc);
        if (Version < DyldSharedCache::HeaderVersion::V1) {
            return RunResult();
        }

        PrintDscHeaderV1Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V2) {
            return RunResult();
        }

        PrintDscHeaderV2Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V3) {
            return RunResult();
        }

        PrintDscHeaderV3Info(OutFile, Dsc);
        if (Version < DyldSharedCache::HeaderVersion::V4) {
            return RunResult();
        }

        PrintDscHeaderV4Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V5) {
            return RunResult();
        }

        PrintDscHeaderV5Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V6) {
            return RunResult();
        }

        PrintDscHeaderV6Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V7) {
            return RunResult();
        }

        PrintDscHeaderV7Info(OutFile, Dsc, Opt);
        if (Version < DyldSharedCache::HeaderVersion::V8) {
            return RunResult();
        }

        PrintDscHeaderV8Info(OutFile, Dsc, Opt, List);
        if (Version < DyldSharedCache::HeaderVersion::V9) {
            return RunResult();
        }

        PrintDscHeaderV9Info(OutFile, Dsc, Opt, List);
        return RunResult();
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
