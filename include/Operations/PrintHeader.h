//
//  Operations/PrintHeader.h
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#pragma once

#include "Objects/DyldSharedCache.h"
#include "Objects/FatMachO.h"
#include "Objects/MachO.h"

#include "Base.h"

namespace Operations {
    struct PrintHeader : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintHeader;

        explicit
        PrintHeader(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintHeader() noexcept override {}

        struct RunResult {
            enum class Error : uint32_t {
                None,
            };

            Error Error = Error::None;
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept;
        RunResult run(const Objects::DyldSharedCache &Dsc) const noexcept;
        RunResult run(const Objects::MachO &MachO) const noexcept;
        RunResult run(const Objects::FatMachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}
