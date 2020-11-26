//
//  src/Operations/Base.cpp
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <unistd.h>

#include "Kind.h"
#include "Utils/MachOTypePrinter.h"
#include "Operation.h"

#include "PrintArchList.h"
#include "PrintHeader.h"
#include "PrintLoadCommands.h"
#include "PrintSharedLibraries.h"
#include "PrintId.h"

Operation::Operation(OperationKind Kind) noexcept : Kind(Kind) {
    assert(Kind != OperationKind::None);
}

bool
Operation::SupportsObjectKind(OperationKind OpKind, ObjectKind ObjKind) noexcept
{
    switch (OpKind) {
        using Enum = OperationKind;
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
            return
                OperationTypeFromKind<Enum::PrintHeader>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintLoadCommands:
            return
                OperationTypeFromKind<Enum::PrintLoadCommands>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintSharedLibraries:
            return
                OperationTypeFromKind<Enum::PrintSharedLibraries>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintId:
            return
                OperationTypeFromKind<Enum::PrintId>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintArchList:
            return
                OperationTypeFromKind<Enum::PrintArchList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintExportTrie:
            return
                OperationTypeFromKind<Enum::PrintExportTrie>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintObjcClassList:
            return
                OperationTypeFromKind<Enum::PrintObjcClassList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintBindActionList:
            return
                OperationTypeFromKind<Enum::PrintBindActionList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintBindOpcodeList:
            return
                OperationTypeFromKind<Enum::PrintBindOpcodeList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintBindSymbolList:
            return
                OperationTypeFromKind<Enum::PrintBindSymbolList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintRebaseActionList:
            return
                OperationTypeFromKind<Enum::PrintRebaseActionList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintRebaseOpcodeList:
            return
                OperationTypeFromKind<Enum::PrintRebaseOpcodeList>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintCStringSection:
            return
                OperationTypeFromKind<Enum::PrintCStringSection>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintSymbolPtrSection:
            return
                OperationTypeFromKind<Enum::PrintSymbolPtrSection>::
                    SupportsObjectKind(ObjKind);
        case OperationKind::PrintImageList:
            return
                OperationTypeFromKind<Enum::PrintImageList>::
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

static void PrintSelectImageMessage(const DscMemoryObject &Object) noexcept {
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
            if (Operation::SupportsObjectKind(OpKind, ObjectKind::FatMachO)) {
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
            if (Operation::SupportsObjectKind(OpKind, ObjectKind::MachO)) {
                PrintSelectArchMessage(cast<ObjectKind::FatMachO>(Object));
            } else {
                fprintf(stderr,
                        "Operation %s is not supported for Mach-O files\n",
                        OperationKindGetName(OpKind).data());
            }

            return;
        case ObjectKind::DyldSharedCache:
            if (Operation::SupportsObjectKind(OpKind, ObjectKind::DscImage)) {
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
            if (Operation::SupportsObjectKind(OpKind, ObjKind)) {
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
Operation::PrintOptionHelpMenu(FILE *OutFile,
                               OperationKind Kind,
                               const char *Prefix,
                               const char *LinePrefix,
                               const char *Suffix) noexcept
{
    constexpr auto Tab = "    ";
    fprintf(OutFile, "%s%sOptions:\n", Prefix, LinePrefix);

    switch (Kind) {
        case OperationKind::None:
            assert(0 && "Operation-Kind is None");
        case OperationKind::PrintHeader:
        case OperationKind::PrintLoadCommands:
            fprintf(OutFile,
                    "%s%s-v, --verbose, Print more Verbose Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintSharedLibraries:
            fprintf(OutFile,
                    "%s%s    --sort-by-current-version, Sort by "
                    "Current-Version\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-compat-version,  Sort by "
                    "Compat-Version\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-index,           Sort by LC Index\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-name,            Sort by Name\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-timestamp,       Sort by TimeStamp\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --verbose,                 Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintId:
        case OperationKind::PrintArchList:
            fprintf(OutFile,
                    "%s%s-v, --verbose, Print more Verbose Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintExportTrie:
            fprintf(OutFile,
                    "%s%s    --only-count,      Only print the count of "
                    "exports, or with --tree, print the number of nodes\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --require-kind,    Only print exports with a "
                    "specific kind\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --require-segment, Only print exports with a "
                    "specific segment\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --require-section, Only print exports with a "
                    "specific segment & section\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort,            Sort every node "
                    "alphabetically\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose,         Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintObjcClassList:
            fprintf(OutFile,
                    "%s%s    --include-categories,    Include Objective-C "
                    "Class Categories\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-dylib-ordinal, Sort Objective-C "
                    "Classes by Dylib-Ordinal\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-kind,          Sort Objective-C "
                    "Classes by Kind\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-name,          Sort Objective-C "
                    "Classes by Name\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --tree,                  Print Objective-C "
                    "Classes in a tree\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintBindActionList:
            fprintf(OutFile,
                    "%s%s    --only-normal,           Only Print Normal-Bind "
                    "Action-List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --only-lazy,             Only Print Lazy-Bind "
                    "Action-List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --only-weak,             Only Print Weak-Bind "
                    "Action-List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-dylib-ordinal, Sort Bind-Actions by "
                    "Dylib-Ordinal\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-name,          Sort Bind-Actions by "
                    "Symbol-Name\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-type,          Sort Bind-Actions by "
                    "Type\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintBindOpcodeList:
            fprintf(OutFile,
                    "\t%s    --only-normal,           Only Print Normal-Bind "
                    "Opcode-List\n",
                    LinePrefix);
            fprintf(OutFile,
                    "\t%s    --only-lazy,             Only Print Lazy-Bind "
                    "Opcode-List\n",
                    LinePrefix);
            fprintf(OutFile,
                    "\t%s    --only-weak,             Only Print Weak-Bind "
                    "Opcode-List\n",
                    LinePrefix);
            fprintf(OutFile,
                    "\t%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    LinePrefix);
            break;
        case OperationKind::PrintBindSymbolList:
            fprintf(OutFile,
                    "%s%s    --only-normal,           Only Print Normal-Bind "
                    "Action-List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --only-lazy,             Only Print Lazy-Bind "
                    "Action-List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --only-weak,             Only Print Weak-Bind "
                    "Action-List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-dylib-ordinal, Sort Bind-Actions by "
                    "Dylib-Ordinal\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-name,          Sort Bind-Actions by "
                    "Symbol-Name\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-type,          Sort Bind-Actions by "
                    "Type\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintRebaseActionList:
            fprintf(OutFile,
                    "%s%s    --sort,    Sort Rebase-Actions\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose, Print more Verbose Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintRebaseOpcodeList:
            fprintf(OutFile,
                    "%s%s-v, --verbose, Print more Verbose Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintCStringSection:
            fprintf(OutFile,
                    "%s%s    --sort,    Sort C-String List\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose, Print more Verbose Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintSymbolPtrSection:
            fprintf(OutFile,
                    "%s%s    --sort-by-dylib-ordinal, Sort C-String List by "
                    "Dylib-Ordinal\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-index,         Sort C-String List by "
                    "Index\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-symbol,        Sort C-String List by "
                    "Symbol-Name\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose,               Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
        case OperationKind::PrintImageList:
            fprintf(OutFile,
                    "%s%s    --count,              Only print image-count\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-address,    Sort Image List by "
                    "Image-Address\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-inode,      Sort Image List by Inode\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-modtime,    Sort Image List by "
                    "Modification-Time\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s    --sort-by-name,       Sort Image List by Name\n",
                    LinePrefix,
                    Tab);
            fprintf(OutFile,
                    "%s%s-v, --verbose,            Print more Verbose "
                    "Information\n",
                    LinePrefix,
                    Tab);
            break;
    }

    fprintf(OutFile, "%s", Suffix);
}

static void
PrintItemForSupportList(FILE *OutFile,
                        bool &DidPrint,
                        const char *LinePrefix,
                        const char *Name) noexcept
{
    PrintUtilsWriteItemAfterFirstForList(OutFile, " │ ", DidPrint);
    fprintf(OutFile, "%s%s", LinePrefix, Name);
}

void
Operation::PrintObjectKindSupportsList(FILE *OutFile,
                                       OperationKind OpKind,
                                       const char *Prefix,
                                       const char *LinePrefix,
                                       const char *Suffix) noexcept
{
    fprintf(OutFile, "%s%sSupports: ", Prefix, LinePrefix);

    auto DidPrint = false;
    auto ObjKind = ObjectKind::None;

    switch (ObjKind) {
        case ObjectKind::None:
        case ObjectKind::MachO:
            if (Operation::SupportsObjectKind(OpKind, ObjectKind::MachO)) {
                PrintItemForSupportList(OutFile,
                                        DidPrint,
                                        "Mach-O Files",
                                        LinePrefix);
            }

        case ObjectKind::FatMachO:
            if (Operation::SupportsObjectKind(OpKind, ObjectKind::FatMachO)) {
                PrintItemForSupportList(OutFile,
                                        DidPrint,
                                        "FAT Mach-O Files",
                                        LinePrefix);
            }

        case ObjectKind::DyldSharedCache:
            if (Operation::SupportsObjectKind(OpKind,
                                              ObjectKind::DyldSharedCache))
            {
                PrintItemForSupportList(OutFile,
                                        DidPrint,
                                        "Apple dyld_shared_cache Files",
                                        LinePrefix);
            }

        case ObjectKind::DscImage:
            if (Operation::SupportsObjectKind(OpKind, ObjectKind::DscImage)) {
                PrintItemForSupportList(OutFile,
                                        DidPrint,
                                        "Apple dyld_shared_cache Mach-O Images",
                                        LinePrefix);
            }
    }

    fprintf(OutFile, "\n%s", Suffix);
}

void
Operation::PrintPathOptionHelpMenu(FILE *OutFile,
                                   OperationKind ForKind,
                                   const char *Prefix,
                                   const char *LinePrefix,
                                   const char *Suffix) noexcept
{
    constexpr auto SelectArchString =
        "%s\t--arch <ordinal>,          Select arch (at ordinal) of a FAT "
        "Mach-O File\n";

    constexpr auto SelectDscImageString =
        "%s\t--image <path-or-ordinal>, Select image (at path or ordinal) of "
        "an Apple dyld_shared_cache file\n";

    if (ForKind == OperationKind::None) {
        fprintf(OutFile, "%sPath-Options:\n", Prefix);
        fprintf(OutFile, SelectArchString, LinePrefix);
        fprintf(OutFile, SelectDscImageString, LinePrefix);

        return;
    }

    const auto SupportsFatMachO =
        Operation::SupportsObjectKind(ForKind, ObjectKind::FatMachO);
    const auto SupportsDsc =
        Operation::SupportsObjectKind(ForKind, ObjectKind::DyldSharedCache);

    if (!SupportsFatMachO && !SupportsDsc) {
        return;
    }

    fprintf(OutFile, "%s%sPath Options:\n", Prefix, LinePrefix);
    if (SupportsFatMachO) {
        fprintf(OutFile,
                "%s\t--arch <ordinal>,          Select arch (at ordinal) of a "
                "FAT Mach-O File\n",
                LinePrefix);
    }

    if (SupportsDsc) {
        fprintf(OutFile,
                "%s\t--image <path-or-ordinal>, Select image (at path or "
                "ordinal) of an Apple dyld_shared_cache file\n",
                LinePrefix);
    }

    fprintf(OutFile, "%s", Suffix);
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

            assert(Name != OperationKindGetOptionName(JKind));
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
            "\t%5s--%s,%" PRINTF_LEFTPAD_FMT "s Print this Menu\n",
            "",
            UsageOption.data(),
            static_cast<int>(LongestOptionNameLength - UsageOption.length()),
            "");

    fputs("Operations:\n", OutFile);
    for (auto Iter = Begin;;) {
        const auto &Kind = *Iter;
        const auto &ShortName = OperationKindGetOptionShortName(Kind);

        if (!ShortName.empty()) {
            fprintf(OutFile, "\t-%s, ", ShortName.data());
        } else {
            fprintf(OutFile, "\t%3s", "");
        }

        PrintUtilsRightPadSpaces(OutFile,
                                 static_cast<int>(ShortName.size()),
                                 MaxShortOptionNameLength);

        const auto &OptName = OperationKindGetOptionName(Kind);
        PrintUtilsRightPadSpaces(OutFile,
                                 fprintf(OutFile, "--%s,", OptName.data()),
                                 LongestOptionNameLength + LENGTH_OF("--,"));

        fprintf(OutFile, " %s\n", OperationKindGetDescription(Kind).data());

        PrintObjectKindSupportsList(OutFile, Kind, "\t\t");
        PrintOptionHelpMenu(OutFile, Kind, "", "\t\t");

        ++Iter;
        if (Iter == End) {
            break;
        }

        fputc('\n', stdout);
    }

    PrintPathOptionHelpMenu(OutFile, OperationKind::None);
}
