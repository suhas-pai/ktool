/*
 * DyldSharedCache/DeVirtualizer.cpp
 * © suhas pai
 */

#include "DyldSharedCache/DeVirtualizer.h"

namespace DyldSharedCache {
    void *
    DeVirtualizer::getPtrForAddress(const uint64_t Address,
                                    const bool IgnoreProtBounds,
                                    void **const EndOut) const noexcept
    {
        uint64_t TotalSize = 0;
        if (const auto Result =
                Dsc.getPtrForAddress(Address, !IgnoreProtBounds, &TotalSize))
        {
            *EndOut = Result->second + TotalSize;
            return Result->second;
        }

        return nullptr;
    }

    [[nodiscard]] std::optional<ADT::MemoryMap>
    DeVirtualizer::getMapForRange(const ADT::Range &Range,
                                  const bool IgnoreProtBounds) const noexcept 
    {

        if (const auto Result =
                Dsc.getMapForAddrRange(Range, !IgnoreProtBounds))
        {
            return Result->second;
        }

        return std::nullopt;
    }
}