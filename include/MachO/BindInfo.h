/*
 * MachO/BindInfo.h
 * © suhas pai
 */

#pragma once

#include <memory>
#include <string_view>

#include "ADT/MemoryMap.h"

#include "MachO/DyldInfo.h"
#include "MachO/OpcodeList.h"
#include "MachO/SegmentList.h"

#include "Utils/Overflow.h"

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
        None,
        Pointer,
        TextAbsolute32,
        TextPcrel32
    };

    [[nodiscard]]
    constexpr auto BindWriteKindIsValid(const BindWriteKind Kind) noexcept {
        switch (Kind) {
            case BindWriteKind::None:
                return false;
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
            case BindWriteKind::None:
                assert(
                    false &&
                    "Got BindWriteKind None in MachO::BindWriteKindGetName()");
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

    [[nodiscard]]
    constexpr auto BindWriteKindGetDesc(const BindWriteKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case BindWriteKind::None:
                assert(
                    false &&
                    "Got BindWriteKind None in MachO::BindWriteKindGetName()");
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
    public:
        BindByte() noexcept = default;
        using ADT::FlagsBase<uint8_t>::FlagsBase;

        using Masks = DyldInfoByteMasks;
        using Opcode = BindByteOpcode;
        using SubOpcode = BindByteSubOpcode;
        using WriteKind = BindWriteKind;

        [[nodiscard]] constexpr auto opcode() const noexcept {
            return Opcode(valueForMask(Masks::Opcode));
        }

        [[nodiscard]] constexpr auto immediate() const noexcept {
            return uint8_t(valueForMask(Masks::Immediate));
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
            int64_t Skip = 0;
            uint64_t Scale;
            BindByteSubOpcode SubOpcode;
        };

        BindSymbolFlags Flags;
        BindWriteKind WriteKind;

        [[nodiscard]] constexpr bool hasError() const noexcept {
            return (Error != BindOpcodeParseError::None);
        }

        [[nodiscard]] constexpr auto error() const noexcept {
            return Error;
        }
    };

    struct BindOpcodeIteratorEnd {};

    template <BindInfoKind BindKind>
    struct BindOpcodeIteratorBase {
    public:
        using ErrorEnum = BindOpcodeParseError;
    protected:
        typename OpcodeList::Iterator Iter;
        std::unique_ptr<BindOpcodeIterateInfo> Info;

        const BindByte *Prev;
        bool ReachedEnd : 1 = false;
    public:
        explicit
        BindOpcodeIteratorBase(const BindByte *const Begin,
                               const BindByte *const End) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End))
        {
            Info = std::make_unique<BindOpcodeIterateInfo>();
            Info->Kind = BindKind;

            Advance();
        }

        explicit
        BindOpcodeIteratorBase(
            const BindByte *const Begin,
            const BindByte *const End,
            std::unique_ptr<BindOpcodeIterateInfo> &&Info) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End)),
          Info(std::move(Info)), ReachedEnd(false)
        {
            this->Info->Kind = BindKind;
            Advance();
        }

        BindOpcodeIteratorBase(const BindOpcodeIteratorBase &) = delete;

        [[nodiscard]]
        inline auto getOffset(const uint8_t *const Base) const noexcept {
            assert(Base <= Iter->ptr());
            return static_cast<uint64_t>(Iter->ptr() - Base);
        }

        [[nodiscard]]
        inline auto getOffset(const BindByte *const Ptr) const noexcept {
            const auto Base = reinterpret_cast<const uint8_t *>(Ptr);

            assert(Base <= Iter->ptr());
            return static_cast<uint64_t>(Iter->ptr() - Base);
        }

        [[nodiscard]] inline const uint8_t *getPtr() const noexcept {
            return Iter->ptr();
        }

        [[nodiscard]] constexpr auto isAtEnd() const noexcept {
            if constexpr (BindKind == BindInfoKind::Lazy) {
                return Iter.isAtEnd() && ReachedEnd;
            }

            return ReachedEnd;
        }
        [[nodiscard]]
        constexpr auto operator*() const noexcept -> decltype(*this) {
            return *this;
        }

        [[nodiscard]] constexpr auto operator->() const noexcept {
            return this;
        }

        [[nodiscard]] constexpr auto &info() noexcept {
            return *Info;
        }

        [[nodiscard]] constexpr auto &info() const noexcept {
            return *Info;
        }

        [[nodiscard]] inline auto &byte() const noexcept {
            return *reinterpret_cast<const BindByte *>(Prev);
        }

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return Info->hasError();
        }

        [[nodiscard]] constexpr auto error() const noexcept {
            return Info->error();
        }

        constexpr auto operator++() noexcept -> decltype(*this) {
            Info->Error = Advance();
            return *this;
        }

        constexpr auto operator++(int) noexcept -> decltype(*this) {
            return ++(*this);
        }

        constexpr auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] constexpr
        auto operator==(const BindOpcodeIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr
        auto operator!=(const BindOpcodeIteratorEnd &End) const noexcept {
            return !operator==(End);
        }

        constexpr BindOpcodeParseError Advance() noexcept {
            using ErrorEnum = BindOpcodeParseError;

            const auto BytePtr = Iter->asByte<BindByte>();
            const auto &Byte = *BytePtr;

            Info->Error = ErrorEnum::None;
            Prev = BytePtr;
            Iter++;

            if (Iter.isAtEnd()) {
                ReachedEnd = true;
                return ErrorEnum::None;
            }

            switch (Byte.opcode()) {
                case BindByte::Opcode::Done:
                    ReachedEnd = true;
                    return ErrorEnum::None;
                case BindByte::Opcode::SetDylibOrdinalImm:
                    if constexpr (BindKind == BindInfoKind::Weak) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    Info->DylibOrdinal = Byte.immediate();
                    return ErrorEnum::None;
                case BindByte::Opcode::SetDylibOrdinalUleb:
                    if constexpr (BindKind == BindInfoKind::Weak) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    if (const auto Uleb = Iter->ReadUleb128()) {
                        Info->DylibOrdinal = static_cast<int64_t>(Uleb.value());
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case BindByte::Opcode::SetDylibSpecialImm: {
                    if constexpr (BindKind == BindInfoKind::Weak) {
                        return ErrorEnum::IllegalBindOpcode;
                    }

                    auto DylibOrdinal = Byte.immediate();
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
                    Info->Flags = BindSymbolFlags(Byte.immediate());
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

                    Info->WriteKind = BindWriteKind(Byte.immediate());
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
                        Info->SegmentIndex = Byte.immediate();

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

                    Info->Scale = Byte.immediate();
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
                    Info->SubOpcode = BindByteSubOpcode(Byte.immediate());
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
        using Iterator = BindOpcodeIteratorBase<BindKind>;
    protected:
        const BindByte *Begin;
        const BindByte *End;
        bool Is64Bit : 1;
    public:
        constexpr explicit
        BindOpcodeListBase(const uint8_t *const Begin,
                           const uint8_t *const End,
                           const bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const BindByte *>(Begin)),
          End(reinterpret_cast<const BindByte *>(End)),
          Is64Bit(Is64Bit) {}

        explicit
        BindOpcodeListBase(const ADT::MemoryMap &Map,
                           const bool Is64Bit) noexcept
        : Begin(Map.base<const BindByte>()), End(Map.end<const BindByte>()),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] constexpr auto begin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return BindOpcodeIteratorEnd();
        }

        [[nodiscard]]
        constexpr auto getOffsetFromIter(const Iterator &Iter) const noexcept {
            return static_cast<uint64_t>(Iter->ptr() - Begin);
        }
    };

    using BindOpcodeList = BindOpcodeListBase<BindInfoKind::Normal>;
    using LazyBindOpcodeList = BindOpcodeListBase<BindInfoKind::Lazy>;
    using WeakBindOpcodeList = BindOpcodeListBase<BindInfoKind::Weak>;

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

        [[nodiscard]] constexpr auto hasDylibOrdinal() const noexcept {
            return DylibOrdinal != -1;
        }
    };

    struct BindOpcodeThreadedData {
        BindWriteKind Kind;
        std::string_view SymbolName;

        int64_t DylibOrdinal;
        int64_t Addend;

        BindSymbolFlags Flags;
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
                .NewSymbolName = this->NewSymbolName,
                .Flags = this->Flags
            };

            return Result;
        };

        uint64_t AddrInSeg;
        bool NewSymbolName : 1;

        std::vector<BindOpcodeThreadedData> ThreadedDataTable;
        uint64_t ThreadDataTableMaxSize = 0;

        [[nodiscard]]
        constexpr static auto canIgnoreError(const ErrorEnum Error) noexcept {
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
        const SegmentList &SegList;

        BindOpcodeIteratorBase<BindKind> Iter;
        BindByte LastByte;

        int64_t SegAddAddress = 0;
        uint64_t AddAmt = 0;
        uint64_t Count = 0;

        bool Is64Bit : 1;
    public:
        constexpr explicit
        BindActionIteratorBase(const uint8_t *const Map,
                               const SegmentList &SegList,
                               const BindByte *const Begin,
                               const BindByte *const End,
                               bool Is64Bit) noexcept
        : Map(Map), SegList(SegList),
          Iter(Begin, End, std::make_unique<BindActionIterateInfo>()),
          Is64Bit(Is64Bit)
        {
            LastByte.setOpcode(BindByte::Opcode::SetDylibOrdinalImm);
            operator++();
        }

        [[nodiscard]] inline auto &info() noexcept {
            auto &Info = Iter.info();
            return reinterpret_cast<BindActionIterateInfo &>(Info);
        }

        [[nodiscard]] inline auto &info() const noexcept {
            const auto &Info = Iter.info();
            return reinterpret_cast<const BindActionIterateInfo &>(Info);
        }

        [[nodiscard]]
        inline auto getOffset(const uint8_t *const Base) const noexcept {
            assert(Base <= Iter->ptr());
            return static_cast<uint64_t>(Iter->getPtr() - Base);
        }

        [[nodiscard]]
        inline uint64_t getOffset(const BindByte *const Ptr) const noexcept {
            const auto Base = reinterpret_cast<const uint8_t *>(Ptr);

            assert(Base <= Iter->getPtr());
            return (Iter->getPtr() - Base);
        }

        [[nodiscard]] constexpr auto
        IsValidForSegmentList(const SegmentList &SegList,
                              const bool Is64Bit) noexcept
        {
            const auto &Info = info();
            const auto SegmentIndex = Info.Action.SegmentIndex;

            if (SegmentIndex == -1) {
                return false;
            }

            if (SegmentIndex >= SegList.size()) {
                return false;
            }

            const auto &Segment = SegList.at(SegmentIndex);
            auto ContainsPtr = false;

            if (Is64Bit) {
                ContainsPtr =
                    Segment.VmRange.template containsLoc<uint64_t>(
                        Info.AddrInSeg);
            } else {
                ContainsPtr =
                    Segment.VmRange.template containsLoc<uint32_t>(
                        Info.AddrInSeg);
            }

            return ContainsPtr;
        }

        [[nodiscard]] constexpr bool isAtEnd() const noexcept {
            return Iter.isAtEnd();
        }

        [[nodiscard]] inline auto &operator*() const noexcept {
            return info();
        }

        [[nodiscard]] inline auto *operator->() const noexcept {
            return &info();
        }

        constexpr auto operator++() noexcept -> decltype(*this) {
            auto &Info = info();
            if (Info.hasError()) {
                Iter++;
            }

            Info.Error = Advance();
            return *this;
        }

        constexpr auto operator++(int) noexcept {
            return operator++();
        }

        constexpr auto operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                operator++();
            }

            return *this;
        }

        [[nodiscard]] constexpr
        auto operator==(const BindActionIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr
        auto operator!=(const BindActionIteratorEnd &End) const noexcept {
            return !operator==(End);
        }

        constexpr BindOpcodeParseError Advance() noexcept {
            auto &Info = info();
            const auto AddChangeToSegmentAddress =
                [&](const int64_t Add) noexcept
            {
                const auto SegAddAddressOpt =
                    Utils::AddAndCheckOverflow<int64_t>(SegAddAddress, Add);

                if (!SegAddAddressOpt.has_value()) {
                    return false;
                }

                SegAddAddress = SegAddAddressOpt.value();
                if (Utils::WillAddOverflow(SegAddAddress, Info.AddrInSeg)) {
                    return false;
                }

                return true;
            };

            using ErrorEnum = BindOpcodeParseError;

            Info.Error = ErrorEnum::None;
            Info.NewSymbolName = false;

            const auto FinalizeChangesForSegmentAddress = [&]() noexcept {
                if (SegAddAddress > 0) {
                    Info.AddrInSeg += static_cast<uint64_t>(SegAddAddress);
                } else {
                    Info.AddrInSeg -= static_cast<uint64_t>(-SegAddAddress);
                }

                SegAddAddress = 0;
            };

            const auto DoThreadedBind = [&]() noexcept {
                const auto &SegInfo =
                    SegList.at(static_cast<uint64_t>(Info.SegmentIndex));

                const auto SegVmAddr = SegInfo.VmRange.begin() + Info.AddrInSeg;

                const auto PtrSize = Utils::PointerSize(Is64Bit);
                const auto FileOffsetOpt =
                    SegList.getFileOffsetForVmAddr(SegVmAddr, PtrSize);

                if (!FileOffsetOpt.has_value()) {
                    return ErrorEnum::OutOfBoundsSegmentAddr;
                }

                auto Value = uint64_t();
                if (Is64Bit) {
                    Value =
                        *reinterpret_cast<const uint64_t *>(
                            Map + FileOffsetOpt.value());
                } else {
                    Value =
                        *reinterpret_cast<const uint32_t *>(
                            Map + FileOffsetOpt.value());
                }

                const auto IsBind = Value & (1ull << 62);
                if (IsBind) {
                    const auto ThreadOrdinal = (Value & 0xFFFF);
                    if (ThreadOrdinal >= Info.ThreadDataTableMaxSize) {
                        return ErrorEnum::InvalidThreadOrdinal;
                    }
                }

                Info.AddrInSeg += PtrSize;

                Value &= ~(1ull << 62);
                AddAmt = (Value & 0x3FF8000000000000) >> 51;

                if (AddAmt == 0) {
                    LastByte.setOpcode(BindByte::Opcode::SetDylibOrdinalImm);
                }

                return ErrorEnum::None;
            };

            switch (LastByte.opcode()) {
                case BindByte::Opcode::DoBindUlebTimesSkippingUleb:
                    if (Count == 0) {
                        break;
                    }

                    Info.AddrInSeg += AddAmt;
                    Count--;

                    if (Count != 0) {
                        return ErrorEnum::None;
                    }

                    Info.AddrInSeg += AddAmt;
                    LastByte.setOpcode(BindByte::Opcode::SetDylibOrdinalImm);
                    Iter++;

                    break;
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

            const auto CheckChangeToSegmentAddress =
                [&](const int64_t Add) noexcept
            {
                const auto NewSegAddAddressOpt =
                    Utils::AddAndCheckOverflow<int64_t>(SegAddAddress,
                                                        Add,
                                                        Info.AddrInSeg);

                if (!NewSegAddAddressOpt.has_value()) {
                    return false;
                }

                if (Utils::WillAddOverflow(NewSegAddAddressOpt.value(),
                                           Info.AddrInSeg))
                {
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
                    return Iter->error();
                }

                const auto &Byte = Iter.byte();
                const auto &IterInfo = Iter.info();

                switch (Byte.opcode()) {
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
                    case BindByte::Opcode::AddAddrUleb: {
                        const auto AddrInSegOpt =
                            Utils::AddAndCheckOverflow(Info.AddrInSeg,
                                                       IterInfo.AddAddr);

                        if (!AddrInSegOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        Info.AddrInSeg = AddrInSegOpt.value();
                        continue;
                    }
                    case BindByte::Opcode::DoBind: {
                        const auto Error = CheckIfCanBind();
                        if (Error != ErrorEnum::None) {
                            return Error;
                        }

                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        if (!AddChangeToSegmentAddress(PtrSize)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (Info.ThreadedCount != 0) {
                            if (Info.ThreadedDataTable.size() ==
                                Info.ThreadDataTableMaxSize)
                            {
                                return ErrorEnum::TooManyThreadedBinds;
                            }

                            Info.ThreadedDataTable.emplace_back(
                                BindOpcodeThreadedData {
                                    .Kind = Info.WriteKind,
                                    .SymbolName = Info.SymbolName,
                                    .DylibOrdinal = Info.DylibOrdinal,
                                    .Addend = Info.Addend,
                                    .Flags = Info.Flags
                                }
                            );
                        }

                        LastByte = Byte;
                        return ErrorEnum::None;
                    }
                    case BindByte::Opcode::DoBindAddAddrUleb: {
                        if (!AddChangeToSegmentAddress(IterInfo.AddAddr)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        if (AddChangeToSegmentAddress(PtrSize)) {
                            LastByte = Byte;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::None;
                    }
                    case BindByte::Opcode::DoBindAddAddrImmScaled: {
                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        const auto AddOpt =
                            Utils::MulAddAndCheckOverflow<int64_t>(PtrSize,
                                                                   Info.Scale,
                                                                   PtrSize);

                        if (!AddOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (AddChangeToSegmentAddress(AddOpt.value())) {
                            LastByte = Byte;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::OutOfBoundsSegmentAddr;
                    }
                    case BindByte::Opcode::DoBindUlebTimesSkippingUleb: {
                        auto Total = int64_t();

                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        const auto SingleStepOpt =
                            Utils::AddAndCheckOverflow(PtrSize, Info.Skip);

                        if (!SingleStepOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto SingleStep = SingleStepOpt.value();
                        const auto Count = Info.Count;
                        const auto TotalOpt =
                            Utils::MulAndCheckOverflow<int64_t>(SingleStep,
                                                                Count);

                        if (!TotalOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        Total = TotalOpt.value();
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
                                Info.ThreadedDataTable.reserve(
                                    Info.ThreadedCount);
                                break;
                            case Enum::ThreadedApply: {
                                if (Info.ThreadedDataTable.size() ==
                                    Info.ThreadDataTableMaxSize)
                                {
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

    template <BindInfoKind BindKind>
    struct BindActionListBase {
        using Iterator = BindActionIteratorBase<BindKind>;
    protected:
        const uint8_t *Map;
        const SegmentList &SegList;

        const BindByte *Begin;
        const BindByte *End;

        bool Is64Bit : 1;
    public:
        constexpr explicit
        BindActionListBase(const uint8_t *const Map,
                           const SegmentList &SegList,
                           const uint8_t *const Begin,
                           const uint8_t *const End,
                           const bool Is64Bit) noexcept
        : Map(Map), SegList(SegList),
          Begin(reinterpret_cast<const BindByte *>(Begin)),
          End(reinterpret_cast<const BindByte *>(End)),
          Is64Bit(Is64Bit) {}

        constexpr explicit
        BindActionListBase(const ADT::MemoryMap &Map,
                           const ADT::Range &Range,
                           const SegmentList &SegList,
                           const bool Is64Bit) noexcept
        : Map(Map.base<uint8_t>()), SegList(SegList),
          Begin(Map.get<const BindByte>(Range.begin())),
          End(Map.get<const BindByte>(Range.end().value())),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] inline auto getBegin() const noexcept {
            return Begin;
        }

        [[nodiscard]] inline auto getEnd() const noexcept {
            return End;
        }

        [[nodiscard]] constexpr auto begin() const noexcept {
            return Iterator(Map, SegList, Begin, End, Is64Bit);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return BindActionIteratorEnd();
        }

        inline
        auto getAsList(std::vector<BindActionInfo> &ListOut) const noexcept {
            for (const auto &Iter : *this) {
                const auto Error = Iter.error();
                if (!Iter.canIgnoreError(Error)) {
                    return Error;
                }

                ListOut.emplace_back(Iter.getAction());
            }

            return BindOpcodeParseError::None;
        }

        inline auto
        getListOfSymbols(
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
}

