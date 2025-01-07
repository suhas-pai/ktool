//
//  MachO/LoadCommandsMap.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once
#include <ranges>

#include "ADT/MemoryMap.h"
#include "LoadCommands.h"

namespace MachO {
    struct LoadCommandsMap {
    protected:
        ADT::MemoryMap Map;
        bool IsBigEndian : 1 = false;
    public:
        constexpr
        LoadCommandsMap(const ADT::MemoryMap Map,
                        const bool IsBigEndian) noexcept
        : Map(Map), IsBigEndian(IsBigEndian) {}

        [[nodiscard]] constexpr auto isBigEndian() const noexcept {
            return this->IsBigEndian;
        }

        struct Iterator {
        protected:
            MachO::LoadCommand *Ptr = nullptr;
            bool IsBigEndian : 1 = false;
        public:
            using value_type = LoadCommand;
            using element_type = LoadCommand;
            using difference_type = ptrdiff_t;
            using pointer = LoadCommand *;
            using reference = LoadCommand &;

            constexpr Iterator() noexcept = default;
            constexpr
            Iterator(MachO::LoadCommand *const Ptr,
                     const bool IsBigEndian) noexcept
            : Ptr(Ptr), IsBigEndian(IsBigEndian) {}

            [[nodiscard]] constexpr auto isBigEndian() const noexcept {
                return this->IsBigEndian;
            }

            [[nodiscard]] constexpr auto kind() const noexcept {
                return this->Ptr->kind(this->IsBigEndian);
            }

            [[nodiscard]] constexpr auto cmdsize() const noexcept {
                return this->Ptr->cmdsize(this->IsBigEndian);
            }

            template <MachO::LoadCommandKind Kind>
            [[nodiscard]] constexpr auto isa() const noexcept -> decltype(auto)
            {
                return MachO::isa<Kind>(this->Ptr, this->IsBigEndian);
            }

            template <MachO::LoadCommandKind Kind>
            [[nodiscard]] constexpr auto cast() const noexcept -> decltype(auto)
            {
                return MachO::cast<Kind>(this->Ptr, this->IsBigEndian);
            }

            template <MachO::LoadCommandKind Kind>
            [[nodiscard]]
            constexpr auto dyn_cast() const noexcept -> decltype(auto) {
                return MachO::dyn_cast<Kind>(this->Ptr, this->IsBigEndian);
            }

            template <LoadCommandDerived T>
            [[nodiscard]] constexpr auto isa() const noexcept -> decltype(auto)
            {
                return MachO::isa<T>(this->Ptr, this->IsBigEndian);
            }

            template <LoadCommandDerived T>
            [[nodiscard]] constexpr auto cast() const noexcept -> decltype(auto)
            {
                return MachO::cast<T>(*this->Ptr, this->IsBigEndian);
            }

            template <LoadCommandDerived T>
            [[nodiscard]]
            constexpr auto dyn_cast() const noexcept -> decltype(auto) {
                return MachO::dyn_cast<T>(this->Ptr, this->IsBigEndian);
            }

            inline auto operator++() noexcept -> decltype(*this) {
                this->Ptr =
                    reinterpret_cast<MachO::LoadCommand *>(
                        reinterpret_cast<uint8_t *>(this->Ptr) +
                        this->cmdsize());

                return *this;
            }

            inline auto operator++(int) noexcept {
                return this->operator++();
            }

            auto
            operator+=(const difference_type Amount) noexcept -> decltype(*this)
            {
                for (auto I = difference_type(); I != Amount; I++) {
                    this->operator++();
                }

                return *this;
            }

            [[nodiscard]] constexpr auto &operator*() const noexcept {
                return *this->Ptr;
            }

            [[nodiscard]] constexpr auto operator->() const noexcept {
                return this->Ptr;
            }

            [[nodiscard]] constexpr
            auto operator<=>(const Iterator &Other) const noexcept = default;

            [[nodiscard]]
            constexpr auto operator==(const Iterator &Other) const noexcept {
                return this->Ptr == Other.Ptr;
            }

            [[nodiscard]]
            constexpr auto operator!=(const Iterator &Other) const noexcept {
                return !this->operator==(Other);
            }

            [[nodiscard]] constexpr
            auto operator==(const LoadCommand *const Other) const noexcept {
                return this->Ptr == Other;
            }

            [[nodiscard]] constexpr
            auto operator!=(const LoadCommand *const Other) const noexcept {
                return !this->operator==(Other);
            }
        };

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(this->Map.base<LoadCommand>(), this->IsBigEndian);
        }

        [[nodiscard]] inline auto end() const noexcept {
            const auto End =
                const_cast<MachO::LoadCommand *>(this->Map.end<LoadCommand>());

            return Iterator(End, this->IsBigEndian);
        }
    };

    template <LoadCommandDerived T>
    const auto LCMapFilterType =
        [](const bool IsBigEndian) noexcept {
            return
                std::views::transform(
                    [IsBigEndian](const LoadCommand &Cmd) noexcept {
                        return dyn_cast<T>(&Cmd, IsBigEndian);
                    }) |
                std::views::filter([](const LoadCommand *const Cmd) noexcept {
                    return Cmd != nullptr;
                });
        };

    template <MachO::LoadCommandKind Kind>
    const auto LCMapFilterKind =
        [](const bool IsBigEndian) noexcept {
            return
                std::views::transform(
                    [IsBigEndian](const LoadCommand &Cmd) noexcept {
                        return dyn_cast<Kind>(&Cmd, IsBigEndian);
                    }) |
                std::views::filter([](const LoadCommand *const Cmd) noexcept {
                    return Cmd != nullptr;
                });
        };

    static_assert(std::forward_iterator<LoadCommandsMap::Iterator>);
    static_assert(std::ranges::forward_range<LoadCommandsMap>);
}
