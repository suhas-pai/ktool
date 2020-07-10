//
//  include/ADT/Mach-O/SharedLibraryInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <string>
#include "LoadCommands.h"

namespace MachO {
    struct SharedLibraryInfo {
        LoadCommand::Kind Kind;
        std::string Path;

        uint32_t Index;
        uint32_t Timestamp;

        PackedVersion CurrentVersion;
        PackedVersion CompatibilityVersion;
    };
}
