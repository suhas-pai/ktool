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
    [[nodiscard]] static bool IsAbsolute(const std::string_view &Path) noexcept;

    [[nodiscard]]
    static std::string Absolutify(const std::string_view &Path) noexcept;

    [[nodiscard]] static std::string
    ConcatPaths(const std::string_view &Lhs,
                const std::string_view &Rhs) noexcept;
};
