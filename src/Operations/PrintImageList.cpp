//
//  PrintImageList.cpp
//  ktool
//
//  Created by Suhas Pai on 7/11/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/DyldSharedCache.h"
#include "Utils/Timestamp.h"

#include "Common.h"
#include "Operation.h"
#include "PrintImageList.h"

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

static int
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

    assert(0 && "Unrecognized (and invalid) Sort-Type");
}


int
PrintImageListOperation::Run(const ConstDscMemoryObject &Object,
                             const struct Options &Options) noexcept
{
    const auto Begin = Object.getMap().getBeginAs<const char>();
    const auto &Header = Object.getHeaderV0();

    auto ImageInfoList = std::vector<ImageInfo>();
    auto LongestImagePath = LargestIntHelper();

    ImageInfoList.reserve(Header.ImagesCount);

    for (const auto &Info : Header.getImageInfoList()) {
        const auto Path = Begin + Info.PathFileOffset;
        auto NewInfo = ImageInfo { .Path = Path };

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

            return true;
        };

        std::sort(ImageInfoList.begin(), ImageInfoList.end(), Comparator);
    }

    const auto ImageInfoListSizeDigitLength =
        PrintUtilsGetIntegerDigitLength(ImageInfoList.size());

    fprintf(Options.OutFile,
            "Provided file has %" PRIuPTR " Images:\n",
            ImageInfoList.size());

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
            PrintUtilsWriteOffset64(Options.OutFile,
                                    Info.Address,
                                    " <Address: ");

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

static inline bool
ListHasSortKind(
    std::vector<PrintImageListOperation::Options::SortKind> &List,
    const PrintImageListOperation::Options::SortKind &Sort) noexcept
{
    const auto ListEnd = List.cend();
    return (std::find(List.cbegin(), ListEnd, Sort) != ListEnd);
}

static inline void
AddSortKind(PrintImageListOperation::Options::SortKind SortKind,
            const char *Option,
            struct PrintImageListOperation::Options &Options) noexcept
{
    if (ListHasSortKind(Options.SortKindList, SortKind)) {
        fprintf(Options.OutFile, "Option %s specified twice\n", Option);
    } else {
        Options.SortKindList.push_back(SortKind);
    }
}

struct PrintImageListOperation::Options
PrintImageListOperation::ParseOptionsImpl(const ArgvArray &Argv,
                                          int *IndexOut) noexcept
{
    struct Options Options;
    for (const auto &Argument : Argv) {
        if (strcmp(Argument, "-v") == 0 || strcmp(Argument, "--verbose") == 0) {
            Options.Verbose = true;
        } else if (strcmp(Argument, "--sort-by-address") == 0) {
            AddSortKind(Options::SortKind::ByAddress, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-modtime") == 0) {
            AddSortKind(Options::SortKind::ByModTime, Argument, Options);
        } else if (strcmp(Argument, "--sort-by-name") == 0) {
            AddSortKind(Options::SortKind::ByName, Argument, Options);
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

    return Options;
}

void
PrintImageListOperation::ParseOptions(const ArgvArray &Argv,
                                      int *IndexOut) noexcept
{
    Options = ParseOptionsImpl(Argv, IndexOut);
}

int PrintImageListOperation::Run(const MemoryObject &Object) const noexcept {
    switch (Object.getKind()) {
        case ObjectKind::None:
            assert(0 && "Object-Kind is None");
        case ObjectKind::MachO:
        case ObjectKind::FatMachO:
            return InvalidObjectKind;
        case ObjectKind::DyldSharedCache:
            return Run(cast<ObjectKind::DyldSharedCache>(Object), Options);
    }

    assert(0 && "Unrecognized Object-Kind");
}
