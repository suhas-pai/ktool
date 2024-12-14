//
//  Operations/PrintArchList.cpp
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "Operations/Operation.h"
#include "Operations/PrintArchList.h"

#include "Utils/MachOTypePrinter.h"

PrintArchListOperation::PrintArchListOperation() noexcept : Operation(OpKind) {}
PrintArchListOperation::PrintArchListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

int
PrintArchListOperation::Run(const FatMachOMemoryObject &Object,
                            const struct Options &Options) noexcept
{
    const auto ArchCount = Object.getArchCount();
    fprintf(stdout, "Provided file has %" PRIu32 " archs:\n", ArchCount);

    // General safe-guard against over-printing. More likely than too-many valid
    // architectures is a mistake in this program.

    if (ArchCount > 25) {
        fputs("Provided file has too many archs to list\n", Options.ErrFile);
        return 1;
    }

    if (Options.Verbose) {
        MachOTypePrinter<MachO::FatHeader>::PrintArchListVerbose(
            Options.OutFile, Object.getConstHeader(), "\t");
    } else {
        MachOTypePrinter<MachO::FatHeader>::PrintArchList(Options.OutFile,
            Object.getConstHeader(), "\t");
    }

    return 0;
}

auto
PrintArchListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                         int *const IndexOut) noexcept
    -> struct PrintArchListOperation::Options
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (!Argument.isOption()) {
            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }

        Index++;
    }

    if (IndexOut != nullptr) {
        *IndexOut = Index;
    }

    return Options;
}

int PrintArchListOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int PrintArchListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
        case ObjectKind::DyldSharedCache:
        case ObjectKind::DscImage:
            return InvalidObjectKind;
        case ObjectKind::FatMachO:
            return Run(cast<ObjectKind::FatMachO>(Object), Options);
    }

    assert(0 && "Unrecognized Object-Kind");
}
