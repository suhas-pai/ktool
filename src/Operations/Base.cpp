//
//  src/Operations/Base.cpp
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cassert>
#include <cstdio>

#include "Utils/MachOPrinter.h"
#include "Operation.h"

#include "PrintHeader.h"
#include "PrintLoadCommands.h"
#include "PrintSharedLibraries.h"
#include "PrintId.h"
#include "PrintArchList.h"

Operation::Operation(OperationKind Kind) noexcept : Kind(Kind) {
    assert(Kind != OperationKind::None);
}

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

static
void PrintSelectArchMessage(const ConstFatMachOMemoryObject &Object) noexcept {
    const auto ArchCount = Object.getArchCount();
    fprintf(stdout, "Please select one of %" PRIu32 " archs", ArchCount);

    if (ArchCount > 3) {
        fprintf(stdout, ". Use option --list-archs to see a list of archs\n");
        return;
    }

    fputs(":\n", stdout);

    const auto &Header = Object.getConstHeader();
    MachOTypePrinter<MachO::FatHeader>::PrintArchList(stdout, Header);
}

static
void PrintSelectImageMessage(const ConstDscMemoryObject &Object) noexcept {
    fprintf(stdout,
            "Please select one of %" PRIu32 " images.\n"
            "Use option --list-dsc-images to see a list of images\n",
            Object.getImageCount());
}

void
Operation::PrintObjectKindNotSupportedError(OperationKind OpKind,
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
            if (OperationKindSupportsObjectKind(OpKind, ObjectKind::DscImage)) {
                const auto &DscObj = cast<ObjectKind::DyldSharedCache>(Object);
                PrintSelectImageMessage(DscObj);
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Dyld Shared-Cache "
                        "files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
        case ObjectKind::DscImage: {
            const auto ObjKind = ObjectKind::DyldSharedCache;
            if (OperationKindSupportsObjectKind(OpKind, ObjKind)) {
                fprintf(stderr,
                        "Operation %s only supports Dyld Shared-Cache files\n",
                        OperationKindGetName(OpKind).data());
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Dyld Shared-Cache "
                        "Image files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
        }
    }

    assert(0 && "Reached end of PrintObjectKindNotSupportedError()");
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
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintSharedLibraries:
            fprintf(OutFile,
                    "%s    --sort-by-current-version, Sort by "
                    "Current-Version\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-compat-version,  Sort by Compat-Version\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-index,           Sort by LC Index\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-name,            Sort by Name\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-timestamp,       Sort by TimeStamp\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --verbose,                 Print more Verbose "
                    "Information\n",
                    Prefix);
            break;
        case OperationKind::PrintId:
        case OperationKind::PrintArchList:
            fprintf(OutFile,
                    "%s-v, --verbose, Print more Verbose Information\n",
                    Prefix);
            break;
        case OperationKind::PrintExportTrie:
            fprintf(OutFile,
                    "%s    --only-count,      Only print the count of "
                    "exports, or with --tree, print the number of nodes\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --require-kind,    Only print exports with a "
                    "specific kind\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --require-segment, Only print exports with a "
                    "specific segment\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --require-section, Only print exports with a "
                    "specific segment & section\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort,            Sort every node alphabetically\n",
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
                    "%s    --count,              Only print image-count\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-address,    Sort Image List by "
                    "Image-Address\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-inode,      Sort Image List by Inode\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-modtime,    Sort Image List by "
                    "Modification-Time\n",
                    Prefix);
            fprintf(OutFile,
                    "%s    --sort-by-name,       Sort Image List by Name\n",
                    Prefix);
            fprintf(OutFile,
                    "%s-v, --verbose,            Print more Verbose "
                    "Information\n",
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

        const auto ShortName = OperationKindGetOptionShortName(IKind);
        const auto Name = OperationKindGetOptionName(IKind);
        const auto Description = OperationKindGetDescription(IKind);

        assert(!Name.empty() && !Description.empty());

        assert(ShortName != Description && Name != Description);
        assert(ShortName.size() <= MaxShortOptionNameLength);

        LongestOptionNameLength = static_cast<int>(Name.length());
        for (auto Jter = Begin; Jter != Iter; Jter++) {
            const auto &JKind = *Jter;
            if (!ShortName.empty()) {
                assert(ShortName != OperationKindGetOptionShortName(JKind));
            }

            assert(Name != OperationKindGetName(JKind));
            assert(Description != OperationKindGetDescription(JKind));
        }
    }

    LongestOptionNameLength = HelpOption.length();
    LongestOptionNameLength = UsageOption.length();

    fprintf(OutFile,
            "\t%5s--%s,%" PRINTF_LEFTPAD_FMT "s Print this Menu\n",
            "",
            HelpOption.data(),
            static_cast<int>(LongestOptionNameLength - HelpOption.length()),
            "");

    fprintf(OutFile,
            "\t%5s--%s,%" PRINTF_LEFTPAD_FMT "s Print this Menu\n\n",
            "",
            UsageOption.data(),
            static_cast<int>(LongestOptionNameLength - UsageOption.length()),
            "");

    for (auto Iter = Begin; Iter != End; Iter++) {
        const auto &Kind = *Iter;
        if (Kind == OperationKind::None) {
            continue;
        }

        const auto &ShortName = OperationKindGetOptionShortName(Kind);
        if (!ShortName.empty()) {
            fprintf(OutFile, "\t-%s, ", ShortName.data());
        } else {
            fprintf(OutFile, "\t%3s", "");
        }

        PrintUtilsRightPadSpaces(OutFile,
                                 static_cast<int>(ShortName.size()),
                                 MaxShortOptionNameLength);

        const auto &Name = OperationKindGetName(Kind);
        PrintUtilsRightPadSpaces(OutFile,
                                 fprintf(OutFile, "--%s,", Name.data()),
                                 LongestOptionNameLength + LENGTH_OF("--,"));

        fprintf(OutFile, " %s\n", OperationKindGetDescription(Kind).data());
        PrintOptionHelpMenu(Kind, stdout, "\t\t\t");

        if (Iter != (End - 1)) {
            fputc('\n', stdout);
        }
    }
}
