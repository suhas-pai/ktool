//
//  ADT/PrintUtils.cpp
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <assert.h>
#include "ADT/PrintUtils.h"

namespace ADT {
    auto
    PadSpaces(FILE *const OutFile, const int SpaceAmount) noexcept -> int {
        assert(SpaceAmount >= 0);
        return fprintf(OutFile, "%" RIGHTPAD_FMT "s", SpaceAmount, "");
    }

    auto
    RightPadSpaces(FILE *const OutFile,
                   const int WrittenOut,
                   const int Total) -> int
    {
        assert(WrittenOut >= 0);
        assert(WrittenOut < Total);

        return PadSpaces(OutFile, Total - WrittenOut);
    }
}
