//
//  include/Operations/Base.h
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"
#include "Objects/MachOMemory.h"
#include "Objects/FatMachOMemory.h"
#include "Objects/MemoryBase.h"

#include "Kind.h"

using namespace std::literals;

struct Operation {
public:
    struct Options {
    private:
        OperationKind Kind;
    public:
        Options(OperationKind Kind) noexcept : Kind(Kind) {}
        [[nodiscard]] constexpr inline OperationKind getKind() const noexcept {
            return Kind;
        }

        FILE *OutFile = stdout;
        FILE *ErrFile = stderr;

        Options(OperationKind Kind, FILE *OutFile) noexcept
        : Kind(Kind), OutFile(OutFile) {}

        Options(OperationKind Kind, FILE *OutFile, FILE *ErrFile) noexcept
        : Kind(Kind), OutFile(OutFile), ErrFile(ErrFile) {}
    };
private:
    OperationKind Kind;
public:
    Operation() noexcept = default;
    Operation(OperationKind Kind) noexcept;

    virtual ~Operation() noexcept = default;

    [[nodiscard]] inline OperationKind getKind() const noexcept { return Kind; }
    bool RequiresMap(OperationKind Kind) noexcept;

    static void
    PrintLineSpamWarning(FILE *OutFile, uint64_t LineAmount) noexcept;

    static void
    PrintObjectKindNotSupportedError(OperationKind OpKind,
                                     const MemoryObject &Object) noexcept;

    [[nodiscard]] static std::string_view
    GetOptionShortName(OperationKind Kind) noexcept;

    [[nodiscard]] static std::string_view
    GetOptionName(OperationKind Kind) noexcept;

    [[nodiscard]] static std::string_view
    GetOptionDescription(OperationKind Kind) noexcept;

    constexpr static auto HelpOption = "help"sv;
    constexpr static auto UsageOption = "usage"sv;
    constexpr static auto MaxShortOptionNameLength = 2;
    constexpr static auto InvalidObjectKind = -2;

    static void PrintHelpMenu(FILE *OutFile) noexcept;

    static void
    PrintOptionHelpMenu(OperationKind Kind,
                        FILE *OutFile,
                        const char *Prefix) noexcept;

    virtual
    void ParseOptions(const ArgvArray &Argv, int *IndexOut) noexcept = 0;
    
    virtual int Run(const MemoryObject &Object) const noexcept = 0;
};
