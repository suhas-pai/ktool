//
//  Operations/Base.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <cstdio>

#include "ADT/FileMap.h"
#include "Operations/Base.h"

namespace Operations {
    auto PrintUnsupportedError(const std::string_view Path) noexcept {
        fprintf(stderr,
                "print-header isn't supported for file at path %s\n",
                Path.data());
        exit(1);
    }

    auto
    Base::runAndHandleFile(const HandleFileOptions Options) const noexcept
        -> RunResult
    {
        const auto FileMap =
            ADT::FileMap(Options.Path.data(), ADT::FileMap::Prot::Read);

        const auto ObjectOrError = Objects::Open(FileMap.map());
        if (ObjectOrError.hasError()) {
            fprintf(stderr,
                    "File (at path %s) is not recognized\n",
                    Options.Path.data());
            exit(1);
        }

        const auto Object = ObjectOrError.ptr();
        const auto Result = runAndHandleFile(*Object, Options);

        delete Object;
        return Result;
    }

    auto
    Base::runAndHandleFile(const Objects::Base &Object,
                           const HandleFileOptions Options) const noexcept
        -> RunResult
    {
        switch (Object.kind()) {
            case Objects::Kind::None:
                assert(false && "Got Object-Kind None in runAndHandleFile");
            case Objects::Kind::MachO:
                if (supportsObjectKind(Objects::Kind::MachO)) {
                    PrintUnsupportedError(Options.Path);
                }

                return run(Object);
            case Objects::Kind::FatMachO: {
                if (!supportsObjectKind(Objects::Kind::MachO)) {
                    if (supportsObjectKind(Objects::Kind::FatMachO)) {
                        if (Options.ArchIndex != -1) {
                            fputs("Operation doesn't support Mach-O Files, but "
                                  "does support Fat Mach-O Files.\nDrop "
                                  "--arch-index option to run on Fat Mach-O "
                                  "Option\n",
                                  stderr);
                            exit(1);
                        }

                        return run(Object);
                    }

                    PrintUnsupportedError(Options.Path);
                }

                if (Options.ArchIndex == -1) {
                    fputs("Operation doesn't support Fat Mach-O Files. "
                          "Please select an arch by its index using "
                          "option --arch-index\n",
                          stderr);
                    exit(1);
                }

                const auto Fat = static_cast<const Objects::FatMachO &>(Object);
                const auto ArchIndex = static_cast<uint32_t>(Options.ArchIndex);
                const auto ArchCount = Fat.archCount();

                if (Options.ArchIndex >= ArchCount) {
                    fprintf(stderr,
                            "An Arch-Index of %d is invalid. The provided Fat "
                            "Mach-O file has %d architectures\n",
                            ArchIndex,
                            ArchCount);
                    exit(1);
                }

                const auto ArchObject = Fat.getArchObjectAtIndex(ArchIndex);
                const auto Error = ArchObject.error();

                if (!Error.isNone()) {
                    if (Error.isUnrecognizedFormat()) {
                        fprintf(stderr,
                                "Architecture at index %d is of an "
                                "unrecognized format\n",
                                ArchIndex);
                        exit(1);
                    }

                    switch (Error.Kind) {
                        case Objects::Kind::None:
                            assert(false &&
                                   "Got Object-Kind None for OpenError\n");
                        case Objects::Kind::MachO:
                            using ErrorKind = Objects::MachO::OpenError;
                            switch (ErrorKind(Error.Error)) {
                                case Objects::MachO::OpenError::None:
                                    assert(false &&
                                           "Got Error None for MachO "
                                           "OpenError");
                                case Objects::MachO::OpenError::WrongFormat:
                                    assert(false &&
                                           "Got Error WrongFormat for MachO "
                                           "OpenError");
                                case ErrorKind::SizeTooSmall:
                                    fprintf(stderr,
                                            "Arch at index %d is too small to "
                                            "be a valid mach-o\n",
                                            ArchIndex);
                                    exit(1);
                                case ErrorKind::TooManyLoadCommands:
                                    fprintf(stderr,
                                            "Arch at index %d has too many "
                                            "load-commands for its size\n",
                                            ArchIndex);
                                    exit(1);
                            }
                        case Objects::Kind::FatMachO:
                            assert(false &&
                                   "Arch-Object is somehow a Fat-MachO");
                    }
                }

                return run(*ArchObject.ptr());
            }
        }

        assert(false && "Got unrecognized Object-Kind in runAndHandleFile");
    }
}
