//
//  MachO/LoadCommandsMap.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once
#include <compare>

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
            return IsBigEndian;
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
                return IsBigEndian;
            }

            template <MachO::LoadCommandKind Kind>
            [[nodiscard]] constexpr auto isa() const noexcept -> decltype(auto)
            {
                return MachO::isa<Kind>(Ptr, IsBigEndian);
            }

            template <MachO::LoadCommandKind Kind>
            [[nodiscard]] constexpr auto cast() const noexcept -> decltype(auto)
            {
                return MachO::cast<Kind>(Ptr, IsBigEndian);
            }

            template <MachO::LoadCommandKind Kind>
            [[nodiscard]] constexpr auto dyn_cast() const noexcept
                -> decltype(auto)
            {
                return MachO::dyn_cast<Kind>(Ptr, IsBigEndian);
            }

            template <LoadCommandDerived T>
            [[nodiscard]] constexpr auto isa() const noexcept -> decltype(auto)
            {
                return MachO::isa<T>(Ptr, IsBigEndian);
            }

            template <LoadCommandDerived T>
            [[nodiscard]] constexpr auto cast() const noexcept -> decltype(auto)
            {
                return MachO::cast<T>(Ptr, IsBigEndian);
            }

            template <LoadCommandDerived T>
            [[nodiscard]] constexpr auto dyn_cast() const noexcept
                -> decltype(auto)
            {
                return MachO::dyn_cast<T>(Ptr, IsBigEndian);
            }

            inline auto operator++() noexcept -> decltype(*this) {
                Ptr =
                    reinterpret_cast<MachO::LoadCommand *>(
                        reinterpret_cast<uint8_t *>(Ptr) +
                        Ptr->cmdsize(IsBigEndian));
                return *this;
            }

            auto operator++(int) noexcept {
                return operator++();
            }

            auto
            operator+=(const difference_type Amount) noexcept -> decltype(*this)
            {
                for (auto I = difference_type(); I != Amount; I++) {
                    operator++();
                }

                return *this;
            }

            [[nodiscard]] constexpr auto &operator*() const noexcept {
                return *Ptr;
            }

            [[nodiscard]] constexpr auto operator->() const noexcept {
                return Ptr;
            }

            constexpr auto
            operator<=>(const Iterator &Other) const noexcept = default;

            [[nodiscard]] constexpr
            auto operator==(const Iterator &Other) const noexcept {
                return Ptr == Other.Ptr;
            }

            [[nodiscard]] constexpr
            auto operator!=(const Iterator &Other) const noexcept {
                return !operator==(Other);
            }

            [[nodiscard]] constexpr
            auto operator==(const LoadCommand *const Other) const noexcept {
                return Ptr == Other;
            }

            [[nodiscard]] constexpr
            auto operator!=(const LoadCommand *const Other) const noexcept {
                return Ptr != Other;
            }
        };

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Map.base<LoadCommand>(), IsBigEndian);
        }

        [[nodiscard]] inline auto end() const noexcept {
            const auto End =
                const_cast<MachO::LoadCommand *>(Map.end<LoadCommand>());

            return Iterator(End, IsBigEndian);
        }
    };

    static_assert(std::forward_iterator<LoadCommandsMap::Iterator>);
}
