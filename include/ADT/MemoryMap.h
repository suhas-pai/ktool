//
//  ADT/MemoryMap.h
//  ktool
//
//  Created by Suhas Pai on 4/3/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include "Range.h"

struct ConstMemoryMap {
protected:
    const uint8_t *Begin;
    const uint8_t *End;
public:
    constexpr
    ConstMemoryMap(const void *const Begin, const void *const End) noexcept
    : Begin(static_cast<const uint8_t *>(Begin)),
      End(static_cast<const uint8_t *>(End))
    {
        assert(Begin <= End);
    }

    [[nodiscard]] inline auto getBegin() const noexcept {
        return this->Begin;
    }

    [[nodiscard]] inline auto getEnd() const noexcept {
        return this->End;
    }

    [[nodiscard]] inline auto size() const noexcept {
        return this->getEnd() - this->getBegin();
    }

    template <typename T>
    [[nodiscard]] inline auto getBeginAs() const noexcept {
        return reinterpret_cast<const T *>(this->getBegin());
    }

    template <typename T>
    [[nodiscard]] inline auto getEndAs() const noexcept {
        return reinterpret_cast<const T *>(this->getEnd());
    }

    [[nodiscard]] inline auto getRange() const noexcept {
        return Range::CreateWithSize(0, this->size());
    }

    [[nodiscard]] inline bool containsOffset(uint64_t Offset) const noexcept {
        return this->getRange().hasLocation(Offset);
    }

    [[nodiscard]]
    inline bool containsEndOffset(uint64_t EndOffset) const noexcept {
        return this->getRange().hasEndLocation(EndOffset);
    }

    [[nodiscard]]
    inline bool containsLocRange(const Range &LocRange) const noexcept {
        return this->getRange().contains(LocRange);
    }

    [[nodiscard]] inline bool containsPtr(const void *Ptr) const noexcept {
        const auto Range = Range::CreateWithEnd(this->getBegin(), this->getEnd());
        return Range.hasLocation(Ptr);
    }

    [[nodiscard]] inline bool containsEndPtr(const void *Ptr) const noexcept {
        const auto Range = Range::CreateWithEnd(this->getBegin(), this->getEnd());
        return Range.hasEndLocation(Ptr);
    }

    template <typename T>
    [[nodiscard]] constexpr
    inline bool isLargeEnoughForType(uint64_t Count = 1) const noexcept {
        return this->getRange().isLargeEnoughForType<T>(Count);
    }

    [[nodiscard]]
    constexpr bool isLargeEnoughForSize(uint64_t Size) const noexcept {
        return this->getRange().isLargeEnoughForSize(Size);
    }

    [[nodiscard]]
    constexpr ConstMemoryMap mapFromPtr(const void *Begin) const noexcept {
        assert(containsPtr(Begin));
        return ConstMemoryMap(this->Begin, this->getEnd());
    }

    [[nodiscard]]
    constexpr ConstMemoryMap mapFromOffset(uint64_t Offset) const noexcept {
        assert(containsOffset(Offset));
        return ConstMemoryMap(this->getBegin() + Offset, this->getEnd());
    }

    [[nodiscard]] constexpr ConstMemoryMap
    mapFromLocRange(const Range &LocRange) const noexcept {
        assert(containsLocRange(LocRange));

        const auto ThisBegin = this->getBegin();
        const auto Begin = ThisBegin + LocRange.getBegin();
        const auto End = ThisBegin + LocRange.getEnd().value();

        return ConstMemoryMap(Begin, End);
    }
};

struct MemoryMap : public ConstMemoryMap {
public:
    constexpr MemoryMap(uint8_t *const Begin, uint8_t *const End) noexcept
    : ConstMemoryMap(Begin, End) {}

    [[nodiscard]] inline uint8_t *getBegin() const noexcept {
        return const_cast<uint8_t *>(ConstMemoryMap::getBegin());
    }

    [[nodiscard]] inline uint8_t *getEnd() const noexcept {
        return const_cast<uint8_t *>(ConstMemoryMap::getEnd());
    }

    template <typename T>
    [[nodiscard]] inline T *getBeginAs() const noexcept {
        return reinterpret_cast<T *>(getBegin());
    }

    template <typename T>
    [[nodiscard]] inline T *getEndAs() const noexcept {
        return reinterpret_cast<T *>(getEnd());
    }

    [[nodiscard]]
    constexpr MemoryMap mapFromPtr(uint8_t *const Begin) const noexcept {
        assert(containsPtr(Begin));
        return MemoryMap(Begin, getEnd());
    }

    [[nodiscard]] constexpr
    ConstMemoryMap constMapFromPtr(const uint8_t *Begin) const noexcept {
        return ConstMemoryMap::mapFromPtr(Begin);
    }

    [[nodiscard]]
    constexpr MemoryMap mapFromOffset(const uint64_t Offset) const noexcept {
        assert(containsOffset(Offset));
        return MemoryMap(getBegin() + Offset, getEnd());
    }

    [[nodiscard]] constexpr
    ConstMemoryMap constMapFromOffset(uint64_t Offset) const noexcept {
        return ConstMemoryMap::mapFromOffset(Offset);
    }

    [[nodiscard]] constexpr ConstMemoryMap
    constMapFromLocRange(const Range &LocRange) const noexcept {
        return ConstMemoryMap::mapFromLocRange(LocRange);
    }

    [[nodiscard]] constexpr MemoryMap
    mapFromLocRange(const Range &LocRange) const noexcept {
        assert(this->containsLocRange(LocRange));

        const auto ThisBegin = this->getBegin();
        const auto Begin = ThisBegin + LocRange.getBegin();
        const auto End = ThisBegin + LocRange.getEnd().value();

        return MemoryMap(Begin, End);
    }
};
