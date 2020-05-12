//
//  include/Objects/MachOMemory.h
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/Mach-O/LoadCommandStorage.h"
#include "ADT/Mach.h"
#include "ADT/MachO.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerErrorStorage.h"

#include "MemoryBase.h"

struct MachOMemoryObject : public MemoryObject {
public:
    enum class Error : uintptr_t {
        None,

        WrongFormat,
        SizeTooSmall,

        TooManyLoadCommands,
    };
protected:
    union {
        uint8_t *Map;
        MachO::Header *Header;
        PointerErrorStorage<Error> ErrorStorage;
    };

    uint8_t *End;

    MachOMemoryObject(Error Error) noexcept;
    explicit MachOMemoryObject(const MemoryMap &Map) noexcept;
public:
    static MachOMemoryObject Open(const MemoryMap &Map) noexcept;
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.GetKind() == ObjectKind::MachO);
    }

    virtual ~MachOMemoryObject() noexcept = default;

    bool DidMatchFormat() const noexcept override;
    MemoryObject *ToPtr() const noexcept override;

    inline operator ConstMachOMemoryObject &() noexcept {
        return reinterpret_cast<ConstMachOMemoryObject &>(*this);
    }

    inline operator const ConstMachOMemoryObject &() const noexcept {
        return reinterpret_cast<const ConstMachOMemoryObject &>(*this);
    }

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

    inline MachO::Header &GetHeader() noexcept { return *Header; }
    inline const MachO::Header &GetConstHeader() const noexcept {
        return *Header;
    }

    MachO::LoadCommandStorage
    GetLoadCommands(bool Verify = true) noexcept;

    MachO::ConstLoadCommandStorage
    GetConstLoadCommands(bool Verify = true) const noexcept;

    inline bool IsBigEndian() const noexcept { return Header->IsBigEndian(); }
    inline bool Is64Bit() const noexcept { return Header->IsBigEndian(); }

    inline enum MachO::Header::Magic GetMagic() const noexcept {
        return GetConstHeader().Magic;
    }

    inline Mach::CpuType GetCpuType() const noexcept {
        return GetConstHeader().GetCpuType();
    }

    inline enum MachO::Header::FileType GetFileType() const noexcept {
        return GetConstHeader().GetFileType();
    }
};

struct ConstMachOMemoryObject : public ConstMemoryObject {
    friend struct MachOMemoryObject;
public:
    using Error = MachOMemoryObject::Error;
protected:
    union {
        const uint8_t *Map;
        const MachO::Header *Header;
        PointerErrorStorage<Error> ErrorStorage;
    };

    const uint8_t *End;

    ConstMachOMemoryObject(Error Error) noexcept;
    explicit ConstMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
public:
    static ConstMachOMemoryObject Open(const ConstMemoryMap &Map) noexcept;
    static inline bool IsOfKind(const ConstMemoryObject &Obj) noexcept {
        return (Obj.GetKind() == ObjectKind::MachO);
    }

    virtual ~ConstMachOMemoryObject() noexcept = default;

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
        return RelativeRange(End - Map);
    }

    inline const MachO::Header &GetHeader() noexcept { return *Header; }
    inline const MachO::Header &GetConstHeader() const noexcept {
        return *Header;
    }

    inline operator ConstMemoryMap() const noexcept override {
        return ConstMemoryMap(this->Map, this->End);
    }

    MachO::ConstLoadCommandStorage
    GetLoadCommands(bool Verify = true) const noexcept;

    inline bool IsBigEndian() const noexcept { return Header->IsBigEndian(); }
    inline bool Is64Bit() const noexcept { return Header->Is64Bit(); }

    inline enum MachO::Header::Magic GetMagic() const noexcept {
        return GetConstHeader().Magic;
    }

    inline Mach::CpuType GetCpuType() const noexcept {
        return GetConstHeader().GetCpuType();
    }

    inline enum MachO::Header::FileType GetFileType() const noexcept {
        return GetConstHeader().GetFileType();
    }
};

static_assert(sizeof(MachOMemoryObject) == sizeof(ConstMachOMemoryObject),
              "MachOMemoryObject and its const-class are not the same size");
