//
//  ADT/PrintUtils.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include <charconv>
#include <cstdio>
#include <inttypes.h>

#define STRING_VIEW_FMT "%*s"
#define STRING_VIEW_FMT_ARGS(SV) static_cast<int>((SV).length()), (SV).data()

#define ADDRESS_32_FMT "0x%08" PRIX32
#define ADDRESS_64_FMT "0x%016" PRIX64

#define ADDR_RANGE_32_FMT ADDRESS_32_FMT " - " ADDRESS_32_FMT
#define ADDR_RANGE_64_FMT ADDRESS_64_FMT " - " ADDRESS_64_FMT

#define ADDR_RANGE_FMT_ARGS(BEGIN, END) BEGIN, END
