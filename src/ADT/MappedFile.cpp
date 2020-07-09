//
//  src/ADT/MappedFile.cpp
//  stool
//
//  Created by Suhas Pai on 4/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "MappedFile.h"

MappedFile::MappedFile(OpenError Error) noexcept : ErrorStorage(Error) {}
MappedFile::MappedFile(void *Map, uint64_t Size) noexcept
: Map(Map), Size(Size) {}

MappedFile::MappedFile(MappedFile &&Rhs) noexcept
: Map(Rhs.Map), Size(Rhs.Size) {
    Rhs.Map = nullptr;
    Rhs.Size = 0;
}

MappedFile
MappedFile::Open(const FileDescriptor &Fd, Protections Prot, Type Type) noexcept
{
    const auto OptSize = Fd.GetSize();
    if (!OptSize) {
        return OpenError::FailedToGetSize;
    }

    const auto FdV = Fd.getDescriptor();
    const auto Size = OptSize.value();

    if (Size == 0) {
        return OpenError::EmptyFile;
    }

    const auto Map = mmap(nullptr, Size, Prot, static_cast<int>(Type), FdV, 0);
    if (Map == MAP_FAILED) {
        return OpenError::MmapCallFailed;
    }

    return MappedFile(Map, Size);
}

MappedFile &MappedFile::operator=(MappedFile &&Rhs) noexcept {
    Map = Rhs.Map;
    Size = Rhs.Size;

    Rhs.Map = nullptr;
    Rhs.Size = 0;

    return *this;
}
