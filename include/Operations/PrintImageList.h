//
//  include/Operations/PrintImageList.h
//  ktool
//
//  Created by Suhas Pai on 7/11/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "Base.h"

struct PrintImageListOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintImageList;

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
        enum class SortKind {
            ByAddress,
            ByModTime,
            ByName,
        };

        std::vector<SortKind> SortKindList;
        bool Verbose : 1;
    };
protected:
    Options Options;
public:
    PrintImageListOperation() noexcept;
    PrintImageListOperation(const struct Options &Options) noexcept;

    static int
    Run(const ConstDscMemoryObject &Object,
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
            case ObjectKind::FatMachO:
            case ObjectKind::DscImage:
                return false;
            case ObjectKind::DyldSharedCache:
                return true;
        }

        assert(0 && "Reached end of SupportsObjectKind()");
    }
};

