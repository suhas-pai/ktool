/*
 * include/ADT/DyldSharedCacheHeaders.h
 * © suhas pai
 */

#pragma once

#include "ADT/List.h"
#include "ADT/Maximizer.h"
#include "ADT/Range.h"

#include "Dyld3/PackedVersion.h"
#include "Dyld3/Platform.h"

#include "Mach/VmProt.h"
#include "Utils/Misc.h"

namespace DyldSharedCache {
    enum class HeaderVersion {
        V0,
        V1,
        V2,
        V3,
        V4,
        V5,
        V6,
        V7,
        V8,
        V9
    };

    struct MappingInfo {
        uint64_t Address;
        uint64_t Size;
        uint64_t FileOffset;
        uint32_t MaxProt;
        uint32_t InitProt;

        [[nodiscard]] constexpr auto maxProt() const noexcept {
            return Mach::VmProt(MaxProt);
        }

        [[nodiscard]] constexpr auto initProt() const noexcept {
            return Mach::VmProt(InitProt);
        }

        [[nodiscard]] constexpr auto addressRange() const noexcept {
            return ADT::Range::FromSize(Address, Size);
        }

        [[nodiscard]] constexpr auto fileRange() const noexcept {
            return ADT::Range::FromSize(FileOffset, Size);
        }

