//
//  include/ADT/Mach-O/ObjC.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include "ADT/BasicMasksHandler.h"

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

        [[nodiscard]] inline bool IsSwift(bool IsBigEndian) const noexcept {
            const auto Data = SwitchEndianIf(this->Data, IsBigEndian);
            const auto Mask =
                (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

            return (Data & Mask);
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

        [[nodiscard]] inline bool IsSwift(bool IsBigEndian) const noexcept {
            const auto Data = SwitchEndianIf(this->Data, IsBigEndian);
            const auto Mask =
                (IsSwiftObjcClassPreStableMask | IsSwiftObjcClassStableMask);

            return (Data & Mask);
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

        [[nodiscard]] inline bool IsMeta() const noexcept {
            return hasFlag(Flags::IsMeta);
        }

        [[nodiscard]] inline bool IsRoot() const noexcept {
            return hasFlag(Flags::IsRoot);
        }

        [[nodiscard]] inline bool hasCxxStructors() const noexcept {
            return hasFlag(Flags::HasCxxStructors);
        }

        [[nodiscard]] inline bool IsHidden() const noexcept {
            return hasFlag(Flags::IsHidden);
        }

        [[nodiscard]] inline bool IsException() const noexcept {
            return hasFlag(Flags::IsException);
        }

        [[nodiscard]] inline bool hasSwiftInitializer() const noexcept {
            return hasFlag(Flags::HasSwiftInitializer);
        }

        [[nodiscard]] inline bool IsARC() const noexcept {
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

        [[nodiscard]] inline bool IsFromBundle() const noexcept {
            return hasFlag(Flags::IsFromBundle);
        }

        [[nodiscard]] inline bool IsFuture() const noexcept {
            return hasFlag(Flags::IsFuture);
        }

        [[nodiscard]] inline bool IsRealized() const noexcept {
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

        [[nodiscard]]
        inline ObjcClassRoFlags getFlags(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        inline ObjcClassRo &
        setFlags(const ObjcClassRoFlags &Flags, bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
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
        inline ObjcClassRoFlags getFlags(bool IsBigEndian) const noexcept {
            return SwitchEndianIf(Flags, IsBigEndian);
        }

        inline ObjcClassRo64 &
        setFlags(const ObjcClassRoFlags &Flags, bool IsBigEndian) noexcept {
            this->Flags = SwitchEndianIf(Flags.value(), IsBigEndian);
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
    };
}
