//
//  include/ADT/FileDescriptor.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include <fcntl.h>
#include <optional>

#include "ADT/BasicMasksHandler.h"

struct FileDescriptor {
protected:
    int Fd = -1;
public:
    enum class OpenType {
        Read = O_RDONLY,
        Write = O_WRONLY,
        ReadWrite = O_RDWR
    };

    enum class FlagsEnum {
        Append = O_APPEND,
        Directory = O_DIRECTORY,
        Truncate = O_TRUNC
    };

    struct Flags : public BasicMasksHandler<FlagsEnum> {};

    explicit FileDescriptor() noexcept = default;
    explicit FileDescriptor(const FileDescriptor &) noexcept = delete;
    explicit FileDescriptor(FileDescriptor &&) noexcept;

    FileDescriptor(int Fd) noexcept;
    ~FileDescriptor() noexcept;

    static FileDescriptor
    Open(const char *Path,
         OpenType Type,
         Flags Flags = FileDescriptor::Flags()) noexcept;

    static FileDescriptor Create(const char *Path, int Mode) noexcept;

    inline bool IsOpen() const noexcept { return (Fd != -1); }
    inline bool IsEmpty() const noexcept { return !IsOpen(); }
    inline bool HasError() const noexcept { return IsEmpty(); }
    inline int GetFd() const noexcept { return Fd; }

    bool Read(void *Buf, size_t Size) const noexcept;
    bool Write(const void *Buf, size_t Size) noexcept;
    bool TruncateToSize(uint64_t Length) noexcept;

    std::optional<uint64_t> GetSize() const noexcept;

    FileDescriptor &operator=(const FileDescriptor &) noexcept = delete;
    FileDescriptor &operator=(FileDescriptor &&) noexcept;

    void Close() noexcept;
};
