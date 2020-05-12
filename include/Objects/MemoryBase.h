//
//  include/Objects/MemoryBase.h
//  stool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/FileDescriptor.h"
#include "ADT/MemoryMap.h"
#include "ADT/RelativeRange.h"

#include "TypeTraits/DisableIfNotConst.h"
#include "TypeTraits/DisableIfNotPointer.h"

#include "Kind.h"

// Make casting between MemoryObject and ConstMemoryObject possible with a
// common base-class

struct MemoryObjectBase {};

struct ConstMemoryObject;
struct MemoryObject : public MemoryObjectBase {
private:
    ObjectKind Kind;
protected:
    explicit MemoryObject(ObjectKind Kind) noexcept;
public:
    virtual ~MemoryObject() noexcept = default;
    inline ObjectKind GetKind() const noexcept { return Kind; }

    virtual bool HasError() const noexcept;
    virtual bool DidMatchFormat() const noexcept;

    virtual MemoryMap GetMap() const noexcept;
    virtual ConstMemoryMap GetConstMap() const noexcept;
    virtual RelativeRange GetRange() const noexcept;

    virtual MemoryObject *ToPtr() const noexcept;

    inline operator ConstMemoryObject &() noexcept {
        return reinterpret_cast<ConstMemoryObject &>(*this);
    }

    inline operator const ConstMemoryObject &() const noexcept {
        return reinterpret_cast<const ConstMemoryObject &>(*this);
    }

    static MemoryObject *Open(const MemoryMap &Map) noexcept;
    static inline bool IsOfKind(ObjectKind Kind) { return true; }
};

struct ConstMemoryObject : public MemoryObjectBase {
    friend struct MemoryObject;
private:
    ObjectKind Kind;
protected:
    explicit ConstMemoryObject(ObjectKind Kind) noexcept;
public:
    virtual ~ConstMemoryObject() noexcept = default;
    inline ObjectKind GetKind() const noexcept { return Kind; }

    virtual bool HasError() const noexcept;
    virtual bool DidMatchFormat() const noexcept;

    virtual ConstMemoryMap GetMap() const noexcept;
    virtual ConstMemoryMap GetConstMap() const noexcept;
    virtual RelativeRange GetRange() const noexcept;

    virtual ConstMemoryObject *ToPtr() const noexcept;
    virtual operator ConstMemoryMap() const noexcept;

    static ConstMemoryObject *Open(const ConstMemoryMap &Map) noexcept;
    static inline bool IsOfKind(const MemoryObjectBase &Base) { return true; }
};

static_assert(sizeof(MemoryObject) == sizeof(ConstMemoryObject),
              "MemoryObject and its const-class are not the same size");

struct MachOMemoryObject;
struct FatMachOMemoryObject;

struct ConstMachOMemoryObject;
struct ConstFatMachOMemoryObject;

template <enum ObjectKind>
struct ObjectKindInfo {};

template<>
struct ObjectKindInfo<ObjectKind::None> {
    constexpr static const auto Kind = ObjectKind::None;
};

template<>
struct ObjectKindInfo<ObjectKind::MachO> {
    constexpr static const auto Kind = ObjectKind::MachO;

    typedef MachOMemoryObject Type;
    typedef MachOMemoryObject *Ptr;

    typedef ConstMachOMemoryObject ConstType;
    typedef ConstMachOMemoryObject *ConstPtr;
};

template<>
struct ObjectKindInfo<ObjectKind::FatMachO> {
    constexpr static const auto Kind = ObjectKind::FatMachO;

    typedef FatMachOMemoryObject Type;
    typedef FatMachOMemoryObject *Ptr;

    typedef ConstFatMachOMemoryObject ConstType;
    typedef ConstFatMachOMemoryObject *ConstPtr;
};

template <ObjectKind Kind>
using ObjectClassFromKindType = typename ObjectKindInfo<Kind>::Type;

template <ObjectKind Kind>
using ObjectClassFromKindPtr = typename ObjectKindInfo<Kind>::Ptr;

template <ObjectKind Kind>
using ObjectClassFromKindConstType = typename ObjectKindInfo<Kind>::ConstType;

template <ObjectKind Kind>
using ObjectClassFromKindConstPtr = typename ObjectKindInfo<Kind>::ConstPtr;

template <typename T>
using IsSubclassOfMemoryObject =
    typename std::enable_if_t<std::is_base_of<MemoryObject, T>::value>;

template <typename T>
using IsSubclassOfConstMemoryObject =
    typename std::enable_if_t<std::is_base_of<ConstMemoryObject, T>::value>;

// isa<T> templates
// isa<MemoryObjectType>(const MemoryObject *) -> bool

template <typename MemoryObjectType,
          typename = IsSubclassOfMemoryObject<MemoryObjectType>>

static inline bool isa(const MemoryObject &Object) noexcept {
    return MemoryObjectType::IsOfKind(Object);
}

// isa<MemoryObjectType>(const ConstMemoryObject *) -> bool

template <typename MemoryObjectType,
          typename = IsSubclassOfConstMemoryObject<MemoryObjectType>>

static inline bool isa(const ConstMemoryObject &Object) noexcept {
    return MemoryObjectType::IsOfKind(Object);
}

