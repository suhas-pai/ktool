//
//  include/ADT/MemoryMap.h
//  ktool
//
//  Created by Suhas Pai on 4/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <type_traits>

#include "LocationRange.h"
#include "RelativeRange.h"

struct ConstMemoryMap {
protected:
    const uint8_t *Begin;
    const uint8_t *End;
public:
    constexpr
    ConstMemoryMap(const void *Begin, const void *End) noexcept
    : Begin(static_cast<const uint8_t *>(Begin)),
      End(static_cast<const uint8_t *>(End))
    {
        assert(Begin <= End);
    }

    [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const uint8_t *getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return (getEnd() - getBegin());
    }

    template <typename T>
    [[nodiscard]] inline const T *getBeginAs() const noexcept {
        return reinterpret_cast<const T *>(getBegin());
    }

    template <typename T>
    [[nodiscard]] inline const T *getEndAs() const noexcept {
        return reinterpret_cast<const T *>(getEnd());
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept {
        return RelativeRange(size());
    }

    [[nodiscard]] inline bool containsOffset(uint64_t Offset) const noexcept {
        return getRange().containsLocation(Offset);
    }

    [[nodiscard]]
    inline bool containsEndOffset(uint64_t EndOffset) const noexcept {
        return getRange().containsEndLocation(EndOffset);
    }

    [[nodiscard]]
    inline bool containsLocRange(const LocationRange &LocRange) const noexcept {
        return getRange().containsLocRange(LocRange);
    }

    [[nodiscard]] inline bool containsPtr(const void *Ptr) const noexcept {
        const auto Range = LocationRange::CreateWithEnd(getBegin(), getEnd());
        return Range.containsLocation(Ptr);
    }

    [[nodiscard]] inline bool containsEndPtr(const void *Ptr) const noexcept {
        const auto Range = LocationRange::CreateWithEnd(getBegin(), getEnd());
        return Range.containsEndLocation(Ptr);
    }

    template <typename T>
    [[nodiscard]] constexpr
    inline bool isLargeEnoughForType(uint64_t Count = 1) const noexcept {
        return getRange().isLargeEnoughForType<T>(Count);
    }

    [[nodiscard]]
    constexpr bool isLargeEnoughForSize(uint64_t Size) const noexcept {
        return getRange().isLargeEnoughForSize(Size);
    }

    [[nodiscard]] constexpr
    inline ConstMemoryMap mapFromPtr(const void *Begin) const noexcept {
        assert(containsPtr(Begin));
        return ConstMemoryMap(Begin, getEnd());
    }

    [[nodiscard]] constexpr
    inline ConstMemoryMap mapFromOffset(uint64_t Offset) const noexcept {
        assert(containsOffset(Offset));
        return ConstMemoryMap(getBegin() + Offset, getEnd());
    }

    [[nodiscard]] constexpr ConstMemoryMap
    mapFromLocRange(const LocationRange &LocRange) const noexcept {
        assert(containsLocRange(LocRange));

        const auto ThisBegin = getBegin();
        const auto Begin = ThisBegin + LocRange.getBegin();
        const auto End = ThisBegin + LocRange.getEnd();

        return ConstMemoryMap(Begin, End);
    }
};

struct MemoryMap : public ConstMemoryMap {
public:
    constexpr MemoryMap(uint8_t *Begin, uint8_t *End) noexcept
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
    constexpr MemoryMap mapFromPtr(uint8_t *Begin) const noexcept {
        assert(containsPtr(Begin));
        return MemoryMap(Begin, getEnd());
    }

    [[nodiscard]] constexpr
    inline ConstMemoryMap constMapFromPtr(uint8_t *Begin) const noexcept {
        return ConstMemoryMap::mapFromPtr(Begin);
    }

    [[nodiscard]]
    constexpr MemoryMap mapFromOffset(uint64_t Offset) const noexcept {
        assert(containsOffset(Offset));
        return MemoryMap(getBegin() + Offset, getEnd());
    }

    [[nodiscard]] constexpr
    inline ConstMemoryMap constMapFromOffset(uint64_t Offset) const noexcept {
        return ConstMemoryMap::mapFromOffset(Offset);
    }

    [[nodiscard]] constexpr ConstMemoryMap
    constMapFromLocRange(const LocationRange &LocRange) const noexcept {
        return ConstMemoryMap::mapFromLocRange(LocRange);
    }

    [[nodiscard]] constexpr MemoryMap
    mapFromLocRange(const LocationRange &LocRange) const noexcept {
        assert(containsLocRange(LocRange));

        const auto ThisBegin = getBegin();
        const auto Begin = ThisBegin + LocRange.getBegin();
        const auto End = ThisBegin + LocRange.getEnd();

        return MemoryMap(Begin, End);
    }
};
