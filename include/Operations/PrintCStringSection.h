//
//  include/Operations/PrintCStringSection.h
//  ktool
//
//  Created by Suhas Pai on 6/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "Base.h"

struct PrintCStringSectionOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintCStringSection;

    [[nodiscard]]
    constexpr static inline bool IsOfKind(const Operation &Op) noexcept {
        return (Op.getKind() == OpKind);
    }

    struct Options : public Operation::Options {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(const Operation::Options &Opt) noexcept {
            return (Opt.getKind() == OpKind);
        }

        std::string_view SegmentName;
        std::string_view SectionName;

        Options() noexcept
        : Operation::Options(OpKind), Sort(false), Verbose(false) {}

        bool Sort : 1;
        bool Verbose : 1;
    };
protected:
    Options Options;
public:
    PrintCStringSectionOperation() noexcept;
    PrintCStringSectionOperation(const struct Options &Options) noexcept;

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
