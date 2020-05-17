//
//  include/ADT/Mach-O/SegmentInfo.h
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>
#include "ADT/LocationRange.h"

namespace MachO {
    struct SectionInfo {
        std::string Name;

        LocationRange File;
        LocationRange Memory;
    };

    struct SegmentInfo {
        std::string Name;

        LocationRange File;
        LocationRange Memory;

        std::vector<SectionInfo> SectionList;
    };
}
