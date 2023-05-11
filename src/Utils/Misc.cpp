//
//  Utils/Misc.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <array>
#include "Utils/Misc.h"

namespace Utils {
    std::string GetHumanReadableTimestamp(const time_t Timestamp) noexcept {
        auto Buffer = std::array<char, 64>();
        strftime(Buffer.data(), Buffer.size(), "%c", localtime(&Timestamp));

        const auto Length = strnlen(Buffer.data(), Buffer.size());
        return std::string(Buffer.data(), Length);
    }
}
