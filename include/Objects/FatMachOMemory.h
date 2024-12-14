//
//  Objects/FatMachOMemory.h
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/Mach-O/Headers.h"
#include "ADT/MemoryMap.h"
#include "MemoryBase.h"

struct FatMachOMemoryObject : public MemoryObject {
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
        uint8_t *Map;
        MachO::FatHeader *Header;
    };

    uint8_t *End;
    FatMachOMemoryObject(const MemoryMap &Map) noexcept;
public:
    [[nodiscard]] static auto Open(const MemoryMap &Map) noexcept
        -> ExpectedPointer<FatMachOMemoryObject, Error>;

    [[nodiscard]]
    static inline auto IsOfKind(const MemoryObject &Obj) noexcept {
        return Obj.getKind() == ObjKind;
    }

    [[nodiscard]] static auto errorDidMatchFormat(Error Error) noexcept
        -> bool;

    [[nodiscard]]
    static inline auto getErrorFromInt(const uint8_t Int) noexcept {
        return static_cast<Error>(Int);
    }

    [[nodiscard]]
    static inline auto errorDidMatchFormat(const uint8_t Int) noexcept {
        return errorDidMatchFormat(getErrorFromInt(Int));
    }

    [[nodiscard]] constexpr auto getMap() const noexcept {
        return MemoryMap(Map, End);
    }

    [[nodiscard]] inline auto getMap() noexcept {
        const auto End = const_cast<uint8_t *>(this->End);
        return MemoryMap(const_cast<uint8_t *>(Map), End);
    }

    [[nodiscard]] inline ConstMemoryMap getConstMap() const noexcept override {
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline Range getRange() const noexcept override {
        return this->getMap().getRange();
    }

    [[nodiscard]] inline auto &getHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] inline auto &getConstHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] inline auto &getHeader() noexcept {
        return const_cast<MachO::FatHeader &>(this->getConstHeader());
    }

    [[nodiscard]] inline auto isBigEndian() const noexcept {
        return this->getConstHeader().isBigEndian();
    }

    [[nodiscard]] inline auto is64Bit() const noexcept {
        return this->getConstHeader().is64Bit();
    }

    [[nodiscard]] inline auto getArchCount() const noexcept {
        return this->getConstHeader().getArchCount();
    }

    [[nodiscard]] inline auto getMagic() const noexcept {
        return this->getConstHeader().Magic;
    }

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    [[nodiscard]] inline auto getArch32List() const noexcept {
        return this->getHeader().getArch32List();
    }

    [[nodiscard]] inline auto getArch64List() const noexcept {
        return this->getHeader().getArch64List();
    }

    [[nodiscard]] inline auto getConstArch32List() const noexcept {
        return this->getConstHeader().getConstArch32List();
    }

    [[nodiscard]] inline auto getConstArch64List() const noexcept {
        return this->getConstHeader().getConstArch64List();
    }

    [[nodiscard]] inline auto getArch32List() noexcept {
        return this->getHeader().getArch32List();
    }

    [[nodiscard]] inline auto getArch64List() noexcept {
        return this->getHeader().getArch64List();
    }

    struct ArchInfo {
        Mach::CpuKind CpuKind;
        int32_t CpuSubKind;
        uint64_t Offset;
        uint64_t Size;
        uint32_t Align;
    };

    [[nodiscard]] auto GetArchInfoAtIndex(uint32_t Index) const noexcept
        -> ArchInfo;

    enum class GetArchObjectError {
        None,
        InvalidArchRange,
        UnsupportedObjectKind
    };

    enum class GetArchObjectWarning {
        None,
        MachOCpuKindMismatch
    };

    struct GetArchObjectResult {
    public:
        using ErrorEnum = GetArchObjectError;
        using WarningEnum = GetArchObjectWarning;
    protected:
        ExpectedPointer<MemoryObject, ErrorEnum> ObjectOrError;
        WarningEnum Warning = WarningEnum::None;
    public:
        GetArchObjectResult(MemoryObject *const Object) noexcept
        : ObjectOrError(Object) {}

        GetArchObjectResult(std::nullptr_t) noexcept = delete;
        GetArchObjectResult(const ErrorEnum Error) noexcept
        : ObjectOrError(Error) {}

        GetArchObjectResult(const WarningEnum Warning) noexcept
        : Warning(Warning) {}

        GetArchObjectResult(MemoryObject *const Object,
                            const WarningEnum Warning) noexcept
        : ObjectOrError(Object), Warning(Warning) {}

        [[nodiscard]] inline auto hasError() const noexcept {
            return this->ObjectOrError.hasError();
        }

        [[nodiscard]] inline auto getObject() const noexcept {
            assert(!this->hasError());
            return this->ObjectOrError.value();
        }

        [[nodiscard]] inline auto getWarning() const noexcept {
            assert(!this->hasError());
            return this->Warning;
        }

        [[nodiscard]] inline auto getError() const noexcept {
            return this->ObjectOrError.getError();
        }

        [[nodiscard]] constexpr auto raw() const noexcept {
            return this->ObjectOrError;
        }
    };

    [[nodiscard]]
    auto GetArchObjectFromInfo(const ArchInfo &Info) const noexcept
        -> GetArchObjectResult;
};

