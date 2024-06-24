/*
 * Objc/Info.h
 * © suhas pai
 */

#pragma once

#include "ADT/FlagsBase.h"
#include "ADT/SwitchEndian.h"

namespace ObjC {
    constexpr static auto IsSwiftObjcClassPreStableMask = 1ull << 0;
    constexpr static auto IsSwiftObjcClassStableMask = 1ull << 1;

    struct Class {
        uint32_t Isa;
        uint32_t SuperClass;
        uint32_t Cache;
        uint32_t Vtable;
        uint32_t Data;
        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;

        [[nodiscard]]
        constexpr auto isaAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto superClassAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cacheAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto vtableAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto data(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved1(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved2(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved3(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr auto
        setIsaAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Isa = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setSuperClassAddress(const uint32_t Value,
                             const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->SuperClass = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setCacheAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Cache = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setVtableAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Vtable = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr
        auto setData(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Data = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved1(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Reserved1 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved2(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved2 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved3(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved3 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        constexpr auto isSwift(const bool IsBigEndian) const noexcept -> bool {
            const auto Mask =
                (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

            return data(IsBigEndian) & Mask;
        }
    };

    struct Class64 {
        uint64_t Isa;
        uint64_t SuperClass;
        uint64_t Cache;
        uint64_t Vtable;
        uint64_t Data;
        uint64_t Reserved1;
        uint64_t Reserved2;
        uint64_t Reserved3;

        [[nodiscard]]
        constexpr auto isaAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto superClassAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto cacheAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto vtableAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto data(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved1(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved2(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto reserved3(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr auto
        setIsaAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Isa = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setSuperClassAddress(const uint64_t Value,
                             const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->SuperClass = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setCacheAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Cache = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setVtableAddress(const uint64_t Value,
                         const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Vtable = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr
        auto setData(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Data = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved1(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved1 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved2(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved2 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved3(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved3 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        constexpr auto isSwift(const bool IsBigEndian) const noexcept -> bool {
            const auto Mask =
                (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

            return data(IsBigEndian) & Mask;
        }
    };

    struct ClassRoFlags : public ADT::FlagsBase<uint32_t> {
    public:
        using ADT::FlagsBase<uint32_t>::FlagsBase;

        enum class Enum : uint32_t {
            IsMeta                   = 1ull << 0,
            IsRoot                   = 1ull << 1,
            HasCxxStructors          = 1ull << 2,
            IsHidden                 = 1ull << 4,
            IsException              = 1ull << 5,
            HasSwiftInitializer      = 1ull << 6,
            IsARC                    = 1ull << 7,
            HasCxxDestructorOnly     = 1ull << 8,
            HasWeakWithoutARC        = 1ull << 9,
            ForbidsAssociatedObjects = 1ull << 10,
            IsFromBundle             = 1ull << 29,
            IsFuture                 = 1ull << 30,
            IsRealized               = static_cast<uint32_t>(1ull << 31),
        };

        [[nodiscard]] constexpr auto meta() const noexcept {
            return has(Enum::IsMeta);
        }

        [[nodiscard]] constexpr auto root() const noexcept {
            return has(Enum::IsRoot);
        }

        [[nodiscard]] constexpr auto hasCxxStructors() const noexcept {
            return has(Enum::HasCxxStructors);
        }

        [[nodiscard]] constexpr auto hidden() const noexcept {
            return has(Enum::IsHidden);
        }

        [[nodiscard]] constexpr auto exception() const noexcept {
            return has(Enum::IsException);
        }

        [[nodiscard]] constexpr auto hasSwiftInitializer() const noexcept {
            return has(Enum::HasSwiftInitializer);
        }

        [[nodiscard]] constexpr auto arc() const noexcept {
            return has(Enum::IsARC);
        }

        [[nodiscard]] constexpr auto hasCxxDestructorOnly() const noexcept {
            return has(Enum::HasCxxDestructorOnly);
        }

        [[nodiscard]] constexpr auto hasWeakWithoutARC() const noexcept {
            return has(Enum::HasWeakWithoutARC);
        }

        [[nodiscard]] constexpr auto forbidsAssociatedObjects() const noexcept {
            return has(Enum::ForbidsAssociatedObjects);
        }

        [[nodiscard]] constexpr auto fromBundle() const noexcept {
            return has(Enum::IsFromBundle);
        }

        [[nodiscard]] constexpr auto future() const noexcept {
            return has(Enum::IsFuture);
        }

        [[nodiscard]] constexpr auto realized() const noexcept {
            return has(Enum::IsRealized);
        }

        constexpr
        auto setMeta(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsMeta, 0, Value);
            return *this;
        }

        constexpr
        auto setRoot(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsRoot, 0, Value);
            return *this;
        }

        constexpr auto
        setHasCxxStructors(const bool Value = true) noexcept -> decltype(*this)
        {
            setValueForMask(Enum::HasCxxStructors, 0, Value);
            return *this;
        }

        constexpr
        auto setHidden(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsHidden, 0, Value);
            return *this;
        }

        constexpr
        auto setException(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsException, 0, Value);
            return *this;
        }

        constexpr auto
        setHasSwiftInitializer(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Enum::HasSwiftInitializer, 0, Value);
            return *this;
        }

        constexpr
        auto setARC(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsARC, 0, Value);
            return *this;
        }

        constexpr auto
        setHasCxxDestructorsOnly(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Enum::HasCxxDestructorOnly, 0, Value);
            return *this;
        }

        constexpr auto
        setHasWeakWithoutARC(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Enum::HasWeakWithoutARC, 0, Value);
            return *this;
        }

        constexpr auto
        setForbidsAssociatedObjects(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Enum::ForbidsAssociatedObjects, 0, Value);
            return *this;
        }

        constexpr auto
        setFromBundle(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsFromBundle, 0, Value);
            return *this;
        }

        constexpr
        auto setFuture(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsFuture, 0, Value);
            return *this;
        }

        constexpr
        auto setRealized(const bool Value = true) noexcept -> decltype(*this) {
            setValueForMask(Enum::IsRealized, 0, Value);
            return *this;
        }
    };

    struct ClassRo {
        uint32_t Flags;
        uint32_t InstanceStart;
        uint32_t InstanceSize;
        uint32_t IvarLayout;
        uint32_t Name;
        uint32_t BaseMethods;
        uint32_t BaseProtocols;
        uint32_t Ivars;
        uint32_t WeakIvarLayout;
        uint32_t BaseProperties;

        [[nodiscard]]
        constexpr auto flags(const bool IsBigEndian) const noexcept {
            return ClassRoFlags(ADT::SwitchEndianIf(Flags, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto instanceStart(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto instanceSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto ivarLayout(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto nameAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto methodsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto protocolsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto ivarsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto weakIvarLayout(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto propertiesAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr auto
        setFlags(const ClassRoFlags &Flags, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Flags = ADT::SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceStart(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceStart = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->InstanceSize = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarLayout(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->IvarLayout = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setNameAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Name = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setMethodsAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseMethods = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint32_t Value,
                            const bool IsBigEndian) noexcept -> decltype(*this)
        {
            this->BaseProtocols = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarsAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Ivars = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setWeakIvarLayout(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->WeakIvarLayout = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setPropertiesAddress(const uint32_t Value,
                             const bool IsBigEndian) noexcept -> decltype(*this)
        {
            this->BaseProperties = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct ClassRo64 {
        uint32_t Flags;
        uint32_t InstanceStart;
        uint32_t InstanceSize;
        uint64_t IvarLayout;
        uint64_t Name;
        uint64_t BaseMethods;
        uint64_t BaseProtocols;
        uint64_t Ivars;
        uint64_t WeakIvarLayout;
        uint64_t BaseProperties;

        [[nodiscard]]
        constexpr auto flags(const bool IsBigEndian) const noexcept {
            return ClassRoFlags(ADT::SwitchEndianIf(Flags, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto instanceStart(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto instanceSize(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto ivarLayout(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto nameAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto methodsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto protocolsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto ivarsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto weakIvarLayout(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto propertiesAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr auto
        setFlags(const ClassRoFlags &Flags, const bool IsBigEndian) noexcept
        {
            this->Flags = ADT::SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceStart(const uint32_t Value,
                         const bool IsBigEndian) noexcept
        {
            this->InstanceStart = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->InstanceSize = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarLayout(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->IvarLayout = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setNameAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Name = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setMethodsAddress(const uint64_t Value, const bool IsBigEndian) noexcept
        {
            this->BaseMethods = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint64_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->BaseProtocols = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarsAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Ivars = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setWeakIvarLayout(const uint64_t Value, const bool IsBigEndian) noexcept
        {
            this->WeakIvarLayout = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setPropertiesAddress(const uint64_t Value,
                             const bool IsBigEndian) noexcept
        {
            this->BaseProperties = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct ClassCategory {
        uint32_t Name;
        uint32_t Class;
        uint32_t InstanceMethods;
        uint32_t ClassMethods;
        uint32_t Protocols;
        uint32_t InstanceProperties;
        uint32_t V7;
        uint32_t V8;

        [[nodiscard]]
        constexpr auto nameAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto classAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto instanceMethodsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto classMethodsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto protocolsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto instancePropertiesAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]] constexpr auto v7(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]] constexpr auto v8(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr auto
        setNameAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Name = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Class = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceMethodsAddress(const uint32_t Value,
                                  const bool IsBigEndian) noexcept
        {
            this->InstanceMethods = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassMethodsAddress(const uint32_t Value,
                               const bool IsBigEndian) noexcept
        {
            this->ClassMethods = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint32_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->Protocols = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstancePropertiesAddress(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
        {
            this->InstanceProperties = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr
        auto setV7(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->V7 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr
        auto setV8(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->V8 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct ClassCategory64 {
        uint64_t Name;
        uint64_t Class;
        uint64_t InstanceMethods;
        uint64_t ClassMethods;
        uint64_t Protocols;
        uint64_t InstanceProperties;
        uint64_t V7;
        uint64_t V8;

        [[nodiscard]]
        constexpr auto nameAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto classAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto instanceMethodsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto classMethodsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto protocolsAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto instancePropertiesAddress(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]] constexpr auto v7(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]] constexpr auto v8(const bool IsBigEndian) const noexcept {
            return ADT::SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr auto
        setNameAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Name = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Class = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceMethodsAddress(const uint64_t Value,
                                  const bool IsBigEndian) noexcept
        {
            this->InstanceMethods = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassMethodsAddress(const uint64_t Value,
                               const bool IsBigEndian) noexcept
        {
            this->ClassMethods = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint64_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->Protocols = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstancePropertiesAddress(const uint64_t Value,
                                     const bool IsBigEndian) noexcept
        {
            this->InstanceProperties = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setV7(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->V7 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setV8(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->V8 = ADT::SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };
}