/*
 * MachO/SegmentList.h
 * © suhas pai
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Mach/VmProt.h"
#include "MachO/LoadCommands.h"
#include "MachO/LoadCommandsMap.h"

namespace MachO {
    struct SectionInfo {
        std::string Name;

        uint64_t Addr;
        uint64_t Size;

        uint32_t FileOffset;

        uint32_t Align;
        uint32_t RelocFileOffset;
        uint32_t RelocsCount;

        MachO::SegmentCommand::Section::FlagsStruct Flags;

        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;

        [[nodiscard]] constexpr auto fileRange() const noexcept {
            return ADT::Range::FromSize(FileOffset, Size);
        }

        [[nodiscard]] constexpr auto vmRange() const noexcept {
            return ADT::Range::FromSize(Addr, Size);
        }
    };

    struct SegmentInfo {
        std::string Name;

        ADT::Range VmRange;
        ADT::Range FileRange;

        Mach::VmProt MaxProt;
        Mach::VmProt InitProt;

        MachO::SegmentCommand::FlagsStruct Flags;
        std::vector<SectionInfo> SectionList;

        [[nodiscard]] constexpr auto
        findSectionWithName(const std::string_view Name) const noexcept
            -> const SectionInfo *
        {
            for (const auto &Section : SectionList) {
                if (Section.Name == Name) {
                    return &Section;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto
        findSectionWithFileOffset(const uint64_t Offset) const noexcept
            -> const SectionInfo *
        {
            for (const auto &Section : SectionList) {
                if (Section.fileRange().containsLoc(Offset)) {
                    return &Section;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto
        findSectionWithVmAddr(const uint64_t Offset) const noexcept
            -> const SectionInfo *
        {
            for (const auto &Section : SectionList) {
                if (Section.vmRange().containsLoc(Offset)) {
                    return &Section;
                }
            }

            return nullptr;
        }
    };

    struct SegmentList {
    protected:
        std::vector<SegmentInfo> List;
    public:
        SegmentList() noexcept = default;

        explicit
        SegmentList(const MachO::LoadCommandsMap &Map, bool Is64Bit) noexcept;

        constexpr auto
        addSegment(const MachO::SegmentCommand &Segment,
                   const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            auto &Info = List.emplace_back(SegmentInfo {
                .Name = std::string(Segment.segmentName()),
                .VmRange =
                    ADT::Range::FromSize(Segment.vmAddr(IsBigEndian),
                                         Segment.vmSize(IsBigEndian)),
                .FileRange =
                    ADT::Range::FromSize(Segment.fileOffset(IsBigEndian),
                                         Segment.fileSize(IsBigEndian)),
                .MaxProt = Segment.maxProt(IsBigEndian),
                .InitProt = Segment.initProt(IsBigEndian),
                .Flags = Segment.flags(IsBigEndian),
                .SectionList = {}
            });

            for (const auto &Section :
                    Segment.sectionList(IsBigEndian))
            {
                Info.SectionList.emplace_back(SectionInfo {
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
                });
            }

            return *this;
        }

        constexpr auto
        addSegment(const MachO::SegmentCommand64 &Segment,
                   const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            auto &Info = List.emplace_back(SegmentInfo {
                .Name = std::string(Segment.segmentName()),
                .VmRange =
                    ADT::Range::FromSize(Segment.vmAddr(IsBigEndian),
                                         Segment.vmSize(IsBigEndian)),
                .FileRange =
                    ADT::Range::FromSize(Segment.fileOffset(IsBigEndian),
                                         Segment.fileSize(IsBigEndian)),
                .MaxProt = Segment.maxProt(IsBigEndian),
                .InitProt = Segment.initProt(IsBigEndian),
                .Flags = Segment.flags(IsBigEndian),
                .SectionList = {}
            });

            for (const auto &Section :
                    Segment.sectionList(IsBigEndian))
            {
                Info.SectionList.emplace_back(SectionInfo {
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
                });
            }

            return *this;
        }

        [[nodiscard]] virtual auto
        findSegmentWithName(const std::string_view Name) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.Name == Name) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]] virtual auto
        findSegmentWithFileOffset(const uint64_t Offset) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.FileRange.containsLoc(Offset)) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]] virtual auto
        findSegmentWithVmAddr(const uint64_t Addr) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.VmRange.containsLoc(Addr)) {
                    return &Info;
                }
            }

            return nullptr;
        }
    };
}