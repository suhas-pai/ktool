//
//  Dyld3/ChainedFixups.h
//  ktool
//
//  Created by suhaspai on 12/15/24.
//

#pragma once

#include "ADT/Range.h"
#include "ADT/SwitchEndian.h"

#include "Utils/Misc.h"

namespace Dyld3 {
    struct ChainedStartsInImage {
        uint32_t SegmentCount;

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wc99-extensions"
        uint32_t SegInfoOffset[];
    #pragma clang diagnostic pop

        [[nodiscard]]
        constexpr auto segmentCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->SegmentCount, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto segmentOffsetsRange(const bool IsBigEndian) const noexcept {
            return ADT::Range::FromSizeAndCount(
                /*Begin=*/0,
                sizeof(uint32_t),
                this->segmentCount(IsBigEndian)).value();
        }

        [[nodiscard]] constexpr auto
        segmentOffset(const uint32_t Index,
                      const bool IsBigEndian) const noexcept
        {
            assert(!
                Utils::IndexOutOfBounds(Index,
                                        this->segmentCount(IsBigEndian)));

            return ADT::SwitchEndianIf(this->SegInfoOffset[Index], IsBigEndian);
        }
    };

    enum class ChainedPointerKind {
        None,
        Arm64e,
        Bits64,
        Bits32,
        Bits32Cache,
        Bits32Firmware,
        Bits64Offset,
        Arm64eKernel,
        Bits64KernelCache,
        Arm64eUserland,
        Arm64eFirmware,
        X86_64KernelCache,
        Arm64eUserland24,
        Arm64eSharedCache,
    };

    struct ChainedStartsInSegment {
        uint32_t Size;
        uint16_t PageSize;
        uint16_t PointerFormat;
        uint64_t SegmentOffset;
        uint32_t MaxValidPointer;
        uint16_t PageCount;
        uint16_t PageStart;

        [[nodiscard]]
        constexpr auto size(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->Size, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto pageSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->PageSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto pointerFormat(const bool IsBigEndian) const noexcept {
            const auto Value =
                ADT::SwitchEndianIf(this->PointerFormat, IsBigEndian);

            return ChainedPointerKind(Value);
        }

        [[nodiscard]]
        constexpr auto segmentOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->SegmentOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto maxValidPointer(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->MaxValidPointer, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto pageCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->PageCount, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto pageStart(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->PageStart, IsBigEndian);
        }
    };

    enum class ChainedPointerStartKind {
        None,
        MultipleStarts,
        LastStart,
    };

    struct ChainedStartsOffsets {
        ChainedPointerStartKind Kind : 32;

        uint32_t StartsCount;
        uint32_t ChainStarts;

        [[nodiscard]]
        constexpr auto startsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->StartsCount, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto chainStarts(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->ChainStarts, IsBigEndian);
        }
    };

    struct ChainedFixupsHeader {
        uint32_t FixupsVersion;
        uint32_t StartsOffset;
        uint32_t ImportsOffset;
        uint32_t SymbolsOffset;
        uint32_t ImportsCount;
        uint32_t ImportsFormat;
        uint32_t SymbolsFormat;

