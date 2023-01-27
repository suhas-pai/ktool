//
//  Operations/PrintObjcClassList.h
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#pragma once
#include <vector>

#include "Objects/FatMachO.h"
#include "Base.h"

namespace Operations {
    struct PrintObjcClassList : public Base {
    public:
        struct Options {
            bool PrintCategories : 1 = false;
            bool PrintTree : 1 = false;
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
        constexpr static auto Kind = Operations::Kind::PrintObjcClassList;
        explicit
        PrintObjcClassList(FILE *OutFile,
                           const struct Options &Options) noexcept;

        ~PrintObjcClassList() noexcept override {}

        enum class RunError : uint32_t {
            None,
            NoDyldInfo,
            NoObjcData,
            UnalignedSection,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::MachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}