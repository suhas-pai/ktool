//
//  include/Utils/Casting.h
//  ktool
//
//  Created by Suhas Pai on 4/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#ifndef USE_LLVM
#define USE_LLVM 1
#endif /* USE_LLVM */

#if USE_LLVM
#include "llvm/Support/Casting.h"
#else
#pragma once

#include <cassert>
#include <utility>

#include "ADT/TypedAllocation.h"

#include "Concepts/Const.h"
#include "Concepts/RemovePointersAndRefs.h"

template <typename From, typename To, typename Enabler = void>
struct CastChecks {
private:
    using ToType = typename Concepts::RemovePointersAndRefs<To>::Type;
public:
    [[nodiscard]] static inline bool CanCast(const From &Fr) {
        return ToType::IsOfKind(Fr);
    }
};

template <typename From, typename To>
struct CastChecks<From, To,
                  typename std::enable_if_t<std::is_base_of_v<To, From>>>
{
    [[nodiscard]] static inline bool CanCast(const From &Fr) { return true; }
};

template <typename From, typename To>
struct CastChecks<From *, To> {
    [[nodiscard]] static inline bool CanCast(const From *Fr) {
        return CastChecks<From, To>::CanCast(*Fr);
    }
};

template <typename From, typename To>
struct CastChecks<const From *, To> {
    [[nodiscard]] static inline bool CanCast(const From *Fr) {
        return CastChecks<From, To>::CanCast(*Fr);
    }
};

template <typename From, typename To>
struct CastChecks<TypedAllocation<From>, To>
{
    [[nodiscard]] static inline bool CanCast(const TypedAllocation<From> &Fr) {
        return CastChecks<From, std::remove_pointer_t<To>>::CanCast(*Fr.get());
    }
};

template <typename From, typename To>
struct CastChecks<const TypedAllocation<From>, To>
{
    [[nodiscard]] static inline bool CanCast(const TypedAllocation<From> &Fr) {
        return CastChecks<From, std::remove_pointer_t<To>>::CanCast(*Fr.get());
    }
};

template <typename To, typename From>
[[nodiscard]] static inline bool isa(const From &Obj) {
    return CastChecks<From, To>::CanCast(Obj);
}

template <typename To, typename From>
[[nodiscard]] static inline bool isa(const From *Obj) {
    return CastChecks<From, To>::CanCast(Obj);
}

template <typename From, typename To, typename Enabler = void>
struct CastReturnTypeCalculator {
    using Type = To;
   [[nodiscard]] static inline Type Convert(From &Fr) {
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
    [[nodiscard]] static inline Type Convert(From &Fr) {
        return reinterpret_cast<Type>(&Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From, To &,
                                std::enable_if_t<
                                    !IsTypedAllocation<From>::Value>>
{
    using Type = To &;
    [[nodiscard]] static inline Type Convert(From &Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From, const To &,
                                std::enable_if_t<
                                    !IsTypedAllocation<From>::Value>>
{
    using Type = const To &;
    [[nodiscard]] static inline Type Convert(const From &Fr) {
        return *reinterpret_cast<const To *>(&Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<From *, To *> {
    using Type = To *;
    [[nodiscard]] static inline Type Convert(From *Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<const From *, const To *> requires Concepts::Const<From>
{
    using Type = const To *;
    [[nodiscard]] static inline Type Convert(const From *Fr) {
        return reinterpret_cast<Type>(Fr);
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<TypedAllocation<From>, To> {
private:
    using ToType = typename std::remove_pointer_t<To>;
public:
    using Type = TypedAllocation<ToType>;
    [[nodiscard]] static inline Type Convert(const TypedAllocation<From> &Fr) {
        return Type(reinterpret_cast<ToType *>(Fr.get()));
    }
};

template <typename From, typename To>
struct CastReturnTypeCalculator<TypedAllocation<const From>, To> {
private:
    using ToType = typename std::remove_pointer_t<To>;
public:
    using Type = TypedAllocation<ToType>;

    [[nodiscard]]
    static inline Type Convert(const TypedAllocation<const From> &Fr) {
        return Type(reinterpret_cast<const ToType *>(Fr.get()));
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
[[nodiscard]] static inline CastReturnType<From &, To> cast(From &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<From &, To>(Fr);
}

template <typename To, Concepts::Const From>

[[nodiscard]]
static inline CastReturnType<const From &, To> cast(const From &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<const From &, To>(Fr);
}

template <typename To, typename From>
[[nodiscard]] static inline CastReturnType<From *, To> cast(From *Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<From *, To>(Fr);
}

template <typename To, Concepts::Const From>
[[nodiscard]]
static inline CastReturnType<const From *, To> cast(const From *Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<const From *, To>(Fr);
}

template <typename To, typename From,
          typename ToType = std::remove_pointer_t<To>>

[[nodiscard]] static inline CastReturnType<TypedAllocation<From>, To>
cast(const TypedAllocation<From> &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<TypedAllocation<From>, To>(Fr);
}

template <typename To, typename From,
          typename ToType = std::remove_pointer_t<To>>

[[nodiscard]] static inline CastReturnType<TypedAllocation<const From>, To>
cast(const TypedAllocation<const From> &Fr) {
    assert(isa<To>(Fr));
    return CastReturnTypeConvert<TypedAllocation<const From>, To>(Fr);
}

template <typename To, typename From,
          typename = std::enable_if_t<!IsTypedAllocation<From>::Value>>

[[nodiscard]] static inline To *dyn_cast(From &Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, Concepts::Const From>
[[nodiscard]] static inline const To *dyn_cast(const From &Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, typename From>
[[nodiscard]] static inline To *dyn_cast(From *Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, Concepts::Const From>
[[nodiscard]] static inline const To *dyn_cast(const From *Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}

template <typename To, typename From>
[[nodiscard]]
static inline TypedAllocation<To> dyn_cast(const TypedAllocation<From> &Fr) {
    if (isa<To>(Fr)) {
        return cast<To>(Fr);
    }

    return nullptr;
}
#endif
