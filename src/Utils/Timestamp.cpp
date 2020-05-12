//
//  src/Utils/Timestamp.cpp
//  stool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <array>
#include <cstring>

#include "Timestamp.h"

std::string GetHumanReadableTimestamp(time_t Timestamp) noexcept {
    auto Buffer = std::array<char, 64>();
    strftime(Buffer.data(), Buffer.size(), "%c", localtime(&Timestamp));

    return std::string(Buffer.data(), strnlen(Buffer.data(), Buffer.size()));
}
