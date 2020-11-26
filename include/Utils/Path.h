//
//  include/Utils/Path.h
//  ktool
//
//  Created by Suhas Pai on 4/11/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string>
#include <string_view>

struct PathUtil {
    [[nodiscard]]
    static inline bool isAbsolute(std::string_view Path) noexcept {
        if (Path.empty()) {
            return false;
        }

        return (Path.front() == '/');
    }

    [[nodiscard]]
    static std::string MakeAbsolute(std::string_view Path) noexcept;

    [[nodiscard]] static std::string
    ConcatPaths(std::string_view Lhs, std::string_view Rhs) noexcept;
};
