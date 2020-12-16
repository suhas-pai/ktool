//
//  include/Objects/MemoryBase.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/FileDescriptor.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerErrorStorage.h"
#include "ADT/RelativeRange.h"

#include "TypeTraits/DisableIfNotConst.h"
#include "TypeTraits/DisableIfNotPointer.h"

#include "Kind.h"

struct MemoryObject;
struct MemoryObjectOrError {
protected:
    union {
        MemoryObject *Ptr;
        uintptr_t Storage;

        struct {
            ObjectKind ObjKind : 8;
            uint8_t Error;
        };
    };
public:
    constexpr MemoryObjectOrError(MemoryObject *Ptr) noexcept : Ptr(Ptr) {}
    constexpr MemoryObjectOrError(ObjectKind ObjKind, uint8_t Error) noexcept
    : ObjKind(ObjKind), Error(static_cast<uint8_t>(Error)) {}

    [[nodiscard]] inline bool hasError() const noexcept {
        return PointerHasErrorValue(Storage);
    }

    [[nodiscard]] inline uint8_t getErrorInt() const noexcept {
        if (hasError()) {
            return Error;
        }

        return 0;
    }

    [[nodiscard]] inline ObjectKind getObjectKind() const noexcept {
        assert(hasError());
        return ObjKind;
    }

    [[nodiscard]] inline MemoryObject *getObject() const noexcept {
        assert(!hasError());
        return Ptr;
    }
};

struct MemoryObject {
private:
    ObjectKind Kind;
protected:
    explicit MemoryObject(ObjectKind Kind) noexcept;
public:
    [[nodiscard]] static inline bool IsOfKind(ObjectKind Kind) noexcept {
        assert(0 && "IsOfKind() called on base-class");
    }

    [[nodiscard]]
    static MemoryObjectOrError Open(const ConstMemoryMap &Map) noexcept;

    [[nodiscard]] inline ObjectKind getKind() const noexcept { return Kind; }
    virtual ~MemoryObject() noexcept = default;

    virtual ConstMemoryMap getConstMap() const noexcept = 0;
    virtual RelativeRange getRange() const noexcept = 0;
};

struct MachOMemoryObject;
struct FatMachOMemoryObject;

struct ConstMachOMemoryObject;
struct ConstFatMachOMemoryObject;

struct DscMemoryObject;
struct ConstDscMemoryObject;

struct DscImageMemoryObject;
struct ConstDscImageMemoryObject;

template <enum ObjectKind>
struct ObjectKindInfo {};

template<>
struct ObjectKindInfo<ObjectKind::None> {
    constexpr static auto Kind = ObjectKind::None;
};

template<>
struct ObjectKindInfo<ObjectKind::MachO> {
    constexpr static auto Kind = ObjectKind::MachO;

    typedef MachOMemoryObject Type;
    typedef MachOMemoryObject *Ptr;

    typedef ConstMachOMemoryObject ConstType;
    typedef ConstMachOMemoryObject *ConstPtr;
};

template<>
struct ObjectKindInfo<ObjectKind::FatMachO> {
    constexpr static auto Kind = ObjectKind::FatMachO;

    typedef FatMachOMemoryObject Type;
    typedef FatMachOMemoryObject *Ptr;

    typedef ConstFatMachOMemoryObject ConstType;
    typedef ConstFatMachOMemoryObject *ConstPtr;
};

template<>
struct ObjectKindInfo<ObjectKind::DyldSharedCache> {
    constexpr static auto Kind = ObjectKind::DyldSharedCache;

    typedef DscMemoryObject Type;
    typedef DscMemoryObject *Ptr;

    typedef ConstDscMemoryObject ConstType;
    typedef ConstDscMemoryObject *ConstPtr;
};

template<>
struct ObjectKindInfo<ObjectKind::DscImage> {
    constexpr static auto Kind = ObjectKind::DscImage;

    typedef DscImageMemoryObject Type;
    typedef DscImageMemoryObject *Ptr;

    typedef ConstDscImageMemoryObject ConstType;
    typedef ConstDscImageMemoryObject *ConstPtr;
};

template <ObjectKind Kind>
using ObjectClassFromKindType = typename ObjectKindInfo<Kind>::Type;

template <ObjectKind Kind>
using ObjectClassFromKindPtr = typename ObjectKindInfo<Kind>::Ptr;

template <ObjectKind Kind>
using ObjectClassFromKindConstType = typename ObjectKindInfo<Kind>::ConstType;

template <ObjectKind Kind>
using ObjectClassFromKindConstPtr = typename ObjectKindInfo<Kind>::ConstPtr;

// isa<T> templates
// isa<ObjectKind>(const MemoryObject &) -> bool

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline bool isa(const MemoryObject &Object) noexcept {
    return MemoryObjectType::IsOfKind(Object);
}

// isa<ObjectKind>(const MemoryObject *) -> bool

template <ObjectKind Kind,
          typename MemoryObjectType = ObjectClassFromKindType<Kind>>

static inline bool isa(const MemoryObject *Object) noexcept {
    return MemoryObjectType::IsOfKind(*Object);
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

static
inline const MemoryObjectType *dyn_cast(const MemoryObject *Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    return nullptr;
}
