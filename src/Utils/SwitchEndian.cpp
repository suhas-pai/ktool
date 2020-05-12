//
//  src/Utils/SwitchEndian.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "Utils/SwitchEndian.h"

uint16_t EndianSwitcherFuncs::SwitchEndian16(uint16_t Value) noexcept {
    Value = ((Value >> 8) & 0x00ff) | ((Value << 8) & 0xff00);
    return Value;
}

uint32_t EndianSwitcherFuncs::SwitchEndian32(uint32_t Value) noexcept {
    Value = ((Value >> 8) & 0x00ff00ff) | ((Value << 8) & 0xff00ff00);
    Value = ((Value >> 16) & 0x0000ffff) | ((Value << 16) & 0xffff0000);

    return Value;
}

uint64_t EndianSwitcherFuncs::SwitchEndian64(uint64_t Value) noexcept {
    Value =
        (Value & 0x00000000ffffffffULL) << 32 |
        (Value & 0xffffffff00000000ULL) >> 32;

    Value =
        (Value & 0x0000ffff0000ffffULL) << 16 |
        (Value & 0xffff0000ffff0000ULL) >> 16;

    Value =
        (Value & 0x00ff00ff00ff00ffULL) << 8 |
        (Value & 0xff00ff00ff00ff00ULL) >> 8;

    return Value;
}

