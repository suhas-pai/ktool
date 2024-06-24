//
//  Dyld3/PackedVersion.h
//  ktool
//
//  Created by suhaspai on 10/13/22.
//

#pragma once

#include <compare>
#include <cstdint>

#include "ADT/FlagsBase.h"

namespace Dyld3 {
    struct PackedVersion : public ADT::FlagsBase<uint32_t> {
    public:
        enum class Masks : uint32_t {
            Revision = 0xff,
            Minor = 0xff00,
            Major = 0xffff0000
        };

        enum class Shifts : uint8_t {
            Revision,
            Minor = 8,
            Major = 16
        };
    protected:
        using ADT::FlagsBase<uint32_t>::FlagsBase;

        [[nodiscard]] constexpr auto
        getForMaskShift(const Masks Mask, const Shifts Shift) const noexcept {
            const auto Value = Flags & static_cast<uint32_t>(Mask);
            return Value >> static_cast<uint8_t>(Shift);
        }
    public:
        [[nodiscard]] constexpr auto revision() const noexcept -> uint8_t {
            return getForMaskShift(Masks::Revision, Shifts::Revision);
        }

        [[nodiscard]] constexpr auto minor() const noexcept -> uint8_t {
            return getForMaskShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr auto major() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Major, Shifts::Major);
        }

        constexpr auto setRevision(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Revision, Shifts::Revision, Value);
            return *this;
        }

        constexpr auto setMinor(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr auto setMajor(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Major, Shifts::Major, Value);
            return *this;
        }

        [[nodiscard]] constexpr auto
        operator<=>(const Dyld3::PackedVersion &Rhs) const noexcept = default;
    };

    struct PackedVersion64 : public ADT::FlagsBase<uint64_t> {
    public:
        enum class Masks : uint64_t {
            Revision3 = 0x3ff,
            Revision2 = 0xffc00,
            Revision1 = 0x3ff00000,
            Minor = 0xffc0000000,
            Major = 0xffffff0000000000
        };

        enum class Shifts : uint8_t {
            Revision3,
            Revision2 = 10,
            Revision1 = 20,
            Minor = 30,
            Major = 40
        };
    protected:
        [[nodiscard]] constexpr auto
        getForMaskShift(const Masks Mask, const Shifts Shift) const noexcept {
            const auto Value = (Flags & static_cast<uint64_t>(Mask));
            return Value >> static_cast<uint8_t>(Shift);
        }

        [[nodiscard]] constexpr auto
        setWithMaskShift(const Masks Mask,
                         const Shifts Shift,
                         const uint32_t Value) noexcept
        {
            const auto MaskedValue = Value & static_cast<uint64_t>(Mask);
            Flags |= MaskedValue << static_cast<uint8_t>(Shift);

            return *this;
        }
    public:
        using ADT::FlagsBase<uint64_t>::FlagsBase;

        [[nodiscard]] constexpr auto revision3() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Revision3, Shifts::Revision3);
        }

        [[nodiscard]] constexpr auto revision2() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Revision2, Shifts::Revision2);
        }

        [[nodiscard]] constexpr auto revision1() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Revision1, Shifts::Revision1);
        }

        [[nodiscard]] constexpr auto minor() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr auto major() const noexcept -> uint32_t {
            return static_cast<uint32_t>(getForMaskShift(Masks::Major, Shifts::Major));
        }

        constexpr auto setRevision3(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Revision3, Shifts::Revision3, Value);
            return *this;
        }

        constexpr auto setRevision2(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Revision2, Shifts::Revision2, Value);
            return *this;
        }

        constexpr auto setRevision1(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Revision1, Shifts::Revision1, Value);
            return *this;
        }

        constexpr auto setMinor(const uint8_t Value) noexcept -> decltype(*this)
        {
            setValueForMask(Masks::Minor, Shifts::Minor, Value);
            return *this;
        }

        constexpr auto setMajor(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Major, Shifts::Major, Value);
            return *this;
        }

        [[nodiscard]] constexpr auto
        operator<=>(const Dyld3::PackedVersion64 &Rhs) const noexcept = default;
    };

#define DYLD3_PACKED_VERSION_FMT "%" PRIu16 ".%" PRIu8 ".%" PRIu8
#define DYLD3_PACKED_VERSION_FMT_ARGS(VERSION) \
    (VERSION).major(), \
    (VERSION).minor(), \
    (VERSION).revision()

#define DYLD3_PACKED_VERSION_64_FMT \
    "%" PRIu32 ".%" PRIu16 ".%" PRIu16 ".%" PRIu16 ".%" PRIu16
#define DYLD3_PACKED_VERSION_64_FMT_ARGS(VERSION) \
    (VERSION).major(), \
    (VERSION).minor(), \
    (VERSION).revision1(), \
    (VERSION).revision2(), \
    (VERSION).revision3()
}
