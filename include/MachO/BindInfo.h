/*
 * MachO/BindInfo.h
 * © suhas pai
 */

#pragma once

#include <string_view>

#include "ADT/FlagsBase.h"
#include "MachO/DyldInfo.h"

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

    [[nodiscard]]
    constexpr auto BindByteOpcodeIsValid(const BindByteOpcode Opcode) noexcept {
        switch (Opcode) {
            case BindByteOpcode::Done:
            case BindByteOpcode::SetDylibOrdinalImm:
            case BindByteOpcode::SetDylibOrdinalUleb:
            case BindByteOpcode::SetDylibSpecialImm:
            case BindByteOpcode::SetSymbolTrailingFlagsImm:
            case BindByteOpcode::SetKindImm:
            case BindByteOpcode::SetAddendSleb:
            case BindByteOpcode::SetSegmentAndOffsetUleb:
            case BindByteOpcode::AddAddrUleb:
            case BindByteOpcode::DoBind:
            case BindByteOpcode::DoBindAddAddrUleb:
            case BindByteOpcode::DoBindAddAddrImmScaled:
            case BindByteOpcode::DoBindUlebTimesSkippingUleb:
            case BindByteOpcode::Threaded:
                return true;
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto BindByteOpcodeGetName(const BindByteOpcode Opcode) noexcept
        -> std::string_view
    {
        switch (Opcode) {
            case BindByteOpcode::Done:
                return "BIND_OPCODE_DONE";
            case BindByteOpcode::SetDylibOrdinalImm:
                return "BIND_OPCODE_SET_DYLIB_ORDINAL_IMM";
            case BindByteOpcode::SetDylibOrdinalUleb:
                return "BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB";
            case BindByteOpcode::SetDylibSpecialImm:
                return "BIND_OPCODE_SET_DYLIB_SPECIAL_IMM";
            case BindByteOpcode::SetSymbolTrailingFlagsImm:
                return "BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM";
            case BindByteOpcode::SetKindImm:
                return "BIND_OPCODE_SET_TYPE_IMM";
            case BindByteOpcode::SetAddendSleb:
                return "BIND_OPCODE_SET_ADDEND_SLEB";
            case BindByteOpcode::SetSegmentAndOffsetUleb:
                return "BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB";
            case BindByteOpcode::AddAddrUleb:
                return "BIND_OPCODE_ADD_ADDR_ULEB";
            case BindByteOpcode::DoBind:
                return "BIND_OPCODE_DO_BIND";
            case BindByteOpcode::DoBindAddAddrUleb:
                return "BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB";
            case BindByteOpcode::DoBindAddAddrImmScaled:
                return "BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED";
            case BindByteOpcode::DoBindUlebTimesSkippingUleb:
                return "BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB";
            case BindByteOpcode::Threaded:
                return "Threaded";
        }

        assert(false &&
               "BindByteOpcodeGetName() got unrecognized BindByteOpcode");
    }

    [[nodiscard]]
    constexpr auto BindByteOpcodeGetDesc(const BindByteOpcode Opcode) noexcept
        -> std::string_view
    {
        switch (Opcode) {
            case BindByteOpcode::Done:
                return "Done";
            case BindByteOpcode::SetDylibOrdinalImm:
                return "Set Dylib-Ordinal (Immediate)";
            case BindByteOpcode::SetDylibOrdinalUleb:
                return "Set Dylib-Ordinal (Uleb128)";
            case BindByteOpcode::SetDylibSpecialImm:
                return "Set Special Dylib-Ordinal (Immediate)";
            case BindByteOpcode::SetSymbolTrailingFlagsImm:
                return "Set Trailing Symbol-Flags (Immediate)";
            case BindByteOpcode::SetKindImm:
                return "Set Bind-Kind (Immediate)";
            case BindByteOpcode::SetAddendSleb:
                return "Set Addend (Sleb128)";
            case BindByteOpcode::SetSegmentAndOffsetUleb:
                return "Set Addend (Sleb128)";
            case BindByteOpcode::AddAddrUleb:
                return "Add Address (Uleb128)";
            case BindByteOpcode::DoBind:
                return "Do Bind";
            case BindByteOpcode::DoBindAddAddrUleb:
                return "Do Bind And Add Address (Uleb)";
            case BindByteOpcode::DoBindAddAddrImmScaled:
                return "Do Bind And Add Address Scaled (Immediate)";
            case BindByteOpcode::DoBindUlebTimesSkippingUleb:
                return "Do Bind Uleb128 Skipping Uleb128 Bytes";
            case BindByteOpcode::Threaded:
                return "Threaded";
        }

        assert(false &&
               "BindByteOpcodeGetDesc() got unrecognized BindByteOpcode");
    }

    enum class BindByteSubOpcode : uint8_t {
        ThreadedSetBindOrdinalTableSizeUleb,
        ThreadedApply
    };

    enum class BindWriteKind : uint8_t {
        Pointer = 1,
        TextAbsolute32,
        TextPcrel32
    };

    [[nodiscard]]
    constexpr auto BindWriteKindIsValid(const BindWriteKind Kind) noexcept {
        switch (Kind) {
            case BindWriteKind::Pointer:
            case BindWriteKind::TextAbsolute32:
            case BindWriteKind::TextPcrel32:
                return true;
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto BindWriteKindGetName(const BindWriteKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case BindWriteKind::Pointer:
                return "BIND_TYPE_POINTER";
            case BindWriteKind::TextAbsolute32:
                return "BIND_TYPE_TEXT_ABSOLUTE32";
            case BindWriteKind::TextPcrel32:
                return "BIND_TYPE_TEXT_PCREL32";
        }

        assert(false &&
               "BindWriteKindGetName() got unrecognized BindWriteKind");
    }

    [[nodiscard]] constexpr
    auto BindWriteKindGetDescription(const BindWriteKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case BindWriteKind::Pointer:
                return "Pointer";
            case BindWriteKind::TextAbsolute32:
                return "Text Absolute (32-Bit)";
            case BindWriteKind::TextPcrel32:
                return "PC Relative (32-Bit)";
        }

        assert(false &&
               "BindWriteKindGetDesc() got unrecognized BindWriteKind");
    }

    enum class BindByteDylibSpecialOrdinal : uint8_t {
        DylibSelf = 0,
        DylibMainExecutable = static_cast<uint8_t>(-1),
        DylibFlatLookup = static_cast<uint8_t>(-2),
        DylibWeakLookup = static_cast<uint8_t>(-3),
    };

    [[nodiscard]] constexpr auto
    BindByteDylibSpecialOrdinalIsValid(
        const BindByteDylibSpecialOrdinal Ordinal) noexcept
    {
        switch (Ordinal) {
            case BindByteDylibSpecialOrdinal::DylibSelf:
            case BindByteDylibSpecialOrdinal::DylibMainExecutable:
            case BindByteDylibSpecialOrdinal::DylibFlatLookup:
            case BindByteDylibSpecialOrdinal::DylibWeakLookup:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto
    BindByteDylibSpecialOrdinalGetName(
        const BindByteDylibSpecialOrdinal Ordinal) noexcept -> std::string_view
    {
        switch (Ordinal) {
            case BindByteDylibSpecialOrdinal::DylibSelf:
                return "BIND_SPECIAL_DYLIB_SELF";
            case BindByteDylibSpecialOrdinal::DylibMainExecutable:
                return "BIND_SPECIAL_DYLIB_MAIN_EXECUTABLE";
            case BindByteDylibSpecialOrdinal::DylibFlatLookup:
                return "BIND_SPECIAL_DYLIB_FLAT_LOOKUP";
            case BindByteDylibSpecialOrdinal::DylibWeakLookup:
                return "BIND_SPECIAL_DYLIB_WEAK_LOOKUP";
        }

        assert(false &&
               "BindByteDylibSpecialOrdinalGetName() got unrecognized "
               "BindByteDylibSpecialOrdinal");
    }

    [[nodiscard]] constexpr auto
    BindByteDylibSpecialOrdinalGetDesc(
        const BindByteDylibSpecialOrdinal Ordinal) noexcept -> std::string_view
    {
        switch (Ordinal) {
            case BindByteDylibSpecialOrdinal::DylibSelf:
                return "Dylib-Self";
            case BindByteDylibSpecialOrdinal::DylibMainExecutable:
                return "Main-Executable";
            case BindByteDylibSpecialOrdinal::DylibFlatLookup:
                return "Flat-Lookup";
            case BindByteDylibSpecialOrdinal::DylibWeakLookup:
                return "Weak-Lookup";
        }

        assert(false &&
               "BindByteDylibSpecialOrdinalGetDesc() got unrecognized "
               "BindByteDylibSpecialOrdinal");
    }

    struct BindSymbolFlags : public ADT::FlagsBase<uint8_t> {
    public:
        using ADT::FlagsBase<uint8_t>::FlagsBase;

        enum class FlagsEnum : uint8_t {
            WeakImport        = 1,
            NonWeakDefinition = 1ull << 4
        };

        [[nodiscard]] constexpr auto isWeakImport() const noexcept {
            return has(FlagsEnum::WeakImport);
        }

        [[nodiscard]]
        constexpr auto hasNonWeakDefinition() const noexcept {
            return has(FlagsEnum::NonWeakDefinition);
        }

        constexpr auto setIsWeakImport(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(FlagsEnum::WeakImport, 0, Value);
            return *this;
        }

        constexpr auto setHasNonWeakDefinition(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(FlagsEnum::NonWeakDefinition, 0, Value);
            return *this;
        }
    };

    struct BindByte : protected ADT::FlagsBase<uint8_t> {
    protected:
        uint8_t Byte = 0;
    public:
        BindByte() noexcept = default;
        BindByte(const uint8_t Byte) noexcept : Byte(Byte) {}

        using Masks = DyldInfoByteMasks;
        using Opcode = BindByteOpcode;
        using SubOpcode = BindByteSubOpcode;
        using WriteKind = BindWriteKind;

        [[nodiscard]] constexpr auto opcode() const noexcept {
            return Opcode(valueForMask(Masks::Opcode));
        }

        [[nodiscard]] constexpr auto immediate() const noexcept {
            return Opcode(valueForMask(Masks::Immediate));
        }

        constexpr auto setOpcode(const Opcode Value) noexcept -> decltype(*this)
        {
            setValueForMask(Masks::Opcode, 0, Value);
            return *this;
        }

        constexpr auto setImmediate(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            assert(Value <= 0xF);
            setValueForMask(Masks::Immediate, 0, Value);

            return *this;
        }
    };
}
