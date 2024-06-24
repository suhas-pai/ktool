//
//  Objects/FatMachO.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once

#include "ADT/PointerOrError.h"
#include "MachO/Fat.h"

#include "Base.h"

namespace Objects {
    struct FatMachO : public Base {
    protected:
        ADT::MemoryMap Map;

        explicit FatMachO(const ADT::MemoryMap &Map) noexcept
        : Base(Kind::FatMachO), Map(Map) {}
    public:
        enum class OpenError {
            None,
            WrongFormat,
            SizeTooSmall,
            TooManyArchitectures,

            ArchOutOfBounds,
            OverlappingArchs,
            ArchsForSameCpu
        };

        ~FatMachO() noexcept override {}

        static auto Open(const ADT::MemoryMap &Map) noexcept
            -> ADT::PointerOrError<FatMachO, OpenError>;

        static auto OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept
            -> ADT::PointerOrError<FatMachO, OpenError>;

        [[nodiscard]] auto
        getArchObjectForCpu(Mach::CpuKind CpuKind,
                            int32_t SubKind) const noexcept
            -> Objects::OpenResult;

        [[nodiscard]]
        auto getArchObjectAtIndex(const uint32_t Index) const noexcept
            -> Objects::OpenResult;

        [[nodiscard]] constexpr auto map() const noexcept {
            return Map;
        }

        [[nodiscard]] inline auto header() const noexcept {
            return *map().base<::MachO::FatHeader, false>();
        }

        [[nodiscard]] inline auto isBigEndian() const noexcept {
            return header().isBigEndian();
        }

        [[nodiscard]] inline auto is64Bit() const noexcept {
            return header().is64Bit();
        }

        [[nodiscard]] inline auto archCount() const noexcept {
            return header().archCount();
        }

        [[nodiscard]] inline auto archs() const noexcept {
            assert(!is64Bit());
            return
                map().get<::MachO::FatArch, false>(sizeof(header()),
                                                   archCount());
        }

        [[nodiscard]] inline auto archList() const noexcept {
            assert(!is64Bit());
            return ADT::List(archs(), archCount());
        }

        [[nodiscard]] inline auto archs64() const noexcept {
            assert(is64Bit());
            return
                map().get<::MachO::FatArch64, false>(sizeof(header()),
                                                     archCount());
        }

        [[nodiscard]] inline auto arch64List() const noexcept {
            assert(is64Bit());
            return ADT::List(archs64(), archCount());
        }
    };
}
