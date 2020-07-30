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

struct ConstMachOMemoryObject : public MemoryObject {
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
        const uint8_t *Map;
        MachO::Header *Header;
        PointerErrorStorage<Error> ErrorStorage;
    };

    const uint8_t *End;

    ConstMachOMemoryObject(Error Error) noexcept;

    explicit ConstMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
    explicit
    ConstMachOMemoryObject(ObjectKind Kind, const ConstMemoryMap &Map) noexcept;
public:
    [[nodiscard]]
    static ConstMachOMemoryObject Open(const ConstMemoryMap &Map) noexcept;

    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjKind);
    }

    virtual ~ConstMachOMemoryObject() noexcept = default;

    [[nodiscard]] bool DidMatchFormat() const noexcept override;
    [[nodiscard]] MemoryObject *ToPtr() const noexcept override;

    [[nodiscard]] inline Error getError() const noexcept {
        return ErrorStorage.getValue();
    }

    [[nodiscard]] inline bool hasError() const noexcept override {
        return ErrorStorage.hasValue();
    }

    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return getMap();
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        return RelativeRange(End - Map);
    }

    [[nodiscard]] inline const MachO::Header &getHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] inline const MachO::Header &getConstHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] MachO::ConstLoadCommandStorage
    GetLoadCommands(bool Verify = true) const noexcept;

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

struct MachOMemoryObject : public ConstMachOMemoryObject {
protected:
    MachOMemoryObject(Error Error) noexcept;
    explicit MachOMemoryObject(const MemoryMap &Map) noexcept;
public:
    [[nodiscard]] static MachOMemoryObject Open(const MemoryMap &Map) noexcept;
    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline MachO::Header &getHeader() noexcept { return *Header; }

    [[nodiscard]] MachO::LoadCommandStorage
    GetLoadCommands(bool Verify = true) noexcept;

    [[nodiscard]] MachO::ConstLoadCommandStorage
    GetConstLoadCommands(bool Verify = true) const noexcept {
        return ConstMachOMemoryObject::GetLoadCommands(Verify);
    }
};
