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

#include "MemoryBase.h"

struct ConstMachOMemoryObject : public MemoryObject {
public:
    constexpr static auto ObjKind = ObjectKind::MachO;
    enum class Error {
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
    };

    const uint8_t *End;
    ConstMachOMemoryObject(Error Error) noexcept;

    explicit ConstMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
    explicit
    ConstMachOMemoryObject(ObjectKind Kind, const ConstMemoryMap &Map) noexcept;
public:
    [[nodiscard]] static PointerOrError<ConstMachOMemoryObject, Error>
    Open(const ConstMemoryMap &Map) noexcept;

    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjKind);
    }

    virtual ~ConstMachOMemoryObject() noexcept = default;

    [[nodiscard]] static bool errorDidMatchFormat(Error Error) noexcept;

    [[nodiscard]]
    static inline bool errorDidMatchFormat(const uint8_t Int) noexcept {
        return errorDidMatchFormat(getErrorFromInt(Int));
    }

    [[nodiscard]]
    static inline Error getErrorFromInt(const uint8_t Int) noexcept {
        return static_cast<Error>(Int);
    }

    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return getMap();
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        return getMap().getRange();
    }

    [[nodiscard]] inline const MachO::Header &getHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] inline const MachO::Header &getConstHeader() const noexcept {
        return getHeader();
    }

    [[nodiscard]] inline MachO::ConstLoadCommandStorage
    GetLoadCommandsStorage(const bool Verify = true) const noexcept {
        return getHeader().GetConstLoadCmdStorage(Verify);
    }

    [[nodiscard]] inline bool isBigEndian() const noexcept {
        return getConstHeader().isBigEndian();
    }

    [[nodiscard]] inline bool is64Bit() const noexcept {
        return getConstHeader().is64Bit();
    }

    [[nodiscard]] inline enum MachO::Header::Magic getMagic() const noexcept {
        return getConstHeader().Magic;
    }

    [[nodiscard]] inline Mach::CpuKind getCpuKind() const noexcept {
        return getConstHeader().getCpuKind();
    }

    [[nodiscard]] inline int32_t getCpuSubKind() const noexcept {
        return getConstHeader().getCpuSubKind();
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
    [[nodiscard]] static inline PointerOrError<MachOMemoryObject, Error>
    Open(const MemoryMap &Map) noexcept {
        auto Result = ConstMachOMemoryObject::Open(Map);
        return reinterpret_cast<MachOMemoryObject *>(Result.value());
    }

    [[nodiscard]] inline MemoryMap getMap() const noexcept {
        const auto End = const_cast<uint8_t *>(getConstMap().getEnd());
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline MachO::Header &getHeader() noexcept {
        return const_cast<MachO::Header &>(getConstHeader());
    }

    constexpr MachOMemoryObject &setCpuKind(const Mach::CpuKind Kind) noexcept {
        getHeader().setCpuKind(Kind);
        return *this;
    }

    constexpr
    MachOMemoryObject &setLoadCommandsCount(const uint32_t Count) noexcept {
        getHeader().setLoadCommandsCount(Count);
        return *this;
    }

    constexpr
    MachOMemoryObject &setLoadCommandsSize(const uint32_t Size) noexcept {
        getHeader().setLoadCommandsSize(Size);
        return *this;
    }

    constexpr MachOMemoryObject &
    setHeaderFlags(const MachO::Header::FlagsType &Flags) noexcept {
        getHeader().setFlags(Flags);
        return *this;
    }

    [[nodiscard]] inline MachO::LoadCommandStorage
    GetLoadCommandsStorage(const bool Verify = true) noexcept {
        return getHeader().GetLoadCmdStorage(Verify);
    }

    [[nodiscard]] inline MachO::ConstLoadCommandStorage
    GetConstLoadCommandsStorage(const bool Verify = true) const noexcept {
        return ConstMachOMemoryObject::GetLoadCommandsStorage(Verify);
    }
};
