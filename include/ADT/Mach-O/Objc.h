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
    struct ObjcClass {
        uint32_t Isa;
        uint32_t SuperClass;
        uint32_t Cache;
        uint32_t Vtable;
        uint32_t Data;
        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;
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
    };

    enum class ObjcClassRoMasks : uint32_t {
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

    struct ObjcClassRoFlags : public BasicFlags<ObjcClassRoMasks> {
    private:
        using Super = BasicFlags<ObjcClassRoMasks>;
    public:
        using Super::Super;
        using Masks = ObjcClassRoMasks;

        [[nodiscard]] inline bool IsMeta() const noexcept {
            return hasValueForMask(Masks::IsMeta);
        }

        [[nodiscard]] inline bool IsRoot() const noexcept {
            return hasValueForMask(Masks::IsRoot);
        }

        [[nodiscard]] inline bool hasCxxStructors() const noexcept {
            return hasValueForMask(Masks::HasCxxStructors);
        }

        [[nodiscard]] inline bool IsHidden() const noexcept {
            return hasValueForMask(Masks::IsHidden);
        }

        [[nodiscard]] inline bool IsException() const noexcept {
            return hasValueForMask(Masks::IsException);
        }

        [[nodiscard]] inline bool hasSwiftInitializer() const noexcept {
            return hasValueForMask(Masks::HasSwiftInitializer);
        }

        [[nodiscard]] inline bool IsARC() const noexcept {
            return hasValueForMask(Masks::IsARC);
        }

        [[nodiscard]] inline bool hasCxxDestructorOnly() const noexcept {
            return hasValueForMask(Masks::HasCxxDestructorOnly);
        }

        [[nodiscard]] inline bool hasWeakWithoutARC() const noexcept {
            return hasValueForMask(Masks::HasWeakWithoutARC);
        }

        [[nodiscard]] inline bool ForbidsAssociatedObjects() const noexcept {
            return hasValueForMask(Masks::ForbidsAssociatedObjects);
        }

        [[nodiscard]] inline bool IsFromBundle() const noexcept {
            return hasValueForMask(Masks::IsFromBundle);
        }

        [[nodiscard]] inline bool IsFuture() const noexcept {
            return hasValueForMask(Masks::IsFuture);
        }

        [[nodiscard]] inline bool IsRealized() const noexcept {
            return hasValueForMask(Masks::IsRealized);
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

    constexpr static auto IsSwiftObjcClassPreStableMask = (1ull << 0);
    constexpr static auto IsSwiftObjcClassStableMask    = (1ull << 1);
}
