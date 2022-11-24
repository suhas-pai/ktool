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

        explicit MemoryMap(const MemoryMap &Map, const Range &Range)
        : Base(reinterpret_cast<void *>(reinterpret_cast<uint64_t>(Map.Base) +
               Range.begin())),
          Size(Range.size()) {}

        [[nodiscard]] inline auto range() const noexcept {
            return Range::FromSize(reinterpret_cast<uint64_t>(Base), Size);
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return Size;
        }

        template <typename T = void *, bool Verify = true>
        [[nodiscard]]
        inline auto base(const uint64_t Count = 1) const noexcept -> T * {
            if constexpr (Verify) {
                if (size() < (sizeof(T) * Count)) {
                    return nullptr;
                }
            }

            return reinterpret_cast<T *>(Base);
        }

        template <typename T = void *>
        [[nodiscard]]
        inline auto end(const uint64_t Count = 1) const noexcept -> const T * {
            const auto Result =
                reinterpret_cast<const T *>(
                    reinterpret_cast<uint64_t>(Base) + Size);
            return Result;
        }

        template <typename T = void *, bool Verify = true>
        [[nodiscard]] inline
        auto get(const uint64_t Offset, const uint64_t Count = 1) const noexcept
            -> T *
        {
            if constexpr (Verify) {
                if (size() < (Offset + sizeof(T) * Count)) {
                    return nullptr;
                }
            }

            const auto AdjBase = reinterpret_cast<uint64_t>(Base) + Offset;
            return reinterpret_cast<T *>(AdjBase);
        }
    };
}
