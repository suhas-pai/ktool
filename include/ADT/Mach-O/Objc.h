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
        inline uint32_t getIsaAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getSuperClassAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getCacheAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getVtableAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getData(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getReserved1(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getReserved2(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getReserved3(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr inline
        ObjcClass &setIsaAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Isa = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setSuperClassAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->SuperClass = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setCacheAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Cache = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setVtableAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Vtable = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setData(uint32_t Value, bool IsBigEndian) noexcept {
            this->Data = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setReserved1(uint32_t Value, bool IsBigEndian) noexcept {
            this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setReserved2(uint32_t Value, bool IsBigEndian) noexcept {
            this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass &
        setReserved3(uint32_t Value, bool IsBigEndian) noexcept {
            this->Reserved3 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline bool isSwift(bool IsBigEndian) const noexcept {
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
        inline uint64_t getIsaAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Isa, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getSuperClassAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(SuperClass, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getCacheAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Cache, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getVtableAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Vtable, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint64_t getData(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Data, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getReserved1(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved1, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getReserved2(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved2, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getReserved3(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved3, IsBigEndian);
        }

        constexpr inline
        ObjcClass64 &setIsaAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Isa = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass64 &
        setSuperClassAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->SuperClass = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass64 &
        setCacheAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Cache = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClass64 &
        setVtableAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Vtable = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClass64 &setData(uint64_t Value, bool IsBigEndian) noexcept {
            this->Data = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClass64 &setReserved1(uint64_t Value, bool IsBigEndian) noexcept {
            this->Reserved1 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClass64 &setReserved2(uint64_t Value, bool IsBigEndian) noexcept {
            this->Reserved2 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClass64 &setReserved3(uint64_t Value, bool IsBigEndian) noexcept {
            this->Reserved3 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        [[nodiscard]] inline bool isSwift(bool IsBigEndian) const noexcept {
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

        inline ObjcClassRoFlags &setIsMeta(bool Value = true) noexcept {
            setValueForFlag(Flags::IsMeta, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsRoot(bool Value = true) noexcept {
            setValueForFlag(Flags::IsRoot, Value);
            return *this;
        }

        inline
        ObjcClassRoFlags &setHasCxxStructors(bool Value = true) noexcept {
            setValueForFlag(Flags::HasCxxStructors, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsHidden(bool Value = true) noexcept {
            setValueForFlag(Flags::IsHidden, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsException(bool Value = true) noexcept {
            setValueForFlag(Flags::IsException, Value);
            return *this;
        }

        inline
        ObjcClassRoFlags &setHasSwiftInitializer(bool Value = true) noexcept {
            setValueForFlag(Flags::HasSwiftInitializer, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsARC(bool Value = true) noexcept {
            setValueForFlag(Flags::IsARC, Value);
            return *this;
        }

        inline
        ObjcClassRoFlags &setHasCxxDestructorOnly(bool Value = true) noexcept {
            setValueForFlag(Flags::HasCxxDestructorOnly, Value);
            return *this;
        }

        inline
        ObjcClassRoFlags &setHasWeakWithoutARC(bool Value = true) noexcept {
            setValueForFlag(Flags::HasWeakWithoutARC, Value);
            return *this;
        }

        inline ObjcClassRoFlags &
        setForbidsAssociatedObjects(bool Value = true) noexcept {
            setValueForFlag(Flags::ForbidsAssociatedObjects, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsFromBundle(bool Value = true) noexcept {
            setValueForFlag(Flags::IsFromBundle, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsFuture(bool Value = true) noexcept {
            setValueForFlag(Flags::IsFuture, Value);
            return *this;
        }

        inline ObjcClassRoFlags &setIsRealized(bool Value = true) noexcept {
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
        inline ObjcClassRoFlags getFlags(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getInstanceStart(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getInstanceSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getIvarLayout(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getNameAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getMethodsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getProtocolsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getIvarsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getWeakIvarLayout(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getPropertiesAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr inline ObjcClassRo &
        setFlags(const ObjcClassRoFlags &Flags, bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setInstanceStart(uint32_t Value, bool IsBigEndian) noexcept {
            this->InstanceStart = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setInstanceSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->InstanceSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setIvarLayout(uint32_t Value, bool IsBigEndian) noexcept {
            this->IvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setNameAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setMethodsAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->BaseMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setProtocolsAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->BaseProtocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setIvarsAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Ivars = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setWeakIvarLayout(uint32_t Value, bool IsBigEndian) noexcept {
            this->WeakIvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo &
        setPropertiesAddress(uint32_t Value, bool IsBigEndian) noexcept {
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
        inline ObjcClassRoFlags getFlags(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getInstanceStart(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceStart, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getInstanceSize(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceSize, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getReserved(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Reserved, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getIvarLayout(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(IvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getNameAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getMethodsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getProtocolsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProtocols, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getIvarsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Ivars, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getWeakIvarLayout(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(WeakIvarLayout, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getPropertiesAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(BaseProperties, IsBigEndian);
        }

        constexpr inline ObjcClassRo64 &
        setFlags(const ObjcClassRoFlags &Flags, bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setInstanceStart(uint32_t Value, bool IsBigEndian) noexcept {
            this->InstanceStart = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setInstanceSize(uint32_t Value, bool IsBigEndian) noexcept {
            this->InstanceSize = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setReserved(uint32_t Value, bool IsBigEndian) noexcept {
            this->Reserved = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setIvarLayout(uint64_t Value, bool IsBigEndian) noexcept {
            this->IvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setNameAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setMethodsAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->BaseMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setProtocolsAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->BaseProtocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setIvarsAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Ivars = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setWeakIvarLayout(uint64_t Value, bool IsBigEndian) noexcept {
            this->WeakIvarLayout = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassRo64 &
        setPropertiesAddress(uint64_t Value, bool IsBigEndian) noexcept {
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
        inline uint32_t getNameAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getClassAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr inline
        uint32_t getInstanceMethodsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr inline
        uint32_t getClassMethodsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint32_t getProtocolsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr inline
        uint32_t getInstancePropertiesAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getV7(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint32_t getV8(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr inline ObjcClassCategory &
        setNameAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory &
        setClassAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Class = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory &
        setInstanceMethodsAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->InstanceMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory &
        setClassMethodsAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->ClassMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory &
        setProtocolsAddress(uint32_t Value, bool IsBigEndian) noexcept {
            this->Protocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory &
        setInstancePropertiesAddress(uint32_t Value, bool IsBigEndian) noexcept
        {
            this->InstanceProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClassCategory &setV7(uint32_t Value, bool IsBigEndian) noexcept {
            this->V7 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClassCategory &setV8(uint32_t Value, bool IsBigEndian) noexcept {
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
        inline uint64_t getNameAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Name, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getClassAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Class, IsBigEndian);
        }

        [[nodiscard]] constexpr inline
        uint64_t getInstanceMethodsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr inline
        uint64_t getClassMethodsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(ClassMethods, IsBigEndian);
        }

        [[nodiscard]] constexpr
        inline uint64_t getProtocolsAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Protocols, IsBigEndian);
        }

        [[nodiscard]] constexpr inline
        uint64_t getInstancePropertiesAddress(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(InstanceProperties, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint64_t getV7(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V7, IsBigEndian);
        }

        [[nodiscard]]
        constexpr inline uint64_t getV8(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(V8, IsBigEndian);
        }

        constexpr inline ObjcClassCategory64 &
        setNameAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Name = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory64 &
        setClassAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Class = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory64 &
        setInstanceMethodsAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->InstanceMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory64 &
        setClassMethodsAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->ClassMethods = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory64 &
        setProtocolsAddress(uint64_t Value, bool IsBigEndian) noexcept {
            this->Protocols = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline ObjcClassCategory64 &
        setInstancePropertiesAddress(uint64_t Value, bool IsBigEndian) noexcept
        {
            this->InstanceProperties = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClassCategory64 &setV7(uint64_t Value, bool IsBigEndian) noexcept {
            this->V7 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }

        constexpr inline
        ObjcClassCategory64 &setV8(uint64_t Value, bool IsBigEndian) noexcept {
            this->V8 = SwitchEndianIf(Value, IsBigEndian);
            return *this;
        }
    };
}
