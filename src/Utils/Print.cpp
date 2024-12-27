//
//  Utils/Print.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include "Utils/Print.h"

namespace Utils {
    auto
    PadSpaces(FILE *const OutFile, const uint32_t SpaceAmount) noexcept -> int {
        std::print(OutFile, "{:<{}}", "", SpaceAmount);
        return static_cast<int>(SpaceAmount);
    }

    auto
    RightPadSpaces(FILE *const OutFile,
                   const int WrittenOut,
                   const int Total) -> int
    {
        assert(WrittenOut >= 0);
        assert(WrittenOut <= Total);

        return PadSpaces(OutFile, static_cast<uint32_t>(Total - WrittenOut));
    }

    auto
    PrintMultTimes(FILE *const OutFile,
                   const std::string_view String,
                   const uint64_t Times,
                   const std::string_view Prefix,
                   const std::string_view Suffix) -> int
    {
        std::print(OutFile, "{}", Prefix);

        auto Result = int();
        for (auto I = uint64_t(); I != Times; I++) {
            std::print(OutFile, "{}", String);
        }

        std::print(OutFile, "{}", Suffix);
        return Result;
    }

    auto
    PrintDylibOrdinalPath(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const std::string_view DylibPath,
                          const bool PrintPath,
                          const bool IsOutOfBounds,
                          const std::string_view Prefix,
                          const std::string_view Suffix) noexcept -> int
    {
        if (DylibOrdinal == 0) {
            std::print(OutFile, "{}<invalid>{}", Prefix, Suffix);
            return static_cast<int>(9 + Prefix.length() + Suffix.length());
        }

        if (IsOutOfBounds) {
            std::print(OutFile, "{}<out-of-bounds!>{}", Prefix, Suffix);
            return static_cast<int>(16 + Prefix.length() + Suffix.length());
        }

        auto WrittenOut = int();
        if (PrintPath) {
            std::print(OutFile, "{}\"{}\"{}", Prefix, DylibPath, Suffix);
            WrittenOut +=
                DylibPath.length() + Prefix.length() + Suffix.length() +
                STR_LENGTH("\"\"");
        } else {
            std::print(OutFile, "{}", Suffix);
            WrittenOut += Suffix.length();
        }

        return WrittenOut;
    }

    auto
    PrintDylibOrdinalInfo(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const std::string_view DylibPath,
                          const bool PrintPath,
                          const bool IsOutOfBounds,
                          const std::string_view Prefix,
                          const std::string_view Suffix) noexcept -> int
    {
        auto WrittenOut = 15;
        std::print(OutFile, "{}Dylib-Ordinal {:02}", Prefix, DylibOrdinal);

        if (IsOutOfBounds) {
            std::print(OutFile, " (Out Of Bounds!){}", Suffix);
            return WrittenOut + static_cast<int>(Suffix.length()) + 18;
        }

        if (PrintPath) {
            std::print(OutFile, " - \"{}\"{}", DylibPath, Suffix);
            WrittenOut += DylibPath.length() + Suffix.length();
        } else {
            std::print(OutFile, "{}", Suffix);
            WrittenOut += Suffix.length();
        }

        return WrittenOut;
    }

    auto
    PrintOnlyOnce(FILE *const OutFile,
                  const std::string_view String,
                  bool &DidPrint) noexcept -> int
    {
        if (DidPrint) {
            return 0;
        }

        DidPrint = true;
        std::print(OutFile, "{}", String);

        return static_cast<int>(String.length());
    }

    auto
    PrintOnlyAfterFirst(FILE *const OutFile,
                        const std::string_view String,
                        bool &DidPassFirst) noexcept -> int
    {
        if (DidPassFirst) {
            std::print(OutFile, "{}", String);
            return static_cast<int>(String.length());
        }

        DidPassFirst = true;
        return 0;
    }
}
