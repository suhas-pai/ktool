//
//  ADT/FileDescriptor.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <sys/stat.h>

#include <cassert>
#include <unistd.h>

#include "ADT/FileDescriptor.h"

FileDescriptor
FileDescriptor::Open(const char *const Path,
                     const OpenKind Kind,
                     const Flags Flags) noexcept
{
    return open(Path, static_cast<int>(Kind), static_cast<int>(Flags));
}

FileDescriptor
FileDescriptor::Create(const char *const Path, const int Mode) noexcept {
    return creat(Path, Mode);
}

FileDescriptor::~FileDescriptor() noexcept {
    this->Close();
}

bool FileDescriptor::Read(void *const Buf, const size_t Size) const noexcept {
    assert(this->isOpen());
    return (static_cast<size_t>(read(Fd, Buf, Size)) == Size);
}

bool FileDescriptor::Write(const void *const Buf, const size_t Size) noexcept {
    assert(this->isOpen());
    return (static_cast<size_t>(write(Fd, Buf, Size)) == Size);
}

bool FileDescriptor::TruncateToSize(const uint64_t Size) noexcept {
    assert(this->isOpen());
    return (ftruncate(Fd, Size) == 0);
}

auto FileDescriptor::GetInfo() const noexcept
    -> std::optional<struct stat>
{
    assert(this->isOpen());

    struct stat Sbuf = {};
    if (fstat(Fd, &Sbuf) != 0) {
        return std::nullopt;
    }

    return Sbuf;
}

auto FileDescriptor::GetSize() const noexcept -> std::optional<uint64_t> {
    if (const auto Info = GetInfo()) {
        return Info->st_size;
    }

    return std::nullopt;
}

FileDescriptor &FileDescriptor::operator=(FileDescriptor &&Rhs) noexcept {
    Close();

    this->Fd = Rhs.Fd;
    Rhs.Fd = -1;

    return *this;
}

void FileDescriptor::Close() noexcept {
    if (Fd != -1) {
        close(Fd);
        Fd = -1;
    }
}
