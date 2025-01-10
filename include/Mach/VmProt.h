//
//  Mach/VmProt.h
//  ktool
//
//  Created by suhaspai on 10/4/22.
//

#pragma once
#include "ADT/FlagsBase.h"

namespace Mach {
    struct VmProt : public ADT::FlagsBase<uint32_t> {
    public:
        enum class Masks : uint32_t {
            Readable = 1 << 0,
            Writable = 1 << 1,
            Executable = 1 << 2
        };

        using ADT::FlagsBase<uint32_t>::FlagsBase;

        [[nodiscard]] constexpr auto readable() const noexcept {
            return this->has(Masks::Readable);
        }

        [[nodiscard]] constexpr auto writable() const noexcept {
            return this->has(Masks::Writable);
        }

        [[nodiscard]] constexpr auto executable() const noexcept {
            return this->has(Masks::Executable);
        }

        constexpr auto setReadable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::Readable, /*Shift=*/0, Value);
            return *this;
        }

        constexpr auto setWritable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::Writable, /*Shift=*/0, Value);
            return *this;
        }

        constexpr auto setExecutable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::Executable, /*Shift=*/0, Value);
            return *this;
        }
    };

    struct VmProtInitMax {
    protected:
        VmProt Init;
        VmProt Max;
    public:
        constexpr
        explicit VmProtInitMax(const VmProt Init, const VmProt Max) noexcept
        : Init(Init), Max(Max) {}

        [[nodiscard]] constexpr auto getInit() const noexcept {
            return this->Init;
        }

        [[nodiscard]] constexpr auto getMax() const noexcept {
            return this->Max;
        }

        constexpr auto setInit(const VmProt Init) noexcept -> decltype(*this) {
            this->Init = Init;
            return *this;
        }

        constexpr auto setMax(const VmProt Max) noexcept -> decltype(*this) {
            this->Max = Max;
            return *this;
        }
    };
}
