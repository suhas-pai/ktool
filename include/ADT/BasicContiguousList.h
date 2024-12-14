//
//  ADT/BasicContiguousList.h
//  ktool
//
//  Created by Suhas Pai on 3/8/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include "BasicContiguousIterator.h"

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

    [[nodiscard]] inline auto begin() const noexcept {
        return Iterator(Begin);
    }

    [[nodiscard]] inline auto end() const noexcept {
        return Iterator(const_cast<EntryType *>(End));
    }

    [[nodiscard]] inline auto cbegin() const noexcept {
        return ConstIterator(Begin);
    }

    [[nodiscard]] inline auto cend() const noexcept {
        return ConstIterator(End);
    }

    [[nodiscard]] inline auto getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline auto getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) const noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline auto count() const noexcept {
        return End - Begin;
    }

    [[nodiscard]] inline auto size() const noexcept {
        return sizeof(EntryType) * count();
    }

    [[nodiscard]] inline auto empty() const noexcept {
        return count() == 0;
    }

    [[nodiscard]] inline auto &front() noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline auto &front() const noexcept {
        assert(!empty());
        return *Begin;
    }

    [[nodiscard]] inline auto &back() noexcept {
        assert(!empty());
        return End[-1];
    }

    [[nodiscard]] inline auto &back() const noexcept {
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

    [[nodiscard]] inline auto getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline auto getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline auto begin() const noexcept {
        return Iterator(this->getBegin());
    }

    [[nodiscard]] inline auto end() const noexcept {
        return Iterator(this->getEnd());
    }

    [[nodiscard]] inline auto cbegin() const noexcept {
        return ConstIterator(this->getBegin());
    }

    [[nodiscard]] inline auto cend() const noexcept {
        return ConstIterator(this->getEnd());
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) const noexcept {
        return Begin[Index];
    }

    [[nodiscard]] inline auto empty() const noexcept {
        return this->count() == 0;
    }

    [[nodiscard]] inline auto &front() const noexcept {
        assert(!this->empty());
        return *this->getBegin();
    }

    [[nodiscard]] inline auto &back() noexcept {
        assert(!this->empty());
        return this->getEnd()[-1];
    }

    [[nodiscard]] inline auto &back() const noexcept {
        assert(!this->empty());
        return this->getEnd()[-1];
    }

    [[nodiscard]] inline auto count() const noexcept {
        return this->getEnd() - this->getBegin();
    }

    [[nodiscard]] inline auto size() const noexcept {
        return sizeof(EntryType) * this->count();
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

    [[nodiscard]] inline auto getBegin() const noexcept {
        return this->Begin;
    }

    [[nodiscard]] inline auto getEnd() const noexcept {
        return this->End;
    }

    [[nodiscard]] inline auto begin() const noexcept {
        return Iterator(this->getBegin());
    }

    [[nodiscard]] inline auto end() const noexcept {
        return Iterator(const_cast<EntryType *>(this->getEnd()));
    }

    [[nodiscard]] inline auto cbegin() const noexcept {
        return ConstIterator(this->getBegin());
    }

    [[nodiscard]] inline auto cend() const noexcept {
        return ConstIterator(this->getEnd());
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) const noexcept {
        return this->getBegin()[Index];
    }

    [[nodiscard]] inline auto count() const noexcept {
        return this->getEnd() - this->getBegin();
    }

    [[nodiscard]] inline auto size() const noexcept {
        return sizeof(EntryType) * this->count();
    }

    [[nodiscard]] inline auto empty() const noexcept {
        return this->count() == 0;
    }

    [[nodiscard]] inline auto &front() noexcept {
        assert(!this->empty());
        return *this->getBegin();
    }

    [[nodiscard]] inline auto &front() const noexcept {
        assert(!this->empty());
        return *this->getBegin();
    }

    [[nodiscard]] inline auto &back() noexcept {
        assert(!this->empty());
        return this->getEnd()[-1];
    }

    [[nodiscard]] inline auto &back() const noexcept {
        assert(!this->empty());
        return this->getEnd()[-1];
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

    [[nodiscard]] inline auto getBegin() const noexcept {
        return Begin;
    }

    [[nodiscard]] inline auto getEnd() const noexcept {
        return End;
    }

    [[nodiscard]] inline auto begin() const noexcept {
        return Iterator(this->getBegin());
    }

    [[nodiscard]] inline auto end() const noexcept {
        return Iterator(this->getEnd());
    }

    [[nodiscard]] inline auto cbegin() const noexcept {
        return ConstIterator(this->getBegin());
    }

    [[nodiscard]] inline auto cend() const noexcept {
        return ConstIterator(this->getEnd());
    }

    [[nodiscard]] inline auto &at(const uint64_t Index) const noexcept {
        return this->getBegin()[Index];
    }

    [[nodiscard]] inline auto count() const noexcept {
        return static_cast<uint64_t>(End - Begin);
    }

    [[nodiscard]] inline auto size() const noexcept {
        return sizeof(EntryType) * count();
    }

    [[nodiscard]] inline auto empty() const noexcept {
        return this->count() == 0;
    }

    [[nodiscard]] inline auto &front() noexcept {
        assert(!this->empty());
        return *this->getBegin();
    }

    [[nodiscard]] inline auto &front() const noexcept {
        assert(!this->empty());
        return *this->getBegin();
    }

    [[nodiscard]] inline auto &back() noexcept {
        assert(!this->empty());
        return this->getEnd()[-1];
    }

    [[nodiscard]] inline auto &back() const noexcept {
        assert(!this->empty());
        return this->getEnd()[-1];
    }
};