// isa<MemoryObjectType>(const MemoryObject *) -> bool

template <typename MemoryObjectType,
          typename = IsSubclassOfMemoryObject<MemoryObjectType>>

static inline bool isa(const MemoryObject *Object) noexcept {
    return MemoryObjectType::IsOfKind(*Object);
}

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline bool isa(const MemoryObject &Object) noexcept {
    return MemoryObjectType::IsOfKind(Object);
}

// isa<MemoryObjectType>(const ConstMemoryObject *) -> bool

template <ObjectKind Kind,
          typename ConstMemoryObjectType = ObjectClassFromKindConstType<Kind>>

static inline bool isa(const ConstMemoryObject &Object) noexcept {
    return ConstMemoryObjectType::IsOfKind(Object);
}

// isa<MemoryObjectType>(const MemoryObject *) -> bool

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline bool isa(const MemoryObject *Object) noexcept {
    return MemoryObjectType::IsOfKind(*Object);
}

// isa<MemoryObjectType>(const ConstMemoryObject *) -> bool

template <ObjectKind Kind,
          typename ConstMemoryObjectType = ObjectClassFromKindConstType<Kind>>

static inline bool isa(const ConstMemoryObjectType *Object) noexcept {
    return ConstMemoryObjectType::IsOfKind(*Object);
}

// cast<Kind>(MemoryObject &) -> MemoryObjectType &

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline MemoryObjectType &cast(MemoryObject &Object) {
    assert(isa<Kind>(Object));
    return static_cast<MemoryObjectType &>(Object);
}

// cast<Kind>(const MemoryObject &) -> const MemoryObjectType &

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline const MemoryObjectType &cast(const MemoryObject &Object) {
    assert(isa<Kind>(Object));
    return static_cast<const MemoryObjectType &>(Object);
}

// cast<Kind>(MemoryObject *) -> MemoryObjectTypePtr

template <ObjectKind Kind,
          typename MemoryObjectTypePtr = ObjectClassFromKindPtr<Kind>>

static inline MemoryObjectTypePtr cast(MemoryObject *Object) {
    assert(isa<Kind>(Object));
    return static_cast<MemoryObjectTypePtr>(Object);
}

// cast<Kind>(const MemoryObject *) -> const MemoryObjectType *

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline const MemoryObjectType *cast(const MemoryObject *Object) {
    assert(isa<Kind>(Object));
    return static_cast<const MemoryObjectType *>(Object);
}

// cast<Kind>(ConstMemoryObject &) -> ConstMemoryObjectType &

template <ObjectKind Kind,
          typename ConstMemoryObjectType = ObjectClassFromKindConstType<Kind>>

static inline ConstMemoryObjectType &cast(ConstMemoryObject &Object) {
    assert(isa<Kind>(Object));
    return static_cast<ConstMemoryObjectType &>(Object);
}

// cast<Kind>(const ConstMemoryObject &) -> const ConstMemoryObjectType &

template <ObjectKind Kind,
          typename ConstMemoryObjectType = ObjectClassFromKindConstType<Kind>>

static inline
const ConstMemoryObjectType &cast(const ConstMemoryObject &Object) {
    assert(isa<Kind>(Object));
    return static_cast<const ConstMemoryObjectType &>(Object);
}

// cast<Kind>(ConstMemoryObject *) -> ConstMemoryObjectPtr

template <ObjectKind Kind,
          typename ConstMemoryObjectPtr = ObjectClassFromKindConstPtr<Kind>>

static inline ConstMemoryObjectPtr cast(ConstMemoryObject *Object) {
    assert(isa<Kind>(Object));
    return static_cast<ConstMemoryObjectPtr>(Object);
}

// cast<Kind>(const ConstMemoryObject *) -> const ConstMemoryObjectType *

template <ObjectKind Kind,
          typename ConstMemoryObjectType = ObjectClassFromKindConstType<Kind>>

static inline
const ConstMemoryObjectType *cast(const ConstMemoryObject *Object) {
    assert(isa<Kind>(Object));
    return static_cast<const ConstMemoryObjectType *>(Object);
}

// dyn_cast<Kind>(MemoryObject *) -> MemoryObjectPtr

template <ObjectKind Kind,
          typename MemoryObjectPtr = ObjectClassFromKindPtr<Kind>>

static inline MemoryObjectPtr dyn_cast(MemoryObject *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(const MemoryObject *) -> const MemoryObjectType *

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline
const MemoryObjectType *dyn_cast(const MemoryObject *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(ConstMemoryObject *) -> ConstMemoryObjectPtr

template <ObjectKind Kind,
          typename ConstMemoryObjectPtr = ObjectClassFromKindConstPtr<Kind>>

static inline
ConstMemoryObjectPtr *dyn_cast(ConstMemoryObject *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}

// dyn_cast<Kind>(const ConstMemoryObject *) -> const ConstMemoryObjectType *

template <ObjectKind Kind,
          typename ConstMemoryObjectType = ObjectClassFromKindConstType<Kind>>

static inline const ConstMemoryObjectType *
dyn_cast(const ConstMemoryObject *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}
