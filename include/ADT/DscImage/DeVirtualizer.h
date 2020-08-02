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

#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"

namespace DscImage {
    struct ConstDeVirtualizer {
    protected:
        const uint8_t *Map;
        LocationRange MappingsRange;
    public:
        explicit
        ConstDeVirtualizer(const uint8_t *Map,
                           const LocationRange &MappingsRange) noexcept
        : Map(Map), MappingsRange(MappingsRange) {}

        [[nodiscard]] inline
        uint64_t getFileOffsetFromVmAddr(uint64_t VmAddr) const noexcept {
            return (VmAddr - getBeginAddr());
        }

        [[nodiscard]] inline const uint8_t *getMap() const noexcept {
            return Map;
        }

        [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
            return Map + getBeginAddr();
        }

        [[nodiscard]] inline uint64_t getBeginAddr() const noexcept {
            return getMappingsRange().getBegin();
        }

        [[nodiscard]] inline uint64_t getEndAddr() const noexcept {
            return getMappingsRange().getEnd();
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

        [[nodiscard]]
        inline const LocationRange &getMappingsRange() const noexcept {
            return MappingsRange;
        }

        template <typename T>
        [[nodiscard]] inline const T *
        GetDataAtVmAddr(uint64_t VmAddr,
                        uint64_t Size = sizeof(T)) const noexcept
        {
            if (!getMappingsRange().containsLocation(VmAddr)) {
                return nullptr;
            }

            const auto Offset = getFileOffsetFromVmAddr(VmAddr);
            const auto Result = reinterpret_cast<const T *>(Map + Offset);
            const auto End = getEndAs<T>();

            if ((End - Result) < Size) {
                return nullptr;
            }

            return Result;
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
        DeVirtualizer(uint8_t *Map, const LocationRange &MappingsRange) noexcept
        : ConstDeVirtualizer(const_cast<uint8_t *>(Map), MappingsRange) {}

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
