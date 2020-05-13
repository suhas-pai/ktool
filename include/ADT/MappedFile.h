//
//  include/ADT/MappedFile.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <sys/mman.h>
#include <cstdint>

#include "ADT/BasicMasksHandler.h"
#include "ADT/MemoryMap.h"

#include "PointerErrorStorage.h"
#include "FileDescriptor.h"

struct MappedFile {
public:
    enum class OpenError {
        None,
        FailedToGetSize,
        EmptyFile,
        MmapCallFailed,
    };
protected:
    union {
        void *Map;
        PointerErrorStorage<OpenError> ErrorStorage;
    };

    uint64_t Size;
    MappedFile(OpenError Error) noexcept;
public:
    enum class ProtectionsEnum {
        Read = PROT_READ,
        Write = PROT_WRITE,
        Execute = PROT_EXEC
    };

    enum class Type {
        Private = MAP_PRIVATE,
        Shared = MAP_SHARED
    };

    struct Protections : public BasicMasksHandler<ProtectionsEnum> {};

    explicit MappedFile() noexcept = default;
    explicit MappedFile(void *Map, uint64_t Size) noexcept;
    explicit MappedFile(const MappedFile &) noexcept = delete;
    explicit MappedFile(MappedFile &&) noexcept;

    inline ~MappedFile() noexcept { Close(); }

    static MappedFile
    Open(const FileDescriptor &Fd, Protections Prot, Type Type) noexcept;

    inline bool IsEmpty() const noexcept { return (Map == nullptr); }
    inline bool HasError() const noexcept { return ErrorStorage.HasValue(); }
    inline OpenError GetError() const noexcept {
        return ErrorStorage.GetValue();
    }

    inline void *GetMap() const noexcept { return Map; }
    inline uint64_t GetSize() const noexcept { return Size; }

    inline operator MemoryMap() const noexcept {
        const auto Map = reinterpret_cast<uint8_t *>(this->Map);
        return MemoryMap(Map, Map + Size);
    }

    inline operator ConstMemoryMap() const noexcept {
        const auto Map = reinterpret_cast<const uint8_t *>(this->Map);
        return ConstMemoryMap(Map, Map + Size);
    }

    MappedFile &operator=(MappedFile &&) noexcept;
    inline void Close() { munmap(Map, Size); }
};
