//
//  src/Operations/PrintFlags.cpp
//  ktool
//
//  Created by Suhas Pai on 5/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include <vector>

#include "ADT/MachO.h"
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintFlags.h"

PrintFlagsOperation::PrintFlagsOperation() noexcept : Operation(OpKind) {}
PrintFlagsOperation::PrintFlagsOperation(const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

int
PrintFlagsOperation::Run(const ConstMachOMemoryObject &Object,
                         const struct Options &Options) noexcept
{
    OperationCommon::PrintFlagInfoList(
        Options.OutFile,
        OperationCommon::GetFlagInfoList(Object.getConstHeader().getFlags()),
        Options.Verbose);

    return 0;
}

struct PrintFlagsOperation::Options
PrintFlagsOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                      int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (!Argument.IsOption()) {
            if (IndexOut != nullptr) {
                *IndexOut = Argv.indexOf(Argument);
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }
    }

    return Options;
}

void
PrintFlagsOperation::ParseOptions(const ArgvArray &Argv, int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int PrintFlagsOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
        case ObjectKind::DyldSharedCache:
            return InvalidObjectKind;
        case ObjectKind::DscImage:
            return Run(cast<ObjectKind::DscImage>(Object), Options);
    }

    assert(0 && "Unrecognized Object-Kind");
}
