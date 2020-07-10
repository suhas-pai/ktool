//
//  src/Operations/PrintObjcClassList.cpp
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "ADT/MachO.h"

#include "Utils/MachOPrinter.h"
#include "Utils/MiscTemplates.h"
#include "Utils/PointerUtils.h"
#include "Utils/PrintUtils.h"

#include "Common.h"
#include "Operation.h"
#include "PrintObjcClassList.h"

PrintObjcClassListOperation::PrintObjcClassListOperation() noexcept
: Operation(OpKind) {}

PrintObjcClassListOperation::PrintObjcClassListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

static inline void PrintFlagSeparator(FILE *OutFile, bool &DidPrint) noexcept {
    if (DidPrint) {
        fputs(" - ", OutFile);
    }

    DidPrint = true;
}

static void
PrintClassRoFlags(FILE *OutFile, const MachO::ObjcClassRoFlags &Flags) noexcept
{
    fputc('<', OutFile);

    auto DidPrint = false;
    if (Flags.IsRoot()) {
        fputs("Root", OutFile);
        DidPrint = true;
    }

    if (Flags.IsMeta()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Meta", OutFile);
    }

    if (Flags.IsHidden()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Hidden", OutFile);
    }

    if (Flags.IsARC()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("ARC", OutFile);
    }

    if (Flags.IsException()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Exception", OutFile);
    }

    if (Flags.hasSwiftInitializer()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Swift-Initializer", OutFile);
    }

    if (Flags.IsFromBundle()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("From Bundle", OutFile);
    }

    if (Flags.hasWeakWithoutARC()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Non-ARC Weak", OutFile);
    }

    if (Flags.hasCxxDestructorOnly()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Has C++ Destructor", OutFile);
    } else if (Flags.hasCxxStructors()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Has C++ Constructor & Destructor", OutFile);
    }

    if (Flags.ForbidsAssociatedObjects()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Forbids Associated-Objects", OutFile);
    }

    fputc('>', OutFile);
}

static int
CompareActionsBySortKind(
    const MachO::ObjcClassInfo &Lhs,
    const MachO::ObjcClassInfo &Rhs,
    const PrintObjcClassListOperation::Options::SortKind SortKind) noexcept
{
    switch (SortKind) {
        case PrintObjcClassListOperation::Options::SortKind::None:
            assert(0 && "Unrecognized Sort-Type");
        case PrintObjcClassListOperation::Options::SortKind::ByName:
            return Lhs.Name.compare(Rhs.Name);
        case PrintObjcClassListOperation::Options::SortKind::ByDylibOrdinal:
            if (Lhs.DylibOrdinal < Rhs.DylibOrdinal) {
                return -1;
            } else if (Lhs.DylibOrdinal == Rhs.DylibOrdinal) {
                return 0;
            }

            return 1;
        case PrintObjcClassListOperation::Options::SortKind::ByKind: {
            if (Lhs.IsExternal) {
                if (Rhs.IsExternal) {
                    return 0;
                }

                return 1;
            }

            return -1;
        }
    }

    return false;
}

static int
CompareObjcClasses(
    const BasicTreeNode &LEntry,
    const BasicTreeNode &REntry,
    const struct PrintObjcClassListOperation::Options &Options) noexcept
{
    const auto &Lhs = reinterpret_cast<const MachO::ObjcClassInfo &>(LEntry);
    const auto &Rhs = reinterpret_cast<const MachO::ObjcClassInfo &>(REntry);

    for (const auto &SortKind : Options.SortKindList) {
        const auto CmpResult = CompareActionsBySortKind(Lhs, Rhs, SortKind);
        if (CmpResult != 0) {
            return (CmpResult < 0);
        }

        continue;
    }

    return false;
};

static void
PrintOffset(FILE *OutFile,
            uint64_t Offset,
            bool IsExternal,
            bool Is64Bit) noexcept
{
    if (IsExternal) {
        PrintUtilsRightPadSpaces(OutFile,
                                 fputs("<imported>", OutFile),
                                 OFFSET_LEN(Is64Bit));
    } else {
        PrintUtilsPrintOffset(OutFile, Offset, Is64Bit);
    }
}

