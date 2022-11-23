//
//  Operations/PrintLibraries.h
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#pragma once
#include <vector>

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintLibraries : public Base {
    public:
        struct Options {
            enum class SortKind {
                ByCurrentVersion,
                ByCompatVersion,
                ByIndex,
                ByName,
                ByTimeStamp
            };

            std::vector<SortKind> SortKindList;
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        static constexpr auto Kind = Operations::Kind::PrintLibraries;
        explicit PrintLibraries(FILE *OutFile, const struct Options &Options);

        ~PrintLibraries() noexcept override {}

        enum class RunError : uint32_t {
            None,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept { return Opt; }
    };
}