        [[nodiscard]] inline auto empty() const noexcept {
            return Size == 0;
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddr(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
                -> std::optional<uint64_t>
        {
            if (!addressRange().containsLoc(Addr)) {
                return std::nullopt;
            }

            const auto AddrIndex = addressRange().indexForLoc(Addr, MaxSizeOut);
            if (fileRange().containsIndex(AddrIndex)) {
                return fileRange().locForIndex(AddrIndex);
            }

            return std::nullopt;
        }
    };

    struct MappingWithSlideInfo {
        struct FlagsStruct : public ADT::FlagsBase<uint64_t> {
        public:
            using ADT::FlagsBase<uint64_t>::FlagsBase;

            enum class Masks : uint64_t {
                AuthData  = 1 << 0,
                DirtyData = 1 << 1,
                ConstData = 1 << 2
            };

            [[nodiscard]] inline bool isAuthData() const noexcept {
                return has(Masks::AuthData);
            }

            [[nodiscard]] inline bool isDirtyData() const noexcept {
                return has(Masks::DirtyData);
            }

            [[nodiscard]] inline bool isConstData() const noexcept {
                return has(Masks::ConstData);
            }
        };

        uint64_t Address;
        uint64_t Size;
        uint64_t FileOffset;
        uint64_t SlideInfoFileOffset;
        uint64_t SlideInfoFileSize;
        uint64_t Flags;
        uint32_t MaxProt;
        uint32_t InitProt;

        [[nodiscard]] constexpr auto maxProt() const noexcept {
            return Mach::VmProt(MaxProt);
        }

        [[nodiscard]] constexpr auto initProt() const noexcept {
            return Mach::VmProt(InitProt);
        }

        [[nodiscard]] constexpr auto flags() const noexcept {
            return FlagsStruct(Flags);
        }

        [[nodiscard]] constexpr auto addressRange() const noexcept {
            return ADT::Range::FromSize(Address, Size);
        }

        [[nodiscard]] constexpr auto fileRange() const noexcept {
            return ADT::Range::FromSize(FileOffset, Size);
        }

        [[nodiscard]] constexpr auto slideInfoFileRange() const noexcept {
            return ADT::Range::FromSize(SlideInfoFileOffset, SlideInfoFileSize);
        }

        [[nodiscard]] inline auto empty() const noexcept {
            return Size == 0;
        }

        [[nodiscard]] inline uint64_t
        getFileOffsetFromAddr(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
        {
            if (!addressRange().containsLoc(Addr)) {
                return 0;
            }

            const auto Delta = Addr - this->Address;
            if (MaxSizeOut != nullptr) {
                *MaxSizeOut = (this->Address + this->Size) - Addr;
            }

            return FileOffset + Delta;
        }
    };

    struct HeaderV0;
    struct ImageInfo {
        uint64_t Address;
        uint64_t ModTime;
        uint64_t Inode;
        uint32_t PathFileOffset;
        uint32_t Pad;

        [[nodiscard]]
        inline auto pathPtr(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<const char *>(Map) + PathFileOffset;
        }

        [[nodiscard]]
        inline auto isAlias(const HeaderV0 &Header) const noexcept -> bool;

        [[nodiscard]]
        inline auto isAlias(const uint8_t *const Map) const noexcept {
            return isAlias(*reinterpret_cast<const HeaderV0 *>(Map));
        }
    };

    struct ImageInfoExtra {
        uint64_t ExportsTrieAddr;
        uint64_t WeakBindingsAddr;
        uint32_t ExportsTrieSize;
        uint32_t WeakBindingsSize;
        uint32_t DependentsStartArrayIndex;
        uint32_t ReExportsStartArrayIndex;
    };

    struct ImageTextInfo {
        uint8_t Uuid[16];
        uint64_t LoadAddress;
        uint32_t TextSegmentSize;
        uint32_t PathOffset;
    };

    struct AcceleratorInfo {
        uint32_t Version;
        uint32_t ImageExtrasCount;
        uint32_t ImageExtrasOffset;
        uint32_t BottomUpListOffset;
        uint32_t DylibTrieOffset;
        uint32_t DylibTrieSize;
        uint32_t InitializersOffset;
        uint32_t InitializersCount;
        uint32_t DofSectionsOffset;
        uint32_t DofSectionsCount;
        uint32_t ReExportListOffset;
        uint32_t ReExportCount;
        uint32_t DepListOffset;
        uint32_t DepListCount;
        uint32_t RangeTableOffset;
        uint32_t RangeTableCount;
        uint64_t DyldSectionAddr;

        [[nodiscard]] inline ImageInfoExtra &
        getImageInfoExtraAtIndex(const uint32_t Index) noexcept {
            auto &Result =
                const_cast<ImageInfoExtra &>(
                    getConstImageInfoExtraAtIndex(Index));

            return Result;
        }

        [[nodiscard]]
        inline const ImageInfoExtra &
        getImageInfoExtraAtIndex(const uint32_t Index) const noexcept {
            return getConstImageInfoExtraAtIndex(Index);
        }

        [[nodiscard]] inline const ImageInfoExtra &
        getConstImageInfoExtraAtIndex(const uint32_t Index) const noexcept {
            assert(!Utils::IndexOutOfBounds(Index, ImageExtrasCount));

            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto ImageInfoExtraTable =
                reinterpret_cast<const ImageInfoExtra *>(
                    Map + ImageExtrasOffset);

            return ImageInfoExtraTable[Index];
        }
    };

    struct SubCacheEntryV1 {
        uint8_t Uuid[16];
        uint64_t CacheVMOffset;
    };

    struct SubCacheEntry {
        uint8_t Uuid[16];
        uint64_t CacheVMOffset;
        char FileSuffix[32];
    };

    // Apple doesn't provide versions for their dyld_shared_caches, so we have
    // to make up our own.

    // This is the original header from dyld v95.3

    struct HeaderV0 {
        char Magic[16];

        uint32_t MappingOffset;
        uint32_t MappingCount;

        uint32_t ImagesOffsetOld;
        uint32_t ImagesCountOld;

        uint64_t DyldBaseAddress;

        [[nodiscard]] constexpr auto magic() const noexcept {
            return std::string_view(Magic, strnlen(Magic, sizeof(Magic)));
        }

        [[nodiscard]] constexpr auto isV1() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV2() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV3() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV4() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV5() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV6() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV7() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV8() const noexcept -> bool;
        [[nodiscard]] constexpr auto isV9() const noexcept -> bool;

        [[nodiscard]]
        constexpr auto getVersion() const noexcept -> HeaderVersion;

        [[nodiscard]] constexpr auto imageOffset() const noexcept -> uint32_t;
        [[nodiscard]] constexpr auto imageCount() const noexcept -> uint32_t;

        [[nodiscard]]
        inline auto getImageInfoListRange() const noexcept
            -> std::optional<ADT::Range>
        {
            const auto End =
                Utils::MulAddAndCheckOverflow(ImagesCountOld,
                                              sizeof(ImageInfo),
                                              ImagesOffsetOld);

            if (End.has_value()) {
                return ADT::Range::FromEnd(ImagesOffsetOld, End.value());
            }

            return std::nullopt;
        }

        [[nodiscard]]
        inline auto getMappingInfoListRange() const noexcept
            -> std::optional<ADT::Range>
        {
            const auto End =
                Utils::MulAddAndCheckOverflow(MappingCount,
                                              sizeof(MappingInfo),
                                              MappingOffset);

            if (End.has_value()) {
                return ADT::Range::FromEnd(MappingOffset, End.value());
            }

            return std::nullopt;
        }

        [[nodiscard]] inline auto imageInfoList() const noexcept {
            const auto Ptr =
                reinterpret_cast<const ImageInfo *>(
                    reinterpret_cast<const uint8_t *>(this) + ImagesOffsetOld);

            return ADT::List(Ptr, ImagesCountOld);
        }

        [[nodiscard]] inline auto mappingInfoList() const noexcept {
            const auto Ptr =
                reinterpret_cast<const MappingInfo *>(
                    reinterpret_cast<const uint8_t *>(this) + MappingOffset);

            return ADT::List(Ptr, MappingCount);
        }

        [[nodiscard]] inline auto imageInfoList() noexcept {
            const auto Ptr =
                reinterpret_cast<ImageInfo *>(
                    reinterpret_cast<uint8_t *>(this) + ImagesOffsetOld);

            return ADT::List(Ptr, ImagesCountOld);
        }

        [[nodiscard]] inline auto mappingInfoList() noexcept {
            const auto Ptr =
                reinterpret_cast<MappingInfo *>(
                    reinterpret_cast<uint8_t *>(this) + MappingOffset);

            return ADT::List(Ptr, MappingCount);
        }

        [[nodiscard]] inline ADT::Range getMappingsRange() const noexcept {
            auto End = ADT::Maximizer<uint64_t>();

            const auto List = mappingInfoList();
            const auto Begin = List.front().Address;

            for (const auto &Mapping : List) {
                End.set(Mapping.Address + Mapping.Size);
            }

            return ADT::Range::FromEnd(Begin, End.value());
        }

        [[nodiscard]] constexpr auto hasSubCacheV1List() const noexcept;
        [[nodiscard]] constexpr auto hasSubCacheList() const noexcept;
    };

    inline bool ImageInfo::isAlias(const HeaderV0 &Header) const noexcept {
        const auto MappingList = Header.mappingInfoList();
        const auto FirstMappingFileOff = MappingList.front().FileOffset;

        return PathFileOffset < FirstMappingFileOff;
    }

    // From dyld v195.5

    struct HeaderV1 : public HeaderV0 {
        uint64_t CodeSignatureOffset;
        uint64_t CodeSignatureSize;

        uint64_t SlideInfoOffset;
        uint64_t SlideInfoSize;

        [[nodiscard]] constexpr auto codeSignatureRange() const noexcept {
            return ADT::Range::FromSize(CodeSignatureOffset, CodeSignatureSize);
        }

        [[nodiscard]] constexpr auto slideInfoRange() const noexcept {
            return ADT::Range::FromSize(SlideInfoOffset, SlideInfoSize);
        }
    };

    // From dyld v239.3

    struct HeaderV2 : public HeaderV1 {
        uint64_t LocalSymbolsOffset;
        uint64_t LocalSymbolsSize;

        uint8_t Uuid[16];

        [[nodiscard]] inline auto localSymbolInfoRange() const noexcept {
            return ADT::Range::FromSize(LocalSymbolsOffset, LocalSymbolsSize);
        }
    };

    // From dyld v360.14

    enum class CacheKind : uint64_t {
        Development,
        Production
    };

    struct HeaderV3 : public HeaderV2 {
        CacheKind Kind;
    };

    // From dyld v421.1

    struct HeaderV4 : public HeaderV3 {
        uint32_t BranchPoolsOffset;
        uint32_t BranchPoolsCount;

        uint64_t DyldInCacheMachHeaderAddr;
        uint64_t DyldInCacheEntryPointAddr;

        uint64_t ImagesTextOffset;
        uint64_t ImagesTextCount;

        [[nodiscard]]
        inline auto getImageTextInfoListRange() const noexcept
            -> std::optional<ADT::Range>
        {
            const auto End =
                Utils::MulAddAndCheckOverflow(sizeof(ImageTextInfo),
                                              ImagesTextCount,
                                              ImagesTextOffset);
            if (End.has_value()) {
                return ADT::Range::FromEnd(ImagesTextOffset, End.value());
            }

            return std::nullopt;
        }

        [[nodiscard]] inline auto getImageTextInfoList() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<ImageTextInfo *>(Map + ImagesTextOffset);

            return ADT::List(Ptr, ImagesTextCount);
        }

        [[nodiscard]] inline auto imageTextInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const ImageTextInfo *>(Map + ImagesTextOffset);

            return ADT::List(Ptr, ImagesTextCount);
        }
    };

