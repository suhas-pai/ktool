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

        struct RunResult {
            enum class Error : uint32_t {
                None,
                Unsupported,

                NoDyldInfo,
                NoOpcodes,
            };

            Error Error;
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        auto run(const Objects::Base &Base) const noexcept -> RunResult;
        auto run(const Objects::MachO &MachO) const noexcept -> RunResult;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return this->Opt;
        }
    };
}
