//
//  ADT/Mach-O/DeVirtualizer.h
//  ktool
//
//  Created by Suhas Pai on 5/28/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstring>
#include "SegmentUtil.h"

namespace MachO {
    struct DeVirtualizer {
    protected:
        uint8_t *Map;
        const SegmentInfoCollection &Collection;
    public:
        explicit
        DeVirtualizer(uint8_t *const Map,
                      const SegmentInfoCollection &Collection) noexcept
        : Map(Map), Collection(Collection) {}

        explicit
        DeVirtualizer(const MemoryMap &Map,
                      const SegmentInfoCollection &Collection) noexcept
        : Map(Map.getBegin()), Collection(Collection) {}

        [[nodiscard]]
        inline auto &getSegmentCollection() const noexcept {
            return Collection;
        }

        template <typename T>
        [[nodiscard]] auto
        GetDataAtAddress(const uint64_t Address,
                         T **const EndOut = nullptr) const noexcept
            -> T *
        {
            const auto Result =
                Collection.GetPtrForVirtualAddr<T>(Map,
                                                   Address,
                                                   sizeof(T),
                                                   EndOut);
            return Result;
        }

        template <typename T>
        [[nodiscard]] auto
        GetDataAtAddress(const uint64_t Address,
                         const uint64_t Size,
                         T **const EndOut = nullptr) const noexcept
            -> T *
        {
            const auto Result =
                Collection.GetPtrForVirtualAddr<T>(Map, Address, Size, EndOut);

            return Result;
        }

        template <typename T>
        [[nodiscard]] auto
        GetDataAtAddressIgnoreSections(
            const uint64_t Address,
            T **const EndOut = nullptr) const noexcept
                -> T *
        {
            const auto Result =
                Collection.GetPtrForVirtualAddrIgnoreSections<T>(Map,
                                                                 Address,
                                                                 sizeof(T),
                                                                 EndOut);
            return Result;
        }

        template <typename T>
        [[nodiscard]] auto
        GetDataAtAddressIgnoreSections(
            const uint64_t Address,
            const uint64_t Size,
            T **const EndOut = nullptr) const noexcept
                -> T *
        {
            const auto Result =
                Collection.GetPtrForVirtualAddrIgnoreSections<T>(Map,
                                                                 Address,
                                                                 Size,
                                                                 EndOut);

            return Result;
        }

        [[nodiscard]]
        auto GetStringAtAddress(uint64_t Address) const noexcept
            -> std::optional<std::string_view>
        {
            auto End = static_cast<const char *>(nullptr);
            const auto Ptr = GetDataAtAddress<const char>(Address, &End);

            if (Ptr == nullptr) {
                return std::nullopt;
            }

            const auto NameMaxLength = strnlen(Ptr, End - Ptr);
            const auto Name = std::string_view(Ptr, NameMaxLength);

            return Name;
        }
    };

    struct ConstDeVirtualizer : public DeVirtualizer {
        explicit ConstDeVirtualizer(
            const uint8_t *const Map,
            const SegmentInfoCollection &Collection) noexcept
        : DeVirtualizer(const_cast<uint8_t *>(Map), Collection) {}

        explicit ConstDeVirtualizer(
            const ConstMemoryMap &Map,
            const SegmentInfoCollection &Collection) noexcept
        : DeVirtualizer(const_cast<uint8_t *>(Map.getBegin()), Collection) {}

        template <typename T>
        [[nodiscard]] T *
        GetDataAtAddress(const uint64_t Address,
                         T **const EndOut = nullptr) const noexcept
        {
            const auto Result =
                Collection.GetPtrForVirtualAddr<T>(Map,
                                                   Address,
                                                   sizeof(T),
                                                   EndOut);
            return Result;
        }

        template <typename T>
        [[nodiscard]] T *
        GetDataAtAddress(const uint64_t Address,
                         const uint64_t Size,
                         T **const EndOut = nullptr) const noexcept
        {
            const auto Result =
                Collection.GetPtrForVirtualAddr<T>(Map, Address, Size, EndOut);
            
            return Result;
        }
    };
}
