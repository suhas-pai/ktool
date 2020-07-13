//
//  include/Objects/MachOMemory.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/Mach.h"
#include "ADT/MachO.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerErrorStorage.h"

#include "MemoryBase.h"

struct ConstMachOMemoryObject;
struct MachOMemoryObject : public MemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::MachO;
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
    explicit MachOMemoryObject(ObjectKind Kind, const MemoryMap &Map) noexcept;
public:
    [[nodiscard]] static MachOMemoryObject Open(const MemoryMap &Map) noexcept;

    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjectKind::MachO);
    }

    virtual ~MachOMemoryObject() noexcept = default;

    [[nodiscard]] bool DidMatchFormat() const noexcept override;
    [[nodiscard]] MemoryObject *ToPtr() const noexcept override;

    [[nodiscard]]
    inline const ConstMachOMemoryObject &toConst() const noexcept {
        return reinterpret_cast<const ConstMachOMemoryObject &>(*this);
    }

    [[nodiscard]] inline ConstMachOMemoryObject &toConst() noexcept {
        return reinterpret_cast<ConstMachOMemoryObject &>(*this);
    }

    [[nodiscard]] inline operator ConstMachOMemoryObject &() noexcept {
        return toConst();
    }

    [[nodiscard]]
    inline operator const ConstMachOMemoryObject &() const noexcept {
        return toConst();
    }

    [[nodiscard]] inline Error getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] inline bool hasError() const noexcept override {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        return MemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return MemoryMap(Map, End);
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        return RelativeRange(End - Map);
    }

    [[nodiscard]] inline MachO::Header &getHeader() noexcept { return *Header; }
    [[nodiscard]] inline const MachO::Header &getConstHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] MachO::LoadCommandStorage
    GetLoadCommands(bool Verify = true) noexcept;

    [[nodiscard]] MachO::ConstLoadCommandStorage
    GetConstLoadCommands(bool Verify = true) const noexcept;

    [[nodiscard]] inline bool IsBigEndian() const noexcept {
        return Header->IsBigEndian();
    }

    [[nodiscard]] inline bool Is64Bit() const noexcept {
        return Header->Is64Bit();
    }

    [[nodiscard]] inline enum MachO::Header::Magic getMagic() const noexcept {
        return getConstHeader().Magic;
    }

    [[nodiscard]] inline Mach::CpuKind getCpuKind() const noexcept {
        return getConstHeader().getCpuKind();
    }

    [[nodiscard]]
    inline enum MachO::Header::FileKind getFileKind() const noexcept {
        return getConstHeader().getFileKind();
    }
};

struct ConstMachOMemoryObject : public MachOMemoryObject {
protected:
    ConstMachOMemoryObject(Error Error) noexcept;
    explicit ConstMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
public:
    [[nodiscard]]
    static ConstMachOMemoryObject Open(const ConstMemoryMap &Map) noexcept;
    virtual ~ConstMachOMemoryObject() noexcept = default;

    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline const MachO::Header &getHeader() noexcept {
        return *Header;
    }

    [[nodiscard]] MachO::ConstLoadCommandStorage
    GetLoadCommands(bool Verify = true) const noexcept;
};
