//
//  MachO/Magic.h
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#pragma once

#include <string_view>
#include "Utils/Assert.h"

namespace MachO {
    enum class Magic : uint32_t {
        Default = 0xFEEDFACE,
        Swapped = 0xCEFADEFE,

        Default64 = 0xFEEDFACF,
        Swapped64 = 0xCEFADEFC,

        Fat = 0xCAFEBABE,
        SwappedFat = 0xBEBAFECA,

        Fat64 = 0xCAFEBABF,
        SwappedFat64 = 0xBFBAFECA
    };

    [[nodiscard]] constexpr auto MagicIsValid(const Magic Magic) noexcept {
        switch (Magic) {
            case Magic::Default:
            case Magic::Swapped:
            case Magic::Default64:
            case Magic::Swapped64:
            case Magic::Fat:
            case Magic::SwappedFat:
            case Magic::Fat64:
            case Magic::SwappedFat64:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto MagicIsThin(const Magic Magic) noexcept {
        switch (Magic) {
            case Magic::Default:
            case Magic::Swapped:
            case Magic::Default64:
            case Magic::Swapped64:
                return true;
            case Magic::Fat:
            case Magic::SwappedFat:
            case Magic::Fat64:
            case Magic::SwappedFat64:
                return false;
        }

        assert(false && "MachO::MagicIsThin() got unrecognized magic");
    }

    [[nodiscard]] constexpr auto MagicIsFat(const Magic Magic) noexcept {
        switch (Magic) {
            case Magic::Fat:
            case Magic::SwappedFat:
            case Magic::Fat64:
            case Magic::SwappedFat64:
                return true;
            case Magic::Default:
            case Magic::Swapped:
            case Magic::Default64:
            case Magic::Swapped64:
                return false;
        }

        assert(false && "MachO::MagicIsFat() got unrecognized magic");
    }

    [[nodiscard]] constexpr auto MagicIs64Bit(const Magic Magic) noexcept {
        switch (Magic) {
            case Magic::Default64:
            case Magic::Swapped64:
            case Magic::Fat64:
            case Magic::SwappedFat64:
                return true;
            case Magic::Default:
            case Magic::Swapped:
            case Magic::Fat:
            case Magic::SwappedFat:
                return false;
        }

        assert(false && "MachO::MagicIs64Bit() got unrecognized magic");
    }

    [[nodiscard]] constexpr auto MagicIsBigEndian(const Magic Magic) noexcept {
        switch (Magic) {
            case Magic::Swapped:
            case Magic::Swapped64:
            case Magic::SwappedFat:
            case Magic::SwappedFat64:
                return true;
            case Magic::Default:
            case Magic::Default64:
            case Magic::Fat:
            case Magic::Fat64:
                return false;
        }

        assert(false && "MachO::MagicIsBigEndian() got unrecognized magic");
    }

    [[nodiscard]]
    constexpr auto MagicGetString(const Magic Magic) noexcept
        -> std::string_view
    {
        switch (Magic) {
            case Magic::Default:
                return "MH_MAGIC";
            case Magic::Swapped:
                return "MH_CIGAM";
            case Magic::Default64:
                return "MH_MAGIC_64";
            case Magic::Swapped64:
                return "MH_CIGAM_64";
            case Magic::Fat:
                return "FAT_MAGIC";
            case Magic::SwappedFat:
                return "FAT_CIGAM";
            case Magic::Fat64:
                return "FAT_MAGIC_64";
            case Magic::SwappedFat64:
                return "FAT_CIGAM_64";
        }

        assert(false && "Called MachO::MagicGetString() with unknown Magic");
    }

    [[nodiscard]]
    constexpr auto MagicGetDesc(const Magic Magic) noexcept -> std::string_view
    {
        switch (Magic) {
            case Magic::Default:
                return "Default";
            case Magic::Swapped:
                return "Swapped";
            case Magic::Default64:
                return "Default (64-bit)";
            case Magic::Swapped64:
                return "Swapped (64-bit)";
            case Magic::Fat:
                return "Fat";
            case Magic::SwappedFat:
                return "Fat (Swapped)";
            case Magic::Fat64:
                return "Fat (64-bit)";
            case Magic::SwappedFat64:
                return "Fat (64-bit, Swapped)";
        }

        assert(false && "Called MachO::MagicGetDesc() with unknown Magic");
    }
}
