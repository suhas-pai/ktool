/*
 * MachO/SegmentList.cpp
 * © suhas pai
 */

#include "MachO/SegmentList.h"

namespace MachO {
    SegmentList::SegmentList(const MachO::LoadCommandsMap &Map,
                             const bool Is64Bit) noexcept
    {
        const auto IsBigEndian = Map.isBigEndian();
        if (Is64Bit) {
            auto SegmentList = Map |
                std::views::transform([IsBigEndian](const auto &Lc) noexcept {
                  return dyn_cast<SegmentCommand64>(&Lc, IsBigEndian);
                }) |
                std::views::filter([](const auto Segment) noexcept {
                  return Segment != nullptr;
                });

            std::ranges::for_each(SegmentList,
                                  [this, IsBigEndian](const auto Segment) {
                                    this->add(*Segment, IsBigEndian);
                                  });
        } else {
            auto SegmentList = Map |
                std::views::transform([IsBigEndian](const auto &Lc) noexcept {
                    return dyn_cast<SegmentCommand>(&Lc, IsBigEndian);
                }) |
                std::views::filter([](const auto Segment) noexcept {
                    return Segment != nullptr;
                });

            std::ranges::for_each(SegmentList,
                                  [this, IsBigEndian](const auto Segment) {
                                    this->add(*Segment, IsBigEndian);
                                  });
        }
    }

    auto
    SegmentList::add(const SegmentCommand &Segment,
                     const bool IsBigEndian) noexcept
        -> decltype(*this)
    {
        auto &Info = List.emplace_back(SegmentInfo {
            .Name = std::string(Segment.segmentName()),
            .VmRange = Segment.vmRange(IsBigEndian),
            .FileRange = Segment.fileRange(IsBigEndian),
            .MaxProt = Segment.maxProt(IsBigEndian),
            .InitProt = Segment.initProt(IsBigEndian),
            .Flags = Segment.flags(IsBigEndian),
            .SectionList = {},
            .Index = static_cast<uint32_t>(List.size())
        });

        const auto SectionListRange = Segment.sectionList(IsBigEndian) |
            std::views::transform(
                [IsBigEndian](const auto &Section) noexcept {
                    return SectionInfo {
                        .Name = std::string(Section.sectionName()),
                        .Addr = Section.addr(IsBigEndian),
                        .Size = Section.size(IsBigEndian),
                        .FileOffset = Section.fileOffset(IsBigEndian),
                        .Align = Section.align(IsBigEndian),
                        .RelocFileOffset = Section.relocFileOffset(IsBigEndian),
                        .RelocsCount = Section.relocsCount(IsBigEndian),
                        .Flags = Section.flags(IsBigEndian),
                        .Reserved1 = Section.reserved1(IsBigEndian),
                        .Reserved2 = Section.reserved2(IsBigEndian),
                        .Reserved3 = 0
                    };
                }
            );

        Info.SectionList.append_range(SectionListRange);
        return *this;
    }

    auto
    SegmentList::add(const SegmentCommand64 &Segment,
                     const bool IsBigEndian) noexcept
        -> decltype(*this)
    {
        auto &Info = List.emplace_back(SegmentInfo {
            .Name = std::string(Segment.segmentName()),
            .VmRange = Segment.vmRange(IsBigEndian),
            .FileRange = Segment.fileRange(IsBigEndian),
            .MaxProt = Segment.maxProt(IsBigEndian),
            .InitProt = Segment.initProt(IsBigEndian),
            .Flags = Segment.flags(IsBigEndian),
            .SectionList = {},
            .Index = static_cast<uint32_t>(List.size())
        });

        const auto SectionListRange = Segment.sectionList(IsBigEndian) |
            std::views::transform(
                [IsBigEndian](const auto &Section) noexcept {
                    return SectionInfo {
                        .Name = std::string(Section.sectionName()),
                        .Addr = Section.addr(IsBigEndian),
                        .Size = Section.size(IsBigEndian),
                        .FileOffset = Section.fileOffset(IsBigEndian),
                        .Align = Section.align(IsBigEndian),
                        .RelocFileOffset = Section.relocFileOffset(IsBigEndian),
                        .RelocsCount = Section.relocsCount(IsBigEndian),
                        .Flags = Section.flags(IsBigEndian),
                        .Reserved1 = Section.reserved1(IsBigEndian),
                        .Reserved2 = Section.reserved2(IsBigEndian),
                        .Reserved3 = Section.reserved3(IsBigEndian)
                    };
                }
            );

        Info.SectionList.append_range(SectionListRange);
        return *this;
    }

    auto
    SegmentList::getFileOffsetForVmAddr(const uint64_t VmAddr,
                                        const uint64_t Size) const noexcept
        -> std::optional<uint64_t>
    {
        const auto Filter = [VmAddr](const auto &SegInfo) noexcept {
            return SegInfo.VmRange.hasLoc(VmAddr);
        };

        for (const auto &SegInfo : this->List | std::views::filter(Filter)) {
            const uint64_t VmIndex = SegInfo.VmRange.indexForLoc(VmAddr);
            if (!SegInfo.FileRange.hasIndex(VmIndex)) {
                return std::nullopt;
            }

            if (!SegInfo.FileRange.hasEndIndex(VmIndex + Size)) {
                return std::nullopt;
            }

            return SegInfo.FileRange.locForIndex(VmIndex);
        }

        return std::nullopt;
    }
}
