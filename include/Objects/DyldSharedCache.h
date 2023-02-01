/*
 * Objects/DyldSharedCache.h
 * © suhas pai
 */

#pragma once

#include "ADT/PointerOrError.h"

#include "DyldSharedCache/Headers.h"
#include "Mach/Machine.h"

#include "Objects/Base.h"
#include "Objects/DscImage.h"

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
    protected:
        ADT::MemoryMap Map;
        CpuKind CpuKind;

        explicit
        DyldSharedCache(const ADT::MemoryMap &Map,
                        enum CpuKind CpuKind) noexcept
        : Base(Kind::DyldSharedCache), Map(Map), CpuKind(CpuKind) {}
    public:
        enum class OpenError {
            None,

            WrongFormat,
            UnrecognizedCpuKind,
            SizeTooSmall,
        };

        static auto Open(const ADT::MemoryMap &Map) noexcept ->
            ADT::PointerOrError<DyldSharedCache, OpenError>;

        ~DyldSharedCache() noexcept override {}

        [[nodiscard]] constexpr auto map() const noexcept {
            return Map;
        }

        [[nodiscard]] constexpr auto range() const noexcept {
            return ADT::Range::FromSize(0, Map.size());
        }

        [[nodiscard]] constexpr auto cpuKind() const noexcept {
            return CpuKind;
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
                    return CpuKind == Mach::CpuKind::Arm64 &&
                           SubKind == static_cast<int32_t>(Mach::Arm64::arm64e);
                case CpuKind::arm64_32:
                    return CpuKind == Mach::CpuKind::Arm64_32 &&
                           SubKind == static_cast<int32_t>(Mach::Arm64_32::All);
            }

            assert(false &&
                   "Objects::getMachCpuKindAndSubKind() couldn't recognize "
                   "internally set CpuKind");
        }

        [[nodiscard]] inline auto &header() const noexcept {
            return *map().base<::DyldSharedCache::HeaderV9, false>();
        }

        [[nodiscard]] inline auto getVersion() const noexcept {
            return header().getVersion();
        }

        [[nodiscard]]
        inline auto headerV0() const noexcept -> ::DyldSharedCache::HeaderV0 & {
            return header();
        }

        [[nodiscard]]
        inline auto headerV1() const noexcept -> ::DyldSharedCache::HeaderV1 & {
            assert(header().isV1());
            return header();
        }

        [[nodiscard]]
        inline auto headerV2() const noexcept -> ::DyldSharedCache::HeaderV2 & {
            assert(header().isV2());
            return header();
        }

        [[nodiscard]]
        inline auto headerV3() const noexcept -> ::DyldSharedCache::HeaderV3 & {
            assert(header().isV3());
            return header();
        }

        [[nodiscard]]
        inline auto headerV4() const noexcept -> ::DyldSharedCache::HeaderV4 & {
            assert(header().isV4());
            return header();
        }

        [[nodiscard]]
        inline auto headerV5() const noexcept -> ::DyldSharedCache::HeaderV5 & {
            assert(header().isV5());
            return header();
        }

        [[nodiscard]]
        inline auto headerV6() const noexcept -> ::DyldSharedCache::HeaderV6 & {
            assert(header().isV6());
            return header();
        }

        [[nodiscard]]
        inline auto headerV7() const noexcept -> ::DyldSharedCache::HeaderV7 & {
            assert(header().isV7());
            return header();
        }

        [[nodiscard]]
        inline auto headerV8() const noexcept -> ::DyldSharedCache::HeaderV8 & {
            assert(header().isV8());
            return header();
        }

        [[nodiscard]]
        inline auto headerV9() const noexcept -> ::DyldSharedCache::HeaderV9 & {
            assert(header().isV9());
            return header();
        }

        [[nodiscard]] inline auto mappingCount() const noexcept {
            return header().MappingCount;
        }

        [[nodiscard]] inline auto imageCount() const noexcept {
            return header().imageCount();
        }

        [[nodiscard]] inline auto mappingInfoList() const noexcept {
            return header().mappingInfoList();
        }

        [[nodiscard]] inline auto imageInfoList() const noexcept {
            return header().imageInfoList();
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T)>
        [[nodiscard]] inline auto
        getPtrForAddress(
            const uint64_t Address,
            uint64_t *const TotalAvailSize = nullptr,
            uint64_t *const FileOffsetOut = nullptr) const noexcept
            -> T *
        {
            auto AvailSize = uint64_t();
            if (const auto Offset =
                    header().getFileOffsetForAddress(Address, &AvailSize))
            {
                if (AvailSize < Size) {
                    return nullptr;
                }

                if (TotalAvailSize != nullptr) {
                    *TotalAvailSize = AvailSize;
                }

                if (FileOffsetOut != nullptr) {
                    *FileOffsetOut = Offset.value();
                }

                return Map.get<T>(Offset.value());
            }

            return nullptr;
        }

        [[nodiscard]]
        auto getImageObjectAtIndex(uint32_t Index) const noexcept
            -> Objects::OpenResult;
    };
}