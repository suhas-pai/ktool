/*
 * include/ADT/DyldSharedCacheHeaders.h
 * © suhas pai
 */

#pragma once

#include <span>
#include "ADT/Range.h"

#include "Dyld3/ChainedFixups.h"
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

        [[nodiscard]] constexpr auto addressRange() const noexcept {
            return ADT::Range::FromSize(this->Address, this->Size);
        }

        [[nodiscard]] constexpr auto fileRange() const noexcept {
            return ADT::Range::FromSize(this->FileOffset, this->Size);
        }

        [[nodiscard]] inline auto empty() const noexcept {
            return this->Size == 0;
        }

        [[nodiscard]] constexpr auto maxProt() const noexcept {
            return Mach::VmProt(this->MaxProt);
        }

        [[nodiscard]] constexpr auto initProt() const noexcept {
            return Mach::VmProt(this->InitProt);
        }

        [[nodiscard]] constexpr auto initAndMaxProt() const noexcept {
            return Mach::VmProtInitMax(this->initProt(), this->maxProt());
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddr(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
                -> std::optional<uint64_t>
        {
            if (!this->addressRange().hasLoc(Addr)) {
                return std::nullopt;
            }

            const auto AddrIndex =
                this->addressRange().indexForLoc(Addr, MaxSizeOut);

            if (this->fileRange().hasIndex(AddrIndex)) {
                return this->fileRange().locForIndex(AddrIndex);
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
                ConstData = 1 << 2,
                TextStubs = 1 << 3,
                DynamicConfigData = 1 << 4,
                ReadOnlyData = 1 << 5,
                ConstTproData = 1 << 6,
            };

            [[nodiscard]] inline auto isAuthData() const noexcept {
                return this->has(Masks::AuthData);
            }

            [[nodiscard]] inline auto isDirtyData() const noexcept {
                return this->has(Masks::DirtyData);
            }

            [[nodiscard]] inline auto isConstData() const noexcept {
                return this->has(Masks::ConstData);
            }

            [[nodiscard]] inline auto isTextStubs() const noexcept {
                return this->has(Masks::TextStubs);
            }

            [[nodiscard]] inline auto isDynamicConfigData() const noexcept {
                return this->has(Masks::DynamicConfigData);
            }

            [[nodiscard]] inline auto isReadOnlyData() const noexcept {
                return this->has(Masks::ReadOnlyData);
            }

            [[nodiscard]] inline auto isConstTproData() const noexcept {
                return this->has(Masks::ConstTproData);
            }

            constexpr auto setAuthData(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::AuthData, 0, Value);
                return *this;
            }

            constexpr auto setDirtyData(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::DirtyData, 0, Value);
                return *this;
            }

            constexpr auto setConstData(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::ConstData, 0, Value);
                return *this;
            }

            constexpr auto setTextStubs(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::TextStubs, 0, Value);
                return *this;
            }

            constexpr
            auto setDynamicConfigData(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::DynamicConfigData, 0, Value);
                return *this;
            }

            constexpr auto setReadOnlyData(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::ReadOnlyData, 0, Value);
                return *this;
            }

            constexpr auto setConstTproData(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->setValueForMask(Masks::ConstTproData, 0, Value);
                return *this;
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

        [[nodiscard]] constexpr auto addressRange() const noexcept {
            return ADT::Range::FromSize(this->Address, this->Size);
        }

        [[nodiscard]] constexpr auto fileRange() const noexcept {
            return ADT::Range::FromSize(this->FileOffset, this->Size);
        }

        [[nodiscard]] constexpr auto slideInfoFileRange() const noexcept {
            return ADT::Range::FromSize(this->SlideInfoFileOffset,
                                        this->SlideInfoFileSize);
        }

        [[nodiscard]] inline auto empty() const noexcept {
            return this->Size == 0;
        }

        [[nodiscard]] constexpr auto flags() const noexcept {
            return FlagsStruct(this->Flags);
        }

        [[nodiscard]] constexpr auto maxProt() const noexcept {
            return Mach::VmProt(this->MaxProt);
        }

        [[nodiscard]] constexpr auto initProt() const noexcept {
            return Mach::VmProt(this->InitProt);
        }

        [[nodiscard]] constexpr auto initAndMaxProt() const noexcept {
            return Mach::VmProtInitMax(this->initProt(), this->maxProt());
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddr(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept -> uint64_t
        {
            if (!this->addressRange().hasLoc(Addr)) {
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

        [[nodiscard]] inline
        auto constImageInfoExtraAtIndex(const uint32_t Index) const noexcept
            -> const ImageInfoExtra &
        {
            assert(!Utils::IndexOutOfBounds(Index, ImageExtrasCount));

            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto ImageInfoExtraTable =
                reinterpret_cast<const ImageInfoExtra *>(
                    Map + ImageExtrasOffset);

            return ImageInfoExtraTable[Index];
        }

        [[nodiscard]]
        inline auto imageInfoExtraAtIndex(const uint32_t Index) noexcept
            -> ImageInfoExtra &
        {
            auto &Result =
                const_cast<ImageInfoExtra &>(
                    this->constImageInfoExtraAtIndex(Index));

            return Result;
        }

        [[nodiscard]]
        inline auto imageInfoExtraAtIndex(const uint32_t Index) const noexcept
            -> const ImageInfoExtra &
        {
            return this->constImageInfoExtraAtIndex(Index);
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

        [[nodiscard]] constexpr auto fileSuffix() const noexcept {
            return std::string_view(FileSuffix,
                                    strnlen(FileSuffix, sizeof(FileSuffix)));
        }
    };

    struct DynamicDataHeader {
        char Magic[16];
        uint64_t FsId;
        uint64_t FsObjectId;

        [[nodiscard]] constexpr auto magic() const noexcept {
            return std::string_view(Magic, strnlen(Magic, sizeof(Magic)));
        }
    };

    struct ObjcOptimizationHeader {
        struct FlagsStruct : public ADT::FlagsBase<uint32_t> {
            using ADT::FlagsBase<uint32_t>::FlagsBase;

            enum class Kind : uint32_t {
                IsProduction = 1 << 0,
                NoMissingWeakSuperclasses = 1 << 1,
                LargeSharedCache = 1 << 2
            };

            [[nodiscard]] constexpr
            static auto KindIsValid(const FlagsStruct::Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::IsProduction:
                    case Kind::NoMissingWeakSuperclasses:
                    case Kind::LargeSharedCache:
                        return true;
                }

                return false;
            }

            [[nodiscard]] constexpr
            static auto KindGetString(const FlagsStruct::Kind Kind) noexcept
                -> std::string_view
            {
                switch (Kind) {
                    case Kind::IsProduction:
                        return "Is Production";
                    case Kind::NoMissingWeakSuperclasses:
                        return "No Missing Weak Superclasses";
                    case Kind::LargeSharedCache:
                        return "Large Shared-Cache";
                }

                assert(false &&
                       "DyldSharedCache::ObjcOptimizationHeader::FlagsStruct::"
                       "KindGetString() got unknown Kind");
            }


            [[nodiscard]] constexpr auto isProduction() const noexcept {
                return valueForMask(Kind::IsProduction);
            }

            [[nodiscard]]
            constexpr auto noMissingWeakSuperclasses() const noexcept {
                return valueForMask(Kind::NoMissingWeakSuperclasses);
            }

            [[nodiscard]] constexpr auto largeSharedCache() const noexcept {
                return valueForMask(Kind::LargeSharedCache);
            }

            constexpr auto setIsProduction(const bool Value = true) noexcept
                -> decltype(*this)
            {
                setValueForMask(Kind::IsProduction, 0, Value);
                return *this;
            }

            constexpr auto
            setNoMissingWeakSuperclasses(const bool Value = true) noexcept
                -> decltype(*this)
            {
                setValueForMask(Kind::IsProduction, 0, Value);
                return *this;
            }

            constexpr
            auto setLargeSharedCache(const bool Value = true) noexcept
                -> decltype(*this)
            {
                setValueForMask(Kind::LargeSharedCache, 0, Value);
                return *this;
            }
        };

        uint32_t Version;
        uint32_t Flags;
        uint64_t HeaderInfoReadOnlyCacheOffset;
        uint64_t HeaderInfoReadWriteCacheOffset;
        uint64_t SelectorHashTableCacheOffset;
        uint64_t ClassHashTableCacheOffset;
        uint64_t ProtocolHashTableCacheOffset;
        uint64_t RelativeMethodSelectorBaseAddressOffset;

        [[nodiscard]] constexpr auto flags() const noexcept {
            return FlagsStruct(Flags);
        }
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
            const auto Length = strnlen(this->Magic, sizeof(this->Magic));
            return std::string_view(this->Magic, Length);
        }

        [[nodiscard]] constexpr auto isAtleastV1() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV2() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV3() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV4() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV5() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV6() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV7() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV8() const noexcept -> bool;
        [[nodiscard]] constexpr auto isAtleastV9() const noexcept -> bool;

        [[nodiscard]]
        constexpr auto getVersion() const noexcept -> HeaderVersion;

        [[nodiscard]] constexpr auto imageOffset() const noexcept -> uint32_t;
        [[nodiscard]] constexpr auto imageCount() const noexcept -> uint32_t;

        [[nodiscard]] inline auto imageInfoListRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->imageOffset(),
                                                sizeof(ImageInfo),
                                                this->imageCount());
        }

        [[nodiscard]] inline auto mappingInfoListRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->MappingOffset,
                                                sizeof(MappingInfo),
                                                this->MappingCount);
        }

        [[nodiscard]] constexpr auto hasSubCacheV1Array() const noexcept;
        [[nodiscard]] constexpr auto hasSubCacheArray() const noexcept;
    };

    struct SlideInfoBase {
        uint32_t Version;
    };

    enum class SlideInfoVersion {
        V1 = 1,
        V2,
        V3,
        V4,
        V5
    };

    struct SlideInfoV1 : public SlideInfoBase {
        struct Entry {
            uint8_t Bits[4096 / (8 * 4)];
        };

        uint32_t TocOffset;
        uint32_t TocCount;
        uint32_t EntriesOffset;
        uint32_t EntriesCount;
        uint32_t EntriesSize;
        // uint16_t toc[toc_count];
        // entrybitmap entries[entries_count];

        [[nodiscard]] constexpr auto tocRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->TocOffset,
                                                sizeof(uint16_t),
                                                this->TocCount);
        }

        [[nodiscard]] constexpr auto entriesRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->EntriesOffset,
                                                this->EntriesSize,
                                                this->EntriesCount);
        }

        [[nodiscard]] auto tocSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<uint16_t *>(Map + this->EntriesOffset);

            return std::span<uint16_t>(Ptr, this->EntriesCount);
        }

        [[nodiscard]] constexpr auto entriesSpan() noexcept
            -> std::optional<std::span<Entry>>
        {
            if (this->EntriesSize != sizeof(Entry)) {
                return std::nullopt;
            }

            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<Entry *>(Map + this->EntriesOffset);

            return std::span<Entry>(Ptr, this->EntriesCount);
        }
    };

    struct SlideInfoV2 : public SlideInfoBase {
        uint32_t PageSize;
        uint32_t PageStartsOffset;
        uint32_t PageStartsCount;
        uint32_t PageExtrasOffset;
        uint32_t PageExtrasCount;
        uint64_t DeltaMask;
        uint64_t ValueAdd;

        [[nodiscard]] constexpr auto pageStartsRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PageStartsOffset,
                                                sizeof(uint16_t),
                                                this->PageStartsCount);
        }

        [[nodiscard]] auto pageStartsSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<uint16_t *>(Map + this->PageStartsOffset);

            return std::span<uint16_t>(Ptr, this->PageStartsCount);
        }

        [[nodiscard]] auto pageStartsSpan() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const uint16_t *>(
                    Map + this->PageStartsOffset);

            return std::span<const uint16_t>(Ptr, this->PageStartsCount);
        }

        [[nodiscard]] constexpr auto pageExtrasRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PageExtrasOffset,
                                                sizeof(uint16_t),
                                                this->PageExtrasCount);
        }

        [[nodiscard]] auto pageExtrasSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<uint16_t *>(Map + this->PageExtrasOffset);

            return std::span<uint16_t>(Ptr, this->PageExtrasCount);
        }

        [[nodiscard]] auto pageExtrasSpan() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const uint16_t *>(
                    Map + this->PageExtrasOffset);

            return std::span<const uint16_t>(Ptr, this->PageExtrasCount);
        }
    };

    struct SlideInfoV3 : public SlideInfoBase {
        union PointerV3 {
            uint64_t Raw;
            struct {
                uint64_t PointerValue        : 51,
                         OffsetToNextPointer : 11,
                         Unused              :  2;
            } Plain;

            struct {
                uint64_t OffsetFromSharedCacheBase : 32,
                         DiversityData             : 16,
                         HasAddressDiversity       :  1,
                         Key                       :  2,
                         OffsetToNextPointer       : 11,
                         Unused                    :  1,
                         Authenticated             :  1;
            } Auth;
        };

        uint32_t PageSize;
        uint32_t PageStartsCount;
        uint64_t AuthValueAdd;

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wc99-extensions"
        uint16_t PageStarts[];
    #pragma clang diagnostic pop

        [[nodiscard]] constexpr auto pageStartsSpan() noexcept {
            return std::span(this->PageStarts, this->PageStartsCount);
        }

        [[nodiscard]] constexpr auto pageStartsSpan() const noexcept {
            return std::span(this->PageStarts, this->PageStartsCount);
        }
    };

    struct SlideInfoV4 : public SlideInfoBase {
        uint32_t PageSize;
        uint32_t PageStartsOffset;
        uint32_t PageStartsCount;
        uint32_t PageExtrasOffset;
        uint32_t PageExtrasCount;
        uint64_t DeltaMask;
        uint64_t ValueAdd;

        [[nodiscard]] constexpr auto pageStartsRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PageStartsOffset,
                                                sizeof(uint16_t),
                                                this->PageStartsCount);
        }

        [[nodiscard]] auto pageStartsSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<uint16_t *>(Map + this->PageStartsOffset);

            return std::span<uint16_t>(Ptr, this->PageStartsCount);
        }

        [[nodiscard]] auto pageStartsSpan() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const uint16_t *>(
                    Map + this->PageStartsOffset);

            return std::span<const uint16_t>(Ptr, this->PageStartsCount);
        }

        [[nodiscard]] constexpr auto pageExtrasRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PageExtrasOffset,
                                                sizeof(uint16_t),
                                                this->PageExtrasCount);
        }

        [[nodiscard]] auto pageExtrasSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<uint16_t *>(Map + this->PageExtrasOffset);

            return std::span<uint16_t>(Ptr, this->PageExtrasCount);
        }

        [[nodiscard]] auto pageExtrasSpan() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const uint16_t *>(
                    Map + this->PageExtrasOffset);

            return std::span<const uint16_t>(Ptr, this->PageExtrasCount);
        }
    };

    struct SlideInfoV5 : public SlideInfoBase {
        union PointerV5 {
            uint64_t Raw;

            struct Dyld3::ChainedPointerArm64eSharedCacheRebase Regular;
            struct Dyld3::ChainedPointerArm64eSharedCacheAuthRebase Auth;
        };

        uint32_t PageSize;
        uint32_t PageStartsCount;
        uint64_t ValueAdd;

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wc99-extensions"
        uint16_t PageStarts[];
    #pragma clang diagnostic pop

        [[nodiscard]] constexpr auto pageStartsSpan() const noexcept {
            return std::span(this->PageStarts, this->PageStartsCount);
        }
    };

    // From dyld v195.5
    struct HeaderV1 : public HeaderV0 {
        uint64_t CodeSignatureOffset;
        uint64_t CodeSignatureSize;

        uint64_t SlideInfoOffset;
        uint64_t SlideInfoSize;

        [[nodiscard]] constexpr auto codeSignatureRange() const noexcept {
            return ADT::Range::FromSize(this->CodeSignatureOffset,
                                        this->CodeSignatureSize);
        }

        [[nodiscard]] constexpr auto slideInfoRange() const noexcept {
            return ADT::Range::FromSize(this->SlideInfoOffset,
                                        this->SlideInfoSize);
        }

        [[nodiscard]]
        constexpr auto slideInfoHeader() noexcept -> SlideInfoBase * {
            if (this->SlideInfoSize < sizeof(SlideInfoV1)) {
                return nullptr;
            }

            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<uint8_t *>(Map + this->SlideInfoOffset);

            return reinterpret_cast<SlideInfoBase *>(Ptr);
        }

        [[nodiscard]]
        constexpr auto slideInfoHeader() const noexcept
            -> const SlideInfoBase *
        {
            if (this->SlideInfoSize < sizeof(SlideInfoV1)) {
                return nullptr;
            }

            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const uint8_t *>(Map + this->SlideInfoOffset);

            return reinterpret_cast<const SlideInfoBase *>(Ptr);
        }
    };

    // From dyld v239.3

    struct HeaderV2 : public HeaderV1 {
        uint64_t LocalSymbolsOffset;
        uint64_t LocalSymbolsSize;

        uint8_t Uuid[16];

        [[nodiscard]] inline auto localSymbolInfoRange() const noexcept {
            return ADT::Range::FromSize(this->LocalSymbolsOffset,
                                        this->LocalSymbolsSize);
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

        [[nodiscard]] inline auto imageTextInfoListRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->ImagesTextOffset,
                                                sizeof(ImageTextInfo),
                                                this->ImagesTextCount);
        }

        [[nodiscard]] inline auto imageTextInfoSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<ImageTextInfo *>(Map + this->ImagesTextOffset);

            return std::span(Ptr, this->ImagesTextCount);
        }

        [[nodiscard]] inline auto imageTextInfoSpan() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const ImageTextInfo *>(
                    Map + this->ImagesTextOffset);

            return std::span(Ptr, this->ImagesTextCount);
        }
    };

    // From dyld v519.2.1

    struct HeaderV5 : public HeaderV4 {
        uint64_t PatchInfoAddr;
        uint64_t PatchInfoSize;

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

        [[nodiscard]] constexpr auto patchInfoRange() const noexcept {
            return ADT::Range::FromSize(this->PatchInfoAddr,
                                        this->PatchInfoSize);
        }

        [[nodiscard]] constexpr auto otherImageGroupRange() const noexcept {
            return ADT::Range::FromSize(this->OtherImageGroupAddr,
                                        this->OtherImageGroupSize);
        }

        [[nodiscard]] constexpr auto progClosuresRange() const noexcept {
            return ADT::Range::FromSize(this->ProgClosuresAddr,
                                        this->ProgClosuresSize);
        }

        [[nodiscard]] constexpr auto progClosuresTrieRange() const noexcept {
            return ADT::Range::FromSize(this->ProgClosuresTrieAddr,
                                        this->ProgClosuresTrieSize);
        }

        [[nodiscard]] constexpr auto sharedRegionRange() const noexcept {
            return ADT::Range::FromSize(this->SharedRegionStart,
                                        this->SharedRegionSize);
        }

        [[nodiscard]] constexpr auto platform() const noexcept {
            return Dyld3::Platform(this->Platform);
        }

        [[nodiscard]] constexpr auto isLocallyBuiltCache() const noexcept
            -> std::optional<bool>
        {
            return this->isAtleastV6() ?
                std::optional(this->LocallyBuiltCache != 0) : std::nullopt;
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
            return ADT::Range::FromSize(this->DylibsImageArrayAddr,
                                        this->DylibsImageArraySize);
        }

        [[nodiscard]] constexpr auto dylibsTrieRange() const noexcept {
            return ADT::Range::FromSize(this->DylibsTrieAddr,
                                        this->DylibsTrieSize);
        }

        [[nodiscard]] constexpr auto otherImageArrayRange() const noexcept {
            return ADT::Range::FromSize(this->OtherImageArrayAddr,
                                        this->OtherImageArraySize);
        }

        [[nodiscard]] constexpr auto otherTrieRange() const noexcept {
            return ADT::Range::FromSize(this->OtherTrieAddr,
                                        this->OtherTrieSize);
        }
    };

    // From dyld v832.7.1
    struct HeaderV7 : public HeaderV6 {
        uint32_t MappingWithSlideOffset;
        uint32_t MappingWithSlideCount;

        [[nodiscard]]
        constexpr auto mappingWithSlideInfoRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->MappingWithSlideOffset,
                                                sizeof(MappingWithSlideInfo),
                                                this->MappingWithSlideCount);
        }

        [[nodiscard]] inline auto mappingWithSlideInfoSpan() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<MappingWithSlideInfo *>(
                    Map + this->MappingWithSlideOffset);

            return std::span(Ptr, this->MappingWithSlideCount);
        }

        [[nodiscard]] inline auto mappingWithSlideInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr =
                reinterpret_cast<const MappingWithSlideInfo *>(
                    Map + this->MappingWithSlideOffset);

            return std::span(Ptr, this->MappingWithSlideCount);
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
        constexpr auto imageOffset() const noexcept -> uint32_t {
            return this->ImagesOffset;
        }

        [[nodiscard]]
        constexpr auto imageCount() const noexcept -> uint32_t {
            return this->ImagesCount;
        }

        [[nodiscard]]
        inline auto imageInfoListRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->imageOffset(),
                                                sizeof(ImageInfo),
                                                this->imageCount());
        }

        [[nodiscard]] inline auto programsPBLSetPoolRange() const noexcept {
            return ADT::Range::FromSize(this->ProgramsPBLSetPoolAddr,
                                        this->ProgramsPBLSetPoolSize);
        }

        [[nodiscard]] inline auto programTrieRange() const noexcept {
            return ADT::Range::FromSize(this->ProgramTrieAddr,
                                        this->ProgramTrieSize);
        }

        [[nodiscard]] inline auto swiftOptsRange() const noexcept {
            return ADT::Range::FromSize(this->SwiftOptsOffset,
                                        this->SwiftOptsSize);
        }

        [[nodiscard]] inline auto subCacheArrayRange() const noexcept {
            return ADT::Range::FromSize(this->SubCacheArrayOffset,
                                        this->SubCacheArrayCount);
        }

        [[nodiscard]] inline auto rosettaReadOnlyRange() const noexcept {
            return ADT::Range::FromSize(this->RosettaReadOnlyAddr,
                                        this->RosettaReadOnlySize);
        }

        [[nodiscard]] inline auto rosettaReadWriteRange() const noexcept {
            return ADT::Range::FromSize(this->RosettaReadWriteAddr,
                                        this->RosettaReadWriteSize);
        }

        [[nodiscard]] constexpr auto osVersion() const noexcept {
            return Dyld3::PackedVersion(this->OsVersion);
        }

        [[nodiscard]] constexpr auto altOsVersion() const noexcept {
            return Dyld3::PackedVersion(this->AltOsVersion);
        }

        [[nodiscard]] constexpr auto altPlatform() const noexcept {
            return Dyld3::Platform(this->AltPlatform);
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
            return CacheKind(this->CacheSubKind);
        }

        [[nodiscard]] constexpr auto objcOptsRange() const noexcept {
            return ADT::Range::FromSize(this->ObjcOptsOffset,
                                        this->ObjcOptsSize);
        }

        [[nodiscard]] constexpr auto cacheAtlasRange() const noexcept {
            return ADT::Range::FromSize(this->CacheAtlasOffset,
                                        this->CacheAtlasSize);
        }

        [[nodiscard]] constexpr auto dynamicDataMaxRange() const noexcept {
            return ADT::Range::FromSize(this->DynamicDataOffset,
                                        this->DynamicDataMaxSize);
        }
    };

    [[nodiscard]]
    constexpr auto HeaderV0::getVersion() const noexcept -> HeaderVersion {
        auto Version = HeaderVersion::V9;
    #define CHECK_VERSION_LAST(VERS)                                           \
        do {                                                                   \
            if (this->MappingOffset >= sizeof(VAR_CONCAT(Header, VERS))) {     \
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
        return this->isAtleastV8() ?
            static_cast<const HeaderV8 &>(*this).ImagesOffset :
            this->ImagesOffsetOld;
    }

    [[nodiscard]]
    constexpr auto HeaderV0::imageCount() const noexcept -> uint32_t {
        return this->isAtleastV8() ?
            static_cast<const HeaderV8 &>(*this).ImagesCount :
            this->ImagesCountOld;
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV1() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV1);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV2() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV2);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV3() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV3);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV4() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV4);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV5() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV5);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV6() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV6);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV7() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV7);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV8() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV8);
    }

    [[nodiscard]]
    constexpr auto HeaderV0::isAtleastV9() const noexcept -> bool {
        return this->MappingOffset >= sizeof(DyldSharedCache::HeaderV9);
    }

    [[nodiscard]] constexpr auto HeaderV0::hasSubCacheV1Array() const noexcept {
        return this->isAtleastV8();
    }

    [[nodiscard]] constexpr auto HeaderV0::hasSubCacheArray() const noexcept {
        return this->isAtleastV9();
    }

    using Header = HeaderV9;
}
