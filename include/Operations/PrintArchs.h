//
//  Operations/PrintArchs.h
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#pragma once

#include "Objects/FatMachO.h"
#include "Base.h"

namespace Operations {
    struct PrintArchs : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        static constexpr auto Kind = Operations::Kind::PrintArchs;
        explicit PrintArchs(FILE *OutFile, const struct Options &Options);

        enum class RunError : uint32_t {
            None,
        };

        virtual
        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        virtual
        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::FatMachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept { return Opt; }
    };
}

