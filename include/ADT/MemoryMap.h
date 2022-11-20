//
//  ADT/MemoryMap.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once
#include "Range.h"

namespace ADT {
    struct MemoryMap {
    protected:
        void *Base = nullptr;
        uint64_t Size = 0;
    public:
        constexpr explicit MemoryMap() noexcept = default;
        constexpr explicit MemoryMap(void *Base, uint64_t Size)
        : Base(Base), Size(Size) {}

        [[nodiscard]] inline auto getRange() const noexcept {
            return Range::FromSize(reinterpret_cast<uint64_t>(Base), Size);
        }

        template <typename T = void *>
        [[nodiscard]] inline auto base() const noexcept {
            return reinterpret_cast<T *>(Base);
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return Size;
        }
    };
}
