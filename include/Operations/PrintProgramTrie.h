/*
 * Operations/PrintProgramTrie.h
 * © suhas pai
 */

#pragma once

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
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintProgramTrie;

        explicit
        PrintProgramTrie(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintProgramTrie() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NoProgramTrie,
            OutOfBounds,
            NoExports
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::DyldSharedCache &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}
