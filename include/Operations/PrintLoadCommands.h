//
//  Operations/PrintLoadCommands.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintLoadCommands : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        static constexpr auto Kind = Operations::Kind::PrintLoadCommands;

        explicit
        PrintLoadCommands(FILE *OutFile,
                          const struct Options &Options) noexcept;

        ~PrintLoadCommands() noexcept override {}

        enum class RunError : uint32_t {
            None,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}
