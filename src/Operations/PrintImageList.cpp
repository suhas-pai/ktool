//
//  Operations/PrintImageList.cpp
//  ktool
//
//  Created by Suhas Pai on 7/11/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <algorithm>

#include "Operations/Operation.h"
#include "Operations/PrintImageList.h"

#include "Utils/PrintUtils.h"
#include "Utils/Timestamp.h"

PrintImageListOperation::PrintImageListOperation() noexcept
: Operation(OpKind) {}

PrintImageListOperation::PrintImageListOperation(
    const struct Options &Options) noexcept
: Operation(OpKind), Options(Options) {}

struct ImageInfo : public DyldSharedCache::ImageInfo {
    std::string_view Path;

    [[nodiscard]] inline bool operator<(const ImageInfo &Rhs) const noexcept {
        return (Path < Rhs.Path);
    }
};

[[nodiscard]] static int
CompareInfosBySortKind(
    const ImageInfo &Lhs,
    const ImageInfo &Rhs,
    const PrintImageListOperation::Options::SortKind SortKind) noexcept
{
    switch (SortKind) {
        case PrintImageListOperation::Options::SortKind::ByAddress:
            if (Lhs.Address == Rhs.Address) {
                return 0;
            } else if (Lhs.Address < Rhs.Address) {
                return -1;
            }

            return 1;
        case PrintImageListOperation::Options::SortKind::ByInode:
            if (Lhs.Inode == Rhs.Inode) {
                return 0;
            } else if (Lhs.Inode < Rhs.Inode) {
                return -1;
            }

            return 1;
        case PrintImageListOperation::Options::SortKind::ByModTime:
            if (Lhs.ModTime == Rhs.ModTime) {
                return 0;
            } else if (Lhs.ModTime < Rhs.ModTime) {
                return -1;
            }

            return 1;
        case PrintImageListOperation::Options::SortKind::ByName:
            return Lhs.Path.compare(Rhs.Path);
    }

    assert(0 && "Unrecognized (and invalid) Sort-Kind");
}

static void
PrintImageCount(FILE *const OutFile,
                const uint32_t ImageCount,
                const bool PrintColon = true) noexcept
{
    fprintf(OutFile, "Provided file has %" PRIu32 " Images", ImageCount);
    if (PrintColon) {
        fputc(':', OutFile);
    }

    fputc('\n', OutFile);
}

int
PrintImageListOperation::Run(const DscMemoryObject &Object,
                             const struct Options &Options) noexcept
{
    const auto ImageCount = Object.getImageCount();
    if (ImageCount == 0) {
        fputs("Provided file has no images\n", Options.ErrFile);
        return 1;
    }

    if (Options.OnlyCount) {
        PrintImageCount(Options.OutFile, ImageCount, false);
        return 0;
    }

    const auto Map = Object.getMap().getBegin();

    auto ImageInfoList = std::vector<ImageInfo>();
    auto LongestImagePath = LargestIntHelper();

    ImageInfoList.reserve(ImageCount);

    for (const auto &Info : Object.getConstImageInfoList()) {
        auto NewInfo = ImageInfo { .Path = Info.getPath(Map) };

        NewInfo.Address = Info.Address;
        NewInfo.ModTime = Info.ModTime;
        NewInfo.Inode = Info.Inode;
        NewInfo.PathFileOffset = Info.PathFileOffset;
        NewInfo.Pad = Info.Pad;

        LongestImagePath = NewInfo.Path.length();
        ImageInfoList.emplace_back(std::move(NewInfo));
    }

    if (!Options.SortKindList.empty()) {
        const auto Comparator = [&](const auto &Lhs, const auto &Rhs) noexcept {
            for (const auto &Sort : Options.SortKindList) {
                const auto Compare = CompareInfosBySortKind(Lhs, Rhs, Sort);
                if (Compare != 0) {
                    return (Compare < 0);
                }
            }

            return false;
        };

        std::sort(ImageInfoList.begin(), ImageInfoList.end(), Comparator);
    }

    PrintImageCount(Options.OutFile, ImageCount);
    const auto ImageInfoListSizeDigitLength =
        PrintUtilsGetIntegerDigitLength(ImageCount);

    auto Counter = static_cast<uint64_t>(1);
    for (const auto &Info : ImageInfoList) {
        fprintf(Options.OutFile,
                "Image %0*" PRIu64 ": ",
                ImageInfoListSizeDigitLength,
                Counter);

        const auto WrittenOut =
            fprintf(Options.OutFile, "\"%s\"", Info.Path.data());

        if (Options.Verbose) {
            const auto RightPad =
                static_cast<int>(LongestImagePath + LENGTH_OF("\"\""));

            PrintUtilsRightPadSpaces(Options.OutFile, WrittenOut, RightPad);

            fputs(" <", Options.OutFile);
            if (Info.isAlias(Map)) {
                fputs("Alias, ", Options.OutFile);
            }

            PrintUtilsWriteOffset(Options.OutFile,
                                  Info.Address,
                                  true,
                                  "Address: ");

            fprintf(Options.OutFile,
                    ", Modification-Time: %s, Inode: %" PRIu64 ">",
                    GetHumanReadableTimestamp(Info.ModTime).data(),
                    Info.Inode);
        }

        fputc('\n', Options.OutFile);
        Counter++;
    }

    return 0;
}

[[nodiscard]] static inline bool
ListHasSortKind(
    const std::vector<PrintImageListOperation::Options::SortKind> &List,
    const PrintImageListOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(PrintImageListOperation::Options::SortKind SortKind,
            const char *const Option,
            struct PrintImageListOperation::Options &Options) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Option);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

auto
PrintImageListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                          int *const IndexOut) noexcept
    -> struct PrintImageListOperation::Options
{
    auto Index = int();
    struct Options Options;

    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--count") == 0) {
            Options.OnlyCount = true;
        } else if (strcmp(Argument, "--sort-by-address") == 0) {
            AddSortKind(Options::SortKind::ByAddress, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-inode") == 0) {
            AddSortKind(Options::SortKind::ByInode, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-modtime") == 0) {
            AddSortKind(Options::SortKind::ByModTime, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-name") == 0) {
            AddSortKind(Options::SortKind::ByName, Argument, Options);
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

    if (Options.OnlyCount && !Options.SortKindList.empty()) {
        fputs("Error: Provided --sort-by-* when only printing count\n", stderr);
        exit(1);
    }

    if (IndexOut != nullptr) {
        *IndexOut = Index;
    }

    return Options;
}

int PrintImageListOperation::ParseOptions(const ArgvArray &Argv) noexcept {
    auto Index = int();
    Options = ParseOptionsImpl(Argv, &Index);

    return Index;
}

int PrintImageListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::DyldSharedCache:
            return Run(cast<ObjectKind::DyldSharedCache>(Object), Options);
        case ObjectKind::MachO:
        case ObjectKind::FatMachO:
        case ObjectKind::DscImage:
            return InvalidObjectKind;
    }

    assert(0 && "Unrecognized Object-Kind");
}
