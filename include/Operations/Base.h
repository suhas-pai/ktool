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
    private:
        Kind Kind;
    protected:
        explicit Base(const enum Kind Kind) noexcept : Kind(Kind) {}
    public:
        virtual ~Base() noexcept {}

        [[nodiscard]] constexpr auto kind() const noexcept {
            return Kind;
        }

        virtual
        auto supportsObjectKind(Objects::Kind Kind) const noexcept -> bool = 0;
    };
}
