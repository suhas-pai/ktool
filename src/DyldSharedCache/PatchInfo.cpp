//
//  DyldSharedCache/PatchInfo.cpp
//  ktool
//
//  Created by Suhas Pai on 12/16/24.
//

#include <variant>

#include "DyldSharedCache/PatchInfo.h"
#include "Utils/Misc.h"

namespace DyldSharedCache {
    auto
    PatchInfo::Create(const struct DeVirtualizer &DeVirtualizer,
                      const uint32_t ImageIndex,
                      const uint64_t ImageBaseAddress,
                      const DyldSharedCache::HeaderV5 &Header) noexcept
        -> std::expected<PatchInfo, ParseResult>
    {
        using Error = enum ParseResult::Error;
        const auto PatchHeader =
            DeVirtualizer.getDataAtAddress<DyldSharedCache::PatchInfoV3>(
                Header.PatchInfoAddr);

        if (PatchHeader == nullptr) {
            return std::unexpected(Error::PatchInfoNotFound);
        }

        auto Version = PatchInfoVersion::V1;
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
        if (Header.MappingOffset >
                offsetof(DyldSharedCache::HeaderV8, SwiftOptsOffset))
    #pragma clang diagnostic pop
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
                        return std::unexpected(Error::PatchTableArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchTableArrayNotFound);
                    }
                }

                if (Header->PatchExportArrayCount != 0) {
                    const auto RangeOpt = Header->patchExportArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::unexpected(Error::PatchExportArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchExportArrayNotFound);
                    }
                }

                if (Header->PatchLocationArrayCount != 0) {
                    const auto RangeOpt = Header->patchLocationArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::unexpected(Error::PatchLocArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchLocArrayNotFound);
                    }
                }

                const auto Range = Header->patchExportNamesRange();
                if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                    return std::unexpected(Error::PatchExportNamesNotFound);
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
                        return std::unexpected(Error::PatchTableArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchTableArrayNotFound);
                    }
                }

                if (Header->PatchImageExportsArrayCount != 0) {
                    const auto RangeOpt = Header->patchImageExportsArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::unexpected(Error::PatchExportArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchExportArrayNotFound);
                    }
                }

                if (Header->PatchClientsArrayCount != 0) {
                    const auto RangeOpt = Header->patchClientsArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::unexpected(
                            Error::PatchClientsArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(
                            Error::PatchClientsArrayNotFound);
                    }
                }

                if (Header->PatchClientExportsArrayCount != 0) {
                    const auto RangeOpt =
                        Header->patchClientExportsArrayRange();

                    if (!RangeOpt.has_value()) {
                        return std::unexpected(
                            Error::PatchClientExportsArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(
                            Error::PatchClientExportsArrayNotFound);
                    }
                }

                if (Header->PatchLocationArrayCount != 0) {
                    const auto RangeOpt = Header->patchLocationArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::unexpected(Error::PatchLocArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchLocArrayNotFound);
                    }
                }

                const auto Range = Header->patchExportNamesRange();
                if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchExportNamesNotFound);
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
                        return std::unexpected(
                            Error::PatchClientsArrayNotFound);
                    }
                }

                if (HeaderV3->GotClientExportsArrayCount != 0) {
                    const auto RangeOpt =
                        HeaderV3->gotClientExportsArrayRange();

                    if (!RangeOpt.has_value()) {
                        return std::unexpected(
                            Error::PatchClientExportsArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(
                            Error::PatchClientExportsArrayNotFound);
                    }
                }

                if (HeaderV3->GotLocationArrayCount != 0) {
                    const auto RangeOpt = HeaderV3->gotLocationArrayRange();
                    if (!RangeOpt.has_value()) {
                        return std::unexpected(Error::PatchLocArrayNotFound);
                    }

                    const auto Range = RangeOpt.value();
                    if (!DeVirtualizer.getMapForVmRange(Range).has_value()) {
                        return std::unexpected(Error::PatchLocArrayNotFound);
                    }
                }

                break;
            }
        }

        return PatchInfo(DeVirtualizer,
                         ImageIndex,
                         ImageBaseAddress,
                         Version,
                         *PatchHeader);
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
        const auto ListMapOpt = DeVirtualizer.getMapForVmRange(Range);

        if (!ListMapOpt.has_value()) {
            return std::nullopt;
        }

        return ListMapOpt.value().span<T>().value();
    }

    auto
    PatchInfo::collectListOfExportPatchesV1ForRange(
        PatchLocationMap &Map,
        const ADT::Range VmRange) const noexcept -> ParseResult
    {
        using Error = enum ParseResult::Error;

        const auto ImagePatchesListOpt =
            GetPatchDataStructure<ImagePatchesV1>(
                this->DeVirtualizer,
                this->headerV1().patchTableArrayRange());

        if (!ImagePatchesListOpt.has_value()) {
            return ParseResult(Error::PatchTableArrayNotFound);
        }

        const auto ImagePatchExportsListOpt =
            GetPatchDataStructure<PatchableExportV1>(
                this->DeVirtualizer,
                this->headerV1().patchExportArrayRange());

        if (!ImagePatchExportsListOpt.has_value()) {
            return ParseResult(Error::PatchExportArrayNotFound);
        }

        const auto ImagePatchesList = ImagePatchesListOpt.value();
        const auto ImagePatchExportsList = ImagePatchExportsListOpt.value();

        if (!Utils::IndexOutOfBounds(ImageIndex, ImagePatchesList.size())) {
            return ParseResult(Error::ImageIndexNotFoundInPatchTable);
        }

        const auto &ImageInfo = ImagePatchesList[ImageIndex];
        if (!Utils::IndexAndCountOutOfBounds(ImageInfo.PatchExportsStartIndex,
                                             ImageInfo.PatchExportsCount,
                                             ImagePatchExportsList.size()))
        {
            return ParseResult(Error::ImagePatchExportsOutOfBounds);
        }

        const auto ImagePatchLocationsListOpt =
            GetPatchDataStructure<PatchableLocationV1>(
                this->DeVirtualizer,
                this->headerV1().patchLocationArrayRange());

        if (!ImagePatchLocationsListOpt.has_value()) {
            return ParseResult(Error::PatchLocArrayNotFound);
        }

        const auto ImagePatchLocationsList = ImagePatchLocationsListOpt.value();
        const auto &ImageExportList =
            ImagePatchExportsList.subspan(ImageInfo.PatchExportsStartIndex,
                                          ImageInfo.PatchExportsCount);

        for (const auto &Export : ImageExportList) {
            const auto ExportNameOpt =
                this->DeVirtualizer.getStringAtAddress(Export.ExportNameOffset);

            if (!ExportNameOpt.has_value()) {
                continue;
            }

            const auto ExportName = ExportNameOpt.value();
            if (!Utils::IndexAndCountOutOfBounds(
                    Export.PatchLocationsStartIndex,
                    Export.PatchLocationsCount,
                    ImagePatchLocationsList.size()))
            {
                continue;
            }

            const auto DscBaseAddress = this->DeVirtualizer.getBaseAddress();
            const auto &ExportPatchLocations =
                ImagePatchLocationsList.subspan(Export.PatchLocationsStartIndex,
                                                Export.PatchLocationsCount);

            for (const auto &Location : ExportPatchLocations) {
                const auto FullAddr =
                    (DscBaseAddress + Location.CacheOffset) -
                    this->ImageBaseAddress;

                if (!VmRange.hasLoc(FullAddr)) {
                    continue;
                }

                const auto PatchLoc = PatchLocation {
                    .ExportName = ExportName,
                    .Addend = Location.getAddend(),
                    .FullImplAddress = 0,
                    .IsAuthenticated = Location.Authenticated != 0,
                    .UsesAddressDiversity = Location.UsesAddressDiversity != 0,
                    .IsWeakImport = false,
                    .Key = static_cast<uint8_t>(Location.Key),
                };

                Map.emplace(FullAddr, PatchLoc);
            }
        }

        return ParseResult();
    }

    auto PatchInfo::collectListOfExportPatchesV3ForRange(
        PatchLocationMap &Map,
        const ADT::Range VmRange) const noexcept -> ParseResult
    {
        using Error = enum ParseResult::Error;
        const auto ImagePatchesListOpt =
            GetPatchDataStructure<ImagePatchesV2>(
                this->DeVirtualizer,
                this->headerV2().patchTableArrayRange());

        if (!ImagePatchesListOpt.has_value()) {
            return ParseResult(Error::PatchTableArrayNotFound);
        }

        const auto ImageClientsListOpt =
            GetPatchDataStructure<ImageClientsV2>(
                this->DeVirtualizer,
                this->headerV2().patchClientsArrayRange());

        if (!ImageClientsListOpt.has_value()) {
            return ParseResult(Error::PatchClientsArrayNotFound);
        }

        const auto ImageExportsListOpt =
            GetPatchDataStructure<ImageExportV2>(
                this->DeVirtualizer,
                this->headerV2().patchImageExportsArrayRange());

        if (!ImageExportsListOpt.has_value()) {
            return ParseResult(Error::PatchExportArrayNotFound);
        }

        const auto ImageClientPatchesListOpt =
            GetPatchDataStructure<PatchableExportV2>(
                this->DeVirtualizer,
                this->headerV2().patchClientExportsArrayRange());

        if (!ImageClientPatchesListOpt.has_value()) {
            return ParseResult(Error::PatchClientExportsArrayNotFound);
        }

        const auto ExportNameListOpt =
            GetPatchDataStructure<const char>(
                this->DeVirtualizer,
                this->headerV2().patchExportNamesRange());

        if (!ExportNameListOpt.has_value()) {
            return ParseResult(Error::PatchExportNamesNotFound);
        }

        if (Utils::IndexOutOfBounds(this->ImageIndex,
                                    ImagePatchesListOpt->size()))
        {
            return ParseResult(Error::ImageExportInfoNotFound);
        }

        const auto &ImageInfo = ImagePatchesListOpt.value()[ImageIndex];
        if (Utils::IndexAndCountOutOfBounds(ImageInfo.PatchClientsStartIndex,
                                            ImageInfo.PatchClientsCount,
                                            ImageClientsListOpt.value().size()))
        {
            return ParseResult(Error::ImageIndexNotFoundOfClientsArray);
        }

        auto PatchGOTLocationList =
            std::variant<std::span<PatchableLocationV3>,
                         std::span<PatchableLocationV4Got>>();

        auto PatchLocationList =
            std::variant<std::span<PatchableLocationV2>,
                         std::span<PatchableLocationV3>,
                         std::span<PatchableLocationV4>>();

        switch (this->Version) {
            case PatchInfoVersion::V1:
                VERIFY_NOT_REACHED();
            case PatchInfoVersion::V2: {
                const auto ImagePatchLocationsListOpt =
                    GetPatchDataStructure<PatchableLocationV2>(
                        this->DeVirtualizer,
                        this->headerV2().patchLocationArrayRange());

                if (!ImagePatchLocationsListOpt.has_value()) {
                    return ParseResult(Error::PatchLocArrayNotFound);
                }

                PatchLocationList = ImagePatchLocationsListOpt.value();
                break;
            }
            case PatchInfoVersion::V3: {
                const auto ImagePatchLocationsListOpt =
                    GetPatchDataStructure<PatchableLocationV3>(
                        this->DeVirtualizer,
                        this->headerV3().patchLocationArrayRange());

                if (!ImagePatchLocationsListOpt.has_value()) {
                    return ParseResult(Error::PatchLocArrayNotFound);
                }

                const auto GOTLocationsListOpt =
                    GetPatchDataStructure<PatchableLocationV3>(
                        this->DeVirtualizer,
                        this->headerV3().gotLocationArrayRange());

                if (GOTLocationsListOpt.has_value()) {
                    PatchGOTLocationList = GOTLocationsListOpt.value();
                }

                PatchLocationList = ImagePatchLocationsListOpt.value();
                break;
            }
            case PatchInfoVersion::V4: {
                const auto ImagePatchLocationsListOpt =
                    GetPatchDataStructure<PatchableLocationV4>(
                        this->DeVirtualizer,
                        this->headerV3().patchLocationArrayRange());

                if (!ImagePatchLocationsListOpt.has_value()) {
                    return ParseResult(Error::PatchLocArrayNotFound);
                }

                const auto GOTLocationsListOpt =
                    GetPatchDataStructure<PatchableLocationV4Got>(
                        this->DeVirtualizer,
                        this->headerV3().gotLocationArrayRange());

                if (GOTLocationsListOpt.has_value()) {
                    PatchGOTLocationList = GOTLocationsListOpt.value();
                }

                PatchLocationList = ImagePatchLocationsListOpt.value();
                break;
            }
        }

        assert(!PatchLocationList.valueless_by_exception());

        const auto ImageClientsList =
            ImageClientsListOpt.value().subspan(
                ImageInfo.PatchClientsStartIndex,
                ImageInfo.PatchClientsCount);

        const auto ImageClientPatchesList = ImageClientPatchesListOpt.value();
        const auto ImageExportsList = ImageExportsListOpt.value();
        const auto ExportNameList = ExportNameListOpt.value();

        for (const auto &ImageClient : ImageClientsList) {
            if (ImageClient.ClientDylibIndex != this->ImageIndex) {
                continue;
            }

            const auto &ImageExports =
                ImageClientPatchesList.subspan(
                    ImageClient.PatchExportsStartIndex,
                    ImageClient.PatchExportsCount);

            switch (this->Version) {
                case PatchInfoVersion::V1:
                    VERIFY_NOT_REACHED();
                case PatchInfoVersion::V2: {
                    const auto ImagePatchLocationsList =
                        std::get<std::span<PatchableLocationV2>>(
                            PatchLocationList);

                    for (const auto &Export : ImageExports) {
                        if (Utils::IndexOutOfBounds(Export.ImageExportIndex,
                                                    ImageExportsList.size()))
                        {
                            continue;
                        }

                        const auto &ExportInfo =
                            ImageExportsList[Export.ImageExportIndex];

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

                        for (const auto &Loc : LocationList) {
                            const auto FullOffset =
                                this->ImageBaseAddress + Loc.DylibOffsetOfUse;

                            if (!VmRange.hasLoc(FullOffset)) {
                                continue;
                            }

                            const auto PatchLoc = PatchLocation {
                                .ExportName = std::string_view(),
                                .Addend = Loc.getAddend(),
                                .FullImplAddress =
                                    this->ImageBaseAddress +
                                    ExportInfo.DylibOffsetOfImpl,
                                .IsAuthenticated = Loc.Authenticated != 0,
                                .UsesAddressDiversity =
                                    Loc.UsesAddressDiversity != 0,
                                .IsWeakImport = false,
                                .Key = static_cast<uint8_t>(Loc.Key)
                            };

                            Map.emplace(FullOffset, PatchLoc);
                        }
                    }

                    break;
                }
                case PatchInfoVersion::V3: {
                    const auto ImagePatchLocationsList =
                        std::get<std::span<PatchableLocationV3>>(
                            PatchLocationList);
                    const auto GOTPatchList =
                        std::get<std::span<PatchableLocationV3>>(
                            PatchGOTLocationList);

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

                        if (Utils::IndexOutOfBounds(ExportInfo.ExportNameOffset,
                                                    ExportNameList.size()))
                        {
                            continue;
                        }

                        const auto ExportName =
                            ExportNameList.begin().base() +
                            ExportInfo.ExportNameOffset;

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

                        for (const auto &Loc : LocationList) {
                            const auto PatchLoc = PatchLocation {
                                .ExportName = ExportName,
                                .Addend = Loc.Addend,
                                .FullImplAddress =
                                    this->ImageBaseAddress +
                                    ExportInfo.DylibOffsetOfImpl,
                                .IsAuthenticated = Loc.Authenticated != 0,
                                .UsesAddressDiversity =
                                    Loc.UsesAddressDiversity != 0,
                                .IsWeakImport = false,
                                .Key = static_cast<uint8_t>(Loc.Key)
                            };

                            if (!VmRange.hasLoc(Loc.CacheOffsetOfUse)) {
                                continue;
                            }

                            Map.emplace(Loc.CacheOffsetOfUse, PatchLoc);
                        }

                        if (Utils::IndexAndCountOutOfBounds(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount,
                                GOTPatchList.size()))
                        {
                            continue;
                        }

                        const auto &GOTLocationList =
                            GOTPatchList.subspan(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount);

                        for (const auto &Loc : GOTLocationList) {
                            if (!VmRange.hasLoc(Loc.CacheOffsetOfUse)) {
                                continue;
                            }

                            const auto PatchLoc = PatchLocation {
                                .ExportName = ExportName,
                                .Addend = Loc.Addend,
                                .FullImplAddress =
                                    this->ImageBaseAddress +
                                    ExportInfo.DylibOffsetOfImpl,
                                .IsAuthenticated = Loc.Authenticated != 0,
                                .UsesAddressDiversity =
                                    Loc.UsesAddressDiversity != 0,
                                .IsWeakImport = false,
                                .Key = static_cast<uint8_t>(Loc.Key)
                            };

                            Map.emplace(Loc.CacheOffsetOfUse, PatchLoc);
                        }
                    }

                    break;
                }
                case PatchInfoVersion::V4: {
                    const auto ImagePatchLocationsList =
                        std::get<std::span<PatchableLocationV4>>(
                            PatchLocationList);
                    const auto GOTPatchList =
                        std::get<std::span<PatchableLocationV4Got>>(
                            PatchGOTLocationList);

                    for (const auto &Export : ImageExports) {
                        if (Utils::IndexOutOfBounds(Export.ImageExportIndex,
                                                    ImageExportsList.size()))
                        {
                            continue;
                        }

                        const auto &ExportInfo =
                            ImageExportsList[Export.ImageExportIndex];

                        if (Utils::IndexOutOfBounds(ExportInfo.ExportNameOffset,
                                                    ExportNameList.size()))
                        {
                            continue;
                        }

                        const auto ExportName =
                            ExportNameList.begin().base() +
                            ExportInfo.ExportNameOffset;

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

                        for (const auto &Loc : LocationList) {
                            const auto FullOffset =
                                this->ImageBaseAddress + Loc.DylibOffsetOfUse;

                            if (!VmRange.hasLoc(FullOffset)) {
                                continue;
                            }

                            const auto PatchLoc = PatchLocation {
                                .ExportName = ExportName,
                                .Addend = Loc.getAddend(),
                                .FullImplAddress =
                                    this->ImageBaseAddress +
                                    ExportInfo.DylibOffsetOfImpl,
                                .IsAuthenticated = Loc.Auth.Authenticated != 0,
                                .UsesAddressDiversity =
                                    Loc.Auth.UsesAddressDiversity != 0,
                                .IsWeakImport = Loc.isWeakImport(),
                                .Key = static_cast<uint8_t>(Loc.Auth.KeyIsD),
                            };

                            Map.emplace(FullOffset, PatchLoc);
                        }

                        if (Utils::IndexAndCountOutOfBounds(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount,
                                GOTPatchList.size()))
                        {
                            continue;
                        }

                        const auto &GOTLocationList =
                            GOTPatchList.subspan(
                                Export.PatchLocationsStartIndex,
                                Export.PatchLocationsCount);

                        for (const auto &Loc : GOTLocationList) {
                            if (!VmRange.hasLoc(Loc.CacheOffsetOfUse)) {
                                continue;
                            }

                            const auto PatchLoc = PatchLocation {
                                .ExportName = ExportName,
                                .Addend = Loc.getAddend(),
                                .FullImplAddress =
                                    this->ImageBaseAddress +
                                    ExportInfo.DylibOffsetOfImpl,
                                .IsAuthenticated = Loc.Auth.Authenticated != 0,
                                .UsesAddressDiversity =
                                    Loc.Auth.UsesAddressDiversity != 0,
                                .IsWeakImport = Loc.isWeakImport(),
                                .Key = static_cast<uint8_t>(Loc.Auth.KeyIsD),
                            };

                            Map.emplace(Loc.CacheOffsetOfUse, PatchLoc);
                        }
                    }

                    break;
                }
            }
        }

        return ParseResult();
    }

    auto PatchInfo::collectListOfExportPatchesForRange(
        PatchLocationMap &Map,
        const ADT::Range VmRange) const noexcept-> ParseResult
    {
        switch (this->Version) {
            case PatchInfoVersion::V1:
                return this->collectListOfExportPatchesV1ForRange(Map, VmRange);
            case PatchInfoVersion::V2:
            case PatchInfoVersion::V3:
            case PatchInfoVersion::V4:
                return this->collectListOfExportPatchesV3ForRange(Map, VmRange);
        }
    }
}
