//
//  Utils/SwapRanges.h
//  ktool
//
//  Created by Suhas Pai on 6/2/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include "ADT/ByteVector.h"

void
SwapRanges(void *LhsBegin,
           void *LhsEnd,
           void *RhsBegin,
           void *RhsEnd,
           void *End,
           ByteVector &Buffer) noexcept;
