//
//  Objects/Kind.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

namespace Objects {
    enum class Kind : uint8_t {
        None,
        MachO,
        FatMachO
    };
}
