//
//  include/ADT/Mach-O/ObjC.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
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

        [[nodiscard]] constexpr
        uint32_t getIsaAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getSuperClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getCacheAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getVtableAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getData(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getReserved1(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getReserved2(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getReserved3(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr ObjcClass &
        setIsaAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Isa = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setSuperClassAddress(const uint32_t Value,
                             const bool IsBigEndian) noexcept
        {
            this->SuperClass = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setCacheAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Cache = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setVtableAddress(const uint32_t Value,
                         const bool IsBigEndian) noexcept
        {
            this->Vtable = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setData(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Data = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setReserved1(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setReserved2(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass &
        setReserved3(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Reserved3 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        inline bool isSwift(const bool IsBigEndian) const noexcept {
            const auto Mask =
                (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

            return (getData(IsBigEndian) & Mask);
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

        [[nodiscard]] constexpr
        uint64_t getIsaAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getSuperClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getCacheAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getVtableAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getData(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getReserved1(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getReserved2(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getReserved3(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr ObjcClass64 &
        setIsaAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Isa = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setSuperClassAddress(const uint64_t Value,
                             const bool IsBigEndian) noexcept
        {
            this->SuperClass = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setCacheAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Cache = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setVtableAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Vtable = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setData(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Data = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setReserved1(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setReserved2(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClass64 &
        setReserved3(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Reserved3 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]]
        inline bool isSwift(const bool IsBigEndian) const noexcept {
            const auto Mask =
                (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

            return (getData(IsBigEndian) & Mask);
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
            return hasFlag(Flags::IsMeta);
        }

        [[nodiscard]] inline bool isRoot() const noexcept {
            return hasFlag(Flags::IsRoot);
        }

        [[nodiscard]] inline bool hasCxxStructors() const noexcept {
            return hasFlag(Flags::HasCxxStructors);
        }

        [[nodiscard]] inline bool isHidden() const noexcept {
            return hasFlag(Flags::IsHidden);
        }

        [[nodiscard]] inline bool isException() const noexcept {
            return hasFlag(Flags::IsException);
        }

        [[nodiscard]] inline bool hasSwiftInitializer() const noexcept {
            return hasFlag(Flags::HasSwiftInitializer);
        }

        [[nodiscard]] inline bool isARC() const noexcept {
            return hasFlag(Flags::IsARC);
        }

        [[nodiscard]] inline bool hasCxxDestructorOnly() const noexcept {
            return hasFlag(Flags::HasCxxDestructorOnly);
        }

        [[nodiscard]] inline bool hasWeakWithoutARC() const noexcept {
            return hasFlag(Flags::HasWeakWithoutARC);
        }

        [[nodiscard]] inline bool forbidsAssociatedObjects() const noexcept {
            return hasFlag(Flags::ForbidsAssociatedObjects);
        }

        [[nodiscard]] inline bool isFromBundle() const noexcept {
            return hasFlag(Flags::IsFromBundle);
        }

        [[nodiscard]] inline bool isFuture() const noexcept {
            return hasFlag(Flags::IsFuture);
        }

        [[nodiscard]] inline bool isRealized() const noexcept {
            return hasFlag(Flags::IsRealized);
        }

        constexpr
        ObjcClassRoFlags &setIsMeta(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsMeta, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setIsRoot(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsRoot, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setHasCxxStructors(const bool Value = true) noexcept {
            setValueForFlag(Flags::HasCxxStructors, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setIsHidden(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsHidden, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setIsException(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsException, Value);
            return *this;
        }

        constexpr ObjcClassRoFlags &
        setHasSwiftInitializer(const bool Value = true) noexcept {
            setValueForFlag(Flags::HasSwiftInitializer, Value);
            return *this;
        }

        constexpr ObjcClassRoFlags &setIsARC(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsARC, Value);
            return *this;
        }

        constexpr ObjcClassRoFlags &
        setHasCxxDestructorOnly(const bool Value = true) noexcept {
            setValueForFlag(Flags::HasCxxDestructorOnly, Value);
            return *this;
        }

        constexpr ObjcClassRoFlags &
        setHasWeakWithoutARC(const bool Value = true) noexcept {
            setValueForFlag(Flags::HasWeakWithoutARC, Value);
            return *this;
        }

        constexpr ObjcClassRoFlags &
        setForbidsAssociatedObjects(const bool Value = true) noexcept {
            setValueForFlag(Flags::ForbidsAssociatedObjects, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setIsFromBundle(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsFromBundle, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setIsFuture(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsFuture, Value);
            return *this;
        }

        constexpr
        ObjcClassRoFlags &setIsRealized(const bool Value = true) noexcept {
            setValueForFlag(Flags::IsRealized, Value);
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

        [[nodiscard]] constexpr
        ObjcClassRoFlags getFlags(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getInstanceStart(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getInstanceSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getIvarsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getWeakIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getPropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr ObjcClassRo &
        setFlags(const ObjcClassRoFlags &Flags,
                 const bool IsBigEndian) noexcept
        {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setInstanceStart(uint32_t Value, const bool IsBigEndian) noexcept {
            this->InstanceStart = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setInstanceSize(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->InstanceSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setIvarLayout(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->IvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setNameAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setMethodsAddress(const uint32_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->BaseMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setProtocolsAddress(const uint32_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->BaseProtocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setIvarsAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Ivars = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setWeakIvarLayout(const uint32_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->WeakIvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo &
        setPropertiesAddress(const uint32_t Value,
                             const bool IsBigEndian) noexcept
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

        [[nodiscard]] constexpr
        ObjcClassRoFlags getFlags(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getInstanceStart(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getInstanceSize(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getReserved(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getIvarsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getWeakIvarLayout(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getPropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr ObjcClassRo64 &
        setFlags(const ObjcClassRoFlags &Flags,
                 const bool IsBigEndian) noexcept
        {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setInstanceStart(const uint32_t Value,
                         const bool IsBigEndian) noexcept
        {
            this->InstanceStart = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setInstanceSize(const uint32_t Value,
                        const bool IsBigEndian) noexcept
        {
            this->InstanceSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setReserved(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Reserved = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setIvarLayout(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->IvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setNameAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setMethodsAddress(const uint64_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->BaseMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setProtocolsAddress(const uint64_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->BaseProtocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setIvarsAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Ivars = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setWeakIvarLayout(const uint64_t Value,
                          const bool IsBigEndian) noexcept
        {
            this->WeakIvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassRo64 &
        setPropertiesAddress(const uint64_t Value,
                             const bool IsBigEndian) noexcept
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

        [[nodiscard]] constexpr
        uint32_t getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr uint32_t
        getInstanceMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getClassMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint32_t getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr uint32_t
        getInstancePropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getV7(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint32_t getV8(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr ObjcClassCategory &
        setNameAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setClassAddress(const uint32_t Value, const bool IsBigEndian) noexcept {
            this->Class = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setInstanceMethodsAddress(const uint32_t Value,
                                  const bool IsBigEndian) noexcept
        {
            this->InstanceMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setClassMethodsAddress(const uint32_t Value,
                               const bool IsBigEndian) noexcept
        {
            this->ClassMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setProtocolsAddress(const uint32_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->Protocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setInstancePropertiesAddress(const uint32_t Value,
                                     const bool IsBigEndian) noexcept
        {
            this->InstanceProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setV7(const uint32_t Value, const bool IsBigEndian) noexcept
        {
            this->V7 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory &
        setV8(const uint32_t Value,
              const bool IsBigEndian) noexcept
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
        uint64_t getNameAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getClassAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr uint64_t
        getInstanceMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getClassMethodsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        uint64_t getProtocolsAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr uint64_t
        getInstancePropertiesAddress(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getV7(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]]
        constexpr uint64_t getV8(const bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr ObjcClassCategory64 &
        setNameAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setClassAddress(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->Class = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setInstanceMethodsAddress(const uint64_t Value,
                                  const bool IsBigEndian) noexcept
        {
            this->InstanceMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setClassMethodsAddress(const uint64_t Value,
                               const bool IsBigEndian) noexcept
        {
            this->ClassMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setProtocolsAddress(const uint64_t Value,
                            const bool IsBigEndian) noexcept
        {
            this->Protocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setInstancePropertiesAddress(const uint64_t Value,
                                     const bool IsBigEndian) noexcept
        {
            this->InstanceProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setV7(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->V7 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr ObjcClassCategory64 &
        setV8(const uint64_t Value, const bool IsBigEndian) noexcept {
            this->V8 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };
}
