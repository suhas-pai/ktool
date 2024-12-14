//
//  Objects/MachOMemory.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/Mach-O/Headers.h"
#include "ADT/MemoryMap.h"

#include "MemoryBase.h"

struct MachOMemoryObject : public MemoryObject {
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
        uint8_t *Map;
        MachO::Header *Header;
    };

    uint8_t *End;
    MachOMemoryObject(Error Error) noexcept;

    explicit MachOMemoryObject(const MemoryMap &Map) noexcept;
    explicit
    MachOMemoryObject(ObjectKind Kind, const MemoryMap &Map) noexcept;
public:
    [[nodiscard]] static auto Open(const MemoryMap &Map) noexcept
        -> ExpectedPointer<MachOMemoryObject, Error>;

    [[nodiscard]]
    static inline auto IsOfKind(const MemoryObject &Obj) noexcept {
        return Obj.getKind() == ObjKind;
    }

    virtual ~MachOMemoryObject() noexcept = default;

    [[nodiscard]] static auto errorDidMatchFormat(Error Error) noexcept -> bool;

    [[nodiscard]]
    static inline auto getErrorFromInt(const uint8_t Int) noexcept {
        return static_cast<Error>(Int);
    }

    [[nodiscard]]
    static inline auto errorDidMatchFormat(const uint8_t Int) noexcept {
        return errorDidMatchFormat(getErrorFromInt(Int));
    }

    [[nodiscard]] inline auto getMap() const noexcept {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline auto getMap() noexcept {
        return MemoryMap(Map, End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return this->getMap();
    }

    [[nodiscard]] inline Range getRange() const noexcept override {
        return this->getMap().getRange();
    }

    [[nodiscard]] inline auto &getHeader() const noexcept {
        return *this->Header;
    }

    [[nodiscard]] inline auto &getHeader() noexcept {
        return *this->Header;
    }

    [[nodiscard]] inline auto &getConstHeader() const noexcept {
        return this->getHeader();
    }

    [[nodiscard]] inline
    auto GetLoadCommandsStorage(const bool Verify = true) const noexcept {
        return this->getHeader().GetConstLoadCmdStorage(Verify);
    }

    [[nodiscard]] inline auto isBigEndian() const noexcept {
        return this->getConstHeader().isBigEndian();
    }

    [[nodiscard]] inline auto is64Bit() const noexcept {
        return this->getConstHeader().is64Bit();
    }

    [[nodiscard]] inline auto getMagic() const noexcept {
        return this->getConstHeader().Magic;
    }

    [[nodiscard]] inline auto getCpuKind() const noexcept {
        return this->getConstHeader().getCpuKind();
    }

    [[nodiscard]] inline auto getCpuSubKind() const noexcept {
        return this->getConstHeader().getCpuSubKind();
    }

    [[nodiscard]] inline auto getFileKind() const noexcept {
        return this->getConstHeader().getFileKind();
    }

    [[nodiscard]] inline auto getHeaderFlags() const noexcept {
        return this->getConstHeader().getFlags();
    }

    [[nodiscard]] inline auto getLoadCommandsCount() const noexcept {
        return this->getConstHeader().getLoadCommandsCount();
    }

    [[nodiscard]] inline auto getLoadCommandsSize() const noexcept {
        return this->getConstHeader().getLoadCommandsSize();
    }

    constexpr auto setCpuKind(const Mach::CpuKind Kind) noexcept
        -> decltype(*this)
    {
        this->getHeader().setCpuKind(Kind);
        return *this;
    }

    constexpr auto setLoadCommandsCount(const uint32_t Count) noexcept
        -> decltype(*this)
    {
        this->getHeader().setLoadCommandsCount(Count);
        return *this;
    }

    constexpr auto setLoadCommandsSize(const uint32_t Size) noexcept
        -> decltype(*this)
    {
        this->getHeader().setLoadCommandsSize(Size);
        return *this;
    }

    constexpr
    auto setHeaderFlags(const MachO::Header::FlagsType &Flags) noexcept
        -> decltype(*this)
    {
        this->getHeader().setFlags(Flags);
        return *this;
    }

    [[nodiscard]]
    inline auto GetLoadCommandsStorage(const bool Verify = true) noexcept {
        return this->getHeader().GetLoadCmdStorage(Verify);
    }

    [[nodiscard]] inline
    auto GetConstLoadCommandsStorage(const bool Verify = true) const noexcept {
        return this->GetLoadCommandsStorage(Verify);
    }
};
