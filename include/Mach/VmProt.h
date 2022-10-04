//
//  Mach/VmProt.h
//  ktool
//
//  Created by suhaspai on 10/4/22.
//

#pragma once

namespace Mach {
    struct VmProt {
    protected:
        uint32_t Prot;
    public:
        enum class Masks : uint32_t {
            Readable   = 1 << 0,
            Writable   = 1 << 1,
            Executable = 1 << 2
        };

        [[nodiscard]] constexpr auto isReadable() const noexcept -> bool {
            return (Prot & static_cast<uint32_t>(Masks::Readable));
        }

        [[nodiscard]] constexpr auto isWritable() const noexcept -> bool {
            return (Prot & static_cast<uint32_t>(Masks::Writable));
        }

        [[nodiscard]] constexpr auto isExecutable() const noexcept -> bool {
            return (Prot & static_cast<uint32_t>(Masks::Executable));
        }

        constexpr auto setReadable() noexcept {
            Prot |= static_cast<uint32_t>(Masks::Readable);
            return *this;
        }

        constexpr auto setWritable() noexcept {
            Prot |= static_cast<uint32_t>(Masks::Writable);
            return *this;
        }

        constexpr auto setExecutable() noexcept {
            Prot |= static_cast<uint32_t>(Masks::Executable);
            return *this;
        }
    };
}
