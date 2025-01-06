/*
 * Operations/PrintObjcClassList.cpp
 * © suhas pai
 */

#include "ADT/Maximizer.h"
#include "DyldSharedCache/DeVirtualizer.h"

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
    : Base(Operations::Kind::PrintObjcClassList), OutFile(OutFile),
      Opt(Options) {}

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
    auto PrintFlagSeparator(FILE *const OutFile, bool &DidPrint) noexcept {
        Utils::PrintOnlyAfterFirst(OutFile, " - ", DidPrint);
    }

    static void
    PrintClassRoFlags(FILE *const OutFile,
                      const ObjC::ClassRoFlags &Flags) noexcept
    {
        if (Flags.empty()) {
            return;
        }

        std::print(OutFile, "<");

        auto DidPrint = false;
        if (Flags.root()) {
            std::print(OutFile, "Root");
            DidPrint = true;
        }

        if (Flags.meta()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Meta");
        }

        if (Flags.hidden()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Hidden");
        }

        if (Flags.arc()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "ARC");
        }

        if (Flags.exception()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Exception");
        }

        if (Flags.hasSwiftInitializer()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Swift-Initializer");
        }

        if (Flags.fromBundle()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "From Bundle");
        }

        if (Flags.hasWeakWithoutARC()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Non-ARC Weak");
        }

        if (Flags.hasCxxDestructorOnly()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Has C++ Destructor");
        } else if (Flags.hasCxxStructors()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Has C++ Constructor & Destructor");
        }

        if (Flags.forbidsAssociatedObjects()) {
            PrintFlagSeparator(OutFile, DidPrint);
            std::print(OutFile, "Forbids Associated-Objects");
        }

        std::print(OutFile, ">");
    }

    static auto
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
                return Lhs.name() <=> Rhs.name();
            case PrintObjcClassList::Options::SortKind::ByDylibOrdinal:
                return Lhs.dylibOrdinal() <=> Rhs.dylibOrdinal();
            case PrintObjcClassList::Options::SortKind::ByKind: {
                if (Lhs.external()) {
                    if (Rhs.external()) {
                        return std::strong_ordering::equivalent;
                    }

                    return std::strong_ordering::greater;
                }

                return std::strong_ordering::less;
            }
        }

        VERIFY_NOT_REACHED();
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
        const std::vector<MachO::ObjcClassCategoryInfo *> CategoryList,
        const bool Is64Bit) noexcept
    {
        switch (CategoryList.size()) {
            case 0:
                return;
            case 1:
                std::println(OutFile, "\t1 Category:");
                break;
            default:
                std::println(OutFile, "\t{} Categories:", CategoryList.size());
                break;
        }

        auto Index = static_cast<uint64_t>(1);
        const auto CategoryListSizeDigitLength =
            Utils::GetIntegerDigitCount(CategoryList.size());

        for (const auto &Category : CategoryList) {
            std::println(OutFile,
                         "\t\tObjc-Class Category {:>{}}: {}\"{}\"",
                         Index,
                         CategoryListSizeDigitLength,
                         Utils::CustomAddress(Category->address(), Is64Bit),
                         Category->name());

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

        std::print(OutFile, " ");
        Utils::PrintMultTimes(OutFile, "-", CharCount);

        if (IsExternal) {
            std::print(OutFile, "> ");
            if (IsTree) {
                std::print(OutFile, "Imported - ");
            }

            Operations::PrintDylibOrdinalInfo(OutFile,
                                              Node.dylibOrdinal(),
                                              LibraryList,
                                              /*PrintPath=*/true);
        } else {
            std::print(OutFile, "> ");
            if (IsSwift) {
                std::print(OutFile, "<Swift> ");
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
            std::println(OutFile, "Provided file has no Objective-C Classes");
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

                std::print(OutFile, "\"{}\"", Node.name());
                WrittenOut += STR_LENGTH("\"\"") + Node.name().length();

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
                std::ranges::sort(ObjcClassList,
                                  [&](const auto Lhs, const auto Rhs) noexcept {
                                    return CompareObjcClasses(*Lhs,
                                                              *Rhs,
                                                              Options);
                                  });
            }

            const auto ObjcClassListSize = ObjcClassList.size();
            const auto MaxDigitLength =
                Utils::GetIntegerDigitCount(ObjcClassListSize);

            std::println(OutFile,
                         "Provided file has {} Objective-C Classes:",
                         Utils::FormattedNumber(ObjcClassListSize));

            auto I = static_cast<uint64_t>(1);
            for (const auto &Iter : ObjcClassList) {
                const auto &Node = Iter;
                if (Node->null()) {
                    I++;
                    continue;
                }

                std::print(OutFile,
                           "Objective-C Class {:0{}}: ",
                           I,
                           MaxDigitLength);

                if (Node->external()) {
                    std::print(OutFile,
                               "{:<{}}",
                               "<imported>",
                               Utils::AddressLength(Is64Bit));
                } else {
                    std::print(OutFile,
                               "{}",
                               Utils::CustomAddress(Node->address(), Is64Bit));
                }

                std::print(OutFile, " \"{}\"", Node->name());
                const auto NamePrintLength =
                    STR_LENGTH(" \"\"") + Node->name().length();

                PrintClassVerboseInfo(OutFile,
                                      LibraryList,
                                      LongestName.value(),
                                      *Node,
                                      false,
                                      static_cast<int>(
                                        NamePrintLength - 1));

                std::println(OutFile, "");
                if (Options.PrintCategories) {
                    PrintCategoryList(OutFile, Node->categoryList(), Is64Bit);
                }

                I++;
            }
        }
    }

    auto
    PrintObjcClassList::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        const auto OutFile = this->OutFile;
        const auto &Opt = this->Opt;

        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        auto SegmentList = MachO::SegmentList();
        auto LibraryList = MachO::LibraryList();
        auto DyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);

        for (const auto &LC : MachO.loadCommandsMap()) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                LibraryList.add(cast<MachO::DylibCommand>(LC, IsBigEndian),
                                IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment =
                        dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfoCmd =
                    dyn_cast<MachO::DyldInfoCommand>(&LC, IsBigEndian))
            {
                DyldInfo = DyldInfoCmd;
            }
        }

        const auto ObjcSectionInfoOpt =
            MachO::FindObjcClassListOrRefsSection(SegmentList);

        if (!ObjcSectionInfoOpt.has_value()) {
            return RunResult(RunResult::Error::NoObjcData);
        }

        const auto &ObjcSectionInfo = ObjcSectionInfoOpt.value();
        const auto DeVirtualizer =
            MachO::DeVirtualizer(MachO.map(), SegmentList);

        const auto AddrResolverOpt =
            ADT::AddressResolver::FromLoadCommands(DeVirtualizer.map(),
                                                   MachO.header(),
                                                   DyldInfo,
                                                   /*ChainedFixups=*/nullptr,
                                                   SegmentList);

        if (!AddrResolverOpt.has_value()) {
            const auto AddrResolverError = AddrResolverOpt.error();
            if (std::holds_alternative<ADT::AddressResolver::BindParseError>(
                    AddrResolverError))
            {
                auto &[BindKind, BindResult] =
                    std::get<ADT::AddressResolver::BindParseError>(
                        AddrResolverError);

                return RunResult(BindKind, std::move(BindResult));
            }

            if (std::holds_alternative<ADT::AddressResolver::RebaseParseError>(
                    AddrResolverError))
            {
                auto &RebaseResult =
                    std::get<ADT::AddressResolver::RebaseParseError>(
                        AddrResolverError);

                return RunResult(std::move(RebaseResult));
            }

            assert(0 && "Expected a recognizable error");
        }

        const auto &AddrResolver = AddrResolverOpt.value();

        auto ObjcClassInfoList = MachO::ObjcClassInfoList();
        auto Error =
            ObjcClassInfoList.Parse(DeVirtualizer,
                                    AddrResolver,
                                    ObjcSectionInfo,
                                    SegmentList,
                                    IsBigEndian,
                                    Is64Bit);

        switch (Error) {
            case MachO::ObjcParse::Error::None:
                break;
            case MachO::ObjcParse::Error::NoObjcData:
                return RunResult(RunResult::Error::NoObjcData);
            case MachO::ObjcParse::Error::UnalignedSection:
                return RunResult(RunResult::Error::UnalignedSection);
            case MachO::ObjcParse::Error::DataOutOfBounds:
                return RunResult(RunResult::Error::ObjcDataOutOfBounds);
        }

        if (Opt.PrintCategories) {
            auto CategoryInfoList = MachO::ObjcClassCategoryInfoList();
            Error =
                CategoryInfoList.CollectFrom(MachO.map(),
                                             DeVirtualizer,
                                             AddrResolver,
                                             SegmentList,
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

        return RunResult(RunResult::Error::None);
    }

    auto
    PrintObjcClassList::run(const Objects::DscImage &Image) const noexcept
        -> RunResult
    {
        const auto IsBigEndian = Image.isBigEndian();
        const auto Is64Bit = Image.is64Bit();

        auto SegmentList = MachO::SegmentList();
        auto LibraryList = MachO::LibraryList();

        auto DyldInfo = static_cast<const MachO::DyldInfoCommand *>(nullptr);
        auto ChainedFixups =
            static_cast<const MachO::LinkeditDataCommand *>(nullptr);

        for (const auto &LC : Image.loadCommandsMap()) {
            if (LC.isSharedLibrary(IsBigEndian)) {
                LibraryList.add(cast<MachO::DylibCommand>(LC, IsBigEndian),
                                IsBigEndian);
                continue;
            }

            using Kind = MachO::LoadCommandKind;
            if (Is64Bit) {
                if (const auto Segment =
                        dyn_cast<Kind::Segment64>(&LC, IsBigEndian))
                {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            } else {
                if (const auto Segment =
                        dyn_cast<Kind::Segment>(&LC, IsBigEndian))
                {
                    SegmentList.add(*Segment, IsBigEndian);
                    continue;
                }
            }

            if (const auto DyldInfoCmd =
                    dyn_cast<MachO::DyldInfoCommand>(&LC, IsBigEndian))
            {
                DyldInfo = DyldInfoCmd;
                continue;
            }

            if (const auto ChainedFixupsCmd =
                    dyn_cast<MachO::LoadCommandKind::DyldChainedFixups>(
                        &LC, IsBigEndian))
            {
                ChainedFixups = ChainedFixupsCmd;
                continue;
            }
        }

        const auto Map = Image.dscMap();
        const auto ObjcSectionInfoOpt =
            MachO::FindObjcClassListOrRefsSection(SegmentList);

        if (!ObjcSectionInfoOpt.has_value()) {
            return RunResult(RunResult::Error::NoObjcData);
        }

        auto SlideInfoHeader =
            static_cast<::DyldSharedCache::SlideInfoBase *>(nullptr);

        const auto &Dsc = Image.dsc();
        const auto DeVirtualizer = DyldSharedCache::DeVirtualizer(Dsc);
        const auto SlideInfoHeaderOrRangeOpt = Dsc.slideInfoHeaderOrFileRange();

        if (SlideInfoHeaderOrRangeOpt.has_value()) {
            const auto &SlideInfoHeaderOrRange =
                SlideInfoHeaderOrRangeOpt.value();

            if (const auto SlideInfoHeaderRange =
                    std::get_if<ADT::Range>(&SlideInfoHeaderOrRange))
            {
                SlideInfoHeader =
                    Dsc.map().get<::DyldSharedCache::SlideInfoBase>(
                        SlideInfoHeaderRange->front());
            } else {
                SlideInfoHeader =
                    std::get<::DyldSharedCache::SlideInfoBase *>(
                        SlideInfoHeaderOrRange);
            }
        }

        const auto &ObjcSectionInfo = ObjcSectionInfoOpt.value();
        const auto AddrResolverOpt =
            ADT::AddressResolver::ForDscImage(DeVirtualizer,
                                              SlideInfoHeader,
                                              Image,
                                              DyldInfo,
                                              ChainedFixups,
                                              SegmentList);

        if (!AddrResolverOpt.has_value()) {
            const auto AddrResolverError = AddrResolverOpt.error();
            if (std::holds_alternative<ADT::AddressResolver::BindParseError>(
                    AddrResolverError))
            {
                auto &[BindKind, BindResult] =
                    std::get<ADT::AddressResolver::BindParseError>(
                        AddrResolverError);

                return RunResult(BindKind, std::move(BindResult));
            }

            if (std::holds_alternative<ADT::AddressResolver::RebaseParseError>(
                    AddrResolverError))
            {
                auto &RebaseResult =
                    std::get<ADT::AddressResolver::RebaseParseError>(
                        AddrResolverError);

                return RunResult(std::move(RebaseResult));
            }

            if (std::holds_alternative<ADT::AddressResolver::PatchParseError>(
                    AddrResolverError))
            {
                auto &ParseResult =
                    std::get<ADT::AddressResolver::PatchParseError>(
                        AddrResolverError);

                return RunResult(std::move(ParseResult));
            }

            assert(0 && "Expected a recognizable error");
        }

        const auto &AddrResolver = AddrResolverOpt.value();

        auto ObjcClassInfoList = MachO::ObjcClassInfoList();
        auto Error =
            ObjcClassInfoList.Parse(DeVirtualizer,
                                    AddrResolver,
                                    ObjcSectionInfo,
                                    SegmentList,
                                    IsBigEndian,
                                    Is64Bit);

        switch (Error) {
            case MachO::ObjcParse::Error::None:
                break;
            case MachO::ObjcParse::Error::NoObjcData:
                return RunResult(RunResult::Error::NoObjcData);
            case MachO::ObjcParse::Error::UnalignedSection:
                return RunResult(RunResult::Error::UnalignedSection);
            case MachO::ObjcParse::Error::DataOutOfBounds:
                return RunResult(RunResult::Error::ObjcDataOutOfBounds);
        }

        if (Opt.PrintCategories) {
            auto CategoryInfoList = MachO::ObjcClassCategoryInfoList();
            Error =
                CategoryInfoList.CollectFrom(Map,
                                             DeVirtualizer,
                                             AddrResolver,
                                             SegmentList,
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

        return RunResult();
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
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintObjcClassList::run()");
    }
}
