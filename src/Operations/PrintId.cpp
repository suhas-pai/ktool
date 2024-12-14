//
//  Operations/PrintId.cpp
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "Operations/Common.h"
#include "Operations/Operation.h"
#include "Operations/PrintId.h"

#include "Utils/MachOTypePrinter.h"

PrintIdOperation::PrintIdOperation() noexcept : Operation(OpKind) {}
PrintIdOperation::PrintIdOperation(const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static auto
PrintFromLoadCommands(const MachOMemoryObject &Object,
                      const struct PrintIdOperation::Options &Options) noexcept
{
    using LCKind = MachO::LoadCommand::Kind;

    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    struct MachO::DylibCommand::Info Info = {};
    auto Id = std::string_view();

    for (const auto &LC : LoadCmdStorage) {
        const auto *const Dylib = dyn_cast<LCKind::IdDylib>(LC, IsBigEndian);
        if (Dylib == nullptr) {
            continue;
        }

        const auto NameResult = Dylib->GetName(IsBigEndian);
        if (NameResult.hasError()) {
            fputs("Provided file has an invalid Id-Name\n", Options.ErrFile);
            return 1;
        }

        const auto &Name = NameResult.getString();
        if (!Id.empty() && Id != Name) {
            fputs("Provided file has multiple (differing) Id-Names\n",
                  Options.ErrFile);
            return 1;
        }

        Id = Name;
        Info = Dylib->Info;
    }

    if (Id.empty()) {
        fputs("No Id found for the provided dylib\n", Options.ErrFile);
        return 1;
    }

    fprintf(Options.OutFile, "\"%s\"\n", Id.data());
    if (Options.Verbose) {
        MachOTypePrinter<struct MachO::DylibCommand::Info>::Print(
            Options.OutFile, Info, IsBigEndian, true, "\t");
    }

    return 0;
}

int
PrintIdOperation::Run(const DscImageMemoryObject &Object,
                      const struct Options &Options) noexcept
{
    if (Options.Verbose) {
        return PrintFromLoadCommands(Object, Options);
    }

    fprintf(Options.OutFile, "\"%s\"\n", Object.getPath());
    return 0;
}

int
PrintIdOperation::Run(const MachOMemoryObject &Object,
                      const struct Options &Options) noexcept
{
    if (Object.getFileKind() != MachO::Header::FileKind::Dylib) {
        fputs("Provided file is not a Dynamic-Library file\n", Options.OutFile);
        return 0;
    }

    return PrintFromLoadCommands(Object, Options);
}

auto
PrintIdOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                   int *const IndexOut) noexcept
    -> struct PrintIdOperation::Options
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

int PrintIdOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int PrintIdOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::DscImage:
            return Run(cast<ObjectKind::DscImage>(Object), Options);
        case ObjectKind::FatMachO:
        case ObjectKind::DyldSharedCache:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}

