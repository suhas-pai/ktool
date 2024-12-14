//
//  Utils/Path.cpp
//  ktool
//
//  Created by Suhas Pai on 4/11/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cassert>
#include <cerrno>
#include <cstring>
#include <string_view>
#include <unistd.h>

#include "Utils/Path.h"

static auto Cd = std::string();

auto PathUtil::MakeAbsolute(const std::string_view Path) noexcept
    -> std::string
{
    if (isAbsolute(Path)) {
        return std::string(Path);
    }

    if (Cd.empty()) {
        const auto CdString = getcwd(nullptr, 0);
        if (CdString == nullptr) {
            fprintf(stderr,
                    "Failed to get current-directory. Error: \"%s\"\n",
                    strerror(errno));
            exit(1);
        }

        Cd = CdString;
    }

    return ConcatPaths(Cd, Path);
}

auto
PathUtil::ConcatPaths(const std::string_view Lhs,
                      const std::string_view Rhs) noexcept
    -> std::string
{
    assert(!Lhs.empty() && !Rhs.empty() && "Lhs or Rhs is empty");

    // Add one for the separator-slash
    auto NewPath = std::string();
    const auto NewPathLength = Lhs.length() + Rhs.length() + 1;

    NewPath.reserve(NewPathLength);

    auto LhsEnd = Lhs.cend();
    if (Lhs.back() == '/') {
        LhsEnd--;
    }

    auto RhsBegin = Rhs.cbegin();
    if (*RhsBegin == '/') {
        RhsBegin += 1;
    }

    NewPath.append(Lhs.cbegin(), LhsEnd);
    NewPath.append(1, '/');
    NewPath.append(RhsBegin, Rhs.cend());

    return NewPath;
}
