//
//  include/Operations/Base.h
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "Objects/MachOMemory.h"
#include "Objects/FatMachOMemory.h"
#include "Objects/MemoryBase.h"
#include "Kind.h"

struct Operation {
public:
    struct Options {
    private:
        OperationKind Kind;
    public:
        constexpr Options(OperationKind Kind) noexcept : Kind(Kind) {}
        constexpr inline OperationKind GetKind() const noexcept { return Kind; }
    };
private:
    OperationKind Kind;
public:
    explicit Operation(OperationKind Kind) noexcept;
    inline OperationKind GetKind() const noexcept { return Kind; }

    bool RequiresMap(OperationKind Kind) noexcept;

    static int
    Run(OperationKind Kind,
        const MemoryObject &Object,
        int Argc,
        const char *Argv[]) noexcept;

    static int
    Run(OperationKind Kind,
        const MemoryObject &Object,
        const struct Options &Options) noexcept;

    static struct Options *
    ParseOptions(int Argc, const char *Argv[], int *IndexOut) = delete;
};

struct PrintOperation : Operation {
public:
    constexpr static inline bool IsOfKind(const Operation &Op) {
        const auto Kind =
            static_cast<std::underlying_type_t<OperationKind>>(Op.GetKind());

        return (Kind & 1);
    }

    struct Options : public Operation::Options {
    public:
        constexpr static inline
        bool IsOfKind(const Operation::Options &Options) noexcept {
            const auto Kind =
                static_cast<std::underlying_type_t<OperationKind>>(
                    Options.GetKind());

            return (Kind & 1);
        }

        FILE *OutFile = stdout;
        FILE *ErrFile = stderr;
        
        Options(OperationKind Kind) noexcept : Operation::Options(Kind) {}
        Options(OperationKind Kind, FILE *OutFile) noexcept
        : Operation::Options(Kind), OutFile(OutFile) {}

        inline FILE *GetOutFile() const noexcept { return OutFile; }
        inline FILE *GetErrFile() const noexcept { return ErrFile; }
    };
public:
    explicit PrintOperation(OperationKind Kind) noexcept;
    bool RequiresMap(OperationKind Kind) noexcept;

    static int
    Run(OperationKind Kind,
        const MemoryObject &Object,
        int Argc,
        const char *Argv[]) noexcept;

    static int
    Run(OperationKind Kind,
        const MemoryObject &Object,
        const struct Options &Options) noexcept;

    static struct Options *
    ParseOptions(int Argc, const char *Argv[], int *IndexOut) = delete;

    static void
    PrintObjectKindNotSupportedError(OperationKind OpKind,
                                     const ConstMemoryObject &Object) noexcept;
};
