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
#include "ADT/PointerOrError.h"

#include "MachOMemory.h"
#include "MemoryBase.h"

struct ConstFatMachOMemoryObject;
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
        PointerErrorStorage<Error> ErrorStorage;
    };

    uint8_t *End;

    FatMachOMemoryObject(const MemoryMap &Map) noexcept;
    FatMachOMemoryObject(Error Error) noexcept;
public:
    [[nodiscard]]
    static FatMachOMemoryObject Open(const MemoryMap &Map) noexcept;

    [[nodiscard]]
    static inline bool IsOfKind(const MemoryObject &Obj) noexcept {
        return (Obj.getKind() == ObjectKind::FatMachO);
    }

    [[nodiscard]] bool DidMatchFormat() const noexcept override;
    [[nodiscard]] MemoryObject *ToPtr() const noexcept override;

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
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        return RelativeRange(End - Map);
    }

    [[nodiscard]] inline MachO::FatHeader &getHeader() noexcept {
        return *Header;
    }

    [[nodiscard]]
    inline const MachO::FatHeader &getConstHeader() const noexcept {
        return *Header;
    }

    [[nodiscard]] inline bool IsBigEndian() const noexcept {
        return Header->IsBigEndian();
    }

    [[nodiscard]] inline bool Is64Bit() const noexcept {
        return Header->Is64Bit();
    }

    [[nodiscard]] inline uint32_t getArchCount() const noexcept {
        return SwitchEndianIf(Header->NFatArch, this->IsBigEndian());
    }

    [[nodiscard]]
    inline const ConstFatMachOMemoryObject &toConst() const noexcept {
        return reinterpret_cast<const ConstFatMachOMemoryObject &>(*this);
    }

    [[nodiscard]] inline ConstFatMachOMemoryObject &toConst() noexcept {
        return reinterpret_cast<ConstFatMachOMemoryObject &>(*this);
    }

    [[nodiscard]] inline operator ConstFatMachOMemoryObject &() noexcept {
        return toConst();
    }

    [[nodiscard]]
    inline operator const ConstFatMachOMemoryObject &() const noexcept {
        return toConst();
    }

    [[nodiscard]]
    inline enum MachO::FatHeader::Magic getMagic() const noexcept {
        return getConstHeader().Magic;
    }

    using Arch32List = MachO::FatHeader::Arch32List;
    using Arch64List = MachO::FatHeader::Arch64List;

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    [[nodiscard]] inline Arch32List getArch32List(bool IsBigEndian) noexcept {
        return Header->getArch32List(IsBigEndian);
    }

    [[nodiscard]] inline Arch64List getArch64List(bool IsBigEndian) noexcept {
        return Header->getArch64List(IsBigEndian);
    }

    [[nodiscard]]
    inline ConstArch32List getConstArch32List(bool IsBigEndian) const noexcept {
        return Header->getConstArch32List(IsBigEndian);
    }

    [[nodiscard]]
    inline ConstArch64List getConstArch64List(bool IsBigEndian) const noexcept {
        return Header->getConstArch64List(IsBigEndian);
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

        GetObjectResultTemplate(MemoryObjectType *Object,
                                WarningEnum Warning) noexcept
        : Object(Object), Warning(Warning) {}

        [[nodiscard]] inline MemoryObjectType *getObject() const noexcept {
            return Object;
        }

        [[nodiscard]] inline WarningEnum getWarning() const noexcept {
            return Warning;
        }
    };

    using GetObjectResult = GetObjectResultTemplate<MemoryObject>;
    GetObjectResult GetArchObjectFromInfo(const ArchInfo &Info) const noexcept;
};

struct ConstFatMachOMemoryObject : public FatMachOMemoryObject {
protected:
    ConstFatMachOMemoryObject(const ConstMemoryMap &Map) noexcept;
    ConstFatMachOMemoryObject(Error Error) noexcept;
public:
    [[nodiscard]]
    static ConstFatMachOMemoryObject Open(const ConstMemoryMap &Map) noexcept;

    [[nodiscard]] inline ConstMemoryMap getMap() const noexcept {
        assert(!hasError());
        return ConstMemoryMap(Map, End);
    }

    [[nodiscard]] inline RelativeRange getRange() const noexcept override {
        assert(!hasError());
        return RelativeRange(End - Map);
    }

    [[nodiscard]] inline const MachO::FatHeader &getHeader() noexcept {
        return *Header;
    }

    [[nodiscard]]
    inline const MachO::FatHeader &getConstHeader() const noexcept {
        return *Header;
    }

    using ConstArch32List = MachO::FatHeader::ConstArch32List;
    using ConstArch64List = MachO::FatHeader::ConstArch64List;

    [[nodiscard]]
    inline ConstArch32List getConstArch32List(bool IsBigEndian) const noexcept {
        return Header->getConstArch32List(IsBigEndian);
    }

    [[nodiscard]]
    inline ConstArch64List getConstArch64List(bool IsBigEndian) const noexcept {
        return Header->getConstArch64List(IsBigEndian);
    }

    using ArchInfo = FatMachOMemoryObject::ArchInfo;
    using GetObjectResult =
        FatMachOMemoryObject::GetObjectResultTemplate<MemoryObject>;
};
