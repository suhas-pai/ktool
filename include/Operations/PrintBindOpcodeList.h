//
//  include/Operations/PrintBindOpcodeList.h
//  ktool
//
//  Created by Suhas Pai on 5/22/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "Base.h"
#include "Kind.h"
#include "Objects/MachOMemory.h"

struct PrintBindOpcodeListOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintBindOpcodeList;

    [[nodiscard]]
    constexpr static bool IsOfKind(const Operation::Options &Opt) noexcept {
        return (Opt.getKind() == OpKind);
    }

    struct Options : public Operation::Options {
        [[nodiscard]]
        constexpr static bool IsOfKind(const Operation::Options &Opt) noexcept {
            return (Opt.getKind() == OpKind);
        }

        Options() noexcept : Operation::Options(OpKind) {}

        union {
            uint8_t PrintAll;

            struct {
                bool PrintNormal : 1 = true;
                bool PrintLazy : 1 = true;
                bool PrintWeak : 1 = true;
            };
        };

        bool Verbose : 1 = false;
    };
protected:
    Options Options;
public:
    PrintBindOpcodeListOperation() noexcept;
    PrintBindOpcodeListOperation(const struct Options &Options) noexcept;

    static int
    Run(const ConstMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const ConstDscImageMemoryObject &Object,
        const struct Options &Options) noexcept;

    [[nodiscard]] static struct Options
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept;

    int ParseOptions(const ArgvArray &Argv) noexcept override;
    int Run(const MemoryObject &Object) const noexcept override;

    [[nodiscard]]
    constexpr static bool SupportsObjectKind(const ObjectKind Kind) noexcept {
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
