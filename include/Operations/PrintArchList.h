//
//  Operations/PrintArchList.h
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"
#include "Objects/FatMachOMemory.h"

#include "Base.h"
#include "Kind.h"

using namespace std::literals;

struct PrintArchListOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintArchList;

    [[nodiscard]]
    constexpr static auto IsOfKind(const Operation &Opt) noexcept {
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
    PrintArchListOperation() noexcept;
    PrintArchListOperation(const struct Options &Options) noexcept;

    static int
    Run(const FatMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

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
            case ObjectKind::MachO:
            case ObjectKind::DyldSharedCache:
            case ObjectKind::DscImage:
              return false;
            case ObjectKind::FatMachO:
                return true;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};

