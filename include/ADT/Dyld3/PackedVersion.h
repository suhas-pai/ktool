//
//  ADT/Dyld3/PackedVersion.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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

        [[nodiscard]] constexpr auto getRevision() const noexcept -> uint8_t {
            return this->getValueForMaskAndShift(Masks::Revision,
                                                 Shifts::Revision);
        }

        [[nodiscard]] constexpr auto getMinor() const noexcept -> uint8_t {
            return this->getValueForMaskAndShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr auto getMajor() const noexcept -> uint16_t {
            return this->getValueForMaskAndShift(Masks::Major, Shifts::Major);
        }

        constexpr auto setRevision(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Revision,
                                          Shifts::Revision,
                                          Value);
            return *this;
        }

        constexpr auto setMinor(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr auto setMajor(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Major, Shifts::Major, Value);
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

        [[nodiscard]] constexpr auto getRevision3() const noexcept
            -> uint16_t
        {
            return this->getValueForMaskAndShift(Masks::Revision3,
                                                 Shifts::Revision3);
        }

        [[nodiscard]] constexpr auto getRevision2() const noexcept
            -> uint16_t
        {
            return this->getValueForMaskAndShift(Masks::Revision2,
                                                 Shifts::Revision2);
        }

        [[nodiscard]] constexpr auto getRevision1() const noexcept
            -> uint16_t
        {
            return this->getValueForMaskAndShift(Masks::Revision1,
                                                 Shifts::Revision1);
        }

        [[nodiscard]] constexpr auto getMinor() const noexcept
            -> uint16_t
        {
            return this->getValueForMaskAndShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr auto getMajor() const noexcept
            -> uint32_t
        {
            const auto Value =
                this->getValueForMaskAndShift(Masks::Major, Shifts::Major);

            return static_cast<uint32_t>(Value);
        }

        constexpr auto setRevision3(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Revision3,
                                          Shifts::Revision3,
                                          Value);
            return *this;
        }

        constexpr auto setRevision2(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Revision2,
                                          Shifts::Revision2,
                                          Value);
            return *this;
        }

        constexpr auto setRevision1(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Revision1,
                                          Shifts::Revision1,
                                          Value);
            return *this;
        }

        constexpr auto setMinor(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr auto setMajor(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskAndShift(Masks::Major, Shifts::Major, Value);
            return *this;
        }
    };

}