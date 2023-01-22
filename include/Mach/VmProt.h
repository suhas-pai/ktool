//
//  Mach/VmProt.h
//  ktool
//
//  Created by suhaspai on 10/4/22.
//

#pragma once

#include <cstdint>
#include "ADT/FlagsBase.h"

namespace Mach {
    struct VmProt : public ADT::FlagsBase<uint32_t> {
    public:
        enum class Masks : uint32_t {
            Readable   = 1 << 0,
            Writable   = 1 << 1,
            Executable = 1 << 2
        };

        using ADT::FlagsBase<uint32_t>::FlagsBase;

        [[nodiscard]] constexpr auto readable() const noexcept {
            return has(Masks::Readable);
        }

        [[nodiscard]] constexpr auto writable() const noexcept {
            return has(Masks::Writable);
        }

        [[nodiscard]] constexpr auto executable() const noexcept {
            return has(Masks::Executable);
        }

        constexpr auto setReadable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Readable, 0, Value);
            return *this;
        }

        constexpr auto setWritable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Writable, 0, Value);
            return *this;
        }

        constexpr auto setExecutable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Executable, 0, Value);
            return *this;
        }
    };

#define MACH_VMPROT_FMT "%c%c%c"
#define MACH_VMPROT_FMT_ARGS(VMPROT) \
    (VMPROT).readable() ? 'R' : '-', \
    (VMPROT).writable() ? 'W' : '-', \
    (VMPROT).executable() ? 'X' : '-'
}
