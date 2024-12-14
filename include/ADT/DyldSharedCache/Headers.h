//
//  ADT/DyldSharedCacheHeaders.h
//  ktool
//
//  Created by Suhas Pai on 7/10/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>

#include "ADT/BasicContiguousList.h"
#include "ADT/LargestIntHelper.h"
#include "ADT/Range.h"

#include "ADT/Dyld3/PackedVersion.h"
#include "ADT/Dyld3/PlatformKind.h"
#include "ADT/Mach-O/MemoryProtections.h"

#define DscHeaderHasField(Header, Field) \
    (Header.MappingOffset >= (offsetof(DyldSharedCache::HeaderV8, Field) + \
     SizeOfField(DyldSharedCache::HeaderV8, Field)))

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
        V8
    };

    struct MappingInfo {
        uint64_t Address;
        uint64_t Size;
        uint64_t FileOffset;
        uint32_t MaxProt;
        uint32_t InitProt;

        [[nodiscard]] inline auto getMaxProt() const noexcept {
            return MachO::MemoryProtections(MaxProt);
        }

        [[nodiscard]] inline auto getInitProt() const noexcept {
            return MachO::MemoryProtections(InitProt);
        }

        [[nodiscard]] inline auto getAddressRange() const noexcept
            -> std::optional<Range>
        {
            return Range::CreateWithSize(Address, Size);
        }

        [[nodiscard]] inline auto getFileRange() const noexcept
            -> std::optional<Range>
        {
            return Range::CreateWithSize(FileOffset, Size);
        }

        [[nodiscard]] inline auto isEmpty() const noexcept {
            return Size == 0;
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddrUnsafe(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
        {
            const auto Delta = Addr - this->Address;
            if (MaxSizeOut != nullptr) {
                *MaxSizeOut = (this->Address + this->Size) - Addr;
            }

            return FileOffset + Delta;
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddr(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
                -> uint64_t
        {
            const auto Range = this->getAddressRange();
            if (!Range || !Range->hasLocation(Addr)) {
                return 0;
            }

            return this->getFileOffsetFromAddrUnsafe(Addr, MaxSizeOut);
        }
    };

    struct MappingWithSlideInfo {
        enum class FlagsEnum : uint64_t {
            AuthData  = 1 << 0,
            DirtyData = 1 << 1,
            ConstData = 1 << 2
        };

        struct FlagsDef : public BasicFlags<FlagsEnum> {
        private:
            using Base = ::BasicFlags<FlagsEnum>;
        public:
            explicit FlagsDef(const uint64_t Flags) : Base(Flags) {}

            [[nodiscard]] inline auto isAuthData() const noexcept {
                return this->hasFlag(FlagsEnum::AuthData);
            }

            [[nodiscard]] inline auto isDirtyData() const noexcept {
                return this->hasFlag(FlagsEnum::DirtyData);
            }

            [[nodiscard]] inline auto isConstData() const noexcept {
                return this->hasFlag(FlagsEnum::ConstData);
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

        [[nodiscard]] inline auto getMaxProt() const noexcept {
            return MachO::MemoryProtections(MaxProt);
        }

        [[nodiscard]] inline auto getInitProt() const noexcept {
            return MachO::MemoryProtections(InitProt);
        }

        [[nodiscard]] inline auto getFlags() const noexcept {
            return FlagsDef(Flags);
        }

        [[nodiscard]] inline auto getAddressRange() const noexcept
            -> std::optional<Range>
        {
            return Range::CreateWithSize(Address, Size);
        }

        [[nodiscard]] inline auto getFileRange() const noexcept
            -> std::optional<Range>
        {
            return Range::CreateWithSize(FileOffset, Size);
        }

        [[nodiscard]] inline auto isEmpty() const noexcept {
            return Size == 0;
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddr(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
                -> uint64_t
        {
            const auto Range = this->getAddressRange();
            if (!Range || !Range->hasLocation(Addr)) {
                return 0;
            }

            return this->getFileOffsetFromAddrUnsafe(Addr, MaxSizeOut);
        }

        [[nodiscard]] inline auto
        getFileOffsetFromAddrUnsafe(
            const uint64_t Addr,
            uint64_t *const MaxSizeOut = nullptr) const noexcept
                -> uint64_t
        {
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
        inline const char *getPath(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<const char *>(Map) + PathFileOffset;
        }

        [[nodiscard]]
        inline bool isAlias(const HeaderV0 &Header) const noexcept;

        [[nodiscard]]
        inline bool isAlias(const uint8_t *const Map) const noexcept {
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

        [[nodiscard]] inline auto &
        getConstImageInfoExtraAtIndex(const uint32_t Index) const noexcept {
            assert(!IndexOutOfBounds(Index, ImageExtrasCount));

            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto ImageInfoExtraTable =
                reinterpret_cast<const ImageInfoExtra *>(
                    Map + ImageExtrasOffset);

            return ImageInfoExtraTable[Index];
        }

        [[nodiscard]]
        inline auto &getImageInfoExtraAtIndex(const uint32_t Index) noexcept {
            auto &Result =
                const_cast<ImageInfoExtra &>(
                    this->getConstImageInfoExtraAtIndex(Index));

            return Result;
        }

        [[nodiscard]] inline
        auto &getImageInfoExtraAtIndex(const uint32_t Index) const noexcept {
            return this->getConstImageInfoExtraAtIndex(Index);
        }
    };

    using ImageInfoList = BasicContiguousList<ImageInfo>;
    using ConstImageInfoList = BasicContiguousList<const ImageInfo>;

    using MappingInfoList = BasicContiguousList<MappingInfo>;
    using ConstMappingInfoList = BasicContiguousList<const MappingInfo>;

    using MappingWithSlideInfoList = BasicContiguousList<MappingWithSlideInfo>;
    using ConstMappingWithSlideInfoList =
        BasicContiguousList<const MappingWithSlideInfo>;

    using ImageTextInfoList = BasicContiguousList<ImageTextInfo>;
    using ConstImageTextInfoList = BasicContiguousList<const ImageTextInfo>;

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

        [[nodiscard]] inline auto isV1() const noexcept -> bool;
        [[nodiscard]] inline auto isV2() const noexcept -> bool;
        [[nodiscard]] inline auto isV3() const noexcept -> bool;
        [[nodiscard]] inline auto isV4() const noexcept -> bool;
        [[nodiscard]] inline auto isV5() const noexcept -> bool;
        [[nodiscard]] inline auto isV6() const noexcept -> bool;
        [[nodiscard]] inline auto isV7() const noexcept -> bool;
        [[nodiscard]] inline auto isV8() const noexcept -> bool;

        [[nodiscard]] inline auto getVersion() const noexcept
            -> HeaderVersion;

        [[nodiscard]] inline auto getImageInfoListRange() const noexcept
            -> std::optional<Range>
            {
            auto End = uint64_t();
            if (DoesMultiplyAndAddOverflow(ImagesCountOld, sizeof(ImageInfo),
                                           ImagesOffsetOld, &End))
            {
                return std::nullopt;
            }

            return Range::CreateWithEnd(ImagesOffsetOld, End);
        }

        [[nodiscard]] inline auto getMappingInfoListRange() const noexcept
            -> std::optional<Range>
        {
            auto End = uint64_t();
            if (DoesMultiplyAndAddOverflow(MappingCount, sizeof(MappingInfo),
                                           MappingOffset, &End))
            {
                return std::nullopt;
            }

            return Range::CreateWithEnd(MappingOffset, End);
        }

        [[nodiscard]] inline auto getImageInfoList() noexcept {
            const auto Ptr =
                reinterpret_cast<uint8_t *>(this) + ImagesOffsetOld;

            return BasicContiguousList<ImageInfo>(Ptr, ImagesCountOld);
        }

        [[nodiscard]] inline auto getConstImageInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + ImagesOffsetOld;

            return ConstImageInfoList(Ptr, ImagesCountOld);
        }

        [[nodiscard]] inline auto getImageInfoList() const noexcept {
            return this->getConstImageInfoList();
        }

        [[nodiscard]] inline auto getMappingInfoList() noexcept {
            const auto Ptr = reinterpret_cast<uint8_t *>(this) + MappingOffset;
            return MappingInfoList(Ptr, MappingCount);
        }

        [[nodiscard]] inline auto getConstMappingInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + MappingOffset;

            return ConstMappingInfoList(Ptr, MappingCount);
        }

        [[nodiscard]] inline auto getMappingInfoList() const noexcept {
            return this->getConstMappingInfoList();
        }

        [[nodiscard]] inline uint64_t
        GetFileOffsetForAddress(uint64_t Addr,
                                uint64_t *MaxSizeOut = nullptr) const noexcept
        {
            if (Addr == 0) {
                return 0;
            }

            for (const auto &Mapping : this->getConstMappingInfoList()) {
                const auto Offset =
                    Mapping.getFileOffsetFromAddr(Addr, MaxSizeOut);

                if (Offset != 0) {
                    return Offset;
                }
            }

            return 0;
        }

        template <typename T = uint8_t>
        [[nodiscard]]
        inline T *GetPtrForAddress(const uint64_t Address) noexcept {
            auto Size = uint64_t();
            if (const auto Offset = GetFileOffsetForAddress(Address, &Size)) {
                if (Size < sizeof(T)) {
                    return nullptr;
                }

                const auto Map = reinterpret_cast<uint8_t *>(this);
                return reinterpret_cast<T *>(Map + Offset);
            }

            return nullptr;
        }

        template <typename T = uint8_t>
        [[nodiscard]] inline
        const T *GetPtrForAddress(const uint64_t Address) const noexcept {
            auto Size = uint64_t();
            if (const auto Offset = GetFileOffsetForAddress(Address, &Size)) {
                if (Size < sizeof(T)) {
                    return nullptr;
                }

                const auto Map = reinterpret_cast<const uint8_t *>(this);
                return reinterpret_cast<const T *>(Map + Offset);
            }

            return nullptr;
        }

        [[nodiscard]] inline auto getMappingsRange() const noexcept {
            auto End = LargestIntHelper();

            const auto List = this->getConstMappingInfoList();
            const auto Begin = List.front().Address;

            for (const auto &Mapping : List) {
                End = Mapping.Address + Mapping.Size;
            }

            return Range::CreateWithEnd(Begin, End);
        }
    };

    inline bool ImageInfo::isAlias(const HeaderV0 &Header) const noexcept {
        const auto MappingList = Header.getConstMappingInfoList();
        const auto FirstMappingFileOff = MappingList.front().FileOffset;

        return PathFileOffset < FirstMappingFileOff;
    }

    // From dyld v195.5

    struct HeaderV1 : public HeaderV0 {
        uint64_t CodeSignatureOffset;
        uint64_t CodeSignatureSize;

        uint64_t SlideInfoOffset;
        uint64_t SlideInfoSize;

        [[nodiscard]] inline auto getCodeSignatureRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(CodeSignatureOffset,
                                              CodeSignatureSize);

            return Range;
        }

        [[nodiscard]] inline auto getSlideInfoRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(SlideInfoOffset, SlideInfoSize);

            return Range;
        }
    };

    // From dyld v239.3

    struct HeaderV2 : public HeaderV1 {
        uint64_t LocalSymbolsOffset;
        uint64_t LocalSymbolsSize;

        uint8_t Uuid[16];

        [[nodiscard]] inline auto getLocalSymbolInfoRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(LocalSymbolsOffset,
                                              LocalSymbolsSize);

            return Range;
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

        uint64_t AccelerateInfoAddr;
        uint64_t AccelerateInfoSize;

        uint64_t ImagesTextOffset;
        uint64_t ImagesTextCount;

        [[nodiscard]] inline auto GetAcceleratorInfo() noexcept
            -> AcceleratorInfo * {
            if (AccelerateInfoAddr == 0 || AccelerateInfoSize == 0) {
                return nullptr;
            }

            return GetPtrForAddress<AcceleratorInfo>(AccelerateInfoAddr);
        }

        [[nodiscard]] inline auto GetAcceleratorInfo() const noexcept
            -> const AcceleratorInfo *
        {
            if (AccelerateInfoAddr == 0 || AccelerateInfoSize == 0) {
                return nullptr;
            }

            return GetPtrForAddress<AcceleratorInfo>(AccelerateInfoAddr);
        }

        [[nodiscard]] inline auto getAcceleratorInfoRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(AccelerateInfoAddr,
                                              AccelerateInfoSize);

            return Range;
        }

        [[nodiscard]] inline auto getImageTextInfoListRange() const noexcept
            -> std::optional<Range>
        {
            auto End = uint64_t();
            if (DoesMultiplyAndAddOverflow(sizeof(ImageTextInfo),
                                           ImagesTextOffset, ImagesTextCount,
                                           &End))
            {
                return std::nullopt;
            }

            return Range::CreateWithEnd(ImagesTextOffset, End);
        }

        [[nodiscard]] inline auto getImageTextInfoList() noexcept {
            const auto Map = reinterpret_cast<uint8_t *>(this);
            const auto Ptr = Map + ImagesTextOffset;

            return ImageTextInfoList(Ptr, ImagesTextCount);
        }

        [[nodiscard]] inline auto getConstImageTextInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + ImagesTextOffset;

            return ConstImageTextInfoList(Ptr, ImagesTextCount);
        }

        [[nodiscard]] inline auto getImageTextInfoList() const noexcept {
            return this->getConstImageTextInfoList();
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

        [[nodiscard]] inline auto getDylibsImageGroupRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(DylibsImageGroupAddr,
                                              DylibsImageGroupSize);

            return Range;
        }

        [[nodiscard]] inline auto getOtherImageGroupRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(OtherImageGroupAddr,
                                              OtherImageGroupSize);

            return Range;
        }

        [[nodiscard]] inline auto getProgClosuresRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(ProgClosuresAddr,
                                              ProgClosuresSize);

            return Range;
        }

        [[nodiscard]] inline auto getProgClosuresTrieRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(ProgClosuresTrieAddr,
                                              ProgClosuresTrieSize);

            return Range;
        }

        [[nodiscard]] inline auto getSharedRegionRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(SharedRegionStart,
                                              SharedRegionSize);

            return Range;
        }

        [[nodiscard]] inline auto getPlatform() const noexcept {
            return Dyld3::PlatformKind(Platform);
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

        [[nodiscard]] inline auto getDylibsImageArrayRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(DylibsImageArrayAddr,
                                              DylibsImageArraySize);

            return Range;
        }

        [[nodiscard]] inline auto getDylibsTrieRange() const noexcept
            -> std::optional<Range>
        {
            return Range::CreateWithSize(DylibsTrieAddr,
                                                 DylibsTrieSize);
        }

        [[nodiscard]] inline auto getOtherImageArrayRange() const noexcept
            -> std::optional<Range>
        {
            const auto Range =
                Range::CreateWithSize(OtherImageArrayAddr,
                                              OtherImageArraySize);

            return Range;
        }

        [[nodiscard]] inline auto getOtherTrieRange() const noexcept
            -> std::optional<Range>
        {
            return Range::CreateWithSize(OtherTrieAddr, OtherTrieSize);
        }
    };

    // From dyld v832.7.1
    struct HeaderV7 : public HeaderV6 {
        uint32_t MappingWithSlideOffset;
        uint32_t MappingWithSlideCount;

        [[nodiscard]] inline auto getMappingWithSlideInfoRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(MappingWithSlideOffset,
                                              MappingWithSlideCount);
        }

        [[nodiscard]] inline auto getMappingWithSlideInfoList() noexcept {
            const auto Ptr =
                reinterpret_cast<uint8_t *>(this) + MappingWithSlideOffset;

            return MappingWithSlideInfoList(Ptr, MappingWithSlideCount);
        }

        [[nodiscard]]
        inline auto getConstMappingWithSlideInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + MappingWithSlideOffset;

            return ConstMappingWithSlideInfoList(Ptr, MappingWithSlideCount);
        }

        [[nodiscard]] inline auto getMappingWithSlideInfoList() const noexcept {
            return this->getConstMappingWithSlideInfoList();
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

        [[nodiscard]] inline auto getImageInfoListRange() const noexcept
            -> std::optional<Range>
        {
            auto End = uint64_t();
            if (DoesMultiplyAndAddOverflow(ImagesCount, sizeof(ImageInfo),
                                           ImagesOffset, &End))
            {
                return std::nullopt;
            }

            return Range::CreateWithEnd(ImagesOffset, End);
        }

        [[nodiscard]] inline auto getImageInfoList() noexcept {
            const auto Ptr = reinterpret_cast<uint8_t *>(this) + ImagesOffset;
            return BasicContiguousList<ImageInfo>(Ptr, ImagesCount);
        }

        [[nodiscard]] inline auto getConstImageInfoList() const noexcept {
            const auto Map = reinterpret_cast<const uint8_t *>(this);
            const auto Ptr = Map + ImagesOffset;

            return ConstImageInfoList(Ptr, ImagesCount);
        }

        [[nodiscard]] inline auto getImageInfoList() const noexcept {
            return this->getConstImageInfoList();
        }

        [[nodiscard]] inline auto getProgramsPBLSetPoolRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(ProgramsPBLSetPoolAddr,
                                              ProgramsPBLSetPoolSize);
        }

        [[nodiscard]] inline auto getProgramTrieRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(ProgramTrieAddr, ProgramTrieSize);
        }

        [[nodiscard]] inline auto getSwiftOptsRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(SwiftOptsOffset, SwiftOptsSize);
        }

        [[nodiscard]] inline auto getSubCacheArrayRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(SubCacheArrayOffset,
                                              SubCacheArrayCount);
        }

        [[nodiscard]] inline auto getRosettaReadOnlyRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(RosettaReadOnlyAddr,
                                              RosettaReadOnlySize);
        }

        [[nodiscard]] inline auto getRosettaReadWriteRange() const noexcept
            -> std::optional<Range>
        {
            return
                Range::CreateWithSize(RosettaReadWriteAddr,
                                              RosettaReadWriteSize);
        }

        [[nodiscard]] constexpr auto getOsVersion() const noexcept {
            return Dyld3::PackedVersion(OsVersion);
        }

        [[nodiscard]] constexpr auto getAltOsVersion() const noexcept {
            return Dyld3::PackedVersion(AltOsVersion);
        }

        [[nodiscard]] constexpr auto getAltPlatform() const noexcept {
            return Dyld3::PlatformKind(AltPlatform);
        }
    };

    auto HeaderV0::getVersion() const noexcept -> HeaderVersion {
        auto Version = HeaderVersion::V0;
        if (MappingOffset >= sizeof(HeaderV8)) {
            Version = HeaderVersion::V8;
        } else if (MappingOffset >= sizeof(HeaderV7)) {
            Version = HeaderVersion::V7;
        } else if (MappingOffset >= sizeof(HeaderV6)) {
            Version = HeaderVersion::V6;
        } else if (MappingOffset >= sizeof(HeaderV5)) {
            Version = HeaderVersion::V5;
        } else if (MappingOffset >= sizeof(HeaderV4)) {
            Version = HeaderVersion::V4;
        } else if (MappingOffset >= sizeof(HeaderV3)) {
            Version = HeaderVersion::V3;
        } else if (MappingOffset >= sizeof(HeaderV2)) {
            Version = HeaderVersion::V2;
        } else if (MappingOffset >= sizeof(HeaderV1)) {
            Version = HeaderVersion::V1;
        }

        return Version;
    }

    [[nodiscard]] inline bool HeaderV0::isV1() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV1);
    }

    [[nodiscard]] inline bool HeaderV0::isV2() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV2);
    }

    [[nodiscard]] inline bool HeaderV0::isV3() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV3);
    }

    [[nodiscard]] inline bool HeaderV0::isV4() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV4);
    }

    [[nodiscard]] inline bool HeaderV0::isV5() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV5);
    }

    [[nodiscard]] inline bool HeaderV0::isV6() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV6);
    }

    [[nodiscard]] inline bool HeaderV0::isV7() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV7);
    }

    [[nodiscard]] inline bool HeaderV0::isV8() const noexcept {
        return MappingOffset >= sizeof(DyldSharedCache::HeaderV8);
    }

    using Header = HeaderV8;
}
