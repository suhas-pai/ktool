//
//  DyldSharedCache/PatchInfo.h
//  DyldSharedCache
//
//  Created by Suhas Pai on 12/16/24.
//

#pragma once

#include <span>
#include <unordered_map>
#include <vector>

#include "ADT/Range.h"

#include "DyldSharedCache/DeVirtualizer.h"
#include "DyldSharedCache/Headers.h"

namespace DyldSharedCache {
    struct ImagePatchesV1 {
        uint32_t PatchExportsStartIndex;
        uint32_t PatchExportsCount;
    };

    struct PatchableExportV1 {
        uint32_t CacheOffsetOfImpl;
        uint32_t PatchLocationsStartIndex;
        uint32_t PatchLocationsCount;
        uint32_t ExportNameOffset;
    };

    struct PatchableLocationV1 {
        uint32_t CacheOffset;
        uint64_t High7 : 7,
                 Addend : 5,
                 Authenticated : 1,
                 UsesAddressDiversity : 1,
                 Key : 2,
                 Discriminator : 16;

        [[nodiscard]] constexpr auto getAddend() const noexcept -> uint64_t {
            const auto SignedAddend =
                (static_cast<int64_t>(this->Addend) << 52) >> 52;

            return static_cast<uint64_t>(SignedAddend);
        }
    };

    struct PatchInfoV1 {
        uint64_t PatchTableArrayAddress;
        uint64_t PatchTableArrayCount;
        uint64_t PatchExportArrayAddress;
        uint64_t PatchExportArrayCount;
        uint64_t PatchLocationArrayAddress;
        uint64_t PatchLocationArrayCount;
        uint64_t PatchExportNamesAddress;
        uint64_t PatchExportNamesSize;

