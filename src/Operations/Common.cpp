/*
 * Operations/Common.cpp
 * © suhas pai
 */

#include "Operations/Common.h"

#include "Utils/Misc.h"
#include "Utils/Print.h"

namespace Operations {
    auto
    PrintDylibOrdinalPath(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const MachO::LibraryList &List,
                          const bool PrintPath,
                          const std::string_view Prefix,
                          const std::string_view Suffix) noexcept -> int
    {
        auto LibraryPath = std::string_view("<Invalid>");
        auto IsOutOfBounds = true;

        if (!Utils::OrdinalOutOfBounds(DylibOrdinal, List.size())) {
            if (const auto &PathOpt = List.at(DylibOrdinal - 1).Path) {
                LibraryPath = PathOpt.value();
            } else {
                LibraryPath = "<Malformed>";
            }

            IsOutOfBounds = false;
        }

        const auto WrittenOut =
            Utils::PrintDylibOrdinalPath(OutFile,
                                         DylibOrdinal,
                                         LibraryPath,
                                         PrintPath,
                                         IsOutOfBounds,
                                         Prefix,
                                         Suffix);

        return WrittenOut;
    }

    auto
    PrintDylibOrdinalInfo(FILE *const OutFile,
                          const uint8_t DylibOrdinal,
                          const MachO::LibraryList &List,
                          const bool PrintPath,
                          const std::string_view Prefix,
                          const std::string_view Suffix) noexcept -> int
    {
        auto LibraryPath = std::string_view("<Invalid>");
        auto IsOutOfBounds = true;

        if (!Utils::OrdinalOutOfBounds(DylibOrdinal, List.size())) {
            const auto &PathOpt = List.at(DylibOrdinal - 1).Path;
            if (PathOpt.has_value()) {
                LibraryPath = PathOpt.value();
            } else {
                LibraryPath = "<Malformed>";
            }

            IsOutOfBounds = false;
        }

        const auto WrittenOut =
            Utils::PrintDylibOrdinalInfo(OutFile,
                                         DylibOrdinal,
                                         LibraryPath,
                                         PrintPath,
                                         IsOutOfBounds,
                                         Prefix,
                                         Suffix);

        return WrittenOut;
    }
}