//
//  include/ADT/TypedAllocation.h
//  stool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

template <typename T>
struct TypedAllocation {
private:
    T *Ptr = nullptr;
public:
    explicit TypedAllocation() noexcept = default;
    TypedAllocation(T *Ptr) noexcept : Ptr(Ptr) {}

    ~TypedAllocation() noexcept {
        delete Ptr;
        Ptr = nullptr;
    }

    inline T *Get() const noexcept { return Ptr; }
    inline const T *GetConst() const noexcept { return Ptr; }

    inline void Set(T *Ptr) noexcept {
        delete this->Ptr;
        this->Ptr = Ptr;
    }

    inline void Replace(T *Ptr) noexcept {
        this->Ptr = Ptr;
    }

    inline void operator=(T *Ptr) noexcept { Set(Ptr); }

    operator T *() const noexcept { return Get(); }
    T *operator->() const noexcept { return Get(); }

    inline bool operator==(const T &Rhs) const noexcept { return Ptr == Rhs; }
    inline bool operator!=(const T &Rhs) const noexcept { return Ptr != Rhs; }
};

template <typename T>
struct IsTypedAllocation { constexpr static auto Value = false; };

template <typename T>
struct IsTypedAllocation<TypedAllocation<T>> {
    constexpr static auto Value = true;
};
