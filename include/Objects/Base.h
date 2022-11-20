//
//  Objects/Base.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "ADT/MemoryMap.h"
#include "Kind.h"

namespace Objects {
    struct Base {
    private:
        Kind Kind = Kind::None;
    public:
        explicit Base(const enum Kind Kind) noexcept : Kind(Kind) {
            assert(Kind != Kind::None);
        }

        explicit Base(const ADT::MemoryMap &Map) noexcept = delete;
        virtual ~Base() noexcept = 0;

        [[nodiscard]] constexpr auto kind() const noexcept {
            return Kind;
        }
    };

    auto Open(const ADT::MemoryMap &Map) noexcept -> Base *;
}
