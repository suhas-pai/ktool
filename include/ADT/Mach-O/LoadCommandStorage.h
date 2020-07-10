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
        explicit LoadCommandStorageIteratorBase(PtrType *Ptr, bool IsBigEndian)
        noexcept : Base(Ptr), IsBigEndian(IsBigEndian) {};

        using DifferenceType = std::ptrdiff_t;

        inline LoadCommandStorageIteratorBase &operator++() noexcept {
            this->Item += SwitchEndianIf(getLoadCmd()->CmdSize, IsBigEndian);
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

    struct LoadCommandStorage {
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
        [[nodiscard]] static
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

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(Begin, mIsBigEndian);
        }

        [[nodiscard]] inline EndIterator end() const noexcept {
            return EndIterator(End);
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return ConstIterator(Begin, mIsBigEndian);
        }

        [[nodiscard]] inline ConstEndIterator cend() const noexcept {
            return ConstEndIterator(End);
        }

        [[nodiscard]] inline uint32_t count() const noexcept { return Ncmds; }
        [[nodiscard]] inline uint32_t size() const noexcept {
            return static_cast<uint32_t>(End - Begin);
        }

        [[nodiscard]] inline Error getError() const noexcept {
            return ErrorStorage.getValue();
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            return ErrorStorage.hasValue();
        }

        [[nodiscard]] inline bool IsBigEndian() const noexcept {
            return mIsBigEndian;
        }

        [[nodiscard]] inline uint8_t *getMap() const noexcept {
            return Begin;
        }
    };

    struct ConstLoadCommandStorage {
    public:
        using Error = LoadCommandStorage::Error;
    protected:
        union {
            const uint8_t *Begin;
            PointerErrorStorage<Error> ErrorStorage;
        };

        const uint8_t *End;
        uint8_t Ncmds;
        bool sIsBigEndian : 1;

        ConstLoadCommandStorage(Error Error) noexcept;
        explicit
        ConstLoadCommandStorage(const uint8_t *Begin,
                                const uint8_t *End,
                                uint32_t Ncmds,
                                bool IsBigEndian) noexcept;
    public:
        [[nodiscard]] static
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

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(Begin, sIsBigEndian);
        }

        [[nodiscard]] inline EndIterator end() const noexcept {
            return EndIterator(End);
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return ConstIterator(Begin, sIsBigEndian);
        }

        [[nodiscard]] inline ConstEndIterator cend() const noexcept {
            return ConstEndIterator(End);
        }

        [[nodiscard]] inline uint32_t count() const noexcept { return Ncmds; }
        [[nodiscard]] inline uint32_t size() const noexcept {
            return static_cast<uint32_t>(End - Begin);
        }

        [[nodiscard]] inline Error getError() const noexcept {
            return ErrorStorage.getValue();
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            return ErrorStorage.hasValue();
        }

        [[nodiscard]] inline bool IsBigEndian() const noexcept {
            return sIsBigEndian;
        }

        [[nodiscard]] inline const uint8_t *getMap() const noexcept {
            return Begin;
        }
    };
}
