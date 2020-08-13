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

#include "TypeTraits/IsEnumClass.h"
#include "Utils/SwitchEndian.h"

template <typename T = uint64_t>
[[nodiscard]]
constexpr static inline T GetValueForMask(T Integer, T Mask) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");
    return (Integer & Mask);
}

template <typename T = uint64_t>
constexpr
static inline T SetValueForMask(T &Integer, T Mask, T Value) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");
    assert((Value & Mask) == Value &&
           "SetValueForMask(): Value falls outside mask");

    Integer &= ~Mask;
    Integer |= Value;

    return Integer;
}

template <typename T = uint64_t>
[[nodiscard]] constexpr static inline
T GetValueForMaskAndShift(T Integer, T Mask, T Shift) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");

    const auto MaskedValue = GetValueForMask(Integer, Mask);
    return (MaskedValue >> Shift);
}

template <typename T = uint64_t>
constexpr static inline
T SetValueForMaskAndShift(T &Integer, T Mask, T Shift, T Value) noexcept {
    static_assert(std::is_integral_v<T>, "T must be an integer-type");
    return SetValueForMask(Integer, Mask, (Value << Shift));
}

template <typename MaskType,
          typename IntegerTypeT = std::underlying_type_t<MaskType>>

struct BasicMasksHandler {
public:
    using IntegerType = IntegerTypeT;
    static_assert(std::is_integral_v<IntegerType>,
                  "IntegerType must be an integer type");
    static_assert(TypeTraits::IsEnumClassValue<MaskType>,
                  "EnumType must be an enum class");
protected:
    IntegerType Integer = 0;
public:
    using MaskIntegerType = std::underlying_type_t<MaskType>;

    constexpr BasicMasksHandler() noexcept = default;
    constexpr BasicMasksHandler(IntegerType Integer) noexcept
    : Integer(Integer) {};

    [[nodiscard]] constexpr
    inline IntegerType getValueForMask(MaskType Mask) const noexcept {
        return GetValueForMask(Integer, static_cast<MaskIntegerType>(Mask));
    }

    [[nodiscard]] constexpr inline bool empty() const noexcept {
        return (Integer == 0);
    }

    [[nodiscard]]
    constexpr inline bool hasValueForMask(const MaskType Mask) const noexcept {
        return getValueForMask(Mask);
    }

    constexpr inline BasicMasksHandler &
    setValueForMask(MaskType Mask, IntegerType Value) noexcept {
        const auto MaskValue = static_cast<MaskIntegerType>(Mask);
        SetValueForMask(this->Integer, MaskValue, Value);

        return *this;
    }

    [[nodiscard]]
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

    constexpr inline
    BasicMasksHandler &operator&=(const BasicMasksHandler &Rhs) const noexcept {
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

    [[nodiscard]]
    constexpr inline BasicMasksHandler operator~() const noexcept {
        return BasicMasksHandler(~Integer);
    }

    [[nodiscard]]
    constexpr inline IntegerType value() const noexcept {
        return Integer;
    }

    [[nodiscard]] constexpr inline operator IntegerType() const noexcept {
        return Integer;
    }

    constexpr inline BasicMasksHandler &clear() noexcept {
        Integer = 0;
        return *this;
    }
};

template <typename MaskType,
          typename ShiftType,
          typename IntegerTypeT = std::underlying_type_t<MaskType>>

struct BasicMasksAndShiftsHandler :
    public BasicMasksHandler<MaskType, IntegerTypeT>
{
    static_assert(TypeTraits::IsEnumClassValue<ShiftType>,
                  "ShiftsType must be an enum class");
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

    [[nodiscard]] constexpr inline IntegerType
    getValueForMaskAndShift(MaskType Mask, ShiftType Shift) const noexcept {
        const auto MaskedValue = static_cast<MaskIntegerType>(Mask);
        const auto ShiftInteger = static_cast<ShiftIntegerType>(Shift);

        return GetValueForMaskAndShift(this->Integer, MaskedValue, ShiftInteger);
    }

    [[nodiscard]] constexpr inline IntegerType
    getValueForMaskNoShift(MaskType Mask) const noexcept {
        return Base::getValueForMask(Mask);
    }

    constexpr inline BasicMasksAndShiftsHandler &
    setValueForMaskAndShift(MaskType Mask,
                            ShiftType Shift,
                            IntegerType Value) noexcept
    {
        const auto ShiftAmount = static_cast<IntegerType>(Shift);
        const auto ShiftedValue =
            static_cast<IntegerType>(Value) << ShiftAmount;

        this->setValueForMask(Mask, ShiftedValue);
        return *this;
    }

    constexpr inline BasicMasksAndShiftsHandler &
    setValueForMaskNoShift(MaskType Mask, IntegerType Value) noexcept {
        this->setValueForMask(Mask, Value);
        return *this;
    }

    constexpr inline BasicMasksAndShiftsHandler &clear() noexcept {
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

    constexpr inline BasicFlags &add(FlagsType Flag) noexcept {
        this->setValueForMask(Flag, static_cast<int>(Flag));
        return *this;
    }

    constexpr inline BasicFlags &remove(FlagsType Flag) noexcept {
        this->setValueForMask(Flag, 0);
        return *this;
    }

    [[nodiscard]]
    constexpr inline bool hasFlag(const FlagsType &Flag) const noexcept {
        return this->getValueForMask(Flag);
    }

    constexpr inline
    BasicFlags &setValueForFlag(const FlagsType &Flag, bool Value) noexcept {
        this->setValueForMask(Flag, static_cast<IntegerType>(Flag));
        return *this;
    }

    [[nodiscard]]
    constexpr inline bool hasBitsSetAfterFlag(FlagsType Flag) const noexcept {
        const auto FlagInteger = static_cast<IntegerType>(Flag);
        const auto Mask = ~(FlagInteger - 1);

        return (this->Integer & Mask);
    }

    [[nodiscard]] constexpr inline operator bool() const noexcept {
        return this->Integer;
    }
};
