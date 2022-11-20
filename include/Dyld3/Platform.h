//
//  Dyld3/Platform.h
//  ktool
//
//  Created by suhaspai on 10/13/22.
//

#pragma once

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
}
