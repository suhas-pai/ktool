//
//  ADT/MemoryMap.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include <optional>
#include <span>
#include <string_view>

#include "Utils/Misc.h"
#include "Utils/Overflow.h"

#include "Range.h"

namespace ADT {
    struct MemoryMap {
    protected:
        void *Base = nullptr;
        uint64_t Size = 0;
    public:
        constexpr explicit MemoryMap() noexcept = default;

        constexpr
        explicit MemoryMap(void *const Base, const uint64_t Size) noexcept
        : Base(Base), Size(Size) {}

        template <typename T>
        constexpr explicit MemoryMap(const std::span<T> Span) noexcept
        : Base(Span.begin()),
          Size(Utils::MulAndCheckOverflow(Span.size(), sizeof(T)).value()) {}

        [[nodiscard]] constexpr auto size() const noexcept {
            return this->Size;
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return this->size() == 0;
        }

        [[nodiscard]] inline auto range() const noexcept {
            return Range::FromSize(0, this->size());
        }

        explicit MemoryMap(const MemoryMap &Map, const Range &Range)
        : Base(reinterpret_cast<void *>(reinterpret_cast<uint64_t>(Map.Base) +
               Range.front())),
          Size(Range.size())
        {
            assert(Map.range().contains(Range));
        }

        template <typename T = uint8_t,
                  bool Verify = true,
                  uint64_t Size = sizeof(T)>

        [[nodiscard]]
        inline auto base(const uint64_t Count = 1) const noexcept -> T * {
            if constexpr (Verify) {
                const auto TotalSizeOpt =
                    Utils::MulAndCheckOverflow(Size, Count);

                if (!TotalSizeOpt.has_value()) {
                    return nullptr;
                }

                if (!this->range().canContainSize(TotalSizeOpt.value())) {
                    return nullptr;
                }
            }

            return reinterpret_cast<T *>(this->Base);
        }

        template <typename T = void *>
        [[nodiscard]] inline auto end() const noexcept {
            const auto Result =
                reinterpret_cast<const T *>(
                    reinterpret_cast<uint64_t>(this->base()) + this->size());
            return Result;
        }

        template <typename T = uint8_t,
                  bool Verify = true,
                  uint64_t Size = sizeof(T)>

        [[nodiscard]] inline
        auto get(const uint64_t Offset, const uint64_t Count = 1) const noexcept
            -> T *
        {
            if constexpr (Verify) {
                const auto EndOpt =
                    Utils::AddMulAndCheckOverflow(Offset, Size, Count);

                if (!EndOpt.has_value() || this->size() < EndOpt.value()) {
                    return nullptr;
                }
            }

            const auto AdjBase =
                reinterpret_cast<uint8_t *>(this->base()) + Offset;

            return reinterpret_cast<T *>(AdjBase);
        }

        template <typename T = uint8_t,
                  bool Verify = true,
                  uint64_t Size = sizeof(T)>

        [[nodiscard]] inline
        auto get(const Range &Range, const uint64_t Count = 1) const noexcept
            -> T *
        {
            if constexpr (Verify) {
                const auto FullRangeOpt = Range.multiply(Count);
                if (!FullRangeOpt.has_value()) {
                    return nullptr;
                }

                const auto FullRange = FullRangeOpt.value();
                if (!this->range().contains(FullRange)) {
                    return nullptr;
                }
            }

            const auto AdjBase =
                reinterpret_cast<uint8_t *>(this->base()) + Range.front();

            return reinterpret_cast<T *>(AdjBase);
        }

        template <typename T = uint8_t, bool Verify = true>
        [[nodiscard]] inline auto
        getSpan(const uint64_t Offset, const uint64_t Count) const noexcept
            -> std::optional<std::span<T>>
        {
            if constexpr (Verify) {
                const auto EndOpt =
                    Utils::AddMulAndCheckOverflow(Offset, sizeof(T), Count);

                if (!EndOpt.has_value() || this->size() < EndOpt.value()) {
                    return std::nullopt;
                }
            }

            const auto AdjBase =
                reinterpret_cast<uint64_t>(this->base()) + Offset;

            return std::span(reinterpret_cast<T *>(AdjBase), Count);
        }

        template <typename T = uint8_t, bool Verify = true>
        [[nodiscard]] inline auto getRange(const Range &Range) const noexcept
            -> std::optional<std::span<T>>
        {
            if constexpr (Verify) {
                if (!this->range().contains(Range)) {
                    return std::nullopt;
                }
            }

            const auto AdjBase =
                reinterpret_cast<uint64_t>(this->base()) + Range.front();

            return std::span(reinterpret_cast<T *>(AdjBase),
                             Range.size() / sizeof(T));
        }

        template <typename T = uint8_t, uint64_t Size = sizeof(T),
                  bool Verify = true>
        [[nodiscard]] constexpr auto span() const noexcept
            -> std::optional<std::span<T>>
        {
            const auto Base = this->base<T, Verify>();
            if (Base == nullptr) {
                return std::nullopt;
            }

            if (this->size() < Size) {
                return std::nullopt;
            }

            return std::span<T>(Base, this->size() / Size);
        }

        [[nodiscard]] inline auto string(const uint64_t Offset) const noexcept
            -> std::optional<std::string_view>
        {
            const auto Ptr = this->get<const char>(Offset);
            if (Ptr == nullptr) {
                return std::nullopt;
            }

            const auto Length = strnlen(Ptr, this->size() - Offset);
            if (Length == 0) {
                return std::nullopt;
            }

            return std::string_view(Ptr, Length);
        }

        [[nodiscard]]
        inline auto indexForPtr(const void *const Ptr) const noexcept
            -> std::optional<uint64_t>
        {
            const auto Result =
                static_cast<uint64_t>(
                    reinterpret_cast<const uint8_t *>(Ptr) -
                    this->base<uint8_t>());

            if (Utils::IndexOutOfBounds(Result, this->size())) {
                return std::nullopt;
            }

            return Result;
        }
    };
}
