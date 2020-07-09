//
//  src/ADT/Mach-O/LoadCommandsCommon.cpp
//  stool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "LoadCommandsCommon.h"

namespace MachO {
    NakedOpcodeList::IteratorInfo::IteratorInfo(const uint8_t *Iter,
                                                const uint8_t *End) noexcept
    : Iter(Iter), End(End) {}

    NakedOpcodeList::Iterator::Iterator(const uint8_t *Iter,
                                        const uint8_t *End) noexcept
    : Info(Iter, End) {}

    NakedOpcodeList::Iterator NakedOpcodeList::begin() const noexcept {
        return Iterator(Begin, End);
    }

    NakedOpcodeList::EndIterator NakedOpcodeList::end() const noexcept {
        return EndIterator();
    }

    std::optional<std::string_view>
    NakedOpcodeList::IteratorInfo::ParseString() noexcept {
        const auto Ptr = reinterpret_cast<const char *>(Iter);
        const auto Length = strnlen(Ptr, End - Iter);

        Iter += (Length + 1);
        if (Iter >= End) {
            return std::nullopt;
        }

        return std::string_view(Ptr, Length);
    }
}
