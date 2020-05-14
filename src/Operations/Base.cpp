//
//  include/Operations/Base.cpp
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstdio>

#include "Utils/MachOPrinter.h"
#include "Operation.h"

#include "PrintHeader.h"
#include "PrintLoadCommands.h"
#include "PrintSharedLibraries.h"
#include "PrintId.h"
#include "PrintArchList.h"
#include "PrintFlags.h"

Operation::Operation(OperationKind Kind) noexcept : Kind(Kind) {}
PrintOperation::PrintOperation(OperationKind Kind) noexcept :
Operation(Kind) {}

int
Operation::Run(OperationKind Kind,
               const MemoryObject &Object,
               int Argc,
               const char *Argv[]) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
        case OperationKind::PrintLoadCommands:
        case OperationKind::PrintSharedLibraries:
        case OperationKind::PrintId:
        case OperationKind::PrintArchList:
        case OperationKind::PrintFlags:
        case OperationKind::PrintExportTrie:
            return PrintOperation::Run(Kind, Object, Argc, Argv);
    }
}

int
Operation::Run(OperationKind Kind,
               const MemoryObject &Object,
               const struct Options &Options) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
        case OperationKind::PrintLoadCommands:
        case OperationKind::PrintSharedLibraries:
        case OperationKind::PrintId:
        case OperationKind::PrintArchList:
        case OperationKind::PrintFlags:
        case OperationKind::PrintExportTrie:
            return PrintOperation::Run(Kind,
                                       Object,
                                       cast<const PrintOperation::Options &>(
                                            Options));
    }
}

static bool
OperationKindSupportsObjectKind(OperationKind OpKind, ObjectKind ObjKind) {
    switch (OpKind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            return OperationTypeFromKind<OperationKind::PrintHeader>::
                SupportsObjectKind(ObjKind);
        case OperationKind::PrintLoadCommands:
            return OperationTypeFromKind<OperationKind::PrintLoadCommands>::
                SupportsObjectKind(ObjKind);
        case OperationKind::PrintSharedLibraries:
            return OperationTypeFromKind<OperationKind::PrintSharedLibraries>::
                SupportsObjectKind(ObjKind);
        case OperationKind::PrintId:
            return OperationTypeFromKind<OperationKind::PrintId>::
                SupportsObjectKind(ObjKind);
        case OperationKind::PrintArchList:
            return OperationTypeFromKind<OperationKind::PrintArchList>::
                SupportsObjectKind(ObjKind);
        case OperationKind::PrintFlags:
            return OperationTypeFromKind<OperationKind::PrintFlags>::
                SupportsObjectKind(ObjKind);
        case OperationKind::PrintExportTrie:
            return OperationTypeFromKind<OperationKind::PrintExportTrie>::
                SupportsObjectKind(ObjKind);
    }

    assert(0 && "Reached end of OperationKindSupportsObjectKind()");
}

static void PrintSelectArchMessage(const ConstFatMachOMemoryObject &Object) {
    const auto ArchCount = Object.GetArchCount();
    fprintf(stdout, "Please select one of %" PRIu32 " archs", ArchCount);

    if (ArchCount > 3) {
        fprintf(stdout, ". Use option --list-archs to see a list of archs\n");
        return;
    }

    fputs(":\n", stdout);
    MachOTypePrinter<MachO::FatHeader>::PrintArchList(stdout,
                                                      Object.GetConstHeader(),
                                                      "",
                                                      "");
}

void
PrintOperation::PrintObjectKindNotSupportedError(
    OperationKind OpKind,
    const ConstMemoryObject &Object) noexcept
{
    assert(OpKind != OperationKind::None && "Operation-Kind is None");
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            if (OperationKindSupportsObjectKind(OpKind, ObjectKind::FatMachO)) {
                fprintf(stderr,
                        "Operation %s only supports Fat Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
        case ObjectKind::FatMachO:
            if (OperationKindSupportsObjectKind(OpKind, ObjectKind::MachO)) {
                PrintSelectArchMessage(cast<ObjectKind::FatMachO>(Object));
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
    }

    assert(0 && "Reached end of PrintObjectKindNotSupportedError()");
}

int
PrintOperation::Run(OperationKind Kind,
                    const MemoryObject &Object,
                    int Argc,
                    const char *Argv[]) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            return
                OperationKindInfo<OperationKind::PrintHeader>::Type::run(Object,
                                                                         Argc,
                                                                         Argv);
        case OperationKind::PrintLoadCommands:
            return
                OperationKindInfo<OperationKind::PrintLoadCommands>::Type::run(
                    Object, Argc, Argv);
        case OperationKind::PrintSharedLibraries:
            return
                OperationKindInfo<
                    OperationKind::PrintSharedLibraries>::Type::run(Object,
                                                                    Argc,
                                                                    Argv);
        case OperationKind::PrintId:
            return
                OperationKindInfo<OperationKind::PrintId>::Type::run(Object,
                                                                     Argc,
                                                                     Argv);
        case OperationKind::PrintArchList:
            return
                OperationKindInfo<OperationKind::PrintArchList>::Type::run(
                    Object, Argc, Argv);
        case OperationKind::PrintFlags:
            return
                OperationKindInfo<OperationKind::PrintFlags>::Type::run(Object,
                                                                        Argc,
                                                                        Argv);
        case OperationKind::PrintExportTrie:
            return
                OperationKindInfo<OperationKind::PrintExportTrie>::Type::run(
                    Object, Argc, Argv);
    }
}

int
PrintOperation::Run(OperationKind Kind,
                    const MemoryObject &Object,
                    const struct Options &Options) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            return
                OperationTypeFromKind<OperationKind::PrintHeader>::run(
                    Object, cast<OperationKind::PrintHeader>(Options));
        case OperationKind::PrintLoadCommands:
            return
                OperationTypeFromKind<OperationKind::PrintLoadCommands>::run(
                    Object, cast<OperationKind::PrintLoadCommands>(Options));
        case OperationKind::PrintSharedLibraries:
            return
                OperationTypeFromKind<OperationKind::PrintSharedLibraries>::run(
                    Object, cast<OperationKind::PrintSharedLibraries>(Options));
        case OperationKind::PrintId:
            return
                OperationTypeFromKind<OperationKind::PrintId>::run(Object,
                    cast<OperationKind::PrintId>(Options));
        case OperationKind::PrintArchList:
            return
                OperationTypeFromKind<OperationKind::PrintArchList>::run(Object,
                    cast<OperationKind::PrintArchList>(Options));
        case OperationKind::PrintFlags:
            return
                OperationTypeFromKind<OperationKind::PrintFlags>::run(Object,
                    cast<OperationKind::PrintFlags>(Options));
        case OperationKind::PrintExportTrie:
            return
                OperationTypeFromKind<OperationKind::PrintExportTrie>::run(
                    Object, cast<OperationKind::PrintExportTrie>(Options));
    }

    return 0;
}
