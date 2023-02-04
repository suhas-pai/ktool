/*
 * Operations/PrintImageList.h
 * © suhas pai
 */

#pragma once

#include <vector>

#include "Objects/DyldSharedCache.h"
#include "Operations/Base.h"

namespace Operations {
    struct PrintImageList : public Base {
    public:
        struct Options {
            enum class SortKind {
                ByAddress,
                ByInode,
                ByModTime,
                ByName,
            };

            std::vector<SortKind> SortKindList;

            bool OnlyCount : 1 = false;
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintImageList;
        explicit
        PrintImageList(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintImageList() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NoImages,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::DyldSharedCache &Image) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}