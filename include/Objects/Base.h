//
//  Objects/Base.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "ADT/MemoryMap.h"
#include "ADT/PointerOrError.h"

#include "Kind.h"

namespace Objects {
    struct Base {
    private:
        Objects::Kind Kind = Kind::None;
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

    struct OpenError {
        Objects::Kind Kind : 8 = Kind::None;
        uint32_t Error = 0;

        constexpr OpenError() noexcept = default;
        constexpr OpenError(const enum Kind Kind, const uint32_t Error) noexcept
        : Kind(Kind), Error(Error) {
            assert(Error != 0);
        }

        [[nodiscard]] constexpr auto isNone() const noexcept {
            return Error == 0;
        }

        [[nodiscard]] constexpr auto isUnrecognizedFormat() const noexcept {
            return Kind == Kind::None && Error == 1;
        }
    } __attribute__((packed));

    constexpr static auto OpenErrorNone = OpenError();
    constexpr static auto OpenErrorUnrecognized = OpenError(Kind::None, 1);

    using OpenResult = typename ADT::PointerOrError<Base, OpenError>;
    auto Open(const ADT::MemoryMap &Map) noexcept -> OpenResult;

    auto
    OpenFrom(const ADT::MemoryMap &Map, const Kind FromKind) noexcept
        -> OpenResult;
}
