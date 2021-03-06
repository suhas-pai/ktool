//
//  include/Operations/PrintBindSymbolList.h
//  ktool
//
//  Created by Suhas Pai on 5/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>
#include "Base.h"
#include "Kind.h"
#include "Objects/MachOMemory.h"

struct PrintBindSymbolListOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintBindSymbolList;

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

        union {
            uint8_t PrintAll = 7;

            struct {
                bool PrintNormal : 1;
                bool PrintLazy : 1;
                bool PrintWeak : 1;
            };
        };

        enum class SortKind {
            ByName,
            ByDylibOrdinal,
            ByType
        };

        bool Verbose : 1;
        std::vector<SortKind> SortKindList;
    };
protected:
    Options Options;
public:
    PrintBindSymbolListOperation() noexcept;
    PrintBindSymbolListOperation(const struct Options &Options) noexcept;

    static int
    Run(const ConstDscImageMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const ConstMachOMemoryObject &Object,
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


