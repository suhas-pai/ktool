//
//  Objects/MemoryBase.h
//  ktool
//
//  Created by Suhas Pai on 3/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/ExpectedAlloc.h"
#include "ADT/MemoryMap.h"

#include "Kind.h"

struct MemoryObject;
struct MemoryObjectOrError : public ExpectedAlloc<MemoryObject, AnyError> {
    using Base = ExpectedAlloc<MemoryObject, AnyError>;
protected:
    constexpr static auto ErrorShift = 0;
    constexpr static auto KindShift = 8;
public:
    MemoryObjectOrError(MemoryObject *const Object) noexcept
    : Base(Object) {}

    explicit
    MemoryObjectOrError(const ObjectKind Kind, const uint8_t Error) noexcept
    : Base(static_cast<AnyError>(
            static_cast<uint16_t>(Kind) << KindShift |
            static_cast<uint16_t>(Error << ErrorShift)))
    {

    }

    [[nodiscard]] inline auto getObjectKind() const noexcept -> ObjectKind;
    [[nodiscard]] inline auto getErrorInt() const noexcept {
        return static_cast<uint16_t>(this->getError()) >> ErrorShift;
    }
};

struct MemoryObject {
private:
    ObjectKind Kind;
protected:
    explicit MemoryObject(ObjectKind Kind) noexcept;
public:
    [[nodiscard]] static inline auto IsOfKind(const ObjectKind) noexcept {
        assert(0 && "IsOfKind() called on base-class");
    }

    [[nodiscard]] static auto Open(const MemoryMap &Map) noexcept
        -> MemoryObjectOrError;

    [[nodiscard]] inline ObjectKind getKind() const noexcept { return Kind; }
    virtual ~MemoryObject() noexcept = default;

    virtual ConstMemoryMap getConstMap() const noexcept = 0;
    virtual Range getRange() const noexcept = 0;
};

auto MemoryObjectOrError::getObjectKind() const noexcept -> ObjectKind {
    if (this->hasError()) {
        const auto Error = static_cast<uint16_t>(this->getError());
        return static_cast<ObjectKind>(Error >> KindShift);
    }

    return this->value()->getKind();
}

struct MachOMemoryObject;
struct FatMachOMemoryObject;

struct DscMemoryObject;
struct DscImageMemoryObject;

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
};

template<>
struct ObjectKindInfo<ObjectKind::FatMachO> {
    constexpr static auto Kind = ObjectKind::FatMachO;

    typedef FatMachOMemoryObject Type;
    typedef FatMachOMemoryObject *Ptr;
};

template<>
struct ObjectKindInfo<ObjectKind::DyldSharedCache> {
    constexpr static auto Kind = ObjectKind::DyldSharedCache;

    typedef DscMemoryObject Type;
    typedef DscMemoryObject *Ptr;
};

template<>
struct ObjectKindInfo<ObjectKind::DscImage> {
    constexpr static auto Kind = ObjectKind::DscImage;

    typedef DscImageMemoryObject Type;
    typedef DscImageMemoryObject *Ptr;
};

template <ObjectKind Kind>
using ObjectClassFromKindType = typename ObjectKindInfo<Kind>::Type;

template <ObjectKind Kind>
using ObjectClassFromKindPtr = typename ObjectKindInfo<Kind>::Ptr;

// isa<T> templates
// isa<ObjectKind>(const MemoryObject &) -> bool

template <ObjectKind Kind>
static inline auto isa(const MemoryObject &Object) noexcept {
    return ObjectClassFromKindType<Kind>::IsOfKind(Object);
}

// isa<ObjectKind>(const MemoryObject *) -> bool

template <ObjectKind Kind>
static inline auto isa(const MemoryObject *Object) noexcept {
    return ObjectClassFromKindType<Kind>::IsOfKind(*Object);
}

// cast<Kind>(MemoryObject &) -> MemoryObjectType &

template <ObjectKind Kind>
static inline auto &cast(MemoryObject &Object) {
    assert(isa<Kind>(Object));
    return static_cast<ObjectClassFromKindType<Kind> &>(Object);
}

// cast<Kind>(const MemoryObject &) -> const MemoryObjectType &

template <ObjectKind Kind>
static inline auto &cast(const MemoryObject &Object) {
    using MemoryObjectType = ObjectClassFromKindType<Kind>;

    assert(isa<Kind>(Object));
    return static_cast<const MemoryObjectType &>(Object);
}

// cast<Kind>(MemoryObject *) -> MemoryObjectTypePtr

template <ObjectKind Kind>
static inline auto cast(MemoryObject *const Object) {
    assert(isa<Kind>(Object));
    return static_cast<ObjectClassFromKindPtr<Kind>>(Object);
}

// cast<Kind>(const MemoryObject *) -> const MemoryObjectType *

template <ObjectKind Kind>
static inline auto cast(const MemoryObject *const Object) {
    using MemoryObjectType = ObjectClassFromKindType<Kind>;

    assert(isa<Kind>(Object));
    return static_cast<const MemoryObjectType *>(Object);
}

// dyn_cast<Kind>(MemoryObject *) -> MemoryObjectPtr

template <ObjectKind Kind>
static inline auto dyn_cast(MemoryObject *const Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    using MemoryObjectTypePtr = ObjectClassFromKindPtr<Kind>;
    return static_cast<MemoryObjectTypePtr>(nullptr);
}

// dyn_cast<Kind>(const MemoryObject *) -> const MemoryObjectType *

template <ObjectKind Kind>
static inline auto dyn_cast(const MemoryObject *const Object) noexcept {
    if (isa<Kind>(Object)) {
        return cast<Kind>(Object);
    }

    using MemoryObjectType = ObjectClassFromKindType<Kind>;
    return static_cast<const MemoryObjectType *>(nullptr);
}
