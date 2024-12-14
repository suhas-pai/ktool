//
//  ADT/Mach-O/BindInfo.cpp
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/BindInfo.h"

namespace MachO {
    auto
    GetBindNakedOpcodeList(const ConstMemoryMap &Map,
                           const uint32_t BindOffset,
                           const uint32_t BindSize) noexcept
        -> ExpectedAlloc<ConstBindNakedOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        return new BindNakedOpcodeList(MapBegin + BindOffset, MapBegin + End);
    }

    auto
    GetConstBindNakedOpcodeList(const ConstMemoryMap &Map,
                                const uint32_t BindOffset,
                                const uint32_t BindSize) noexcept
        -> ExpectedAlloc<ConstBindNakedOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new ConstBindNakedOpcodeList(MapBegin + BindOffset, MapBegin + End);

        return Result;
    }

    auto
    GetBindOpcodeList(const ConstMemoryMap &Map,
                      const uint32_t BindOffset,
                      const uint32_t BindSize,
                      const bool Is64Bit) noexcept
        -> ExpectedAlloc<BindOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new BindOpcodeList(MapBegin + BindOffset, MapBegin + End, Is64Bit);

        return Result;
    }

    auto
    GetConstBindOpcodeList(const ConstMemoryMap &Map,
                           const uint32_t BindOffset,
                           const uint32_t BindSize,
                           const bool Is64Bit) noexcept
        -> ExpectedAlloc<ConstBindOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new ConstBindOpcodeList(MapBegin + BindOffset,
                                    MapBegin + End,
                                    Is64Bit);

        return Result;
    }

    auto
    GetLazyBindOpcodeList(const ConstMemoryMap &Map,
                          const uint32_t BindOffset,
                          const uint32_t BindSize,
                          const bool Is64Bit) noexcept
        -> ExpectedAlloc<LazyBindOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new LazyBindOpcodeList(MapBegin + BindOffset,
                                   MapBegin + End,
                                   Is64Bit);

        return Result;
    }

    auto
    GetConstLazyBindOpcodeList(const ConstMemoryMap &Map,
                               const uint32_t BindOffset,
                               const uint32_t BindSize,
                               const bool Is64Bit) noexcept
        -> ExpectedAlloc<ConstLazyBindOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new ConstLazyBindOpcodeList(MapBegin + BindOffset,
                                        MapBegin + End,
                                        Is64Bit);

        return Result;
    }

    auto
    GetWeakBindOpcodeList(const ConstMemoryMap &Map,
                          const uint32_t BindOffset,
                          const uint32_t BindSize,
                          const bool Is64Bit) noexcept
        -> ExpectedAlloc<WeakBindOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new WeakBindOpcodeList(MapBegin + BindOffset,
                                   MapBegin + End,
                                   Is64Bit);

        return Result;
    }

    auto
    GetConstWeakBindOpcodeList(const ConstMemoryMap &Map,
                               const uint32_t BindOffset,
                               const uint32_t BindSize,
                               const bool Is64Bit) noexcept
        -> ExpectedAlloc<ConstWeakBindOpcodeList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new ConstWeakBindOpcodeList(MapBegin + BindOffset,
                                        MapBegin + End,
                                        Is64Bit);

        return Result;
    }

    auto
    GetBindActionList(const ConstMemoryMap &Map,
                      const SegmentInfoCollection &SegmentCollection,
                      const uint32_t BindOffset,
                      const uint32_t BindSize,
                      const bool Is64Bit) noexcept
        -> ExpectedAlloc<BindActionList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new BindActionList(MapBegin,
                               SegmentCollection,
                               MapBegin + BindOffset,
                               MapBegin + End,
                               Is64Bit);
        return Result;
    }

    auto
    GetLazyBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &SegmentCollection,
                          const uint32_t BindOffset,
                          const uint32_t BindSize,
                          const bool Is64Bit) noexcept
        -> ExpectedAlloc<LazyBindActionList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new LazyBindActionList(MapBegin,
                                   SegmentCollection,
                                   MapBegin + BindOffset,
                                   MapBegin + End,
                                   Is64Bit);
        return Result;
    }

    auto
    GetWeakBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &SegmentCollection,
                          const uint32_t BindOffset,
                          const uint32_t BindSize,
                          const bool Is64Bit) noexcept
        -> ExpectedAlloc<WeakBindActionList, SizeRangeError>
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, BindOffset, BindSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new WeakBindActionList(MapBegin,
                                   SegmentCollection,
                                   MapBegin + BindOffset,
                                   MapBegin + End,
                                   Is64Bit);
        return Result;
    }
}
