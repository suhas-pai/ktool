//
//  Objects/MachO.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "ADT/PointerOrError.h"
#include "MachO/Header.h"
#include "Base.h"

namespace Objects {
    struct MachO : public Base {
    protected:
        ADT::MemoryMap Map;

        explicit MachO(const ADT::MemoryMap &Map) noexcept
        : Base(Kind::MachO), Map(Map) {}
    public:
        enum class OpenError {
            None,
            WrongFormat,

            SizeTooSmall,
            TooManyLoadCommands
        };

        static auto Open(const ADT::MemoryMap &Map) noexcept ->
            ADT::PointerOrError<MachO, OpenError>;

        [[nodiscard]] constexpr auto getMemoryMap() const noexcept {
            return Map;
        }

        [[nodiscard]] constexpr auto header() const noexcept {
            return *getMemoryMap().base<::MachO::Header>();
        }

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
            return header().isBigEndian();
        }

        [[nodiscard]] constexpr auto is64Bit() const noexcept {
            return header().is64Bit();
        }
    };
}
