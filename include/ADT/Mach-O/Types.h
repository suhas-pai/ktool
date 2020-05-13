//
//  include/ADT/Mach-O/Types.h
//  stool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <bitset>
#include "TypeTraits/IsEnumClass.h"

namespace MachO {
    template <typename MasksType,
              typename IntegerType = std::underlying_type_t<MasksType>>

    struct BasicMasksHandlerBigEndian {
    protected:
        IntegerType Integer;
    public:
        explicit BasicMasksHandlerBigEndian() noexcept = default;
        BasicMasksHandlerBigEndian(IntegerType Integer) noexcept
        : Integer(Integer) {};

        using MaskIntegerType = std::underlying_type_t<MasksType>;

        [[nodiscard]] constexpr inline IntegerType
        GetValueForMask(MasksType Mask, bool IsBigEndian) const noexcept {
            const auto SwappedInteger = GetIntegerValue(IsBigEndian);
            const auto MaskInteger =
                static_cast<std::underlying_type_t<MasksType>>(Mask);

            return (SwappedInteger & MaskInteger);
        }

        [[nodiscard]] constexpr
        inline bool Has(const MasksType Mask, bool IsBigEndian) const noexcept {
            return GetValueForMask(Mask, IsBigEndian);
        }

        constexpr inline BasicMasksHandlerBigEndian &
        Add(const MasksType Mask, bool IsBigEndian) noexcept {
            const auto SwappedInteger = GetIntegerValue(IsBigEndian);
            const auto MaskInteger = static_cast<MaskIntegerType>(Mask);
            const auto MaskedInteger = (SwappedInteger | MaskInteger);

            Integer = SwitchEndianIf(MaskedInteger, IsBigEndian);
            return *this;
        }

        constexpr inline BasicMasksHandlerBigEndian &
        Remove(const MasksType Mask, bool IsBigEndian) noexcept {
            const auto MaskInteger = static_cast<MaskIntegerType>(Mask);
            const auto SwappedInteger = GetIntegerValue(IsBigEndian);
            const auto MaskedInteger = (SwappedInteger & ~MaskInteger);

            Integer = SwitchEndianIf(MaskedInteger, IsBigEndian);
            return *this;
        }

        constexpr inline uint64_t GetSetCount(bool IsBigEndian) const noexcept {
            const auto BitSet = std::bitset<32>(GetIntegerValue(IsBigEndian));
            return BitSet.count();
        }

        inline bool
        operator==(const BasicMasksHandlerBigEndian &Rhs) const noexcept {
            return Integer == Rhs.Integer;
        }

        inline bool
        operator!=(const BasicMasksHandlerBigEndian &Rhs) const noexcept {
            return Integer != Rhs.Integer;
        }

        inline bool
        operator<(const BasicMasksHandlerBigEndian&Rhs) const noexcept {
            return Integer < Rhs.Integer;
        }

        inline bool
        operator<=(const BasicMasksHandlerBigEndian&Rhs) const noexcept {
            return Integer <= Rhs.Integer;
        }

        inline bool
        operator>(const BasicMasksHandlerBigEndian&Rhs) const noexcept {
            return Integer > Rhs.Integer;
        }

        inline bool
        operator>=(const BasicMasksHandlerBigEndian&Rhs) const noexcept {
            return Integer >= Rhs.Integer;
        }

        [[nodiscard]] constexpr
        inline IntegerType GetIntegerValue(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Integer, IsBigEndian);
        }

        inline operator IntegerType() const noexcept { return Integer; }
    };

    template <typename MasksType, typename ShiftsType,
              typename IntegerType = std::underlying_type_t<MasksType>>
    
    struct BasicMasksAndShifts :
        public BasicMasksHandlerBigEndian<MasksType, IntegerType>
    {
        static_assert(TypeTraits::IsEnumClassValue<ShiftsType>,
                      "ShiftsType must be an enum class");
    private:
        using BaseType = BasicMasksHandlerBigEndian<MasksType, IntegerType>;
    public:
        explicit BasicMasksAndShifts() noexcept = default;

        BasicMasksAndShifts(IntegerType Integer) noexcept :
        BaseType(Integer) {};

        using ShiftIntegerType = std::underlying_type_t<ShiftsType>;

        [[nodiscard]] inline IntegerType
        GetValueForMask(MasksType Mask,
                        ShiftsType Shift,
                        bool IsBigEndian) const noexcept
        {
            const auto MaskedValue =
                BaseType::GetValueForMask(Mask, IsBigEndian);

            const auto ShiftInteger =
                static_cast<ShiftIntegerType>(Shift);

            return (MaskedValue >> ShiftInteger);
        }
    };

    template <typename FlagsType>
    struct BasicFlags : public BasicMasksHandlerBigEndian<FlagsType> {
        operator bool() const noexcept { return this->Integer; }
    };
}
