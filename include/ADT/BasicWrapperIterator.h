//
//  include/ADT/BasicWrapperIterator.h
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <utility>

template <typename T, typename StepType = unsigned>
struct BasicWrapperIterator {
protected:
    T Item;
public:
    constexpr BasicWrapperIterator() = delete;
    constexpr BasicWrapperIterator(const T &Item) noexcept : Item(Item) {}
    constexpr BasicWrapperIterator(T &&Item) noexcept : Item(std::move(Item)) {}

    inline BasicWrapperIterator &moveBack() noexcept {
        Item--;
        return *this;
    }

    inline BasicWrapperIterator &advance() noexcept {
        Item++;
        return *this;
    }

    [[nodiscard]]
    constexpr inline BasicWrapperIterator operator+(StepType Amt) noexcept {
        return BasicWrapperIterator(Item + Amt);
    }

    [[nodiscard]]
    constexpr inline BasicWrapperIterator operator-(StepType Amt) noexcept {
        return BasicWrapperIterator(Item - Amt);
    }

    constexpr inline BasicWrapperIterator &operator++() noexcept {
        return advance();
    }

    constexpr inline BasicWrapperIterator &operator++(int) noexcept {
        return advance();
    }

    constexpr inline BasicWrapperIterator &operator--() noexcept {
        return moveBack();
    }

    constexpr inline BasicWrapperIterator &operator--(int) noexcept {
        return moveBack();
    }

    constexpr inline BasicWrapperIterator &operator+=(StepType Amt) noexcept {
        Item += Amt;
        return *this;
    }

    constexpr inline BasicWrapperIterator &operator-=(StepType Amt) noexcept {
        Item -= Amt;
        return *this;
    }

    [[nodiscard]]
    constexpr inline bool operator==(const BasicWrapperIterator &Rhs) noexcept {
        return (Item == Rhs.Item);
    }

    [[nodiscard]]
    constexpr inline bool operator!=(const BasicWrapperIterator &Rhs) noexcept {
        return (Item != Rhs.Item);
    }

    [[nodiscard]]
    constexpr inline bool operator<(const BasicWrapperIterator &Rhs) noexcept {
        return (Item < Rhs.Item);
    }

    [[nodiscard]]
    constexpr inline bool operator<=(const BasicWrapperIterator &Rhs) noexcept {
        return (Item <= Rhs.Item);
    }

    [[nodiscard]]
    constexpr inline bool operator>(const BasicWrapperIterator &Rhs) noexcept {
        return (Item > Rhs.Item);
    }

    [[nodiscard]]
    constexpr inline bool operator>=(const BasicWrapperIterator &Rhs) noexcept {
        return (Item >= Rhs.Item);
    }

    constexpr inline T &operator*() noexcept { return Item; }
    constexpr inline const T &operator*() const noexcept { return Item; }

    constexpr inline T &operator->() noexcept { return Item; }
    constexpr inline const T &operator->() const noexcept { return Item; }
};
