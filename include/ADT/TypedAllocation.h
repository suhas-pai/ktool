//
//  include/ADT/TypedAllocation.h
//  stool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

// Type-Safe representation of an Allocation. Different that unique_ptr as
// TypedAllocation allows replacing.

template <typename T>
struct TypedAllocation {
private:
    T *Ptr = nullptr;
public:
    constexpr TypedAllocation() noexcept = default;
    constexpr TypedAllocation(T *Ptr) noexcept : Ptr(Ptr) {}

    ~TypedAllocation() noexcept {
        delete Ptr;
        Ptr = nullptr;
    }

    [[nodiscard]] constexpr inline T *get() noexcept { return Ptr; }
    [[nodiscard]] constexpr inline const T *get() const noexcept {
        return Ptr;
    }

    constexpr inline TypedAllocation &set(T *Ptr) noexcept {
        delete this->Ptr;
        this->Ptr = Ptr;

        return *this;
    }

    constexpr inline TypedAllocation &replace(T *Ptr) noexcept {
        this->Ptr = Ptr;
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    constexpr inline TypedAllocation &set(U *Ptr) noexcept {
        delete this->Ptr;
        this->Ptr = Ptr;

        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    constexpr inline TypedAllocation &replace(U *Ptr) noexcept {
        this->Ptr = Ptr;
        return *this;
    }

    constexpr inline void operator=(T *Ptr) noexcept { set(Ptr); }

    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    constexpr inline void operator=(U *Ptr) noexcept {
        set(Ptr);
    }

    [[nodiscard]] constexpr operator T *() noexcept {
        return get();
    }

    [[nodiscard]] constexpr operator const T *() const noexcept {
        return get();
    }

    [[nodiscard]] constexpr T *operator->() noexcept {
        return get();
    }

    [[nodiscard]] constexpr const T *operator->() const noexcept {
        return get();
    }

    [[nodiscard]]
    constexpr inline bool operator==(const T &Rhs) const noexcept {
        return Ptr == Rhs;
    }

    [[nodiscard]]
    constexpr inline bool operator!=(const T &Rhs) const noexcept {
        return Ptr != Rhs;
    }
};

template <typename T>
struct IsTypedAllocation { constexpr static auto Value = false; };

template <typename T>
struct IsTypedAllocation<TypedAllocation<T>> {
    constexpr static auto Value = true;
};
