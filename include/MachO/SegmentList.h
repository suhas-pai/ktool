/*
 * MachO/SegmentList.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <initializer_list>
#include <optional>
#include <vector>

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
            return ADT::Range::FromSize(this->FileOffset, this->Size);
        }

        [[nodiscard]] constexpr auto vmRange() const noexcept {
            return ADT::Range::FromSize(this->Addr, this->Size);
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

        uint32_t Index;

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

        [[nodiscard]] constexpr
        auto findSectionWithFileOffset(const uint64_t Offset) const noexcept
            -> const SectionInfo *
        {
            for (const auto &Section : SectionList) {
                if (Section.fileRange().hasLoc(Offset)) {
                    return &Section;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr
        auto findSectionWithVmAddr(const uint64_t VmAddr) const noexcept
            -> const SectionInfo *
        {
            for (const auto &Section : SectionList) {
                if (Section.vmRange().hasLoc(VmAddr)) {
                    return &Section;
                }
            }

            return nullptr;
        }

        [[nodiscard]] auto
        findSectionWithVmAddrIndex(
            const uint64_t AddrIndex,
            uint64_t *const AddrOut = nullptr) const noexcept
            -> const SectionInfo *
        {
            assert(VmRange.hasIndex(AddrIndex));

            const auto FullAddr = VmRange.locForIndex(AddrIndex);
            if (AddrOut != nullptr) {
                *AddrOut = FullAddr;
            }

            return findSectionWithVmAddr(FullAddr);
        }

        [[nodiscard]] constexpr auto
        findSectionContainingVmRange(const ADT::Range &VmRange) const noexcept
            -> const SectionInfo *
        {
            for (const auto &Section : SectionList) {
                if (Section.vmRange().contains(VmRange)) {
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

        [[nodiscard]] constexpr auto size() const noexcept {
            return List.size();
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return List.empty();
        }

        [[nodiscard]] inline auto &at(const size_t Index) const noexcept {
            return List.at(Index);
        }

        [[nodiscard]]
        inline auto atOrNull(const size_t Index) const noexcept
            -> const SegmentInfo *
        {
            if (Utils::IndexOutOfBounds(Index, size())) {
                return nullptr;
            }

            return &List.at(Index);
        }

        auto
        addSegment(const MachO::SegmentCommand &Segment,
                   const bool IsBigEndian) noexcept -> decltype(*this);

        auto
        addSegment(const MachO::SegmentCommand64 &Segment,
                   const bool IsBigEndian) noexcept -> decltype(*this);

        [[nodiscard]] virtual auto
        getFileOffsetForVmAddr(const uint64_t VmAddr,
                               const uint64_t Size = 1) const noexcept
            -> std::optional<uint64_t>;

        [[nodiscard]] virtual
        auto findSegmentWithName(const std::string_view Name) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.Name == Name) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]] virtual
        auto findSegmentWithFileOffset(const uint64_t Offset) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.FileRange.hasLoc(Offset)) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]]
        virtual auto findSegmentWithVmAddr(const uint64_t Addr) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.VmRange.hasLoc(Addr)) {
                    return &Info;
                }
            }

            return nullptr;
        }

        [[nodiscard]] virtual
        auto findSegmentWithVmRange(const ADT::Range &Range) const noexcept
            -> const SegmentInfo *
        {
            for (const auto &Info : List) {
                if (Info.VmRange.contains(Range)) {
                    return &Info;
                }
            }

            return nullptr;
        }

        struct SegmentSectionNameListPair {
            std::string_view SegmentName;
            std::initializer_list<std::string_view> SectionNameList;
        };

        [[nodiscard]] constexpr auto
        findSectionWithName(
            const std::initializer_list<SegmentSectionNameListPair> &L)
                const noexcept -> const SectionInfo *
        {
            for (const auto &[SegmentName, SectionNameList] : L) {
                if (const auto Segment =
                        this->findSegmentWithName(SegmentName)) {

                    for (const auto &SectName : SectionNameList) {
                        if (const auto Section =
                                Segment->findSectionWithName(SectName))
                        {
                            return Section;
                        }
                    }
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto
        findSectionWithNameAndSegment(
            const std::initializer_list<SegmentSectionNameListPair> &L)
                const noexcept
                    -> std::optional<
                        std::pair<const SegmentInfo &, const SectionInfo &>>
        {
            for (const auto &[SegmentName, SectionNameList] : L) {
                if (const auto Segment =
                        this->findSegmentWithName(SegmentName)) {

                    for (const auto &SectName : SectionNameList) {
                        if (const auto Section =
                                Segment->findSectionWithName(SectName))
                        {
                            return std::pair(*Segment, *Section);
                        }
                    }
                }
            }

            return std::nullopt;
        }
    };
}