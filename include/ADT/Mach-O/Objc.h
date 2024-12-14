//
//  ADT/Mach-O/ObjC.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/BasicMasksHandler.h"
#include "Utils/SwitchEndian.h"

namespace MachO {
    constexpr static auto IsSwiftObjcClassPreStableMask = (1ull << 0);
    constexpr static auto IsSwiftObjcClassStableMask    = (1ull << 1);

    struct ObjcClass {
        uint32_t Isa;
        uint32_t SuperClass;
        uint32_t Cache;
        uint32_t Vtable;
        uint32_t Data;
        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;

        [[nodiscard]]
        constexpr auto getIsaAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getSuperClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getCacheAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getVtableAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getData(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved1(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved2(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved3(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr auto
        setIsaAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Isa = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setSuperClassAddress(const uint32_t Value,
                             const bool IsBigEndian) noexcept

            -> decltype(*this)
        {
            this->SuperClass = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setCacheAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Cache = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setVtableAddress(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Vtable = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setData(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Data = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved1(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved2(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved3(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved3 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        inline auto isSwift(const bool IsBigEndian) const noexcept {
            const auto Mask =
                IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask;

            return this->getData(IsBigEndian) & Mask;
        }
    };

    struct ObjcClass64 {
        uint64_t Isa;
        uint64_t SuperClass;
        uint64_t Cache;
        uint64_t Vtable;
        uint64_t Data;
        uint64_t Reserved1;
        uint64_t Reserved2;
        uint64_t Reserved3;

        [[nodiscard]]
        constexpr auto getIsaAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getSuperClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getCacheAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getVtableAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getData(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved1(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved2(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved3(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr auto
        setIsaAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Isa = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setSuperClassAddress(const uint64_t Value,
                             const bool IsBigEndian) noexcept

            -> decltype(*this)
        {
            this->SuperClass = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setCacheAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Cache = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setVtableAddress(const uint64_t Value,
                         const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Vtable = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setData(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Data = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved1(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved2(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved3(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved3 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        inline bool isSwift(const bool IsBigEndian) const noexcept {
            const auto Mask =
                IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask;

            return this->getData(IsBigEndian) & Mask;
        }
    };

    enum class ObjcClassRoFlagsEnum : uint32_t {
        IsMeta                   = 1 << 0,
        IsRoot                   = 1 << 1,
        HasCxxStructors          = 1 << 2,
        IsHidden                 = 1 << 4,
        IsException              = 1 << 5,
        HasSwiftInitializer      = 1 << 6,
        IsARC                    = 1 << 7,
        HasCxxDestructorOnly     = 1 << 8,
        HasWeakWithoutARC        = 1 << 9,
        ForbidsAssociatedObjects = 1 << 10,
        IsFromBundle             = 1 << 29,
        IsFuture                 = 1 << 30,
        IsRealized               = static_cast<uint32_t>(1 << 31),
    };

    struct ObjcClassRoFlags : public BasicFlags<ObjcClassRoFlagsEnum> {
    private:
        using Base = BasicFlags<ObjcClassRoFlagsEnum>;
    public:
        using Base::Base;
        using Flags = ObjcClassRoFlagsEnum;

        [[nodiscard]] inline bool isMeta() const noexcept {
            return this->hasFlag(Flags::IsMeta);
        }

        [[nodiscard]] inline bool isRoot() const noexcept {
            return this->hasFlag(Flags::IsRoot);
        }

        [[nodiscard]] inline bool hasCxxStructors() const noexcept {
            return this->hasFlag(Flags::HasCxxStructors);
        }

        [[nodiscard]] inline bool isHidden() const noexcept {
            return this->hasFlag(Flags::IsHidden);
        }

        [[nodiscard]] inline bool isException() const noexcept {
            return this->hasFlag(Flags::IsException);
        }

        [[nodiscard]] inline bool hasSwiftInitializer() const noexcept {
            return this->hasFlag(Flags::HasSwiftInitializer);
        }

        [[nodiscard]] inline bool isARC() const noexcept {
            return this->hasFlag(Flags::IsARC);
        }

        [[nodiscard]] inline bool hasCxxDestructorOnly() const noexcept {
            return this->hasFlag(Flags::HasCxxDestructorOnly);
        }

        [[nodiscard]] inline bool hasWeakWithoutARC() const noexcept {
            return this->hasFlag(Flags::HasWeakWithoutARC);
        }

        [[nodiscard]] inline bool forbidsAssociatedObjects() const noexcept {
            return this->hasFlag(Flags::ForbidsAssociatedObjects);
        }

        [[nodiscard]] inline bool isFromBundle() const noexcept {
            return this->hasFlag(Flags::IsFromBundle);
        }

        [[nodiscard]] inline bool isFuture() const noexcept {
            return this->hasFlag(Flags::IsFuture);
        }

        [[nodiscard]] inline bool isRealized() const noexcept {
            return this->hasFlag(Flags::IsRealized);
        }

        constexpr auto setIsMeta(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsMeta, Value);
            return *this;
        }

        constexpr auto setIsRoot(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsRoot, Value);
            return *this;
        }

        constexpr auto setHasCxxStructors(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::HasCxxStructors, Value);
            return *this;
        }

        constexpr auto setIsHidden(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsHidden, Value);
            return *this;
        }

        constexpr auto setIsException(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsException, Value);
            return *this;
        }

        constexpr auto
        setHasSwiftInitializer(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::HasSwiftInitializer, Value);
            return *this;
        }

        constexpr auto setIsARC(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsARC, Value);
            return *this;
        }

        constexpr auto
        setHasCxxDestructorOnly(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::HasCxxDestructorOnly, Value);
            return *this;
        }

        constexpr auto
        setHasWeakWithoutARC(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::HasWeakWithoutARC, Value);
            return *this;
        }

        constexpr auto
        setForbidsAssociatedObjects(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::ForbidsAssociatedObjects, Value);
            return *this;
        }

        constexpr auto setIsFromBundle(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsFromBundle, Value);
            return *this;
        }

        constexpr auto setIsFuture(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsFuture, Value);
            return *this;
        }

        constexpr auto setIsRealized(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(Flags::IsRealized, Value);
            return *this;
        }
    };

    struct ObjcClassRo {
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
        constexpr auto getFlags(const bool IsBigEndian) const noexcept {
            return ObjcClassRoFlags(SwitchEndianIf(Flags, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getInstanceStart(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getInstanceSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getIvarsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getWeakIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getPropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr auto
        setFlags(const ObjcClassRoFlags &Flags,
                 const bool IsBigEndian) noexcept
        {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceStart(uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceStart = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceSize(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarLayout(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->IvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setNameAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setMethodsAddress(const uint32_t Value,
                          const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint32_t Value,
                            const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseProtocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarsAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Ivars = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setWeakIvarLayout(const uint32_t Value,
                          const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->WeakIvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setPropertiesAddress(const uint32_t Value,
                             const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct ObjcClassRo64 {
        uint32_t Flags;
        uint32_t InstanceStart;
        uint32_t InstanceSize;
        uint32_t Reserved;
        uint64_t IvarLayout;
        uint64_t Name;
        uint64_t BaseMethods;
        uint64_t BaseProtocols;
        uint64_t Ivars;
        uint64_t WeakIvarLayout;
        uint64_t BaseProperties;

        [[nodiscard]]
        constexpr auto getFlags(const bool IsBigEndian) const noexcept {
            return ObjcClassRoFlags(SwitchEndianIf(Flags, IsBigEndian));
        }

        [[nodiscard]]
        constexpr auto getInstanceStart(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getInstanceSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getReserved(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getIvarsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getWeakIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getPropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr auto
        setFlags(const ObjcClassRoFlags &Flags,
                 const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceStart(const uint32_t Value,
                         const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceStart = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceSize(const uint32_t Value,
                        const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setReserved(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Reserved = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarLayout(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->IvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setNameAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setMethodsAddress(const uint64_t Value,
                          const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint64_t Value,
                            const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseProtocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setIvarsAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Ivars = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setWeakIvarLayout(const uint64_t Value,
                          const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->WeakIvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setPropertiesAddress(const uint64_t Value,
                             const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->BaseProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct ObjcClassCategory {
        uint32_t Name;
        uint32_t Class;
        uint32_t InstanceMethods;
        uint32_t ClassMethods;
        uint32_t Protocols;
        uint32_t InstanceProperties;
        uint32_t V7;
        uint32_t V8;

        [[nodiscard]]
        constexpr auto getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getInstanceMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getClassMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        getInstancePropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getV7(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getV8(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr auto
        setNameAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassAddress(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Class = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceMethodsAddress(const uint32_t Value,
                                  const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassMethodsAddress(const uint32_t Value,
                               const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->ClassMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint32_t Value,
                            const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Protocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstancePropertiesAddress(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->InstanceProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setV7(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->V7 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setV8(const uint32_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->V8 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };

    struct ObjcClassCategory64 {
        uint64_t Name;
        uint64_t Class;
        uint64_t InstanceMethods;
        uint64_t ClassMethods;
        uint64_t Protocols;
        uint64_t InstanceProperties;
        uint64_t V7;
        uint64_t V8;

        [[nodiscard]] constexpr
        auto getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        getInstanceMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getClassMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        auto getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr auto
        getInstancePropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getV7(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]]
        constexpr auto getV8(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr auto
        setNameAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassAddress(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->Class = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstanceMethodsAddress(const uint64_t Value,
                                  const bool IsBigEndian) noexcept

            -> decltype(*this)
        {
            this->InstanceMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setClassMethodsAddress(const uint64_t Value,
                               const bool IsBigEndian) noexcept

            -> decltype(*this)
        {
            this->ClassMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setProtocolsAddress(const uint64_t Value,
                            const bool IsBigEndian) noexcept

            -> decltype(*this)
        {
            this->Protocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setInstancePropertiesAddress(const uint64_t Value,
                                     const bool IsBigEndian) noexcept

            -> decltype(*this)
        {
            this->InstanceProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setV7(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->V7 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr auto
        setV8(const uint64_t Value, const bool IsBigEndian) noexcept
            -> decltype(*this)
        {
            this->V8 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };
}
