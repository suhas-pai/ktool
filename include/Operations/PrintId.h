//
//  PrintId.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintId : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        static constexpr auto Kind = Operations::Kind::PrintId;
        explicit PrintId(FILE *OutFile, const struct Options &Options);

        enum class RunError : uint32_t {
            None,
            NotADylib,
            BadIdString,
            IdNotFound
        };

        virtual
        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        virtual
        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto options() const noexcept { return Opt; }
    };
}
