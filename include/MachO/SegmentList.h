/*
 * MachO/SegmentList.h
 * © suhas pai
 */

#pragma once

#include <algorithm>
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

        [[nodiscard]] constexpr
        auto findSectionWithName(const std::string_view Name) const noexcept
            -> const SectionInfo *
        {
            const auto Iter =
                std::ranges::find(this->SectionList, Name, &SectionInfo::Name);

            return Iter != this->SectionList.end() ? &*Iter : nullptr;
        }

        [[nodiscard]] constexpr
        auto findSectionWithFileOffset(const uint64_t Offset) const noexcept
            -> const SectionInfo *
        {
            const auto Iter =
                std::ranges::find_if(this->SectionList,
                                     [Offset](const auto &Section) noexcept {
                                        return Section.fileRange()
                                            .hasLoc(Offset);
                                     });

            if (Iter != this->SectionList.end()) {
                return &*Iter;
            }

            return nullptr;
        }

        [[nodiscard]] constexpr
        auto findSectionWithVmAddr(const uint64_t VmAddr) const noexcept
            -> const SectionInfo *
        {
            const auto Iter =
                std::ranges::find_if(this->SectionList,
                                     [VmAddr](const auto &Section) noexcept {
                                        return Section.vmRange()
                                            .hasLoc(VmAddr);
                                     });

            if (Iter != this->SectionList.end()) {
                return &*Iter;
            }

            return nullptr;
        }

        [[nodiscard]] auto
        findSectionWithVmAddrIndex(
            const uint64_t AddrIndex,
            uint64_t *const AddrOut = nullptr) const noexcept
            -> const SectionInfo *
        {
            assert(this->VmRange.hasIndex(AddrIndex));

            const auto FullAddr = this->VmRange.locForIndex(AddrIndex);
            if (AddrOut != nullptr) {
                *AddrOut = FullAddr;
            }

            return findSectionWithVmAddr(FullAddr);
        }

        [[nodiscard]] constexpr auto
        findSectionContainingVmRange(const ADT::Range VmRange) const noexcept
            -> const SectionInfo *
        {
            const auto Iter =
                std::ranges::find_if(this->SectionList,
                                     [VmRange](const auto &Section) noexcept {
                                        return Section.vmRange()
                                            .contains(VmRange);
                                     });

            if (Iter != this->SectionList.end()) {
                return &*Iter;
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
            return this->List.size();
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return this->List.empty();
        }

        [[nodiscard]] inline auto &at(const size_t Index) const noexcept {
            return this->List.at(Index);
        }

        [[nodiscard]] inline auto atOrNull(const size_t Index) const noexcept
            -> const SegmentInfo *
        {
            if (Utils::IndexOutOfBounds(Index, size())) {
                return nullptr;
            }

            return &this->List.at(Index);
        }

        auto
        add(const MachO::SegmentCommand &Segment,
            const bool IsBigEndian) noexcept -> decltype(*this);

        auto
        add(const MachO::SegmentCommand64 &Segment,
            const bool IsBigEndian) noexcept -> decltype(*this);

        [[nodiscard]] virtual auto
        getFileOffsetForVmAddr(const uint64_t VmAddr,
                               const uint64_t Size = 1) const noexcept
            -> std::optional<uint64_t>;

        [[nodiscard]] virtual
        auto findSegmentWithName(const std::string_view Name) const noexcept
            -> const SegmentInfo *
        {
            const auto Iter =
                std::ranges::find(this->List, Name, &SegmentInfo::Name);

            if (Iter != this->List.end()) {
                return &*Iter;
            }

            return nullptr;
        }

        [[nodiscard]] virtual
        auto findSegmentWithFileOffset(const uint64_t Offset) const noexcept
            -> const SegmentInfo *
        {
            const auto Iter =
                std::ranges::find_if(this->List,
                                     [Offset](const auto &Segment) noexcept {
                                        return Segment.FileRange.hasLoc(Offset);
                                     });

            if (Iter != this->List.end()) {
                return &*Iter;
            }

            return nullptr;
        }

        [[nodiscard]]
        virtual auto findSegmentWithVmAddr(const uint64_t Addr) const noexcept
            -> const SegmentInfo *
        {
            const auto Iter =
                std::ranges::find_if(this->List,
                                     [Addr](const auto &Segment) noexcept {
                                        return Segment.VmRange.hasLoc(Addr);
                                     });

            if (Iter != this->List.end()) {
                return &*Iter;
            }

            return nullptr;
        }

        [[nodiscard]] virtual
        auto findSegmentWithVmRange(const ADT::Range Range) const noexcept
            -> const SegmentInfo *
        {
            const auto Iter =
                std::ranges::find_if(this->List,
                                     [Range](const auto &Segment) noexcept {
                                        return Segment.VmRange.contains(Range);
                                     });

            if (Iter != this->List.end()) {
                return &*Iter;
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
            const std::initializer_list<
                SegmentSectionNameListPair> &L) const noexcept
                -> std::optional<
                        std::pair<const SegmentInfo *, const SectionInfo *>>
        {
            for (const auto &[SegmentName, SectionNameList] : L) {
                if (const auto Segment =
                        this->findSegmentWithName(SegmentName)) {

                    for (const auto &SectName : SectionNameList) {
                        if (const auto Section =
                                Segment->findSectionWithName(SectName))
                        {
                            return std::pair(Segment, Section);
                        }
                    }
                }
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr auto span() const noexcept
            -> std::span<const SegmentInfo>
        {
            return this->List;
        }
    };
}