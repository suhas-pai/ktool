//
//  src/ADT/Mach-O/BindInfo.cpp
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "BindInfo.h"

namespace MachO {
    TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
    GetBindNakedOpcodeList(const MemoryMap &Map,
                           uint32_t BindOffset,
                           uint32_t BindSize) noexcept
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

    TypedAllocationOrError<ConstBindNakedOpcodeList, SizeRangeError>
    GetConstBindNakedOpcodeList(const ConstMemoryMap &Map,
                                uint32_t BindOffset,
                                uint32_t BindSize) noexcept
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

    TypedAllocationOrError<BindOpcodeList, SizeRangeError>
    GetBindOpcodeList(const MemoryMap &Map,
                      uint32_t BindOffset,
                      uint32_t BindSize,
                      bool Is64Bit) noexcept
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

    TypedAllocationOrError<ConstBindOpcodeList, SizeRangeError>
    GetConstBindOpcodeList(const ConstMemoryMap &Map,
                           uint32_t BindOffset,
                           uint32_t BindSize,
                           bool Is64Bit) noexcept
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

    TypedAllocationOrError<LazyBindOpcodeList, SizeRangeError>
    GetLazyBindOpcodeList(const MemoryMap &Map,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept
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

    TypedAllocationOrError<ConstLazyBindOpcodeList, SizeRangeError>
    GetConstLazyBindOpcodeList(const ConstMemoryMap &Map,
                               uint32_t BindOffset,
                               uint32_t BindSize,
                               bool Is64Bit) noexcept
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

    TypedAllocationOrError<WeakBindOpcodeList, SizeRangeError>
    GetWeakBindOpcodeList(const MemoryMap &Map,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept
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

    TypedAllocationOrError<ConstWeakBindOpcodeList, SizeRangeError>
    GetConstWeakBindOpcodeList(const ConstMemoryMap &Map,
                               uint32_t BindOffset,
                               uint32_t BindSize,
                               bool Is64Bit) noexcept
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

    TypedAllocationOrError<BindActionList, SizeRangeError>
    GetBindActionList(const MemoryMap &Map,
                      const SegmentInfoCollection &SegmentCollection,
                      uint32_t BindOffset,
                      uint32_t BindSize,
                      bool Is64Bit) noexcept
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

    TypedAllocationOrError<LazyBindActionList, SizeRangeError>
    GetLazyBindActionList(const MemoryMap &Map,
                          const SegmentInfoCollection &SegmentCollection,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept
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

    TypedAllocationOrError<WeakBindActionList, SizeRangeError>
    GetWeakBindActionList(const MemoryMap &Map,
                          const SegmentInfoCollection &SegmentCollection,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept
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
