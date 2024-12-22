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

        struct RunResult {
            enum class Error : uint32_t {
                None,
                Unsupported,

                NoDyldInfo,
                NoOpcodes,
            };

            Error Error = Error::None;
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        auto run(const Objects::Base &Base) const noexcept -> RunResult;
        auto run(const Objects::MachO &MachO) const noexcept -> RunResult;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}