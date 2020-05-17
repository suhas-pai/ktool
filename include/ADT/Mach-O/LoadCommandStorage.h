//
//  include/ADT/Mach-O/LoadCommandStorage.h
//  stool
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
        PtrType *GetEnd() const noexcept { return End; }
    };

    template <typename PtrType, typename LoadCommandType>
    class LoadCommandStorageIteratorBase :
        public BasicWrapperIterator<PtrType *> {
    private:
        using Base = BasicWrapperIterator<PtrType *>;
    protected:
        bool IsBigEndian : 1;
    public:
        explicit LoadCommandStorageIteratorBase(PtrType *Ptr, bool IsBigEndian)
        noexcept : Base(Ptr), IsBigEndian(IsBigEndian) {};

        using DifferenceType = std::ptrdiff_t;

        inline LoadCommandStorageIteratorBase &operator++() noexcept {
            this->Item += SwitchEndianIf(GetLoadCmd()->CmdSize, IsBigEndian);
            return *this;
        }

        inline LoadCommandStorageIteratorBase operator++(int) noexcept {
            ++(*this);
            return *this;
        }

        inline LoadCommandStorageIteratorBase &
        operator+=(DifferenceType Amt) noexcept {
            for (DifferenceType I = DifferenceType(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        inline LoadCommandType *GetLoadCmd() const noexcept {
            return reinterpret_cast<LoadCommandType *>(this->Item);
        }

        inline LoadCommandType &operator*() const noexcept {
            return *GetLoadCmd();
        }

        inline LoadCommandType *operator->() const noexcept {
            return GetLoadCmd();
        }

        using EndIterator = LoadCommandStorageEndIteratorBase<PtrType>;

        inline bool operator==(const EndIterator &Iter) const noexcept {
            return (this->Item >= Iter.GetEnd());
        }

        inline bool operator!=(const EndIterator &Iter) const noexcept {
            return !(*this == Iter);
        }
    };

    class LoadCommandStorage {
    public:
        enum class Error : uintptr_t {
            None,
            NoLoadCommands,

            CmdSizeTooSmall,
            CmdSizeNotAligned,

            SizeOfCmdsTooSmall,
        };
    protected:
        union {
            uint8_t *Begin;
            PointerErrorStorage<Error> ErrorStorage;
        };

        uint8_t *End;
        uint8_t Ncmds;
        bool mIsBigEndian : 1;

        LoadCommandStorage(Error Error) noexcept;
        explicit LoadCommandStorage(uint8_t *Begin,
                                    uint8_t *End,
                                    uint32_t Ncmds,
                                    bool IsBigEndian) noexcept;
    public:
        static
        LoadCommandStorage Open(uint8_t *Begin,
                                uint32_t Ncmds,
                                uint32_t SizeOfCmds,
                                bool IsBigEndian,
                                bool Is64Bit,
                                bool Verify) noexcept;

        using Iterator =
            LoadCommandStorageIteratorBase<uint8_t, MachO::LoadCommand>;

        using ConstIterator =
            LoadCommandStorageIteratorBase<const uint8_t,
                                           const MachO::LoadCommand>;

        using EndIterator = LoadCommandStorageEndIteratorBase<uint8_t>;
        using ConstEndIterator =
            LoadCommandStorageEndIteratorBase<const uint8_t>;

        inline Iterator begin() const noexcept {
            return Iterator(Begin, mIsBigEndian);
        }

        inline EndIterator end() const noexcept {
            return EndIterator(End);
        }

        inline ConstIterator cbegin() const noexcept {
            return ConstIterator(Begin, mIsBigEndian);
        }

        inline ConstEndIterator cend() const noexcept {
            return ConstEndIterator(End);
        }

        inline uint32_t Count() const noexcept { return Ncmds; }
        inline uint32_t Size() const noexcept {
            return static_cast<uint32_t>(End - Begin);
        }

        inline Error GetError() const noexcept {
            return ErrorStorage.GetValue();
        }

        inline bool HasError() const noexcept {
            return ErrorStorage.HasValue();
        }

        inline bool IsBigEndian() const noexcept {
            return mIsBigEndian;
        }
    };

    class ConstLoadCommandStorage {
    public:
        using Error = LoadCommandStorage::Error;
    protected:
        union {
            const uint8_t *Begin;
            PointerErrorStorage<Error> ErrorStorage;
        };

        const uint8_t *End;
        uint8_t Ncmds;
        bool mIsBigEndian : 1;

        ConstLoadCommandStorage(Error Error) noexcept;
        explicit ConstLoadCommandStorage(const uint8_t *Begin,
                                         const uint8_t *End,
                                         uint32_t Ncmds,
                                         bool IsBigEndian) noexcept;
    public:
        static
        ConstLoadCommandStorage Open(const uint8_t *Begin,
                                     uint32_t Ncmds,
                                     uint32_t SizeOfCmds,
                                     bool IsBigEndian,
                                     bool Is64Bit,
                                     bool Verify) noexcept;

        using Iterator =
            LoadCommandStorageIteratorBase<const uint8_t,
                                           const MachO::LoadCommand>;

        using ConstIterator = Iterator;

        using EndIterator = LoadCommandStorageEndIteratorBase<const uint8_t>;
        using ConstEndIterator = EndIterator;

        inline Iterator begin() const noexcept {
            return Iterator(Begin, mIsBigEndian);
        }

        inline EndIterator end() const noexcept {
            return EndIterator(End);
        }

        inline ConstIterator cbegin() const noexcept {
            return ConstIterator(Begin, mIsBigEndian);
        }

        inline ConstEndIterator cend() const noexcept {
            return ConstEndIterator(End);
        }

        inline uint32_t Count() const noexcept { return Ncmds; }
        inline uint32_t Size() const noexcept {
            return static_cast<uint32_t>(End - Begin);
        }

        inline Error GetError() const noexcept {
            return ErrorStorage.GetValue();
        }

        inline bool HasError() const noexcept {
            return ErrorStorage.HasValue();
        }

        inline bool IsBigEndian() const noexcept {
            return mIsBigEndian;
        }
    };
}
