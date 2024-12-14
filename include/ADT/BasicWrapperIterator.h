//
//  ADT/BasicWrapperIterator.h
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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

    inline auto moveBack() noexcept -> decltype(*this) {
        this->Item--;
        return *this;
    }

    inline auto advance() noexcept -> decltype(*this) {
        this->Item++;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator+(const StepType Amt) noexcept {
        return BasicWrapperIterator(this->Item + Amt);
    }

    [[nodiscard]]
    constexpr auto operator-(const StepType Amt) noexcept {
        return BasicWrapperIterator(this->Item - Amt);
    }

    constexpr auto operator++() noexcept -> decltype(*this) {
        return this->advance();
    }

    constexpr auto operator++(int) noexcept -> decltype(*this) {
        return this->advance();
    }

    constexpr auto operator--() noexcept -> decltype(*this) {
        return this->moveBack();
    }

    constexpr auto operator--(int) noexcept -> decltype(*this) {
        return this->moveBack();
    }

    constexpr auto operator+=(const StepType Amt) noexcept -> decltype(*this) {
        this->Item += Amt;
        return *this;
    }

    constexpr auto operator-=(const StepType Amt) noexcept -> decltype(*this) {
        this->Item -= Amt;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator<=>(const BasicWrapperIterator &Rhs) const noexcept {
        return this->Item <=> Rhs.Item;
    }

    [[nodiscard]]
    constexpr auto operator!=(const BasicWrapperIterator &Rhs) const noexcept {
        return this->Item != Rhs.Item;
    }

    constexpr auto &operator*() noexcept { return this->Item; }
    constexpr auto &operator*() const noexcept { return this->Item; }

    constexpr auto &operator->() noexcept { return this->Item; }
    constexpr auto &operator->() const noexcept { return this->Item; }
};
