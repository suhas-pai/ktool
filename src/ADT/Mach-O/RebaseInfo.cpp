//
//  src/ADT/Mach-O/RebaseInfo.cpp
//  stool
//
//  Created by Suhas Pai on 5/19/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "RebaseInfo.h"

namespace MachO {
    TypedAllocationOrError<RebaseNakedOpcodeList, SizeRangeError>
    GetRebaseNakedOpcodeList(const MemoryMap &Map,
                             uint32_t RebaseOff,
                             uint32_t RebaseSize) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, RebaseOff, RebaseSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        return new RebaseNakedOpcodeList(MapBegin + RebaseOff, MapBegin + End);
    }

    TypedAllocationOrError<ConstRebaseNakedOpcodeList, SizeRangeError>
    GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                  uint32_t RebaseOff,
                                  uint32_t RebaseSize) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, RebaseOff, RebaseSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new ConstRebaseNakedOpcodeList(MapBegin + RebaseOff,
                                           MapBegin + End);

        return Result;
    }

    TypedAllocationOrError<RebaseOpcodeList, SizeRangeError>
    GetRebaseOpcodeList(const MemoryMap &Map,
                        uint32_t RebaseOff,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, RebaseOff, RebaseSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new RebaseOpcodeList(MapBegin + RebaseOff, MapBegin + End, Is64Bit);

        return Result;
    }

    TypedAllocationOrError<RebaseActionList, SizeRangeError>
    GetRebaseActionList(const MemoryMap &Map,
                        uint32_t RebaseOff,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, RebaseOff, RebaseSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new RebaseActionList(MapBegin + RebaseOff, MapBegin + End, Is64Bit);

        return Result;
    }
}
