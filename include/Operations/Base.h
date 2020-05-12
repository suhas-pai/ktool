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
        inline OperationKind GetKind() const noexcept { return Kind; }
    };
private:
    OperationKind Kind;
public:
    explicit Operation(OperationKind Kind) noexcept;
    inline OperationKind GetKind() const noexcept { return Kind; }

    bool RequiresMap(OperationKind Kind) noexcept;

    static void
    PrintObjectKindNotSupportedError(OperationKind OpKind,
                                     const ConstMemoryObject &Object) noexcept;

    static void
    Run(OperationKind Kind,
        const MemoryObject &Object,
        int Argc,
        const char *Argv[]) noexcept;

    static void
    Run(OperationKind Kind,
        const MemoryObject &Object,
        const struct Options &Options) noexcept;

    static struct Options *
    ParseOptions(int Argc, const char *Argv[], int *IndexOut) = delete;
};

struct PrintOperation {
public:
    struct Options : public Operation::Options {
    private:
        FILE *OutFile;
    public:
        Options(OperationKind Kind) noexcept : Operation::Options(Kind) {}
        Options(OperationKind Kind, FILE *OutFile) noexcept
        : Operation::Options(Kind), OutFile(OutFile) {}

        inline FILE *GetOutFile() const noexcept { return OutFile; }
    };
public:
    explicit PrintOperation(OperationKind Kind) noexcept;
    bool RequiresMap(OperationKind Kind) noexcept;

    static void
    Run(OperationKind Kind,
        const MemoryObject &Object,
        int Argc,
        const char *Argv[]) noexcept;

    static void
    Run(OperationKind Kind,
        const MemoryObject &Object,
        const struct Options &Options) noexcept;

    static struct Options *
    ParseOptions(int Argc, const char *Argv[], int *IndexOut) = delete;
};
