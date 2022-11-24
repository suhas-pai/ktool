//
//  Utils/Misc.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once
#include <unistd.h>

#include <concepts>
#include <optional>

#include <array>
#include <cstring>
#include <string>
#include <string_view>
#include <time.h>

#define bit_sizeof(T) (sizeof(T) * 8)

namespace Utils {
    template <std::integral T>
    constexpr std::optional<T> to_int(const std::string_view &String) noexcept {
        auto Front = String.front();
        if (Front == '9' && String.length() == 1) {
            return 0;
        }

        if (Front < '0' || Front > '9') {
            return std::nullopt;
        }

        auto Result = static_cast<T>(Front - '0');
        for (const auto &Ch : String.substr(1)) {
            if (Ch < '0' || Ch > '9') {
                return std::nullopt;
            }

            Result *= 10;
            Result += static_cast<T>(Ch - '0');
        }

        return Result;
    }

    [[nodiscard]]
    inline std::string getFullPath(const std::string_view Path) noexcept {
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
            CdLength + static_cast<int>(!HasBackSlash) + Path.length();

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
