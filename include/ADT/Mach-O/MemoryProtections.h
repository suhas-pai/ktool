//
//  include/ADT/Mach-O/MemoryProtections.h
//  ktool
//
//  Created by Suhas Pai on 8/25/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
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

    [[nodiscard]] constexpr std::string_view
    MemoryProtectionGetName(const MemoryProtectionEnum Val) noexcept {
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

        return std::string_view();
    }

    [[nodiscard]] constexpr std::string_view
    MemoryProtectionGetDesc(const MemoryProtectionEnum Val) noexcept {
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

        return std::string_view();
    }

    struct MemoryProtections : public ::BasicFlags<MemoryProtectionEnum> {
    public:
        using Masks = MemoryProtectionEnum;
        using MaskIntegerType = std::underlying_type_t<MemoryProtectionEnum>;

        constexpr MemoryProtections() noexcept = default;
        constexpr MemoryProtections(MaskIntegerType Integer) noexcept
        : ::BasicFlags<MemoryProtectionEnum>(Integer) {}

        [[nodiscard]] constexpr bool isReadable() const noexcept {
            return hasFlag(Masks::Read);
        }

        [[nodiscard]] constexpr bool isWritable() const noexcept {
            return hasFlag(Masks::Write);
        }

        [[nodiscard]] constexpr bool isExecutable() const noexcept {
            return hasFlag(Masks::Execute);
        }

        constexpr
        MemoryProtections &setReadable(const bool Value = true) noexcept {
            setValueForFlag(Masks::Read, Value);
            return *this;
        }

        constexpr
        MemoryProtections &setWritable(const bool Value = true) noexcept {
            setValueForFlag(Masks::Write, Value);
            return *this;
        }

        constexpr
        MemoryProtections &setExecutable(const bool Value = true) noexcept {
            setValueForFlag(Masks::Execute, Value);
            return *this;
        }
    };
}
