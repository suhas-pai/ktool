//
//  include/Objects/FatMachOMemory.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/MachO.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerOrError.h"

#include "MachOMemory.h"
#include "MemoryBase.h"

struct FatMachOMemoryObject : public MemoryObject {
public:
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
        uint8_t *Map;
        MachO::FatHeader *Header;
        PointerErrorStorage<Error> ErrorStorage;
    };

    uint8_t *End;

    FatMachOMemoryObject(const MemoryMap &Map) noexcept;
    FatMachOMemoryObject(Error Error) noexcept;
public:
    static FatMachOMemoryObject Open(const MemoryMap &Map) noexcept;
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.GetKind() == ObjectKind::FatMachO);
    }

    bool DidMatchFormat() const noexcept override;
    MemoryObject *ToPtr() const noexcept override;

    inline Error GetError() const noexcept { return ErrorStorage.GetValue(); }
    inline bool HasError() const noexcept override {
        return ErrorStorage.HasValue();
    }

    inline MemoryMap GetMap() const noexcept override {
        return MemoryMap(Map, End);
    }

    inline ConstMemoryMap GetConstMap() const noexcept override {
        return ConstMemoryMap(Map, End);
    }

    inline RelativeRange GetRange() const noexcept override {
        return RelativeRange(End - Map);
    }

    inline MachO::FatHeader &GetHeader() noexcept { return *Header; }
    inline const MachO::FatHeader &GetConstHeader() const noexcept {
        return *Header;
    }

    inline bool IsBigEndian() const noexcept { return Header->IsBigEndian(); }
    inline bool Is64Bit() const noexcept { return Header->Is64Bit(); }
    inline uint32_t GetArchCount() const noexcept {
        return SwitchEndianIf(Header->NFatArch, this->IsBigEndian());
    }

    inline operator ConstFatMachOMemoryObject &() noexcept {
        return reinterpret_cast<ConstFatMachOMemoryObject &>(*this);
    }

    inline operator const ConstFatMachOMemoryObject &() const noexcept {
        return reinterpret_cast<const ConstFatMachOMemoryObject &>(*this);
    }

    inline enum MachO::FatHeader::Magic GetMagic() const noexcept {
        return GetConstHeader().Magic;
    }

    using Arch32List = MachO::FatHeader::Arch32List;
    using Arch64List = MachO::FatHeader::Arch64List;

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    inline Arch32List GetArch32List(bool IsBigEndian) noexcept {
        return Header->GetArch32List(IsBigEndian);
    }

    inline Arch64List GetArch64List(bool IsBigEndian) noexcept {
        return Header->GetArch64List(IsBigEndian);
    }

    inline ConstArch32List GetConstArch32List(bool IsBigEndian) const noexcept {
        return Header->GetConstArch32List(IsBigEndian);
    }

    inline ConstArch64List GetConstArch64List(bool IsBigEndian) const noexcept {
        return Header->GetConstArch64List(IsBigEndian);
    }

    struct ArchInfo {
        Mach::CpuType CpuType;
        int32_t CpuSubType;
        uint64_t Offset;
        uint64_t Size;
        uint32_t Align;
    };

    ArchInfo GetArchInfoAtIndex(uint32_t Index) const noexcept;

    enum class GetObjectResultWarningEnum {
        None,
        MachOCpuTypeMismatch
    };

    template <typename MemoryObjectType>
    struct GetObjectResultTemplate {
    public:
        using WarningEnum = GetObjectResultWarningEnum;
    protected:
        MemoryObjectType *Object;
        WarningEnum Warning = WarningEnum::None;
    public:
        GetObjectResultTemplate(MemoryObjectType *Object) noexcept
        : Object(Object) {}

        GetObjectResultTemplate(WarningEnum Warning) noexcept
        : Warning(Warning) {}

        GetObjectResultTemplate(MemoryObjectType *Object, WarningEnum Warning)
        noexcept : Object(Object), Warning(Warning) {}

        inline MemoryObjectType *GetObject() const noexcept { return Object; }
        inline WarningEnum GetWarning() const noexcept {
            return Warning;
        }
    };

    using GetObjectResult = GetObjectResultTemplate<MemoryObject>;
    GetObjectResult GetArchObjectFromInfo(const ArchInfo &Info) const noexcept;
};

struct ConstFatMachOMemoryObject : public ConstMemoryObject {
public:
    using Error = FatMachOMemoryObject::Error;
protected:
    union {
        const uint8_t *Map;
        const MachO::FatHeader *Header;
        PointerErrorStorage<Error> ErrorStorage;
    };

    const uint8_t *End;

    ConstFatMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
    ConstFatMachOMemoryObject(Error Error) noexcept;
public:
    static ConstFatMachOMemoryObject Open(const ConstMemoryMap &Map) noexcept;
    static inline bool IsOfKind(const ConstMemoryObject &Obj) noexcept {
        return (Obj.GetKind() == ObjectKind::FatMachO);
    }

    bool DidMatchFormat() const noexcept override;
    ConstMemoryObject *ToPtr() const noexcept override;

    inline Error GetError() const noexcept { return ErrorStorage.GetValue(); }
    inline bool HasError() const noexcept override {
        return ErrorStorage.HasValue();
    }

    inline ConstMemoryMap GetMap() const noexcept override {
        return ConstMemoryMap(Map, End);
    }

    inline ConstMemoryMap GetConstMap() const noexcept override {
        return ConstMemoryMap(Map, End);
    }

    inline RelativeRange GetRange() const noexcept override {
        assert(!HasError());
        return RelativeRange(End - Map);
    }

    inline const MachO::FatHeader &GetHeader() noexcept {
        return *Header;
    }

    inline const MachO::FatHeader &GetConstHeader() const noexcept {
        return *Header;
    }

    inline bool IsBigEndian() const noexcept { return Header->IsBigEndian(); }
    inline bool Is64Bit() const noexcept { return Header->Is64Bit(); }
    inline uint32_t GetArchCount() const noexcept {
        return Header->GetArchsCount();
    }

    inline enum MachO::FatHeader::Magic GetMagic() const noexcept {
        return GetConstHeader().Magic;
    }

    using Arch32List = MachO::FatHeader::Arch32List;
    using Arch64List = MachO::FatHeader::Arch64List;

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    inline ConstArch32List GetConstArch32List(bool IsBigEndian) const noexcept {
        return Header->GetConstArch32List(IsBigEndian);
    }

    inline ConstArch64List GetConstArch64List(bool IsBigEndian) const noexcept {
        return Header->GetConstArch64List(IsBigEndian);
    }

    using ArchInfo = FatMachOMemoryObject::ArchInfo;
    using GetObjectResult =
        FatMachOMemoryObject::GetObjectResultTemplate<ConstMemoryObject>;

    ArchInfo GetArchInfoAtIndex(uint32_t Index) const noexcept;
    GetObjectResult GetArchObjectFromInfo(const ArchInfo &Info) const noexcept;
};

static_assert(sizeof(FatMachOMemoryObject) == sizeof(ConstFatMachOMemoryObject),
              "FatMachOMemoryObject and its const-class are not the same size");