static void
PrintCategoryList(
    FILE *OutFile,
    const std::vector<MachO::ObjcClassCategoryInfo *> &CategoryList,
    bool Is64Bit) noexcept
{
    switch (CategoryList.size()) {
        case 0:
            return;

        case 1:
            fputs("1 Category:\n", OutFile);
            break;

        default:
            fprintf(OutFile, "%" PRIuPTR " Categories\n", CategoryList.size());
            break;
    }

    for (const auto &Category : CategoryList) {
        fputs("\t\t", OutFile);

        PrintOffset(OutFile, Category->Address, false, Is64Bit);
        fprintf(OutFile, " \"%s\"\n", Category->Name.data());
    }
}

constexpr static auto TabLength = 8;

int
PrintObjcClassListOperation::Run(const ConstMachOMemoryObject &Object,
                                 const struct Options &Options) noexcept
{
    const auto IsBigEndian = Object.IsBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.Is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    auto SharedLibraryCollectionError =
        MachO::SharedLibraryInfoCollection::Error::None;

    const auto SharedLibraryCollection =
        MachO::SharedLibraryInfoCollection::Open(LoadCmdStorage,
                                                 &SharedLibraryCollectionError);

    auto BindCollection = MachO::BindActionCollection();
    const auto GetBindActionResult =
        OperationCommon::GetBindActionCollection(Options.ErrFile,
                                                 Object.getMap(),
                                                 LoadCmdStorage,
                                                 SegmentCollection,
                                                 BindCollection,
                                                 Is64Bit);

    if (GetBindActionResult != 0) {
        return GetBindActionResult;
    }

    const auto Map = Object.getMap();

    auto DeVirtualizer = MachO::ConstDeVirtualizer(Map, SegmentCollection);
    auto Error = MachO::ObjcClassInfoTree::Error();

    auto ObjcClassCollection =
        MachO::ObjcClassInfoTree::Open(Map.getBegin(),
                                       SegmentCollection,
                                       DeVirtualizer,
                                       BindCollection,
                                       Is64Bit,
                                       IsBigEndian,
                                       &Error);

    if (Options.PrintCategories) {
        MachO::ObjcClassCategoryCollection::Open(Map.getBegin(),
                                                 SegmentCollection,
                                                 DeVirtualizer,
                                                 BindCollection,
                                                 &ObjcClassCollection,
                                                 Is64Bit,
                                                 IsBigEndian,
                                                 &Error);
    }

    if (ObjcClassCollection.empty()) {
        fputs("Provided file has no Objective-C Classes\n", Options.OutFile);
        return 0;
    }

    auto LongestLength = LargestIntHelper<uint64_t>();

    const auto End = ObjcClassCollection.end();
    for (auto Iter = ObjcClassCollection.begin(); Iter != End; Iter++) {
        if (Iter->IsNull) {
            continue;
        }

        const auto DepthIndex = (Iter.getDepthLevel() - 1);
        const auto Length = (Iter->Name.length() + (TabLength * DepthIndex));

        LongestLength = Length;
    }

    if (Options.PrintTree) {
        if (!Options.SortKindList.empty()) {
            ObjcClassCollection.Sort([&](const auto &Lhs, const auto &Rhs) {
                return CompareObjcClasses(Lhs, Rhs, Options);
            });
        }

        const auto Printer =
            [&](FILE *OutFile,
                int WrittenOut,
                uint64_t DepthLevel,
                const BasicTreeNode &TreeNode) noexcept
        {
            const auto &Node =
                reinterpret_cast<const MachO::ObjcClassInfo &>(TreeNode);

            if (Node.IsNull) {
                return false;
            }

            const auto NamePrintLength =
                fprintf(OutFile, "\"%s\"", Node.Name.data()) +
                ((DepthLevel - 1) * TabLength);

            if (!Options.Verbose) {
                return true;
            }

            const auto RightPad =
                static_cast<int>(LongestLength + LENGTH_OF("\"\""));

            if (Node.IsExternal) {
                PrintUtilsRightPadSpaces(OutFile,
                                         static_cast<int>(NamePrintLength),
                                         RightPad);

                fputs("Imported - ", Options.OutFile);
                OperationCommon::PrintDylibOrdinalInfo(OutFile,
                                                       SharedLibraryCollection,
                                                       Node.DylibOrdinal,
                                                       Options.Verbose);
            } else {
                if (Node.IsSwift) {
                    fputs("<Swift> ", Options.OutFile);
                }

                if (!Node.Flags.empty()) {
                    PrintUtilsRightPadSpaces(OutFile,
                                             static_cast<int>(NamePrintLength),
                                             RightPad);

                    PrintClassRoFlags(Options.OutFile, Node.Flags);
                }
            }

            return true;
        };

        ObjcClassCollection.PrintHorizontal(Options.OutFile, 8, Printer);
    } else {
        auto ObjcClassList = ObjcClassCollection.GetAsList();
        if (!Options.SortKindList.empty()) {
            std::sort(ObjcClassList.begin(),
                      ObjcClassList.end(),
                      [&](const auto &Lhs, const auto &Rhs) noexcept
            {
                return CompareObjcClasses(*Lhs, *Rhs, Options);
            });
        }

        fprintf(Options.OutFile,
                "Provided file has %" PRIuPTR " Objective-C Classes:\n",
                ObjcClassList.size());

        const auto MaxDigitLength =
            PrintUtilsGetIntegerDigitLength(ObjcClassList.size());

        auto I = static_cast<uint64_t>(1);
        for (const auto &Iter : ObjcClassList) {
            const auto &Node =
                reinterpret_cast<const MachO::ObjcClassInfo &>(*Iter);

            if (Node.IsNull) {
                I++;
                continue;
            }

            fprintf(Options.OutFile,
                    "Objective-C Class %0*" PRIu64 ": ",
                    MaxDigitLength,
                    I);

            if (Node.IsExternal) {
                PrintUtilsRightPadSpaces(Options.OutFile,
                                         fputs("<imported>", Options.OutFile),
                                         OFFSET_LEN(Is64Bit));
            } else {
                PrintUtilsPrintOffset(Options.OutFile, Node.Addr, Is64Bit);
            }

            const auto NamePrintLength =
                fprintf(Options.OutFile, " \"%s\"", Node.Name.data());

            if (Node.IsExternal) {
                const auto RightPad =
                    static_cast<int>(LongestLength + LENGTH_OF(" \"\""));

                PrintUtilsRightPadSpaces(Options.OutFile,
                                         NamePrintLength,
                                         RightPad);

                OperationCommon::PrintDylibOrdinalInfo(Options.OutFile,
                                                       SharedLibraryCollection,
                                                       Node.DylibOrdinal,
                                                       Options.Verbose);
            }

            fputc('\n', Options.OutFile);
            if (Options.PrintCategories) {
                PrintCategoryList(Options.OutFile, Node.CategoryList, Is64Bit);
            }

            I++;
        }
    }

    return 0;
}

