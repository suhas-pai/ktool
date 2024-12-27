/*
 * Operations/PrintProgramTrie.h
 * © suhas pai
 */

#pragma once

#include "ADT/Tree.h"
#include "Objects/DyldSharedCache.h"
#include "Base.h"

namespace Operations {
    struct PrintProgramTrie : public Base {
    public:
        struct Options {
            bool OnlyCount : 1 = false;
            bool PrintTree : 1 = false;
            bool Sort : 1 = false;
            bool Verbose : 1 = false;

            uint32_t TabLength = ADT::TreeNode::DefaultTabLength;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintProgramTrie;

        explicit
        PrintProgramTrie(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintProgramTrie() noexcept override {}

        struct RunResult {
            enum class Error : uint32_t {
                None,
                Unsupported,

                NoProgramTrie,
                OutOfBounds,

                InvalidTrieUleb128,
                InvalidTrieFormat,

                OverlappingTrieRanges,
                TrieIsTooDeep,

                NoExports
            };

            Error Error = Error::None;
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        auto run(const Objects::Base &Base) const noexcept -> RunResult;
        auto run(const Objects::DyldSharedCache &MachO) const noexcept
            -> RunResult;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return this->Opt;
        }
    };
}
