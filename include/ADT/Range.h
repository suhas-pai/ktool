//
//  ADT/Range.h
//  ktool
//
//  Created by Suhas Pai on 3/14/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstdint>
#include <optional>

#include "Utils/DoesOverflow.h"

struct Range {
private:
    uint64_t Begin = 0;
    uint64_t Size = 0;

    constexpr explicit Range(const uint64_t Begin, const uint64_t Size) noexcept
    : Begin(Begin), Size(Size) {}
public:
    constexpr Range() noexcept = default;
    [[nodiscard]] static inline auto Empty() noexcept {
        return Range();
    }

    [[nodiscard]] constexpr static
    auto CreateWithSize(const uint64_t Begin, const uint64_t Size) noexcept {
        return Range(Begin, Size);
    }

    template <typename T = uint64_t>
    [[nodiscard]] constexpr static
    auto CreateWithSize(void *const Begin, const uint64_t Size) noexcept {
        const auto BeginInt = reinterpret_cast<uint64_t>(Begin);
        return Range::CreateWithSize<T>(BeginInt, Size);
    }

    [[nodiscard]] constexpr static
    auto CreateWithEnd(const uint64_t Begin, const uint64_t End) noexcept {
        assert(Begin <= End);
        return Range(Begin, (End - Begin));
    }

    [[nodiscard]] static inline auto
    CreateWithEnd(const void *const Begin, const void *const End) noexcept {
        const auto BeginInt = reinterpret_cast<uint64_t>(Begin);
        const auto EndInt = reinterpret_cast<uint64_t>(End);

        return Range::CreateWithEnd(BeginInt, EndInt);
    }

    [[nodiscard]] constexpr auto getBegin() const noexcept {
        return this->Begin;
    }

    [[nodiscard]] constexpr auto getEnd() const noexcept
        -> std::optional<uint64_t>
    {
        auto End = uint64_t();
        if (DoesAddOverflow(this->Begin, this->Size, &End)) {
            return std::nullopt;
        }

        return End;
    }

    [[nodiscard]] constexpr auto overflows() const noexcept {
        auto End = uint64_t();
        return DoesAddOverflow(this->Begin, this->Size, &End);
    }

    constexpr auto setBegin(const uint64_t Begin) noexcept
        -> decltype(*this)
    {
        this->Begin = Begin;
        return *this;
    }

    constexpr auto setSize(const uint64_t Size) noexcept
        -> decltype(*this)
    {
        this->Size = Size;
        return *this;
    }

    [[nodiscard]] constexpr auto size() const noexcept {
        return this->Size;
    }

    [[nodiscard]] constexpr auto empty() const noexcept {
        return this->size() == 0;
    }

    [[nodiscard]]
    constexpr auto hasLocation(const uint64_t Location) const noexcept {
        return Location >= this->getBegin() &&
               (Location - this->getBegin()) < this->size();
    }

    [[nodiscard]] constexpr
    auto hasEndLocation(const uint64_t EndLoc) const noexcept {
        return EndLoc > this->getBegin() &&
               (EndLoc - this->getBegin()) <= this->size();
    }

    [[nodiscard]] constexpr
    auto hasLocation(const void *const Location) const noexcept {
        return this->hasLocation(reinterpret_cast<uint64_t>(Location));
    }

    [[nodiscard]] constexpr
    auto hasEndLocation(const void *const EndLocation) const noexcept {
        return this->hasEndLocation(
            reinterpret_cast<uint64_t>(EndLocation));
    }

    [[nodiscard]]
    constexpr auto hasIndex(const uint64_t Index) const noexcept {
        return Index < this->size();
    }

    [[nodiscard]] constexpr
    auto hasEndIndex(const uint64_t EndIndex) const noexcept {
        return EndIndex != 0 && EndIndex <= this->size();
    }

    [[nodiscard]] constexpr
    auto indexForLocation(const uint64_t Location) const noexcept {
        assert(this->hasLocation(Location));
        return Location - this->getBegin();
    }

    template <typename T>
    [[nodiscard]] constexpr auto
    canContainTypeAtLocation(const uint64_t Location,
                             const uint64_t Count = 1) const noexcept
    {
        auto Size = uint64_t();
        if (DoesMultiplyOverflow(sizeof(T), Count, &Size)) {
            return false;
        }

        auto End = uint64_t();
        if (DoesAddOverflow(Location, Size, &End)) {
            return false;
        }

        return this->hasLocation(Location) && this->hasEndLocation(End);
    }

    template <typename T>
    [[nodiscard]] constexpr auto
    canContainTypeAtLocation(const void *const Location,
                             const uint64_t Count = 1) const noexcept
    {
        return canContainTypeAtLocation<T>(
            reinterpret_cast<uint64_t>(Location), Count);
    }

    template <typename T>
    [[nodiscard]] constexpr auto
    containsTypeAtRelativeLocation(const uint64_t Location,
                                   const uint64_t Count = 1) const noexcept
    {
        auto Size = uint64_t();
        if (DoesMultiplyOverflow(sizeof(T), Count, &Size)) {
            return false;
        }

        auto End = uint64_t();
        if (DoesAddOverflow(Location, sizeof(T), &End)) {
            return false;
        }

        return this->hasIndex(Location) && this->hasEndIndex(End);
    }

    [[nodiscard]] constexpr auto contains(const Range &Range) const noexcept {
        const auto ContainsBegin = this->hasLocation(Range.Begin);
        if (!ContainsBegin) {
            return false;
        }

        const auto Index = this->indexForLocation(Range.Begin);
        if (!this->hasIndex(Index)) {
            return false;
        }

        return this->size() - Index >= Range.size();
    }

    [[nodiscard]]
    constexpr auto containsAsIndex(const Range &LocRange) const noexcept {
        if (!this->hasIndex(LocRange.getBegin())) {
            return false;
        }

        return this->size() >= LocRange.size() - LocRange.getBegin();
    }

    [[nodiscard]]
    constexpr auto overlaps(const Range &LocRange) const noexcept {
        return this->hasLocation(LocRange.getBegin()) ||
               LocRange.hasLocation(this->getBegin());
    }

    [[nodiscard]]
    constexpr auto sizeFromIndex(const uint64_t Location) const noexcept {
        assert(this->hasIndex(Location));
        return this->size() - (Location - this->getBegin());
    }

    [[nodiscard]]
    constexpr auto sizeFromLocation(const uint64_t Location) const noexcept {
        assert(this->hasLocation(Location));
        return this->sizeFromIndex(Location - Begin);
    }

    [[nodiscard]]
    constexpr auto goesPastEnd(const uint64_t Size) const noexcept {
        return this->size() > Size;
    }

    [[nodiscard]]
    constexpr auto isLargeEnoughForSize(const uint64_t Size) const noexcept {
        return this->size() >= Size;
    }

    template <typename T>
    [[nodiscard]] constexpr
    auto isLargeEnoughForType(const uint64_t Count = 1) const noexcept {
        auto Size = uint64_t();
        if (DoesAddOverflow(sizeof(T), Count, &Size)) {
            return false;
        }

        return isLargeEnoughForSize(Size);
    }
};
