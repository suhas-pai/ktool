//
//  Operations/PrintCStringSection.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once
#include <limits>

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintCStringSection : public Base {
    public:
        struct Options {
            bool Sort : 1 = false;
            bool Verbose : 1 = false;
            uint64_t Limit = std::numeric_limits<uint64_t>::max();
        };
    protected:
        FILE *OutFile;
        Options Opt;

        std::optional<std::string> SegmentName;
        std::string SectionName;
    public:
        constexpr static auto Kind = Operations::Kind::PrintCStringSection;

        explicit
        PrintCStringSection(FILE *OutFile,
                            const std::optional<std::string> &SegmentName,
                            const std::string_view SectionName,
                            const struct Options &Options) noexcept;

        ~PrintCStringSection() noexcept override {}

        struct RunResult {
            enum class Error : uint32_t {
                None,
                Unsupported,

                EmptySectionName,
                SectionNotFound,
                ProtectedSegment,
                NotCStringSection,
                HasNoStrings
            };

            Error Error = Error::None;
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}
