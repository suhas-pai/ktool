/*
 * Operations/PrintObjcClassList.cpp
 * © suhas pai
 */

#include "ADT/Maximizer.h"
#include "DyldSharedCache/DeVirtualizer.h"

#include "MachO/BindInfo.h"
#include "MachO/DeVirtualizer.h"
#include "MachO/LibraryList.h"
#include "MachO/ObjcInfo.h"

#include "Operations/Common.h"
#include "Operations/PrintObjcClassList.h"

#include "ObjC/Info.h"
#include "Utils/Print.h"

namespace Operations {
    PrintObjcClassList::PrintObjcClassList(
        FILE *const OutFile,
        const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options) {}

    bool
    PrintObjcClassList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintObjcClassList::supportsObjectKind()");
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
                return true;
            case Objects::Kind::DyldSharedCache:
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintObjcClassList::supportsObjectKind()");
    }

    static inline
    void PrintFlagSeparator(FILE *const OutFile, bool &DidPrint) noexcept {
        Utils::PrintOnlyAfterFirst(OutFile, " - ", DidPrint);
    }

    static void
    PrintClassRoFlags(FILE *const OutFile,
                      const ObjC::ClassRoFlags &Flags) noexcept
    {
        if (Flags.empty()) {
            return;
        }

        fputc('<', OutFile);

        auto DidPrint = false;
        if (Flags.root()) {
            fputs("Root", OutFile);
            DidPrint = true;
        }

        if (Flags.meta()) {
            PrintFlagSeparator(OutFile, DidPrint);
            fputs("Meta", OutFile);
        }

        if (Flags.hidden()) {
            PrintFlagSeparator(OutFile, DidPrint);
            fputs("Hidden", OutFile);
        }

        if (Flags.arc()) {
            PrintFlagSeparator(OutFile, DidPrint);
            fputs("ARC", OutFile);
        }

        if (Flags.exception()) {
            PrintFlagSeparator(OutFile, DidPrint);
            fputs("Exception", OutFile);
        }

        if (Flags.hasSwiftInitializer()) {
            PrintFlagSeparator(OutFile, DidPrint);
            fputs("Swift-Initializer", OutFile);
        }

        if (Flags.fromBundle()) {
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
        const PrintObjcClassList::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            case PrintObjcClassList::Options::SortKind::None:
                assert(false &&
                       "Unrecognized PrintObjcClassList::Options::SortKind");
            case PrintObjcClassList::Options::SortKind::ByName:
                return Lhs.name().compare(Rhs.name());
            case PrintObjcClassList::Options::SortKind::ByDylibOrdinal:
                if (Lhs.dylibOrdinal() < Rhs.dylibOrdinal()) {
                    return -1;
                } else if (Lhs.dylibOrdinal() == Rhs.dylibOrdinal()) {
                    return 0;
                }

                return 1;
            case PrintObjcClassList::Options::SortKind::ByKind: {
                if (Lhs.external()) {
                    if (Rhs.external()) {
                        return 0;
                    }

                    return 1;
                }

                return -1;
            }
        }

        return false;
    }

    [[maybe_unused]] static int
    CompareObjcClasses(const ADT::TreeNode &LEntry,
                       const ADT::TreeNode &REntry,
                       const PrintObjcClassList::Options &Options) noexcept
    {
        const auto &Lhs =
            reinterpret_cast<const MachO::ObjcClassInfo &>(LEntry);
        const auto &Rhs =
            reinterpret_cast<const MachO::ObjcClassInfo &>(REntry);

        for (const auto &SortKind : Options.SortKindList) {
            const auto CmpResult = CompareActionsBySortKind(Lhs, Rhs, SortKind);
            if (CmpResult != 0) {
                return CmpResult < 0;
            }

            continue;
        }

        return 0;
    };

    static void
    PrintCategoryList(
        FILE *const OutFile,
        const std::vector<MachO::ObjcClassCategoryInfo *> &CategoryList,
        const bool Is64Bit) noexcept
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

        auto Index = uint64_t(1);
        const auto CategoryListSizeDigitLength =
            Utils::GetIntegerDigitCount(CategoryList.size());

        for (const auto &Category : CategoryList) {
            fprintf(OutFile,
                    "\t\tObjc-Class Category %" ZEROPAD_FMT PRIu64 ": ",
                    ZEROPAD_FMT_ARGS(CategoryListSizeDigitLength),
                    Index);

            Utils::PrintAddress(OutFile, Category->address(), Is64Bit);
            fprintf(OutFile, " \"%s\"\n", Category->name().data());

            Index++;
        }
    }

    static void
    PrintClassVerboseInfo(FILE *const OutFile,
                          const MachO::LibraryList &LibraryList,
                          const uint64_t LongestLength,
                          const MachO::ObjcClassInfo &Node,
                          const bool IsTree,
                          const int WrittenOut) noexcept
    {
        const auto IsExternal = Node.external();
        const auto IsSwift = Node.isSwift();
        const auto Flags = Node.flags();

        if (!IsExternal && !IsSwift && Flags.empty()) {
            return;
        }

        const auto RightPad =
            static_cast<int>(LongestLength + STR_LENGTH("\"\" -"));

        const auto CharCount = static_cast<uint64_t>(RightPad - WrittenOut - 1);

        fputc(' ', OutFile);
        Utils::PrintMultTimes(OutFile, "-", CharCount);

        if (IsExternal) {
            fputs("> ", OutFile);
            if (IsTree) {
                fputs("Imported - ", OutFile);
            }

            Operations::PrintDylibOrdinalInfo(OutFile,
                                              Node.dylibOrdinal(),
                                              LibraryList,
                                              /*PrintPath=*/true);
        } else {
            fputs("> ", OutFile);
            if (IsSwift) {
                fputs("<Swift> ", OutFile);
            }

            PrintClassRoFlags(OutFile, Flags);
        }
    }

    static void
    PrintObjcClassInfoList(
        FILE *const OutFile,
        const MachO::LibraryList &LibraryList,
        MachO::ObjcClassInfoList &ObjcClassCollection,
        const bool Is64Bit,
        const struct PrintObjcClassList::Options &Options) noexcept
    {
        if (ObjcClassCollection.empty()) {
            fputs("Provided file has no Objective-C Classes\n", OutFile);
            return;
        }

        auto LongestLength = ADT::Maximizer<uint64_t>();
        auto LongestName = ADT::Maximizer<uint64_t>();

        const auto DFS = ObjcClassCollection.dfs<MachO::ObjcClassInfo>();
        for (auto Iter = DFS.begin(); Iter != DFS.end(); Iter++) {
            const auto &Class = *Iter;
            if (Class.null()) {
                continue;
            }

            const auto IsExternal = Class.external();
            if (!IsExternal && Class.flags().empty() && !Class.isSwift()) {
                continue;
            }

            const auto NameLength = Class.name().length();
            const auto Length =
                Iter.printLineLength(Options.TabLength) + NameLength;

            LongestLength.set(Length);
            LongestName.set(NameLength);
        }

        if (Options.PrintTree) {
            if (!Options.SortKindList.empty()) {
                ObjcClassCollection.sort([&](const auto &Lhs,
                                             const auto &Rhs) noexcept
                {
                    return CompareObjcClasses(Lhs, Rhs, Options);
                });
            }

            const auto Printer =
                [&](FILE *OutFile,
                    int WrittenOut,
                    [[maybe_unused]] const uint64_t DepthLevel,
                    const ADT::TreeNode &TreeNode) noexcept
            {
                const auto &Node =
                    reinterpret_cast<const MachO::ObjcClassInfo &>(TreeNode);

                if (Node.null()) {
                    return false;
                }

                WrittenOut += fprintf(OutFile, "\"%s\"", Node.name().data());
                if (!Options.Verbose) {
                    return true;
                }

                PrintClassVerboseInfo(OutFile,
                                      LibraryList,
                                      LongestLength.value(),
                                      Node,
                                      true,
                                      WrittenOut);

                return true;
            };

            ObjcClassCollection.printHorizontal(OutFile,
                                                Options.TabLength,
                                                Printer);
        } else {
            auto ObjcClassList = ObjcClassCollection.getAsList();
            if (!Options.SortKindList.empty()) {
                std::sort(ObjcClassList.begin(),
                          ObjcClassList.end(),
                          [&](const auto &Lhs, const auto &Rhs) noexcept
                {
                    return CompareObjcClasses(*Lhs, *Rhs, Options);
                });
            }

            const auto ObjcClassListSize = ObjcClassList.size();
            fprintf(OutFile,
                    "Provided file has %" PRIuPTR " Objective-C Classes:\n",
                    ObjcClassListSize);

            const auto MaxDigitLength =
                Utils::GetIntegerDigitCount(ObjcClassListSize);

            auto I = uint64_t(1);
            for (const auto &Iter : ObjcClassList) {
                const auto &Node = Iter;
                if (Node->null()) {
                    I++;
                    continue;
                }

                fprintf(OutFile,
                        "Objective-C Class %" ZEROPAD_FMT PRIu64 ": ",
                        ZEROPAD_FMT_ARGS(MaxDigitLength),
                        I);

                if (Node->external()) {
                    Utils::RightPadSpaces(OutFile,
                                          fputs("<imported>", OutFile),
                                          Is64Bit ?
                                            Utils::Address64Length :
                                            Utils::Address32Length);
                } else {
                    Utils::PrintAddress(OutFile, Node->address(), Is64Bit);
                }

                const auto NamePrintLength =
                    fprintf(OutFile, " \"%s\"", Node->name().data());

                PrintClassVerboseInfo(OutFile,
                                      LibraryList,
                                      LongestName.value(),
                                      *Node,
                                      false,
                                      NamePrintLength - 1);

                fputc('\n', OutFile);
                if (Options.PrintCategories) {
                    const auto &CategoryList = Node->categoryList();
                    PrintCategoryList(OutFile, CategoryList, Is64Bit);
                }

                I++;
            }
        }
    }

    static void
    PrintBindParseError(const MachO::BindOpcodeParseError ParseError) noexcept {
        switch (ParseError) {
            case MachO::BindOpcodeParseError::None:
                break;
            case MachO::BindOpcodeParseError::InvalidLeb128:
                fputs("Encountered invalid uleb128 when parsing bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::InvalidSegmentIndex:
                fputs("Bind-Opcodes set segment-index to an invalid number\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::InvalidString:
                fputs("Encountered invalid string in bind-opcodes\n", stderr);
                break;
            case MachO::BindOpcodeParseError::NotEnoughThreadedBinds:
                fputs("Not enough threaded-binds in bind-opcodes\n", stderr);
                break;
            case MachO::BindOpcodeParseError::TooManyThreadedBinds:
                fputs("Too many threaded-binds in bind-opcodes\n", stderr);
                break;
            case MachO::BindOpcodeParseError::InvalidThreadOrdinal:
                fputs("Encountered invalid thread-ordinal in "
                      "bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::EmptySymbol:
                fputs("Encountered invalid thread-ordinal in "
                      "bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::IllegalBindOpcode:
                fputs("Encountered invalid bind-opcode when parsing\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::OutOfBoundsSegmentAddr:
                fputs("Got out-of-bounds segment-address in bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::UnrecognizedBindWriteKind:
                fputs("Encountered unknown write-kind in bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::UnrecognizedBindOpcode:
                fputs("Encountered unknown bind-opcode when parsing\n", stderr);
                break;
            case MachO::BindOpcodeParseError::UnrecognizedBindSubOpcode:
                fputs("Encountered unknown bind sub-opcode when parsing\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::
                UnrecognizedSpecialDylibOrdinal:
                fputs("Encountered unknown specialty dylib-ordinal in "
                      "bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::NoDylibOrdinal:
                fputs("No dylib-ordinal found when parsing bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::NoSegmentIndex:
                fputs("No segment-index found when parsing bind-opcodes\n",
                      stderr);
                break;
            case MachO::BindOpcodeParseError::NoWriteKind:
                fputs("No write-type found when parsing bind-opcodes\n",
                      stderr);
                break;
        }
    }

    auto
    PrintObjcClassList::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);

        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        auto SegmentList = MachO::SegmentList();
        auto LibraryList = MachO::LibraryList();

        auto BindRange = ADT::Range();
        auto LazyBindRange = ADT::Range();
        auto WeakBindRange = ADT::Range();

        for (const auto &LC : MachO.loadCommandsMap()) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                LibraryList.addLibrary(
                    cast<MachO::DylibCommand>(LC, IsBigEndian), IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment =
                        MachO::dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        MachO::dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfo =
                    MachO::dyn_cast<MachO::DyldInfoCommand>(&LC, IsBigEndian))
            {
                BindRange = DyldInfo->bindRange(IsBigEndian);
                LazyBindRange = DyldInfo->lazyBindRange(IsBigEndian);
                WeakBindRange = DyldInfo->weakBindRange(IsBigEndian);
            }
        }

        auto BindActionInfoList = MachO::BindActionList::UnorderedMap();
        auto ParseError = MachO::BindOpcodeParseError::None;

        if (MachO.map().range().contains(BindRange)) {
            const auto BindList =
                MachO::BindActionList(MachO.map(),
                                      BindRange,
                                      SegmentList,
                                      Is64Bit);

            ParseError =
                BindList.getAsUnorderedMap(SegmentList, BindActionInfoList);

            PrintBindParseError(ParseError);
        }

        if (MachO.map().range().contains(LazyBindRange)) {
            const auto LazyBindList =
                MachO::LazyBindActionList(MachO.map(),
                                          LazyBindRange,
                                          SegmentList,
                                          Is64Bit);

            ParseError =
                LazyBindList.getAsUnorderedMap(SegmentList, BindActionInfoList);

            PrintBindParseError(ParseError);
        }

        if (MachO.map().range().contains(WeakBindRange)) {
            const auto WeakBindList =
                MachO::WeakBindActionList(MachO.map(),
                                          WeakBindRange,
                                          SegmentList,
                                          Is64Bit);

            ParseError =
                WeakBindList.getAsUnorderedMap(SegmentList, BindActionInfoList);

            PrintBindParseError(ParseError);
        }

        const auto DeVirtualizer =
            MachO::DeVirtualizer(MachO.map(), SegmentList);

        auto ObjcClassInfoList = MachO::ObjcClassInfoList();
        auto Error =
            ObjcClassInfoList.Parse(DeVirtualizer,
                                    SegmentList,
                                    BindActionInfoList,
                                    IsBigEndian,
                                    Is64Bit);

        switch (Error) {
            case MachO::ObjcParse::Error::None:
                break;
            case MachO::ObjcParse::Error::NoObjcData:
                return Result.set(RunError::NoObjcData);
            case MachO::ObjcParse::Error::UnalignedSection:
                return Result.set(RunError::UnalignedSection);
            case MachO::ObjcParse::Error::DataOutOfBounds:
                return Result.set(RunError::ObjcDataOutOfBounds);
        }

        if (Opt.PrintCategories) {
            auto CategoryInfoList = MachO::ObjcClassCategoryInfoList();
            Error =
                CategoryInfoList.CollectFrom(MachO.map(),
                                             DeVirtualizer,
                                             SegmentList,
                                             BindActionInfoList,
                                             &ObjcClassInfoList,
                                             IsBigEndian,
                                             Is64Bit);

            switch (Error) {
                case MachO::ObjcParse::Error::None:
                case MachO::ObjcParse::Error::NoObjcData:
                case MachO::ObjcParse::Error::UnalignedSection:
                case MachO::ObjcParse::Error::DataOutOfBounds:
                    break;
            }
        }

        PrintObjcClassInfoList(OutFile,
                               LibraryList,
                               ObjcClassInfoList,
                               Is64Bit,
                               Opt);

        return Result.set(RunError::None);
    }

    auto
    PrintObjcClassList::run(const Objects::DscImage &Image) const noexcept
        -> RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);

        const auto IsBigEndian = Image.isBigEndian();
        const auto Is64Bit = Image.is64Bit();

        auto SegmentList = MachO::SegmentList();
        auto LibraryList = MachO::LibraryList();

        auto BindRange = ADT::Range();
        auto LazyBindRange = ADT::Range();
        auto WeakBindRange = ADT::Range();

        for (const auto &LC : Image.loadCommandsMap()) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                LibraryList.addLibrary(
                    cast<MachO::DylibCommand>(LC, IsBigEndian), IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment =
                        MachO::dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        MachO::dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.addSegment(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfo =
                    MachO::dyn_cast<MachO::DyldInfoCommand>(&LC, IsBigEndian))
            {
                BindRange = DyldInfo->bindRange(IsBigEndian);
                LazyBindRange = DyldInfo->lazyBindRange(IsBigEndian);
                WeakBindRange = DyldInfo->weakBindRange(IsBigEndian);
            }
        }

        auto BindActionInfoList = MachO::BindActionList::UnorderedMap();
        auto ParseError = MachO::BindOpcodeParseError::None;

        const auto Map = Image.dscMap();
        if (Map.range().contains(BindRange)) {
            const auto BindList =
                MachO::BindActionList(Map,
                                      BindRange,
                                      SegmentList,
                                      Is64Bit);

            ParseError =
                BindList.getAsUnorderedMap(SegmentList, BindActionInfoList);

            PrintBindParseError(ParseError);
        }

        if (Map.range().contains(LazyBindRange)) {
            const auto LazyBindList =
                MachO::LazyBindActionList(Map,
                                          LazyBindRange,
                                          SegmentList,
                                          Is64Bit);

            ParseError =
                LazyBindList.getAsUnorderedMap(SegmentList, BindActionInfoList);

            PrintBindParseError(ParseError);
        }

        if (Map.range().contains(WeakBindRange)) {
            const auto WeakBindList =
                MachO::WeakBindActionList(Map,
                                          WeakBindRange,
                                          SegmentList,
                                          Is64Bit);

            ParseError =
                WeakBindList.getAsUnorderedMap(SegmentList, BindActionInfoList);

            PrintBindParseError(ParseError);
        }

        const auto DeVirtualizer = DyldSharedCache::DeVirtualizer(Image.dsc());

        auto ObjcClassInfoList = MachO::ObjcClassInfoList();
        auto Error =
            ObjcClassInfoList.Parse(DeVirtualizer,
                                    SegmentList,
                                    BindActionInfoList,
                                    IsBigEndian,
                                    Is64Bit);

        switch (Error) {
            case MachO::ObjcParse::Error::None:
                break;
            case MachO::ObjcParse::Error::NoObjcData:
                return Result.set(RunError::NoObjcData);
            case MachO::ObjcParse::Error::UnalignedSection:
                return Result.set(RunError::UnalignedSection);
            case MachO::ObjcParse::Error::DataOutOfBounds:
                return Result.set(RunError::ObjcDataOutOfBounds);
        }

        if (Opt.PrintCategories) {
            auto CategoryInfoList = MachO::ObjcClassCategoryInfoList();
            Error =
                CategoryInfoList.CollectFrom(Map,
                                             DeVirtualizer,
                                             SegmentList,
                                             BindActionInfoList,
                                             &ObjcClassInfoList,
                                             IsBigEndian,
                                             Is64Bit);

            switch (Error) {
                case MachO::ObjcParse::Error::None:
                case MachO::ObjcParse::Error::NoObjcData:
                case MachO::ObjcParse::Error::UnalignedSection:
                case MachO::ObjcParse::Error::DataOutOfBounds:
                    break;
            }
        }

        PrintObjcClassInfoList(OutFile,
                               LibraryList,
                               ObjcClassInfoList,
                               Is64Bit,
                               Opt);

        return Result.set(RunError::None);
    }

    auto
    PrintObjcClassList::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintObjcClassList::run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::DscImage:
                return run(static_cast<const Objects::DscImage &>(Base));
            case Objects::Kind::FatMachO:
            case Objects::Kind::DyldSharedCache:
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintObjcClassList::run()");
    }
}
