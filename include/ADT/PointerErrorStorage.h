//
//  include/ADT/PointerErrorStorage.h
//  ktool
//
//  Created by Suhas Pai on 3/28/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <limits>

#include "Concepts/EnumClass.h"

[[nodiscard]]
constexpr bool PointerHasErrorValue(const uintptr_t Storage) noexcept {
    return (Storage <= std::numeric_limits<uint16_t>::max());
}

template <Concepts::EnumClass Enum>
struct PointerErrorStorage {
    static_assert(sizeof(uintptr_t) == sizeof(void *),
                  "uintptr_t is not pointer-size");
    static_assert(sizeof(Enum) <= sizeof(void *),
                  "Enum is larger than pointer-size");
protected:
    uintptr_t Storage = 0;
public:
    constexpr PointerErrorStorage() noexcept = default;
    constexpr PointerErrorStorage(const Enum Error) noexcept
    : Storage(static_cast<uintptr_t>(Error)) {}

    [[nodiscard]] constexpr bool hasValue() const noexcept {
        return PointerHasErrorValue(this->Storage);
    }

    [[nodiscard]] constexpr Enum getValue() const noexcept {
        if (!hasValue()) {
            return Enum::None;
        }

        return static_cast<Enum>(Storage);
    }

    constexpr PointerErrorStorage &setValue(const Enum &Value) noexcept {
        if (Value != Enum::None) {
            Storage = static_cast<uintptr_t>(Value);
        }

        return *this;
    }

    constexpr PointerErrorStorage &clear() noexcept {
        Storage = 0;
        return *this;
    }

    constexpr PointerErrorStorage &operator=(const Enum &Value) noexcept {
        setValue(Value);
        return *this;
    }

    [[nodiscard]] inline bool operator==(const Enum &Value) noexcept {
        return static_cast<Enum>(Storage) == Value;
    }

    [[nodiscard]] inline bool operator!=(const Enum &Value) noexcept {
        return !(*this == Value);
    }
};