static inline bool
ListHasSortKind(
    std::vector<PrintObjcClassListOperation::Options::SortKind> &List,
    const PrintObjcClassListOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(const char *Opt,
            struct PrintObjcClassListOperation::Options &Options,
            PrintObjcClassListOperation::Options::SortKind SortKind) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Opt);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

struct PrintObjcClassListOperation::Options
PrintObjcClassListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                              int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--include-categories") == 0) {
            Options.PrintCategories = true;
        } else if (strcmp(Argument, "--sort-by-name") == 0) {
            AddSortKind(Argument, Options, Options::SortKind::ByName);
        } else if (strcmp(Argument, "--sort-by-dylib-ordinal") == 0) {
            AddSortKind(Argument, Options, Options::SortKind::ByDylibOrdinal);
        } else if (strcmp(Argument, "--sort-by-kind") == 0) {
            AddSortKind(Argument, Options, Options::SortKind::ByKind);
        } else if (strcmp(Argument, "--tree") == 0) {
            Options.PrintTree = true;
        } else if (!Argument.IsOption()) {
            if (IndexOut != nullptr) {
                *IndexOut = Argv.indexOf(Argument);
            }

            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }
    }

    if (Options.PrintTree && Options.PrintCategories) {
        fputs("Cannot both print a tree and print categories\n",
              Options.OutFile);
        exit(1);
    }

    return Options;
}

void
PrintObjcClassListOperation::ParseOptions(const ArgvArray &Argv,
                                          int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int
PrintObjcClassListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::FatMachO:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}
