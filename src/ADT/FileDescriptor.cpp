//
//  src/ADT/FileDescriptor.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <sys/stat.h>

#include <cassert>
#include <unistd.h>

#include "FileDescriptor.h"

FileDescriptor
FileDescriptor::Open(const char *Path, OpenKind Kind, Flags Flags) noexcept {
    return open(Path, static_cast<int>(Kind), Flags);
}

FileDescriptor FileDescriptor::Create(const char *Path, int Mode) noexcept {
    return creat(Path, Mode);
}

FileDescriptor::~FileDescriptor() noexcept {
    this->Close();
}

bool FileDescriptor::Read(void *Buf, size_t Size) const noexcept {
    assert(this->isOpen());
    return (static_cast<size_t>(read(Fd, Buf, Size)) == Size);
}

bool FileDescriptor::Write(const void *Buf, size_t Size) noexcept {
    assert(this->isOpen());
    return (static_cast<size_t>(write(Fd, Buf, Size)) == Size);
}

bool FileDescriptor::TruncateToSize(uint64_t Size) noexcept {
    assert(this->isOpen());
    return (ftruncate(Fd, Size) == 0);
}

std::optional<struct stat> FileDescriptor::GetInfo() const noexcept {
    assert(this->isOpen());

    struct stat Sbuf = {};
    if (fstat(Fd, &Sbuf) != 0) {
        return std::nullopt;
    }

    return Sbuf;
}

std::optional<uint64_t> FileDescriptor::GetSize() const noexcept {
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
