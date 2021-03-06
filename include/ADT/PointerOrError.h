//
//  include/ADT/PointerOrError.h
//  ktool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstddef>

#include "ADT/PointerErrorStorage.h"

template <typename T, typename ErrorType>
struct PointerOrError {
    static_assert(TypeTraits::IsEnumClassValue<ErrorType>,
                  "ErrorType must be an enum-class");
protected:
    union {
        PointerErrorStorage<ErrorType> ErrorStorage;
        T *Ptr;
    };
public:
    constexpr PointerOrError() noexcept {};
    constexpr PointerOrError(const ErrorType &Error) noexcept
    : ErrorStorage(Error) {}

    constexpr PointerOrError(const std::nullptr_t &Error) noexcept = delete;
    constexpr PointerOrError(T *Ptr) noexcept : Ptr(Ptr) {}

    [[nodiscard]] constexpr inline bool hasError() const noexcept {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] constexpr inline bool hasPtr() const noexcept {
        return !hasError();
    }

    [[nodiscard]] constexpr inline ErrorType getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] constexpr inline T *getPtr() const noexcept {
        if (hasError()) {
            return nullptr;
        }

        return Ptr;
    }

    [[nodiscard]] constexpr inline T &getRef() const noexcept {
        assert(!hasError());
        return *Ptr;
    }

    constexpr inline PointerOrError &setError(const ErrorType &Error) noexcept {
        ErrorStorage = Error;
        return *this;
    }

    constexpr inline PointerOrError &setPtr(T *Ptr) const noexcept {
        this->Ptr = Ptr;
        return *this;
    }

    [[nodiscard]] constexpr inline uint64_t value() const noexcept {
        return reinterpret_cast<uint64_t>(Ptr);
    }

    constexpr inline PointerOrError &clear() noexcept {
        Ptr = nullptr;
        return *this;
    }

    constexpr
    inline PointerOrError &operator=(const ErrorType &Error) const noexcept {
        return setError(Error);
    }

    inline PointerOrError &operator=(T *Ptr) const noexcept {
        return setPtr(Ptr);
    }
};

template <typename T, typename ErrorType>
struct TypedAllocationOrError : public PointerOrError<T, ErrorType> {
private:
    using Base = PointerOrError<T, ErrorType>;
public:
    constexpr TypedAllocationOrError() noexcept = default;
    constexpr TypedAllocationOrError(const ErrorType &Error) noexcept
    : Base(Error) {}

    constexpr TypedAllocationOrError(const std::nullptr_t &) noexcept = delete;
    constexpr TypedAllocationOrError(T *Ptr) noexcept : Base(Ptr) {}

    TypedAllocationOrError(const TypedAllocationOrError &) noexcept = delete;
    TypedAllocationOrError(TypedAllocationOrError &&Ty) noexcept
    : Base(reinterpret_cast<T *>(Ty.value())) {}

    [[nodiscard]] constexpr inline T *getAndClaimPtr() noexcept {
        const auto Result = this->getPtr();
        this->clear();

        return Result;
    }

    TypedAllocationOrError &
    operator=(const TypedAllocationOrError &) noexcept = delete;

    constexpr
    TypedAllocationOrError &operator=(TypedAllocationOrError &&Rhs) noexcept {
        if (this->hasPtr()) {
            delete this->getPtr();
        }

        this->Ptr = Rhs.Ptr;
        Rhs.Ptr = nullptr;

        return *this;
    }

    ~TypedAllocationOrError() noexcept {
        if (this->hasPtr()) {
            delete this->getPtr();
        }
    }
};