    // From dyld v519.2.1

    struct HeaderV5 : public HeaderV4 {
        uint64_t DylibsImageGroupAddr;
        uint64_t DylibsImageGroupSize;

        uint64_t OtherImageGroupAddr;
        uint64_t OtherImageGroupSize;

        uint64_t ProgClosuresAddr;
        uint64_t ProgClosuresSize;

        uint64_t ProgClosuresTrieAddr;
        uint64_t ProgClosuresTrieSize;

        uint32_t Platform;
        uint32_t FormatVersion          : 8,
                 DylibsExpectedOnDisk   : 1,
                 Simulator              : 1,
                 LocallyBuiltCache      : 1, // Only in HeaderV6+
                 BuiltFromChainedFixups : 1, // Only in dyld v732.8 +
                 Padding                : 20;

        uint64_t SharedRegionStart;
        uint64_t SharedRegionSize;
        uint64_t MaxSlide;

        [[nodiscard]] constexpr auto dylibsImageGroupRange() const noexcept {
            return ADT::Range::FromSize(DylibsImageGroupAddr,
                                        DylibsImageGroupSize);
        }

        [[nodiscard]] constexpr auto otherImageGroupRange() const noexcept {
            return ADT::Range::FromSize(OtherImageGroupAddr,
                                        OtherImageGroupSize);
        }

