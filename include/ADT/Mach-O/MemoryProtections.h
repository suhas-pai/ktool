//
//  ADT/Mach-O/MemoryProtections.h
//  ktool
//
//  Created by Suhas Pai on 8/25/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include "ADT/BasicMasksHandler.h"

namespace MachO {
    enum class MemoryProtectionEnum : uint32_t {
        None,
        Read    = (1ull << 0),
        Write   = (1ull << 1),
        Execute = (1ull << 2)
    };

    [[nodiscard]] constexpr auto
    MemoryProtectionGetName(const MemoryProtectionEnum Val) noexcept
        -> std::optional<std::string_view>
    {
        switch (Val) {
            using Enum = MemoryProtectionEnum;
            case Enum::None:
                return "VM_PROT_NONE";
            case Enum::Read:
                return "VM_PROT_READ";
            case Enum::Write:
                return "VM_PROT_WRITE";
            case Enum::Execute:
                return "VM_PROT_EXECUTE";
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    MemoryProtectionGetDesc(const MemoryProtectionEnum Val) noexcept
        -> std::optional<std::string_view>
    {
        switch (Val) {
            using Enum = MemoryProtectionEnum;
            case Enum::None:
                return "None";
            case Enum::Read:
                return "Read";
            case Enum::Write:
                return "Write";
            case Enum::Execute:
                return "Execute";
        }

        return std::nullopt;
    }

    struct MemoryProtections : public ::BasicFlags<MemoryProtectionEnum> {
    public:
        using Masks = MemoryProtectionEnum;
        using MaskIntegerType = std::underlying_type_t<MemoryProtectionEnum>;

        constexpr MemoryProtections() noexcept = default;
        constexpr MemoryProtections(MaskIntegerType Integer) noexcept
        : ::BasicFlags<MemoryProtectionEnum>(Integer) {}

        [[nodiscard]] constexpr auto isReadable() const noexcept {
            return this->hasFlag(Masks::Read);
        }

        [[nodiscard]] constexpr auto isWritable() const noexcept {
            return this->hasFlag(Masks::Write);
        }

        [[nodiscard]] constexpr auto isExecutable() const noexcept {
            return this->hasFlag(Masks::Execute);
        }

        constexpr auto setReadable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Masks::Read, Value);
            return *this;
        }

        constexpr auto setWritable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Masks::Write, Value);
            return *this;
        }

        constexpr auto setExecutable(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Masks::Execute, Value);
            return *this;
        }
    };
}
