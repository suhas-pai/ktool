//
//  src/Utils/Path.cpp
//  stool
//
//  Created by Suhas Pai on 4/11/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cassert>
#include <cerrno>
#include <cstring>
#include <string_view>
#include <unistd.h>

#include "Path.h"

static auto Cd = std::string();

bool PathUtil::IsAbsolute(const std::string_view &Path) noexcept {
    return (Path.front() == '/');
}

std::string PathUtil::Absolutify(const std::string_view &Path) noexcept {
    if (IsAbsolute(Path)) {
        return std::string(Path);
    }

    if (Cd.empty()) {
        const auto CdString = getcwd(nullptr, 0);
        if (CdString == nullptr) {
            fprintf(stderr,
                    "Failed to get current-directory. Error: %s\n",
                    strerror(errno));
            exit(1);
        }

        Cd = CdString;
    }

    return ConcatPaths(Cd, Path);
}

std::string
PathUtil::ConcatPaths(const std::string_view &Lhs,
                      const std::string_view &Rhs) noexcept
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
