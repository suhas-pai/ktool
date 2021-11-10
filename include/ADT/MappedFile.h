//
//  include/ADT/MappedFile.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <sys/mman.h>
#include <sys/stat.h>

#include <cstdint>

#include "ADT/BasicMasksHandler.h"
#include "ADT/MemoryMap.h"

#include "PointerErrorStorage.h"
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
    union {
        void *Map = NULL;
        PointerErrorStorage<OpenError> ErrorStorage;
    };

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
        [[nodiscard]] constexpr bool canRead() const noexcept {
            return hasFlag(Flags::Read);
        }

        [[nodiscard]] constexpr bool canWrite() const noexcept {
            return hasFlag(Flags::Write);
        }

        [[nodiscard]] constexpr bool canExecute() const noexcept {
            return hasFlag(Flags::Execute);
        }

        constexpr Protections &setCanRead(bool Value = true) noexcept {
            setValueForFlag(Flags::Read, Value);
            return *this;
        }

        constexpr Protections &setCanWrite(bool Value = true) noexcept {
            setValueForFlag(Flags::Write, Value);
            return *this;
        }

        constexpr
        inline Protections &setCanExecute(bool Value = true) noexcept {
            setValueForFlag(Flags::Execute, Value);
            return *this;
        }

        constexpr Protections &clear() noexcept {
            this->Base::clear();
            return *this;
        }
    };

    constexpr MappedFile() noexcept {};
    explicit MappedFile(void *Map, uint64_t Size) noexcept;
    explicit MappedFile(const MappedFile &) noexcept = delete;
    explicit MappedFile(MappedFile &&) noexcept;

    inline ~MappedFile() noexcept { Close(); }

    [[nodiscard]] static MappedFile
    Open(const FileDescriptor &Fd, Protections Prot, MapKind MapKind) noexcept;

    [[nodiscard]] inline bool isEmpty() const noexcept {
        return (Map == nullptr);
    }

    [[nodiscard]] inline bool hasError() const noexcept {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] inline OpenError getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] inline uint8_t *getBegin() const noexcept {
        return static_cast<uint8_t *>(Map);
    }

    [[nodiscard]] inline uint8_t *getEnd() const noexcept {
        return getBegin() + size();
    }

    [[nodiscard]] inline uint64_t size() const noexcept { return Size; }

    [[nodiscard]] inline operator MemoryMap() const noexcept {
        const auto Map = reinterpret_cast<uint8_t *>(this->Map);
        return MemoryMap(Map, Map + Size);
    }

    [[nodiscard]] inline operator ConstMemoryMap() const noexcept {
        const auto Map = reinterpret_cast<const uint8_t *>(this->Map);
        return ConstMemoryMap(Map, Map + Size);
    }

    MappedFile &operator=(MappedFile &&Rhs) noexcept {
        Map = Rhs.Map;
        Size = Rhs.Size;

        Rhs.Map = nullptr;
        Rhs.Size = 0;

        return *this;
    }

    inline MappedFile &Close() noexcept {
        munmap(Map, Size);
        return *this;
    }
};
