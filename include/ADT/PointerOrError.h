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
    template <typename PointerType, typename ErrorType, typename T2=void>
    struct PointerOrError {};

    template <typename PointerType, typename ErrorType>
    struct PointerOrError<PointerType,
                          ErrorType,
                          std::enable_if_t<std::is_enum_v<ErrorType>>>
    {
        static_assert(sizeof(ErrorType) < sizeof(void *),
                      "ErrorType cannt be greater than 7 Bytes");
    protected:
        union {
            PointerType *Ptr;
            uint64_t Data;
        };
    public:
        constexpr PointerOrError(PointerType *const Ptr) noexcept : Ptr(Ptr) {}
        constexpr PointerOrError(const ErrorType E) noexcept
        : Data(static_cast<uint64_t>(E) << 1 | 1) {}

        constexpr PointerOrError(const std::nullptr_t &Nullptr) noexcept
            = delete;

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return (Data & 1);
        }

        [[nodiscard]] constexpr auto error() const noexcept {
            const auto Result =
                hasError() ?
                    static_cast<ErrorType>(Data >> 1) :
                    static_cast<ErrorType>(0);

            return Result;
        }

        [[nodiscard]] constexpr auto ptr() const noexcept {
            return !hasError() ? Ptr : nullptr;
        }
    };

    template <typename PointerType, typename ErrorType>
    struct PointerOrError<PointerType,
                          ErrorType,
                          std::enable_if_t<!std::is_enum_v<ErrorType>>>
    {
    protected:
        PointerType *Ptr;
        ErrorType Error;
    public:
        constexpr PointerOrError(PointerType *const Ptr) noexcept : Ptr(Ptr) {}
        constexpr PointerOrError(const ErrorType Error) noexcept
        : Ptr(reinterpret_cast<PointerType *>(0x1)), Error(Error) {}

        constexpr PointerOrError(const std::nullptr_t &Nullptr) noexcept
            = delete;

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return (reinterpret_cast<uint64_t>(Ptr) & 1);
        }

        [[nodiscard]] constexpr auto error() const noexcept {
            return Error;
        }

        [[nodiscard]] constexpr auto ptr() const noexcept {
            return !hasError() ? Ptr : nullptr;
        }
    };
}
