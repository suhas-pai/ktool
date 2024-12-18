//
//  Operations/PrintExportTrie.h
//  ktool
//
//  Created by Suhas Pai on 5/9/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include <set>

#include "Objects/DscImageMemory.h"
#include "Objects/MachOMemory.h"

#include "Base.h"
#include "Kind.h"

struct PrintExportTrieOperation : public Operation {
public:
    constexpr static auto OpKind = OperationKind::PrintExportTrie;

    [[nodiscard]]
    constexpr static auto IsOfKind(const Operation::Options &Opt) noexcept {
        return Opt.getKind() == OpKind;
    }

    struct Options : public Operation::Options {
        [[nodiscard]]
        constexpr static auto IsOfKind(const Operation::Options &Opt) noexcept {
            return Opt.getKind() == OpKind;
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

        Options() noexcept : Operation::Options(OpKind) {}

        bool PrintTree : 1 = false;
        bool OnlyCount : 1 = false;
        bool Sort : 1 = false;
        bool Verbose : 1 = false;

        std::vector<SegmentSectionPair> SectionRequirements;
        std::set<MachO::ExportTrieExportKind> KindRequirements;
    };
protected:
    Options Options;
public:
    PrintExportTrieOperation() noexcept;
    PrintExportTrieOperation(const struct Options &Options) noexcept;

    static int
    Run(const MachOMemoryObject &Object,
        const struct Options &Options) noexcept;

    static int
    Run(const DscImageMemoryObject &Object,
        const struct Options &Options) noexcept;

    [[nodiscard]] static struct Options
    ParseOptionsImpl(const ArgvArray &Argv, int *IndexOut) noexcept;

    int Run(const MemoryObject &Object) const noexcept override;
    int ParseOptions(const ArgvArray &Argv) noexcept override;

    [[nodiscard]]
    constexpr static auto SupportsObjectKind(const ObjectKind Kind) noexcept {
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

