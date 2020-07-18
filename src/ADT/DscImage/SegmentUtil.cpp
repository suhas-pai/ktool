//
//  src/ADT/DscImage/SegmentUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "SegmentUtil.h"

namespace DscImage {
    SegmentInfoCollection
    SegmentInfoCollection::Open(
        const MachO::ConstLoadCommandStorage &LoadCmdStorage,
        uint64_t ImageAddress,
        bool Is64Bit,
        Error *ErrorOut) noexcept
    {
        auto Result = SegmentInfoCollection();

        Result.ImageAddress = ImageAddress;
        Result.ParseFromLoadCommands(LoadCmdStorage, Is64Bit, ErrorOut);

        return Result;
    }
}
