//
//  src/Operations/Base.cpp
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstdio>

#include "Utils/MachOPrinter.h"
#include "Operation.h"

#include "PrintHeader.h"
#include "PrintLoadCommands.h"
#include "PrintSharedLibraries.h"
#include "PrintId.h"
#include "PrintArchList.h"
#include "PrintFlags.h"

Operation::Operation(OperationKind Kind) noexcept : Kind(Kind) {}

static bool
OperationKindSupportsObjectKind(OperationKind OpKind,
                                ObjectKind ObjKind) noexcept
{
    switch (OpKind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            return
                OperationTypeFromKind<OperationKind::PrintHeader>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintLoadCommands:
            return
                OperationTypeFromKind<OperationKind::PrintLoadCommands>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintSharedLibraries:
            return
                OperationTypeFromKind<OperationKind::PrintSharedLibraries>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintId:
            return
                OperationTypeFromKind<OperationKind::PrintId>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintArchList:
            return
                OperationTypeFromKind<OperationKind::PrintArchList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintFlags:
            return
                OperationTypeFromKind<OperationKind::PrintFlags>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintExportTrie:
            return
                OperationTypeFromKind<OperationKind::PrintExportTrie>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintObjcClassList:
            return
                OperationTypeFromKind<OperationKind::PrintObjcClassList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintBindActionList:
            return
                OperationTypeFromKind<OperationKind::PrintBindActionList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintBindOpcodeList:
            return
                OperationTypeFromKind<OperationKind::PrintBindOpcodeList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintBindSymbolList:
            return
                OperationTypeFromKind<OperationKind::PrintBindSymbolList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintRebaseActionList:
            return
                OperationTypeFromKind<OperationKind::PrintRebaseActionList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintRebaseOpcodeList:
            return
                OperationTypeFromKind<OperationKind::PrintRebaseOpcodeList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintCStringSection:
            return
                OperationTypeFromKind<OperationKind::PrintCStringSection>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintSymbolPtrSection:
            return
                OperationTypeFromKind<OperationKind::PrintSymbolPtrSection>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintImageList:
            return
                OperationTypeFromKind<OperationKind::PrintImageList>::
                    SupportsObjectKind(ObjKind);
    }

    assert(0 && "Reached end of OperationKindSupportsObjectKind()");
}

static
void PrintSelectArchMessage(const ConstFatMachOMemoryObject &Object) noexcept {
    const auto ArchCount = Object.getArchCount();
    fprintf(stdout, "Please select one of %" PRIu32 " archs", ArchCount);

    if (ArchCount > 3) {
        fprintf(stdout, ". Use option --list-archs to see a list of archs\n");
        return;
    }

    fputs(":\n", stdout);
    MachOTypePrinter<MachO::FatHeader>::PrintArchList(stdout,
                                                      Object.getConstHeader(),
                                                      "",
                                                      "");
}

void
Operation::PrintLineSpamWarning(FILE *OutFile, uint64_t LineAmount) noexcept {
    if (!isatty(fileno(OutFile))) {
        return;
    }

    if (LineAmount < 4500) {
        return;
    }

    constexpr static auto DelayAmount = 2;
    fprintf(OutFile,
            "Warning: %" PRIu64 " Lines will be printed in %d seconds\n\n",
            LineAmount,
            DelayAmount);

    sleep(DelayAmount);
}

