//
//  include/ADT/DscImage/DeVirtualizer.h
//  ktool
//
//  Created by Suhas Pai on 7/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <cstring>
#include <optional>
#include <string_view>

#include "ADT/DyldSharedCache.h"
#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"

namespace DscImage {
    struct ConstDeVirtualizer {
    protected:
        const uint8_t *Map;
        DyldSharedCache::ConstMappingList MappingList;
    public:
        explicit
        ConstDeVirtualizer(
            const uint8_t *Map,
            const DyldSharedCache::ConstMappingList &MappingList) noexcept
        : Map(Map), MappingList(MappingList) {}

        [[nodiscard]]
        inline const DyldSharedCache::ConstMappingList &
        getMappingsList() const noexcept {
            return MappingList;
        }

        [[nodiscard]] inline const uint8_t *getMap() const noexcept {
            return Map;
        }

        [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
            return Map + getBeginAddr();
        }

        [[nodiscard]] inline uint64_t getBeginAddr() const noexcept {
            return getMappingsList().front().Address;
        }

        [[nodiscard]] inline uint64_t getEndAddr() const noexcept {
            const auto &Back = getMappingsList().back();
            return Back.Address + Back.Size;
        }

        [[nodiscard]] inline const uint8_t *getEnd() const noexcept {
            return Map + getEndAddr();
        }

        template <typename T>
        [[nodiscard]] inline const T *getBeginAs() const noexcept {
            return reinterpret_cast<const T *>(getBegin());
        }

        template <typename T>
        [[nodiscard]] inline const T *getEndAs() const noexcept {
            return reinterpret_cast<const T *>(getEnd());
        }

        [[nodiscard]] inline const DyldSharedCache::MappingInfo *
        GetMappingInfoForRange(const LocationRange &Range) const noexcept {
            for (const auto &Mapping : getMappingsList()) {
                if (const auto MapRange = Mapping.getAddressRange()) {
                    if (MapRange->contains(Range)) {
                        return &Mapping;
                    }
                }
            }

            return nullptr;
        }

        template <typename T>
        [[nodiscard]] inline const T *
        GetDataAtVmAddr(uint64_t VmAddr,
                        uint64_t Size = sizeof(T)) const noexcept
        {
            const auto Range = LocationRange::CreateWithSize(VmAddr, Size);
            if (const auto Info = GetMappingInfoForRange(Range.value())) {
                const auto Offset = VmAddr - Info->Address;
                const auto Result = reinterpret_cast<const T *>(Map + Offset);

                return Result;
            }

            return nullptr;
        }

        [[nodiscard]]
        inline std::optional<std::string_view>
        GetStringAtAddress(uint64_t Address) const noexcept {
            const auto Ptr = GetDataAtVmAddr<const char>(Address);
            if (Ptr == nullptr) {
                return std::nullopt;
            }

            const auto End = getEndAs<const char>();
            const auto NameMaxLength = strnlen(Ptr, End - Ptr);
            const auto Name = std::string_view(Ptr, NameMaxLength);

            return Name;
        }
    };

    struct DeVirtualizer : public ConstDeVirtualizer {
    public:
        explicit
        DeVirtualizer(
            uint8_t *Map,
            const DyldSharedCache::ConstMappingList &MappingList) noexcept
        : ConstDeVirtualizer(const_cast<uint8_t *>(Map), MappingList) {}

        [[nodiscard]] inline uint8_t *getMap() const noexcept {
            return const_cast<uint8_t *>(Map);
        }

        [[nodiscard]] inline uint8_t *getBegin() const noexcept {
            return const_cast<uint8_t *>(Map) + getBeginAddr();
        }

        template <typename T>
        [[nodiscard]] inline T *getBeginAs() const noexcept {
            return reinterpret_cast<T *>(getBegin());
        }

        template <typename T>
        [[nodiscard]] inline T *getEndAs() const noexcept {
            return reinterpret_cast<T *>(getEnd());
        }

        template <typename T>
        [[nodiscard]] inline T *
        GetDataAtVmAddr(uint64_t VmAddr,
                        uint64_t Size = sizeof(T)) const noexcept
        {
            const auto Result =
                ConstDeVirtualizer::GetDataAtVmAddr<T>(VmAddr, Size);

            return const_cast<T *>(Result);
        }
    };
}
