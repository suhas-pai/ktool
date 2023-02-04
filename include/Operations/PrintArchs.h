//
//  Operations/PrintArchs.h
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#pragma once

#include "Objects/FatMachO.h"
#include "Base.h"

namespace Operations {
    struct PrintArchs : public Base {
    public:
        struct Options {
            bool Verbose : 1 = false;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintArchs;
        explicit
        PrintArchs(FILE *OutFile, const struct Options &Options) noexcept;

        ~PrintArchs() noexcept override {}

        enum class RunError : uint32_t {
            None,
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        static void
        PrintArch(FILE *OutFile,
                  const MachO::FatArch &Arch,
                  const Objects::Base *ArchObject,
                  uint32_t Ordinal,
                  bool Verbose,
                  bool IsBigEndian,
                  const std::string_view Prefix = "") noexcept;

        static void
        PrintArch64(FILE *OutFile,
                    const MachO::FatArch64 &Arch,
                    const Objects::Base *ArchObject,
                    uint32_t Ordinal,
                    bool Verbose,
                    bool IsBigEndian,
                    const std::string_view Prefix = "") noexcept;

        RunResult run(const Objects::Base &Base) const noexcept override;
        RunResult run(const Objects::FatMachO &MachO) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}

