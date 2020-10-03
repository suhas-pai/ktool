//
//  src/Operations/PrintObjcClassList.cpp
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "ADT/DscImage.h"
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
    PrintUtilsWriteItemAfterFirstForList(OutFile, " - ", DidPrint);
}

static void
PrintClassRoFlags(FILE *OutFile, const MachO::ObjcClassRoFlags &Flags) noexcept
{
    fputc('<', OutFile);

    auto DidPrint = false;
    if (Flags.isRoot()) {
        fputs("Root", OutFile);
        DidPrint = true;
    }

    if (Flags.isMeta()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Meta", OutFile);
    }

    if (Flags.isHidden()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Hidden", OutFile);
    }

    if (Flags.isARC()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("ARC", OutFile);
    }

    if (Flags.isException()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Exception", OutFile);
    }

    if (Flags.hasSwiftInitializer()) {
        PrintFlagSeparator(OutFile, DidPrint);
        fputs("Swift-Initializer", OutFile);
    }

    if (Flags.isFromBundle()) {
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

    if (Flags.forbidsAssociatedObjects()) {
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
            assert(0 && "Unrecognized Sort-Kind");
        case PrintObjcClassListOperation::Options::SortKind::ByName:
            return Lhs.getName().compare(Rhs.getName());
        case PrintObjcClassListOperation::Options::SortKind::ByDylibOrdinal:
            if (Lhs.getDylibOrdinal() < Rhs.getDylibOrdinal()) {
                return -1;
            } else if (Lhs.getDylibOrdinal() == Rhs.getDylibOrdinal()) {
                return 0;
            }

            return 1;
        case PrintObjcClassListOperation::Options::SortKind::ByKind: {
            if (Lhs.isExternal()) {
                if (Rhs.isExternal()) {
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
PrintCategoryList(
    FILE *OutFile,
    const std::vector<MachO::ObjcClassCategoryInfo *> &CategoryList,
    bool Is64Bit) noexcept
{
    switch (CategoryList.size()) {
        case 0:
            return;

        case 1:
            fputs("\t1 Category:\n", OutFile);
            break;

        default:
            fprintf(OutFile,
                    "\t%" PRIuPTR " Categories:\n",
                    CategoryList.size());
            break;
    }

    auto Index = static_cast<uint64_t>(1);
    const auto CategoryListSizeDigitLength =
        PrintUtilsGetIntegerDigitLength(CategoryList.size());

    for (const auto &Category : CategoryList) {
        fprintf(OutFile,
                "\t\tObjc-Class Category %0*" PRIu64 ": ",
                CategoryListSizeDigitLength,
                Index);

        PrintUtilsWriteOffset32Or64(OutFile, Is64Bit, Category->getAddress());
        fprintf(OutFile, " \"%s\"\n", Category->getName().data());

        Index++;
    }
}

static void
PrintClassVerboseInfo(
    FILE *OutFile,
    const MachO::SharedLibraryInfoCollection &SharedLibraryCollection,
    uint64_t LongestLength,
    const MachO::ObjcClassInfo &Node,
    bool IsTree,
    int WrittenOut) noexcept
{
    const auto IsExternal = Node.isExternal();
    const auto IsSwift = Node.isSwift();
    const auto Flags = Node.getFlags();

    if (!IsExternal && !IsSwift && Flags.empty()) {
        return;
    }

    const auto RightPad = static_cast<int>(LongestLength + LENGTH_OF("\"\" -"));
    const auto CharCount = static_cast<int>(RightPad - WrittenOut - 1);

    fputc(' ', OutFile);
    PrintUtilsCharMultTimes(OutFile, '-', CharCount);

    if (IsExternal) {
        fputs("> ", OutFile);
        if (IsTree) {
            fputs("Imported - ", OutFile);
        }

        OperationCommon::PrintDylibOrdinalInfo(OutFile,
                                               SharedLibraryCollection,
                                               Node.getDylibOrdinal(),
                                               true);
    } else {
        fputs("> ", OutFile);
        if (IsSwift) {
            fputs("<Swift> ", OutFile);
        }

        if (!Flags.empty()) {
            PrintClassRoFlags(OutFile, Flags);
        }
    }
}

constexpr static auto TabLength = 8;

static void
PrintObjcClassList(
    const MachO::SharedLibraryInfoCollection &SharedLibraryCollection,
    MachO::ObjcClassInfoCollection &ObjcClassCollection,
    bool Is64Bit,
    const struct PrintObjcClassListOperation::Options &Options) noexcept
{
    if (ObjcClassCollection.empty()) {
        fputs("Provided file has no Objective-C Classes\n", Options.OutFile);
        return;
    }

    const auto End = ObjcClassCollection.end();

    auto LongestLength = LargestIntHelper();
    auto LongestName = LargestIntHelper();

    for (auto Iter = ObjcClassCollection.begin(); Iter != End; Iter++) {
        if (Iter->isNull()) {
            continue;
        }

        const auto IsExternal = Iter->isExternal();
        if (!IsExternal && Iter->getFlags().empty() && !Iter->isSwift()) {
            continue;
        }

        const auto NameLength = Iter->getName().length();
        const auto Length = Iter.getPrintLineLength(TabLength) + NameLength;

        LongestLength = Length;
        LongestName = NameLength;
    }

    if (Options.PrintTree) {
        if (!Options.SortKindList.empty()) {
            ObjcClassCollection.Sort([&](const auto &Lhs,
                                         const auto &Rhs) noexcept
            {
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

            if (Node.isNull()) {
                return false;
            }

            WrittenOut += fprintf(OutFile, "\"%s\"", Node.getName().data());
            if (!Options.Verbose) {
                return true;
            }

            PrintClassVerboseInfo(OutFile,
                                  SharedLibraryCollection,
                                  LongestLength,
                                  Node,
                                  true,
                                  WrittenOut);

            return true;
        };

        ObjcClassCollection.PrintHorizontal(Options.OutFile,
                                            TabLength,
                                            Printer);
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

        const auto ObjcClassListSize = ObjcClassList.size();
        fprintf(Options.OutFile,
                "Provided file has %" PRIuPTR " Objective-C Classes:\n",
                ObjcClassListSize);

        const auto MaxDigitLength =
        PrintUtilsGetIntegerDigitLength(ObjcClassListSize);

        auto I = static_cast<uint64_t>(1);
        for (const auto &Iter : ObjcClassList) {
            const auto &Node = *Iter;
            if (Node.isNull()) {
                I++;
                continue;
            }

            fprintf(Options.OutFile,
                    "Objective-C Class %0*" PRIu64 ": ",
                    MaxDigitLength,
                    I);

            if (Node.isExternal()) {
                PrintUtilsRightPadSpaces(Options.OutFile,
                                         fputs("<imported>", Options.OutFile),
                                         OFFSET_LEN(Is64Bit));
            } else {
                PrintUtilsWriteOffset32Or64(Options.OutFile,
                                            Is64Bit,
                                            Node.getAddress());
            }

            const auto NamePrintLength =
            fprintf(Options.OutFile, " \"%s\"", Node.getName().data());

            PrintClassVerboseInfo(Options.OutFile,
                                  SharedLibraryCollection,
                                  LongestName,
                                  Node,
                                  false,
                                  NamePrintLength - 1);

            fputc('\n', Options.OutFile);
            if (Options.PrintCategories) {
                const auto &CategoryList = Node.getCategoryList();
                PrintCategoryList(Options.OutFile, CategoryList, Is64Bit);
            }

            I++;
        }
    }
}

int
PrintObjcClassListOperation::Run(const ConstDscImageMemoryObject &Object,
                                 const struct Options &Options) noexcept
{
    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    auto SharedLibraryCollectionError =
        MachO::SharedLibraryInfoCollection::Error::None;

    const auto SharedLibraryCollection =
        MachO::SharedLibraryInfoCollection::Open(LoadCmdStorage,
                                                 &SharedLibraryCollectionError);

    auto DyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);

    const auto Map = Object.getMap();
    const auto GetDyldInfoResult =
        OperationCommon::GetDyldInfoCommand(Options.ErrFile,
                                            LoadCmdStorage,
                                            DyldInfo);

    if (GetDyldInfoResult != 0) {
        return GetDyldInfoResult;
    }

    auto BindList = static_cast<const MachO::BindActionList *>(nullptr);
    auto LazyBindList = static_cast<const MachO::LazyBindActionList *>(nullptr);
    auto WeakBindList = static_cast<const MachO::WeakBindActionList *>(nullptr);

    const auto GetBindListsResult =
        OperationCommon::GetBindActionLists(Options.ErrFile,
                                            Object.getDscMap(),
                                            SegmentCollection,
                                            *DyldInfo,
                                            IsBigEndian,
                                            Is64Bit,
                                            BindList,
                                            LazyBindList,
                                            WeakBindList);

    if (GetBindListsResult != 0) {
        return GetBindListsResult;
    }

    const auto DscMap = Object.getDscMap();
    const auto MappingList =
        Object.getDscHeaderV0().getConstMappingInfoList();

    auto DeVirtualizer =
        DscImage::ConstDeVirtualizer(DscMap.getBegin(), MappingList);

    auto Error = DscImage::ObjcClassInfoCollection::Error::None;
    auto CollectionError = MachO::BindActionCollection::Error::None;
    auto ParseError = MachO::BindOpcodeParseError::None;
    auto ObjcClassCollection =
        DscImage::ObjcClassInfoCollection::Open(DscMap,
                                                Map,
                                                SegmentCollection,
                                                DeVirtualizer,
                                                BindList,
                                                LazyBindList,
                                                WeakBindList,
                                                IsBigEndian,
                                                Is64Bit,
                                                &Error,
                                                &ParseError,
                                                &CollectionError);

    auto CategoryCollection = MachO::ObjcClassCategoryCollection();
    if (Options.PrintCategories) {
        CategoryCollection =
            DscImage::ObjcClassCategoryCollection::Open(DscMap,
                                                        SegmentCollection,
                                                        DeVirtualizer,
                                                        &ObjcClassCollection,
                                                        BindList,
                                                        LazyBindList,
                                                        WeakBindList,
                                                        IsBigEndian,
                                                        Is64Bit,
                                                        &Error,
                                                        &ParseError,
                                                        &CollectionError);
    }

    PrintObjcClassList(SharedLibraryCollection,
                       ObjcClassCollection,
                       Is64Bit,
                       Options);

    return 0;
}

int
PrintObjcClassListOperation::Run(const ConstMachOMemoryObject &Object,
                                 const struct Options &Options) noexcept
{
    const auto IsBigEndian = Object.isBigEndian();
    const auto LoadCmdStorage =
        OperationCommon::GetConstLoadCommandStorage(Object, Options.ErrFile);

    if (LoadCmdStorage.hasError()) {
        return 1;
    }

    auto SegmentCollectionError = MachO::SegmentInfoCollection::Error::None;

    const auto Is64Bit = Object.is64Bit();
    const auto SegmentCollection =
        MachO::SegmentInfoCollection::Open(LoadCmdStorage,
                                           Is64Bit,
                                           &SegmentCollectionError);

    auto SharedLibraryCollectionError =
        MachO::SharedLibraryInfoCollection::Error::None;

    const auto SharedLibraryCollection =
        MachO::SharedLibraryInfoCollection::Open(LoadCmdStorage,
                                                 &SharedLibraryCollectionError);

    auto DyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);

    const auto Map = Object.getMap();
    const auto GetDyldInfoResult =
        OperationCommon::GetDyldInfoCommand(Options.ErrFile,
                                            LoadCmdStorage,
                                            DyldInfo);

    if (GetDyldInfoResult != 0) {
        return GetDyldInfoResult;
    }

    auto BindList = static_cast<const MachO::BindActionList *>(nullptr);
    auto LazyBindList = static_cast<const MachO::LazyBindActionList *>(nullptr);
    auto WeakBindList = static_cast<const MachO::WeakBindActionList *>(nullptr);

    const auto GetBindListsResult =
        OperationCommon::GetBindActionLists(Options.ErrFile,
                                            Map,
                                            SegmentCollection,
                                            *DyldInfo,
                                            IsBigEndian,
                                            Is64Bit,
                                            BindList,
                                            LazyBindList,
                                            WeakBindList);

    if (GetBindListsResult != 0) {
        return GetBindListsResult;
    }

    auto DeVirtualizer = MachO::ConstDeVirtualizer(Map, SegmentCollection);
    auto Error = MachO::ObjcClassInfoCollection::Error::None;
    auto CollectionError = MachO::BindActionCollection::Error::None;
    auto ParseError = MachO::BindOpcodeParseError::None;

    auto ObjcClassCollection =
        MachO::ObjcClassInfoCollection::Open(Map,
                                             SegmentCollection,
                                             DeVirtualizer,
                                             BindList,
                                             LazyBindList,
                                             WeakBindList,
                                             IsBigEndian,
                                             Is64Bit,
                                             &Error,
                                             &ParseError,
                                             &CollectionError);

    auto CategoryCollection = MachO::ObjcClassCategoryCollection();
    if (Options.PrintCategories) {
        CategoryCollection =
            MachO::ObjcClassCategoryCollection::Open(Map,
                                                     SegmentCollection,
                                                     DeVirtualizer,
                                                     &ObjcClassCollection,
                                                     BindList,
                                                     LazyBindList,
                                                     WeakBindList,
                                                     IsBigEndian,
                                                     Is64Bit,
                                                     &Error,
                                                     &ParseError,
                                                     &CollectionError);
    }

    PrintObjcClassList(SharedLibraryCollection,
                       ObjcClassCollection,
                       Is64Bit,
                       Options);

    return 0;
}

static inline bool
ListHasSortKind(
    const std::vector<PrintObjcClassListOperation::Options::SortKind> &List,
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
    auto Index = int();
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
        } else if (!Argument.isOption()) {
            break;
        } else {
            fprintf(stderr,
                    "Unrecognized argument for operation %s: %s\n",
                    OperationKindInfo<OpKind>::Name.data(),
                    Argument.getString());
            exit(1);
        }

        Index++;
    }

    if (Options.PrintTree && Options.PrintCategories) {
        fputs("Cannot both print a tree and print categories\n",
              Options.OutFile);
        exit(1);
    }

    if (IndexOut != nullptr) {
        *IndexOut = Index;
    }

    return Options;
}

int PrintObjcClassListOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int
PrintObjcClassListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
            return Run(cast<ObjectKind::MachO>(Object), Options);
        case ObjectKind::DscImage:
            return Run(cast<ObjectKind::DscImage>(Object), Options);
        case ObjectKind::FatMachO:
        case ObjectKind::DyldSharedCache:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}
