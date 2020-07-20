//
//  include/ADT/DscImage/DeVirtualizer.h
//  ktool
//
//  Created by Suhas Pai on 7/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"

namespace DscImage {
    struct DeVirtualizer {
    protected:
        uint8_t *Map;
        LocationRange MappingsRange;
    public:
        explicit
        DeVirtualizer(uint8_t *Map, const LocationRange &MappingsRange) noexcept
        : Map(Map), MappingsRange(MappingsRange) {}

        [[nodiscard]] inline
        uint64_t getFileOffsetFromVmAddr(uint64_t VmAddr) const noexcept {
            return (VmAddr - MappingsRange.getBegin());
        }

        template <typename T>
        [[nodiscard]] inline T *
        GetDataAtVmAddr(uint64_t VmAddr,
                        uint64_t Size = sizeof(T)) const noexcept
        {
            if (VmAddr < MappingsRange.getBegin()) {
                return nullptr;
            }
            
            const auto Offset = getFileOffsetFromVmAddr(VmAddr);
            const auto Result = reinterpret_cast<T *>(Map + Offset);
            const auto End = getEndAs<T>();

            if (Result >= End || (End - Result) < Size) {
                return nullptr;
            }

            return Result;
        }

        [[nodiscard]] inline uint8_t *getMap() const noexcept {
            return Map;
        }

        [[nodiscard]] inline uint8_t *getBegin() const noexcept {
            return Map + MappingsRange.getBegin();
        }

        [[nodiscard]] inline const uint8_t *getEnd() const noexcept {
            return Map + MappingsRange.getEnd();
        }

        template <typename T>
        [[nodiscard]] inline T *getBeginAs() const noexcept {
            return reinterpret_cast<T *>(getBegin());
        }

        template <typename T>
        [[nodiscard]] inline T *getEndAs() const noexcept {
            return reinterpret_cast<T *>(getEnd());
        }

        [[nodiscard]] inline LocationRange getMappingsRange() const noexcept {
            return MappingsRange;
        }

        [[nodiscard]]
        std::optional<std::string_view>
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

    struct ConstDeVirtualizer : public DeVirtualizer {
    public:
        explicit
        ConstDeVirtualizer(const uint8_t *Map,
                           const LocationRange &MappingsRange) noexcept
        : DeVirtualizer(const_cast<uint8_t *>(Map), MappingsRange) {}

        [[nodiscard]] inline const uint8_t *getMap() const noexcept {
            return Map;
        }

        [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
            return Map + MappingsRange.getBegin();
        }

        [[nodiscard]] inline const uint8_t *getEnd() const noexcept {
            return Map + MappingsRange.getEnd();
        }

        template <typename T>
        [[nodiscard]] inline const T *getBeginAs() const noexcept {
            return reinterpret_cast<T *>(getBegin());
        }

        template <typename T>
        [[nodiscard]] inline const T *getEndAs() const noexcept {
            return reinterpret_cast<T *>(getEnd());
        }
    };
}
