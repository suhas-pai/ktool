//
//  src/ADT/Mach-O/LoadCommandsCommon.cpp
//  ktool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>
#include "LoadCommandsCommon.h"

namespace MachO {
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
