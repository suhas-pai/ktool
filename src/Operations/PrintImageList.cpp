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
                    const bool PrintColon) noexcept
    {
        std::println(OutFile,
                     "Provided file has {} Images{}",
                     Utils::FormattedNumber(ImageCount),
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
                return Lhs.Address <=> Rhs.Address;
            case PrintImageList::Options::SortKind::ByInode:
                return Lhs.Inode <=> Rhs.Inode;
            case PrintImageList::Options::SortKind::ByModTime:
                return Lhs.ModTime <=> Rhs.ModTime;
            case PrintImageList::Options::SortKind::ByName:
                return Lhs.Path <=> Rhs.Path;
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

        const auto &Opt = this->Opt;
        const auto OutFile = this->OutFile;

        if (Opt.OnlyCount) {
            PrintImageCount(OutFile, ImageCount, /*PrintColon=*/false);
            return RunResult();
        }

        const auto Map = Dsc.map();

        auto ImageInfoList = std::vector<ImageInfo>();
        auto LongestImagePath = ADT::Maximizer<uint64_t>();

        ImageInfoList.reserve(ImageCount);
        for (const auto &Info : Dsc.imageInfoList()) {
            auto NewInfo = ImageInfo();

            NewInfo.Address = Info.Address;
            NewInfo.ModTime = Info.ModTime;
            NewInfo.Inode = Info.Inode;
            NewInfo.PathFileOffset = Info.PathFileOffset;
            NewInfo.Pad = Info.Pad;
            NewInfo.Path =
                Map.string(Info.PathFileOffset).value_or("<invalid>");

            LongestImagePath.set(NewInfo.Path.length());
            ImageInfoList.emplace_back(std::move(NewInfo));
        }

        if (!Opt.SortKindList.empty()) {
            const auto Comparator =
                [&](const auto &Lhs, const auto &Rhs) noexcept
            {
                for (const auto &Sort : Opt.SortKindList) {
                    const auto Compare = CompareInfosBySortKind(Lhs, Rhs, Sort);
                    if (Compare != 0) {
                        return Compare == std::strong_ordering::less;
                    }
                }

                return false;
            };

            std::ranges::sort(ImageInfoList, Comparator);
        }

        PrintImageCount(OutFile, ImageCount, /*PrintColon=*/true);
        const auto ImageInfoListSizeDigitCount =
            Utils::GetIntegerDigitCount(ImageCount);

        auto Counter = static_cast<uint64_t>(1);
        for (const auto &Info : ImageInfoList) {
            std::print(OutFile,
                       "Image {:>{}}: \"{}\"",
                       Counter,
                       ImageInfoListSizeDigitCount,
                       Info.Path);

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
                           Utils::Timestamp(static_cast<time_t>(Info.ModTime)),
                           Info.ModTime,
                           Info.Inode);
            }

            std::println(OutFile, "");
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
