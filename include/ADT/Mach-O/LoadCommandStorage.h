//
//  ADT/Mach-O/LoadCommandStorage.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <cstddef>

#include "ADT/BasicWrapperIterator.h"
#include "ADT/ExpectedPointer.h"

#include "LoadCommands.h"
#include "LoadCommandTemplates.h"

namespace MachO {
    template <typename PtrType>
    struct LoadCommandStorageEndIteratorBase {
    protected:
        PtrType *End;
    public:
        explicit
        LoadCommandStorageEndIteratorBase(PtrType *const End) : End(End) {}

        [[nodiscard]] PtrType *getEnd() const noexcept { return End; }
    };

    template <typename PtrType, typename LoadCommandType>
    struct LoadCommandStorageIteratorBase :
        public BasicWrapperIterator<PtrType *> {
    private:
        using Base = BasicWrapperIterator<PtrType *>;
    protected:
        bool IsBigEndian : 1;
    public:
        explicit
        LoadCommandStorageIteratorBase(PtrType *const Ptr,
                                       const bool IsBigEndian) noexcept
        : Base(Ptr), IsBigEndian(IsBigEndian) {};

        using DifferenceType = std::ptrdiff_t;
        inline auto operator++() noexcept -> decltype(*this) {
            this->Item += getLoadCmd()->getCmdSize(IsBigEndian);
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            ++(*this);
            return *this;
        }

        inline auto operator+=(DifferenceType Amt) noexcept -> decltype(*this) {
            for (auto I = DifferenceType(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline auto getLoadCmd() const noexcept {
            return reinterpret_cast<LoadCommandType *>(this->Item);
        }

        template <LoadCommand::Kind Kind>
        [[nodiscard]] inline auto getAs() noexcept
            -> LoadCommandPtrTypeFromKind<Kind>
        {
            using LCPtrType = LoadCommandPtrTypeFromKind<Kind>;
            return dyn_cast<LCPtrType>(this->getLoadCmd());
        }

        [[nodiscard]] inline auto &operator*() const noexcept {
            return *this->getLoadCmd();
        }

        [[nodiscard]] inline auto operator->() const noexcept {
            return this->getLoadCmd();
        }

        using EndIterator = LoadCommandStorageEndIteratorBase<PtrType>;

        [[nodiscard]]
        inline auto operator==(const EndIterator &Iter) const noexcept {
            return this->Item >= Iter.getEnd();
        }

        [[nodiscard]]
        inline auto operator!=(const EndIterator &Iter) const noexcept {
            return !(*this == Iter);
        }
    };

    struct ConstLoadCommandStorage {
    public:
        enum class Error : uintptr_t {
            None,
            NoLoadCommands,

            CmdSizeTooSmall,
            CmdSizeNotAligned,

            StorageSizeTooSmall,
        };
    protected:
        ExpectedPointer<const uint8_t, Error> BeginOrError;

        const uint8_t *End;
        uint32_t Count;
        bool sIsBigEndian : 1;

        ConstLoadCommandStorage(Error Error) noexcept;

        explicit
        ConstLoadCommandStorage(const uint8_t *Begin,
                                const uint8_t *End,
                                uint32_t Count,
                                bool IsBigEndian) noexcept;
    public:
        [[nodiscard]] static auto
        Open(const uint8_t *Begin,
             uint32_t Count,
             uint32_t Size,
             bool IsBigEndian,
             bool Is64Bit,
             bool Verify) noexcept
                -> ConstLoadCommandStorage;

        using Iterator =
            LoadCommandStorageIteratorBase<const uint8_t, const LoadCommand>;

        using ConstIterator = Iterator;

        using EndIterator = LoadCommandStorageEndIteratorBase<const uint8_t>;
        using ConstEndIterator = EndIterator;

        [[nodiscard]] inline auto hasError() const noexcept {
            return BeginOrError.hasError();
        }

        [[nodiscard]] inline auto getError() const noexcept {
            return BeginOrError.getError();
        }

        [[nodiscard]] inline auto getBegin() const noexcept {
            assert(!this->hasError());
            return BeginOrError.value();
        }

        [[nodiscard]] inline auto getEnd() const noexcept {
            assert(!this->hasError());
            return End;
        }

        [[nodiscard]] inline auto getMap() const noexcept {
            assert(!this->hasError());
            return ConstMemoryMap(this->getBegin(), this->getEnd());
        }

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(this->getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return EndIterator(this->getEnd());
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return ConstIterator(this->getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return ConstEndIterator(this->getEnd());
        }

        [[nodiscard]] inline auto count() const noexcept {
            assert(!this->hasError());
            return Count;
        }

        [[nodiscard]] inline auto size() const noexcept {
            assert(!this->hasError());
            return static_cast<uint32_t>(this->getEnd() - this->getBegin());
        }

        [[nodiscard]] inline auto isBigEndian() const noexcept {
            assert(!this->hasError());
            return sIsBigEndian;
        }
    };

    struct LoadCommandStorage : public ConstLoadCommandStorage {
    public:
        using Error = ConstLoadCommandStorage::Error;
    protected:
        LoadCommandStorage(Error Error) noexcept;

        explicit
        LoadCommandStorage(uint8_t *Begin,
                           uint8_t *End,
                           uint32_t Count,
                           bool IsBigEndian) noexcept;
    public:
        [[nodiscard]] static inline auto
        Open(uint8_t *const Begin,
             const uint32_t Count,
             const uint32_t Size,
             const bool IsBigEndian,
             const bool Is64Bit,
             const bool Verify) noexcept
        {
            const auto Result =
                ConstLoadCommandStorage::Open(Begin,
                                              Count,
                                              Size,
                                              IsBigEndian,
                                              Is64Bit,
                                              Verify);

            return *reinterpret_cast<const LoadCommandStorage *>(&Result);
        }

        using Iterator =
            LoadCommandStorageIteratorBase<uint8_t, LoadCommand>;
        using ConstIterator =
            LoadCommandStorageIteratorBase<const uint8_t, const LoadCommand>;

        using EndIterator = LoadCommandStorageEndIteratorBase<uint8_t>;
        using ConstEndIterator =
            LoadCommandStorageEndIteratorBase<const uint8_t>;

        [[nodiscard]] inline auto getBegin() const noexcept {
            return const_cast<uint8_t *>(ConstLoadCommandStorage::getBegin());
        }

        [[nodiscard]] inline auto getEnd() const noexcept {
            return const_cast<uint8_t *>(ConstLoadCommandStorage::getEnd());
        }

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return EndIterator(getEnd());
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return ConstIterator(getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return ConstEndIterator(getEnd());
        }
    };
}
