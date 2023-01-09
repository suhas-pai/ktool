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
            for (const auto &LC : Map) {
                if (const auto Segment =
                        dyn_cast<MachO::SegmentCommand64>(&LC, IsBigEndian))
                {
                    auto &Info = List.emplace_back(SegmentInfo {
                        .Name = std::string(Segment->segmentName()),
                        .VmRange =
                            ADT::Range::FromSize(Segment->vmAddr(IsBigEndian),
                                                 Segment->vmSize(IsBigEndian)),
                        .FileRange =
                            ADT::Range::FromSize(
                                Segment->fileOffset(IsBigEndian),
                                Segment->fileSize(IsBigEndian)),
                        .MaxProt = Segment->maxProt(IsBigEndian),
                        .InitProt = Segment->initProt(IsBigEndian),
                        .Flags = Segment->flags(IsBigEndian),
                    });

                    for (const auto &Section :
                            Segment->sectionList(IsBigEndian))
                    {
                        Info.SectionList.emplace_back(SectionInfo {
                            .Name = std::string(Section.sectionName()),
                            .Addr = Section.addr(IsBigEndian),
                            .Size = Section.size(IsBigEndian),
                            .FileOffset = Section.fileOffset(IsBigEndian),
                            .Align = Section.align(IsBigEndian),
                            .RelocFileOffset =
                                Section.relocFileOffset(IsBigEndian),
                            .RelocsCount = Section.relocsCount(IsBigEndian),
                            .Flags = Section.flags(IsBigEndian),
                            .Reserved1 = Section.reserved1(IsBigEndian),
                            .Reserved2 = Section.reserved2(IsBigEndian),
                            .Reserved3 = Section.reserved3(IsBigEndian)
                        });
                    }
                }
            }
        } else {
            for (const auto &LC : Map) {
                if (const auto Segment =
                        dyn_cast<MachO::SegmentCommand>(&LC, IsBigEndian))
                {
                    auto &Info = List.emplace_back(SegmentInfo {
                        .Name = std::string(Segment->segmentName()),
                        .VmRange =
                            ADT::Range::FromSize(Segment->vmAddr(IsBigEndian),
                                                 Segment->vmSize(IsBigEndian)),
                        .FileRange =
                            ADT::Range::FromSize(
                                Segment->fileOffset(IsBigEndian),
                                Segment->fileSize(IsBigEndian)),
                        .MaxProt = Segment->maxProt(IsBigEndian),
                        .InitProt = Segment->initProt(IsBigEndian),
                        .Flags = Segment->flags(IsBigEndian),
                    });

                    for (const auto &Section :
                            Segment->sectionList(IsBigEndian))
                    {
                        Info.SectionList.emplace_back(SectionInfo {
                            .Name = std::string(Section.sectionName()),
                            .Addr = Section.addr(IsBigEndian),
                            .Size = Section.size(IsBigEndian),
                            .FileOffset = Section.fileOffset(IsBigEndian),
                            .Align = Section.align(IsBigEndian),
                            .RelocFileOffset =
                                Section.relocFileOffset(IsBigEndian),
                            .RelocsCount = Section.relocsCount(IsBigEndian),
                            .Flags = Section.flags(IsBigEndian),
                            .Reserved1 = Section.reserved1(IsBigEndian),
                            .Reserved2 = Section.reserved2(IsBigEndian),
                            .Reserved3 = 0
                        });
                    }
                }
            }
        }
    }
}