//
//  Objects/MachO.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include <expected>
#include "ADT/MemoryMap.h"

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

        struct Error {
            OpenError Kind;
        };
    protected:
        ADT::MemoryMap Map;

        static auto VerifyMap(const ADT::MemoryMap &Map) noexcept -> OpenError;
        static auto
        VerifyLoadCommands(const ADT::MemoryMap &Map) noexcept -> OpenError;

        explicit MachO(const ADT::MemoryMap &Map) noexcept
        : Base(Kind::MachO), Map(Map) {}

        explicit MachO(const ADT::MemoryMap &Map, const enum Kind Kind) noexcept
        : Base(Kind), Map(Map) {}
    public:
        ~MachO() noexcept override {}

        static auto Open(const ADT::MemoryMap &Map) noexcept ->
            std::expected<MachO *, Error>;

        [[nodiscard]] constexpr auto map() const noexcept {
            return this->Map;
        }

        [[nodiscard]] inline auto &header() const noexcept {
            return *this->map().base<::MachO::Header, false>();
        }

        [[nodiscard]] inline auto fileKind() const noexcept {
            return this->header().fileKind();
        }

        [[nodiscard]] inline auto isBigEndian() const noexcept {
            return this->header().isBigEndian();
        }

        [[nodiscard]] inline auto is64Bit() const noexcept {
            return this->header().is64Bit();
        }

        [[nodiscard]] inline auto loadCommandsMap() const noexcept {
            const auto IsBigEndian = this->isBigEndian();
            const auto LoadCommandsMemoryMap =
                ADT::MemoryMap(this->map(), this->header().loadCommandsRange());

            return ::MachO::LoadCommandsMap(LoadCommandsMemoryMap, IsBigEndian);
        }

        [[nodiscard]]
        virtual auto getMapForFileOffsets() const noexcept -> ADT::MemoryMap;
    };
}
