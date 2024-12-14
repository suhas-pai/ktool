//
//  Utils/Path.h
//  ktool
//
//  Created by Suhas Pai on 4/11/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <string>
#include <string_view>

struct PathUtil {
    [[nodiscard]]
    static inline auto isAbsolute(const std::string_view Path) noexcept {
        if (Path.empty()) {
            return false;
        }

        return Path.front() == '/';
    }

    [[nodiscard]] static auto MakeAbsolute(std::string_view Path) noexcept
        -> std::string;

    [[nodiscard]] static
    auto ConcatPaths(std::string_view Lhs, std::string_view Rhs) noexcept
        -> std::string;
};
