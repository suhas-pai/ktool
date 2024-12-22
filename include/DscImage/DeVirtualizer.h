//
//  DscImage/DeVirtualizer.h
//  ktool
//
//  Created by Suhas Pai on 12/19/24.
//

#pragma once

#include "ADT/DeVirtualizer.h"
#include "Objects/DyldSharedCache.h"

namespace DscImage {
    struct DeVirtualizer : public ADT::DeVirtualizer {
    protected:
        const Objects::DyldSharedCache &Dsc;
        uint64_t ImageBaseAddress = 0;
    public:
        explicit
        DeVirtualizer(const Objects::DyldSharedCache &Dsc,
                      const uint64_t ImageBaseAddress) noexcept
        : Dsc(Dsc), ImageBaseAddress(ImageBaseAddress) {}

        [[nodiscard]] uint64_t getBaseAddress() const noexcept override {
            return this->ImageBaseAddress;
        }

        [[nodiscard]] void *
        getPtrForAddress(const uint64_t Address,
                         bool IgnoreProtBounds = false,
                         void **EndOut = nullptr) const noexcept override;

        [[nodiscard]] std::optional<ADT::MemoryMap>
        getMapForVmRange(const ADT::Range &Range,
                         bool IgnoreProtBounds = false) const noexcept override;

        [[nodiscard]] constexpr auto map() const noexcept {
            return this->Dsc.map();
        }
    };
}