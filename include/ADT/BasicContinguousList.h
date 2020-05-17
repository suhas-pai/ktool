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

    explicit BasicContiguousList(uint8_t *Begin, uint8_t *End) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)),
      End(reinterpret_cast<EntryType *>(End)) {}

    explicit BasicContiguousList(EntryType *Begin, uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    explicit BasicContiguousList(uint8_t *Begin, uint64_t Count) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)), End(this->Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    Iterator begin() const noexcept { return Iterator(Begin); }
    Iterator end() const noexcept { return Iterator(End); }

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

    explicit BasicContiguousList(const uint8_t *Begin,
                                 const uint8_t *End) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)),
      End(reinterpret_cast<EntryType *>(End)) {}

    explicit BasicContiguousList(EntryType *Begin, uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    explicit BasicContiguousList(const uint8_t *Begin, uint64_t Count) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)), End(this->Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    Iterator begin() const noexcept { return Iterator(Begin); }
    Iterator end() const noexcept { return Iterator(End); }

    ConstIterator cbegin() const noexcept { return ConstIterator(Begin); }
    ConstIterator cend() const noexcept { return ConstIterator(End); }

    inline EntryType *at(uint64_t Index) const noexcept {
        return Begin + Index;
    }
};

template <>
struct BasicContiguousList<uint8_t> {
    using EntryType = uint8_t;
private:
    EntryType *Begin;
    EntryType *End;
public:
    BasicContiguousList(EntryType *Begin, EntryType *End) noexcept
    : Begin(Begin), End(End) {}

    BasicContiguousList(EntryType *Begin, uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    Iterator begin() const noexcept { return Iterator(Begin); }
    Iterator end() const noexcept { return Iterator(End); }

    ConstIterator cbegin() const noexcept { return ConstIterator(Begin); }
    ConstIterator cend() const noexcept { return ConstIterator(End); }

    inline EntryType *at(uint64_t Index) const noexcept {
        return Begin + Index;
    }
};

template <>
struct BasicContiguousList<const uint8_t> {
    using EntryType = const uint8_t;
private:
    EntryType *Begin;
    EntryType *End;
public:
    BasicContiguousList(EntryType *Begin, EntryType *End) noexcept
    : Begin(Begin), End(End) {}

    BasicContiguousList(EntryType *Begin, uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    Iterator begin() const noexcept { return Iterator(Begin); }
    Iterator end() const noexcept { return Iterator(End); }

    ConstIterator cbegin() const noexcept { return ConstIterator(Begin); }
    ConstIterator cend() const noexcept { return ConstIterator(End); }

    inline EntryType *at(uint64_t Index) const noexcept {
        return Begin + Index;
    }
};
