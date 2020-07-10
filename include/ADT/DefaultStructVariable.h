//
//  include/ADT/DefaultStructVariable.h
//  ktool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <utility>

template <typename T>
struct DefaultStructVariable {
protected:
    T Item;
public:
    constexpr DefaultStructVariable() = delete;
    constexpr DefaultStructVariable(const T &Item) noexcept : Item(Item) {}
    constexpr DefaultStructVariable(T &&Item) noexcept
    : Item(std::move(Item)) {}

    constexpr inline operator const T &() const noexcept { return Item; }
    constexpr inline operator T &() noexcept { return Item; }
};
