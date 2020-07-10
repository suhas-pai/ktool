//
//  include/Operations/PrintSharedLibraries.h
//  ktool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"
#include "Objects/MachOMemory.h"
#include "Base.h"

using namespace std::literals;

struct PrintSharedLibrariesOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintSharedLibraries;

    [[nodiscard]]
    constexpr static inline bool IsOfKind(const Operation &Opt) noexcept {
        return (Opt.getKind() == OpKind);
    }

    struct Options : public Operation::Options {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(const Operation::Options &Opt) noexcept {
            return (Opt.getKind() == OpKind);
        }

        Options() noexcept : Operation::Options(OpKind), Verbose(false) {}
        bool Verbose : 1;
    };
protected:
    Options Options;
public:
    PrintSharedLibrariesOperation() noexcept;
    PrintSharedLibrariesOperation(const struct Options &Options) noexcept;

    static int
    Run(const ConstMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    [[nodiscard]] static struct Options
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept;

    void ParseOptions(const ArgvArray &Argv, int *IndexOut) noexcept override;
    int Run(const MemoryObject &Object) const noexcept override;

    [[nodiscard]]
    constexpr static bool SupportsObjectKind(ObjectKind Kind) noexcept {
        switch (Kind) {
            case ObjectKind::None:
                assert(0 && "SupportsObjectKind() got Object-Kind None");
            case ObjectKind::MachO:
                return true;
            case ObjectKind::FatMachO:
                return false;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};
