//
//  include/Operations/Base.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"
#include "Objects/DscMemory.h"
#include "Objects/MachOMemory.h"
#include "Objects/FatMachOMemory.h"
#include "Objects/MemoryBase.h"

#include "Info.h"

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
    Operation(OperationKind Kind) noexcept;
    virtual ~Operation() noexcept = default;

    [[nodiscard]] constexpr inline OperationKind getKind() const noexcept {
        return Kind;
    }

    [[nodiscard]]
    constexpr inline std::string_view getOptionShortName() const noexcept {
        return OperationKindGetOptionShortName(getKind());
    }

    [[nodiscard]]
    constexpr inline std::string_view getOptionName() const noexcept {
        return OperationKindGetOptionName(getKind());
    }

    [[nodiscard]]
    constexpr inline const std::string_view &getName() const noexcept {
        return OperationKindGetName(getKind());
    }

    [[nodiscard]]
    constexpr inline std::string_view getDescription() const noexcept {
        return OperationKindGetDescription(getKind());
    }

    bool RequiresMap(OperationKind Kind) noexcept;

    static void
    PrintLineSpamWarning(FILE *OutFile, uint64_t LineAmount) noexcept;

    static void
    PrintObjectKindNotSupportedError(OperationKind OpKind,
                                     const MemoryObject &Object) noexcept;

    constexpr static auto HelpOption = "help"sv;
    constexpr static auto UsageOption = "usage"sv;
    constexpr static auto MaxShortOptionNameLength = 2;
    constexpr static auto InvalidObjectKind = -2;

    static void PrintHelpMenu(FILE *OutFile) noexcept;

    static void
    PrintOptionHelpMenu(FILE *OutFile,
                        OperationKind Kind,
                        const char *Prefix = "") noexcept;

    virtual
    void ParseOptions(const ArgvArray &Argv, int *IndexOut) noexcept = 0;

    virtual int Run(const MemoryObject &Object) const noexcept = 0;
};

template <OperationKind Kind>
using OperationTypeFromKind = typename OperationKindInfo<Kind>::Type;

template <typename T>
using IsSubclassOfOperationOptions =
    typename std::enable_if_t<std::is_base_of<Operation::Options, T>::value>;

template <typename T>
using IsSubclassOfOperation =
    typename std::enable_if_t<std::is_base_of<Operation, T>::value>;

// isa<T> templates
// isa<OperationType>(const Operation &) -> bool

template <typename OperationType,
          typename = IsSubclassOfOperation<OperationType>>

static inline bool isa(const Operation &Object) noexcept {
    return OperationType::IsOfKind(Object);
}

// isa<OperationType>(const Operation *) -> bool

template <typename OperationType,
          typename = IsSubclassOfOperation<OperationType>>

static inline bool isa(const Operation *Object) noexcept {
    return OperationType::IsOfKind(*Object);
}

// isa<Kind>(const Operation &) -> bool

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline bool isa(const Operation &Object) noexcept {
    return OperationType::IsOfKind(Object);
}

// isa<Kind>(const Operation *) -> bool

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline bool isa(const Operation *Object) noexcept {
    return OperationType::IsOfKind(*Object);
}

// isa<OptionType>(const Operation::Options &) -> bool

template <typename OptionType,
          typename = IsSubclassOfOperationOptions<OptionType>>

static inline bool isa(const Operation::Options &Object) noexcept {
    return OptionType::IsOfKind(Object);
}

// isa<OptionType>(const Operation::Options *) -> bool

template <typename OptionType,
          typename = IsSubclassOfOperationOptions<OptionType>>

static inline bool isa(const Operation::Options *Object) noexcept {
    return OptionType::IsOfKind(Object);
}

// isa<Kind>(const Operation &) -> bool

template <OperationKind Kind,
          typename OptionsType =
            struct OperationTypeFromKind<Kind>::Options>

static inline bool isa(const Operation::Options &Object) noexcept {
    return OptionsType::IsOfKind(Object);
}

// isa<Kind>(const Operation *) -> bool

template <OperationKind Kind,
          typename OptionsType =
            struct OperationTypeFromKind<Kind>::Options>

static inline bool isa(const Operation::Options *Object) noexcept {
    return OptionsType::IsOfKind(*Object);
}

// cast<T> templates
// cast<Kind>(Operation &) -> OperationType &

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline OperationType &cast(Operation &Object) {
    assert(isa<Kind>(Object));
    return static_cast<OperationType &>(Object);
}

// cast<Kind>(const Operation &) -> const OperationType &

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline const OperationType &cast(const Operation &Object) {
    assert(isa<Kind>(Object));
    return static_cast<const OperationType &>(Object);
}

// cast<Kind>(Operation *) -> OperationTypePtr

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline OperationType *cast(Operation *Object) {
    assert(isa<Kind>(Object));
    return static_cast<OperationType *>(Object);
}

// cast<Kind>(const Operation *) -> const OperationType *

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline const OperationType *cast(const Operation *Object) {
    assert(isa<Kind>(Object));
    return static_cast<const OperationType *>(Object);
}

// cast<Kind>(Operation::Options &) -> OperationType::Options &

template <OperationKind Kind,
          typename OptionKind = struct OperationTypeFromKind<Kind>::Options>

static inline OptionKind &cast(Operation::Options &Object) {
    assert(isa<Kind>(Object));
    return static_cast<OptionKind &>(Object);
}

// cast<Kind>(const Operation::Options &) -> const OperationType::Options &

template <OperationKind Kind,
          typename OptionKind = struct OperationTypeFromKind<Kind>::Options>

static inline const OptionKind &cast(const Operation::Options &Object) {
    assert(isa<Kind>(Object));
    return static_cast<const OptionKind &>(Object);
}

// cast<Kind>(Operation::Options *) -> OperationType::Options *

template <OperationKind Kind,
          typename OptionKind = struct OperationTypeFromKind<Kind>::Options>

static inline OptionKind *cast(Operation::Options *Object) {
    assert(isa<Kind>(Object));
    return static_cast<OptionKind *>(Object);
}

// cast<Kind>(const Operation::Options *) -> const OperationType::Options *

template <OperationKind Kind,
          typename OptionKind = struct OperationTypeFromKind<Kind>::Options>

static inline const OptionKind *cast(const Operation::Options *Object) {
    assert(isa<Kind>(Object));
    return static_cast<const OptionKind *>(Object);
}

// dyn_cast<T> tempaltes
// dyn_cast<Kind>(Operation *) -> OperationType *

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline OperationType *dyn_cast(Operation *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(const Operation *) -> const OperationType *

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline
const OperationType *dyn_cast(const Operation *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(Operation::Options *) -> OperationType::Options *

template <OperationKind Kind,
          typename OptionType = struct OperationTypeFromKind<Kind>::Options>

static inline OptionType *dyn_cast(Operation::Options *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(const Operation::Options *) -> const OperationType::Options *

template <OperationKind Kind,
          typename OptionType = struct OperationTypeFromKind<Kind>::Options>

static inline
const OptionType *dyn_cast(const Operation::Options *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}
