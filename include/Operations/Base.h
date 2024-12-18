//
//  Operations/Base.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ArgvArray.h"
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
        [[nodiscard]] constexpr OperationKind getKind() const noexcept {
            return Kind;
        }

        FILE *OutFile = stdout;
        FILE *ErrFile = stderr;

        Options(const OperationKind Kind, FILE *const OutFile) noexcept
        : Kind(Kind), OutFile(OutFile) {}

        Options(const OperationKind Kind,
                FILE *const OutFile,
                FILE *const ErrFile) noexcept
        : Kind(Kind), OutFile(OutFile), ErrFile(ErrFile) {}
    };
private:
    OperationKind Kind;
public:
    Operation(OperationKind Kind) noexcept;
    virtual ~Operation() noexcept = default;

    [[nodiscard]] constexpr auto getKind() const noexcept {
        return this->Kind;
    }

    [[nodiscard]]
    constexpr auto getOptionShortName() const noexcept {
        return OperationKindGetOptionShortName(this->getKind());
    }

    [[nodiscard]] constexpr auto getOptionName() const noexcept {
        return OperationKindGetOptionName(this->getKind());
    }

    [[nodiscard]] constexpr auto getName() const noexcept {
        return OperationKindGetName(this->getKind());
    }

    [[nodiscard]] constexpr auto getDescription() const noexcept {
        return OperationKindGetDescription(this->getKind());
    }

    bool RequiresMap(OperationKind Kind) noexcept;

    static void
    PrintLineSpamWarning(FILE *const OutFile, uint64_t LineAmount) noexcept;

    static void
    PrintObjectKindNotSupportedError(OperationKind OpKind,
                                     const MemoryObject &Object) noexcept;

    inline const Operation &
    printObjectKindNotSupportedError(const MemoryObject &Object) const noexcept
    {
        PrintObjectKindNotSupportedError(this->getKind(), Object);
        return *this;
    }

    constexpr static auto HelpOption = "help"sv;
    constexpr static auto UsageOption = "usage"sv;
    constexpr static auto MaxShortOptionNameLength = 2;
    constexpr static auto InvalidObjectKind = -2;

    static void PrintHelpMenu(FILE *const OutFile) noexcept;

    static void
    PrintOptionHelpMenu(FILE *const OutFile,
                        OperationKind Kind,
                        const char *const Prefix = "",
                        const char *LinePrefix = "",
                        const char *const Suffix = "") noexcept;

    static void
    PrintObjectKindSupportsList(FILE *const OutFile,
                                OperationKind ForKind,
                                const char *const Prefix = "",
                                const char *LinePrefix = "",
                                const char *const Suffix = "") noexcept;

    static void
    PrintPathOptionHelpMenu(FILE *const OutFile,
                            OperationKind ForKind,
                            const char *const Prefix = "",
                            const char *LinePrefix = "",
                            const char *const Suffix = "") noexcept;

    static inline void
    PrintEntireOptionUsageMenu(FILE *const OutFile,
                               const OperationKind ForKind,
                               const char *const Prefix = "",
                               const char *const LinePrefix = "",
                               const char *const Suffix = "") noexcept
    {
        PrintObjectKindSupportsList(OutFile, ForKind, Prefix, LinePrefix);
        PrintOptionHelpMenu(OutFile, ForKind, "", LinePrefix);
        PrintPathOptionHelpMenu(OutFile, ForKind, "\n", LinePrefix, Suffix);
    }

    [[nodiscard]] static
    bool SupportsObjectKind(OperationKind OpKind, ObjectKind ObjKind) noexcept;

    inline const Operation &
    printOptionHelpMenu(FILE *const OutFile,
                        const char *const Prefix = "",
                        const char *const LinePrefix = "",
                        const char *const Suffix = "") const noexcept
    {
        PrintOptionHelpMenu(OutFile,
                            this->getKind(),
                            Prefix,
                            LinePrefix,
                            Suffix);

        return *this;
    }

    inline const Operation &
    printObjectKindSupportsList(FILE *const OutFile,
                                const char *const Prefix = "",
                                const char *const LinePrefix = "",
                                const char *const Suffix = "") const noexcept
    {
        PrintObjectKindSupportsList(OutFile,
                                    this->getKind(),
                                    Prefix,
                                    LinePrefix,
                                    Suffix);

        return *this;
    }

    inline const Operation &
    printPathOptionHelpMenu(FILE *const OutFile,
                            const char *const Prefix = "",
                            const char *const LinePrefix = "",
                            const char *const Suffix = "") const noexcept
    {
        PrintPathOptionHelpMenu(OutFile,
                                this->getKind(),
                                Prefix,
                                LinePrefix,
                                Suffix);

        return *this;
    }

    inline const Operation &
    printEntireOptionUsageMenu(FILE *const OutFile,
                               const char *const Prefix = "",
                               const char *const LinePrefix = "",
                               const char *const Suffix = "") const noexcept
    {
        PrintEntireOptionUsageMenu(OutFile,
                                   this->getKind(),
                                   Prefix,
                                   LinePrefix,
                                   Suffix);

        return *this;
    }

    [[nodiscard]]
    inline bool supportsObjectKind(const ObjectKind Kind) const noexcept {
        return SupportsObjectKind(this->getKind(), Kind);
    }

    virtual int ParseOptions(const ArgvArray &Argv) noexcept = 0;
    virtual int Run(const MemoryObject &Object) const noexcept = 0;
};

