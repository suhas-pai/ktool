//
//  include/ADT/Mach-O/BindInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <memory>

#include "ADT/EnumHelper.h"
#include "ADT/LargestIntHelper.h"
#include "ADT/SpecificCapArray.h"
#include "ADT/PointerOrError.h"
#include "ADT/PointerFlagStorage.h"

#include "Utils/PointerUtils.h"
#include "LoadCommandsCommon.h"
#include "SegmentUtil.h"

namespace MachO {
    enum class BindByteOpcode : uint8_t {
        Done,
        SetDylibOrdinalImm          = 0x10,
        SetDylibOrdinalUleb         = 0x20,
        SetDylibSpecialImm          = 0x30,
        SetSymbolTrailingFlagsImm   = 0x40,
        SetKindImm                  = 0x50,
        SetAddendSleb               = 0x60,
        SetSegmentAndOffsetUleb     = 0x70,
        AddAddrUleb                 = 0x80,
        DoBind                      = 0x90,
        DoBindAddAddrUleb           = 0xA0,
        DoBindAddAddrImmScaled      = 0xB0,
        DoBindUlebTimesSkippingUleb = 0xC0,
        Threaded                    = 0xD0
    };

    using namespace std::literals;

    template <BindByteOpcode>
    struct BindByteOpcodeInfo {};

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::Done> {
        constexpr static const auto Kind = BindByteOpcode::Done;

        constexpr static const auto Name = "BIND_OPCODE_DONE"sv;
        constexpr static const auto Description = "Done"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetDylibOrdinalImm> {
        constexpr static const auto Kind = BindByteOpcode::SetDylibOrdinalImm;

        constexpr static const auto Name =
            "BIND_OPCODE_SET_DYLIB_ORDINAL_IMM"sv;
        constexpr static const auto Description =
            "Set Dylib-Ordinal (Immediate)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetDylibOrdinalUleb> {
        constexpr static const auto Kind = BindByteOpcode::SetDylibOrdinalUleb;

        constexpr static const auto Name =
            "BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB"sv;
        constexpr static const auto Description =
            "Set Dylib-Ordinal (Uleb128)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetDylibSpecialImm> {
        constexpr static const auto Kind = BindByteOpcode::SetDylibSpecialImm;

        constexpr static const auto Name =
            "BIND_OPCODE_SET_DYLIB_SPECIAL_IMM"sv;
        constexpr static const auto Description =
            "Set Special Dylib-Ordinal (Immediate)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetSymbolTrailingFlagsImm> {
        constexpr static const auto Kind =
            BindByteOpcode::SetSymbolTrailingFlagsImm;
        constexpr static const auto Name =
            "BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM"sv;
        constexpr static const auto Description =
            "Set Special Dylib-Ordinal (Immediate)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetKindImm> {
        constexpr static const auto Kind = BindByteOpcode::SetKindImm;

        constexpr static const auto Name = "BIND_OPCODE_SET_TYPE_IMM"sv;
        constexpr static const auto Description =
            "Set Special Dylib-Ordinal (Immediate)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetAddendSleb> {
        constexpr static const auto Kind = BindByteOpcode::SetAddendSleb;

        constexpr static const auto Name = "BIND_OPCODE_SET_ADDEND_SLEB"sv;
        constexpr static const auto Description = "Set Addend (Sleb128)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::SetSegmentAndOffsetUleb> {
        constexpr static const auto Kind =
            BindByteOpcode::SetSegmentAndOffsetUleb;

        constexpr static const auto Name =
            "BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB"sv;
        constexpr static const auto Description =
            "Set Addend (Sleb128)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::AddAddrUleb> {
        constexpr static const auto Kind = BindByteOpcode::AddAddrUleb;

        constexpr static const auto Name = "BIND_OPCODE_ADD_ADDR_ULEB"sv;
        constexpr static const auto Description = "Add Address (Uleb128)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::DoBind> {
        constexpr static const auto Kind = BindByteOpcode::DoBind;

        constexpr static const auto Name = "BIND_OPCODE_DO_BIND"sv;
        constexpr static const auto Description = "Do Bind"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::DoBindAddAddrUleb> {
        constexpr static const auto Kind = BindByteOpcode::DoBindAddAddrUleb;

        constexpr static const auto Name =
            "BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB"sv;
        constexpr static const auto Description =
            "Do Bind And Add Address (Uleb)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::DoBindAddAddrImmScaled> {
        constexpr static const auto Kind =
            BindByteOpcode::DoBindAddAddrImmScaled;
        constexpr static const auto Name =
            "BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED"sv;
        constexpr static const auto Description =
            "Do Bind And Add Address Scaled (Immediate)"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::DoBindUlebTimesSkippingUleb> {
        constexpr static const auto Kind =
            BindByteOpcode::DoBindUlebTimesSkippingUleb;
        constexpr static const auto Name =
            "BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB"sv;
        constexpr static const auto Description =
            "Do Bind Uleb128 Skipping Uleb128 Bytes"sv;
    };

    template <>
    struct BindByteOpcodeInfo<BindByteOpcode::Threaded> {
        constexpr static const auto Kind = BindByteOpcode::Threaded;

        constexpr static const auto Name = "BIND_OPCODE_THREADED"sv;
        constexpr static const auto Description = "Threaded"sv;
    };

    [[nodiscard]] constexpr const std::string_view &
    BindByteOpcodeGetName(BindByteOpcode Opcode) noexcept {
        using Enum = BindByteOpcode;
        switch (Opcode) {
            case Enum::Done:
                return BindByteOpcodeInfo<Enum::Done>::Name;
            case Enum::SetDylibOrdinalImm:
                return BindByteOpcodeInfo<Enum::SetDylibOrdinalImm>::Name;
            case Enum::SetDylibOrdinalUleb:
                return BindByteOpcodeInfo<Enum::SetDylibOrdinalUleb>::Name;
            case Enum::SetDylibSpecialImm:
                return BindByteOpcodeInfo<Enum::SetDylibSpecialImm> ::Name;
            case Enum::SetSymbolTrailingFlagsImm:
                return
                    BindByteOpcodeInfo<Enum::SetSymbolTrailingFlagsImm>::Name;
            case Enum::SetKindImm:
                return BindByteOpcodeInfo<Enum::SetKindImm>::Name;
            case Enum::SetAddendSleb:
                return BindByteOpcodeInfo<Enum::SetAddendSleb>::Name;
            case Enum::SetSegmentAndOffsetUleb:
                return
                    BindByteOpcodeInfo<Enum::SetSegmentAndOffsetUleb>::Name;
            case Enum::AddAddrUleb:
                return BindByteOpcodeInfo<Enum::AddAddrUleb>::Name;
            case Enum::DoBind:
                return BindByteOpcodeInfo<Enum::DoBind>::Name;
            case Enum::DoBindAddAddrUleb:
                return BindByteOpcodeInfo<Enum::DoBindAddAddrUleb>::Name;
            case Enum::DoBindAddAddrImmScaled:
                return BindByteOpcodeInfo<Enum::DoBindAddAddrImmScaled>::Name;
            case Enum::DoBindUlebTimesSkippingUleb:
                return
                    BindByteOpcodeInfo<Enum::DoBindUlebTimesSkippingUleb>::Name;
            case Enum::Threaded:
                return BindByteOpcodeInfo<Enum::Threaded>::Name;
        }

        return EmptyStringValue;
    }

    [[nodiscard]] constexpr std::string_view
    BindByteOpcodeGetDescription(BindByteOpcode Opcode) noexcept {
        using Enum = BindByteOpcode;
        switch (Opcode) {
            case Enum::Done:
                return BindByteOpcodeInfo<Enum::Done>::Description;
            case Enum::SetDylibOrdinalImm:
                return
                    BindByteOpcodeInfo<Enum::SetDylibOrdinalImm>::Description;
            case Enum::SetDylibOrdinalUleb:
                return
                    BindByteOpcodeInfo<Enum::SetDylibOrdinalUleb>::Description;
            case Enum::SetDylibSpecialImm:
                return
                    BindByteOpcodeInfo<Enum::SetDylibSpecialImm>::Description;
            case Enum::SetSymbolTrailingFlagsImm:
                return
                    BindByteOpcodeInfo<Enum::SetSymbolTrailingFlagsImm>
                        ::Description;
            case Enum::SetKindImm:
                return
                    BindByteOpcodeInfo<Enum::SetKindImm>::Description;
            case Enum::SetAddendSleb:
                return
                    BindByteOpcodeInfo<Enum::SetAddendSleb>::Description;
            case Enum::SetSegmentAndOffsetUleb:
                return
                    BindByteOpcodeInfo<Enum::SetSegmentAndOffsetUleb>
                        ::Description;
            case Enum::AddAddrUleb:
                return
                    BindByteOpcodeInfo<Enum::AddAddrUleb>::Description;
            case Enum::DoBind:
                return BindByteOpcodeInfo<Enum::DoBind>::Description;
            case Enum::DoBindAddAddrUleb:
                return
                    BindByteOpcodeInfo<Enum::DoBindAddAddrUleb>::Description;
            case Enum::DoBindAddAddrImmScaled:
                return
                    BindByteOpcodeInfo<Enum::DoBindAddAddrImmScaled>
                        ::Description;
            case Enum::DoBindUlebTimesSkippingUleb:
                return
                    BindByteOpcodeInfo<Enum::DoBindUlebTimesSkippingUleb>
                        ::Description;
            case Enum::Threaded:
                return BindByteOpcodeInfo<Enum::Threaded>::Description;
        }

        return EmptyStringValue;
    }

    enum class BindByteSubOpcode : uint8_t {
        ThreadedSetBindOrdinalTableSizeUleb,
        ThreadedApply
    };

    enum class BindWriteKind : uint8_t {
        None,
        Pointer = 1,
        TextAbsolute32,
        TextPcrel32
    };

    template <BindWriteKind>
    struct BindWriteKindInfo {};

    template <>
    struct BindWriteKindInfo<BindWriteKind::Pointer> {
        constexpr static const auto Kind = BindWriteKind::Pointer;

        constexpr static const auto Name = "BIND_TYPE_POINTER"sv;
        constexpr static const auto Description = "Pointer"sv;
    };

    template <>
    struct BindWriteKindInfo<BindWriteKind::TextAbsolute32> {
        constexpr static const auto Kind = BindWriteKind::TextAbsolute32;

        constexpr static const auto Name = "BIND_TYPE_TEXT_ABSOLUTE32"sv;
        constexpr static const auto Description = "Text Absolute (32-Bit)"sv;
    };

    template <>
    struct BindWriteKindInfo<BindWriteKind::TextPcrel32> {
        constexpr static const auto Kind = BindWriteKind::TextPcrel32;

        constexpr static const auto Name = "BIND_TYPE_TEXT_PCREL32"sv;
        constexpr static const auto Description = "PC Relative (32-Bit)"sv;
    };

    [[nodiscard]] constexpr
    const std::string_view &BindWriteKindGetName(BindWriteKind Kind) noexcept {
        using Enum = BindWriteKind;
        switch (Kind) {
            case Enum::None:
                return EmptyStringValue;
            case Enum::Pointer:
                return BindWriteKindInfo<Enum::Pointer>::Name;
            case Enum::TextAbsolute32:
                return BindWriteKindInfo<Enum::TextAbsolute32>::Name;
            case Enum::TextPcrel32:
                return BindWriteKindInfo<Enum::TextPcrel32>::Name;
        }

        return EmptyStringValue;
    }

    [[nodiscard]] constexpr const std::string_view &
    BindWriteKindGetDescription(BindWriteKind Kind) noexcept {
        using Enum = BindWriteKind;
        switch (Kind) {
            case Enum::None:
                return EmptyStringValue;
            case Enum::Pointer:
                return BindWriteKindInfo<Enum::Pointer>::Description;
            case Enum::TextAbsolute32:
                return BindWriteKindInfo<Enum::TextAbsolute32>::Description;
            case Enum::TextPcrel32:
                return BindWriteKindInfo<Enum::TextPcrel32>::Description;
        }

        return EmptyStringValue;
    }

    [[nodiscard]]
    constexpr uint64_t BindWriteKindGetLongestDescription() noexcept {
        const auto Result =
            EnumHelper<BindWriteKind>::GetLongestAssocLength(
                BindWriteKindGetName);

        return Result;
    }

    enum class BindByteDylibSpecialOrdinal : uint8_t {
        DylibSelf = 0,
        DylibMainExecutable = static_cast<uint8_t>(-1),
        DylibFlatLookup = static_cast<uint8_t>(-2),
        DylibWeakLookup = static_cast<uint8_t>(-3),
    };

    template <BindByteDylibSpecialOrdinal>
    struct BindByteDylibSpecialOrdinalInfo {};

    template <>
    struct BindByteDylibSpecialOrdinalInfo<
        BindByteDylibSpecialOrdinal::DylibSelf>
    {
        constexpr static const auto Kind =
            BindByteDylibSpecialOrdinal::DylibSelf;

        constexpr static const auto Name = "BIND_SPECIAL_DYLIB_SELF"sv;
        constexpr static const auto Description = "Dylib-Self"sv;
    };

    template <>
    struct BindByteDylibSpecialOrdinalInfo<
        BindByteDylibSpecialOrdinal::DylibMainExecutable>
    {
        constexpr static const auto Kind =
            BindByteDylibSpecialOrdinal::DylibMainExecutable;

        constexpr static const auto Name =
            "BIND_SPECIAL_DYLIB_MAIN_EXECUTABLE"sv;
        constexpr static const auto Description = "Main-Executable"sv;
    };

    template <>
    struct BindByteDylibSpecialOrdinalInfo<
        BindByteDylibSpecialOrdinal::DylibFlatLookup>
    {
        constexpr static const auto Kind =
            BindByteDylibSpecialOrdinal::DylibFlatLookup;

        constexpr static const auto Name = "BIND_SPECIAL_DYLIB_FLAT_LOOKUP"sv;
        constexpr static const auto Description = "Flat-Lookup"sv;
    };

    template <>
    struct BindByteDylibSpecialOrdinalInfo<
        BindByteDylibSpecialOrdinal::DylibWeakLookup>
    {
        constexpr static const auto Kind =
            BindByteDylibSpecialOrdinal::DylibWeakLookup;

        constexpr static const auto Name = "BIND_SPECIAL_DYLIB_WEAK_LOOKUP"sv;
        constexpr static const auto Description = "Weak-Lookup"sv;
    };

    [[nodiscard]] constexpr const std::string_view &
    BindByteDylibSpecialOrdinalGetName(
        BindByteDylibSpecialOrdinal Ordinal) noexcept
    {
        using Enum = BindByteDylibSpecialOrdinal;
        switch (Ordinal) {
            case Enum::DylibSelf:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibSelf>::Name;
            case Enum::DylibMainExecutable:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibMainExecutable>
                        ::Name;
            case Enum::DylibFlatLookup:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibFlatLookup>
                        ::Name;
            case Enum::DylibWeakLookup:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibWeakLookup>
                        ::Name;
        }
    }

    [[nodiscard]] constexpr const std::string_view &
    BindByteDylibSpecialOrdinalGetDescription(
        BindByteDylibSpecialOrdinal Ordinal) noexcept
    {
        using Enum = BindByteDylibSpecialOrdinal;
        switch (Ordinal) {
            case Enum::DylibSelf:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibSelf>
                        ::Description;
            case Enum::DylibMainExecutable:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibMainExecutable>
                        ::Description;
            case Enum::DylibFlatLookup:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibFlatLookup>
                        ::Description;
            case Enum::DylibWeakLookup:
                return
                    BindByteDylibSpecialOrdinalInfo<Enum::DylibWeakLookup>
                        ::Description;
        }
    }

    enum class BindSymbolFlagsEnum : uint8_t {
        WeakImport        = 1,
        NonWeakDefinition = 1ull << 4
    };

    struct BindSymbolFlags : public BasicMasksHandler<BindSymbolFlagsEnum> {
    private:
        using Base = BasicMasksHandler<BindSymbolFlagsEnum>;
    public:
        using Enum = BindSymbolFlagsEnum;
        using Base::Base;

        [[nodiscard]] constexpr inline bool isWeakImport() const noexcept {
            return hasValueForMask(Enum::WeakImport);
        }

        [[nodiscard]]
        constexpr inline bool hasNonWeakDefinition() const noexcept {
            return hasValueForMask(Enum::NonWeakDefinition);
        }
    };

    struct BindByte : private DyldInfoByteMasksAndShiftsHandler {
    public:
        constexpr static auto DoneOpcode = BindByteOpcode::Done;

        constexpr BindByte() noexcept = default;
        constexpr BindByte(uint8_t Byte) noexcept
        : DyldInfoByteMasksAndShiftsHandler(Byte) {}

        using Masks = DyldInfoByteMasks;
        using Opcode = BindByteOpcode;
        using SubOpcode = BindByteSubOpcode;
        using WriteKind = BindWriteKind;

        [[nodiscard]]
        constexpr inline BindByte::Opcode getOpcode() const noexcept {
            return BindByte::Opcode(getValueForMaskNoShift(Masks::Opcode));
        }

        [[nodiscard]] constexpr inline uint8_t getImmediate() const noexcept {
            return getValueForMaskNoShift(Masks::Immediate);
        }

        constexpr inline BindByte &setOpcode(Opcode Opcode) noexcept {
            const auto OpcodeInt = static_cast<IntegerType>(Opcode);

            setValueForMaskNoShift(Masks::Opcode, OpcodeInt);
            return *this;
        }

        constexpr inline BindByte &setImmediate(uint8_t Immediate) noexcept {
            setValueForMaskNoShift(Masks::Immediate, Immediate);
            return *this;
        }
    };

    static_assert(sizeof(BindByte) == sizeof(uint8_t),
                  "BindByte has an invalid size");

    enum class BindOpcodeParseError {
        None,

        InvalidLeb128,
        InvalidSegmentIndex,
        InvalidString,

        NotEnoughThreadedBinds,
        TooManyThreadedBinds,
        InvalidThreadOrdinal,

        EmptySymbol,

        IllegalBindOpcode,
        OutOfBoundsSegmentAddr,

        UnrecognizedBindWriteKind,
        UnrecognizedBindOpcode,
        UnrecognizedBindSubOpcode,
        UnrecognizedSpecialDylibOrdinal,

        NoDylibOrdinal,
        NoSegmentIndex,
        NoWriteKind,
    };

    enum class BindInfoKind {
        Normal,
        Lazy,
        Weak
    };

    struct BindOpcodeIterateInfo {
        BindInfoKind Kind;

        int64_t Addend = 0;
        int64_t DylibOrdinal = -1;

        BindOpcodeParseError Error = BindOpcodeParseError::None;
        std::string_view SymbolName;

        int64_t SegmentIndex = -1;
        uint64_t SegOffset = 0;
        uint64_t ThreadedCount = 0;

        union {
            int64_t AddAddr = 0;
            uint64_t Count;
        };

        union {
            uint64_t Skip = 0;
            uint64_t Scale;
            BindByteSubOpcode SubOpcode;
        };

        BindSymbolFlags Flags;
        BindWriteKind WriteKind;

        [[nodiscard]] constexpr inline bool hasError() const noexcept {
            return (Error != BindOpcodeParseError::None);
        }

        [[nodiscard]]
        constexpr inline BindOpcodeParseError getError() const noexcept {
            return Error;
        }
    };

    struct BindOpcodeIteratorEnd {};

    template <BindInfoKind BindKind>
    struct BindOpcodeIteratorBase {
    public:
        using ErrorEnum = BindOpcodeParseError;
    protected:
        typename NakedOpcodeList::Iterator Iter;
        std::unique_ptr<BindOpcodeIterateInfo> Info;

        const BindByte *Prev;
        bool ReachedEnd : 1;
    public:
        constexpr explicit
        BindOpcodeIteratorBase(const BindByte *Begin,
                               const BindByte *End) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End)),
          ReachedEnd(false)
        {
            Info = std::make_unique<BindOpcodeIterateInfo>();
            Info->Kind = BindKind;

            Advance();
        }

        constexpr explicit
        BindOpcodeIteratorBase(
            const BindByte *Begin,
            const BindByte *End,
            std::unique_ptr<BindOpcodeIterateInfo> &&Info) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End)),
          Info(std::move(Info)),
          ReachedEnd(false)
        {
            this->Info->Kind = BindKind;
            Advance();
        }

