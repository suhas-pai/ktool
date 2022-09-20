//
//  include/ADT/Dyld3/PlatformKind.h
//  ktool
//
//  Created by Suhas Pai on 9/20/22.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

namespace Dyld3 {
    enum class PlatformKind : uint32_t {
        macOS = 1,
        iOS,
        tvOS,
        watchOS,
        bridgeOS,
        iOSMac,
        macCatalyst = iOSMac,
        iOSSimulator,
        tvOSSimulator,
        watchOSSimulator,
        DriverKit
    };

    [[nodiscard]] constexpr std::string_view
    PlatformKindGetName(const PlatformKind Kind) noexcept {
        switch (Kind) {
            case PlatformKind::macOS:
                return "PLATFORM_KIND_MACOS";
            case PlatformKind::iOS:
                return "PLATFORM_KIND_IOS";
            case PlatformKind::tvOS:
                return "PLATFORM_TVOS";
            case PlatformKind::watchOS:
                return "PLATFORM_WATCHOS";
            case PlatformKind::bridgeOS:
                return "PLATFORM_BRDIGEOS";
            case PlatformKind::iOSMac:
                return "PLATFORM_IOSMAC";
            case PlatformKind::iOSSimulator:
                return "PLATFORM_IOSSIMULATOR";
            case PlatformKind::tvOSSimulator:
                return "PLATFORM_TVOSSIMULATOR";
            case PlatformKind::watchOSSimulator:
                return "PLATFORM_WATCHOSSIMULATOR";
            case PlatformKind::DriverKit:
                return "PLATFORM_DRIVERKIT";
        }

        return "<unrecognized>";
    }

    [[nodiscard]] constexpr std::string_view
    PlatformKindGetDescription(const PlatformKind Kind) noexcept {
        switch (Kind) {
            case PlatformKind::macOS:
                return "macOS";
            case PlatformKind::iOS:
                return "iOS";
            case PlatformKind::tvOS:
                return "tvOS";
            case PlatformKind::watchOS:
                return "watchOS";
            case PlatformKind::bridgeOS:
                return "bridgeOS";
            case PlatformKind::iOSMac:
                return "iOSMac";
            case PlatformKind::iOSSimulator:
                return "iOS Simulator";
            case PlatformKind::tvOSSimulator:
                return "tvOS Simulator";
            case PlatformKind::watchOSSimulator:
                return "watchOS Simulator";
            case PlatformKind::DriverKit:
                return "Driver";
        }

        return "<unrecognized>";
    }

}