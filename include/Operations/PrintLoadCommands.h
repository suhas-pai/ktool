//
//  PrintLoadCommands.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include "Objects/FatMachO.h"
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

        enum class RunError : uint32_t {
            None,
        };

        virtual
        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        virtual
        RunResult run(const Objects::Base &Base) const noexcept override;
        auto run(const Objects::MachO &MachO) const noexcept -> RunResult;

        [[nodiscard]] constexpr auto &options() const noexcept { return Opt; }
    };
}
