//
//  include/ADT/Mach-O/SegmentInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ADT/LocationRange.h"
#include "Concepts/Const.h"
#include "Utils/MiscTemplates.h"

#include "LoadCommandsCommon.h"

namespace MachO {
    struct SegmentInfo;
    struct SectionInfo {
    protected:
        const SegmentInfo *Segment;
        std::string Name;

        LocationRange FileRange;
        LocationRange MemoryRange;

        SegmentSectionFlags Flags;

        uint32_t Reserved1;
        uint32_t Reserved2;
    public:
        [[nodiscard]]
        constexpr const SegmentInfo *getSegment() const noexcept {
            return Segment;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept {
            return Name;
        }

        [[nodiscard]]
        constexpr const LocationRange &getFileRange() const noexcept {
            return FileRange;
        }

        [[nodiscard]]
        constexpr const LocationRange &getMemoryRange() const noexcept {
            return MemoryRange;
        }

        [[nodiscard]]
        constexpr SegmentSectionFlags getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]]
        constexpr SegmentSectionKind getKind() const noexcept {
            return getFlags().getKind();
        }

        [[nodiscard]] constexpr uint32_t getReserved1() const noexcept {
            return Reserved1;
        }

        [[nodiscard]] constexpr uint32_t getReserved2() const noexcept {
            return Reserved2;
        }

        constexpr
        SectionInfo &setSegment(const SegmentInfo *const Segment) noexcept {
            this->Segment = Segment;
            return *this;
        }

        constexpr SectionInfo &setName(const std::string &Name) noexcept {
            this->Name = Name;
            return *this;
        }

        constexpr
        SectionInfo &setFileRange(const LocationRange &LocRange) noexcept {
            this->FileRange = LocRange;
            return *this;
        }

        constexpr
        SectionInfo &setMemoryRange(const LocationRange &LocRange) noexcept {
            this->MemoryRange = LocRange;
            return *this;
        }

        constexpr
        SectionInfo &setFlags(const SegmentSectionFlags Flags) noexcept {
            this->Flags = Flags;
            return *this;
        }

        constexpr SectionInfo &setReserved1(const uint32_t Num) noexcept {
            this->Reserved1 = Num;
            return *this;
        }

        constexpr SectionInfo &setReserved2(const uint32_t Num) noexcept {
            this->Reserved2 = Num;
            return *this;
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]] inline T *getData(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline T *getData(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]] inline T *getDataEnd(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline T *getDataEnd(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }
    };

    struct SegmentInfo {
    protected:
        std::string Name;

        LocationRange FileRange;
        LocationRange MemoryRange;

        MemoryProtections InitProt;
        MemoryProtections MaxProt;

        SegmentFlags Flags;
        std::vector<std::unique_ptr<SectionInfo>> SectionList;
    public:
        [[nodiscard]] constexpr std::string_view getName() const noexcept {
            return Name;
        }

        [[nodiscard]] constexpr LocationRange getFileRange() const noexcept {
            return FileRange;
        }

        [[nodiscard]] constexpr LocationRange getMemoryRange() const noexcept {
            return MemoryRange;
        }

        [[nodiscard]] constexpr MemoryProtections getInitProt() const noexcept {
            return InitProt;
        }

        [[nodiscard]] constexpr MemoryProtections getMaxProt() const noexcept {
            return MaxProt;
        }

        [[nodiscard]] constexpr SegmentFlags getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr
        inline const decltype(SectionList) &getSectionList() const noexcept {
            return SectionList;
        }

        [[nodiscard]]
        constexpr decltype(SectionList) &getSectionListRef() noexcept {
            return SectionList;
        }

        constexpr SegmentInfo &setName(std::string_view Name) noexcept {
            this->Name = Name;
            return *this;
        }

        constexpr
        inline SegmentInfo &setFileRange(LocationRange LocRange) noexcept {
            this->FileRange = LocRange;
            return *this;
        }

        constexpr
        inline SegmentInfo &setMemoryRange(LocationRange LocRange) noexcept {
            this->MemoryRange = LocRange;
            return *this;
        }

        constexpr
        inline SegmentInfo &setInitProt(MemoryProtections Prot) noexcept {
            this->InitProt = Prot;
            return *this;
        }

        constexpr
        inline SegmentInfo &setMaxProt(MemoryProtections Prot) noexcept {
            this->MaxProt = Prot;
            return *this;
        }

        constexpr SegmentInfo &setFlags(SegmentFlags Flags) noexcept {
            this->Flags = Flags;
            return *this;
        }

        [[nodiscard]] const SectionInfo *
        FindSectionWithName(std::string_view Name) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionContainingAddress(uint64_t Address) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionContainingRelativeAddress(uint64_t Address) const noexcept;

        [[nodiscard]] inline const SectionInfo *
        getSectionAtOrdinal(const uint64_t Ordinal) const noexcept {
            const auto SectionIndex = OrdinalToIndex(Ordinal);
            if (IndexOutOfBounds(SectionIndex, SectionList.size())) {
                return nullptr;
            }

            return getSectionList().at(SectionIndex).get();
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]] inline T *getData(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline T *getData(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <Concepts::NotConst T = uint8_t>
        [[nodiscard]] inline T *getDataEnd(uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }

        template <typename T = const uint8_t>
        [[nodiscard]]
        inline T *getDataEnd(const uint8_t *const Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }
    };
}
