//
//  Operations/PrintCStringSection.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include "Objects/MachO.h"
#include "Base.h"
#include <optional>

namespace Operations {
    struct PrintCStringSection : public Base {
    public:
        struct Options {
            bool Sort : 1 = false;
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;

        std::optional<std::string> SegmentName;
        std::string SectionName;
    public:
        static constexpr auto Kind = Operations::Kind::PrintCStringSection;

        explicit
        PrintCStringSection(FILE *OutFile,
                            const std::optional<std::string> &SegmentName,
                            const std::string &SectionName,
                            const struct Options &Options) noexcept;

        ~PrintCStringSection() noexcept override {}

        enum class RunError : uint32_t {
            None,
            EmptySectionName,
            SectionNotFound,
            NotCStringSection,
            HasNoStrings
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept { return Opt; }
    };
}
