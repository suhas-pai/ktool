//
//  Dyld3/Platform.h
//  ktool
//
//  Created by suhaspai on 10/13/22.
//

#pragma once

#include <assert.h>
#include <string_view>

namespace Dyld3 {
    enum class Platform : uint32_t {
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

    [[nodiscard]]
    constexpr auto PlatformIsValid(const enum Platform Platform) noexcept {
        switch (Platform) {
            case Platform::macOS:
            case Platform::iOS:
            case Platform::tvOS:
            case Platform::watchOS:
            case Platform::bridgeOS:
            case Platform::macCatalyst:
            case Platform::iOSSimulator:
            case Platform::tvOSSimulator:
            case Platform::watchOSSimulator:
            case Platform::DriverKit:
                return true;
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto PlatformGetString(const enum Platform Platform) noexcept
        -> std::string_view
    {
        switch (Platform) {
            case Platform::macOS:
                return "PLATFORM_MACOS";
            case Platform::iOS:
                return "PLATFORM_IOS";
            case Platform::tvOS:
                return "PLATFORM_TVOS";
            case Platform::watchOS:
                return "PLATFORM_WATCHOS";
            case Platform::bridgeOS:
                return "PLATFORM_BRIDGEOS";
            case Platform::macCatalyst:
                return "PLATFORM_MACCATALYST";
            case Platform::iOSSimulator:
                return "PLATFORM_IOSSIMULATOR";
            case Platform::tvOSSimulator:
                return "PLATFORM_TVOSSIMULATOR";
            case Platform::watchOSSimulator:
                return "PLATFORM_WATCHOSSIMULATOR";
            case Platform::DriverKit:
                return "PLATFORM_DRIVERKIT";
        }

        assert(false &&
               "Dyld3::PlatformGetString() called with unknown Platform");
    }

    [[nodiscard]]
    constexpr auto PlatformGetDesc(const enum Platform Platform) noexcept
        -> std::string_view
    {
        switch (Platform) {
            case Platform::macOS:
                return "macOS";
            case Platform::iOS:
                return "iOS";
            case Platform::tvOS:
                return "tvOS";
            case Platform::watchOS:
                return "watchOS";
            case Platform::bridgeOS:
                return "bridgeOS";
            case Platform::macCatalyst:
                return "macCatalyst";
            case Platform::iOSSimulator:
                return "iOSSimulator";
            case Platform::tvOSSimulator:
                return "tvOSSimulator";
            case Platform::watchOSSimulator:
                return "watchOSSimulator";
            case Platform::DriverKit:
                return "DriverKit";
        }

        assert(false &&
               "Dyld3::PlatformGetDesc() called with unknown Platform");
    }
}
