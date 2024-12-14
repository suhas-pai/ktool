//
//  ADT/SpecificCapArray.h
//  ktool
//
//  Created by Suhas Pai on 6/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include "ByteVector.h"

template <typename T>
struct SpecificCapArray : private ByteVector {
public:
    explicit SpecificCapArray() noexcept = default;
    explicit SpecificCapArray(uint64_t Cap) noexcept {
        Reserve(Cap);
    }

    [[nodiscard]] inline auto isFull() const noexcept {
        return this->freeSpace() == 0;
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) noexcept {
        return reinterpret_cast<T *>(DataBegin) + Index;
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) const noexcept {
        return reinterpret_cast<const T *>(DataBegin) + Index;
    }

    inline auto PushBack(const T &Item) noexcept -> decltype(*this) {
        assert(this->freeSpace() != 0);

        const auto Ptr = reinterpret_cast<const uint8_t *>(&Item);
        this->Add(Ptr, sizeof(T));

        return *this;
    }

    inline auto Reallocate(uint64_t Cap) noexcept -> decltype(*this) {
        clear();
        growTo(sizeof(T) * Cap);

        return *this;
    }

    [[nodiscard]] inline auto size() const noexcept {
        return this->ByteVector::size();
    }

    [[nodiscard]] inline auto capacity() const noexcept {
        return this->ByteVector::capacity();
    }
};
