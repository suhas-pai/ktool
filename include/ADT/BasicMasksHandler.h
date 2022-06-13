//
//  include/ADT/BasicMasksHandler.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <bitset>
#include <cassert>

#include "Concepts/EnumClass.h"

template <std::integral T = uint64_t>
[[nodiscard]]
constexpr static T GetValueForMask(const T Integer, const T Mask) noexcept {
    return (Integer & Mask);
}

template <std::integral T = uint64_t>
constexpr
static T SetValueForMask(T &Integer, const T Mask, const T Value) noexcept {
    assert((Value & Mask) == Value &&
           "SetValueForMask(): Value falls outside mask");

    Integer &= ~Mask;
    Integer |= Value;

    return Integer;
}

template <std::integral T = uint64_t>
[[nodiscard]] constexpr
static inline T
GetValueForMaskAndShift(const T Integer, const T Mask, const T Shift) noexcept {
    const auto MaskedValue = GetValueForMask(Integer, Mask);
    return (MaskedValue >> Shift);
}

template <std::integral T = uint64_t>
constexpr static
T
SetValueForMaskAndShift(T &Integer,
                        const T Mask,
                        const T Shift,
                        const T Value) noexcept
{
    return SetValueForMask(Integer, Mask, (Value << Shift));
}

template <Concepts::EnumClass MaskType,
          typename IntegerTypeT = std::underlying_type_t<MaskType>>

struct BasicMasksHandler {
public:
    using IntegerType = IntegerTypeT;
protected:
    IntegerType Integer = 0;
public:
    using MaskIntegerType = std::underlying_type_t<MaskType>;

    constexpr BasicMasksHandler() noexcept = default;
    constexpr BasicMasksHandler(IntegerType Integer) noexcept
    : Integer(Integer) {};

    [[nodiscard]] constexpr
    IntegerType getValueForMask(const MaskType Mask) const noexcept {
        return GetValueForMask(Integer, static_cast<MaskIntegerType>(Mask));
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return (Integer == 0);
    }

    [[nodiscard]]
    constexpr bool hasValueForMask(const MaskType Mask) const noexcept {
        return getValueForMask(Mask);
    }

    constexpr BasicMasksHandler &
    setValueForMask(const MaskType Mask, const IntegerType Value) noexcept {
        const auto MaskValue = static_cast<MaskIntegerType>(Mask);
        SetValueForMask(this->Integer, MaskValue, Value);

        return *this;
    }

    [[nodiscard]] constexpr uint64_t GetSetCount() const noexcept {
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

    [[nodiscard]] constexpr
    BasicMasksHandler operator&(const BasicMasksHandler &Rhs) const noexcept {
        return BasicMasksHandler(Integer & Rhs.Integer);
    }

    constexpr
    BasicMasksHandler &operator&=(const BasicMasksHandler &Rhs) const noexcept {
        return (*this = (*this & Rhs.Integer));
    }

    [[nodiscard]] constexpr
    BasicMasksHandler operator|(const BasicMasksHandler &Rhs) const noexcept {
        return BasicMasksHandler(Integer | Rhs.Integer);
    }

    constexpr
    BasicMasksHandler &operator|=(const BasicMasksHandler &Rhs) const noexcept {
        return (*this = (*this | Rhs.Integer));
    }

    [[nodiscard]]
    constexpr BasicMasksHandler operator~() const noexcept {
        return BasicMasksHandler(~Integer);
    }

    [[nodiscard]] constexpr IntegerType value() const noexcept {
        return Integer;
    }

    [[nodiscard]] constexpr operator IntegerType() const noexcept {
        return Integer;
    }

    constexpr BasicMasksHandler &clear() noexcept {
        Integer = 0;
        return *this;
    }
};

template <typename MaskType,
          Concepts::EnumClass ShiftType,
          typename IntegerTypeT = std::underlying_type_t<MaskType>>

struct BasicMasksAndShiftsHandler :
    public BasicMasksHandler<MaskType, IntegerTypeT>
{
public:
    using IntegerType = IntegerTypeT;
private:
    using Base = BasicMasksHandler<MaskType, IntegerType>;
public:
    constexpr BasicMasksAndShiftsHandler() noexcept = default;
    constexpr BasicMasksAndShiftsHandler(IntegerType Integer) noexcept
    : Base(Integer) {};

    using MaskIntegerType = std::underlying_type_t<MaskType>;
    using ShiftIntegerType = std::underlying_type_t<ShiftType>;

    [[nodiscard]] constexpr IntegerType
    getValueForMaskAndShift(const MaskType Mask,
                            const ShiftType Shift) const noexcept
    {
        const auto MaskedValue = static_cast<MaskIntegerType>(Mask);
        const auto ShiftInteger = static_cast<ShiftIntegerType>(Shift);

        return GetValueForMaskAndShift(this->Integer, MaskedValue, ShiftInteger);
    }

    [[nodiscard]] constexpr IntegerType
    getValueForMaskNoShift(const MaskType Mask) const noexcept {
        return Base::getValueForMask(Mask);
    }

    constexpr BasicMasksAndShiftsHandler &
    setValueForMaskAndShift(const MaskType Mask,
                            const ShiftType Shift,
                            const IntegerType Value) noexcept
    {
        const auto ShiftAmount = static_cast<IntegerType>(Shift);
        const auto ShiftedValue =
            static_cast<IntegerType>(Value) << ShiftAmount;

        this->setValueForMask(Mask, ShiftedValue);
        return *this;
    }

    constexpr BasicMasksAndShiftsHandler &
    setValueForMaskNoShift(const MaskType Mask,
                           const IntegerType Value) noexcept
    {
        this->setValueForMask(Mask, Value);
        return *this;
    }

    constexpr BasicMasksAndShiftsHandler &clear() noexcept {
        this->Base::clear();
        return *this;
    }
};

template <typename FlagsType>
struct BasicFlags : public BasicMasksHandler<FlagsType> {
    using IntegerType = std::underlying_type_t<FlagsType>;

    constexpr BasicFlags() noexcept = default;
    constexpr BasicFlags(IntegerType Integer) noexcept
    : BasicMasksHandler<FlagsType>(Integer) {};

    constexpr BasicFlags &add(const FlagsType Flag) noexcept {
        this->setValueForMask(Flag, static_cast<int>(Flag));
        return *this;
    }

    constexpr BasicFlags &remove(const FlagsType Flag) noexcept {
        this->setValueForMask(Flag, 0);
        return *this;
    }

    [[nodiscard]]
    constexpr bool hasFlag(const FlagsType &Flag) const noexcept {
        return this->getValueForMask(Flag);
    }

    constexpr BasicFlags &
    setValueForFlag(const FlagsType &Flag, const bool Value) noexcept {
        this->setValueForMask(Flag, static_cast<IntegerType>(Flag));
        return *this;
    }

    [[nodiscard]]
    constexpr bool hasBitsSetAfterFlag(const FlagsType Flag) const noexcept {
        const auto FlagInteger = static_cast<IntegerType>(Flag);
        const auto Mask = ~(FlagInteger - 1);

        return (this->Integer & Mask);
    }

    [[nodiscard]] constexpr operator bool() const noexcept {
        return this->Integer;
    }
};
