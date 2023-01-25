/*
 * MachO/LibraryList.cpp
 * © suhas pai
 */

#include "MachO/LibraryList.h"
#include "MachO/LoadCommands.h"

namespace MachO {
    LibraryList::LibraryList(const MachO::LoadCommandsMap &Map,
                             const bool IsBigEndian) noexcept
    {
        for (const auto &LC : Map) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                addLibrary(cast<DylibCommand>(LC, IsBigEndian), IsBigEndian);
            }
        }
    }
}