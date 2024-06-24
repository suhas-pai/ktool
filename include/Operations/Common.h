/*
 * Operations/Common.h
 * © suhas pai
 */

#pragma once

#include <string_view>
#include "MachO/LibraryList.h"

namespace Operations {
    auto
    PrintDylibOrdinalPath(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          const MachO::LibraryList &List,
                          bool PrintPath,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept -> int;

    auto
    PrintDylibOrdinalInfo(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          const MachO::LibraryList &List,
                          bool PrintPath,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept -> int;
}