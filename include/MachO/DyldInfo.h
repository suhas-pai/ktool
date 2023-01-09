/*
 * include/MachO/DyldInfo.h
 * © suhas pai
 */

#pragma once
#include <cstdint>

namespace MachO {
    enum class DyldInfoByteMasks : uint8_t {
        Opcode    = 0xF0,
        Immediate = 0x0F
    };

    enum class DyldInfoByteShifts : uint8_t {
        Opcode    = 4,
        Immediate = 0
    };
}