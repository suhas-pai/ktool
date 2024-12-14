//
//  ADT/Mach-O/SegmentInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ADT/Range.h"
#include "ADT/Mach-O/MemoryProtections.h"

#include "Concepts/Const.h"
#include "Utils/MiscTemplates.h"

#include "LoadCommandsCommon.h"

namespace MachO {
    struct SegmentInfo;
    struct SectionInfo {
    protected:
        const SegmentInfo *Segment;
        std::string Name;

        Range FileRange;
        Range MemoryRange;

        SegmentSectionFlags Flags;

        uint32_t Reserved1;
        uint32_t Reserved2;
    public:
        [[nodiscard]] constexpr auto getSegment() const noexcept {
            return Segment;
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getFileRange() const noexcept {
            return this->FileRange;
        }

        [[nodiscard]] constexpr auto getMemoryRange() const noexcept {
            return this->MemoryRange;
        }

        [[nodiscard]] constexpr auto getFlags() const noexcept {
            return this->Flags;
        }

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->getFlags().getKind();
        }

        [[nodiscard]] constexpr auto getReserved1() const noexcept {
            return this->Reserved1;
        }

        [[nodiscard]] constexpr auto getReserved2() const noexcept {
            return this->Reserved2;
        }

        constexpr auto setSegment(const SegmentInfo *const Segment) noexcept
            -> decltype(*this)
        {
            this->Segment = Segment;
            return *this;
        }

        constexpr auto setNameCopy(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto setName(std::string_view &&Name) noexcept
            -> decltype(*this)
        {
            this->Name = std::move(Name);
            return *this;
        }

        constexpr auto setFileRange(const Range &LocRange) noexcept
            -> decltype(*this)
        {
            this->FileRange = LocRange;
            return *this;
        }

        constexpr auto setMemoryRange(const Range &LocRange) noexcept
            -> decltype(*this)
        {
            this->MemoryRange = LocRange;
            return *this;
        }

        constexpr auto setFlags(const SegmentSectionFlags Flags) noexcept
            -> decltype(*this)
        {
            this->Flags = Flags;
            return *this;
        }

        constexpr auto setReserved1(const uint32_t Num) noexcept
            -> decltype(*this)
        {
            this->Reserved1 = Num;
            return *this;
        }

        constexpr auto setReserved2(const uint32_t Num) noexcept
            -> decltype(*this)
        {
            this->Reserved2 = Num;
            return *this;
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]] inline auto getData(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + this->getFileRange().getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline auto getData(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + this->getFileRange().getBegin());
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]]
        inline auto getDataEnd(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(
                Map + this->getFileRange().getEnd().value());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline auto getDataEnd(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(
                Map + this->getFileRange().getEnd().value());
        }
    };

    struct SegmentInfo {
    protected:
        std::string Name;

        Range FileRange;
        Range MemoryRange;

        MemoryProtections InitProt;
        MemoryProtections MaxProt;

        SegmentFlags Flags;
        std::vector<std::unique_ptr<SectionInfo>> SectionList;
    public:
        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getFileRange() const noexcept {
            return this->FileRange;
        }

        [[nodiscard]] constexpr auto getMemoryRange() const noexcept {
            return this->MemoryRange;
        }

        [[nodiscard]] constexpr auto getInitProt() const noexcept {
            return this->InitProt;
        }

        [[nodiscard]] constexpr auto getMaxProt() const noexcept {
            return this->MaxProt;
        }

        [[nodiscard]] constexpr auto getFlags() const noexcept {
            return this->Flags;
        }

        [[nodiscard]] constexpr auto &getSectionList() const noexcept {
            return this->SectionList;
        }

        [[nodiscard]] constexpr auto &getSectionListRef() noexcept {
            return this->SectionList;
        }

        constexpr auto setName(std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto setFileRange(Range LocRange) noexcept
            -> decltype(*this)
        {
            this->FileRange = LocRange;
            return *this;
        }

        constexpr auto setMemoryRange(Range LocRange) noexcept
            -> decltype(*this)
        {
            this->MemoryRange = LocRange;
            return *this;
        }

        constexpr auto setInitProt(MemoryProtections Prot) noexcept
            -> decltype(*this)
        {
            this->InitProt = Prot;
            return *this;
        }

        constexpr auto setMaxProt(MemoryProtections Prot) noexcept
            -> decltype(*this)
        {
            this->MaxProt = Prot;
            return *this;
        }

        constexpr auto setFlags(SegmentFlags Flags) noexcept
            -> decltype(*this)
        {
            this->Flags = Flags;
            return *this;
        }

        [[nodiscard]] auto
        FindSectionWithName(std::string_view Name) const noexcept
            -> const SectionInfo *;

        [[nodiscard]] auto
        FindSectionContainingAddress(uint64_t Address) const noexcept
            -> const SectionInfo *;

        [[nodiscard]] auto
        FindSectionContainingRelativeAddress(uint64_t Address) const noexcept
            -> const SectionInfo *;

        [[nodiscard]]
        inline auto getSectionAtOrdinal(const uint64_t Ordinal) const noexcept
            -> const SectionInfo *
        {
            const auto SectionIndex = OrdinalToIndex(Ordinal);
            if (IndexOutOfBounds(SectionIndex, SectionList.size())) {
                return nullptr;
            }

            return getSectionList().at(SectionIndex).get();
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]] inline auto getData(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + this->getFileRange().getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline auto getData(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + this->getFileRange().getBegin());
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]]
        inline auto getDataEnd(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(
                Map + this->getFileRange().getEnd().value());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline auto getDataEnd(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(
                Map + this->getFileRange().getEnd().value());
        }
    };
}
