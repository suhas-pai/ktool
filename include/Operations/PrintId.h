//
//  Operations/PrintId.h
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
        constexpr static auto Kind = Operations::Kind::PrintId;
        explicit PrintId(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintId() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NotADylib,
            BadIdString,
            IdNotFound
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}
