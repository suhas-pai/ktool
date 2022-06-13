//
//  include/ADT/EnumHelper.h
//  ktool
//
//  Created by Suhas Pai on 5/23/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "Concepts/EnumClass.h"
#include "External/magic_enum.h"
#include "LargestIntHelper.h"

template <Concepts::EnumClass Enum>
struct EnumHelper {
    using IntegerType = std::underlying_type_t<Enum>;
    [[nodiscard]] constexpr static Enum max() noexcept {
        constexpr auto List = magic_enum::enum_values<Enum>();
        constexpr auto FirstNumber = static_cast<IntegerType>(List[0]);

        auto MaxNumber = LargestIntHelper<IntegerType>(FirstNumber);
        for (auto Value : List) {
            MaxNumber = static_cast<IntegerType>(Value);
        }

        return static_cast<Enum>(MaxNumber);
    }

    template <typename Function>
    [[nodiscard]] constexpr
    static uint64_t GetLongestAssocLength(const Function &Func) noexcept {
        constexpr auto List = magic_enum::enum_values<Enum>();
        auto Length = LargestIntHelper(Func(List.at(0)).length());

        const auto End = List.end();
        for (auto Iter = List.begin() + 1; Iter != End; Iter++) {
            Length = Func(*Iter).length();
        }

        return Length;
    }
};
