//
//  MemoryProtections.h
//  ktool
//
//  Created by Suhas Pai on 8/25/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <string_view>
#include "BasicMasksHandler.h"

enum class MemoryProtectionsEnum : uint32_t {
    None,
    Read    = (1ull << 0),
    Write   = (1ull << 1),
    Execute = (1ull << 2)
};

template <MemoryProtectionsEnum>
struct MemoryProtectionsEnumInfo {};

using namespace std::literals;

template <>
struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::None> {
    constexpr static const auto Kind = MemoryProtectionsEnum::None;
    constexpr static const auto Name = "VM_PROT_NONE"sv;
    constexpr static const auto Description = "None"sv;
};

template <>
struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Read> {
    constexpr static const auto Kind = MemoryProtectionsEnum::Read;
    constexpr static const auto Name = "VM_PROT_READ"sv;
    constexpr static const auto Description = "Read"sv;
};

template <>
struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Write> {
    constexpr static const auto Kind = MemoryProtectionsEnum::Write;
    constexpr static const auto Name = "VM_PROT_WRITE"sv;
    constexpr static const auto Description = "Write"sv;
};

template <>
struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Execute> {
    constexpr static const auto Kind = MemoryProtectionsEnum::Execute;
    constexpr static const auto Name = "VM_PROT_EXECUTE"sv;
    constexpr static const auto Description = "Execute"sv;
};

struct MemoryProtections : public ::BasicFlags<MemoryProtectionsEnum> {
public:
    using Masks = MemoryProtectionsEnum;
    using MaskIntegerType = std::underlying_type_t<MemoryProtectionsEnum>;

    constexpr MemoryProtections() noexcept = default;
    constexpr MemoryProtections(MaskIntegerType Integer) noexcept
    : ::BasicFlags<MemoryProtectionsEnum>(Integer) {}

    [[nodiscard]] constexpr auto isReadable() const noexcept {
        return this->hasFlag(Masks::Read);
    }

    [[nodiscard]] constexpr auto isWritable() const noexcept {
        return this->hasFlag(Masks::Write);
    }

    [[nodiscard]] constexpr auto isExecutable() const noexcept {
        return this->hasFlag(Masks::Execute);
    }

    constexpr inline auto setReadable(bool Value = true) noexcept
        -> decltype(*this)
    {
        this->setValueForFlag(Masks::Read, Value);
        return *this;
    }

    constexpr inline auto setWritable(bool Value = true) noexcept
        -> decltype(*this)
    {
        this->setValueForFlag(Masks::Write, Value);
        return *this;
    }

    constexpr inline auto setExecutable(bool Value = true) noexcept
        -> decltype(*this)
    {
        this->setValueForFlag(Masks::Execute, Value);
        return *this;
    }
};
