//
//  DyldSharedCache/PatchInfo.cpp
//  ktool
//
//  Created by Suhas Pai on 12/16/24.
//

#include "DyldSharedCache/PatchInfo.h"
#include "Utils/Misc.h"

namespace DyldSharedCache {
    auto
    PatchInfo::Create(const struct DeVirtualizer &DeVirtualizer,
                      const DyldSharedCache::HeaderV5 &Header) noexcept
        -> std::optional<PatchInfo>
    {
        const auto PatchHeader =
            DeVirtualizer.getDataAtAddress<DyldSharedCache::PatchInfoV3>(
                Header.PatchInfoAddr);

        if (PatchHeader == nullptr) {
            return std::nullopt;
        }

        auto Version = PatchInfoVersion::V1;
        if (Header.MappingOffset >
                offsetof(DyldSharedCache::HeaderV8, SwiftOptsOffset))
        {
            Version =
                static_cast<PatchInfoVersion>(PatchHeader->PatchTableVersion);
        }

        switch (Version) {
            case PatchInfoVersion::V1: {
                const auto Header =
                    reinterpret_cast<const PatchInfoV1 *>(PatchHeader);

                if (PatchHeader->PatchTableArrayCount != 0) {
                    const auto RangeOpt = Header->patchTableArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (Header->PatchExportArrayCount != 0) {
                    const auto RangeOpt = Header->patchExportArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (Header->PatchLocationArrayCount != 0) {
                    const auto RangeOpt = Header->patchLocationArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                const auto Range = Header->patchExportNamesRange();
                if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                    return std::nullopt;
                }

                break;
            }
            case PatchInfoVersion::V2:
            case PatchInfoVersion::V3:
            case PatchInfoVersion::V4: {
                const auto Header =
                    reinterpret_cast<const PatchInfoV2 *>(PatchHeader);

                if (Header->PatchTableArrayCount != 0) {
                    const auto RangeOpt = Header->patchTableArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (Header->PatchImageExportsArrayCount != 0) {
                    const auto RangeOpt = Header->patchImageExportsArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (Header->PatchClientsArrayCount != 0) {
                    const auto RangeOpt = Header->patchClientsArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (Header->PatchClientExportsArrayCount != 0) {
                    const auto RangeOpt =
                        Header->patchClientExportsArrayRange();

                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (Header->PatchLocationArrayCount != 0) {
                    const auto RangeOpt = Header->patchLocationArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                const auto Range = Header->patchExportNamesRange();
                if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                    return std::nullopt;
                }

                if (Version == PatchInfoVersion::V2) {
                    break;
                }

                const auto HeaderV3 =
                    reinterpret_cast<const PatchInfoV3 *>(PatchHeader);

                if (HeaderV3->GotClientsArrayAddr != 0) {
                    const auto RangeOpt = HeaderV3->gotClientsArrayRange();
                    const auto Range = RangeOpt.value();

                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (HeaderV3->GotClientExportsArrayCount != 0) {
                    const auto RangeOpt =
                        HeaderV3->gotClientExportsArrayRange();

                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                if (HeaderV3->GotLocationArrayCount != 0) {
                    const auto RangeOpt = HeaderV3->gotLocationArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::nullopt;
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::nullopt;
                    }
                }

                break;
            }
        }

        return PatchInfo(DeVirtualizer, Version, *PatchHeader);
    }

    template <typename T>
    [[nodiscard]] static inline auto
    GetPatchDataStructure(const ADT::DeVirtualizer &DeVirtualizer,
                          const std::optional<ADT::Range> RangeOpt) noexcept
        -> std::optional<std::span<T>>
    {
        if (!RangeOpt.has_value()) {
            return std::nullopt;
        }

        const auto Range = RangeOpt.value();
        const auto ListMapOpt =
            DeVirtualizer.getMapForVmRange(Range);

        if (!ListMapOpt.has_value()) {
            return std::nullopt;
        }

        const auto ListMap = ListMapOpt.value();
        return ListMap.span<T>().value();
    }

    auto
    PatchInfo::getListOfExportPatchesV1ForImage(
        const struct DeVirtualizer &DeVirtualizer,
        const DscImage::DeVirtualizer &DscImageDeVirtualizer,
        const uint32_t ImageIndex,
        const ADT::Range VmRange) const noexcept
            -> std::optional<std::unordered_map<uint64_t, PatchLocation>>
    {
        const auto ImagePatchesListOpt =
            GetPatchDataStructure<ImagePatchesV1>(
                DeVirtualizer,
                this->headerV1().patchTableArrayRange());

        if (!ImagePatchesListOpt.has_value()) {
            return std::nullopt;
        }

        const auto ImagePatchExportsListOpt =
            GetPatchDataStructure<PatchableExportV1>(
                DeVirtualizer,
                this->headerV1().patchExportArrayRange());

        if (!ImagePatchExportsListOpt.has_value()) {
            return std::nullopt;
        }

        const auto ImagePatchesList = ImagePatchesListOpt.value();
        const auto ImagePatchExportsList = ImagePatchExportsListOpt.value();

        if (!Utils::IndexOutOfBounds(ImageIndex, ImagePatchesList.size())) {
            return std::nullopt;
        }

        const auto &ImageInfo = ImagePatchesList[ImageIndex];
        if (!Utils::IndexAndCountOutOfBounds(ImageInfo.PatchExportsStartIndex,
                                             ImageInfo.PatchExportsCount,
                                             ImagePatchExportsList.size()))
        {
            return std::nullopt;
        }

        const auto ImagePatchLocationsListOpt =
            GetPatchDataStructure<PatchableLocationV1>(
                DeVirtualizer,
                this->headerV1().patchLocationArrayRange());

        if (!ImagePatchLocationsListOpt.has_value()) {
            return std::nullopt;
        }

        auto Result = std::unordered_map<uint64_t, PatchLocation>();

        const auto ImagePatchLocationsList = ImagePatchLocationsListOpt.value();
        const auto &ImageExportList =
            ImagePatchExportsList.subspan(ImageInfo.PatchExportsStartIndex,
                                          ImageInfo.PatchExportsCount);

        for (const auto &Export : ImageExportList) {
            const auto ExportName =
                DeVirtualizer.getStringAtAddress(Export.ExportNameOffset)
                    .value_or(std::string_view());

            if (!Utils::IndexAndCountOutOfBounds(
                    Export.PatchLocationsStartIndex,
                    Export.PatchLocationsCount,
                    ImagePatchLocationsList.size()))
            {
                continue;
            }

            const auto DscBaseAddress = DeVirtualizer.getBaseAddress();
            const auto &ExportPatchLocations =
                ImagePatchLocationsList.subspan(Export.PatchLocationsStartIndex,
                                                Export.PatchLocationsCount);

            for (const auto &Location : ExportPatchLocations) {
                const auto FullAddr =
                    (DscBaseAddress + Location.CacheOffset) -
                    DscImageDeVirtualizer.getBaseAddress();

                if (!VmRange.hasLoc(FullAddr)) {
                    continue;
                }

                const auto PatchLoc = PatchLocation {
                    .ExportName = ExportName,
                    .IsAuthenticated = Location.Authenticated != 0,
                    .UsesAddressDiversity = Location.UsesAddressDiversity != 0,
                    .IsWeakImport = false,
                    .Key = static_cast<uint8_t>(Location.Key & 0x3)
                };

                Result.emplace(FullAddr, PatchLoc);
            }
        }

        return Result;
    }

    auto PatchInfo::getListOfExportPatchesV3ForImage(
        const struct DeVirtualizer &DeVirtualizer,
        const uint32_t ImageIndex,
        const ADT::Range VmRange) const noexcept
            -> std::optional<std::unordered_map<uint64_t, PatchLocation>>
    {
        const auto ImagePatchesListOpt =
            GetPatchDataStructure<ImagePatchesV2>(
                DeVirtualizer,
                this->headerV2().patchTableArrayRange());

        if (!ImagePatchesListOpt.has_value()) {
            return std::nullopt;
        }

        const auto ImageClientsListOpt =
            GetPatchDataStructure<ImageClientsV2>(
                DeVirtualizer,
                this->headerV2().patchClientsArrayRange());

        if (!ImageClientsListOpt.has_value()) {
            return std::nullopt;
        }

        const auto ImageExportsListOpt =
            GetPatchDataStructure<ImageExportV2>(
                DeVirtualizer,
                this->headerV2().patchImageExportsArrayRange());

        if (!ImageExportsListOpt.has_value()) {
            return std::nullopt;
        }

        const auto ImageClientPatchesListOpt =
            GetPatchDataStructure<PatchableExportV2>(
                DeVirtualizer,
                this->headerV2().patchClientExportsArrayRange());

        if (!ImageClientPatchesListOpt.has_value()) {
            return std::nullopt;
        }

        const auto ImagePatchLocationsListOpt =
            GetPatchDataStructure<PatchableLocation>(
                DeVirtualizer,
                this->headerV2().patchLocationArrayRange());

        if (!ImagePatchLocationsListOpt.has_value()) {
            return std::nullopt;
        }

        const auto &ImageInfo = ImagePatchesListOpt.value()[ImageIndex];
        if (Utils::IndexAndCountOutOfBounds(
                ImageInfo.PatchClientsStartIndex,
                ImageInfo.PatchClientsCount,
                ImageClientsListOpt.value().size()))
        {
            return std::nullopt;
        }

        const auto ImageClientsList =
            ImageClientsListOpt.value().subspan(
                ImageInfo.PatchClientsStartIndex,
                ImageInfo.PatchClientsCount);

        const auto ImageClientPatchesList = ImageClientPatchesListOpt.value();
        const auto ImagePatchLocationsList = ImagePatchLocationsListOpt.value();
        const auto ImageExportsList = ImageExportsListOpt.value();

        auto Result = std::unordered_map<uint64_t, PatchLocation>();
        for (const auto &ImageClient : ImageClientsList) {
            const auto &ImageExports =
                ImageClientPatchesList.subspan(
                    ImageClient.PatchExportsStartIndex,
                    ImageClient.PatchExportsCount);

            switch (this->Version) {
                case PatchInfoVersion::V1:
                    VERIFY_NOT_REACHED();
                case PatchInfoVersion::V2: {
                    for (const auto &Export : ImageExports) {
                        if (Utils::IndexOutOfBounds(Export.ImageExportIndex,
                                                    ImageExportsList.size()))
                        {
                            continue;
                        }

                        const auto &ExportInfo =
                            ImageExportsList[Export.ImageExportIndex];

                        if (!VmRange.hasLoc(ExportInfo.DylibOffsetOfImpl)) {
                            continue;
                        }

                        if (Utils::IndexAndCountOutOfBounds(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount,
                                ImagePatchLocationsList.size()))
                        {
                            continue;
                        }

                        const auto &LocationList =
                            ImagePatchLocationsList.subspan(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount);

                        for (const auto &Location : LocationList) {
                            const auto &V2 = Location.V2;
                            const auto PatchLoc = PatchLocation {
                                .ExportName = std::string_view(),
                                .IsAuthenticated = V2.Authenticated != 0,
                                .UsesAddressDiversity =
                                    V2.UsesAddressDiversity != 0,
                                .IsWeakImport = false,
                                .Key = static_cast<uint8_t>(V2.Key)
                            };

                            Result.emplace(
                                V2.DylibOffsetOfUse + V2.getAddend(), PatchLoc);
                        }
                    }

                    break;
                }
                case PatchInfoVersion::V3: {
                    for (const auto &Export : ImageExports) {
                        if (Utils::IndexAndCountOutOfBounds(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount,
                                ImagePatchLocationsList.size()))
                        {
                            continue;
                        }

                        const auto &LocationList =
                            ImagePatchLocationsList.subspan(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount);

                        for (const auto &Location : LocationList) {
                            const auto &V3 = Location.V3;
                            const auto PatchLoc = PatchLocation {
                                .ExportName = std::string_view(),
                                .IsAuthenticated = V3.Authenticated != 0,
                                .UsesAddressDiversity =
                                    V3.UsesAddressDiversity != 0,
                                .IsWeakImport = false,
                                .Key = static_cast<uint8_t>(V3.Key & 0x3)
                            };

                            Result.emplace(V3.CacheOffsetOfUse + V3.Addend,
                                           PatchLoc);
                        }
                    }

                    break;
                }
                case PatchInfoVersion::V4: {
                    for (const auto &Export : ImageExports) {
                        if (Utils::IndexAndCountOutOfBounds(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount,
                                ImagePatchLocationsList.size()))
                        {
                            continue;
                        }

                        const auto &LocationList =
                            ImagePatchLocationsList.subspan(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount);

                        for (const auto &Location : LocationList) {
                            const auto &V4 = Location.V4;
                            const auto PatchLoc = PatchLocation {
                                .ExportName = std::string_view(),
                                .IsAuthenticated = V4.Auth.Authenticated != 0,
                                .UsesAddressDiversity =
                                    V4.Auth.UsesAddressDiversity != 0,
                                .IsWeakImport = V4.isWeakImport(),
                                .Key = static_cast<uint8_t>(V4.Auth.KeyIsD & 0x3)
                            };

                            Result.emplace(V4.DylibOffsetOfUse + V4.getAddend(),
                                           PatchLoc);
                        }
                    }

                    break;
                }
            }
        }

        return Result;
    }
}