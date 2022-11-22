//
//  ADT/PrintUtils.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <array>
#include <assert.h>
#include <sstream>

#include "ADT/PrintUtils.h"

namespace ADT {
    template <typename T>
    static inline
    auto to_string_with_precision(const T Value, const int n = 3) noexcept {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << Value;
        return out.str();
    }

    constexpr static std::array<std::string_view, 9> FormatSizeNames = {
        "KiB",
        "MiB",
        "GiB",
        "TiB",
        "PiB",
        "EiB",
        "ZiB",
        "YiB",
        "BiB"
    };

    auto FormattedSize(const uint64_t Size) -> std::string {
        constexpr auto Base = 1024;
        auto ResultAmount = double(Size);

        if (ResultAmount < Base) {
            auto Result = std::string();

            Result.reserve(STR_LENGTH("1023 ") + STR_LENGTH("bytes"));
            Result.append(to_string_with_precision(ResultAmount));
            Result.append(" Bytes");

            return Result;
        }

        auto Index = uint32_t();

        ResultAmount /= Base;
        while (ResultAmount >= Base) {
            ResultAmount /= Base;
            Index++;
        };

        assert(Index < FormatSizeNames.size());

        const auto &Name = FormatSizeNames[Index];
        //fprintf(OutFile, "%s%.3f %s%s", Prefix, Result, Name.data(), Suffix);

        auto Result = std::string();

        Result.reserve(STR_LENGTH("1023.999 ") + 1 + Name.length());
        Result.append(to_string_with_precision(ResultAmount));
        Result.append(1, ' ');
        Result.append(Name);

        return Result;
    }

    auto
    PadSpaces(FILE *const OutFile, const int SpaceAmount) noexcept -> int {
        assert(SpaceAmount >= 0);
        return fprintf(OutFile, "%" RIGHTPAD_FMT "s", SpaceAmount, "");
    }

    auto
    RightPadSpaces(FILE *const OutFile,
                   const int WrittenOut,
                   const int Total) -> int
    {
        assert(WrittenOut >= 0);
        assert(WrittenOut < Total);

        return PadSpaces(OutFile, Total - WrittenOut);
    }
}
