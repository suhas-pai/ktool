//
//  Operations/PrintHeader.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"

#include "Objects/DscMemory.h"
#include "Objects/FatMachOMemory.h"
#include "Objects/MachOMemory.h"

#include "Base.h"

using namespace std::literals;

struct PrintHeaderOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintHeader;

    [[nodiscard]]
    constexpr static auto IsOfKind(const Operation::Options &Opt) noexcept {
        return Opt.getKind() == OpKind;
    }

    struct Options : public Operation::Options {
        [[nodiscard]]
        constexpr static auto IsOfKind(const Operation::Options &Opt) noexcept {
            return Opt.getKind() == OpKind;
        }

        Options() noexcept : Operation::Options(OpKind) {}
        bool Verbose : 1 = false;
    };
protected:
    Options Options;
public:
    PrintHeaderOperation() noexcept;
    PrintHeaderOperation(const struct Options &Options) noexcept;

    static int
    Run(const MachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const FatMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const DscMemoryObject &Object, const struct Options &Options) noexcept;

    [[nodiscard]] static auto
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept
        -> struct Options;

    int ParseOptions(const ArgvArray &Argv) noexcept override;
    int Run(const MemoryObject &Object) const noexcept override;

    [[nodiscard]]
    constexpr static auto SupportsObjectKind(const ObjectKind Kind) noexcept {
        switch (Kind) {
            case ObjectKind::None:
                assert(0 && "SupportsObjectKind() got Object-Kind None");
            case ObjectKind::DyldSharedCache:
            case ObjectKind::MachO:
            case ObjectKind::FatMachO:
            case ObjectKind::DscImage:
                return true;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};
