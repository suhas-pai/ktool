//
//  Operations/PrintObjcClassList.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "Objects/DscImageMemory.h"
#include "Objects/MachOMemory.h"

#include "Base.h"
#include "Kind.h"

struct PrintObjcClassListOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintObjcClassList;

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

        bool PrintCategories : 1 = false;
        bool PrintTree : 1 = false;
        bool Verbose : 1 = false;

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
                return true;
            case ObjectKind::FatMachO:
            case ObjectKind::DyldSharedCache:
            case ObjectKind::DscImage:
                return false;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};


