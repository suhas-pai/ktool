//
//  include/Operations/PrintExportTrie.h
//  ktool
//
//  Created by Suhas Pai on 5/9/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <variant>
#include "Base.h"
#include "Kind.h"
#include "Objects/MachOMemory.h"

struct PrintExportTrieOperation : public Operation {
public:
    constexpr static const auto OpKind = OperationKind::PrintExportTrie;

    [[nodiscard]]
    constexpr static inline bool IsOfKind(const Operation &Op) noexcept {
        return (Op.getKind() == OpKind);
    }

    struct Options : public Operation::Options {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(const Operation::Options &Opt) noexcept {
            return (Opt.getKind() == OpKind);
        }

        struct SegmentSectionPair {
            std::string_view SegmentName;
            std::string_view SectionName;

            [[nodiscard]] inline
            bool operator==(const SegmentSectionPair &Rhs) const noexcept {
                const auto Result =
                    (SegmentName == Rhs.SegmentName) &&
                    (SectionName == Rhs.SectionName);

                return Result;
            }
        };

        Options() noexcept
        : Operation::Options(OpKind), PrintTree(false), OnlyCount(false),
          Sort(false), Verbose(false) {}

        bool PrintTree : 1;
        bool OnlyCount : 1;
        bool Sort : 1;
        bool Verbose : 1;

        std::vector<SegmentSectionPair> SectionRequirements;
        std::vector<MachO::ExportTrieExportKind> KindRequirements;
    };
protected:
    Options Options;
public:
    PrintExportTrieOperation() noexcept;
    PrintExportTrieOperation(const struct Options &Options) noexcept;

    static int
    Run(const ConstMachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const ConstDscImageMemoryObject &Object,
        const struct Options &Options) noexcept;

    [[nodiscard]] static struct Options
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept;

    int Run(const MemoryObject &Object) const noexcept override;
    int ParseOptions(const ArgvArray &Argv) noexcept override;

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

