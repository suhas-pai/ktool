//
//  ADT/PointerOrError.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include <cstdint>
#include <type_traits>

namespace ADT {
    template <typename PointerType, typename Enum>
    struct PointerOrError {
        static_assert(std::is_enum_v<Enum>, "Enum type must be an enum");
    protected:
        union {
            PointerType *Ptr;
            uint64_t Data;
        };
    public:
        constexpr PointerOrError(PointerType *Ptr) noexcept : Ptr(Ptr) {}
        constexpr PointerOrError(Enum E) noexcept
        : Data(static_cast<uint64_t>(E) << 1 | 1) {}

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return (Data & 1);
        }

        [[nodiscard]] constexpr auto getError() const noexcept {
            const auto Result =
                hasError() ?
                    static_cast<Enum>(Data >> 1) : static_cast<Enum>(0);

            return Result;
        }

        [[nodiscard]] constexpr auto getPtr() const noexcept {
            return !hasError() ? Ptr : nullptr;
        }
    };
}
