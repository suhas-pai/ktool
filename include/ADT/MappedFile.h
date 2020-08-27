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
    enum class ProtectionsEnum {
        Read = PROT_READ,
        Write = PROT_WRITE,
        Execute = PROT_EXEC
    };

    enum class MapKind {
        Private = MAP_PRIVATE,
        Shared = MAP_SHARED
    };

    struct Protections : public BasicFlags<ProtectionsEnum> {
    private:
        using Base = BasicFlags<ProtectionsEnum>;
    public:
        using Base::Base;

        using Flags = ProtectionsEnum;
        [[nodiscard]] constexpr inline bool canRead() const noexcept {
            return hasFlag(Flags::Read);
        }

        [[nodiscard]] constexpr inline bool canWrite() const noexcept {
            return hasFlag(Flags::Write);
        }

        [[nodiscard]] constexpr inline bool canExecute() const noexcept {
            return hasFlag(Flags::Execute);
        }

        constexpr inline Protections &setCanRead(bool Value = true) noexcept {
            setValueForFlag(Flags::Read, Value);
            return *this;
        }

        constexpr inline Protections &setCanWrite(bool Value = true) noexcept {
            setValueForFlag(Flags::Write, Value);
            return *this;
        }

        constexpr
        inline Protections &setCanExecute(bool Value = true) noexcept {
            setValueForFlag(Flags::Execute, Value);
            return *this;
        }

        constexpr inline Protections &clear() noexcept {
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

    [[nodiscard]] inline bool IsEmpty() const noexcept {
        return (Map == nullptr);
    }

    [[nodiscard]] inline bool hasError() const noexcept {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] inline OpenError getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] inline void *GetMap() const noexcept { return Map; }
    [[nodiscard]] inline uint64_t GetSize() const noexcept { return Size; }

    [[nodiscard]] inline operator MemoryMap() const noexcept {
        const auto Map = reinterpret_cast<uint8_t *>(this->Map);
        return MemoryMap(Map, Map + Size);
    }

    [[nodiscard]] inline operator ConstMemoryMap() const noexcept {
        const auto Map = reinterpret_cast<const uint8_t *>(this->Map);
        return ConstMemoryMap(Map, Map + Size);
    }

    MappedFile &operator=(MappedFile &&) noexcept;
    inline MappedFile &Close() noexcept {
        munmap(Map, Size);
        return *this;
    }
};
