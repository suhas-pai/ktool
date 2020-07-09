//
//  include/Operations/PrintSymbolPtrSection.h
//  stool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "Base.h"

struct PrintSymbolPtrSectionOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintSymbolPtrSection;

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

        std::string_view SegmentName;
        std::string_view SectionName;

        enum class SortKind {
            ByDylibOrdinal,
            ByKind,
            ByIndex,
            BySymbol
        };

        std::vector<SortKind> SortKindList;
        bool Verbose : 1;
    };
protected:
    Options Options;
public:
    PrintSymbolPtrSectionOperation() noexcept = default;
    PrintSymbolPtrSectionOperation(const struct Options &Options) noexcept;

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
