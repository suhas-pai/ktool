//
//  include/Utils/Macros.h
//  stool
//
//  Created by Suhas Pai on 4/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#define LENGTH_OF(Str) (sizeof(Str) - 1)

#define TO_STRING_IMPL(Tok) #Tok
#define TO_STRING(Tok) TO_STRING_IMPL(Tok)
