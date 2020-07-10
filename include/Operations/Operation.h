//
//  include/Operations/Operation.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include "Base.h"
#include "Kind.h"
#include "PrintHeader.h"
#include "PrintLoadCommands.h"
#include "PrintSharedLibraries.h"
#include "PrintId.h"
#include "PrintArchList.h"
#include "PrintExportTrie.h"
#include "PrintFlags.h"
#include "PrintObjcClassList.h"
#include "PrintBindActionList.h"
#include "PrintBindOpcodeList.h"
#include "PrintBindSymbolList.h"
#include "PrintRebaseActionList.h"
#include "PrintRebaseOpcodeList.h"
#include "PrintCStringSection.h"
#include "PrintSymbolPtrSection.h"

using namespace std::literals;

template <OperationKind Kind>
struct OperationKindInfo {};

template<>
struct OperationKindInfo<OperationKind::PrintHeader> {
    constexpr static const auto Kind = OperationKind::PrintHeader;
    constexpr static const auto Name = "print-header"sv;

    typedef PrintHeaderOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintLoadCommands> {
    constexpr static const auto Kind = OperationKind::PrintLoadCommands;
    constexpr static const auto Name = "print-load-commands"sv;

    typedef PrintLoadCommandsOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintSharedLibraries> {
    constexpr static const auto Kind = OperationKind::PrintSharedLibraries;
    constexpr static const auto Name = "print-shared-libraries"sv;

    typedef PrintSharedLibrariesOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintId> {
    constexpr static const auto Kind = OperationKind::PrintId;
    constexpr static const auto Name = "print-id"sv;

    typedef PrintIdOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintArchList> {
    constexpr static const auto Kind = OperationKind::PrintArchList;
    constexpr static const auto Name = "print-arch-list"sv;

    typedef PrintArchListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintFlags> {
    constexpr static const auto Kind = OperationKind::PrintFlags;
    constexpr static const auto Name = "print-flags"sv;

    typedef PrintFlagsOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintExportTrie> {
    constexpr static const auto Kind = OperationKind::PrintFlags;
    constexpr static const auto Name = "print-export-trie"sv;

    typedef PrintExportTrieOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintObjcClassList> {
    constexpr static const auto Kind = OperationKind::PrintFlags;
    constexpr static const auto Name = "print-objc-class-list"sv;

    typedef PrintObjcClassListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintBindActionList> {
    constexpr static const auto Kind = OperationKind::PrintBindActionList;
    constexpr static const auto Name = "print-bind-actions"sv;

    typedef PrintBindActionListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintBindOpcodeList> {
    constexpr static const auto Kind = OperationKind::PrintBindOpcodeList;
    constexpr static const auto Name = "print-bind-opcodes"sv;

    typedef PrintBindOpcodeListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintBindSymbolList> {
    constexpr static const auto Kind = OperationKind::PrintBindSymbolList;
    constexpr static const auto Name = "print-bind-symbols"sv;

    typedef PrintBindSymbolListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintRebaseActionList> {
    constexpr static const auto Kind = OperationKind::PrintRebaseActionList;
    constexpr static const auto Name = "print-rebase-actions"sv;

    typedef PrintRebaseActionListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintRebaseOpcodeList> {
    constexpr static const auto Kind = OperationKind::PrintRebaseOpcodeList;
    constexpr static const auto Name = "print-rebase-opcodes"sv;

    typedef PrintRebaseOpcodeListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintCStringSection> {
    constexpr static const auto Kind = OperationKind::PrintCStringSection;
    constexpr static const auto Name = "print-c-string-section"sv;

    typedef PrintCStringSectionOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintSymbolPtrSection> {
    constexpr static const auto Kind = OperationKind::PrintSymbolPtrSection;
    constexpr static const auto Name = "print-symbol-pointer-section"sv;

    typedef PrintSymbolPtrSectionOperation Type;
};


constexpr const std::string_view &
OperationKindGetName(OperationKind Kind) noexcept {
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "OperationKindGetName() got Operation-Kind None");
        case OperationKind::PrintHeader:
            return OperationKindInfo<OperationKind::PrintHeader>::Name;
        case OperationKind::PrintLoadCommands:
            return OperationKindInfo<OperationKind::PrintLoadCommands>::Name;
        case OperationKind::PrintSharedLibraries:
            return OperationKindInfo<OperationKind::PrintSharedLibraries>::Name;
        case OperationKind::PrintId:
            return OperationKindInfo<OperationKind::PrintId>::Name;
        case OperationKind::PrintArchList:
            return OperationKindInfo<OperationKind::PrintArchList>::Name;
        case OperationKind::PrintFlags:
            return OperationKindInfo<OperationKind::PrintFlags>::Name;
        case OperationKind::PrintExportTrie:
            return OperationKindInfo<OperationKind::PrintFlags>::Name;
        case OperationKind::PrintObjcClassList:
            return OperationKindInfo<OperationKind::PrintObjcClassList>::Name;
        case OperationKind::PrintBindActionList:
            return OperationKindInfo<OperationKind::PrintBindActionList>::Name;
        case OperationKind::PrintBindOpcodeList:
            return OperationKindInfo<OperationKind::PrintBindOpcodeList>::Name;
        case OperationKind::PrintBindSymbolList:
            return OperationKindInfo<OperationKind::PrintBindSymbolList>::Name;
        case OperationKind::PrintRebaseActionList:
            return OperationKindInfo<
                OperationKind::PrintRebaseActionList>::Name;
        case OperationKind::PrintRebaseOpcodeList:
            return OperationKindInfo<
                OperationKind::PrintRebaseOpcodeList>::Name;
        case OperationKind::PrintCStringSection:
            return OperationKindInfo<OperationKind::PrintCStringSection>::Name;
        case OperationKind::PrintSymbolPtrSection:
            return OperationKindInfo<
                OperationKind::PrintSymbolPtrSection>::Name;
    }

    assert(0 && "Reached end of OperationKindGetName()");
}

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
