//
//  include/ADT/PointerErrorStorage.h
//  stool
//
//  Created by Suhas Pai on 3/28/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <limits>

#include "TypeTraits/DisableIfNotEnumClass.h"

template <typename Enum, typename = TypeTraits::DisableIfNotEnumClass<Enum>>
struct PointerErrorStorage {
    static_assert(sizeof(uintptr_t) == sizeof(void *),
                  "uintptr_t is not pointer-size");
    static_assert(sizeof(Enum) <= sizeof(void *),
                  "Enum is larger than pointer-size");
protected:
    uintptr_t Storage;
public:
    explicit PointerErrorStorage() noexcept = default;

    PointerErrorStorage(Enum Error) noexcept
    : Storage(static_cast<uintptr_t>(Error)) {}

    static inline bool PointerHasErrorValue(uintptr_t Storage) noexcept {
        return (Storage < std::numeric_limits<uint8_t>::max());
    }

    inline bool HasValue() const noexcept {
        return PointerHasErrorValue(this->Storage);
    }

    inline Enum GetValue() const noexcept {
        if (!HasValue()) {
            return Enum::None;
        }

        return static_cast<Enum>(Storage);
    }

    inline void SetValue(const Enum &Value) noexcept {
        Storage = static_cast<uintptr_t>(Value);
    }

    inline void Clear() noexcept { Storage = 0; }
    inline void operator=(const Enum &Value) noexcept { SetValue(Value); }
};