        BindOpcodeIteratorBase(const BindOpcodeIteratorBase &) = delete;

        [[nodiscard]]
        inline uint64_t getOffset(const uint8_t *Base) const noexcept {
            assert(Base <= Iter->getPtr());
            return (Iter->getPtr() - Base);
        }

        [[nodiscard]]
        inline uint64_t getOffset(const BindByte *Ptr) const noexcept {
            const auto Base = reinterpret_cast<const uint8_t *>(Ptr);

            assert(Base <= Iter->getPtr());
            return (Iter->getPtr() - Base);
        }

        [[nodiscard]] inline const uint8_t *getPtr() const noexcept {
            return Iter->getPtr();
        }

        [[nodiscard]] constexpr inline bool isAtEnd() const noexcept {
            if constexpr (BindKind == BindInfoKind::Lazy) {
                return (Iter.isAtEnd() && ReachedEnd);
            } else {
                return ReachedEnd;
            }
        }
        [[nodiscard]] constexpr
        inline const BindOpcodeIteratorBase &operator*() const noexcept {
            return *this;
        }

        [[nodiscard]] constexpr
        inline const BindOpcodeIteratorBase *operator->() const noexcept {
            return this;
        }

        [[nodiscard]]
        constexpr inline BindOpcodeIterateInfo &getInfo() noexcept {
            return *Info;
        }

