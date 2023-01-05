//
//  src/MachO/LoadCommandsMap.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <compare>
#include "MachO/LoadCommandsMap.h"

namespace MachO {
    LoadCommandsIterator::LoadCommandsIterator(MachO::LoadCommand *const Ptr,
                                               const bool IsBigEndian) noexcept
    : Ptr(Ptr), IsBigEndian(IsBigEndian) {}

    auto LoadCommandsIterator::operator++() noexcept -> decltype(*this) {
        Ptr =
            reinterpret_cast<MachO::LoadCommand *>(
               reinterpret_cast<uint8_t *>(Ptr) + Ptr->cmdsize(IsBigEndian));

        return *this;
    }

    auto
    LoadCommandsIterator::operator++(int) noexcept -> decltype(*this) {
        return operator++();
    }

    auto
    LoadCommandsIterator::operator+=(const int Amt) noexcept -> decltype(*this)
    {
        for (auto I = int(); I != Amt; I++) {
            operator++();
        }

        return *this;
    }

    LoadCommandsMap::LoadCommandsMap(const ADT::MemoryMap Map,
                                     const bool IsBigEndian) noexcept
    : IsBigEndian(IsBigEndian), Map(Map) {}

    auto LoadCommandsMap::begin() const noexcept -> LoadCommandsIterator {
        return LoadCommandsIterator(Map.base<LoadCommand>(), IsBigEndian);
    }

    auto LoadCommandsMap::end() const noexcept -> const LoadCommand * {
        return Map.end<const LoadCommand>();
    }
}
