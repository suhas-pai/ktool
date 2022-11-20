//
//  Operations/PrintHeader.h
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#pragma once

#include "Objects/FatMachO.h"
#include "Objects/MachO.h"

#include "Base.h"

namespace Operations {
    struct PrintHeader : public Base {
    public:
        struct Options {
            bool Verbose : 1;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        static constexpr auto Kind = Operations::Kind::PrintHeader;
        explicit PrintHeader(FILE *OutFile, const struct Options &Options);

        virtual bool run(const Objects::Base &Base) const noexcept override;

        void run(const Objects::MachO &MachO) const noexcept;
        void run(const Objects::FatMachO &MachO) const noexcept;
    };
}
