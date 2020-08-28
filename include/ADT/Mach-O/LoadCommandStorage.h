//
//  include/ADT/Mach-O/LoadCommandStorage.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <cstddef>

#include "ADT/BasicWrapperIterator.h"
#include "ADT/PointerErrorStorage.h"

#include "LoadCommands.h"
#include "LoadCommandTemplates.h"

namespace MachO {
    template <typename PtrType>
    struct LoadCommandStorageEndIteratorBase {
    protected:
        PtrType *End;
    public:
        explicit LoadCommandStorageEndIteratorBase(PtrType *End) : End(End) {}
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
        LoadCommandStorageIteratorBase(PtrType *Ptr, bool IsBigEndian) noexcept
        : Base(Ptr), IsBigEndian(IsBigEndian) {};

        using DifferenceType = std::ptrdiff_t;
        inline LoadCommandStorageIteratorBase &operator++() noexcept {
            this->Item += getLoadCmd()->getCmdSize(IsBigEndian);
            return *this;
        }

        inline LoadCommandStorageIteratorBase operator++(int) noexcept {
            ++(*this);
            return *this;
        }

        inline LoadCommandStorageIteratorBase &
        operator+=(DifferenceType Amt) noexcept {
            for (auto I = DifferenceType(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline LoadCommandType *getLoadCmd() const noexcept {
            return reinterpret_cast<LoadCommandType *>(this->Item);
        }

        template <LoadCommand::Kind Kind>
        [[nodiscard]] inline LoadCommandPtrTypeFromKind<Kind> getAs() noexcept {
            using LCPtrType = LoadCommandPtrTypeFromKind<Kind>;
            return dyn_cast<LCPtrType>(getLoadCmd());
        }

        [[nodiscard]] inline LoadCommandType &operator*() const noexcept {
            return *getLoadCmd();
        }

        [[nodiscard]] inline LoadCommandType *operator->() const noexcept {
            return getLoadCmd();
        }

        using EndIterator = LoadCommandStorageEndIteratorBase<PtrType>;

        [[nodiscard]]
        inline bool operator==(const EndIterator &Iter) const noexcept {
            return (this->Item >= Iter.getEnd());
        }

        [[nodiscard]]
        inline bool operator!=(const EndIterator &Iter) const noexcept {
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
        union {
            const uint8_t *Begin;
            PointerErrorStorage<Error> ErrorStorage;
        };

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
        [[nodiscard]] static ConstLoadCommandStorage
        Open(const uint8_t *Begin,
             uint32_t Count,
             uint32_t Size,
             bool IsBigEndian,
             bool Is64Bit,
             bool Verify) noexcept;

        using Iterator =
            LoadCommandStorageIteratorBase<const uint8_t, const LoadCommand>;

        using ConstIterator = Iterator;

        using EndIterator = LoadCommandStorageEndIteratorBase<const uint8_t>;
        using ConstEndIterator = EndIterator;

        [[nodiscard]] inline bool hasError() const noexcept {
            return ErrorStorage.hasValue();
        }

        [[nodiscard]] inline Error getError() const noexcept {
            return ErrorStorage.getValue();
        }

        [[nodiscard]] inline const uint8_t *getBegin() const noexcept {
            assert(!hasError());
            return Begin;
        }

        [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
            assert(!hasError());
            return ConstMemoryMap(Begin, End);
        }

        [[nodiscard]] inline const uint8_t *getEnd() const noexcept {
            assert(!hasError());
            return End;
        }

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline EndIterator end() const noexcept {
            return EndIterator(getEnd());
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return ConstIterator(getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline ConstEndIterator cend() const noexcept {
            return ConstEndIterator(getEnd());
        }

        [[nodiscard]] inline uint32_t count() const noexcept {
            assert(!hasError());
            return Count;
        }

        [[nodiscard]] inline uint32_t size() const noexcept {
            assert(!hasError());
            return static_cast<uint32_t>(End - Begin);
        }

        [[nodiscard]] inline bool IsBigEndian() const noexcept {
            assert(!hasError());
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
        [[nodiscard]] static inline LoadCommandStorage
        Open(uint8_t *Begin,
             uint32_t Count,
             uint32_t Size,
             bool IsBigEndian,
             bool Is64Bit,
             bool Verify) noexcept
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

        [[nodiscard]] inline uint8_t *getBegin() const noexcept {
            return const_cast<uint8_t *>(ConstLoadCommandStorage::getBegin());
        }

        [[nodiscard]] inline uint8_t *getEnd() const noexcept {
            return const_cast<uint8_t *>(ConstLoadCommandStorage::getEnd());
        }

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline EndIterator end() const noexcept {
            return EndIterator(getEnd());
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return ConstIterator(getBegin(), sIsBigEndian);
        }

        [[nodiscard]] inline ConstEndIterator cend() const noexcept {
            return ConstEndIterator(getEnd());
        }
    };
}
