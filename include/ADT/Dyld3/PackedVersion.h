//
//  include/ADT/Dyld3/PackedVersion.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "ADT/BasicMasksHandler.h"

namespace Dyld3 {
    enum class PackedVersionMasks : uint32_t {
        Revision = 0xff,
        Minor = 0xff00,
        Major = 0xffff0000
    };

    enum class PackedVersionShifts : uint32_t {
        Revision,
        Minor = 8,
        Major = 16
    };

    struct PackedVersion :
        public
            BasicMasksAndShiftsHandler<PackedVersionMasks, PackedVersionShifts>
    {
    private:
        using Base =
            BasicMasksAndShiftsHandler<PackedVersionMasks, PackedVersionShifts>;
    public:
        constexpr PackedVersion() noexcept = default;
        constexpr PackedVersion(uint32_t Integer) noexcept : Base(Integer) {}

        using Masks = PackedVersionMasks;
        using Shifts = PackedVersionShifts;

        [[nodiscard]] constexpr uint8_t getRevision() const noexcept {
            return getValueForMaskAndShift(Masks::Revision, Shifts::Revision);
        }

        [[nodiscard]] constexpr uint8_t getMinor() const noexcept {
            return getValueForMaskAndShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr uint16_t getMajor() const noexcept {
            return getValueForMaskAndShift(Masks::Major, Shifts::Major);
        }

        constexpr PackedVersion &setRevision(const uint8_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision, Shifts::Revision, Value);
            return *this;
        }

        constexpr PackedVersion &setMinor(const uint8_t Value) noexcept {
            setValueForMaskAndShift(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr PackedVersion &setMajor(const uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Major, Shifts::Major, Value);
            return *this;
        }
    };

    enum class PackedVersion64Masks : uint64_t {
        Revision3 = 0x3ff,
        Revision2 = 0xffc00,
        Revision1 = 0x3ff00000,
        Minor = 0xffc0000000,
        Major = 0xffffff0000000000
    };

    enum class PackedVersion64Shifts : uint64_t {
        Revision3,
        Revision2 = 10,
        Revision1 = 20,
        Minor = 30,
        Major = 40
    };

    struct PackedVersion64 :
        public
            ::BasicMasksAndShiftsHandler<
                PackedVersion64Masks, PackedVersion64Shifts>
    {
    private:
        using Base =
            ::BasicMasksAndShiftsHandler<
                PackedVersion64Masks, PackedVersion64Shifts>;
    public:
        using Masks = PackedVersion64Masks;
        using Shifts = PackedVersion64Shifts;

        constexpr PackedVersion64() noexcept = default;
        constexpr PackedVersion64(uint64_t Integer) noexcept : Base(Integer) {}

        [[nodiscard]] constexpr uint16_t getRevision3() const noexcept {
            return getValueForMaskAndShift(Masks::Revision3, Shifts::Revision3);
        }

        [[nodiscard]] constexpr uint16_t getRevision2() const noexcept {
            return getValueForMaskAndShift(Masks::Revision2, Shifts::Revision2);
        }

        [[nodiscard]] constexpr uint16_t getRevision1() const noexcept {
            return getValueForMaskAndShift(Masks::Revision1, Shifts::Revision1);
        }

        [[nodiscard]] constexpr uint16_t getMinor() const noexcept {
            return getValueForMaskAndShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr uint32_t getMajor() const noexcept {
            const auto Value =
                getValueForMaskAndShift(Masks::Major, Shifts::Major);

            return static_cast<uint32_t>(Value);
        }

        constexpr
        PackedVersion64 &setRevision3(const uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision3, Shifts::Revision3, Value);
            return *this;
        }

        constexpr
        PackedVersion64 &setRevision2(const uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision2, Shifts::Revision2, Value);
            return *this;
        }

        constexpr
        PackedVersion64 &setRevision1(const uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Revision1, Shifts::Revision1, Value);
            return *this;
        }

        constexpr PackedVersion64 &setMinor(const uint16_t Value) noexcept {
            setValueForMaskAndShift(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr PackedVersion64 &setMajor(const uint32_t Value) noexcept {
            setValueForMaskAndShift(Masks::Major, Shifts::Major, Value);
            return *this;
        }
    };

}