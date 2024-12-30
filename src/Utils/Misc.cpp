//
//  Utils/Misc.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <array>
#include <string>

namespace Utils {
    auto GetHumanReadableTimestamp(const time_t Timestamp) noexcept
        -> std::string
    {
        auto Buffer = std::array<char, 64>();
        strftime(Buffer.data(), Buffer.size(), "%c", localtime(&Timestamp));

        const auto Length = strnlen(Buffer.data(), Buffer.size());
        return std::string(Buffer.data(), Length);
    }
}
