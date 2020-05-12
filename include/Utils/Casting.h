//
//  include/Utils/Casting.h
//  stool
//
//  Created by Suhas Pai on 4/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>

#include "ADT/TypedAllocation.h"
#include "TypeTraits/DisableIfNotConst.h"
#include "TypeTraits/RemovePointersAndRefs.h"

template <typename From, typename To, typename Enabler = void>
struct CastChecks {
private:
    using ToType = typename TypeTraits::RemovePointersAndRefs<To>::Type;
public:
    static inline bool CanCast(const From &Fr) { return ToType::IsOfKind(Fr); }
};

template <typename From, typename To>
struct CastChecks<From, To,
                  typename std::enable_if_t<std::is_base_of_v<To, From>>>
{
    static inline bool CanCast(const From &Fr) { return true; }
};

template <typename From, typename To>
struct CastChecks<From *, To> {
    static inline bool CanCast(const From *Fr) {
        return CastChecks<From, To>::CanCast(*Fr);
    }
};

template <typename From, typename To>
struct CastChecks<const From *, To> {
    static inline bool CanCast(const From *Fr) {
        return CastChecks<From, To>::CanCast(*Fr);
    }
};

template <typename From, typename To>
struct CastChecks<TypedAllocation<From>, To>
{
    static inline bool CanCast(const TypedAllocation<From> &Fr) {
        return CastChecks<From, std::remove_pointer_t<To>>::CanCast(*Fr.Get());
    }
};

template <typename From, typename To>
struct CastChecks<const TypedAllocation<From>, To>
{
    static inline bool CanCast(const TypedAllocation<From> &Fr) {
        return CastChecks<From, std::remove_pointer_t<To>>::CanCast(*Fr.Get());
    }
};

template <typename To, typename From>
static inline bool isa(const From &Obj) {
    return CastChecks<From, To>::CanCast(Obj);
}

template <typename To, typename From>
static inline bool isa(const From *Obj) {
    return CastChecks<From, To>::CanCast(Obj);
}

template <typename From, typename To, typename Enabler = void>
struct CastReturnTypeCalculator {
    using Type = To;
    static inline Type Convert(From &Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From, To *,
                                std::enable_if_t<
                                    !std::is_pointer_v<From> &&
                                    !IsTypedAllocation<From>::Value>>
{
    using Type = To *;
    static inline Type Convert(From &Fr) {
        return reinterpret_cast<Type>(&Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From, To &,
                                std::enable_if_t<
                                    !IsTypedAllocation<From>::Value>>
{
    using Type = To &;
    static inline Type Convert(From &Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From, const To &,
                                std::enable_if_t<
                                    !IsTypedAllocation<From>::Value>>
{
    using Type = const To &;
    static inline Type Convert(const From &Fr) {
        return *reinterpret_cast<const To *>(&Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From *, To *> {
    using Type = To *;
    static inline Type Convert(From *Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<const From *, const To *,
                                TypeTraits::DisableIfNotConst<From>>
{
    using Type = const To *;
    static inline Type Convert(const From *Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<TypedAllocation<From>, To> {
private:
    using ToType = typename std::remove_pointer_t<To>;
public:
    using Type = TypedAllocation<ToType>;
    static inline Type Convert(const TypedAllocation<From> &Fr) {
        return Type(reinterpret_cast<ToType *>(Fr.Get()));
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<TypedAllocation<const From>, To> {
private:
    using ToType = typename std::remove_pointer_t<To>;
public:
    using Type = TypedAllocation<ToType>;
    static inline Type Convert(const TypedAllocation<const From> &Fr) {
        return Type(reinterpret_cast<const ToType *>(Fr.Get()));
    }
};

template <typename From, typename To>
using CastReturnType = typename CastReturnTypeCalculator<From, To>::Type;

template <typename From, typename To, typename... Args>
auto CastReturnTypeConvert(Args&&... args) -> CastReturnType<From, To> {
    return
        CastReturnTypeCalculator<From, To>::Convert(
            std::forward<Args>(args)...);
}

template <typename To, typename From>
static inline CastReturnType<From &, To> cast(From &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<From &, To>(Fr);
}

template <typename To, typename From,
          typename = TypeTraits::DisableIfNotConst<From>>

static inline CastReturnType<const From &, To> cast(const From &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<const From &, To>(Fr);
}

template <typename To, typename From>
static inline CastReturnType<From *, To> cast(From *Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<From *, To>(Fr);
}

template <typename To, typename From,
          typename = TypeTraits::DisableIfNotConst<From>>

static inline CastReturnType<const From *, To> cast(const From *Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<const From *, To>(Fr);
}

template <typename To, typename From,
          typename ToType = std::remove_pointer_t<To>>

static inline CastReturnType<TypedAllocation<From>, To>
cast(const TypedAllocation<From> &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<TypedAllocation<From>, To>(Fr);
}

template <typename To, typename From,
          typename ToType = std::remove_pointer_t<To>>

static inline CastReturnType<TypedAllocation<const From>, To>
cast(const TypedAllocation<const From> &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<TypedAllocation<const From>, To>(Fr);
}

template <typename To, typename From,
          typename = std::enable_if_t<!IsTypedAllocation<From>::Value>>

static inline To *dyn_cast(From &Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, typename From,
          typename = TypeTraits::DisableIfNotConst<From>>

static inline const To *dyn_cast(const From &Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, typename From>
static inline To *dyn_cast(From *Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, typename From,
          typename = TypeTraits::DisableIfNotConst<From>>

static inline const To *dyn_cast(const From *Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, typename From>
static inline TypedAllocation<To> dyn_cast(const TypedAllocation<From> &Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}