        [[nodiscard]]
        constexpr auto fixupsVersion(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->FixupsVersion, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto startsOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->StartsOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto importsOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->ImportsOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto symbolsOffset(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->SymbolsOffset, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto importsCount(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->ImportsCount, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto importsFormat(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->ImportsFormat, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto symbolsFormat(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(this->SymbolsFormat, IsBigEndian);
        }
    };

    // DYLD_CHAINED_PTR_ARM64E
    struct ChainedPointerArm64eRebase {
        uint64_t Target : 43,
                 High8 : 8,
                 Next : 11,
                 Bind : 1,
                 Auth : 1;

        [[nodiscard]] constexpr auto unpackTarget() const noexcept {
            return static_cast<uint64_t>(this->High8) << 56 | Target;
        }
    };

    // DYLD_CHAINED_PTR_ARM64E
    struct ChainedPointerArm64eBind {
        uint64_t Ordinal : 16,
                 Zero : 16,
                 Addend : 19,
                 Next : 11,
                 Bind : 1,
                 Auth : 1;

        [[nodiscard]] constexpr auto signExtendedAddend() const noexcept {
            const auto Addend = this->Addend;
            if (Addend & 1 << 18) {
                return 0xFFFFFFFFFFFC0000ull | Addend;
            }

            return Addend;
        }
    };

    // DYLD_CHAINED_PTR_ARM64E
    struct ChainedPointerArm64eAuthRebase {
        uint64_t Target : 32,
                 Diversity : 16,
                 AddrDiv : 1,
                 Key : 2,
                 Next : 11,
                 Bind : 1,
                 Auth : 1;
    };

    // DYLD_CHAINED_PTR_ARM64E
    struct ChainedPointerArm64eAuthBind {
        uint64_t Ordinal : 16,
                 Zero : 16,
                 Diversity : 16,
                 AddrDiv : 1,
                 Key : 2,
                 Next : 11,
                 Bind : 1,
                 Auth : 1;
    };

    // DYLD_CHAINED_PTR_64/DYLD_CHAINED_PTR_64_OFFSET
    struct ChainedPointer64Rebase {
        uint64_t Target : 36,
                 High8 : 8,
                 Reserved : 7,
                 Next : 12,
                 Bind : 1;

        [[nodiscard]] constexpr auto unpackedTarget() const noexcept {
            return static_cast<uint64_t>(this->High8) << 56 |
                   static_cast<uint64_t>(Target);
        }
    };

    // DYLD_CHAINED_PTR_ARM64E_USERLAND24
    struct ChainedPointerArm64eBind24 {
        uint64_t Ordinal : 24,
                 Zero : 8,
                 Addend : 19,
                 Next : 11,
                 Bind : 1,
                 Auth : 1;

        [[nodiscard]] constexpr auto signExtendedAddend() const noexcept {
            const auto Addend = this->Addend;
            if (Addend & 1 << 18) {
                return 0xFFFFFFFFFFFC0000ull | Addend;
            }

            return Addend;
        }
    };

    // DYLD_CHAINED_PTR_ARM64E_USERLAND24
    struct ChainedPointerArm64eAuthBind24 {
        uint64_t Ordinal : 24,
                 Zero : 8,
                 Diversity : 16,
                 AddrDiv : 1,
                 Key : 2,
                 Next : 11,
                 Bind : 1,
                 Auth : 1;
    };

    // DYLD_CHAINED_PTR_64/DYLD_CHAINED_PTR_64_OFFSET
    struct ChainedPointer64Bind {
        uint64_t Ordinal : 24,
                 Addend : 8,
                 Reserved : 19,
                 Next : 12,
                 Bind : 1;

        [[nodiscard]] constexpr auto signExtendedAddend() const noexcept {
            const auto Addend = this->Addend;
            const auto Top8Bits = Addend & 0x00007F80000ULL;
            const auto Bottom19Bits = Addend & 0x0000007FFFFULL;
            const auto ExtendedAddend =
                Top8Bits << 13 |
                ((static_cast<uint64_t>(Bottom19Bits << 37) >> 37) &
                 0x00FFFFFFFFFFFFFF);

            return ExtendedAddend;
        }
    };

    // DYLD_CHAINED_PTR_64_KERNEL_CACHE, DYLD_CHAINED_PTR_X86_64_KERNEL_CACHE
    struct ChainedPointer64KernelCache {
        uint64_t Target : 30,
                 CacheLevel : 2,
                 Diversity : 16,
                 AddrDiv : 1,
                 Key : 2,
                 Next : 12,
                 IsAuth : 1;
    };

    // DYLD_CHAINED_PTR_32
    struct ChainedPointer32Rebase {
        uint32_t Target : 26,
                 Next : 5,
                 Bind : 1;
    };

    // DYLD_CHAINED_PTR_32
    struct ChainedPointer32Bind {
        uint32_t Ordinal : 20,
                 Addend : 6,
                 Next : 5,
                 Bind : 1;
    };

    // DYLD_CHAINED_PTR_32_CACHE
    struct ChainedPointer32CacheRebase {
        uint32_t Target : 30,
                 Next : 2;
    };

    // DYLD_CHAINED_PTR_32_FIRMWARE
    struct ChainedPointer32FirmwareRebase {
        uint32_t Target : 26,
                 Next : 6;
    };

    struct ChainedPointer32 {
        union {
            ChainedPointer32Bind Bind;
            ChainedPointer32Rebase Rebase;
            ChainedPointer32CacheRebase CacheRebase;
            ChainedPointer32FirmwareRebase FirmwareRebase;

            uint32_t Value;
        };

        constexpr ChainedPointer32(const uint32_t Value) noexcept
        : Value(Value) {}
    };

    // DYLD_CHAINED_PTR_ARM64E_SHARED_CACHE
    struct ChainedPointerArm64eSharedCacheRebase {
        uint64_t RuntimeOffset : 34,
                 High8 : 8,
                 Unused : 10,
                 Next : 11,
                 Auth : 1;
    };

    // DYLD_CHAINED_PTR_ARM64E_SHARED_CACHE
    struct ChainedPointerArm64eSharedCacheAuthRebase {
        uint64_t RuntimeOffset : 34,
                 Diversity : 16,
                 AddrDiv : 1,
                 KeyIsData : 1,
                 Next : 11,
                 Auth : 1;
    };

    struct ChainedPointer64 {
        union {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wnested-anon-types"
            struct {
                ChainedPointerArm64eRebase Rebase;
                ChainedPointerArm64eBind Bind;
                ChainedPointerArm64eAuthRebase AuthRebase;
                ChainedPointerArm64eAuthBind AuthBind;
                ChainedPointerArm64eBind24 Bind24;
                ChainedPointerArm64eAuthBind24 AuthBind24;
                ChainedPointerArm64eSharedCacheRebase SharedCacheRebase;
                ChainedPointerArm64eSharedCacheAuthRebase SharedCacheAuthRebase;
            } arm64e;
        #pragma clang diagnostic pop

            ChainedPointer64Rebase Rebase;
            ChainedPointer64Bind Bind;
            ChainedPointer64KernelCache KernelCache;

            uint64_t Value;
        };

        constexpr ChainedPointer64(const uint64_t Value) noexcept
        : Value(Value) {}
    };

    struct ChainedFixups {

    };
}
