//
//  ADT/ExpectedPointer.h
//  ktool
//
//  Created by Suhas Pai on 3/28/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

#include "Concepts/EnumClass.h"

[[nodiscard]]
constexpr auto PointerHasErrorValue(const uintptr_t Storage) noexcept {
    return Storage <= std::numeric_limits<uint16_t>::max();
}

// Used to represent any possible error;
enum class AnyError : uint16_t {
    None,
};

template <typename T, Concepts::EnumClass Enum>
struct ExpectedPointer {
    static_assert(sizeof(uintptr_t) == sizeof(void *),
                  "uintptr_t is not pointer-size");
    static_assert(sizeof(Enum) <= sizeof(void *),
                  "Enum is larger than pointer-size");
protected:
    union {
        T *Ptr;
        uintptr_t Storage = 0;
    };
public:
    constexpr ExpectedPointer() noexcept = default;
    constexpr ExpectedPointer(const Enum Error) noexcept
    : Storage(static_cast<uintptr_t>(Error)) {}

    constexpr ExpectedPointer(T *const Value) noexcept
    : Ptr(Value) {}

    constexpr ExpectedPointer(std::nullptr_t) noexcept = delete;

    [[nodiscard]] constexpr auto hasError() const noexcept {
        return PointerHasErrorValue(this->Storage);
    }

    [[nodiscard]] constexpr auto hasValue() const noexcept {
        return !this->hasError();
    }

    [[nodiscard]] constexpr auto getError() const noexcept {
        if (this->hasError()) {
            return static_cast<Enum>(this->Storage);
        }

        return Enum::None;
    }

    [[nodiscard]] constexpr auto value() const noexcept -> T* {
        if (this->hasValue()) {
            return this->Ptr;
        }

        return nullptr;
    }

    constexpr auto setError(const Enum Value) noexcept -> decltype(*this) {
        if (Value != Enum::None) {
            this->Storage = static_cast<uintptr_t>(Value);
        }

        return *this;
    }

    constexpr auto setValue(T *const Ptr) noexcept -> decltype(*this) {
        assert(Ptr != nullptr);

        this->Ptr = Ptr;
        return *this;
    }

    constexpr auto clear() noexcept -> decltype(*this) {
        this->Storage = 0;
        return *this;
    }

    constexpr auto operator=(const Enum Error) noexcept -> decltype(*this) {
        this->setError(Error);
        return *this;
    }

    constexpr auto operator=(T *const Value) noexcept -> decltype(*this) {
        this->setValue(Value);
        return *this;
    }

    constexpr auto operator=(std::nullptr_t) noexcept = delete;

    [[nodiscard]] constexpr operator bool() const noexcept {
        return this->hasValue();
    }

    [[nodiscard]] constexpr auto operator<=>(const Enum &Value) noexcept {
        return this->getError() <=> Value;
    }

    [[nodiscard]] constexpr auto operator!=(const Enum &Value) noexcept {
        return !(*this == Value);
    }
};