        [[nodiscard]] constexpr
        inline const BindOpcodeIterateInfo &getInfo() const noexcept {
            return *Info;
        }

        [[nodiscard]]
        constexpr inline const BindByte &getByte() const noexcept {
            return *reinterpret_cast<const BindByte *>(Prev);
        }

        [[nodiscard]] constexpr inline bool hasError() const noexcept {
            return Info->hasError();
        }

        [[nodiscard]] constexpr inline ErrorEnum getError() const noexcept {
            return Info->getError();
        }

        constexpr inline BindOpcodeIteratorBase &operator++() noexcept {
            Info->Error = Advance();
            return *this;
        }

        constexpr inline BindOpcodeIteratorBase &operator++(int) noexcept {
            return ++(*this);
        }

        constexpr
        inline BindOpcodeIteratorBase &operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] constexpr
        inline bool operator==(const BindOpcodeIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr inline
        bool operator!=(const BindOpcodeIteratorEnd &End) const noexcept {
            return !(*this == End);
        }

        constexpr inline BindOpcodeParseError Advance() noexcept {
            using ErrorEnum = BindOpcodeParseError;

            const auto BytePtr = Iter->getAsByte<BindByte>();
            const auto &Byte = *BytePtr;

            Info->Error = ErrorEnum::None;
            Prev = BytePtr;
            Iter++;

            if (Iter.isAtEnd()) {
                ReachedEnd = true;
                return ErrorEnum::None;
            }

            switch (Byte.getOpcode()) {
                case BindByte::Opcode::Done:
                    ReachedEnd = true;
                    return ErrorEnum::None;
                case BindByte::Opcode::SetDylibOrdinalImm:
                    if constexpr (BindKind == BindInfoKind::Weak) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    Info->DylibOrdinal = Byte.getImmediate();
                    return ErrorEnum::None;
                case BindByte::Opcode::SetDylibOrdinalUleb:
                    if constexpr (BindKind == BindInfoKind::Weak) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    if (const auto Uleb = Iter->ReadUleb128()) {
                        Info->DylibOrdinal = Uleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::SetDylibSpecialImm: {
                    if constexpr (BindKind == BindInfoKind::Weak) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    auto DylibOrdinal = Byte.getImmediate();
                    if (DylibOrdinal == 0) {
                        Info->DylibOrdinal = DylibOrdinal;
                        return ErrorEnum::None;
                    }

                    // Sign-Extend the Dylib-Ordinal.
                    DylibOrdinal = (0xF0 | DylibOrdinal);
                    switch (BindByteDylibSpecialOrdinal(DylibOrdinal)) {
                        case BindByteDylibSpecialOrdinal::DylibSelf:
                        case BindByteDylibSpecialOrdinal::DylibMainExecutable:
                        case BindByteDylibSpecialOrdinal::DylibFlatLookup:
                        case BindByteDylibSpecialOrdinal::DylibWeakLookup:
                            Info->DylibOrdinal = DylibOrdinal;
                            return ErrorEnum::None;
                    }

                    return ErrorEnum::UnrecognizedSpecialDylibOrdinal;
                }

                case BindByte::Opcode::SetSymbolTrailingFlagsImm:
                    Info->Flags = BindSymbolFlags(Byte.getImmediate());
                    if (const auto StringOpt = Iter->ParseString()) {
                        const auto &String = StringOpt.value();
                        if (String.empty()) {
                            return ErrorEnum::EmptySymbol;
                        }

                        Info->SymbolName = String;
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidString;
                case BindByte::Opcode::SetKindImm:
                    if constexpr (BindKind == BindInfoKind::Lazy) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    Info->WriteKind = BindWriteKind(Byte.getImmediate());
                    switch (Info->WriteKind) {
                        case BindByte::WriteKind::None:
                            break;
                        case BindByte::WriteKind::Pointer:
                        case BindByte::WriteKind::TextAbsolute32:
                        case BindByte::WriteKind::TextPcrel32:
                            return ErrorEnum::None;
                    }

                    return ErrorEnum::UnrecognizedBindWriteKind;
                case BindByte::Opcode::SetAddendSleb:
                    if (const auto Sleb = Iter->ReadSleb128()) {
                        Info->Addend = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::SetSegmentAndOffsetUleb:
                    if (const auto Uleb = Iter->ReadUleb128()) {
                        Info->SegOffset = Uleb.value();
                        Info->SegmentIndex = Byte.getImmediate();

                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::AddAddrUleb:
                    if constexpr (BindKind == BindInfoKind::Lazy) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    if (const auto Sleb = Iter->ReadSleb128()) {
                        Info->AddAddr = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::DoBind:
                    return ErrorEnum::None;
                case BindByte::Opcode::DoBindAddAddrUleb:
                    if constexpr (BindKind == BindInfoKind::Lazy) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    if (const auto Sleb = Iter->ReadSleb128()) {
                        Info->AddAddr = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::DoBindAddAddrImmScaled: {
                    if constexpr (BindKind == BindInfoKind::Lazy) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    Info->Scale = Byte.getImmediate();
                    return ErrorEnum::None;
                }
                case BindByte::Opcode::DoBindUlebTimesSkippingUleb:
                    if constexpr (BindKind == BindInfoKind::Lazy) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    if (const auto Count = Iter->ReadUleb128()) {
                        if (const auto Skip = Iter->ReadSleb128()) {
                            const auto SkipValue = Skip.value();
                            const auto CountValue = Count.value();

                            Info->Skip = SkipValue;
                            Info->Count = CountValue;

                            return ErrorEnum::None;
                        }
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::Threaded:
                    Info->SubOpcode = BindByteSubOpcode(Byte.getImmediate());
                    switch (Info->SubOpcode) {
                        using Enum = BindByteSubOpcode;
                        case Enum::ThreadedSetBindOrdinalTableSizeUleb:
                            if (const auto Uleb = Iter->ReadUleb128()) {
                                const auto Count = Uleb.value();

                                Info->ThreadedCount = Count;
                                return ErrorEnum::None;
                            }

                            return ErrorEnum::InvalidLeb128;
                        case Enum::ThreadedApply:
                            return ErrorEnum::None;
                    }

                    return ErrorEnum::UnrecognizedBindSubOpcode;
            }

            return ErrorEnum::UnrecognizedBindOpcode;
        }
    };

    template <BindInfoKind BindKind>
    struct BindOpcodeListBase {
        using IteratorType = BindOpcodeIteratorBase<BindKind>;
    protected:
        const BindByte *Begin;
        const BindByte *End;
        bool Is64Bit : 1;
    public:
        constexpr explicit
        BindOpcodeListBase(const uint8_t *Begin,
                           const uint8_t *End,
                           bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const BindByte *>(Begin)),
          End(reinterpret_cast<const BindByte *>(End)),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] constexpr inline IteratorType begin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]]
        constexpr inline BindOpcodeIteratorEnd end() const noexcept {
            return BindOpcodeIteratorEnd();
        }

        [[nodiscard]] constexpr inline
        uint64_t getOffsetFromIter(const IteratorType &Iter) const noexcept {
            return (Iter->getPtr() - Begin);
        }
    };

    struct BindNakedOpcodeList : public NakedOpcodeList {
        using NakedOpcodeList::NakedOpcodeList;
        struct Iterator : public NakedOpcodeList::Iterator {
            explicit Iterator(const uint8_t *Iter, const uint8_t *End) noexcept
            : NakedOpcodeList::Iterator(Iter, End) {}

            [[nodiscard]] inline BindByte getByte() const noexcept {
                return *reinterpret_cast<const BindByte *>(this->getPtr());
            }
        };

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(Begin, End);
        }
    };

    using ConstBindNakedOpcodeList = BindNakedOpcodeList;

    using BindOpcodeList = BindOpcodeListBase<BindInfoKind::Normal>;
    using ConstBindOpcodeList = BindOpcodeListBase<BindInfoKind::Normal>;

    using LazyBindOpcodeList = BindOpcodeListBase<BindInfoKind::Lazy>;
    using ConstLazyBindOpcodeList = BindOpcodeListBase<BindInfoKind::Lazy>;

    using WeakBindOpcodeList = BindOpcodeListBase<BindInfoKind::Weak>;
    using ConstWeakBindOpcodeList = BindOpcodeListBase<BindInfoKind::Weak>;

    struct BindActionInfo {
        BindInfoKind Kind;
        BindWriteKind WriteKind;

        int64_t Addend = 0;
        int64_t DylibOrdinal = -1;

        std::string_view SymbolName;

        int64_t SegmentIndex = -1;
        uint64_t SegOffset = 0;
        uint64_t AddrInSeg = 0;

        bool NewSymbolName : 1;
        BindSymbolFlags Flags;

        [[nodiscard]] constexpr inline bool hasDylibOrdinal() const noexcept {
            return (DylibOrdinal != -1);
        }
    };

    struct BindOpcodeThreadedData {
        BindWriteKind Kind;
        std::string_view SymbolName;

        int64_t DylibOrdinal;
        int64_t Addend;
        uint8_t Flags;
    };

    struct BindActionIterateInfo : public BindOpcodeIterateInfo {
        using ErrorEnum = BindOpcodeParseError;

        [[nodiscard]]
        constexpr struct BindActionInfo getAction() const noexcept {
            const auto Result = BindActionInfo {
                .Kind = this->Kind,
                .WriteKind = this->WriteKind,
                .Addend = this->Addend,
                .DylibOrdinal = this->DylibOrdinal,
                .SymbolName = this->SymbolName,
                .SegmentIndex = this->SegmentIndex,
                .SegOffset = this->SegOffset,
                .AddrInSeg = this->AddrInSeg,
                .NewSymbolName = this->NewSymbolName
            };

            return Result;
        };

        uint64_t AddrInSeg;
        bool NewSymbolName : 1;
        SpecificCapArray<BindOpcodeThreadedData> ThreadedDataTable;

        [[nodiscard]]
        constexpr static inline bool canIgnoreError(ErrorEnum Error) noexcept {
            switch (Error) {
                case ErrorEnum::None:
                case ErrorEnum::EmptySymbol:
                case ErrorEnum::InvalidSegmentIndex:
                case ErrorEnum::NotEnoughThreadedBinds:
                case ErrorEnum::TooManyThreadedBinds:
                case ErrorEnum::InvalidThreadOrdinal:
                case ErrorEnum::OutOfBoundsSegmentAddr:
                case ErrorEnum::UnrecognizedBindSubOpcode:
                case ErrorEnum::UnrecognizedBindWriteKind:
                case ErrorEnum::UnrecognizedSpecialDylibOrdinal:
                case ErrorEnum::NoDylibOrdinal:
                case ErrorEnum::NoSegmentIndex:
                case ErrorEnum::NoWriteKind:
                    return true;

                case ErrorEnum::InvalidLeb128:
                case ErrorEnum::InvalidString:
                case ErrorEnum::IllegalBindOpcode:
                case ErrorEnum::UnrecognizedBindOpcode:
                    return false;
            }

            assert(0 && "Unrecognized Bind-Opcode Parse Error");
        }
    };

    struct BindActionIteratorEnd {};

    template <BindInfoKind BindKind>
    struct BindActionIteratorBase {
    public:
        using ErrorEnum = BindOpcodeParseError;
    protected:
        const uint8_t *Map;
        const SegmentInfoCollection &SegmentCollection;

        BindOpcodeIteratorBase<BindKind> Iter;
        BindByte LastByte;

        int64_t SegAddAddress = 0;
        uint64_t AddAmt = 0;
        uint64_t Count = 0;

        bool Is64Bit : 1;
    public:
        constexpr explicit
        BindActionIteratorBase(const uint8_t *Map,
                               const SegmentInfoCollection &Collection,
                               const BindByte *Begin,
                               const BindByte *End,
                               bool Is64Bit) noexcept
        : Map(Map), SegmentCollection(Collection),
          Iter(Begin, End, std::make_unique<BindActionIterateInfo>()),
          Is64Bit(Is64Bit)
        {
            LastByte.setOpcode(BindByte::Opcode::SetDylibOrdinalImm);
            ++(*this);
        }

        [[nodiscard]]
        constexpr inline BindActionIterateInfo &getInfo() noexcept {
            auto &Info = Iter.getInfo();
            return reinterpret_cast<BindActionIterateInfo &>(Info);
        }

        [[nodiscard]]
        constexpr const BindActionIterateInfo &getInfo() const noexcept {
            const auto &Info = Iter.getInfo();
            return reinterpret_cast<const BindActionIterateInfo &>(Info);
        }

        [[nodiscard]]
        inline uint64_t getOffset(const uint8_t *Base) const noexcept {
            assert(Base <= Iter->getPtr());
            return (Iter->getPtr() - Base);
        }

        [[nodiscard]]
        inline uint64_t getOffset(const BindByte *Ptr) const noexcept {
            const auto Base = reinterpret_cast<const uint8_t *>(Ptr);

            assert(Base <= Iter->getPtr());
            return (Iter->getPtr() - Base);
        }

        [[nodiscard]]
        constexpr bool IsValidForSegmentCollection(
            const SegmentInfoCollection &Collection,
            bool Is64Bit) noexcept
        {
            const auto &Info = getInfo();
            const auto SegmentIndex = Info.Action.SegmentIndex;

            if (SegmentIndex == -1) {
                return false;
            }

            if (IndexOutOfBounds(SegmentIndex, Collection.size())) {
                return false;
            }

            const auto &Segment = Collection.at(SegmentIndex);
            auto ContainsPtr = false;

            if (Is64Bit) {
                using Type = PointerAddrTypeFromKind<PointerKind::s64Bit>;
                ContainsPtr =
                    Segment.File.template ContainsTypeAtRelativeLocation<Type>(
                        Info.Action.AddrInSeg);
            } else {
                using Type = PointerAddrTypeFromKind<PointerKind::s32Bit>;
                ContainsPtr =
                    Segment.File.template ContainsTypeAtRelativeLocation<Type>(
                        Info.Action.AddrInSeg);
            }

            return ContainsPtr;
        }

        [[nodiscard]] constexpr inline bool isAtEnd() const noexcept {
            return Iter.isAtEnd();
        }

        [[nodiscard]] constexpr
        inline const BindActionIterateInfo &operator*() const noexcept {
            return getInfo();
        }

        [[nodiscard]] constexpr
        inline const BindActionIterateInfo *operator->() const noexcept {
            return &getInfo();
        }

        constexpr inline BindActionIteratorBase &operator++() noexcept {
            auto &Info = getInfo();
            if (Info.hasError()) {
                Iter++;
            }

            Info.Error = Advance();
            return *this;
        }

        constexpr inline BindActionIteratorBase &operator++(int) noexcept {
            return ++(*this);
        }

        constexpr
        inline BindActionIteratorBase &operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] constexpr
        inline bool operator==(const BindActionIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr inline
        bool operator!=(const BindActionIteratorEnd &End) const noexcept {
            return !(*this == End);
        }

        constexpr inline BindOpcodeParseError Advance() noexcept {
            auto &Info = getInfo();
            const auto AddChangeToSegmentAddress = [&](int64_t Add) noexcept {
                if (DoesAddOverflow(SegAddAddress, Add, &SegAddAddress)) {
                    return false;
                }

                if (DoesAddOverflow(Info.AddrInSeg, SegAddAddress)) {
                    return false;
                }

                return true;
            };

            using ErrorEnum = BindOpcodeParseError;

            Info.Error = ErrorEnum::None;
            Info.NewSymbolName = false;

            const auto FinalizeChangesForSegmentAddress = [&]() noexcept {
                Info.AddrInSeg += SegAddAddress;
                SegAddAddress = 0;
            };

            const auto DoThreadedBind = [&]() noexcept {
                const auto &SegInfo = SegmentCollection.at(Info.SegmentIndex);
                const auto SegVmAddr =
                    SegInfo.getMemoryRange().getBegin() + Info.AddrInSeg;

                const auto PtrSize = PointerSize(Is64Bit);
                const auto Data =
                    SegmentCollection.GetDataForVirtualAddr(Map,
                                                            SegVmAddr,
                                                            PtrSize);

                if (Data == nullptr) {
                    return ErrorEnum::OutOfBoundsSegmentAddr;
                }

                auto Value = *Data;
                const auto IsBind = (Value & (1ull << 62));

                if (IsBind) {
                    const auto ThreadOrdinal = (Value & 0xFFFF);
                    const auto ThreadOrdinalTableCap =
                        Info.ThreadedDataTable.capacity();

                    if (IndexOutOfBounds(ThreadOrdinal,
                                         ThreadOrdinalTableCap))
                    {
                        return ErrorEnum::InvalidThreadOrdinal;
                    }
                }

                Info.AddrInSeg += PointerSize(Is64Bit);

                Value &= ~(1ull << 62);
                AddAmt = (Value & 0x3FF8000000000000) >> 51;

                if (AddAmt == 0) {
                    LastByte.setOpcode(BindByte::Opcode::SetDylibOrdinalImm);
                }

                return ErrorEnum::None;
            };

            switch (LastByte.getOpcode()) {
                case BindByte::Opcode::DoBindUlebTimesSkippingUleb:
                    if (Count != 0) {
                        Info.AddrInSeg += AddAmt;
                        Count--;

                        if (Count != 0) {
                            return ErrorEnum::None;
                        }

                        Info.AddrInSeg += AddAmt;
                        LastByte.setOpcode(
                            BindByte::Opcode::SetDylibOrdinalImm);

                        Iter++;
                        break;
                    }

                case BindByte::Opcode::DoBind:
                case BindByte::Opcode::DoBindAddAddrUleb:
                case BindByte::Opcode::DoBindAddAddrImmScaled:
                    // Clear the Last-Opcode.
                    LastByte.setOpcode(BindByte::Opcode::SetDylibOrdinalImm);
                    FinalizeChangesForSegmentAddress();
                    Iter++;

                    break;

                case BindByte::Opcode::Done:
                    return ErrorEnum::None;

                case BindByte::Opcode::SetDylibOrdinalImm:
                case BindByte::Opcode::SetDylibOrdinalUleb:
                case BindByte::Opcode::SetDylibSpecialImm:
                case BindByte::Opcode::SetSymbolTrailingFlagsImm:
                case BindByte::Opcode::SetKindImm:
                case BindByte::Opcode::SetAddendSleb:
                case BindByte::Opcode::SetSegmentAndOffsetUleb:
                case BindByte::Opcode::AddAddrUleb:
                    break;
                case BindByte::Opcode::Threaded: {
                    const auto Error = DoThreadedBind();
                    if (Error != ErrorEnum::None) {
                        return Error;
                    }

                    break;
                }
            }

            const auto CheckChangeToSegmentAddress = [&](int64_t Add) noexcept {
                auto NewSegAddAddress = int64_t();
                if (DoesAddOverflow(SegAddAddress, Add, &NewSegAddAddress)) {
                    return false;
                }

                if (DoesAddOverflow(Info.AddrInSeg, NewSegAddAddress)) {
                    return false;
                }

                return true;
            };

            const auto CheckIfCanBind = [&]() noexcept {
                if (Info.DylibOrdinal == -1) {
                    return ErrorEnum::NoDylibOrdinal;
                }

                if (Info.SegmentIndex == -1) {
                    return ErrorEnum::NoSegmentIndex;
                }

                if (Info.WriteKind == BindByte::WriteKind::None) {
                    return ErrorEnum::NoWriteKind;
                }

                return ErrorEnum::None;
            };

            for (; !Iter.isAtEnd(); Iter++) {
                if (Iter->hasError()) {
                    return Iter->getError();
                }

                const auto &Byte = Iter.getByte();
                const auto &IterInfo = Iter.getInfo();

                switch (Byte.getOpcode()) {
                    case BindByte::Opcode::Done:
                        if constexpr (BindKind != BindInfoKind::Lazy) {
                            return ErrorEnum::None;
                        }

                        continue;
                    case BindByte::Opcode::SetDylibOrdinalImm:
                    case BindByte::Opcode::SetDylibSpecialImm:
                    case BindByte::Opcode::SetDylibOrdinalUleb:
                        continue;
                    case BindByte::Opcode::SetSymbolTrailingFlagsImm:
                        Info.NewSymbolName = true;
                        continue;
                    case BindByte::Opcode::SetKindImm:
                    case BindByte::Opcode::SetAddendSleb:
                        continue;
                    case BindByte::Opcode::SetSegmentAndOffsetUleb:
                        Info.AddrInSeg = IterInfo.SegOffset;
                        continue;
                    case BindByte::Opcode::AddAddrUleb:
                        if (DoesAddOverflow(Info.AddrInSeg,
                                            IterInfo.AddAddr,
                                            &Info.AddrInSeg))
                        {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        continue;
                    case BindByte::Opcode::DoBind: {
                        const auto Error = CheckIfCanBind();
                        if (Error != ErrorEnum::None) {
                            return Error;
                        }

                        const auto PtrSize = PointerSize(Is64Bit);
                        if (!AddChangeToSegmentAddress(PtrSize)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (Info.ThreadedCount != 0) {
                            if (Info.ThreadedDataTable.isFull()) {
                                return ErrorEnum::TooManyThreadedBinds;
                            }

                            Info.ThreadedDataTable.PushBack({
                                .Kind = Info.WriteKind,
                                .SymbolName = Info.SymbolName,
                                .DylibOrdinal = Info.DylibOrdinal,
                                .Addend = Info.Addend,
                                .Flags = Info.Flags
                            });
                        }

                        LastByte = Byte;
                        return ErrorEnum::None;
                    }
                    case BindByte::Opcode::DoBindAddAddrUleb: {
                        if (!AddChangeToSegmentAddress(IterInfo.AddAddr)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto PtrSize = PointerSize(Is64Bit);
                        if (AddChangeToSegmentAddress(PtrSize)) {
                            LastByte = Byte;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::None;
                    }
                    case BindByte::Opcode::DoBindAddAddrImmScaled: {
                        const auto PtrSize = PointerSize(Is64Bit);
                        auto Add = uint64_t();

                        if (DoesMultiplyAndAddOverflow(PtrSize,
                                                       Info.Scale,
                                                       PtrSize,
                                                       &Add))
                        {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (AddChangeToSegmentAddress(Add)) {
                            LastByte = Byte;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::OutOfBoundsSegmentAddr;
                    }
                    case BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                        auto SingleStep = int64_t();
                        auto Total = int64_t();

                        const auto PtrSize = PointerSize(Is64Bit);
                        if (DoesAddOverflow(PtrSize, Info.Skip, &SingleStep)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto Count = Info.Count;
                        if (DoesMultiplyOverflow(SingleStep, Count, &Total)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (CheckChangeToSegmentAddress(Total)) {
                            LastByte = Byte;
                            AddAmt = SingleStep;

                            this->Count = Count - 1;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::OutOfBoundsSegmentAddr;
                    }
                    case BindByteOpcode::Threaded:
                        switch (Info.SubOpcode) {
                            using Enum = BindByte::SubOpcode;
                            case Enum::ThreadedSetBindOrdinalTableSizeUleb:
                                Info.ThreadedDataTable.Reallocate(
                                    Info.ThreadedCount);
                                break;
                            case Enum::ThreadedApply: {
                                if (!Info.ThreadedDataTable.isFull()) {
                                    return ErrorEnum::NotEnoughThreadedBinds;
                                }

                                const auto Error = DoThreadedBind();
                                if (Error != ErrorEnum::None) {
                                    return Error;
                                }

                                if (AddAmt != 0) {
                                    LastByte = Byte;
                                }

                                return ErrorEnum::None;
                            }
                        }

                        return ErrorEnum::UnrecognizedBindSubOpcode;
                }

                return ErrorEnum::UnrecognizedBindOpcode;
            }

            LastByte.setOpcode(BindByteOpcode::Done);
            return ErrorEnum::None;
        }
    };

    using BindActionIterator = BindActionIteratorBase<BindInfoKind::Normal>;
    using ConstBindActionIterator =
        BindActionIteratorBase<BindInfoKind::Normal>;

    template <BindInfoKind BindKind>
    struct BindActionListBase {
        using IteratorType = BindActionIteratorBase<BindKind>;
    protected:
        const uint8_t *Map;
        const SegmentInfoCollection &SegmentCollection;

        const BindByte *Begin;
        const BindByte *End;

        bool Is64Bit : 1;
    public:
        constexpr explicit
        BindActionListBase(const uint8_t *Map,
                           const SegmentInfoCollection &Collection,
                           const uint8_t *Begin,
                           const uint8_t *End,
                           bool Is64Bit) noexcept
        : Map(Map),
          SegmentCollection(Collection),
          Begin(reinterpret_cast<const BindByte *>(Begin)),
          End(reinterpret_cast<const BindByte *>(End)),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] inline const BindByte *getBegin() const noexcept {
            return Begin;
        }

        [[nodiscard]] inline const BindByte *getEnd() const noexcept {
            return End;
        }

        [[nodiscard]] constexpr inline IteratorType begin() const noexcept {
            return IteratorType(Map, SegmentCollection, Begin, End, Is64Bit);
        }

        [[nodiscard]]
        constexpr inline BindActionIteratorEnd end() const noexcept {
            return BindActionIteratorEnd();
        }

        inline BindOpcodeParseError
        GetAsList(std::vector<BindActionInfo> &ListOut) const noexcept {
            for (const auto &Iter : *this) {
                const auto Error = Iter.getError();
                if (!Iter.canIgnoreError(Error)) {
                    return Error;
                }

                ListOut.emplace_back(Iter.getAction());
            }

            return BindOpcodeParseError::None;
        }

        inline BindOpcodeParseError
        GetListOfSymbols(
            std::vector<BindActionInfo> &SymbolListOut) const noexcept
        {
            for (const auto &Iter : *this) {
                const auto Error = Iter.getError();
                if (!Iter.canIgnoreError(Error)) {
                    return Error;
                }

                if (Iter.NewSymbolName) {
                    SymbolListOut.emplace_back(std::move(Iter.getAction()));
                }
            }

            return BindOpcodeParseError::None;
        }
    };

    using BindActionList = BindActionListBase<BindInfoKind::Normal>;
    using LazyBindActionList = BindActionListBase<BindInfoKind::Lazy>;
    using WeakBindActionList = BindActionListBase<BindInfoKind::Weak>;

    TypedAllocationOrError<BindNakedOpcodeList, SizeRangeError>
    GetBindNakedOpcodeList(const MemoryMap &Map,
                           uint32_t BindOffset,
                           uint32_t BindSize) noexcept;

    TypedAllocationOrError<BindOpcodeList, SizeRangeError>
    GetBindOpcodeList(const ConstMemoryMap &Map,
                      uint32_t BindOffset,
                      uint32_t BindSize,
                      bool Is64Bit) noexcept;

    TypedAllocationOrError<LazyBindOpcodeList, SizeRangeError>
    GetLazyBindOpcodeList(const ConstMemoryMap &Map,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept;

    TypedAllocationOrError<WeakBindOpcodeList, SizeRangeError>
    GetWeakBindOpcodeList(const ConstMemoryMap &Map,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept;

    TypedAllocationOrError<BindActionList, SizeRangeError>
    GetBindActionList(const ConstMemoryMap &Map,
                      const SegmentInfoCollection &Collection,
                      uint32_t BindOffset,
                      uint32_t BindSize,
                      bool Is64Bit) noexcept;

    TypedAllocationOrError<LazyBindActionList, SizeRangeError>
    GetLazyBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &Collection,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept;

    TypedAllocationOrError<WeakBindActionList, SizeRangeError>
    GetWeakBindActionList(const ConstMemoryMap &Map,
                          const SegmentInfoCollection &Collection,
                          uint32_t BindOffset,
                          uint32_t BindSize,
                          bool Is64Bit) noexcept;
}
