//
//  Utils/Misc.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once

#include <cstring>
#include <string>
#include <unistd.h>

#include "Utils/Overflow.h"

#define bit_sizeof(T) (sizeof(T) * 8)
#define SizeOfField(Type, Field) (sizeof(((Type *)0)->Field))

#define STR_TO_ENUM_SWITCH_CASE(Enum, String, ToStringFunc) \
    case Enum: \
        if (String == ToStringFunc(Enum)) { \
            return Enum; \
        }\
        [[fallthrough]]

#define STR_TO_ENUM_LAST_SWITCH_CASE(Enum, String, ToStringFunc) \
    case Enum: \
        if (String == ToStringFunc(Enum)) { \
            return Enum; \
        }\

#define __VAR_CONCAT_IMPL(a, b) a ## b
#define VAR_CONCAT(a, b) __VAR_CONCAT_IMPL(a, b)

#define __VAR_CONCAT_3_IMPL(a, b, c) a##b##c
#define VAR_CONCAT_3(a, b, c) __VAR_CONCAT_3_IMPL(a, b, c)

#define TO_STRING_IMPL(Tok) #Tok
#define TO_STRING(Tok) TO_STRING_IMPL(Tok)

namespace Utils {
    [[nodiscard]]
    constexpr auto PointerSize(const bool Is64Bit) noexcept -> uint8_t {
        return Is64Bit ? 8 : 4;
    }

    [[nodiscard]]
    constexpr auto PointerLogSize(const bool Is64Bit) noexcept -> uint8_t {
        return Is64Bit ? 3 : 2;
    }

    template <bool Is64Bit>
    [[nodiscard]] constexpr auto PointerSize() noexcept -> uint8_t {
        return PointerSize(Is64Bit);
    }

    template <bool Is64Bit>
    [[nodiscard]] constexpr auto PointerLogSize() noexcept -> uint8_t {
        return PointerLogSize(Is64Bit);
    }

    template <bool Is64Bit>
    using PointerAddrConstType =
        std::conditional_t<Is64Bit, const uint64_t, const uint32_t>;

    template <bool Is64Bit, std::unsigned_integral T>
    constexpr auto IntegerIsPointerAligned(const T Value) noexcept {
        return (Value & (1 << (PointerLogSize<Is64Bit>() - 1))) == 0;
    }

    template <std::unsigned_integral T, std::unsigned_integral U>
    constexpr auto IndexOutOfBounds(const T Index, const U Bound) noexcept {
        return Index >= Bound;
    }

    template <std::unsigned_integral T, std::unsigned_integral U>
    constexpr auto OrdinalOutOfBounds(const T Ordinal, const U Bound) noexcept {
        return Ordinal == 0 || Ordinal > Bound;
    }

    template <std::unsigned_integral T>
    constexpr auto to_uint(const std::string_view String) noexcept
        -> std::optional<T>
    {
        auto Front = String.front();
        if (Front == '0') {
            if (String.length() == 1) {
                return 0;
            }

            // Number with leading 0
            return std::nullopt;
        }

        if (Front < '0' || Front > '9') {
            return std::nullopt;
        }

        auto Result = static_cast<T>(Front - '0');
        for (const auto &Ch : String.substr(1)) {
            if (Ch < '0' || Ch > '9') {
                return std::nullopt;
            }

            const auto NewResultOpt =
                Utils::MulAddAndCheckOverflow(Result, 10, T(Ch - '0'));

            if (!NewResultOpt.has_value()) {
                return std::nullopt;
            }

            Result = static_cast<T>(NewResultOpt.value());
        }

        return Result;
    }

    [[nodiscard]]
    inline auto getFullPath(const std::string_view Path) noexcept {
        if (Path.front() == '/') {
            return std::string(Path);
        }

        const auto CdString = getcwd(nullptr, 0);
        if (CdString == nullptr) {
            fprintf(stderr,
                    "Failed to get current-directory. Error: \"%s\"\n",
                    strerror(errno));
            exit(1);
        }

        auto String = std::string();
        auto CdLength = strlen(CdString);
        auto HasBackSlash = CdString[CdLength - 1] == '/';

        const auto ReserveLength =
            CdLength + static_cast<size_t>(!HasBackSlash) + Path.length();

        String.reserve(ReserveLength);
        String.append(CdString);

        if (!HasBackSlash) {
            String.append(1, '/');
        }

        String.append(Path);
        return String;
    }

    [[nodiscard]]
    std::string GetHumanReadableTimestamp(const time_t Timestamp) noexcept;
}
