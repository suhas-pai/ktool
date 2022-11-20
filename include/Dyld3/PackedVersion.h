//
//  Dyld3/PackedVersion.h
//  ktool
//
//  Created by suhaspai on 10/13/22.
//

#pragma once
#include "ADT/FlagsBase.h"

namespace Dyld3 {
    struct PackedVersion : public ADT::FlagsBase<uint32_t> {
    public:
        enum class Masks : uint32_t {
            Revision = 0xff,
            Minor = 0xff00,
            Major = 0xffff0000
        };

        enum class Shifts : uint32_t {
            Revision,
            Minor = 8,
            Major = 16
        };
    protected:
        using ADT::FlagsBase<uint32_t>::FlagsBase;
        
        [[nodiscard]] constexpr auto
        getForMaskShift(const Masks Mask, const Shifts Shift) const noexcept {
            const auto Value = (Flags & static_cast<uint32_t>(Mask));
            return (Value >> static_cast<uint32_t>(Shift));
        }

        [[nodiscard]] constexpr auto
        setWithMaskShift(const Masks Mask,
                         const Shifts Shift,
                         const uint32_t Value) noexcept
        {
            const auto MaskedValue = Value & static_cast<uint32_t>(Mask);
            Flags |= MaskedValue << static_cast<uint32_t>(Shift);

            return *this;
        }
    public:
        [[nodiscard]] constexpr auto getRevision() const noexcept {
            return getForMaskShift(Masks::Revision, Shifts::Revision);
        }

        [[nodiscard]] constexpr auto getMinor() const noexcept {
            return getForMaskShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr auto getMajor() const noexcept {
            return getForMaskShift(Masks::Major, Shifts::Major);
        }

        constexpr auto setRevision(const uint8_t Value) noexcept {
            return setWithMaskShift(Masks::Revision, Shifts::Revision, Value);
        }

        constexpr auto setMinor(const uint8_t Value) noexcept {
            return setWithMaskShift(Masks::Minor, Shifts::Minor, Value);
        }

        constexpr auto setMajor(const uint16_t Value) noexcept {
            return setWithMaskShift(Masks::Major, Shifts::Major, Value);
        }
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

        enum class Shifts : uint64_t {
            Revision3,
            Revision2 = 10,
            Revision1 = 20,
            Minor = 30,
            Major = 40
        };
    protected:
        [[nodiscard]] constexpr auto
        getForMaskShift(const Masks Mask, const Shifts Shift) const noexcept {
            const auto Value = (Flags & static_cast<uint32_t>(Mask));
            return (Value >> static_cast<uint32_t>(Shift));
        }

        [[nodiscard]] constexpr auto
        setWithMaskShift(const Masks Mask,
                         const Shifts Shift,
                         const uint32_t Value) noexcept
        {
            const auto MaskedValue = Value & static_cast<uint32_t>(Mask);
            Flags |= MaskedValue << static_cast<uint32_t>(Shift);

            return *this;
        }
    public:
        using ADT::FlagsBase<uint64_t>::FlagsBase;

        [[nodiscard]] constexpr auto getRevision3() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Revision3, Shifts::Revision3);
        }

        [[nodiscard]] constexpr auto getRevision2() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Revision2, Shifts::Revision2);
        }

        [[nodiscard]] constexpr auto getRevision1() const noexcept -> uint16_t {
            return getForMaskShift(Masks::Revision1, Shifts::Revision1);
        }

        [[nodiscard]] constexpr auto getMinor() const noexcept {
            return getForMaskShift(Masks::Minor, Shifts::Minor);
        }

        [[nodiscard]] constexpr auto getMajor() const noexcept {
            return getForMaskShift(Masks::Major, Shifts::Major);
        }

        constexpr auto setRevision3(const uint16_t Value) noexcept {
            return setWithMaskShift(Masks::Revision3, Shifts::Revision3, Value);
        }

        constexpr auto setRevision2(const uint16_t Value) noexcept {
            return setWithMaskShift(Masks::Revision2, Shifts::Revision2, Value);
        }

        constexpr auto setRevision1(const uint16_t Value) noexcept {
            return setWithMaskShift(Masks::Revision1, Shifts::Revision1, Value);
        }

        constexpr auto setMinor(const uint8_t Value) noexcept {
            return setWithMaskShift(Masks::Minor, Shifts::Minor, Value);
        }

        constexpr auto setMajor(const uint32_t Value) noexcept {
            return setWithMaskShift(Masks::Major, Shifts::Major, Value);
        }
    };
}
