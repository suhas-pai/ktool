//
//  Objects/Kind.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once
#include <string_view>

namespace Objects {
    enum class Kind : uint8_t {
        None,
        MachO,
        FatMachO
    };

    [[nodiscard]] constexpr auto KindIsValid(const Kind Kind) noexcept {
        switch (Kind) {
            case Kind::None:
            case Kind::MachO:
            case Kind::FatMachO:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto KindGetString(const Kind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case Kind::None:
                assert(false && "Got Kind None in KindGetString()");
            case Kind::MachO:
                return "Mach-O";
            case Kind::FatMachO:
                return "Fat Mach-O";
        }

        assert(false && "Got unrecognized Object-Kind in KindGetString()");
    }
}
