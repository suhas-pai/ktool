//
//  ADT/ExpectedAlloc.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstddef>
#include "ADT/ExpectedPointer.h"

template <typename T, typename ErrorType>
struct ExpectedAlloc : public ExpectedPointer<T, ErrorType> {
private:
    using Base = ExpectedPointer<T, ErrorType>;
public:
    constexpr ExpectedAlloc() noexcept = default;
    constexpr ExpectedAlloc(const ErrorType &Error) noexcept
    : Base(Error) {}

    constexpr ExpectedAlloc(const std::nullptr_t &) noexcept = delete;
    constexpr ExpectedAlloc(T *const Ptr) noexcept : Base(Ptr) {}

    ExpectedAlloc(const ExpectedAlloc &) noexcept = delete;
    ExpectedAlloc(ExpectedAlloc &&Ty) noexcept
    : Base(reinterpret_cast<T *>(Ty.value())) {}

    [[nodiscard]] constexpr auto take() noexcept {
        const auto Result = this->value();
        this->clear();

        return Result;
    }

    inline auto replace(T *const Ptr) noexcept -> decltype(*this) {
        if (this->hasValue()) {
            delete this->value();
        }

        this->Ptr = Ptr;
        return *this;
    }

    ExpectedAlloc &operator=(const ExpectedAlloc &) noexcept = delete;

    constexpr auto operator=(ExpectedAlloc &&Rhs) noexcept
        -> decltype(*this)
    {
        if (this->hasValue()) {
            delete this->value();
        }

        this->Ptr = Rhs.Ptr;
        Rhs.Ptr = nullptr;

        return *this;
    }

    ~ExpectedAlloc() noexcept {
        if (this->hasValue()) {
            delete this->value();
        }
    }
};
