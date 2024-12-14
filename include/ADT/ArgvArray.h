//
//  ADT/ArgvArray.h
//  ktool
//
//  Created by Suhas Pai on 6/4/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <string_view>

#include "BasicContiguousList.h"

struct ArgvArrayIterator : public BasicContiguousIterator<const char *> {
    friend struct ArgvArrayList;
private:
    using Base = BasicContiguousIterator<const char *>;
protected:
    const char *const *End;
public:
    constexpr
    ArgvArrayIterator(const char **const Iter, const char *const *End) noexcept
    : Base(Iter), End(End) {}

    inline auto advance() noexcept -> decltype(*this) {
        this->Item++;
        return *this;
    }

    [[nodiscard]] inline auto isAtBack() const noexcept {
        return this->Item == (this->End - 1);
    }

    [[nodiscard]] inline bool hasNext() const noexcept {
        return !this->isAtBack();
    }

    [[nodiscard]] inline auto getNext() const noexcept {
        assert(this->hasNext());
        return this->Item[1];
    }

    [[nodiscard]] inline auto useNext() noexcept {
        assert(this->hasNext());

        this->advance();
        return *this->Item;
    }

    [[nodiscard]] inline auto operator *() noexcept -> decltype(*this) {
        return *this;
    }

    [[nodiscard]] inline auto operator *() const noexcept -> decltype(*this) {
        return *this;
    }

    [[nodiscard]] inline auto operator->() noexcept -> decltype(*this) {
        return *this;
    }

    [[nodiscard]] inline auto operator->() const noexcept -> decltype(*this) {
        return *this;
    }

    [[nodiscard]] inline auto isAtOrPastEnd() const noexcept {
        return Item >= End;
    }

    [[nodiscard]] inline auto getString() const noexcept {
        assert(!this->isAtOrPastEnd());
        return *this->Item;
    }

    [[nodiscard]] inline const char &front() const noexcept {
        return this->getString()[0];
    }

    [[nodiscard]] inline auto isOption() const noexcept {
        return this->front() == '-';
    }

    [[nodiscard]] inline auto isEmptyOption() const noexcept {
        const auto String = this->getString();
        return strcmp(String, "-") == 0 || strcmp(String, "--") == 0;
    }

    [[nodiscard]] inline auto isHelpOption() const noexcept {
        const auto String = this->getString();
        const auto Result =
            strcmp(String, "--help") == 0 || strcmp(String, "--usage") == 0;

        return Result;
    }

    [[nodiscard]] inline auto isAbsolutePath() const noexcept {
        return this->front() == '/';
    }

    [[nodiscard]] inline auto getPtr() const noexcept {
        return this->Item;
    }

    [[nodiscard]] inline auto GetStringView() const noexcept {
        assert(!this->isAtOrPastEnd());
        return std::string_view(*this->Item);
    }

    [[nodiscard]] inline operator const char *() const noexcept {
        return this->getString();
    }
};

struct ArgvArray : public BasicContiguousList<const char *> {
private:
    using Base = BasicContiguousList<const char *>;
public:
    constexpr
    ArgvArray(const char **const Begin, const char **const End) noexcept
    : Base(Begin, End) {}

    constexpr ArgvArray(const int Argc, const char *Argv[]) noexcept
    : Base(Argv, Argc) {}

    constexpr
    ArgvArray(const char *Argv[],
              const int StartIndex,
              const int EndIndex) noexcept
    : Base(Argv + StartIndex, Argv + EndIndex) {}

    using IteratorType = ArgvArrayIterator;

    [[nodiscard]] inline auto getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline auto getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline auto begin() const noexcept {
        return ArgvArrayIterator(this->getBegin(), this->getEnd());
    }

    [[nodiscard]] inline auto end() const noexcept {
        const auto End = this->getEnd();
        return ArgvArrayIterator(End, End);
    }

    [[nodiscard]] inline auto front() const noexcept {
        assert(!this->empty());
        return this->begin();
    }

    [[nodiscard]] inline auto back() const noexcept {
        assert(!this->empty());
        return ArgvArrayIterator(this->getEnd() - 1, End);
    }

    [[nodiscard]]
    inline auto indexOf(const ArgvArrayIterator &Iter) const noexcept {
        return static_cast<int>(Iter.getPtr() - this->getBegin());
    }

    [[nodiscard]] inline auto fromIndex(const int Index) const noexcept {
        const auto Begin = this->getBegin();
        const auto End = this->getEnd();
        const auto NewBegin = Begin + Index;

        assert(NewBegin <= End);
        return ArgvArray(NewBegin, End);
    }
};
