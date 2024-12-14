//
//  ADT/MappedFile.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <sys/mman.h>
#include <sys/stat.h>

#include <cstdint>

#include "ADT/BasicMasksHandler.h"
#include "ADT/MemoryMap.h"

#include "ExpectedPointer.h"
#include "FileDescriptor.h"

struct MappedFile {
public:
    enum class OpenError {
        None,
        FailedToGetInfo,
        NotAFile,
        EmptyFile,
        MmapCallFailed,
    };
protected:
    ExpectedPointer<void, OpenError> MapOrError;

    uint64_t Size = 0;
    MappedFile(OpenError Error) noexcept;
public:
    enum class ProtectionsFlags {
        Read = PROT_READ,
        Write = PROT_WRITE,
        Execute = PROT_EXEC
    };

    enum class MapKind {
        Private = MAP_PRIVATE,
        Shared = MAP_SHARED
    };

    struct Protections : public BasicFlags<ProtectionsFlags> {
    private:
        using Base = BasicFlags<ProtectionsFlags>;
    public:
        using Base::Base;
        using Flags = ProtectionsFlags;

        [[nodiscard]] constexpr auto canRead() const noexcept {
            return this->hasFlag(Flags::Read);
        }

        [[nodiscard]] constexpr auto canWrite() const noexcept {
            return this->hasFlag(Flags::Write);
        }

        [[nodiscard]] constexpr auto canExecute() const noexcept {
            return this->hasFlag(Flags::Execute);
        }

        constexpr auto setCanRead(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::Read, Value);
            return *this;
        }

        constexpr auto setCanWrite(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::Write, Value);
            return *this;
        }

        constexpr auto setCanExecute(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::Execute, Value);
            return *this;
        }

        constexpr auto clear() noexcept -> decltype(*this) {
            this->Base::clear();
            return *this;
        }
    };

    constexpr MappedFile() noexcept {};
    explicit MappedFile(void *Map, uint64_t Size) noexcept;
    explicit MappedFile(const MappedFile &) noexcept = delete;
    explicit MappedFile(MappedFile &&) noexcept;

    inline ~MappedFile() noexcept { this->Close(); }

    [[nodiscard]] static MappedFile
    Open(const FileDescriptor &Fd, Protections Prot, MapKind MapKind) noexcept;

    [[nodiscard]] inline auto isEmpty() const noexcept {
        return MapOrError.hasError();
    }

    [[nodiscard]] inline auto hasError() const noexcept {
        return MapOrError.hasError();
    }

    [[nodiscard]] inline auto getError() const noexcept {
        return MapOrError.getError();
    }

    [[nodiscard]] inline auto getBegin() const noexcept {
        return static_cast<uint8_t *>(MapOrError.value());
    }

    [[nodiscard]] inline auto size() const noexcept { return this->Size; }
    [[nodiscard]] inline auto getEnd() const noexcept {
        return this->getBegin() + this->size();
    }

    [[nodiscard]] inline operator MemoryMap() const noexcept {
        const auto Map = reinterpret_cast<uint8_t *>(this->getBegin());
        return MemoryMap(Map, Map + Size);
    }

    [[nodiscard]] inline operator ConstMemoryMap() const noexcept {
        const auto Map = reinterpret_cast<const uint8_t *>(this->getBegin());
        return ConstMemoryMap(Map, Map + Size);
    }

    inline auto operator=(MappedFile &&Rhs) noexcept -> decltype(*this) {
        this->MapOrError.setValue(Rhs.MapOrError.value());
        Size = Rhs.Size;

        Rhs.MapOrError.clear();
        Rhs.Size = 0;

        return *this;
    }

    inline auto Close() noexcept -> decltype(*this) {
        munmap(this->getBegin(), this->size());
        return *this;
    }
};
