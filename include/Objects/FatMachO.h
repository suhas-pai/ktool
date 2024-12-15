//
//  Objects/FatMachO.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once
#include <expected>

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

        struct Error {
            OpenError Kind;
        };

        ~FatMachO() noexcept override {}

        static auto Open(const ADT::MemoryMap &Map) noexcept
            -> std::expected<FatMachO *, Error>;

        static auto OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept
            -> std::expected<FatMachO *, Error>;

        [[nodiscard]] constexpr auto map() const noexcept {
            return this->Map;
        }

        [[nodiscard]] inline auto header() const noexcept {
            return *this->map().base<::MachO::FatHeader, false>();
        }

        [[nodiscard]] inline auto isBigEndian() const noexcept {
            return this->header().isBigEndian();
        }

        [[nodiscard]] inline auto is64Bit() const noexcept {
            return this->header().is64Bit();
        }

        [[nodiscard]] inline auto archCount() const noexcept {
            return this->header().archCount();
        }

        [[nodiscard]] inline auto archs() const noexcept {
            assert(!this->is64Bit());
            return
                this->map().get<::MachO::FatArch, false>(sizeof(this->header()),
                                                         this->archCount());
        }

        [[nodiscard]] inline auto archList() const noexcept {
            assert(!this->is64Bit());
            return std::span(this->archs(), this->archCount());
        }

        [[nodiscard]] inline auto archs64() const noexcept {
            assert(is64Bit());
            return
                this->map().get<::MachO::FatArch64, false>(
                    sizeof(this->header()),
                    this->archCount());
        }

        [[nodiscard]] inline auto arch64List() const noexcept {
            assert(this->is64Bit());
            return std::span(this->archs64(), this->archCount());
        }
    };
}
