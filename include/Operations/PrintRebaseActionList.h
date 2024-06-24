/*
 * Operations/PrintRebaseActionList.h
 * © suhas pai
 */

#pragma once

#include "Objects/MachO.h"
#include "Operations/Base.h"

namespace Operations {
    struct PrintRebaseActionList : public Base {
    public:
        struct Options {
            bool Sort : 1 = false;
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintRebaseActionList;
        explicit
        PrintRebaseActionList(FILE *OutFile,
                              const struct Options &Options) noexcept;

        ~PrintRebaseActionList() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NoDyldInfo,
            NoActions
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}