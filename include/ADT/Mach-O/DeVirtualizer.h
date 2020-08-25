//
//  include/ADT/Mach-O/DeVirtualizer.h
//  ktool
//
//  Created by Suhas Pai on 5/28/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstring>
#include "SegmentUtil.h"

namespace MachO {
    struct DeVirtualizer {
    protected:
        uint8_t *Map;
        const MachO::SegmentInfoCollection &Collection;
    public:
        explicit
        DeVirtualizer(uint8_t *Map,
                      const MachO::SegmentInfoCollection &Collection) noexcept
        : Map(Map), Collection(Collection) {}

        explicit
        DeVirtualizer(const MemoryMap &Map,
                      const MachO::SegmentInfoCollection &Collection) noexcept
        : Map(Map.getBegin()), Collection(Collection) {}

        [[nodiscard]] inline const MachO::SegmentInfoCollection &
        getSegmentCollection() const noexcept {
            return Collection;
        }

        template <typename T>
        [[nodiscard]] T *
        GetDataAtAddress(uint64_t Address, T **EndOut = nullptr) const noexcept
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
        GetDataAtAddress(uint64_t Address,
                         uint64_t Size,
                         T **EndOut = nullptr) const noexcept
        {
            const auto Result =
                Collection.GetPtrForVirtualAddr<T>(Map, Address, Size, EndOut);

            return Result;
        }

        template <typename T>
        [[nodiscard]] T *
        GetDataAtAddressIgnoreSections(uint64_t Address,
                                       T **EndOut = nullptr) const noexcept
        {
            const auto Result =
                Collection.GetPtrForVirtualAddrIgnoreSections<T>(Map,
                                                                 Address,
                                                                 sizeof(T),
                                                                 EndOut);
            return Result;
        }

        template <typename T>
        [[nodiscard]] T *
        GetDataAtAddressIgnoreSections(uint64_t Address,
                                       uint64_t Size,
                                       T **EndOut = nullptr) const noexcept
        {
            const auto Result =
                Collection.GetPtrForVirtualAddrIgnoreSections<T>(Map,
                                                                 Address,
                                                                 Size,
                                                                 EndOut);

            return Result;
        }

        [[nodiscard]]
        std::optional<std::string_view>
        GetStringAtAddress(uint64_t Address) const noexcept {
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
            const uint8_t *Map,
            const MachO::SegmentInfoCollection &Collection) noexcept
        : DeVirtualizer(const_cast<uint8_t *>(Map), Collection) {}

        explicit ConstDeVirtualizer(
            const ConstMemoryMap &Map,
            const MachO::SegmentInfoCollection &Collection) noexcept
        : DeVirtualizer(const_cast<uint8_t *>(Map.getBegin()), Collection) {}

        template <typename T>
        [[nodiscard]] T *
        GetDataAtAddress(uint64_t Address, T **EndOut = nullptr) const noexcept
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
        GetDataAtAddress(uint64_t Address,
                         uint64_t Size,
                         T **EndOut = nullptr) const noexcept
        {
            const auto Result =
                Collection.GetPtrForVirtualAddr<T>(Map, Address, Size, EndOut);
            return Result;
        }
    };
}
