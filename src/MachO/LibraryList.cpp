/*
 * MachO/LibraryList.cpp
 * © suhas pai
 */

#include <ranges>

#include "MachO/LibraryList.h"
#include "MachO/LoadCommands.h"

namespace MachO {
    LibraryList::LibraryList(const MachO::LoadCommandsMap &Map,
                             const bool IsBigEndian) noexcept
    {
        const auto Filter = [IsBigEndian](const auto &LC) noexcept {
            return LC.isSharedLibrary(IsBigEndian);
        };

        std::ranges::for_each(Map | std::views::filter(Filter),
                              [this, IsBigEndian](const auto &LC) {
                                  this->add(cast<DylibCommand>(LC, IsBigEndian),
                                            IsBigEndian);
                              });
    }
}
