/*
 * Operations/PrintRebaseOpcodeList.h
 * © suhas pai
 */

#pragma once

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintRebaseOpcodeList : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintRebaseOpcodeList;
        explicit
        PrintRebaseOpcodeList(FILE *OutFile,
                              const struct Options &Options) noexcept;

        ~PrintRebaseOpcodeList() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NoDyldInfo,
            NoOpcodes,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}