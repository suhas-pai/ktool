//
//  ADT/ByteVector.h
//  ktool
//
//  Created by Suhas Pai on 6/2/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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
    explicit ByteVector() noexcept = default;
    explicit ByteVector(uint64_t Capacity) noexcept;

    ~ByteVector() noexcept;

    auto Reserve(uint64_t Capacity) noexcept -> decltype(*this);
    auto Add(const uint8_t *Begin, uint8_t *End) noexcept -> decltype(*this);
    auto Add(const uint8_t *Bytes, uint64_t Count = 1) noexcept
        -> decltype(*this);

    auto Remove(uint8_t *Loc, uint64_t Size) noexcept -> decltype(*this);
    auto Remove(uint8_t *Loc, uint8_t *End) noexcept -> decltype(*this);
    auto Remove(uint64_t Idx, uint64_t Size = 1) noexcept -> decltype(*this);

    [[nodiscard]] inline auto size() const noexcept {
        return static_cast<uint64_t>(DataEnd - DataBegin);
    }

    [[nodiscard]] inline auto capacity() const noexcept {
        return static_cast<uint64_t>(AllocEnd - DataBegin);
    }

    [[nodiscard]] inline auto freeSpace() const noexcept {
        return static_cast<uint64_t>(AllocEnd - DataEnd);
    }

    inline auto clear() noexcept -> decltype(*this) {
        this->DataEnd = this->DataBegin;
        return *this;
    }

    [[nodiscard]] inline auto data() const noexcept { return DataBegin; }
    [[nodiscard]] inline auto dataEnd() const noexcept {
        return static_cast<const uint8_t *>(DataEnd);
    }
};

