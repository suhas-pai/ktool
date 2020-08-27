//
//  include/Objects/FatMachOMemory.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/MachO.h"
#include "ADT/MemoryMap.h"

#include "MachOMemory.h"
#include "MemoryBase.h"

struct ConstFatMachOMemoryObject : public MemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::FatMachO;
    enum class Error {
        None,

        WrongFormat,
        SizeTooSmall,

        ZeroArchitectures,
        TooManyArchitectures,

        ArchOutOfBounds,
        ArchOverlapsArch
    };
protected:
    union {
        const uint8_t *Map;
        const MachO::FatHeader *Header;
    };

    const uint8_t *End;
    ConstFatMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
public:
    [[nodiscard]] static PointerOrError<ConstFatMachOMemoryObject, Error>
    Open(const ConstMemoryMap &Map) noexcept;

    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjKind);
    }

    [[nodiscard]] static bool errorDidMatchFormat(Error Error) noexcept;
    [[nodiscard]] static inline bool errorDidMatchFormat(uint8_t Int) noexcept {
        return errorDidMatchFormat(getErrorFromInt(Int));
    }

    [[nodiscard]] static inline Error getErrorFromInt(uint8_t Int) noexcept {
        return static_cast<Error>(Int);
    }

    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        return getMap().getRange();
    }

    [[nodiscard]] inline const MachO::FatHeader &getHeader() noexcept {
        return *Header;
    }

    [[nodiscard]]
    inline const MachO::FatHeader &getConstHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] inline bool IsBigEndian() const noexcept {
        return getConstHeader().IsBigEndian();
    }

    [[nodiscard]] inline bool Is64Bit() const noexcept {
        return getConstHeader().Is64Bit();
    }

    [[nodiscard]] inline uint32_t getArchCount() const noexcept {
        return getConstHeader().getArchCount();
    }

    [[nodiscard]]
    inline enum MachO::FatHeader::Magic getMagic() const noexcept {
        return getConstHeader().Magic;
    }

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    [[nodiscard]] inline ConstArch32List getConstArch32List() const noexcept {
        return getConstHeader().getConstArch32List();
    }

    [[nodiscard]] inline ConstArch64List getConstArch64List() const noexcept {
        return getConstHeader().getConstArch64List();
    }

    struct ArchInfo {
        Mach::CpuKind CpuKind;
        int32_t CpuSubKind;
        uint64_t Offset;
        uint64_t Size;
        uint32_t Align;
    };

    ArchInfo GetArchInfoAtIndex(uint32_t Index) const noexcept;

    enum class GetObjectResultWarningEnum {
        None,
        MachOCpuKindMismatch
    };

    struct GetObjectResult {
    public:
        using WarningEnum = GetObjectResultWarningEnum;
    protected:
        MemoryObject *Object;
        WarningEnum Warning = WarningEnum::None;
    public:
        GetObjectResult(MemoryObject *Object) noexcept : Object(Object) {}
        GetObjectResult(WarningEnum Warning) noexcept : Warning(Warning) {}
        GetObjectResult(MemoryObject *Object, WarningEnum Warning) noexcept
        : Object(Object), Warning(Warning) {}

        [[nodiscard]] inline MemoryObject *getObject() const noexcept {
            return Object;
        }

        [[nodiscard]] inline WarningEnum getWarning() const noexcept {
            return Warning;
        }
    };

    [[nodiscard]]
    GetObjectResult GetArchObjectFromInfo(const ArchInfo &Info) const noexcept;
};

struct FatMachOMemoryObject : public ConstFatMachOMemoryObject {
protected:
    FatMachOMemoryObject(const MemoryMap &Map) noexcept;
public:
    [[nodiscard]] static inline PointerOrError<FatMachOMemoryObject, Error>
    Open(const MemoryMap &Map) noexcept {
        auto Result = ConstFatMachOMemoryObject::Open(Map);
        return reinterpret_cast<FatMachOMemoryObject *>(Result.value());
    }

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline MachO::FatHeader &getHeader() noexcept {
        return const_cast<MachO::FatHeader &>(getConstHeader());
    }

    using Arch32List = MachO::FatHeader::Arch32List;
    using Arch64List = MachO::FatHeader::Arch64List;

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    [[nodiscard]] inline Arch32List getArch32List() noexcept {
        return getHeader().getArch32List();
    }

    [[nodiscard]] inline Arch64List getArch64List() noexcept {
        return getHeader().getArch64List();
    }

    using ArchInfo = ConstFatMachOMemoryObject::ArchInfo;
};
