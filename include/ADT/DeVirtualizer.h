/*
 * ADT/DeVirtualizer.h
 * © suhas pai
 */

#pragma once
#include <string_view>

#include "ADT/MemoryMap.h"
#include "ADT/Range.h"

namespace ADT {
    struct DeVirtualizer {
    public:
        [[nodiscard]] virtual auto
        getPtrForAddress(const uint64_t Address,
                         bool IgnoreSectionBounds = false,
                         void **EndOut = nullptr) const noexcept -> void * = 0;

        [[nodiscard]] virtual auto
        getMapForRange(const ADT::Range &Range,
                       bool IgnoreSectionBounds = false) const noexcept
            -> std::optional<ADT::MemoryMap> = 0;

        template <typename T>
        [[nodiscard]] inline auto
        getDataAtAddress(const uint64_t Address,
                         const bool IgnoreSectionBounds = false,
                         T **const EndOut = nullptr) const noexcept
        {
            return static_cast<T *>(
                getPtrForAddress(
                    Address,
                    IgnoreSectionBounds,
                    reinterpret_cast<void **>(
                        const_cast<std::remove_const_t<T> **>(EndOut))));
        }

        [[nodiscard]] inline auto
        getStringAtAddress(
            const uint64_t Address,
            const bool IgnoreSectionBounds = false) const noexcept
            -> std::optional<std::string_view>
        {
            auto End = static_cast<const char *>(nullptr);
            const auto Ptr =
                getDataAtAddress<const char>(Address,
                                             IgnoreSectionBounds,
                                             &End);

            if (Ptr == nullptr) {
                return std::nullopt;
            }

            const auto NameMaxLength =
                strnlen(Ptr, static_cast<uint64_t>(End - Ptr));

            return std::string_view(Ptr, NameMaxLength);
        }
    };
}