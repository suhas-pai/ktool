//
//  ADT/MemoryMap.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include <string_view>
#include "Range.h"

#include "Utils/Misc.h"
#include "Utils/Overflow.h"

namespace ADT {
    struct MemoryMap {
    protected:
        void *Base = nullptr;
        uint64_t Size = 0;
    public:
        constexpr explicit MemoryMap() noexcept = default;
        constexpr explicit MemoryMap(void *Base, uint64_t Size)
        : Base(Base), Size(Size) {}

        [[nodiscard]] inline auto range() const noexcept {
            return Range::FromSize(0, Size);
        }

        explicit MemoryMap(const MemoryMap &Map, const Range &Range)
        : Base(reinterpret_cast<void *>(reinterpret_cast<uint64_t>(Map.Base) +
               Range.begin())),
          Size(Range.size())
        {
            assert(Map.range().contains(Range));
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return Size;
        }

        template <typename T = uint8_t,
                  bool Verify = true,
                  uint64_t Size = sizeof(T)>

        [[nodiscard]]
        inline auto base(const uint64_t Count = 1) const noexcept -> T * {
            if constexpr (Verify) {
                const auto TotalSize =
                    Utils::MulAndCheckOverflow(Size, Count).value();

                if (!range().canContainSize(TotalSize)) {
                    return nullptr;
                }
            }

            return reinterpret_cast<T *>(Base);
        }

        template <typename T = void *>
        [[nodiscard]] inline auto end() const noexcept {
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

            const auto AdjBase = reinterpret_cast<uint8_t *>(Base) + Offset;
            return reinterpret_cast<T *>(AdjBase);
        }

        template <typename T = void *, bool Verify = true>
        [[nodiscard]] inline auto
        getFromRange(const Range &Range,
                     T **const EndOut = nullptr) const noexcept -> T *
        {
            if constexpr (Verify) {
                if (!range().contains(Range)) {
                    return nullptr;
                }
            }

            const auto AdjBase =
                reinterpret_cast<uint64_t>(Base) + Range.begin();

            if (EndOut != nullptr) {
                *EndOut = reinterpret_cast<T *>(AdjBase + Range.size());
            }

            return reinterpret_cast<T *>(AdjBase);
        }

        [[nodiscard]]
        inline auto string(const uint64_t Offset) const noexcept ->
            std::optional<std::string_view>
        {
            const auto Ptr = get<const char>(Offset);
            if (Ptr == nullptr) {
                return std::nullopt;
            }

            const auto Length = strnlen(Ptr, size() - Offset);
            return std::string_view(Ptr, Length);
        }

        [[nodiscard]]
        inline auto indexForPtr(const void *const Ptr) const noexcept
            -> std::optional<uint64_t>
        {
            const auto Result =
                static_cast<uint64_t>(
                    reinterpret_cast<const uint8_t *>(Ptr) - base<uint8_t>());

            if (Utils::IndexOutOfBounds(Result, range().size())) {
                return std::nullopt;
            }

            return Result;
        }
    };
}
