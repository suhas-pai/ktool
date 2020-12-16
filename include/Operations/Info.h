//
//  include/Operations/Operation.h
//  ktool
//
//  Created by Suhas Pai on 08/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <string_view>

#include "Kind.h"

struct PrintHeaderOperation;
struct PrintLoadCommandsOperation;
struct PrintSharedLibrariesOperation;
struct PrintIdOperation;
struct PrintArchListOperation;
struct PrintExportTrieOperation;
struct PrintObjcClassListOperation;
struct PrintBindActionListOperation;
struct PrintBindOpcodeListOperation;
struct PrintBindSymbolListOperation;
struct PrintRebaseActionListOperation;
struct PrintRebaseOpcodeListOperation;
struct PrintCStringSectionOperation;
struct PrintSymbolPtrSectionOperation;
struct PrintImageListOperation;

using namespace std::literals;

template <OperationKind Kind>
struct OperationKindInfo {};

template<>
struct OperationKindInfo<OperationKind::PrintHeader> {
    constexpr static auto Kind = OperationKind::PrintHeader;
    constexpr static auto Name = "print-header"sv;

    typedef PrintHeaderOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintLoadCommands> {
    constexpr static auto Kind = OperationKind::PrintLoadCommands;
    constexpr static auto Name = "print-load-commands"sv;

    typedef PrintLoadCommandsOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintSharedLibraries> {
    constexpr static auto Kind = OperationKind::PrintSharedLibraries;
    constexpr static auto Name = "print-shared-libraries"sv;

    typedef PrintSharedLibrariesOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintId> {
    constexpr static auto Kind = OperationKind::PrintId;
    constexpr static auto Name = "print-id"sv;

    typedef PrintIdOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintArchList> {
    constexpr static auto Kind = OperationKind::PrintArchList;
    constexpr static auto Name = "print-arch-list"sv;

    typedef PrintArchListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintExportTrie> {
    constexpr static auto Kind = OperationKind::PrintExportTrie;
    constexpr static auto Name = "print-export-trie"sv;

    typedef PrintExportTrieOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintObjcClassList> {
    constexpr static auto Kind = OperationKind::PrintObjcClassList;
    constexpr static auto Name = "print-objc-class-list"sv;

    typedef PrintObjcClassListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintBindActionList> {
    constexpr static auto Kind = OperationKind::PrintBindActionList;
    constexpr static auto Name = "print-bind-actions"sv;

    typedef PrintBindActionListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintBindOpcodeList> {
    constexpr static auto Kind = OperationKind::PrintBindOpcodeList;
    constexpr static auto Name = "print-bind-opcodes"sv;

    typedef PrintBindOpcodeListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintBindSymbolList> {
    constexpr static auto Kind = OperationKind::PrintBindSymbolList;
    constexpr static auto Name = "print-bind-symbols"sv;

    typedef PrintBindSymbolListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintRebaseActionList> {
    constexpr static auto Kind = OperationKind::PrintRebaseActionList;
    constexpr static auto Name = "print-rebase-actions"sv;

    typedef PrintRebaseActionListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintRebaseOpcodeList> {
    constexpr static auto Kind = OperationKind::PrintRebaseOpcodeList;
    constexpr static auto Name = "print-rebase-opcodes"sv;

    typedef PrintRebaseOpcodeListOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintCStringSection> {
    constexpr static auto Kind = OperationKind::PrintCStringSection;
    constexpr static auto Name = "print-c-string-section"sv;

    typedef PrintCStringSectionOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintSymbolPtrSection> {
    constexpr static auto Kind = OperationKind::PrintSymbolPtrSection;
    constexpr static auto Name = "print-symbol-pointer-section"sv;

    typedef PrintSymbolPtrSectionOperation Type;
};

template<>
struct OperationKindInfo<OperationKind::PrintImageList> {
    constexpr static auto Kind = OperationKind::PrintImageList;
    constexpr static auto Name = "print-image-list"sv;

    typedef PrintImageListOperation Type;
};

[[nodiscard]] constexpr
std::string_view OperationKindGetOptionShortName(OperationKind Kind) noexcept {
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Object-Kind is None");
        case OperationKind::PrintHeader:
            return "h"sv;
        case OperationKind::PrintLoadCommands:
            return "l"sv;
        case OperationKind::PrintSharedLibraries:
            return "L"sv;
        case OperationKind::PrintId:
            return "Id"sv;
        case OperationKind::PrintArchList:
        case OperationKind::PrintExportTrie:
        case OperationKind::PrintObjcClassList:
        case OperationKind::PrintBindActionList:
        case OperationKind::PrintBindOpcodeList:
        case OperationKind::PrintBindSymbolList:
        case OperationKind::PrintRebaseActionList:
        case OperationKind::PrintRebaseOpcodeList:
        case OperationKind::PrintCStringSection:
        case OperationKind::PrintSymbolPtrSection:
        case OperationKind::PrintImageList:
            return std::string_view();
    }
}

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
        case OperationKind::PrintExportTrie:
            return OperationKindInfo<OperationKind::PrintExportTrie>::Name;
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
        case OperationKind::PrintImageList:
            return OperationKindInfo<OperationKind::PrintImageList>::Name;
    }

    assert(0 && "Reached end of OperationKindGetName()");
}

[[nodiscard]] constexpr
std::string_view OperationKindGetOptionName(OperationKind Kind) noexcept {
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Object-Kind is None");
        case OperationKind::PrintHeader:
            return "header"sv;
        case OperationKind::PrintLoadCommands:
            return "lc"sv;
        case OperationKind::PrintSharedLibraries:
            return "libraries"sv;
        case OperationKind::PrintId:
            return "Identity"sv;
        case OperationKind::PrintArchList:
            return "list-archs"sv;
        case OperationKind::PrintExportTrie:
            return "list-export-trie"sv;
        case OperationKind::PrintObjcClassList:
            return "list-objc-classes"sv;
        case OperationKind::PrintBindActionList:
            return "list-bind-actions"sv;
        case OperationKind::PrintBindOpcodeList:
            return "list-bind-opcodes"sv;
        case OperationKind::PrintBindSymbolList:
            return "list-bind-symbols"sv;
        case OperationKind::PrintRebaseActionList:
            return "list-rebase-actions"sv;
        case OperationKind::PrintRebaseOpcodeList:
            return "list-rebase-opcodes"sv;
        case OperationKind::PrintCStringSection:
            return "list-c-string-section"sv;
        case OperationKind::PrintSymbolPtrSection:
            return "list-symbol-ptr-section"sv;
        case OperationKind::PrintImageList:
            return "list-dsc-images"sv;
    }
}

[[nodiscard]] constexpr
std::string_view OperationKindGetDescription(OperationKind Kind) noexcept {
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Object-Kind is None");
        case OperationKind::PrintHeader:
            return "Print header of a Mach-O File"sv;
        case OperationKind::PrintLoadCommands:
            return "Print Load-commands of a Thin Mach-O File"sv;
        case OperationKind::PrintSharedLibraries:
            return "Print imported Shared-Libraries of a Thin Mach-O File"sv;
        case OperationKind::PrintId:
            return "Print Identification of a Thin Mach-O File"sv;
        case OperationKind::PrintArchList:
            return "List Archs of a FAT Mach-O File"sv;
        case OperationKind::PrintExportTrie:
            return "List Export-Trie of a Thin Mach-O File"sv;
        case OperationKind::PrintObjcClassList:
            return "List Objc-Classes of a Thin Mach-O File"sv;
        case OperationKind::PrintBindActionList:
            return "List Bind-Actions of a Thin Mach-O File"sv;
        case OperationKind::PrintBindOpcodeList:
            return "List Bind-Opcodes of a Thin Mach-O File"sv;
        case OperationKind::PrintBindSymbolList:
            return "List Bind-Symbols of a Thin Mach-O File"sv;
        case OperationKind::PrintRebaseActionList:
            return "List Rebase-Actions of a Thin Mach-O File"sv;
        case OperationKind::PrintRebaseOpcodeList:
            return "List Rebase-Opcodes of a Thin Mach-O File"sv;
        case OperationKind::PrintCStringSection: {
            const auto Desc =
                "List C-Strings of a C-String Section of a Thin Mach-O File"sv;

            return Desc;
        }
        case OperationKind::PrintSymbolPtrSection: {
            const auto Desc =
                "List Symbols of a Symbol-Ptr Section of a Thin Mach-O File"sv;

            return Desc;
        }
        case OperationKind::PrintImageList:
            return "List Images of a Dyld Shared-Cache File"sv;
    }
}
