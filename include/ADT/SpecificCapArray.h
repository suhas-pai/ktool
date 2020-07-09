//
//  include/ADT/SpecificCapArray.h
//  stool
//
//  Created by Suhas Pai on 6/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "ByteVector.h"

template <typename T>
struct SpecificCapArray : private ByteVector {
public:
    SpecificCapArray() noexcept = default;
    SpecificCapArray(uint64_t Cap) noexcept {
        Reserve(Cap);
    }

    [[nodiscard]] inline bool IsFull() const noexcept {
        return (freeSpace() == 0);
    }

    [[nodiscard]] inline T &at(uint64_t Index) noexcept {
        return reinterpret_cast<T *>(DataBegin) + Index;
    }

    [[nodiscard]] inline T &at(uint64_t Index) const noexcept {
        return reinterpret_cast<const T *>(DataBegin) + Index;
    }

    inline SpecificCapArray &PushBack(const T &Item) noexcept {
        assert(freeSpace() != 0);

        const auto Ptr = reinterpret_cast<const uint8_t *>(&Item);
        this->Add(Ptr, sizeof(T));

        return *this;
    }

    inline SpecificCapArray &Reallocate(uint64_t Cap) noexcept {
        clear();
        this->growTo(sizeof(T) * Cap);

        return *this;
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return this->ByteVector::size();
    }

    [[nodiscard]] inline uint64_t capacity() const noexcept {
        return this->ByteVector::capacity();
    }
};
