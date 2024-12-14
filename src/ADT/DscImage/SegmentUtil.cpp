//
//  ADT/DscImage/SegmentUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/DscImage/SegmentUtil.h"

namespace DscImage {
    auto
    SegmentInfoCollection::Open(
        const uint64_t ImageAddress,
        const MachO::ConstLoadCommandStorage &LoadCmdStorage,
        const bool Is64Bit,
        Error *const ErrorOut) noexcept
            -> SegmentInfoCollection
    {
        auto Result = SegmentInfoCollection();

        Result.ImageAddress = ImageAddress;
        Result.ParseFromLoadCommands(LoadCmdStorage, Is64Bit, ErrorOut);

        return Result;
    }
}
