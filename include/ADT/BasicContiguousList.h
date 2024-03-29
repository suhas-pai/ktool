//
//  include/ADT/BasicContinguousList.h
//  ktool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>

#include "BasicContiguousIterator.h"
#include "BasicWrapperIterator.h"

template <typename T>
struct BasicContiguousList {
    using EntryType = T;
protected:
    EntryType *Begin;
    EntryType *End;
public:
    constexpr
    BasicContiguousList(EntryType *const Begin, EntryType *const End) noexcept
    : Begin(Begin), End(End) {}

    explicit
    BasicContiguousList(uint8_t *const Begin, const uint8_t *const End) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)),
      End(reinterpret_cast<EntryType *>(End)) {}

    constexpr
    BasicContiguousList(EntryType *const Begin, const uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    constexpr
    BasicContiguousList(uint8_t *const Begin, const uint64_t Count) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)), End(this->Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    [[nodiscard]] inline Iterator begin() const noexcept {
        return Iterator(Begin);
    }

    [[nodiscard]] inline Iterator end() const noexcept {
        return Iterator(const_cast<EntryType *>(End));
    }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator(Begin);
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator(End);
    }

    [[nodiscard]] inline EntryType *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const EntryType *getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline EntryType &at(const uint64_t Index) noexcept {
        return Begin[Index];
    }

    [[nodiscard]]
    inline const EntryType &at(const uint64_t Index) const noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline uint64_t count() const noexcept {
        return (End - Begin);
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return sizeof(EntryType) * count();
    }

    [[nodiscard]] inline bool empty() const noexcept {
        return (count() == 0);
    }

    [[nodiscard]] inline EntryType &front() noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline const EntryType &front() const noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline EntryType &back() noexcept {
        assert(!empty());
        return End[-1];
    }

    [[nodiscard]] inline const EntryType &back() const noexcept {
        assert(!empty());
        return End[-1];
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

    explicit
    BasicContiguousList(const uint8_t *const Begin,
                        const uint8_t *const End) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)),
      End(reinterpret_cast<EntryType *>(End)) {}

    explicit
    BasicContiguousList(EntryType *const Begin, const uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    explicit
    BasicContiguousList(const uint8_t *const Begin,
                        const uint64_t Count) noexcept
    : Begin(reinterpret_cast<EntryType *>(Begin)), End(this->Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    [[nodiscard]] inline Iterator begin() const noexcept {
        return Iterator(Begin);
    }

    [[nodiscard]] inline Iterator end() const noexcept { return Iterator(End); }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator(Begin);
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator(End);
    }

    [[nodiscard]] inline EntryType *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const EntryType *getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline EntryType &at(const uint64_t Index) const noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline bool empty() const noexcept {
        return (count() == 0);
    }

    [[nodiscard]] inline const EntryType &front() const noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline EntryType &back() noexcept {
        assert(!empty());
        return End[-1];
    }

    [[nodiscard]] inline const EntryType &back() const noexcept {
        assert(!empty());
        return End[-1];
    }

    [[nodiscard]] inline uint64_t count() const noexcept {
        return (End - Begin);
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return sizeof(EntryType) * count();
    }
};

template <>
struct BasicContiguousList<uint8_t> {
    using EntryType = uint8_t;
private:
    EntryType *Begin;
    EntryType *End;
public:
    BasicContiguousList(EntryType *const Begin, EntryType *const End) noexcept
    : Begin(Begin), End(End) {}

    BasicContiguousList(EntryType *const Begin, const uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    [[nodiscard]] inline Iterator begin() const noexcept {
        return Iterator(Begin);
    }

    [[nodiscard]] inline Iterator end() const noexcept {
        return Iterator(const_cast<EntryType *>(End));
    }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator(Begin);
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator(End);
    }

    [[nodiscard]] inline EntryType *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const EntryType *getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline EntryType &at(const uint64_t Index) const noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline uint64_t count() const noexcept {
        return (End - Begin);
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return sizeof(EntryType) * count();
    }

    [[nodiscard]] inline bool empty() const noexcept {
        return (count() == 0);
    }

    [[nodiscard]] inline EntryType &front() noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline const EntryType &front() const noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline EntryType &back() noexcept {
        assert(!empty());
        return End[-1];
    }

    [[nodiscard]] inline const EntryType &back() const noexcept {
        assert(!empty());
        return End[-1];
    }
};

template <>
struct BasicContiguousList<const uint8_t> {
    using EntryType = const uint8_t;
private:
    EntryType *Begin;
    EntryType *End;
public:
    BasicContiguousList(EntryType *const Begin, EntryType *const End) noexcept
    : Begin(Begin), End(End) {}

    BasicContiguousList(EntryType *const Begin, const uint64_t Count) noexcept
    : Begin(Begin), End(Begin + Count) {}

    using Iterator = BasicContiguousIterator<EntryType>;
    using ConstIterator = BasicContiguousIterator<const EntryType>;

    [[nodiscard]] inline Iterator begin() const noexcept {
        return Iterator(Begin);
    }

    [[nodiscard]] inline Iterator end() const noexcept { return Iterator(End); }
    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator(Begin);
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator(End);
    }

    [[nodiscard]] inline EntryType *getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline const EntryType *getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline EntryType &at(const uint64_t Index) const noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline uint64_t count() const noexcept {
        return (End - Begin);
    }

    [[nodiscard]] inline uint64_t size() const noexcept {
        return sizeof(EntryType) * count();
    }

    [[nodiscard]] inline bool empty() const noexcept {
        return (count() == 0);
    }

    [[nodiscard]] inline EntryType &front() noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline const EntryType &front() const noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline EntryType &back() noexcept {
        assert(!empty());
        return End[-1];
    }

    [[nodiscard]] inline const EntryType &back() const noexcept {
        assert(!empty());
        return End[-1];
    }
};
