/*
 * Operations/PrintExportTrie.h
 * © suhas pai
 */

#pragma once

#include <set>

#include "MachO/ExportTrie.h"
#include "Objects/MachO.h"
#include "Operations/Base.h"

namespace Operations {
    struct PrintExportTrie : public Base {
    public:
        struct Options {
            bool OnlyCount : 1 = false;
            bool PrintTree : 1 = false;
            bool Sort : 1 = false;
            bool Verbose : 1 = false;

            struct SegmentSectionPair {
                std::optional<std::string> SegmentName;
                std::optional<std::string> SectionName;

                [[nodiscard]] constexpr
                auto operator==(const SegmentSectionPair &Rhs) const noexcept {
                    const auto Result =
                        SegmentName == Rhs.SegmentName &&
                        SectionName == Rhs.SectionName;

                    return Result;
                }
            };

            std::vector<SegmentSectionPair> SectionRequirements;
            std::set<MachO::ExportTrieExportKind> KindRequirements;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintExportTrie;
        explicit
        PrintExportTrie(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintExportTrie() noexcept override {}

        enum class RunError : uint32_t {
            None,
            MultipleExportTries,
            NoExportTrieFound,
            ExportTrieOutOfBounds,
            NoExports
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}