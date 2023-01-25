/*
 * Operations/PrintBindOpcodeList.h
 * © suhas pai
 */

#pragma once

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintBindOpcodeList : public Base {
    public:
        struct Options {
            bool PrintNormal : 1 = true;
            bool PrintLazy : 1 = true;
            bool PrintWeak : 1 = true;
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintBindOpcodeList;
        explicit
        PrintBindOpcodeList(FILE *OutFile,
                            const struct Options &Options) noexcept;

        ~PrintBindOpcodeList() noexcept override {}

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
