//
//  include/Operations/PrintHeader.h
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "Base.h"

struct PrintHeaderOperation : public PrintOperation {
public:
    constexpr static const auto OpKind = OperationKind::PrintHeader;
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
    explicit PrintHeaderOperation() noexcept;
    explicit PrintHeaderOperation(const struct Options &Options) noexcept;

    static struct Options *
    ParseOptions(int Argc, const char *Argv[], int *IndexOut) noexcept;

    void run(const ConstMemoryObject &Object) noexcept;

    static void run(const ConstMemoryObject &Object,
                    const struct Options &Options) noexcept;

    static void run(const ConstMachOMemoryObject &Object,
                    const struct Options &Options) noexcept;

    static void
    run(const ConstFatMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static void
    run(const ConstMemoryObject &Object, int Argc, const char *Argv[]) noexcept;

    constexpr static bool SupportsObjectKind(ObjectKind Kind) {
        switch (Kind) {
            case ObjectKind::None:
                assert(0 && "SupportsObjectKind() got Object-Kind None");
            case ObjectKind::MachO:
            case ObjectKind::FatMachO:
                return true;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};
