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

PrintIdOperation::PrintIdOperation() noexcept : PrintOperation(OpKind) {}
PrintIdOperation::PrintIdOperation(const struct Options &Options) noexcept
: PrintOperation(OpKind), Options(Options) {}

int PrintIdOperation::run(const ConstMemoryObject &Object) noexcept {
    return run(Object, Options);
}

int
PrintIdOperation::run(const ConstMachOMemoryObject &Object,
                      const struct Options &Options) noexcept
{
    if (Object.GetFileType() != MachO::Header::FileType::Dylib) {
        fputs("Provided file is not a Dynamic-Library file\n", Options.OutFile);
        return 0;
    }

    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCommandStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    struct MachO::DylibCommand::Info Info = {};
    auto Id = std::string_view();

    for (const auto &LoadCmd : LoadCommandStorage) {
        if (LoadCmd.GetKind(IsBigEndian) != MachO::LoadCommand::Kind::IdDylib) {
            continue;
        }

        const auto &DylibCmd =
            cast<MachO::LoadCommand::Kind::IdDylib>(LoadCmd, IsBigEndian);

        const auto &Name = DylibCmd.GetName(IsBigEndian);

        Info = DylibCmd.Info;
        Id = OperationCommon::GetLoadCommandStringValue(Name);

        break;
    }

    if (Id.empty()) {
        fputs("No Identification found for the provided dylib\n",
              Options.OutFile);
        return 1;
    }

    fprintf(Options.OutFile, "%s\n", Id.data());
    if (Options.Verbose) {
        MachOTypePrinter<struct MachO::DylibCommand::Info>::Print(
            Options.OutFile, Info, IsBigEndian, true, "\t", "");
    }

    return 0;
}

struct PrintIdOperation::Options
PrintIdOperation::ParseOptionsImpl(int Argc,
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

struct PrintIdOperation::Options *
PrintIdOperation::ParseOptions(int Argc, const char *Argv[], int *IndexOut)
noexcept {
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

int
PrintIdOperation::run(const ConstMemoryObject &Object,
                      const struct Options &Options) noexcept
{
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object Type is None");
        case ObjectKind::MachO:
            return run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
            PrintObjectKindNotSupportedError(OpKind, Object);
            return 1;
    }

    assert(0 && "Reached end with unrecognizable Object-Kind");
}

int
PrintIdOperation::run(const ConstMemoryObject &Object,
                      int Argc,
                      const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    return run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}
