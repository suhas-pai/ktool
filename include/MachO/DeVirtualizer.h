/*
 * MachO/DeVirtualizer.h
 * © suhas pai
 */

#pragma once

#include "ADT/MemoryMap.h"
#include "MachO/SegmentList.h"

namespace MachO {
    struct DeVirtualizer {
    protected:
        ADT::MemoryMap Map;
        MachO::SegmentList SegmentList;
    public:
        explicit
        DeVirtualizer(const ADT::MemoryMap &Map,
                      MachO::SegmentList &SegmentList) noexcept
        : Map(Map), SegmentList(SegmentList) {}

        [[nodiscard]] constexpr auto map() const noexcept {
            return Map;
        }

        [[nodiscard]] constexpr auto &segmentList() const noexcept {
            return SegmentList;
        }

        [[nodiscard]] virtual auto
        getPtrForAddress(const uint64_t Address,
                         bool IgnoreSectionBounds = false,
                         void **EndOut = nullptr) const noexcept -> void *;

        [[nodiscard]] virtual auto
        getPtrForRange(const ADT::Range &Range,
                       bool IgnoreSectionBounds = false,
                       void **EndOut = nullptr) const noexcept -> void *;

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

        template <typename T>
        [[nodiscard]] inline auto
        getDataAtAddress(const ADT::Range &Range,
                         const bool IgnoreSectionBounds = false,
                         T **const EndOut = nullptr) const noexcept
        {
            return static_cast<T *>(
                getPtrForRange(
                    Range,
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