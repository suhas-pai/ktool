//
//  src/Operations/PrintFlags.cpp
//  stool
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

PrintFlagsOperation::PrintFlagsOperation() noexcept : PrintOperation(OpKind) {}
PrintFlagsOperation::PrintFlagsOperation(const struct Options &Options) noexcept
: PrintOperation(OpKind), Options(Options) {}

void PrintFlagsOperation::run(const ConstMemoryObject &Object) noexcept {
    run(Object, Options);
}

void
PrintFlagsOperation::run(const ConstMachOMemoryObject &Object,
                         const struct Options &Options) noexcept
{
    struct FlagInfo {
        const std::string_view &Name;
        const std::string_view &Description;

        uint32_t Mask;
    };

    using namespace MachO;
    constexpr static const int LongestFlagNameLength =
        Header::FlagInfo<
            Header::FlagsEnum::NlistOutOfSyncWithDyldInfo>::Name.length();

    auto FlagInfoList = std::vector<FlagInfo>();

    const auto MachOFlag = Header::FlagsEnum::NoUndefinedReferences;
    const auto ObjectFlags = Object.GetConstHeader().GetFlags();

    switch (MachOFlag) {
        case Header::FlagsEnum::NoUndefinedReferences: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::NoUndefinedReferences>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::IncrementalLink: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::IncrementalLink>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::DyldLink: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::DyldLink>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::BindAtLoad: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::BindAtLoad>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::PreBound: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::PreBound>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::SplitSegments: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::SplitSegments>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::LazyInitialization: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::LazyInitialization>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::TwoLevelNamespaceBindings: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::TwoLevelNamespaceBindings>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::ForceFlatNamespaces: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::ForceFlatNamespaces>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::NoMultipleDefinitions: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::NoMultipleDefinitions>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::NoFixPrebinding: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::NoFixPrebinding>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::Prebindable: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::Prebindable>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::AllModulesBound: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::AllModulesBound>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::SubsectionsViaSymbols: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::SubsectionsViaSymbols>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::Canonical: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::Canonical>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::WeakDefinitions: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::WeakDefinitions>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::BindsToWeak: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::BindsToWeak>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::AllowStackExecution: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::AllowStackExecution>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::RootSafe: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::RootSafe>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::SetuidSafe: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::SetuidSafe>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::NoReexportedDylibs: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::NoReexportedDylibs>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::PositionIndependentExecutable: {
            using FlagInfo =
                Header::FlagInfo<
                    Header::FlagsEnum::PositionIndependentExecutable>;

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::DeadStrippableDylib: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::DeadStrippableDylib>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::HasTlvDescriptors: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::HasTlvDescriptors>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::NoHeapExecution: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::NoHeapExecution>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::AppExtensionSafe: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::AppExtensionSafe>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::NlistOutOfSyncWithDyldInfo: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::NlistOutOfSyncWithDyldInfo>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::SimulatorSupport: {
            using FlagInfo =
                Header::FlagInfo<Header::FlagsEnum::SimulatorSupport>;

            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
        case Header::FlagsEnum::DylibInCache: {
            using FlagInfo = Header::FlagInfo<Header::FlagsEnum::DylibInCache>;
            static_assert(FlagInfo::Name.length() <= LongestFlagNameLength,
                          "Flag has longer length than current record-holder");

            if (ObjectFlags.Has(FlagInfo::Kind)) {
                FlagInfoList.push_back({
                    .Name = FlagInfo::Name,
                    .Description = FlagInfo::Description,
                    .Mask = FlagInfo::Mask
                });
            }
        }
    }

    auto FlagNumber = static_cast<uint32_t>(1);
    for (const auto &Info : FlagInfoList) {
        fprintf(Options.OutFile,
                "Flag %02" PRIu32 ": %" PRINTF_RIGHTPAD_FMT "s",
                FlagNumber,
                LongestFlagNameLength,
                Info.Name.data());

        FlagNumber++;

        if (Options.Verbose) {
            fprintf(Options.OutFile,
                    " (Description: %s)\n",
                    Info.Description.data());
        } else {
            fputc('\n', Options.OutFile);
        }
    }
}

struct PrintFlagsOperation::Options
PrintFlagsOperation::ParseOptionsImpl(int Argc,
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

struct PrintFlagsOperation::Options *
PrintFlagsOperation::ParseOptions(int Argc, const char *Argv[], int *IndexOut)
noexcept {
    return new struct Options(ParseOptionsImpl(Argc, Argv, IndexOut));
}

void
PrintFlagsOperation::run(const ConstMemoryObject &Object,
                         const struct Options &Options) noexcept
{
    switch (Object.GetKind()) {
        case ObjectKind::None:
            assert(0 && "Object Type is None");
        case ObjectKind::MachO:
            run(cast<ObjectKind::MachO>(Object), Options);
            break;
        case ObjectKind::FatMachO:
            PrintObjectKindNotSupportedError(OpKind, Object);
            exit(1);
    }
}

void
PrintFlagsOperation::run(const ConstMemoryObject &Object,
                         int Argc,
                         const char *Argv[]) noexcept
{
    assert(Object.GetKind() != ObjectKind::None);
    run(Object, ParseOptionsImpl(Argc, Argv, nullptr));
}

