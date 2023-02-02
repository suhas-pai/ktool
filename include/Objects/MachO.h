//
//  Objects/MachO.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "ADT/MemoryMap.h"
#include "ADT/PointerOrError.h"

#include "MachO/Header.h"
#include "MachO/LoadCommandsMap.h"

#include "Base.h"

namespace Objects {
    struct MachO : public Base {
    public:
        enum class OpenError {
            None,
            WrongFormat,

            SizeTooSmall,
            TooManyLoadCommands
        };
    protected:
        ADT::MemoryMap Map;

        static auto VerifyMap(const ADT::MemoryMap &Map) noexcept -> OpenError;
        static auto
        VerifyLoadCommands(const ADT::MemoryMap &Map) noexcept -> OpenError;

        explicit MachO(const ADT::MemoryMap &Map) noexcept
        : Base(Kind::MachO), Map(Map) {}
    public:

        ~MachO() noexcept override {}

        static auto Open(const ADT::MemoryMap &Map) noexcept ->
            ADT::PointerOrError<MachO, OpenError>;

        [[nodiscard]] constexpr auto map() const noexcept {
            return Map;
        }

        [[nodiscard]] inline auto header() const noexcept {
            return *map().base<::MachO::Header, false>();
        }

        [[nodiscard]] inline auto fileKind() const noexcept {
            return header().fileKind();
        }

        [[nodiscard]] inline auto isBigEndian() const noexcept {
            return header().isBigEndian();
        }

        [[nodiscard]] inline auto is64Bit() const noexcept {
            return header().is64Bit();
        }

        [[nodiscard]] inline auto loadCommandsMap() const noexcept {
            const auto IsBigEndian = isBigEndian();
            const auto LoadCommandsMemoryMap =
                ADT::MemoryMap(map(), header().loadCommandsRange());

            return ::MachO::LoadCommandsMap(LoadCommandsMemoryMap, IsBigEndian);
        }

        [[nodiscard]]
        virtual auto getMapForFileOffsets() const noexcept -> ADT::MemoryMap;
    };
}
