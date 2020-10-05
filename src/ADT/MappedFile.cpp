//
//  src/ADT/MappedFile.cpp
//  ktool
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
MappedFile::Open(const FileDescriptor &Fd,
                 Protections Prot,
                 MapKind MapKind) noexcept
{
    const auto InfoOpt = Fd.GetInfo();
    if (!InfoOpt.has_value()) {
        return OpenError::FailedToGetInfo;
    }

    const auto &Info = InfoOpt.value();
    const auto Mode = Info.st_mode;

    if (!S_ISREG(Mode) && !S_ISLNK(Mode)) {
        return OpenError::NotAFile;
    }

    const auto Size = Info.st_size;
    if (Size == 0) {
        return OpenError::EmptyFile;
    }

    const auto FdV = Fd.getDescriptor();
    const auto MapKindInt = static_cast<int>(MapKind);
    const auto Map = mmap(nullptr, Size, Prot, MapKindInt, FdV, 0);

    if (Map == MAP_FAILED) {
        return OpenError::MmapCallFailed;
    }

    return MappedFile(Map, Size);
}
