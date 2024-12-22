/*
 * Operations/PrintImageList.cpp
 * © suhas pai
 */

#include <algorithm>
#include "ADT/Maximizer.h"

#include "Objects/DscImage.h"
#include "Operations/PrintImageList.h"

#include "Utils/Print.h"

namespace Operations {
    PrintImageList::PrintImageList(FILE *const OutFile,
                                   const struct Options &Options) noexcept
    : Base(Operations::Kind::PrintImageList), OutFile(OutFile), Opt(Options) {}

    bool
    PrintImageList::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintImageList::supportsObjectKind()");
            case Objects::Kind::DyldSharedCache:
                return true;
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintImageList::supportsObjectKind()");
    }

    static void
    PrintImageCount(FILE *const OutFile,
                    const uint32_t ImageCount,
                    const bool PrintColon = true) noexcept
    {
        std::print(OutFile,
                   "Provided file has {} Images{}\n",
                   ImageCount,
                   PrintColon ? ":" : "");
    }

    struct ImageInfo : public DyldSharedCache::ImageInfo {
        std::string_view Path;

        [[nodiscard]]
        inline auto operator<(const ImageInfo &Rhs) const noexcept {
            return Path < Rhs.Path;
        }
    };

    [[nodiscard]] static auto
    CompareInfosBySortKind(
        const ImageInfo &Lhs,
        const ImageInfo &Rhs,
        const PrintImageList::Options::SortKind SortKind) noexcept
    {
        switch (SortKind) {
            case PrintImageList::Options::SortKind::ByAddress:
                if (Lhs.Address == Rhs.Address) {
                    return 0;
                } else if (Lhs.Address < Rhs.Address) {
                    return -1;
                }

                return 1;
            case PrintImageList::Options::SortKind::ByInode:
                if (Lhs.Inode == Rhs.Inode) {
                    return 0;
                } else if (Lhs.Inode < Rhs.Inode) {
                    return -1;
                }

                return 1;
            case PrintImageList::Options::SortKind::ByModTime:
                if (Lhs.ModTime == Rhs.ModTime) {
                    return 0;
                } else if (Lhs.ModTime < Rhs.ModTime) {
                    return -1;
                }

                return 1;
            case PrintImageList::Options::SortKind::ByName:
                return Lhs.Path.compare(Rhs.Path);
        }

        assert(false && "Unrecognized (and invalid) Sort-Kind");
    }

    auto
    PrintImageList::run(const Objects::DyldSharedCache &Dsc) const noexcept
        -> RunResult
    {
        const auto ImageCount = Dsc.imageCount();
        if (ImageCount == 0) {
            return RunResult(RunResult::Error::NoImages);
        }

        if (Opt.OnlyCount) {
            PrintImageCount(OutFile, ImageCount, false);
            return RunResult();
        }

        const auto Map = Dsc.map();

        auto ImageInfoList = std::vector<ImageInfo>();
        auto LongestImagePath = ADT::Maximizer<uint64_t>();

        ImageInfoList.reserve(ImageCount);
        for (const auto &Info : Dsc.imageInfoList()) {
            auto NewInfo = ImageInfo();
            if (const auto PathOpt = Map.string(Info.PathFileOffset)) {
                NewInfo.Path = PathOpt.value();
            } else {
                NewInfo.Path = "<invalid>";
            }

            NewInfo.Address = Info.Address;
            NewInfo.ModTime = Info.ModTime;
            NewInfo.Inode = Info.Inode;
            NewInfo.PathFileOffset = Info.PathFileOffset;
            NewInfo.Pad = Info.Pad;

            LongestImagePath.set(NewInfo.Path.length());
            ImageInfoList.emplace_back(std::move(NewInfo));
        }

        if (!Opt.SortKindList.empty()) {
            const auto Comparator =
                [&](const auto &Lhs, const auto &Rhs) noexcept
            {
                auto Compare = int();
                for (const auto &Sort : Opt.SortKindList) {
                    Compare = CompareInfosBySortKind(Lhs, Rhs, Sort);
                    if (Compare != 0) {
                        break;
                    }
                }

                return Compare < 0;
            };

            std::sort(ImageInfoList.begin(), ImageInfoList.end(), Comparator);
        }

        PrintImageCount(OutFile, ImageCount);
        const auto ImageInfoListSizeDigitCount =
            Utils::GetIntegerDigitCount(ImageCount);

        auto Counter = uint64_t(1);
        for (const auto &Info : ImageInfoList) {
            std::print(OutFile,
                       "Image {:>{}}: ",
                       Counter,
                       ImageInfoListSizeDigitCount);

            std::print(OutFile, "\"{}\"", Info.Path);

            const auto WrittenOut = STR_LENGTH("\"\"") + Info.Path.length();
            if (Opt.Verbose) {
                const auto RightPad =
                    LongestImagePath.value() + STR_LENGTH("\"\"");

                std::print(OutFile,
                           "{:<{}}"
                           "{}, Modification-Time: {} (Value: {}), Inode: {}",
                           "",
                           RightPad - WrittenOut,
                           Utils::Address(Info.Address),
                           Utils::GetHumanReadableTimestamp(
                               static_cast<time_t>(Info.ModTime)),
                           Info.ModTime,
                           Info.Inode);
            }

            std::print(OutFile, "\n");
            Counter++;
        }

        return RunResult();
    }

    auto
    PrintImageList::run(const Objects::Base &Base) const noexcept -> RunResult {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false &&
                       "PrintImageList::run() got Object with Kind::None");
            case Objects::Kind::DyldSharedCache:
                return run(static_cast<const Objects::DyldSharedCache &>(Base));
            case Objects::Kind::MachO:
            case Objects::Kind::DscImage:
            case Objects::Kind::FatMachO:
                return RunResult(RunResult::Error::Unsupported);
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintImageList::run()");
    }
}
