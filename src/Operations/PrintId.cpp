//
//  src/Operations/PrintId.cpp
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
#include "PrintId.h"

PrintIdOperation::PrintIdOperation() noexcept : Operation(OpKind) {}
PrintIdOperation::PrintIdOperation(const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

int
PrintIdOperation::Run(const ConstMachOMemoryObject &Object,
                      const struct Options &Options) noexcept
{
    if (Object.getFileKind() != MachO::Header::FileKind::Dylib) {
        fputs("Provided file is not a Dynamic-Library file\n", Options.OutFile);
        return 0;
    }

    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    struct MachO::DylibCommand::Info Info = {};
    auto Id = std::string_view();

    for (const auto &LoadCmd : LoadCmdStorage) {
        const auto *DylibCmd =
            LoadCmd.dyn_cast<MachO::LoadCommand::Kind::IdDylib>(IsBigEndian);

        if (DylibCmd == nullptr) {
            continue;
        }

        const auto NameResult = DylibCmd->GetName(IsBigEndian);
        if (NameResult.hasError()) {
            fputs("Provided file has an invalid Id-Name\n", Options.ErrFile);
            return 1;
        }

        const auto &Name =
            OperationCommon::GetLoadCommandStringValue(NameResult);

        if (!Id.empty() && Id != Name) {
            fputs("Provided file has multiple (differing) Id-Names\n",
                  Options.ErrFile);
            return 1;
        }

        Id = Name;
        Info = DylibCmd->Info;
    }

    if (Id.empty()) {
        fputs("No Identification found for the provided dylib\n",
              Options.OutFile);
        return 1;
    }

    fprintf(Options.OutFile, "\"%s\"\n", Id.data());
    if (Options.Verbose) {
        MachOTypePrinter<struct MachO::DylibCommand::Info>::Print(
            Options.OutFile, Info, IsBigEndian, true, "\t", "");
    }

    return 0;
}

struct PrintIdOperation::Options
PrintIdOperation::ParseOptionsImpl(const ArgvArray &Argv,
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
PrintIdOperation::ParseOptions(const ArgvArray &Argv, int *IndexOut) noexcept {
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int PrintIdOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}