void
Operation::PrintObjectKindNotSupportedError(
    OperationKind OpKind,
    const MemoryObject &Object) noexcept
{
    assert(OpKind != OperationKind::None);
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            if (OperationKindSupportsObjectKind(OpKind, ObjectKind::FatMachO)) {
                fprintf(stderr,
                        "Operation %s only supports Fat Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
        case ObjectKind::FatMachO:
            if (OperationKindSupportsObjectKind(OpKind, ObjectKind::MachO)) {
                PrintSelectArchMessage(cast<ObjectKind::FatMachO>(Object));
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
        case ObjectKind::DyldSharedCache:
            fprintf(stderr,
                    "Operation %s is not supported for Dyld Shared-Cache "
                    "files\n",
                    OperationKindGetName(OpKind).data());
    }

    assert(0 && "Reached end of PrintObjectKindNotSupportedError()");
}

std::string_view
Operation::GetOptionShortName(OperationKind Kind) noexcept {
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
        case OperationKind::PrintFlags:
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

std::string_view Operation::GetOptionName(OperationKind Kind) noexcept {
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
        case OperationKind::PrintFlags:
            return "list-flags"sv;
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

std::string_view
Operation::GetOptionDescription(OperationKind Kind) noexcept {
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
        case OperationKind::PrintFlags:
            return "List Header-Flags of a Thin Mach-O File"sv;
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

void
Operation::PrintOptionHelpMenu(OperationKind Kind,
                               FILE *OutFile,
                               const char *Prefix) noexcept
{
    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
        case OperationKind::PrintLoadCommands:
        case OperationKind::PrintSharedLibraries:
        case OperationKind::PrintId:
        case OperationKind::PrintArchList:
        case OperationKind::PrintFlags:
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintExportTrie:
            fprintf(OutFile,
                    "%s    --require-kind,    Only print exports with a "
                    "specific kind\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --require-section, Only print exports with a "
                    "specific section\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort,            Sort every node and exports\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,         Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintObjcClassList:
            fprintf(OutFile,
                    "%s    --include-categories,    Include Objective-C Class "
                    "Categories\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-dylib-ordinal, Sort Objective-C Classes "
                    "by Dylib-Ordinal\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-kind,          Sort Objective-C Classes "
                    "by Kind\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-name,          Sort Objective-C Classes "
                    "by Name\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --tree,                  Print Objective-C Classes "
                    "in a tree\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    Prefix);
            break;
        case OperationKind::PrintBindActionList:
            fprintf(OutFile,
                    "%s    --only-normal,           Only Print Normal-Bind "
                    "Action-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --only-lazy,             Only Print Lazy-Bind "
                    "Action-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --only-weak,             Only Print Weak-Bind "
                    "Action-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-dylib-ordinal, Sort Bind-Actions by "
                    "Dylib-Ordinal\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-name,          Sort Bind-Actions by "
                    "Symbol-Name\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-type,          Sort Bind-Actions by "
                    "Type\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    Prefix);
            break;
        case OperationKind::PrintBindOpcodeList:
            fprintf(OutFile,
                    "%s    --only-normal,           Only Print Normal-Bind "
                    "Opcode-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --only-lazy,             Only Print Lazy-Bind "
                    "Opcode-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --only-weak,             Only Print Weak-Bind "
                    "Opcode-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    Prefix);
            break;
        case OperationKind::PrintBindSymbolList:
            fprintf(OutFile,
                    "%s    --only-normal,           Only Print Normal-Bind "
                    "Action-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --only-lazy,             Only Print Lazy-Bind "
                    "Action-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --only-weak,             Only Print Weak-Bind "
                    "Action-List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-dylib-ordinal, Sort Bind-Actions by "
                    "Dylib-Ordinal\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-name,          Sort Bind-Actions by "
                    "Symbol-Name\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-type,          Sort Bind-Actions by "
                    "Type\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    Prefix);
            break;
        case OperationKind::PrintRebaseActionList:
            fprintf(OutFile,
                    "%s    --sort,    Sort Rebase-Actions\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintRebaseOpcodeList:
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintCStringSection:
            fprintf(OutFile,
                    "%s    --sort,    Sort C-String List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintSymbolPtrSection:
            fprintf(OutFile,
                    "%s    --sort-by-dylib-ordinal, Sort C-String List by "
                    "Dylib-Ordinal\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-index,         Sort C-String List by "
                    "Index\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-symbol,        Sort C-String List by "
                    "Symbol-Name\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    Prefix);
            break;
        case OperationKind::PrintImageList:
            fprintf(OutFile,
                    "%s    --sort,    Sort Image List\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
    }
}

void Operation::PrintHelpMenu(FILE *OutFile) noexcept {
    constexpr auto OperationKindList = magic_enum::enum_values<OperationKind>();
    auto LongestOptionNameLength = LargestIntHelper<int>();

    const auto Begin = OperationKindList.begin() + 1;
    const auto End = OperationKindList.end();

    for (auto Iter = Begin; Iter != End; Iter++) {
        const auto &IKind = *Iter;

        const auto ShortName = GetOptionShortName(IKind);
        const auto Name = GetOptionName(IKind);
        const auto Description = GetOptionDescription(IKind);

        assert(!Name.empty());
        assert(!Description.empty());

        assert(ShortName != Description && Name != Description);
        assert(ShortName.size() <= MaxShortOptionNameLength);

        LongestOptionNameLength = static_cast<int>(Name.length());
        for (auto Jter = Begin; Jter != Iter; Jter++) {
            const auto &JKind = *Jter;
            if (!ShortName.empty()) {
                assert(ShortName != GetOptionShortName(JKind));
            }

            assert(Name != GetOptionName(JKind));
            assert(Description != GetOptionDescription(JKind));
        }
    }

    LongestOptionNameLength = HelpOption.length();
    LongestOptionNameLength = UsageOption.length();

    fprintf(OutFile,
            "\t%5s--%s,%*s Print this Menu\n",
            "",
            HelpOption.data(),
            static_cast<int>(LongestOptionNameLength - HelpOption.length()),
            "");

    fprintf(OutFile,
            "\t%5s--%s,%*s Print this Menu\n\n",
            "",
            UsageOption.data(),
            static_cast<int>(LongestOptionNameLength - UsageOption.length()),
            "");

    for (auto Iter = Begin; Iter != End; Iter++) {
        const auto &Kind = *Iter;
        if (Kind == OperationKind::None) {
            continue;
        }

        const auto &ShortName = GetOptionShortName(Kind);
        if (!ShortName.empty()) {
            fprintf(OutFile, "\t-%s, ", ShortName.data());
        } else {
            fprintf(OutFile, "\t%3s", "");
        }

        PrintUtilsRightPadSpaces(OutFile,
                                 static_cast<int>(ShortName.size()),
                                 MaxShortOptionNameLength);

        const auto &Name = GetOptionName(Kind);
        PrintUtilsRightPadSpaces(OutFile,
                                 fprintf(OutFile, "--%s,", Name.data()),
                                 LongestOptionNameLength + LENGTH_OF("--,"));

        fprintf(OutFile, " %s\n", GetOptionDescription(Kind).data());
        PrintOptionHelpMenu(Kind, stdout, "\t\t\t");

        if (Iter != (End - 1)) {
            fputc('\n', stdout);
        }
    }
}
