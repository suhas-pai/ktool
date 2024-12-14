//
//  ADT/DscImage/DeVirtualizer.h
//  ktool
//
//  Created by Suhas Pai on 7/18/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <cstring>
#include <optional>
#include <string_view>

#include "ADT/DyldSharedCache/Headers.h"
#include "ADT/Range.h"

namespace DscImage {
    struct ConstDeVirtualizer {
    protected:
        const uint8_t *Map;
        DyldSharedCache::ConstMappingInfoList MappingList;
    public:
        explicit inline
        ConstDeVirtualizer(
            const uint8_t *const Map,
            const DyldSharedCache::ConstMappingInfoList &MappingList) noexcept
        : Map(Map), MappingList(MappingList) {}

        [[nodiscard]] constexpr auto &getMappingsList() const noexcept {
            return this->MappingList;
        }

        [[nodiscard]] constexpr auto getMap() const noexcept {
            return this->Map;
        }

        [[nodiscard]] constexpr auto getBeginOffset() const noexcept {
            return this->getMappingsList().front().FileOffset;
        }

        [[nodiscard]] constexpr auto getBegin() const noexcept {
            return this->getMap() + this->getBeginOffset();
        }

        [[nodiscard]] constexpr auto getBeginAddr() const noexcept {
            return this->getMappingsList().front().Address;
        }

        [[nodiscard]] constexpr auto getEndAddr() const noexcept {
            const auto &Back = this->getMappingsList().back();
            return Back.Address + Back.Size;
        }

        [[nodiscard]] constexpr auto getEndOffset() const noexcept {
            const auto &Back = this->getMappingsList().back();
            return Back.FileOffset + Back.Size;
        }

        [[nodiscard]] constexpr const uint8_t *getEnd() const noexcept {
            return this->getMap() + this->getEndOffset();
        }

        template <typename T>
        [[nodiscard]] constexpr auto getBeginAs() const noexcept {
            return reinterpret_cast<const T *>(this->getBegin());
        }

        template <typename T>
        [[nodiscard]] constexpr auto getEndAs() const noexcept {
            return reinterpret_cast<const T *>(this->getEnd());
        }

        [[nodiscard]] constexpr
        auto GetMappingInfoForRange(const Range &Range) const noexcept
            -> const DyldSharedCache::MappingInfo *
        {
            for (const auto &Mapping : this->getMappingsList()) {
                if (const auto MapRange = Mapping.getAddressRange()) {
                    if (MapRange->contains(Range)) {
                        return &Mapping;
                    }
                }
            }

            return nullptr;
        }

        template <typename T>
        [[nodiscard]] constexpr auto
        GetDataAtVmAddr(const uint64_t VmAddr,
                        const uint64_t Size = sizeof(T)) const noexcept
            -> const T *
        {
            const auto Range = Range::CreateWithSize(VmAddr, Size);
            if (const auto Info = this->GetMappingInfoForRange(Range)) {
                const auto Offset = Info->getFileOffsetFromAddrUnsafe(VmAddr);
                return reinterpret_cast<const T *>(getMap() + Offset);
            }

            return nullptr;
        }

        [[nodiscard]]
        inline auto GetStringAtAddress(const uint64_t Address) const noexcept
            -> std::optional<std::string_view>
        {
            const auto Ptr = this->GetDataAtVmAddr<const char>(Address);
            if (Ptr == nullptr) {
                return std::nullopt;
            }

            const auto End = this->getEndAs<const char>();
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
            const DyldSharedCache::ConstMappingInfoList &MappingList) noexcept
        : ConstDeVirtualizer(const_cast<uint8_t *>(Map), MappingList) {}

        [[nodiscard]] constexpr auto getMap() const noexcept {
            return const_cast<uint8_t *>(Map);
        }

        [[nodiscard]] constexpr auto getBegin() const noexcept {
            return this->getMap() + this->getBeginAddr();
        }

        template <typename T>
        [[nodiscard]] constexpr auto getBeginAs() const noexcept {
            return reinterpret_cast<T *>(getBegin());
        }

        template <typename T>
        [[nodiscard]] constexpr auto getEndAs() const noexcept {
            return reinterpret_cast<T *>(getEnd());
        }

        template <typename T>
        [[nodiscard]] constexpr auto
        GetDataAtVmAddr(const uint64_t VmAddr,
                        const uint64_t Size = sizeof(T)) const noexcept
        {
            const auto Result =
                ConstDeVirtualizer::GetDataAtVmAddr<T>(VmAddr, Size);

            return const_cast<T *>(Result);
        }
    };
}
