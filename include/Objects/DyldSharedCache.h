/*
 * Objects/DyldSharedCache.h
 * © suhas pai
 */

#pragma once

#include <filesystem>
#include <expected>
#include <unordered_map>

#include "ADT/FileMap.h"

#include "DyldSharedCache/CacheInfo.h"
#include "DyldSharedCache/Headers.h"

#include "Mach/Machine.h"
#include "Objects/Base.h"

namespace Objects {
    struct DyldSharedCache : public Base {
    public:
        enum class CpuKind {
            i386,
            x86_64,
            x86_64h,
            armv5,
            armv6,
            armv7,
            armv7f,
            armv7k,
            armv7s,
            arm64,
            arm64e,
            arm64_32
        };

        enum class OpenError {
            None,

            WrongFormat,
            UnrecognizedCpuKind,
            SizeTooSmall,

            NoMappings,
            MappingsOutOfBounds,

            FirstMappingFileOffNotZero,

            SubCacheFileDoesNotExist,
            FailedToOpenSubCaches,

            SubCacheListIsInvalid,
            SubCacheHasDiffCpuKind,
            SubCacheHasDiffVersion,

            RecursiveSubCache,
        };

        struct Error {
            OpenError Kind;

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnested-anon-types"
            union {
                struct {
                } SubCacheFileDoesNotExist;
            };
        #pragma GCC diagnostic pop

            explicit Error() noexcept = default;
            explicit Error(const OpenError Kind) noexcept : Kind(Kind) {}
        };

        struct SubCacheProvidedPathInfo {
            std::filesystem::path Path;
            bool ReturnOnSubCacheError : 1 = false;
        };

        using SingleCacheInfo = DyldSharedSingleCacheInfo;
        using SubCacheProvidedPathMap =
            std::unordered_map<std::string, SubCacheProvidedPathInfo>;
    protected:
        SingleCacheInfo Info;
        CpuKind CpuKind;

        struct SubCacheInfo {
            ADT::FileMap *FileMap;
            ADT::FileMap::Prot Prot;

            std::filesystem::path Path;
            SingleCacheInfo Info;

            Error Error;

            explicit
            SubCacheInfo(const std::filesystem::path &Path,
                         const SingleCacheInfo &Info) noexcept
            : FileMap(nullptr), Path(Path), Info(Info) {}

            explicit
            SubCacheInfo(ADT::FileMap *const FileMap,
                         const std::filesystem::path &Path,
                         const SingleCacheInfo &Info) noexcept
            : FileMap(FileMap), Path(Path), Info(Info) {}

            explicit
            SubCacheInfo(ADT::FileMap *const FileMap,
                         std::filesystem::path &&Path,
                         const SingleCacheInfo &Info) noexcept
            : FileMap(FileMap), Path(std::move(Path)), Info(Info) {}

            inline ~SubCacheInfo() noexcept {
                if (FileMap != nullptr) {
                    delete FileMap;
                }
            }
        };

        std::filesystem::path Path;
        mutable std::unordered_map<std::string_view, SubCacheInfo> SubCacheList;

        auto
        FillSubCacheInfo(SubCacheInfo &Info,
                         const std::filesystem::path &Path,
                         ADT::FileMap::Prot Prot) noexcept
            -> Error;

        auto
        CollectSubCaches(
            ADT::FileMap::Prot Prot,
            const SubCacheProvidedPathMap &SubCachePathMap) noexcept
                -> Error;

        auto OpenSubCacheFileMap(const SubCacheInfo &Info) const noexcept
            -> Error;
        auto VerifySubCacheMap(const ADT::MemoryMap &Map) const noexcept
            -> Error;

        explicit
        DyldSharedCache(
            const ADT::MemoryMap &Map,
            const enum CpuKind CpuKind,
            const std::filesystem::path &Path) noexcept
        : Base(Kind::DyldSharedCache), Info(Map, /*VmOffset=*/0, UINT64_MAX),
          CpuKind(CpuKind), Path(Path) {}