        [[nodiscard]] constexpr auto progClosuresRange() const noexcept {
            return ADT::Range::FromSize(ProgClosuresAddr, ProgClosuresSize);
        }

        [[nodiscard]] constexpr auto progClosuresTrieRange() const noexcept {
            return ADT::Range::FromSize(ProgClosuresTrieAddr,
                                        ProgClosuresTrieSize);
        }

        [[nodiscard]] constexpr auto sharedRegionRange() const noexcept {
            return ADT::Range::FromSize(SharedRegionStart, SharedRegionSize);
        }

        [[nodiscard]] constexpr auto platform() const noexcept {
            return Dyld3::Platform(Platform);
        }

        [[nodiscard]]
        constexpr auto isLocallyBuiltCache() const noexcept
            -> std::optional<bool>
        {
            return isV6() ?
                std::optional(LocallyBuiltCache != 0) : std::nullopt;
        }
    };

    // From dyld v625.13

    struct HeaderV6 : public HeaderV5 {
        uint64_t DylibsImageArrayAddr;
        uint64_t DylibsImageArraySize;

        uint64_t DylibsTrieAddr;
        uint64_t DylibsTrieSize;

        uint64_t OtherImageArrayAddr;
        uint64_t OtherImageArraySize;

        uint64_t OtherTrieAddr;
        uint64_t OtherTrieSize;

        [[nodiscard]] constexpr auto dylibsImageArrayRange() const noexcept {
            return ADT::Range::FromSize(DylibsImageArrayAddr,
                                        DylibsImageArraySize);
        }

        [[nodiscard]] constexpr auto dylibsTrieRange() const noexcept {
            return ADT::Range::FromSize(DylibsTrieAddr, DylibsTrieSize);
        }

