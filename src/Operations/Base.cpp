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

void
Operation::Run(OperationKind Kind,
               const MemoryObject &Object,
               int Argc,
               const char *Argv[]) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            OperationKindInfo<OperationKind::PrintHeader>::Type::run(Object,
                                                                     Argc,
                                                                     Argv);
            break;
        case OperationKind::PrintLoadCommands:
            OperationKindInfo<OperationKind::PrintLoadCommands>::Type::run(
                Object, Argc, Argv);
            break;
        case OperationKind::PrintSharedLibraries:
            OperationKindInfo<OperationKind::PrintSharedLibraries>::Type::run(
                Object, Argc, Argv);
            break;
        case OperationKind::PrintId:
            OperationKindInfo<OperationKind::PrintId>::Type::run(Object,
                                                                 Argc,
                                                                 Argv);
            break;
        case OperationKind::PrintArchList:
            OperationKindInfo<OperationKind::PrintArchList>::Type::run(Object,
                                                                       Argc,
                                                                       Argv);
            break;
        case OperationKind::PrintFlags:
            OperationKindInfo<OperationKind::PrintFlags>::Type::run(Object,
                                                                    Argc,
                                                                    Argv);
            break;
        case OperationKind::PrintExportTrie:
            OperationKindInfo<OperationKind::PrintExportTrie>::Type::run(Object,
                                                                         Argc,
                                                                         Argv);
            break;
    }
}

void
Operation::Run(OperationKind Kind,
               const MemoryObject &Object,
               const struct Options &Options) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            OperationTypeFromKind<OperationKind::PrintHeader>::run(
                Object, cast<OperationKind::PrintHeader>(Options));
            break;
        case OperationKind::PrintLoadCommands:
            OperationTypeFromKind<OperationKind::PrintLoadCommands>::run(
                Object, cast<OperationKind::PrintLoadCommands>(Options));
            break;
        case OperationKind::PrintSharedLibraries:
            OperationTypeFromKind<OperationKind::PrintSharedLibraries>::run(
                Object, cast<OperationKind::PrintSharedLibraries>(Options));
            break;
        case OperationKind::PrintId:
            OperationTypeFromKind<OperationKind::PrintId>::run(
                Object, cast<OperationKind::PrintId>(Options));
            break;
        case OperationKind::PrintArchList:
            OperationTypeFromKind<OperationKind::PrintArchList>::run(
                Object, cast<OperationKind::PrintArchList>(Options));
            break;
        case OperationKind::PrintFlags:
            OperationTypeFromKind<OperationKind::PrintFlags>::run(
                Object, cast<OperationKind::PrintFlags>(Options));
            break;
        case OperationKind::PrintExportTrie:
            OperationTypeFromKind<OperationKind::PrintExportTrie>::run(
                Object, cast<OperationKind::PrintExportTrie>(Options));
            break;
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

void
PrintOperation::Run(OperationKind Kind,
                    const MemoryObject &Object,
                    int Argc,
                    const char *Argv[]) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            OperationKindInfo<OperationKind::PrintHeader>::Type::run(Object,
                                                                     Argc,
                                                                     Argv);
            break;
        case OperationKind::PrintLoadCommands:
            OperationKindInfo<OperationKind::PrintLoadCommands>::Type::run(
                Object, Argc, Argv);
            break;
        case OperationKind::PrintSharedLibraries:
            OperationKindInfo<OperationKind::PrintSharedLibraries>::Type::run(
                Object, Argc, Argv);
            break;
        case OperationKind::PrintId:
            OperationKindInfo<OperationKind::PrintId>::Type::run(Object,
                                                                 Argc,
                                                                 Argv);
            break;
        case OperationKind::PrintArchList:
            OperationKindInfo<OperationKind::PrintArchList>::Type::run(Object,
                                                                       Argc,
                                                                       Argv);
            break;
        case OperationKind::PrintFlags:
            OperationKindInfo<OperationKind::PrintFlags>::Type::run(Object,
                                                                    Argc,
                                                                    Argv);
            break;
        case OperationKind::PrintExportTrie:
            OperationKindInfo<OperationKind::PrintExportTrie>::Type::run(Object,
                                                                         Argc,
                                                                         Argv);
            break;
    }
}

void
PrintOperation::Run(OperationKind Kind,
                    const MemoryObject &Object,
                    const struct Options &Options) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            OperationTypeFromKind<OperationKind::PrintHeader>::run(
                Object, cast<OperationKind::PrintHeader>(Options));
            break;
        case OperationKind::PrintLoadCommands:
            OperationTypeFromKind<OperationKind::PrintLoadCommands>::run(
                Object, cast<OperationKind::PrintLoadCommands>(Options));
            break;
        case OperationKind::PrintSharedLibraries:
            OperationTypeFromKind<OperationKind::PrintSharedLibraries>::run(
                Object, cast<OperationKind::PrintSharedLibraries>(Options));
            break;
        case OperationKind::PrintId:
            OperationTypeFromKind<OperationKind::PrintId>::run(
                Object, cast<OperationKind::PrintId>(Options));
            break;
        case OperationKind::PrintArchList:
            OperationTypeFromKind<OperationKind::PrintArchList>::run(
                Object, cast<OperationKind::PrintArchList>(Options));
            break;
        case OperationKind::PrintFlags:
            OperationTypeFromKind<OperationKind::PrintFlags>::run(
                Object, cast<OperationKind::PrintFlags>(Options));
            break;
        case OperationKind::PrintExportTrie:
            OperationTypeFromKind<OperationKind::PrintExportTrie>::run(
                Object, cast<OperationKind::PrintExportTrie>(Options));
            break;
    }
}
