//
//  ADT/Mach-O/RebaseInfo.cpp
//  ktool
//
//  Created by Suhas Pai on 5/19/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/RebaseInfo.h"

namespace MachO {
    auto
    GetRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                             const uint32_t RebaseOff,
                             const uint32_t RebaseSize) noexcept
        -> ExpectedAlloc<RebaseNakedOpcodeList, SizeRangeError>
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

    auto
    GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                  const uint32_t RebaseOff,
                                  const uint32_t RebaseSize) noexcept
        -> ExpectedAlloc<ConstRebaseNakedOpcodeList, SizeRangeError>
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

    auto
    GetRebaseOpcodeList(const ConstMemoryMap &Map,
                        const uint32_t RebaseOff,
                        const uint32_t RebaseSize,
                        const bool Is64Bit) noexcept
        -> ExpectedAlloc<RebaseOpcodeList, SizeRangeError>
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

    auto
    GetRebaseActionList(const ConstMemoryMap &Map,
                        uint32_t RebaseOff,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept
        -> ExpectedAlloc<RebaseActionList, SizeRangeError>
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
