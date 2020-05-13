//
//  src/Operations/PrintArchList.cpp
//  stool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "ADT/MachO.h"

#include "Utils/MachOPrinter.h"
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintArchList.h"

PrintArchListOperation::PrintArchListOperation() noexcept
: PrintOperation(OpKind) {}

PrintArchListOperation::PrintArchListOperation(const struct Options &Options)
noexcept : PrintOperation(OpKind), Options(Options) {}

void PrintArchListOperation::run(const ConstMemoryObject &Object) noexcept {
    run(Object, Options);
}

void
PrintArchListOperation::run(const ConstFatMachOMemoryObject &Object,
                            const struct Options &Options) noexcept
{
    fprintf(stdout,
            "Provided file has %" PRIu32 " archs:\n",
            Object.GetArchCount());

    if (Options.Verbose) {
        MachOTypePrinter<MachO::FatHeader>::PrintArchListVerbose(
            Options.OutFile, Object.GetConstHeader(), "\t");
    } else {
        MachOTypePrinter<MachO::FatHeader>::PrintArchList(Options.OutFile,
            Object.GetConstHeader(), "\t");
    }
}

struct PrintArchListOperation::Options
PrintArchListOperation::ParseOptionsImpl(int Argc,
                                         const char *Argv[],
                                         int *IndexOut) noexcept
{
    struct Options Options;
    for (auto I = int(); I != Argc; I++) {
        const auto Argument = Argv[I];
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (Argument[0] != '-') {
            if (IndexOut != nullptr) {
                *IndexOut = I;
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument);
            exit(1);
        }
    }

    return Options;
}

struct PrintArchListOperation::Options *
PrintArchListOperation::ParseOptions(int Argc,
                                     const char *Argv[],
                                     int *IndexOut) noexcept
{
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

void
PrintArchListOperation::run(const ConstMemoryObject &Object,
                            const struct Options &Options) noexcept
{
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object Type is None");
        case ObjectKind::MachO:
            PrintObjectKindNotSupportedError(OpKind, Object);
            exit(1);
        case ObjectKind::FatMachO:
            run(cast<ObjectKind::FatMachO>(Object), Options);
            break;
    }
}

void
PrintArchListOperation::run(const ConstMemoryObject &Object,
                            int Argc,
                            const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
