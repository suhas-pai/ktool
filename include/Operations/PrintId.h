//
//  include/Operations/PrintId.h
//  stool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "Base.h"
#include "Kind.h"
#include "Objects/MachOMemory.h"

struct PrintIdOperation : public PrintOperation {
public:
    constexpr static const auto OpKind = OperationKind::PrintId;
    constexpr static inline bool IsOfKind(const Operation::Options &Opt) noexcept {
        return (Opt.GetKind() == OpKind);
    }

    struct Options : public PrintOperation::Options {
        explicit Options() noexcept : PrintOperation::Options(OpKind) {}
        static inline bool IsOfKind(const Operation::Options &Opt) noexcept {
            return (Opt.GetKind() == OpKind);
        }

        bool Verbose : 1;
    };
protected:
    Options Options;

    static struct Options
    ParseOptionsImpl(int Argc, const char *Argv[], int *IndexOut) noexcept;
public:
    explicit PrintIdOperation() noexcept;
    explicit PrintIdOperation(const struct Options &Options) noexcept;

    static struct Options *
    ParseOptions(int Argc, const char *Argv[], int *IndexOut) noexcept;

    int run(const ConstMemoryObject &Object) noexcept;

    static int run(const ConstMachOMemoryObject &Object,
                   const struct Options &Options) noexcept;

    static int run(const ConstMemoryObject &Object,
                   int Argc,
                   const char *Argv[]) noexcept;

    static int run(const ConstMemoryObject &Object,
                   const struct Options &Options) noexcept;

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

