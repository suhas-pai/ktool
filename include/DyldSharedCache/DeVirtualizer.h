/*
 * DyldSharedCache/DeVirtualizer.h
 * © suhas pai
 */

#pragma once

#include "ADT/DeVirtualizer.h"
#include "ADT/MemoryMap.h"

#include "Objects/DyldSharedCache.h"

namespace DyldSharedCache {
    struct DeVirtualizer : public ADT::DeVirtualizer {
    protected:
        const Objects::DyldSharedCache &Dsc;
    public:
        explicit DeVirtualizer(const Objects::DyldSharedCache &Dsc) noexcept
        : Dsc(Dsc) {}

        [[nodiscard]] void *
        getPtrForAddress(const uint64_t Address,
                         bool IgnoreProtBounds = false,
                         void **EndOut = nullptr) const noexcept override;

        [[nodiscard]] std::optional<ADT::MemoryMap>
        getMapForRange(const ADT::Range &Range,
                       bool IgnoreProtBounds = false) const noexcept override;
    };
}