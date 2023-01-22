//
//  Operations/PrintSymbolPtrSection.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include <limits>
#include <optional>
#include <vector>

#include "Objects/MachO.h"
#include "Base.h"

namespace Operations {
    struct PrintSymbolPtrSection : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
            bool SkipInvalidIndices : 1 = true;

            enum class SortKind {
                ByDylibOrdinal,
                ByDylibPath,
                ByIndex,
                ByString,
            };

            std::vector<SortKind> SortKindList;
            uint64_t Limit = std::numeric_limits<uint64_t>::max();
        };
    protected:
        FILE *OutFile;
        Options Opt;

        std::optional<std::string> SegmentName;
        std::string SectionName;
    public:
        constexpr static auto Kind = Operations::Kind::PrintSymbolPtrSection;

        explicit
        PrintSymbolPtrSection(FILE *OutFile,
                              const std::optional<std::string> &SegmentName,
                              const std::string &SectionName,
                              const struct Options &Options) noexcept;

        ~PrintSymbolPtrSection() noexcept override {}

        enum class RunError : uint32_t {
            None,
            EmptySectionName,
            SectionNotFound,
            ProtectedSegment,
            NotSymbolPointerSection,
            InvalidSectionRange,
            SymTabNotFound,
            DynamicSymTabNotFound,
            MultipleSymTabCommands,
            MultipleDynamicSymTabCommands,
            IndexListOutOfBounds,
            SymbolTableOutOfBounds,
            StringTableOutOfBounds,
            IndexOutOfBounds,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}