        [[nodiscard]] constexpr auto otherImageArrayRange() const noexcept {
            return ADT::Range::FromSize(OtherImageArrayAddr,
                                        OtherImageArraySize);
        }

        [[nodiscard]] constexpr auto otherTrieRange() const noexcept {
            return ADT::Range::FromSize(OtherTrieAddr, OtherTrieSize);
        }
    };

    // From dyld v832.7.1
    struct HeaderV7 : public HeaderV6 {
        uint32_t MappingWithSlideOffset;
        uint32_t MappingWithSlideCount;

        [[nodiscard]]
        constexpr auto getMappingWithSlideInfoRange() const noexcept {
            return ADT::Range::FromSize(MappingWithSlideOffset,
                                        MappingWithSlideCount);
        }

        [[nodiscard]] inline auto mappingWithSlideInfoList() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<MappingWithSlideInfo *>(
                    Map + MappingWithSlideOffset);

            return ADT::List(Ptr, MappingWithSlideCount);
        }

        [[nodiscard]] inline auto mappingWithSlideInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const MappingWithSlideInfo *>(
                    Map + MappingWithSlideOffset);

            return ADT::List(Ptr, MappingWithSlideCount);
        }
    };

    // From dyld v940
    struct HeaderV8 : public HeaderV7 {
        uint64_t DylibsPBLStateArrayAddrUnused;
        uint64_t DylibsPBLSetAddr;
        uint64_t ProgramsPBLSetPoolAddr;
        uint64_t ProgramsPBLSetPoolSize;
        uint64_t ProgramTrieAddr;
        uint32_t ProgramTrieSize;
        uint32_t OsVersion;
        uint32_t AltPlatform;
        uint32_t AltOsVersion;
        uint64_t SwiftOptsOffset;
        uint64_t SwiftOptsSize;
        uint32_t SubCacheArrayOffset;
        uint32_t SubCacheArrayCount;
        uint8_t  SymbolFileUUID[16];
        uint64_t RosettaReadOnlyAddr;
        uint64_t RosettaReadOnlySize;
        uint64_t RosettaReadWriteAddr;
        uint64_t RosettaReadWriteSize;
        uint32_t ImagesOffset;
        uint32_t ImagesCount;

        [[nodiscard]]
        inline auto getImageInfoListRange() const noexcept
            -> std::optional<ADT::Range>
        {
            const auto End =
                Utils::MulAddAndCheckOverflow(ImagesCount, sizeof(ImageInfo),
                                              ImagesOffset);
            if (End.has_value()) {
                return ADT::Range::FromSize(ImagesOffset, End.value());
            }

            return std::nullopt;
        }

        [[nodiscard]] inline auto imageInfoList() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr = reinterpret_cast<ImageInfo *>(Map + ImagesOffset);

            return ADT::List(Ptr, ImagesCount);
        }

        [[nodiscard]] inline auto imageInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const ImageInfo *>(Map + ImagesOffset);

            return ADT::List(Ptr, ImagesCount);
        }

        [[nodiscard]] inline auto programsPBLSetPoolRange() const noexcept {
            return ADT::Range::FromSize(ProgramsPBLSetPoolAddr,
                                        ProgramsPBLSetPoolSize);
        }

        [[nodiscard]] inline auto programTrieRange() const noexcept {
            return ADT::Range::FromSize(ProgramTrieAddr, ProgramTrieSize);
        }

        [[nodiscard]] inline auto swiftOptsRange() const noexcept {
            return ADT::Range::FromSize(SwiftOptsOffset, SwiftOptsSize);
        }

        [[nodiscard]] inline auto subCacheArrayRange() const noexcept {
            return ADT::Range::FromSize(SubCacheArrayOffset,
                                        SubCacheArrayCount);
        }

        [[nodiscard]] inline auto rosettaReadOnlyRange() const noexcept {
            return ADT::Range::FromSize(RosettaReadOnlyAddr,
                                        RosettaReadOnlySize);
        }

        [[nodiscard]] inline auto rosettaReadWriteRange() const noexcept {
            return ADT::Range::FromSize(RosettaReadWriteAddr,
                                        RosettaReadWriteSize);
        }

        [[nodiscard]] constexpr auto osVersion() const noexcept {
            return Dyld3::PackedVersion(OsVersion);
        }

        [[nodiscard]] constexpr auto altOsVersion() const noexcept {
            return Dyld3::PackedVersion(AltOsVersion);
        }

        [[nodiscard]] constexpr auto altPlatform() const noexcept {
            return Dyld3::Platform(AltPlatform);
        }
    };

    // From dyld v1042.1
    struct HeaderV9 : public HeaderV8 {
        uint32_t CacheSubKind;
        uint64_t ObjcOptsOffset;
        uint64_t ObjcOptsSize;
        uint64_t CacheAtlasOffset;
        uint64_t CacheAtlasSize;
        uint64_t DynamicDataOffset;
        uint64_t DynamicDataMaxSize;

        [[nodiscard]] constexpr auto cacheSubKind() const noexcept {
            return CacheKind(CacheSubKind);
        }

        [[nodiscard]] constexpr auto objcOptsRange() const noexcept {
            return ADT::Range::FromSize(ObjcOptsOffset, ObjcOptsSize);
        }

        [[nodiscard]] constexpr auto cacheAtlasRange() const noexcept {
            return ADT::Range::FromSize(CacheAtlasOffset, CacheAtlasSize);
        }

        [[nodiscard]] constexpr auto dynamicDataMaxRange() const noexcept {
            return ADT::Range::FromSize(DynamicDataOffset, DynamicDataMaxSize);
        }
    };

    [[nodiscard]]
    constexpr auto HeaderV0::getVersion() const noexcept -> HeaderVersion {
        auto Version = HeaderVersion::V9;
    #define CHECK_VERSION_LAST(VERS)                                           \
        do {                                                                   \
            if (MappingOffset >= sizeof(VAR_CONCAT(Header, VERS))) {           \
                return HeaderVersion::VERS;                                    \
            }                                                                  \
        } while (false)

    #define CHECK_VERSION(VERS)                                                \
            CHECK_VERSION_LAST(VERS);                                          \
            [[fallthrough]]

        switch (Version) {
            case HeaderVersion::V9:
                CHECK_VERSION(V9);
            case HeaderVersion::V8:
                CHECK_VERSION(V8);
            case HeaderVersion::V7:
                CHECK_VERSION(V7);
            case HeaderVersion::V6:
                CHECK_VERSION(V6);
            case HeaderVersion::V5:
                CHECK_VERSION(V5);
            case HeaderVersion::V4:
                CHECK_VERSION(V4);
            case HeaderVersion::V3:
                CHECK_VERSION(V3);
            case HeaderVersion::V2:
                CHECK_VERSION(V2);
            case HeaderVersion::V1:
                CHECK_VERSION(V1);
            case HeaderVersion::V0:
                CHECK_VERSION_LAST(V0);
        }

    #undef CHECK_VERSION
    #undef CHECK_VERSION_LAST

        return Version;
    }

    [[nodiscard]]
    constexpr auto HeaderV0::imageOffset() const noexcept -> uint32_t {
        return isV8() ?
            static_cast<const HeaderV8 &>(*this).ImagesOffset :
            ImagesOffsetOld;
    }

    [[nodiscard]]
    constexpr auto HeaderV0::imageCount() const noexcept -> uint32_t {
        return isV8() ?
            static_cast<const HeaderV8 &>(*this).ImagesCount :
            ImagesCountOld;
    }

    [[nodiscard]] constexpr auto HeaderV0::isV1() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV1);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV2() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV2);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV3() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV3);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV4() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV4);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV5() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV5);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV6() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV6);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV7() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV7);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV8() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV8);
    }

    [[nodiscard]] constexpr auto HeaderV0::isV9() const noexcept -> bool {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV9);
    }

    [[nodiscard]] constexpr auto HeaderV0::hasSubCacheV1List() const noexcept {
        return isV8();
    }

    [[nodiscard]] constexpr auto HeaderV0::hasSubCacheList() const noexcept {
        return isV9();
    }

    using Header = HeaderV9;
}
