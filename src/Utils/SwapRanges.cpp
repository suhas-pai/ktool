//
//  src/Utils/SwapRanges.cpp
//  ktool
//
//  Created by Suhas Pai on 6/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <algorithm>

#include "ADT/LocationRange.h"
#include "ADT/RelativeRange.h"
#include "SwapRanges.h"

void
SwapRanges(void *const ArgLhsBegin,
           void *const ArgLhsEnd,
           void *const ArgRhsBegin,
           void *const ArgRhsEnd,
           void *const ArgEnd,
           ByteVector &Buffer) noexcept
{
    const auto End = reinterpret_cast<uint8_t *>(ArgEnd);
    const auto LhsBegin = reinterpret_cast<uint8_t *>(ArgLhsBegin);
    const auto LhsEnd = reinterpret_cast<uint8_t *>(ArgLhsEnd);
    const auto RhsBegin = reinterpret_cast<uint8_t *>(ArgRhsBegin);
    const auto RhsEnd = reinterpret_cast<uint8_t *>(ArgRhsEnd);

    const auto LhsRange = LocationRange::CreateWithEnd(LhsBegin, LhsEnd);
    const auto RhsRange = LocationRange::CreateWithEnd(RhsBegin, RhsEnd);
    const auto FullRange = RelativeRange(End);

    assert(!LhsRange.overlaps(RhsRange));
    assert(FullRange.containsLocRange(LhsRange) &&
           FullRange.containsLocRange(RhsRange));

    const auto LhsSize = LhsRange.size();
    const auto RhsSize = RhsRange.size();

    if (LhsSize > RhsSize) {
        const auto Diff = (LhsSize - RhsSize);
        Buffer.Add(LhsBegin + RhsSize, Diff);

        std::swap_ranges(RhsBegin, RhsEnd, LhsBegin);
        std::move(LhsBegin + LhsSize, End, LhsBegin + RhsSize);
        std::move(RhsBegin + RhsSize - Diff,
                  End - Diff,
                  RhsBegin + LhsSize - Diff);

        std::copy(const_cast<const uint8_t *>(Buffer.data()),
                  Buffer.dataEnd(),
                  RhsBegin + RhsSize - Diff);
    } else if (LhsSize < RhsSize) {
        const auto Diff = (RhsSize - LhsSize);
        Buffer.Add(RhsBegin + LhsSize, Diff);

        std::swap_ranges(LhsBegin, LhsEnd, RhsBegin);
        std::move(RhsBegin + RhsSize, End, RhsBegin + LhsSize);
        std::move(LhsBegin + LhsSize, End, LhsBegin + RhsSize);
        std::copy(const_cast<const uint8_t *>(Buffer.data()),
                  Buffer.dataEnd(),
                  LhsBegin + LhsSize);
    } else {
        std::swap_ranges(LhsBegin, LhsEnd, RhsBegin);
    }

    Buffer.clear();
}
