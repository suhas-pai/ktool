//
//  ADT/FileMap.cpp
//  ktool
//
//  Created by suhaspai on 11/13/22.
//

#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "ADT/FileMap.h"

namespace ADT {
    auto
    FileMap::Open(const char *const Path, const Prot Prot) noexcept
        -> std::expected<FileMap *, OpenError>
    {
        auto Mode = O_RDONLY;
        switch (Prot) {
            case Prot::Read:
            case Prot::ReadExecute:
            case Prot::Execute:
                break;
            case Prot::ReadWrite:
            case Prot::ReadWriteExecute:
                Mode = O_RDWR;
                break;
            case Prot::Write:
            case Prot::WriteExecute:
                Mode = O_WRONLY;
                break;
        }

        const auto Fd = open(Path, Mode);
        if (Fd == -1) {
            return std::unexpected(OpenError::FailedToOpen);
        }

        struct stat Sbuf = {};
        if (fstat(Fd, &Sbuf) != 0) {
            close(Fd);
            return std::unexpected(OpenError::FailedToStat);
        }

        const auto Flags = MAP_PRIVATE;
        void *const Map =
            mmap(nullptr,
                 static_cast<size_t>(Sbuf.st_size),
                 static_cast<int>(Prot),
                 Flags,
                 Fd,
                 0);

        close(Fd);
        if (Map == MAP_FAILED) {
            return std::unexpected(OpenError::FailedToMemMap);
        }

        return new FileMap(Map, static_cast<uint64_t>(Sbuf.st_size));
    }

    FileMap::~FileMap() noexcept {
        if (munmap(this->Base, this->Size) != 0) {
            fprintf(stderr,
                    "Internal Error: munmap() failed with error: %s\n",
                    strerror(errno));
            exit(1);
        }

        this->Base = nullptr;
        this->Size = 0;
    }
}
