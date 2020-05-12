//
//  include/TypeTraits/Identity.h
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

namespace TypeTraits {
    template <typename T>
    struct Identity {
        using Type = T;
    };
}
