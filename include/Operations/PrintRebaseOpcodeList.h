//
//  Operations/PrintRebaseOpcodeList.h
//  ktool
//
//  Created by Suhas Pai on 5/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "Objects/DscImageMemory.h"
#include "Objects/MachOMemory.h"

#include "Base.h"
#include "Kind.h"

struct PrintRebaseOpcodeListOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintRebaseOpcodeList;

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
    PrintRebaseOpcodeListOperation() noexcept;
    PrintRebaseOpcodeListOperation(const struct Options &Options) noexcept;

    static int
    Run(const DscImageMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const MachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    [[nodiscard]] static struct Options
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept;

    int ParseOptions(const ArgvArray &Argv) noexcept override;
    int Run(const MemoryObject &Object) const noexcept override;

    [[nodiscard]]
    constexpr static auto SupportsObjectKind(const ObjectKind Kind) noexcept {
        switch (Kind) {
            case ObjectKind::None:
                assert(0 && "SupportsObjectKind() got Object-Kind None");
            case ObjectKind::MachO:
            case ObjectKind::DscImage:
                return true;
            case ObjectKind::FatMachO:
            case ObjectKind::DyldSharedCache:
                return false;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};
