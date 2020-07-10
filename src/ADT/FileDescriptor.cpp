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
FileDescriptor::Open(const char *Path, OpenType Type, Flags Flags) noexcept {
    return open(Path, static_cast<int>(Type), Flags);
}

FileDescriptor FileDescriptor::Create(const char *Path, int Mode) noexcept {
    return creat(Path, Mode);
}

FileDescriptor::~FileDescriptor() noexcept {
    this->Close();
}

bool FileDescriptor::Read(void *Buf, size_t Size) const noexcept {
    assert(IsOpen());
    return (static_cast<size_t>(read(Fd, Buf, Size)) == Size);
}

bool FileDescriptor::Write(const void *Buf, size_t Size) noexcept {
    assert(IsOpen());
    return (static_cast<size_t>(write(Fd, Buf, Size)) == Size);
}

bool FileDescriptor::TruncateToSize(uint64_t Size) noexcept {
    assert(IsOpen());
    return (ftruncate(Fd, Size) == 0);
}

std::optional<uint64_t> FileDescriptor::GetSize() const noexcept {
    assert(IsOpen());

    struct stat Sbuf = {};
    if (fstat(Fd, &Sbuf) == 0) {
        return Sbuf.st_size;
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
