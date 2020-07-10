//
//  include/Operations/PrintBindActionList.h
//  ktool
//
//  Created by Suhas Pai on 5/28/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>
#include "Base.h"
#include "Kind.h"
#include "Objects/MachOMemory.h"

struct PrintBindActionListOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintBindActionList;

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

        union {
            uint8_t PrintAll = 7;

            struct {
                bool PrintNormal : 1;
                bool PrintLazy : 1;
                bool PrintWeak : 1;
            };
        };

        enum class SortKind {
            None,
            ByName,
            ByDylibOrdinal,
            ByKind
        };

        std::vector<SortKind> SortKindList;
        bool Verbose : 1;
    };
protected:
    Options Options;
public:
    PrintBindActionListOperation() noexcept;
    PrintBindActionListOperation(const struct Options &Options) noexcept;

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


