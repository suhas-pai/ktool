//
//  include/ADT/ArgvArray.h
//  ktool
//
//  Created by Suhas Pai on 6/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
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
    ArgvArrayIterator(const char **Iter, const char *const *End) noexcept
    : Base(Iter), End(End) {}

    inline ArgvArrayIterator &advance() noexcept {
        Item++;
        return *this;
    }

    [[nodiscard]] inline bool hasNext() const noexcept {
        return !IsAtBack();
    }

    [[nodiscard]] inline const char *getNext() const noexcept {
        assert(hasNext());
        return *(Item + 1);
    }

    [[nodiscard]] inline const char *useNext() noexcept {
        assert(hasNext());

        advance();
        return *Item;
    }

    [[nodiscard]] inline ArgvArrayIterator &operator *() noexcept {
        return *this;
    }

    [[nodiscard]] inline const ArgvArrayIterator &operator *() const noexcept {
        return *this;
    }

    [[nodiscard]] inline ArgvArrayIterator &operator->() noexcept {
        return *this;
    }

    [[nodiscard]] inline const ArgvArrayIterator &operator->() const noexcept {
        return *this;
    }

    [[nodiscard]] inline bool IsAtBack() const noexcept {
        return (Item == (End - 1));
    }

    [[nodiscard]] inline bool IsAtOrPastEnd() const noexcept {
        return (Item >= End);
    }

    [[nodiscard]] inline bool IsOption() const noexcept {
        return (getString()[0] == '-');
    }

    [[nodiscard]] inline const char **getPtr() const noexcept {
        return Item;
    }

    [[nodiscard]] inline const char *getString() const noexcept {
        assert(!IsAtOrPastEnd());
        return *Item;
    }

    [[nodiscard]] inline std::string_view GetStringView() const noexcept {
        assert(!IsAtOrPastEnd());
        return *Item;
    }

    [[nodiscard]] inline operator const char *() const noexcept {
        return getString();
    }
};

struct ArgvArray : public BasicContiguousList<const char *> {
private:
    using Base = BasicContiguousList<const char *>;
public:
    constexpr ArgvArray(const char **Begin, const char **End) noexcept
    : Base(Begin, End) {}

    constexpr ArgvArray(int Argc, const char *Argv[]) noexcept
    : Base(Argv, Argc) {}

    constexpr
    ArgvArray(const char *Argv[], int StartIndex, int EndIndex) noexcept
    : Base(Argv + StartIndex, Argv + EndIndex) {}

    using IteratorType = ArgvArrayIterator;

    [[nodiscard]] ArgvArrayIterator begin() const noexcept {
        return ArgvArrayIterator(Begin, End);
    }

    [[nodiscard]] ArgvArrayIterator end() const noexcept {
        return ArgvArrayIterator(const_cast<const char **>(End), End);
    }

    [[nodiscard]] int indexOf(const ArgvArrayIterator &Iter) const noexcept {
        return static_cast<int>(Iter.getPtr() - Begin);
    }

    [[nodiscard]] ArgvArray fromIndex(int Index) const noexcept {
        auto Begin = this->Begin + Index;
        assert(Begin <= End);

        return ArgvArray(Begin, End);
    }
};
