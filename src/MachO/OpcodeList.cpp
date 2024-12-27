/*
 * MachO/OpcodeList.cpp
 * © suhas pai
 */

#include <cstring>
#include "MachO/OpcodeList.h"

namespace MachO {
    auto
    OpcodeList::IteratorInfo::ParseString() noexcept
        -> std::optional<std::string_view>
    {
        const auto Ptr = reinterpret_cast<const char *>(this->Iter);
        const auto Length =
            strnlen(Ptr, static_cast<uint64_t>(this->End - this->Iter));

        this->Iter += Length + 1;
        if (this->Iter >= this->End) {
            return std::nullopt;
        }

        return std::string_view(Ptr, Length);
    }
}