        [[nodiscard]] constexpr auto patchTableArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchTableArrayAddress,
                                                sizeof(ImagePatchesV1),
                                                this->PatchTableArrayCount);
        }

        [[nodiscard]] constexpr auto patchExportArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchExportArrayAddress,
                                                sizeof(PatchableExportV1),
                                                this->PatchExportArrayCount);
        }

        [[nodiscard]] constexpr auto patchLocationArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchLocationArrayAddress,
                                                sizeof(PatchableLocationV1),
                                                this->PatchLocationArrayCount);
        }

        [[nodiscard]] constexpr auto patchExportNamesRange() const noexcept {
            return ADT::Range::FromSize(this->PatchExportNamesAddress,
                                        this->PatchExportNamesSize);
        }
    };

    struct ImagePatchesV2 {
        uint32_t PatchClientsStartIndex;
        uint32_t PatchClientsCount;
        uint32_t PatchExportsStartIndex;
        uint32_t PatchExportsCount;
    };

    struct ImageExportV2 {
        uint32_t DylibOffsetOfImpl;
        uint32_t ExportNameOffset : 28;
        uint32_t PatchKind : 4;
    };

    struct ImageClientsV2 {
        uint32_t ClientDylibIndex;
        uint32_t PatchExportsStartIndex;
        uint32_t PatchExportsCount;
    };

    struct PatchableExportV2 {
        uint32_t ImageExportIndex;
        uint32_t PatchLocationsStartIndex;
        uint32_t PatchLocationsCount;
    };

    struct PatchableLocationV2 {
        uint32_t DylibOffsetOfUse;
        uint32_t High7 : 7,
                 Addend : 5,
                 Authenticated : 1,
                 UsesAddressDiversity : 1,
                 Key : 2,
                 Discriminator : 16;

        [[nodiscard]] constexpr auto getAddend() const noexcept -> uint64_t {
            const auto SignedAddend =
                (static_cast<int64_t>(this->Addend) << 52) >> 52;

            return static_cast<uint64_t>(SignedAddend);
        }
    };

    struct PatchInfoV2 {
        uint32_t PatchTableVersion;
        uint32_t PatchLocationVersion;
        uint64_t PatchTableArrayAddr;
        uint64_t PatchTableArrayCount;
        uint64_t PatchImageExportsArrayAddr;
        uint64_t PatchImageExportsArrayCount;
        uint64_t PatchClientsArrayAddr;
        uint64_t PatchClientsArrayCount;
        uint64_t PatchClientExportsArrayAddr;
        uint64_t PatchClientExportsArrayCount;
        uint64_t PatchLocationArrayAddr;
        uint64_t PatchLocationArrayCount;
        uint64_t PatchExportNamesAddr;
        uint64_t PatchExportNamesSize;

        [[nodiscard]] constexpr auto patchTableArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchTableArrayAddr,
                                                sizeof(ImagePatchesV2),
                                                this->PatchTableArrayCount);
        }

        [[nodiscard]]
        constexpr auto patchImageExportsArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(
                this->PatchImageExportsArrayAddr,
                sizeof(ImageExportV2),
                this->PatchImageExportsArrayCount);
        }

        [[nodiscard]] constexpr auto patchClientsArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchClientsArrayAddr,
                                                sizeof(ImageClientsV2),
                                                this->PatchClientsArrayCount);
        }

        [[nodiscard]]
        constexpr auto patchClientExportsArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(
                this->PatchClientExportsArrayAddr,
                sizeof(PatchableExportV2),
                this->PatchClientExportsArrayCount);
        }

        [[nodiscard]] constexpr auto patchLocationArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchLocationArrayAddr,
                                                sizeof(PatchableLocationV2),
                                                this->PatchLocationArrayCount);
        }

        [[nodiscard]] constexpr auto patchExportNamesRange() const noexcept {
            return ADT::Range::FromSize(this->PatchExportNamesAddr,
                                        this->PatchExportNamesSize);
        }
    };

    struct ImageGotClientsV3 {
        uint32_t PatchExportsStartIndex;
        uint32_t PatchExportsCount;
    };

    struct PatchableExportV3 {
        uint32_t ImageExportIndex;
        uint32_t PatchLocationsStartIndex;
        uint32_t PatchLocationsCount;
    };

    struct PatchableLocationV3 {
        uint64_t CacheOffsetOfUse;
        uint32_t High7 : 7,
                 Addend : 5,
                 Authenticated : 1,
                 UsesAddressDiversity : 1,
                 Key : 2,
                 Discriminator : 16;
    };

    struct PatchableLocationV4 {
        struct Authenticated {
            uint32_t Authenticated : 1,
                     High7 : 7,
                     IsWeakImport : 1,
                     Addend : 5,
                     UsesAddressDiversity : 1,
                     KeyIsD : 1,
                     Discriminator : 16;
        };

        struct NotAuthenticated {
            uint32_t Authenticated : 1,
                     High7 : 7,
                     IsWeakImport : 1,
                     Addend : 23;
        };

        uint32_t DylibOffsetOfUse;
        union {
            Authenticated Auth;
            NotAuthenticated Regular;
        };

        [[nodiscard]] constexpr auto getAddend() const noexcept {
            return this->Auth.Authenticated ?
                this->Auth.Addend : this->Regular.Addend;
        }

        [[nodiscard]] constexpr auto isWeakImport() const noexcept -> bool {
            return this->Auth.Authenticated ?
                this->Auth.IsWeakImport : this->Regular.IsWeakImport;
        }
    };

    struct PatchableLocationV4Got {
        uint64_t CacheOffsetOfUse;
        union {
            PatchableLocationV4::Authenticated Auth;
            PatchableLocationV4::NotAuthenticated Regular;
        };

        uint32_t UnusedPadding;

        [[nodiscard]] constexpr auto getAddend() const noexcept {
            return this->Auth.Authenticated ?
                this->Auth.Addend : this->Regular.Addend;
        }

        [[nodiscard]] constexpr auto isWeakImport() const noexcept -> bool {
            return this->Auth.Authenticated ?
                this->Auth.IsWeakImport : this->Regular.IsWeakImport;
        }
    };

    struct PatchInfoV3 : public PatchInfoV2 {
        uint64_t GotClientsArrayAddr;
        uint64_t GotClientsArrayCount;
        uint64_t GotClientExportsArrayAddr;
        uint64_t GotClientExportsArrayCount;
        uint64_t GotLocationArrayAddr;
        uint64_t GotLocationArrayCount;

        [[nodiscard]] constexpr auto gotClientsArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->GotClientsArrayAddr,
                                                sizeof(ImageGotClientsV3),
                                                this->GotClientsArrayCount);
        }

        [[nodiscard]] constexpr auto gotClientExportsArrayRange() const noexcept
        {
            return ADT::Range::FromSizeAndCount(
                this->GotClientExportsArrayAddr,
                sizeof(PatchableExportV3),
                this->GotClientExportsArrayCount);
        }

        [[nodiscard]] constexpr auto gotLocationArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->GotLocationArrayAddr,
                                                sizeof(PatchableLocationV4Got),
                                                this->GotLocationArrayCount);
        }
    };

    union PatchableLocation {
        PatchableLocationV2 V2;
        PatchableLocationV3 V3;
        PatchableLocationV4 V4;
    };

    struct PatchInfoV4 : public PatchInfoV3 {
        [[nodiscard]] constexpr auto patchLocationArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->PatchLocationArrayAddr,
                                                sizeof(PatchableLocationV4),
                                                this->PatchLocationArrayCount);
        }

        [[nodiscard]] constexpr auto gotLocationArrayRange() const noexcept {
            return ADT::Range::FromSizeAndCount(this->GotLocationArrayAddr,
                                                sizeof(PatchableLocationV4),
                                                this->GotLocationArrayCount);
        }
    };

    struct PatchInfoForImage {
    protected:
        std::span<PatchableExportV3> PatchableExportArray;
        std::vector<PatchableLocationV4> PatchableLocationArray;
        std::vector<PatchableLocationV4Got> GotLocations;
    };

    enum class PatchInfoVersion {
        V1 = 1,
        V2,
        V3,
        V4
    };

    struct PatchInfo {
    public:
        struct PatchLocation {
            std::string_view ExportName;

            uint64_t Addend;
            uint64_t FullImplAddress;

            bool IsAuthenticated : 1;
            bool UsesAddressDiversity : 1;
            bool IsWeakImport : 1;

            uint8_t Key : 2;
        };

        struct ParseResult {
            enum class Error {
                None,
                ImageBaseAddressNotFound,

                PatchInfoNotFound,
                PatchTableArrayNotFound,
                PatchExportArrayNotFound,
                PatchLocArrayNotFound,
                PatchExportNamesNotFound,
                PatchClientsArrayNotFound,
                PatchClientExportsArrayNotFound,
                PatchGOTLocArrayNotFound,

                ImageIndexNotFoundInPatchTable,
                ImagePatchExportsOutOfBounds,
                ImageIndexNotFoundOfClientsArray,

                ImageExportInfoNotFound,
                ImageClientHasNoPatches,
            };

            Error Error = Error::None;

            constexpr explicit ParseResult() noexcept = default;
            constexpr ParseResult(const enum Error Error) noexcept
            : Error(Error) {}
        };

        using PatchLocationMap = std::unordered_map<uint64_t, PatchLocation>;
    protected:
        const DyldSharedCache::DeVirtualizer &DeVirtualizer;
        PatchInfoVersion Version;

        uint32_t ImageIndex;
        uint64_t ImageBaseAddress;

        union {
            const DyldSharedCache::PatchInfoV1 *HeaderV1;
            const DyldSharedCache::PatchInfoV2 *HeaderV2;
            const DyldSharedCache::PatchInfoV3 *HeaderV3;
        };

        explicit
        PatchInfo(
            const DyldSharedCache::DeVirtualizer &DeVirtualizer,
            const uint32_t ImageIndex,
            const uint64_t ImageBaseAddress,
            const PatchInfoVersion Version,
            const DyldSharedCache::PatchInfoV3 &Header) noexcept
        : DeVirtualizer(DeVirtualizer), Version(Version),
          ImageIndex(ImageIndex), ImageBaseAddress(ImageBaseAddress),
          HeaderV3(&Header) {}

        [[nodiscard]] auto
        collectListOfExportPatchesV1ForRange(
            PatchLocationMap &Map,
            ADT::Range VmRange) const noexcept -> ParseResult;

        [[nodiscard]] auto
        collectListOfExportPatchesV3ForRange(
            PatchLocationMap &Map,
            ADT::Range VmRange) const noexcept -> ParseResult;
    public:
        [[nodiscard]] static auto
        Create(const struct DeVirtualizer &DeVirtualizer,
               uint32_t ImageIndex,
               uint64_t ImageBaseAddress,
               const DyldSharedCache::HeaderV5 &Header) noexcept
            -> std::expected<PatchInfo, ParseResult>;

        [[nodiscard]] constexpr auto &headerV1() const noexcept {
            return *this->HeaderV1;
        }

        [[nodiscard]] constexpr auto &headerV2() const noexcept {
            return *this->HeaderV2;
        }

        [[nodiscard]] constexpr auto &headerV3() const noexcept {
            return *this->HeaderV3;
        }

        [[nodiscard]] auto
        collectListOfExportPatchesForRange(
            PatchLocationMap &Map,
            ADT::Range VmRange) const noexcept -> ParseResult;
    };
}