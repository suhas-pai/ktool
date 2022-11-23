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

        [[nodiscard]] constexpr auto readable() const noexcept -> bool {
            return (Flags & static_cast<uint32_t>(Masks::Readable));
        }

        [[nodiscard]] constexpr auto writable() const noexcept -> bool {
            return (Flags & static_cast<uint32_t>(Masks::Writable));
        }

        [[nodiscard]] constexpr auto executable() const noexcept -> bool {
            return (Flags & static_cast<uint32_t>(Masks::Executable));
        }

        constexpr auto setReadable(const bool Value = true) noexcept {
            setForValue(Masks::Readable, Value);
            return *this;
        }

        constexpr auto setWritable(const bool Value = true) noexcept {
            setForValue(Masks::Writable, Value);
            return *this;
        }

        constexpr auto setExecutable(const bool Value = true) noexcept {
            setForValue(Masks::Executable, Value);
            return *this;
        }
    };

#define MACH_VMPROT_FMT "%c%c%c"
#define MACH_VMPROT_FMT_ARGS(VMPROT) \
    (VMPROT).readable() ? 'R' : '-', \
    (VMPROT).writable() ? 'W' : '-', \
    (VMPROT).executable() ? 'X' : '-'
}