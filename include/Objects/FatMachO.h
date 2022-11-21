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
#include "MachO.h"

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

        static auto Open(const ADT::MemoryMap &Map) noexcept ->
            ADT::PointerOrError<FatMachO, OpenError>;
        
        static auto
        OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept ->
            ADT::PointerOrError<FatMachO, OpenError>;

        [[nodiscard]] auto
        getArchObjectForCpu(Mach::CpuKind CpuKind,
                            int32_t SubKind) const noexcept
            -> Objects::OpenResult;

        [[nodiscard]]
        auto getArchObjectAtIndex(const uint32_t Index) const noexcept
            -> Objects::OpenResult;

        [[nodiscard]] constexpr auto getMemoryMap() const noexcept {
            return Map;
        }

        [[nodiscard]] constexpr auto header() const noexcept {
            return *getMemoryMap().base<::MachO::FatHeader, false>();
        }

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
            return header().isBigEndian();
        }

        [[nodiscard]] constexpr auto is64Bit() const noexcept {
            return header().is64Bit();
        }

        [[nodiscard]] constexpr auto archCount() const noexcept {
            return header().archCount();
        }
    };
}
