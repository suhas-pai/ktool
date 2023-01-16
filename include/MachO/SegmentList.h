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
    struct SegmentList {
    public:
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
            GetSectionWithName(const std::string_view Name) const noexcept
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
            GetSectionWithFileOffset(const uint64_t Offset) const noexcept
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
            GetSectionWithVmOffset(const uint64_t Offset) const noexcept
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
    protected:
        std::vector<SegmentInfo> List;
    public:
        explicit
        SegmentList(const MachO::LoadCommandsMap &Map, bool Is64Bit) noexcept;

        [[nodiscard]] constexpr auto
        GetSegmentWithName(const std::string_view Name) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.Name == Name) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto
        GetSegmentWithFileOffset(const uint64_t Offset) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.FileRange.containsLoc(Offset)) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto
        GetSegmentWithVmAddr(const uint64_t Addr) const noexcept
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