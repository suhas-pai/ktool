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

    [[nodiscard]] static Error ValidateMap(const ConstMemoryMap &Map) noexcept;
protected:
    union {
        const uint8_t *Map;
        const MachO::Header *Header;
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
        assert(!hasError());
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return getMap();
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        assert(!hasError());
        return RelativeRange(End - Map);
    }

    [[nodiscard]] inline const MachO::Header &getHeader() const noexcept {
        assert(!hasError());
        return *Header;
    }

    [[nodiscard]] inline const MachO::Header &getConstHeader() const noexcept {
        return getHeader();
    }

    [[nodiscard]] inline MachO::ConstLoadCommandStorage
    GetLoadCommands(bool Verify = true) const noexcept {
        return getHeader().GetConstLoadCmdStorage(Verify);
    }

    [[nodiscard]] inline bool IsBigEndian() const noexcept {
        return getConstHeader().IsBigEndian();
    }

    [[nodiscard]] inline bool Is64Bit() const noexcept {
        return getConstHeader().Is64Bit();
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

    [[nodiscard]]
    inline MachO::Header::FlagsType getHeaderFlags() const noexcept {
        return getConstHeader().getFlags();
    }

    [[nodiscard]] inline uint32_t getLoadCommandsCount() const noexcept {
        return getConstHeader().getLoadCommandsCount();
    }

    [[nodiscard]] inline uint32_t getLoadCommandsSize() const noexcept {
        return getConstHeader().getLoadCommandsSize();
    }
};

struct MachOMemoryObject : public ConstMachOMemoryObject {
protected:
    MachOMemoryObject(Error Error) noexcept;
    explicit MachOMemoryObject(const MemoryMap &Map) noexcept;
public:
    [[nodiscard]] static MachOMemoryObject Open(const MemoryMap &Map) noexcept {
        auto Result = ConstMachOMemoryObject::Open(Map);
        return *reinterpret_cast<MachOMemoryObject *>(&Result);
    }

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        assert(!hasError());

        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline MachO::Header &getHeader() noexcept {
        return const_cast<MachO::Header &>(getConstHeader());
    }

    constexpr inline MachOMemoryObject &
    setCpuKind(Mach::CpuKind Kind) noexcept {
        getHeader().setCpuKind(Kind);
        return *this;
    }

    constexpr inline MachOMemoryObject &
    setLoadCommandsCount(uint32_t Count) noexcept {
        getHeader().setLoadCommandsCount(Count);
        return *this;
    }

    constexpr inline MachOMemoryObject &
    setLoadCommandsSize(uint32_t Size) noexcept {
        getHeader().setLoadCommandsSize(Size);
        return *this;
    }

    constexpr inline MachOMemoryObject &
    setHeaderFlags(const MachO::Header::FlagsType &Flags) noexcept {
        getHeader().setFlags(Flags);
        return *this;
    }

    [[nodiscard]] inline MachO::LoadCommandStorage
    GetLoadCommands(bool Verify = true) noexcept {
        return getHeader().GetLoadCmdStorage(Verify);
    }

    [[nodiscard]] inline MachO::ConstLoadCommandStorage
    GetConstLoadCommands(bool Verify = true) const noexcept {
        return ConstMachOMemoryObject::GetLoadCommands(Verify);
    }
};
