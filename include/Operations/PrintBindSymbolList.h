/*
 * Operations/PrintBindSymbolList.h
 * © suhas pai
 */

#pragma once

#include <vector>
#include "Operations/Base.h"

namespace Operations {
    struct PrintBindSymbolList : public Base {
    public:
        struct Options {
            bool PrintNormal : 1 = true;
            bool PrintLazy : 1 = true;
            bool PrintWeak : 1 = true;
            bool Verbose : 1 = false;

            enum class SortKind {
                None,
                ByName,
                ByDylibOrdinal,
                ByKind
            };

            std::vector<SortKind> SortKindList;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintBindSymbolList;
        explicit
        PrintBindSymbolList(FILE *OutFile,
                            const struct Options &Options) noexcept;

        ~PrintBindSymbolList() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NoDyldInfo,
            NoSymbols
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}