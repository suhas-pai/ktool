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
        constexpr inline const SegmentInfo *getSegment() const noexcept {
            return Segment;
        }

        [[nodiscard]]
        constexpr inline const std::string &getName() const noexcept {
            return Name;
        }

        [[nodiscard]]
        constexpr inline const LocationRange &getFileRange() const noexcept {
            return FileRange;
        }

        [[nodiscard]]
        constexpr inline const LocationRange &getMemoryRange() const noexcept {
            return MemoryRange;
        }

        [[nodiscard]]
        constexpr inline SegmentSectionFlags getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]]
        constexpr inline SegmentSectionKind getKind() const noexcept {
            return Flags.getKind();
        }

        [[nodiscard]] constexpr inline uint32_t getReserved1() const noexcept {
            return Reserved1;
        }

        [[nodiscard]] constexpr inline uint32_t getReserved2() const noexcept {
            return Reserved2;
        }

        constexpr
        inline SectionInfo &setSegment(const SegmentInfo *Segment) noexcept {
            this->Segment = Segment;
            return *this;
        }

        constexpr
        inline SectionInfo &setName(const std::string &Name) noexcept {
            this->Name = Name;
            return *this;
        }

        constexpr inline
        SectionInfo &setFileRange(const LocationRange &LocRange) noexcept {
            this->FileRange = LocRange;
            return *this;
        }

        constexpr inline
        SectionInfo &setMemoryRange(const LocationRange &LocRange) noexcept {
            this->MemoryRange = LocRange;
            return *this;
        }

        constexpr inline
        SectionInfo &setFlags(SegmentSectionFlags Flags) noexcept {
            this->Flags = Flags;
            return *this;
        }

        constexpr inline SectionInfo &setReserved1(uint32_t Num) noexcept {
            this->Reserved1 = Num;
            return *this;
        }

        constexpr inline SectionInfo &setReserved2(uint32_t Num) noexcept {
            this->Reserved2 = Num;
            return *this;
        }

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getData(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getData(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getDataEnd(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getDataEnd(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }
    };

    struct SegmentInfo {
        using SectionListType = std::vector<std::unique_ptr<SectionInfo>>;
    protected:
        std::string Name;

        LocationRange FileRange;
        LocationRange MemoryRange;

        MemoryProtections InitProt;
        MemoryProtections MaxProt;

        SegmentFlags Flags;
        SectionListType SectionList;
    public:
        [[nodiscard]]
        constexpr inline const std::string &getName() const noexcept {
            return Name;
        }

        [[nodiscard]]
        constexpr inline const LocationRange &getFileRange() const noexcept {
            return FileRange;
        }

        [[nodiscard]]
        constexpr inline const LocationRange &getMemoryRange() const noexcept {
            return MemoryRange;
        }

        [[nodiscard]]
        constexpr inline const MemoryProtections &getInitProt() const noexcept {
            return InitProt;
        }

        [[nodiscard]]
        constexpr inline const MemoryProtections &getMaxProt() const noexcept {
            return MaxProt;
        }

        [[nodiscard]]
        constexpr inline const SegmentFlags &getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr
        inline const SectionListType &getSectionList() const noexcept {
            return SectionList;
        }

        constexpr
        inline SegmentInfo &setName(const std::string &Name) noexcept {
            this->Name = Name;
            return *this;
        }

        constexpr inline
        SegmentInfo &setFileRange(const LocationRange &LocRange) noexcept {
            this->FileRange = LocRange;
            return *this;
        }

        constexpr inline
        SegmentInfo &setMemoryRange(const LocationRange &LocRange) noexcept {
            this->MemoryRange = LocRange;
            return *this;
        }

        constexpr inline
        SegmentInfo &setInitProt(const MemoryProtections &Prot) noexcept {
            this->InitProt = Prot;
            return *this;
        }

        constexpr inline
        SegmentInfo &setMaxProt(const MemoryProtections &Prot) noexcept {
            this->MaxProt = Prot;
            return *this;
        }

        constexpr inline SegmentInfo &setFlags(SegmentFlags Flags) noexcept {
            this->Flags = Flags;
            return *this;
        }

        constexpr
        inline SegmentInfo &setSectionList(SectionListType &&List) noexcept {
            this->SectionList = std::move(List);
            return *this;
        }

        [[nodiscard]] const SectionInfo *
        FindSectionWithName(const std::string_view &Name) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionContainingAddress(uint64_t Address) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionContainingRelativeAddress(uint64_t Address) const noexcept;

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getData(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getData(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getBegin());
        }

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getDataEnd(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getDataEnd(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + getFileRange().getEnd());
        }
    };
}
