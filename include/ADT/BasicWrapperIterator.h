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
    constexpr BasicWrapperIterator operator+(StepType Amt) noexcept {
        return BasicWrapperIterator(Item + Amt);
    }

    [[nodiscard]]
    constexpr BasicWrapperIterator operator-(StepType Amt) noexcept {
        return BasicWrapperIterator(Item - Amt);
    }

    constexpr BasicWrapperIterator &operator++() noexcept {
        return advance();
    }

    constexpr BasicWrapperIterator &operator++(int) noexcept {
        return advance();
    }

    constexpr BasicWrapperIterator &operator--() noexcept {
        return moveBack();
    }

    constexpr BasicWrapperIterator &operator--(int) noexcept {
        return moveBack();
    }

    constexpr BasicWrapperIterator &operator+=(StepType Amt) noexcept {
        Item += Amt;
        return *this;
    }

    constexpr BasicWrapperIterator &operator-=(StepType Amt) noexcept {
        Item -= Amt;
        return *this;
    }

    [[nodiscard]]
    constexpr bool operator==(const BasicWrapperIterator &Rhs) const noexcept {
        return (Item == Rhs.Item);
    }

    [[nodiscard]]
    constexpr bool operator!=(const BasicWrapperIterator &Rhs) const noexcept {
        return (Item != Rhs.Item);
    }

    [[nodiscard]]
    constexpr bool operator<(const BasicWrapperIterator &Rhs) const noexcept {
        return (Item < Rhs.Item);
    }

    [[nodiscard]]
    constexpr bool operator<=(const BasicWrapperIterator &Rhs) const noexcept {
        return (Item <= Rhs.Item);
    }

    [[nodiscard]]
    constexpr bool operator>(const BasicWrapperIterator &Rhs) const noexcept {
        return (Item > Rhs.Item);
    }

    [[nodiscard]]
    constexpr bool operator>=(const BasicWrapperIterator &Rhs) const noexcept {
        return (Item >= Rhs.Item);
    }

    constexpr T &operator*() noexcept { return Item; }
    constexpr const T &operator*() const noexcept { return Item; }

    constexpr T &operator->() noexcept { return Item; }
    constexpr const T &operator->() const noexcept { return Item; }
};
