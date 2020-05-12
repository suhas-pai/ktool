//
//  include/ADT/BasicMasksHandler.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <bitset>
#include "TypeTraits/IsEnumClass.h"

template <typename MasksType,
          typename IntegerType = std::underlying_type_t<MasksType>>

struct BasicMasksHandler {
    static_assert(std::is_integral_v<IntegerType>,
                  "IntegerType must be an integer type");
    static_assert(TypeTraits::IsEnumClassValue<MasksType>,
                  "EnumType must be an enum class");
protected:
    IntegerType Integer;
public:
    using MaskIntegerType = std::underlying_type_t<MasksType>;

    constexpr explicit BasicMasksHandler() noexcept = default;
    constexpr BasicMasksHandler(IntegerType Integer) noexcept
    : Integer(Integer) {};

    [[nodiscard]] constexpr
    inline IntegerType GetValueForMask(MasksType Mask) const noexcept {
        return (Integer & static_cast<MaskIntegerType>(Mask));
    }

    [[nodiscard]]
    constexpr inline bool Has(const MasksType Mask) const noexcept {
        return GetValueForMask(Mask);
    }

    constexpr inline BasicMasksHandler &Add(const MasksType Mask) noexcept {
        const auto MaskInteger = static_cast<MaskIntegerType>(Mask);
        const auto MaskedInteger = (Integer | MaskInteger);

        Integer = MaskedInteger;
        return *this;
    }

    constexpr inline BasicMasksHandler &
    Remove(const MasksType Mask, bool IsBigEndian) noexcept {
        const auto MaskInteger = static_cast<MaskIntegerType>(Mask);
        const auto MaskedInteger = (Integer & ~MaskInteger);

        Integer = MaskedInteger;
        return *this;
    }

    constexpr inline uint64_t GetSetCount(bool IsBigEndian) const noexcept {
        const auto Integer = SwitchEndianIf(this->Integer, IsBigEndian);
        const auto BitSet = std::bitset<sizeof(IntegerType) * 8>(Integer);

        return BitSet.count();
    }

    [[nodiscard]] constexpr
    inline bool operator==(const BasicMasksHandler &Rhs) const noexcept {
        return Integer == Rhs.Integer;
    }

    [[nodiscard]] constexpr
    inline bool operator!=(const BasicMasksHandler &Rhs) const noexcept {
        return Integer != Rhs.Integer;
    }

    [[nodiscard]] constexpr
    inline bool operator<(const BasicMasksHandler &Rhs) const noexcept {
        return Integer < Rhs.Integer;
    }

    [[nodiscard]] constexpr
    inline bool operator<=(const BasicMasksHandler &Rhs) const noexcept {
        return Integer <= Rhs.Integer;
    }

    [[nodiscard]] constexpr
    inline bool operator>(const BasicMasksHandler &Rhs) const noexcept {
        return Integer > Rhs.Integer;
    }

    [[nodiscard]] constexpr
    inline bool operator>=(const BasicMasksHandler &Rhs) const noexcept {
        return Integer >= Rhs.Integer;
    }

    [[nodiscard]] constexpr inline
    BasicMasksHandler operator&(const BasicMasksHandler &Rhs) const noexcept {
        return BasicMasksHandler(Integer & Rhs.Integer);
    }

    constexpr inline BasicMasksHandler &
    operator&=(const BasicMasksHandler &Rhs) const noexcept {
        return (*this = (*this & Rhs.Integer));
    }

    [[nodiscard]] constexpr inline
    BasicMasksHandler operator|(const BasicMasksHandler &Rhs) const noexcept {
        return BasicMasksHandler(Integer | Rhs.Integer);
    }

    constexpr inline
    BasicMasksHandler &operator|=(const BasicMasksHandler &Rhs) const noexcept {
        return (*this = (*this | Rhs.Integer));
    }

    constexpr inline BasicMasksHandler operator~() {
        return BasicMasksHandler(~Integer);
    }

    [[nodiscard]]
    inline IntegerType GetIntegerValue() const noexcept { return Integer; }
    inline operator IntegerType() const noexcept { return Integer; }
};
