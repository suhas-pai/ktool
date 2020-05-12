//
//  include/ADT/BasicWrapperIterator.h
//  stool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

template <typename T, typename StepType = unsigned>
struct BasicWrapperIterator {
protected:
    T Item;
public:
    explicit BasicWrapperIterator() = delete;
    explicit BasicWrapperIterator(const T &Item) noexcept : Item(Item) {}
    explicit BasicWrapperIterator(T &&Item) noexcept : Item(Item) {}

    inline BasicWrapperIterator operator+(StepType Amt) noexcept {
        return BasicWrapperIterator(Item + Amt);
    }

    inline BasicWrapperIterator operator-(StepType Amt) noexcept {
        return BasicWrapperIterator(Item - Amt);
    }

    inline BasicWrapperIterator &operator++() noexcept { Item++; return *this; }
    inline BasicWrapperIterator &operator++(int) noexcept {
        Item++; return *this;
    }

    inline BasicWrapperIterator &operator--() noexcept { Item--; return *this; }
    inline BasicWrapperIterator &operator--(int) noexcept {
        Item--;
        return *this;
    }

    inline BasicWrapperIterator &operator+=(StepType Amt) noexcept {
        Item += Amt; return *this;
    }

    inline BasicWrapperIterator &operator-=(StepType Amt) noexcept {
        Item -= Amt; return *this;
    }

    inline bool operator==(const BasicWrapperIterator &Rhs) noexcept {
        return Item == Rhs.Item;
    }

    inline bool operator!=(const BasicWrapperIterator &Rhs) noexcept {
        return Item != Rhs.Item;
    }

    inline bool operator<(const BasicWrapperIterator &Rhs) noexcept {
        return Item < Rhs.Item;
    }

    inline bool operator<=(const BasicWrapperIterator &Rhs) noexcept {
        return Item <= Rhs.Item;
    }

    inline bool operator>(const BasicWrapperIterator &Rhs) noexcept {
        return Item > Rhs.Item;
    }

    inline bool operator>=(const BasicWrapperIterator &Rhs) noexcept {
        return Item >= Rhs.Item;
    }

    inline T &operator*() noexcept { return Item; }
    inline T &operator->() noexcept { return Item; }
};
