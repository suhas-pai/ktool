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
    enum class MemoryProtectionsEnum : uint32_t {
        None,
        Read    = (1ull << 0),
        Write   = (1ull << 1),
        Execute = (1ull << 2)
    };

    template <MemoryProtectionsEnum>
    struct MemoryProtectionsEnumInfo {};

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::None> {
        constexpr static const auto Kind = MemoryProtectionsEnum::None;

        constexpr static const auto Name = std::string_view("VM_PROT_NONE");
        constexpr static const auto Description = std::string_view("None");
    };

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Read> {
        constexpr static const auto Kind = MemoryProtectionsEnum::Read;

        constexpr static const auto Name = std::string_view("VM_PROT_READ");
        constexpr static const auto Description = std::string_view("Write");
    };

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Write> {
        constexpr static const auto Kind = MemoryProtectionsEnum::Write;

        constexpr static const auto Name = std::string_view("VM_PROT_WRITE");
        constexpr static const auto Description = std::string_view("Write");
    };

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Execute> {
        constexpr static const auto Kind = MemoryProtectionsEnum::Execute;

        constexpr static const auto Name = std::string_view("VM_PROT_EXECUTE");
        constexpr static const auto Description = std::string_view("Execute");
    };

    struct MemoryProtections : public ::BasicFlags<MemoryProtectionsEnum> {
    public:
        using Masks = MemoryProtectionsEnum;
        using MaskIntegerType = std::underlying_type_t<MemoryProtectionsEnum>;

        constexpr MemoryProtections() noexcept = default;
        constexpr MemoryProtections(MaskIntegerType Integer) noexcept
        : ::BasicFlags<MemoryProtectionsEnum>(Integer) {}

        [[nodiscard]] constexpr inline bool IsReadable() const noexcept {
            return hasFlag(Masks::Read);
        }

        [[nodiscard]] constexpr inline bool IsWritable() const noexcept {
            return hasFlag(Masks::Write);
        }

        [[nodiscard]] constexpr inline bool IsExecutable() const noexcept {
            return hasFlag(Masks::Execute);
        }

        constexpr
        inline MemoryProtections &setReadable(bool Value = true) noexcept {
            setValueForFlag(Masks::Read, Value);
            return *this;
        }

        constexpr
        inline MemoryProtections &setWritable(bool Value = true) noexcept {
            setValueForFlag(Masks::Write, Value);
            return *this;
        }

        constexpr
        inline MemoryProtections &setExecutable(bool Value = true) noexcept {
            setValueForFlag(Masks::Execute, Value);
            return *this;
        }
    };
}