        [[nodiscard]]
        auto
        subCacheHasAddress(const SubCacheInfo &SubCacheInfo,
                           const uint64_t Address) const
        {
            const auto &SubCache = SubCacheInfo.Info;
            if (Address < SubCache.FirstAddr) {
                return false;
            }

            if (Address - SubCache.FirstAddr >= SubCache.MaxVmSize) {
                return false;
            }

            return true;
        }
    public:
        static auto
        Open(const ADT::MemoryMap &Map,
             const std::filesystem::path &Path,
             ADT::FileMap::Prot SubCacheProt,
             const SubCacheProvidedPathMap &SubCachePathMap = {}) noexcept
                -> std::expected<DyldSharedCache *, Error>;

        ~DyldSharedCache() noexcept override {}

        [[nodiscard]] constexpr auto map() const noexcept {
            return this->Info.map();
        }

        [[nodiscard]] constexpr auto range() const noexcept {
            return this->Info.range();
        }

        [[nodiscard]] constexpr auto cpuKind() const noexcept {
            return this->CpuKind;
        }

        [[nodiscard]] constexpr auto getMachCpuKindAndSubKind() const noexcept {
            switch (CpuKind) {
                case CpuKind::i386:
                    return std::pair(
                        Mach::CpuKind::IntelX86,
                        static_cast<int32_t>(Mach::Intelx86::x86));
                case CpuKind::x86_64:
                    return std::pair(
                        Mach::CpuKind::IntelX86_64,
                        static_cast<int32_t>(Mach::Intelx86_64::All));
                case CpuKind::x86_64h:
                    return std::pair(
                        Mach::CpuKind::IntelX86_64,
                        static_cast<int32_t>(Mach::Intelx86_64::Haswell));
                case CpuKind::armv5:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v5tej));
                case CpuKind::armv6:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v6));
                case CpuKind::armv7:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7));
                case CpuKind::armv7f:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7f));
                case CpuKind::armv7k:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7k));
                case CpuKind::armv7s:
                    return std::pair(Mach::CpuKind::Arm,
                                     static_cast<int32_t>(Mach::Arm::v7s));
                case CpuKind::arm64:
                    return std::pair(Mach::CpuKind::Arm64,
                                     static_cast<int32_t>(Mach::Arm64::All));
                case CpuKind::arm64e:
                    return std::pair(
                        Mach::CpuKind::Arm64,
                        static_cast<int32_t>(Mach::Arm64::arm64e));
                case CpuKind::arm64_32:
                    return std::pair(
                        Mach::CpuKind::Arm64_32,
                        static_cast<int32_t>(Mach::Arm64_32::All));
            }

            assert(false &&
                   "Objects::getMachCpuKindAndSubKind() couldn't recognize "
                   "internally set CpuKind");
        }

        [[nodiscard]] constexpr auto
        cpuInfoMatches(const Mach::CpuKind CpuKind,
                       const int32_t SubKind) const noexcept
        {
            switch (this->CpuKind) {
                case CpuKind::i386:
                    return CpuKind == Mach::CpuKind::IntelX86 &&
                           SubKind == static_cast<int32_t>(Mach::Intelx86::x86);
                case CpuKind::x86_64:
                    return CpuKind == Mach::CpuKind::IntelX86_64 &&
                           SubKind ==
                                static_cast<int32_t>(Mach::Intelx86_64::All);
                case CpuKind::x86_64h:
                    if (CpuKind == Mach::CpuKind::IntelX86_64 &&
                        SubKind ==
                            static_cast<int32_t>(Mach::Intelx86_64::Haswell))
                    {
                        return true;
                    }

                    if (CpuKind == Mach::CpuKind::IntelX86_64 &&
                        SubKind == static_cast<int32_t>(Mach::Intelx86_64::All))
                    {
                        return true;
                    }

                    return false;
                case CpuKind::armv5:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v5tej);
                case CpuKind::armv6:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v6);
                case CpuKind::armv7:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7);
                case CpuKind::armv7f:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7f);
                case CpuKind::armv7k:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7k);
                case CpuKind::armv7s:
                    return CpuKind == Mach::CpuKind::Arm &&
                           SubKind == static_cast<int32_t>(Mach::Arm::v7s);
                case CpuKind::arm64:
                    return CpuKind == Mach::CpuKind::Arm64 &&
                           SubKind == static_cast<int32_t>(Mach::Arm64::All);
                case CpuKind::arm64e:
                    if (CpuKind != Mach::CpuKind::Arm64) {
                        return false;
                    }

                    return
                        SubKind == static_cast<int32_t>(Mach::Arm64::arm64e) ||
                        SubKind == -2147483646;
                case CpuKind::arm64_32:
                    return CpuKind == Mach::CpuKind::Arm64_32 &&
                           SubKind == static_cast<int32_t>(Mach::Arm64_32::All);
            }

            assert(false &&
                   "Objects::getMachCpuKindAndSubKind() couldn't recognize "
                   "internally set CpuKind");
        }

        [[nodiscard]] inline auto &header() const noexcept {
            return this->Info.header();
        }

        [[nodiscard]] inline auto isAtleastV1() const noexcept {
            return this->Info.isAtleastV1();
        }

        [[nodiscard]] inline auto isAtleastV2() const noexcept {
            return this->Info.isAtleastV2();
        }

        [[nodiscard]] inline auto isAtleastV3() const noexcept {
            return this->Info.isAtleastV3();
        }

        [[nodiscard]] inline auto isAtleastV4() const noexcept {
            return this->Info.isAtleastV4();
        }

        [[nodiscard]] inline auto isAtleastV5() const noexcept {
            return this->Info.isAtleastV5();
        }

        [[nodiscard]] inline auto isAtleastV6() const noexcept {
            return this->Info.isAtleastV6();
        }

        [[nodiscard]] inline auto isAtleastV7() const noexcept {
            return this->Info.isAtleastV7();
        }

        [[nodiscard]] inline auto isAtleastV8() const noexcept {
            return this->Info.isAtleastV8();
        }

        [[nodiscard]] inline auto isAtleastV9() const noexcept {
            return this->Info.isAtleastV9();
        }

        [[nodiscard]] inline auto getVersion() const noexcept {
            return this->Info.getVersion();
        }

        [[nodiscard]]
        inline auto headerV0() const noexcept -> ::DyldSharedCache::HeaderV0 & {
            return this->Info.headerV0();
        }

        [[nodiscard]]
        inline auto headerV1() const noexcept -> ::DyldSharedCache::HeaderV1 & {
            return this->Info.headerV1();
        }

        [[nodiscard]]
        inline auto headerV2() const noexcept -> ::DyldSharedCache::HeaderV2 & {
            return Info.headerV2();
        }

        [[nodiscard]]
        inline auto headerV3() const noexcept -> ::DyldSharedCache::HeaderV3 & {
            return this->Info.headerV3();
        }

        [[nodiscard]]
        inline auto headerV4() const noexcept -> ::DyldSharedCache::HeaderV4 & {
            return this->Info.headerV4();
        }

        [[nodiscard]]
        inline auto headerV5() const noexcept -> ::DyldSharedCache::HeaderV5 & {
            return this->Info.headerV5();
        }

        [[nodiscard]]
        inline auto headerV6() const noexcept -> ::DyldSharedCache::HeaderV6 & {
            return this->Info.headerV6();
        }

        [[nodiscard]]
        inline auto headerV7() const noexcept -> ::DyldSharedCache::HeaderV7 & {
            return this->Info.headerV7();
        }

        [[nodiscard]]
        inline auto headerV8() const noexcept -> ::DyldSharedCache::HeaderV8 & {
            return this->Info.headerV8();
        }

        [[nodiscard]]
        inline auto headerV9() const noexcept -> ::DyldSharedCache::HeaderV9 & {
            return this->Info.headerV9();
        }

        [[nodiscard]] inline auto mappingCount() const noexcept {
            return this->Info.mappingCount();
        }

        [[nodiscard]] inline auto imageCount() const noexcept {
            return this->Info.imageCount();
        }

        [[nodiscard]] inline auto mappingInfoList() const noexcept {
            return this->Info.mappingInfoList();
        }

        [[nodiscard]] inline auto imageInfoList() const noexcept {
            return this->Info.imageInfoList();
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getMapForAddrRange(const ADT::Range &AddrRange,
                           const bool InsideMappings = true) const noexcept
            -> std::expected<std::pair<SingleCacheInfo, ADT::MemoryMap>,
                             Error>
        {
            const uint64_t BaseAddr = AddrRange.front();
            if (AddrRange.empty() || BaseAddr == 0) {
                return std::unexpected(Error(OpenError::None));
            }

            if (const auto Result =
                    this->Info.getMapForAddrRange(AddrRange, InsideMappings))
            {
                return Result.value();
            }

            for (const auto &[Name, SubCache] : this->SubCacheList) {
                if (!subCacheHasAddress(SubCache, AddrRange.front())) {
                    continue;
                }

                if (const auto Error = OpenSubCacheFileMap(SubCache);
                    Error.Kind != OpenError::None)
                {
                    return std::unexpected(Error);
                }

                const auto &SubCacheInfo = SubCache.Info;
                if (const auto Result =
                        SubCacheInfo.getMapForAddrRange(AddrRange,
                                                        InsideMappings))
                {
                    return Result.value();
                }
            }

            return std::unexpected(Error(OpenError::None));
        }

        template <typename T, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getForFileRange(ADT::Range FileRange,
                        const bool InsideMappings = true) const noexcept -> T *
        {
            const uint64_t BaseFile = FileRange.front();
            if (FileRange.empty() || BaseFile == 0) {
                return nullptr;
            }

            if (!this->range().contains(FileRange)) {
                return nullptr;
            }

            if (InsideMappings) {
                for (const auto &Mapping : mappingInfoList()) {
                    if (Mapping.fileRange().contains(FileRange)) {
                        return
                            this->map().get<T, /*Verify=*/false, Size>(
                                FileRange.front());
                    }
                }

                return nullptr;
            }

            return this->map().get<T, /*Verify=*/false, Size>(
                FileRange.front());
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getPtrForAddress(const uint64_t Address,
                         const bool InsideMappings = true,
                         uint64_t *const TotalAvailSize = nullptr,
                         uint64_t *const FileOffsetOut = nullptr) const noexcept
            -> std::expected<std::pair<SingleCacheInfo, T *>, Error>
        {
            if (const auto Result =
                    this->Info.getPtrForAddress<T, Size>(Address,
                                                         InsideMappings,
                                                         TotalAvailSize,
                                                         FileOffsetOut))
            {
                return std::make_pair(Info, Result);
            }

            for (const auto &[Name, SubCacheInfo] : this->SubCacheList) {
                if (!this->subCacheHasAddress(SubCacheInfo, Address)) {
                    continue;
                }

                if (const auto Error = this->OpenSubCacheFileMap(SubCacheInfo);
                    Error.Kind != OpenError::None)
                {
                    return std::unexpected(Error);
                }

                const auto &SubCache = SubCacheInfo.Info;
                if (const auto Result =
                        SubCache.getPtrForAddress<T, Size>(Address,
                                                           InsideMappings,
                                                           TotalAvailSize,
                                                           FileOffsetOut))
                {
                    return std::make_pair(SubCache, Result);
                }
            }

            return std::unexpected(Error(OpenError::None));
        }

        [[nodiscard]] auto subCacheEntryV1InfoList() const noexcept
            -> std::optional<std::span<::DyldSharedCache::SubCacheEntryV1>>;

        [[nodiscard]] auto subCacheEntryInfoList() const noexcept
            -> std::optional<std::span<::DyldSharedCache::SubCacheEntry>>;
    };
}