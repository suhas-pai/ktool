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
        fprintf(OutFile, "Provided file has %" PRIu32 " Images", ImageCount);
        if (PrintColon) {
            fputc(':', OutFile);
        }

        fputc('\n', OutFile);
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
            fprintf(OutFile,
                    "Image %" LEFTPAD_FMT PRIu64 ": ",
                    PAD_FMT_ARGS(ImageInfoListSizeDigitCount),
                    Counter);

            const auto WrittenOut =
                fprintf(OutFile, "\"%s\"", Info.Path.data());

            if (Opt.Verbose) {
                const auto RightPad =
                    static_cast<int>(LongestImagePath.value() +
                                     STR_LENGTH("\"\""));

                Utils::RightPadSpaces(OutFile, WrittenOut, RightPad);
                Utils::PrintAddress(OutFile,
                                    Info.Address,
                                    /*Is64Bit=*/true,
                                    /*Prefix=*/" <Address: ");

                fprintf(OutFile,
                        ", Modification-Time: %s (Value: %" PRIu64 "), "
                        "Inode: %" PRIu64 ">",
                        Utils::GetHumanReadableTimestamp(
                            static_cast<time_t>(Info.ModTime)).c_str(),
                        Info.ModTime,
                        Info.Inode);
            }

            fputc('\n', OutFile);
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