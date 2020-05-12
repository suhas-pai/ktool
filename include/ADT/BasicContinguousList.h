//
//  include/ADT/BasicContinguousList.h
//  stool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <type_traits>

#include "BasicContinguousIterator.h"
#include "BasicWrapperIterator.h"

template <typename T>
struct BasicContiguousList {
    using EntryType = T;
private:
    EntryType *Begin;
    EntryType *End;
public:
    explicit BasicContiguousList(EntryType *Begin, EntryType *End) noexcept
    : Begin(Begin), End(End) {}

    explicit BasicContiguousList(EntryType *Begin, uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    explicit BasicContiguousList(uint8_t *Begin, uint64_t Count) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)), End(this->Begin + Count) {}

    using Iterator = BasicContiguousIterator<T>;
    using ConstIterator = BasicContiguousIterator<const T>;

    Iterator begin() noexcept { return Iterator(Begin); }
    Iterator end() noexcept { return Iterator(End); }

    ConstIterator cbegin() const noexcept { return ConstIterator(Begin); }
    ConstIterator cend() const noexcept { return ConstIterator(End); }

    inline EntryType *at(uint64_t Index) const noexcept {
        return Begin + Index;
    }
};

template <typename T>
struct BasicContiguousList<const T> {
    using EntryType = const T;
private:
    EntryType *Begin;
    EntryType *End;
public:
    explicit BasicContiguousList(EntryType *Begin, EntryType *End) noexcept
    : Begin(Begin), End(End) {}

    explicit BasicContiguousList(EntryType *Begin, uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    explicit BasicContiguousList(const uint8_t *Begin, uint64_t Count) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)), End(this->Begin + Count) {}

    using Iterator = BasicContiguousIterator<const T>;
    using ConstIterator = BasicContiguousIterator<const T>;

    Iterator begin() noexcept { return Iterator(Begin); }
    Iterator end() noexcept { return Iterator(End); }

    ConstIterator cbegin() const noexcept { return ConstIterator(Begin); }
    ConstIterator cend() const noexcept { return ConstIterator(End); }

    inline EntryType *at(uint64_t Index) const noexcept {
        return Begin + Index;
    }
};
