//
//  ADT/BasicMasksHandler.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <bitset>
#include <cassert>

#include "Concepts/EnumClass.h"

template <std::integral T = uint64_t>
[[nodiscard]]
constexpr static T GetValueForMask(const T Integer, const T Mask) noexcept {
    return Integer & Mask;
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
[[nodiscard]] constexpr static T
GetValueForMaskAndShift(const T Integer, const T Mask, const T Shift) noexcept {
    const auto MaskedValue = GetValueForMask(Integer, Mask);
    return MaskedValue >> Shift;
}

template <std::integral T = uint64_t>
constexpr static auto
SetValueForMaskAndShift(T &Integer,
                        const T Mask,
                        const T Shift,
                        const T Value) noexcept -> T
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

    [[nodiscard]]
    constexpr auto getValueForMask(const MaskType Mask) const noexcept {
        return GetValueForMask(Integer, static_cast<MaskIntegerType>(Mask));
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return Integer == 0;
    }

    [[nodiscard]]
    constexpr auto hasValueForMask(const MaskType Mask) const noexcept {
        return this->getValueForMask(Mask);
    }

    constexpr auto
    setValueForMask(const MaskType Mask, const IntegerType Value) noexcept
        -> decltype(*this)
    {
        const auto MaskValue = static_cast<MaskIntegerType>(Mask);
        SetValueForMask(this->Integer, MaskValue, Value);

        return *this;
    }

    [[nodiscard]] constexpr uint64_t GetSetCount() const noexcept {
        const auto BitSet = std::bitset<sizeof(IntegerType) * 8>(Integer);
        return BitSet.count();
    }

    [[nodiscard]]
    constexpr auto operator<=>(const BasicMasksHandler &Rhs) const noexcept {
        return Integer <=> Rhs.Integer;
    }

    [[nodiscard]]
    constexpr auto operator&(const BasicMasksHandler &Rhs) const noexcept {
        return BasicMasksHandler(Integer & Rhs.Integer);
    }

    constexpr auto operator&=(const BasicMasksHandler &Rhs) const noexcept
        -> decltype(*this)
    {
        *this = *this & Rhs.Integer;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator|(const BasicMasksHandler &Rhs) const noexcept {
        return BasicMasksHandler(Integer | Rhs.Integer);
    }

    constexpr auto operator|=(const BasicMasksHandler &Rhs) const noexcept
        -> decltype(*this)
    {
        *this = *this | Rhs.Integer;
        return *this;
    }

    [[nodiscard]] constexpr auto operator~() const noexcept {
        return BasicMasksHandler(~Integer);
    }

    [[nodiscard]] constexpr auto value() const noexcept {
        return Integer;
    }

    [[nodiscard]] constexpr operator IntegerType() const noexcept {
        return Integer;
    }

    constexpr auto clear() noexcept -> decltype(*this) {
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

    [[nodiscard]] constexpr auto
    getValueForMaskAndShift(const MaskType Mask,
                            const ShiftType Shift) const noexcept
    {
        const auto MaskedValue = static_cast<MaskIntegerType>(Mask);
        const auto ShiftInteger = static_cast<ShiftIntegerType>(Shift);

        return GetValueForMaskAndShift(this->Integer, MaskedValue, ShiftInteger);
    }

    [[nodiscard]]
    constexpr auto getValueForMaskNoShift(const MaskType Mask) const noexcept {
        return Base::getValueForMask(Mask);
    }

    constexpr auto
    setValueForMaskAndShift(const MaskType Mask,
                            const ShiftType Shift,
                            const IntegerType Value) noexcept
        -> decltype(*this)
    {
        const auto ShiftAmount = static_cast<IntegerType>(Shift);
        const auto ShiftedValue =
            static_cast<IntegerType>(Value) << ShiftAmount;

        this->setValueForMask(Mask, ShiftedValue);
        return *this;
    }

    constexpr auto
    setValueForMaskNoShift(const MaskType Mask,
                           const IntegerType Value) noexcept
        -> decltype(*this)
    {
        this->setValueForMask(Mask, Value);
        return *this;
    }

    constexpr auto clear() noexcept
        -> decltype(*this)
    {
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

    constexpr auto add(const FlagsType Flag) noexcept
        -> decltype(*this)
    {
        this->setValueForMask(Flag, static_cast<int>(Flag));
        return *this;
    }

    constexpr auto remove(const FlagsType Flag) noexcept
        -> decltype(*this)
    {
        this->setValueForMask(Flag, 0);
        return *this;
    }

    [[nodiscard]]
    constexpr bool hasFlag(const FlagsType &Flag) const noexcept {
        return this->getValueForMask(Flag);
    }

    constexpr auto
    setValueForFlag(const FlagsType &Flag, const bool Value) noexcept
        -> decltype(*this)
    {
        this->setValueForMask(Flag, static_cast<IntegerType>(Value));
        return *this;
    }

    [[nodiscard]]
    constexpr auto hasBitsSetAfterFlag(const FlagsType Flag) const noexcept {
        const auto FlagInteger = static_cast<IntegerType>(Flag);
        const auto Mask = ~(FlagInteger - 1);

        return this->Integer & Mask;
    }

    [[nodiscard]] constexpr operator bool() const noexcept {
        return this->Integer;
    }
};
