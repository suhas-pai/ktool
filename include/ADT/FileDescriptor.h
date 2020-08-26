//
//  include/ADT/FileDescriptor.h
//  ktool
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
    enum class OpenKind {
        Read = O_RDONLY,
        Write = O_WRONLY,
        ReadWrite = O_RDWR
    };

    enum class FlagsEnum {
        Append = O_APPEND,
        Directory = O_DIRECTORY,
        Truncate = O_TRUNC
    };

    using Flags = BasicMasksHandler<FlagsEnum>;

    constexpr FileDescriptor() noexcept = default;
    explicit FileDescriptor(const FileDescriptor &) noexcept = delete;
    constexpr FileDescriptor(FileDescriptor &&Rhs) noexcept : Fd(Rhs.Fd) {
        Rhs.Fd = -1;
    }

    constexpr FileDescriptor(int Fd) noexcept : Fd(Fd) {}
    ~FileDescriptor() noexcept;

    [[nodiscard]] static FileDescriptor
    Open(const char *Path,
         OpenKind Kind,
         Flags Flags = FileDescriptor::Flags()) noexcept;

    [[nodiscard]]
    static FileDescriptor Create(const char *Path, int Mode) noexcept;

    [[nodiscard]] inline bool IsOpen() const noexcept { return (Fd != -1); }
    [[nodiscard]] inline bool IsEmpty() const noexcept { return !IsOpen(); }
    [[nodiscard]] inline bool hasError() const noexcept { return IsEmpty(); }
    [[nodiscard]] inline int getDescriptor() const noexcept { return Fd; }

    bool Read(void *Buf, size_t Size) const noexcept;
    bool Write(const void *Buf, size_t Size) noexcept;
    bool TruncateToSize(uint64_t Length) noexcept;

    [[nodiscard]] std::optional<struct stat> GetInfo() const noexcept;
    [[nodiscard]] std::optional<uint64_t> GetSize() const noexcept;

    FileDescriptor &operator=(const FileDescriptor &) noexcept = delete;
    FileDescriptor &operator=(FileDescriptor &&) noexcept;

    void Close() noexcept;
};
