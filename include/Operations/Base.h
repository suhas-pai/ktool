//
//  Operations/Base.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "Objects/Base.h"
#include "Kind.h"

namespace Operations {
    struct Base {
    public:
        virtual ~Base() noexcept {}
        virtual bool run(const Objects::Base &Base) const noexcept = 0;
    };
}
