//
//  include/Operations/PrintObjcClassList.h
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "Base.h"
#include "Kind.h"
#include "Objects/MachOMemory.h"

struct PrintObjcClassListOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintObjcClassList;

    [[nodiscard]]
    constexpr static inline bool IsOfKind(const Operation &Opt) noexcept {
        return (Opt.getKind() == OpKind);
    }

    struct Options : public Operation::Options {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(const Operation::Options &Opt) noexcept {
            return (Opt.getKind() == OpKind);
        }

        Options() noexcept
        : Operation::Options(OpKind), PrintTree(false), Verbose(false) {}

        bool PrintCategories : 1;
        bool PrintTree : 1;
        bool Verbose : 1;

        enum class SortKind {
            None,
            ByName,
            ByDylibOrdinal,
            ByKind
        };

        std::vector<SortKind> SortKindList;
    };
protected:
    Options Options;
public:
    PrintObjcClassListOperation() noexcept;
    PrintObjcClassListOperation(const struct Options &Options) noexcept;

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