template <OperationKind Kind>
using OperationTypeFromKind = typename OperationKindInfo<Kind>::Type;

template <typename T>
concept SubclassOfOperationOptions = std::is_base_of_v<Operation::Options, T>;

template <typename T>
concept SubclassOfOperation = std::is_base_of_v<Operation, T>;

// isa<T> templates
// isa<OperationType>(const Operation &) -> bool

template <SubclassOfOperation OperationType>
static inline bool isa(const Operation &Object) noexcept {
    return OperationType::IsOfKind(Object);
}

// isa<OperationType>(const Operation *) -> bool

template <SubclassOfOperation OperationType>
static inline bool isa(const Operation *const Object) noexcept {
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

static inline bool isa(const Operation *const Object) noexcept {
    return OperationType::IsOfKind(*Object);
}

// isa<OptionType>(const Operation::Options &) -> bool

template <SubclassOfOperationOptions OptionType>
static inline bool isa(const Operation::Options &Object) noexcept {
    return OptionType::IsOfKind(Object);
}

// isa<OptionType>(const Operation::Options *) -> bool

template <SubclassOfOperationOptions OptionType>
static inline bool isa(const Operation::Options *const Object) noexcept {
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

static inline bool isa(const Operation::Options *const Object) noexcept {
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

static inline OperationType *cast(Operation *const Object) {
    assert(isa<Kind>(Object));
    return static_cast<OperationType *>(Object);
}

// cast<Kind>(const Operation *) -> const OperationType *

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline const OperationType *cast(const Operation *const Object) {
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

static inline OptionKind *cast(Operation::Options *const Object) {
    assert(isa<Kind>(Object));
    return static_cast<OptionKind *>(Object);
}

// cast<Kind>(const Operation::Options *) -> const OperationType::Options *

template <OperationKind Kind,
          typename OptionKind = struct OperationTypeFromKind<Kind>::Options>

static inline const OptionKind *cast(const Operation::Options *const Object) {
    assert(isa<Kind>(Object));
    return static_cast<const OptionKind *>(Object);
}

// dyn_cast<T> templates
// dyn_cast<Kind>(Operation *) -> OperationType *

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline OperationType *dyn_cast(Operation *const Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(const Operation *) -> const OperationType *

template <OperationKind Kind,
          typename OperationType = OperationTypeFromKind<Kind>>

static inline
const OperationType *dyn_cast(const Operation *const Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(Operation::Options *) -> OperationType::Options *

template <OperationKind Kind,
          typename OptionType = struct OperationTypeFromKind<Kind>::Options>

static inline OptionType *dyn_cast(Operation::Options *const Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(const Operation::Options *) -> const OperationType::Options *

template <OperationKind Kind,
          typename OptionType = struct OperationTypeFromKind<Kind>::Options>

static inline
const OptionType *dyn_cast(const Operation::Options *const Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}
