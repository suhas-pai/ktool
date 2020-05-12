//
//  include/ADT/PointerOrError.h
//  stool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstddef>

#include "ADT/PointerErrorStorage.h"

template <typename PtrType, typename ErrorType>
struct PointerOrError {
    static_assert(std::is_pointer_v<PtrType>, "PtrType must be a pointer-type");
protected:
    union {
        PointerErrorStorage<ErrorType> ErrorStorage;
        PtrType Ptr;
    };
public:
    explicit PointerOrError() noexcept = default;

    PointerOrError(const ErrorType &Error) noexcept : ErrorStorage(Error) {}
    PointerOrError(const std::nullptr_t &Error) noexcept = delete;
    PointerOrError(const PtrType &Ptr) noexcept : Ptr(Ptr) {}

    inline bool HasError() const noexcept { return ErrorStorage.HasValue(); }
    inline bool HasPtr() const noexcept { return !HasError(); }
    inline ErrorType GetError() const noexcept {
        return ErrorStorage.GetValue();
    }

    inline PtrType GetPtr() const noexcept {
        assert(!HasError());
        return Ptr;
    }

    inline decltype(*Ptr) GetRef() const noexcept {
        assert(!HasError());
        return *Ptr;
    }

    inline void SetError(const ErrorType &Error) const noexcept {
        ErrorStorage = Error;
    }

    inline void SetPtr(const PtrType &Ptr) const noexcept {
        this->Ptr = Ptr;
    }

    inline operator PtrType() const noexcept { return GetPtr(); }

    inline void operator=(const ErrorType &Error) const noexcept {
        SetError(Error);
    }

    inline void operator=(const PtrType &Ptr) const noexcept { SetPtr(Ptr); }
};

template <typename PtrType, typename ErrorType>
struct TypedAllocationOrError : public PointerOrError<PtrType, ErrorType> {
private:
    using Super = PointerOrError<PtrType, ErrorType>;
public:
    explicit TypedAllocationOrError() noexcept = default;
    TypedAllocationOrError(const ErrorType &Error) noexcept : Super(Error) {}
    TypedAllocationOrError(const std::nullptr_t &Error) noexcept = delete;
    TypedAllocationOrError(const PtrType &Ptr) noexcept : Super(Ptr) {}

    explicit TypedAllocationOrError(const TypedAllocationOrError &)
    noexcept = delete;

    explicit TypedAllocationOrError(TypedAllocationOrError &&) noexcept;

    TypedAllocationOrError &operator=(const TypedAllocationOrError &)
    noexcept = delete;

    TypedAllocationOrError &operator=(TypedAllocationOrError &&Rhs) noexcept {
        if (this->HasPtr()) {
            delete this->GetPtr();
        }

        this->Ptr = Rhs.Ptr;
        Rhs.Ptr = nullptr;

        return *this;
    }

    ~TypedAllocationOrError() noexcept {
        if (this->HasPtr()) {
            delete this->GetPtr();
        }
    }
};
