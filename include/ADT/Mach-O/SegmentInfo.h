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
        const SegmentInfo *Segment;
        std::string Name;

        LocationRange File;
        LocationRange Memory;

        MachO::SegmentSectionFlags Flags;

        uint32_t Reserved1;
        uint32_t Reserved2;

        [[nodiscard]]
        inline MachO::SegmentSectionKind getKind() const noexcept {
            return Flags.getKind();
        }

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getData(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getData(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getBegin());
        }

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getDataEnd(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getEnd());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getDataEnd(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getEnd());
        }
    };

    struct SegmentInfo {
        std::string Name;

        LocationRange File;
        LocationRange Memory;

        MemoryProtections InitProt;
        MemoryProtections MaxProt;

        MachO::SegmentFlags Flags;
        std::vector<std::unique_ptr<SectionInfo>> SectionList;

        [[nodiscard]] const SectionInfo *
        FindSectionWithName(const std::string_view &Name) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionContainingAddress(uint64_t Address) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionContainingRelativeAddress(uint64_t Address) const noexcept;

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getData(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getBegin());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getData(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getBegin());
        }

        template <typename T = uint8_t,
                  typename = std::enable_if_t<!std::is_const_v<T>>>

        [[nodiscard]] inline T *getDataEnd(uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getEnd());
        }

        template <typename T = const uint8_t>
        [[nodiscard]] inline T *getDataEnd(const uint8_t *Map) const noexcept {
            return reinterpret_cast<T *>(Map + File.getEnd());
        }
    };
}
