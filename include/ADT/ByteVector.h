//
//  include/ADT/ByteVector.h
//  ktool
//
//  Created by Suhas Pai on 6/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>

struct ByteVector {
protected:
    uint8_t *DataBegin;
    uint8_t *DataEnd;
    uint8_t *AllocEnd;

    void grow() noexcept;
    void growTo(uint64_t Capacity) noexcept;

    uint64_t getAllocSizeForCapacity(uint64_t Capacity) noexcept;
    void growIfNecessaryForSize(uint64_t Size) noexcept;
public:
    ByteVector() noexcept = default;
    ByteVector(uint64_t Capacity) noexcept;

    ~ByteVector() noexcept;

    ByteVector &Reserve(uint64_t Capacity) noexcept;
    ByteVector &Add(const uint8_t *Bytes, uint64_t Count = 1) noexcept;
    ByteVector &Add(const uint8_t *Begin, uint8_t *End) noexcept;

    ByteVector &Remove(uint8_t *Loc, uint64_t Size) noexcept;
    ByteVector &Remove(uint8_t *Loc, uint8_t *End) noexcept;
    ByteVector &Remove(uint64_t Idx, uint64_t Size = 1) noexcept;

    [[nodiscard]] inline uint64_t size() const noexcept {
        return (DataEnd - DataBegin);
    }

    [[nodiscard]] inline uint64_t capacity() const noexcept {
        return (AllocEnd - DataBegin);
    }

    [[nodiscard]] uint64_t freeSpace() const noexcept {
        return (AllocEnd - DataEnd);
    }

    inline ByteVector &clear() noexcept {
        DataEnd = DataBegin;
        return *this;
    }

    [[nodiscard]] inline uint8_t *data() const noexcept { return DataBegin; }
    [[nodiscard]] inline const uint8_t *dataEnd() const noexcept {
        return DataEnd;
    }
};

