//
//  ADT/FileDescriptor.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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

    [[nodiscard]] inline auto isOpen() const noexcept { return Fd != -1; }
    [[nodiscard]] inline auto isEmpty() const noexcept {
        return !this->isOpen();
    }

    [[nodiscard]] inline auto hasError() const noexcept {
        return this->isEmpty();
    }

    [[nodiscard]] inline int getDescriptor() const noexcept {
        assert(this->isOpen());
        return Fd;
    }

    bool Read(void *Buf, size_t Size) const noexcept;
    bool Write(const void *Buf, size_t Size) noexcept;
    bool TruncateToSize(uint64_t Length) noexcept;

    [[nodiscard]] auto GetInfo() const noexcept -> std::optional<struct stat>;
    [[nodiscard]] auto GetSize() const noexcept -> std::optional<uint64_t>;

    auto operator=(const FileDescriptor &) noexcept = delete;
    auto operator=(FileDescriptor &&) noexcept -> decltype(*this);

    void Close() noexcept;
};
