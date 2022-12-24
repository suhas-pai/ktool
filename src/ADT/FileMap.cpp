//
//  ADT/FileMap.cpp
//  ktool
//
//  Created by suhaspai on 11/13/22.
//

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "ADT/FileMap.h"

namespace ADT {
    FileMap::FileMap(const char *const Path, const Prot Prot) noexcept {
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
            fprintf(stderr,
                    "Failed to open file (at path %s), error=%s\n",
                    Path,
                    strerror(errno));
            exit(1);
        }

        struct stat Sbuf = {};
        if (fstat(Fd, &Sbuf) != 0) {
            fprintf(stderr,
                    "Failed to get info on file (at path %s), error=%s\n",
                    Path,
                    strerror(errno));
            exit(1);
        }

        const auto Flags = MAP_PRIVATE;
        void *const Map =
            mmap(NULL,
                 static_cast<size_t>(Sbuf.st_size),
                 static_cast<int>(Prot),
                 Flags,
                 Fd,
                 0);

        if (Map == MAP_FAILED) {
            fprintf(stderr,
                    "Failed to open memory-map of file (at path %s), "
                    "error=%s\n",
                    Path,
                    strerror(errno));
            exit(1);
        }

        this->Base = Map;
        this->Size = static_cast<size_t>(Sbuf.st_size);
    }

    FileMap::~FileMap() noexcept {
        if (munmap(this->Base, this->Size) != 0) {
            fprintf(stderr,
                    "Internal Error: munmap() failed with error: %s\n",
                    strerror(errno));
            exit(1);
        }

        this->Base = NULL;
        this->Size = 0;
    }
}
