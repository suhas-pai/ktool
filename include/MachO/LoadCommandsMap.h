//
//  MachO/LoadCommandsMap.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <compare>

#include "ADT/MemoryMap.h"
#include "LoadCommands.h"

namespace MachO {
    struct LoadCommandsIterator {
    protected:
        MachO::LoadCommand *Ptr = nullptr;
        bool IsBigEndian : 1 = false;
    public:
        explicit
        LoadCommandsIterator(MachO::LoadCommand *Ptr,
                             bool IsBigEndian) noexcept;

        auto operator++() noexcept -> decltype(*this);
        auto operator++(int) noexcept -> LoadCommandsIterator;
        auto operator+=(int) noexcept -> decltype(*this);

        [[nodiscard]]
        constexpr auto operator*() const noexcept -> MachO::LoadCommand & {
            return *Ptr;
        }

        [[nodiscard]]
        constexpr auto operator->() const noexcept -> MachO::LoadCommand * {
            return Ptr;
        }

        constexpr auto
        operator<=>(const LoadCommandsIterator &Other) const noexcept = default;

        [[nodiscard]] constexpr
        auto operator==(const LoadCommand *const Other) const noexcept -> bool {
            return Ptr == Other;
        }

        [[nodiscard]] constexpr
        auto operator!=(const LoadCommand *const Other) const noexcept -> bool {
            return Ptr != Other;
        }
    };

    struct LoadCommandsMap {
    protected:
        bool IsBigEndian : 1 = false;
        ADT::MemoryMap Map;
    public:
        explicit LoadCommandsMap(ADT::MemoryMap Map, bool IsBigEndian) noexcept;

        auto begin() const noexcept -> LoadCommandsIterator;
        auto end() const noexcept -> const LoadCommand *;
    };
}
