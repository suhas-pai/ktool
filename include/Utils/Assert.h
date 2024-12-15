//
//  Utils/Assert.h
//  ktool
//
//  Created by suhaspai on 12/14/24.
//

#pragma once
#include <assert.h>

#if defined(NDEBUG) && NDEBUG
    #undef assert
    #define assert(x) ({ (void)(x); __builtin_unreachable(); })
#endif /* !defined(assert) */
