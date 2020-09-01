//
//  include/Operations/PrintHeader.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"
#include "Base.h"

using namespace std::literals;

struct PrintHeaderOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintHeader;

    [[nodiscard]]
    constexpr static inline bool IsOfKind(const Operation &Op) noexcept {
        return (Op.getKind() == OpKind);
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
    PrintHeaderOperation() noexcept;
    PrintHeaderOperation(const struct Options &Options) noexcept;

    static int
    Run(const ConstMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const ConstFatMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const ConstDscMemoryObject &Object,
        const struct Options &Options) noexcept;

    [[nodiscard]] static struct Options
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept;

    int ParseOptions(const ArgvArray &Argv) noexcept override;
    int Run(const MemoryObject &Object) const noexcept override;

    [[nodiscard]]
    constexpr static bool SupportsObjectKind(ObjectKind Kind) noexcept {
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
