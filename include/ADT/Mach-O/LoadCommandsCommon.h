//
//  include/ADT/Mach-O/DyldInfoCommon.h
//  stool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <optional>

#include "ADT/BasicMasksHandler.h"
#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"

#include "Utils/Leb128.h"

namespace MachO {
    constexpr static const auto KindRequiredByDyld = 0x80000000ul;
    enum class LoadCommandKind : uint32_t {
        Segment = 1,
        SymbolTable,
        SymbolSegment,
        Thread,
        UnixThread,
        LoadFixedVMSharedLibrary,
        IdFixedVMSharedLibrary,
        Ident,
        FixedVMFile,
        PrePage,
        DynamicSymbolTable,
        LoadDylib,
        IdDylib,
        LoadDylinker,
        IdDylinker,
        PreBoundDylib,
        Routines,
        SubFramework,
        SubUmbrella,
        SubClient,
        SubLibrary,
        TwoLevelHints,
        PrebindChecksum,
        LoadWeakDylib = (KindRequiredByDyld | 0x18ul),
        Segment64 = 0x19,
        Routines64,
        Uuid,
        Rpath = (KindRequiredByDyld | 0x1cul),
        CodeSignature = 0x1d,
        SegmentSplitInfo,
        ReexportDylib = (KindRequiredByDyld | 0x1ful),
        LazyLoadDylib = 0x20,
        EncryptionInfo,
        DyldInfo,
        DyldInfoOnly = (KindRequiredByDyld | 0x22ul),
        LoadUpwardDylib = (KindRequiredByDyld | 0x23ul),
        VersionMinimumMacOSX = 0x24,
        VersionMinimumIPhoneOS,
        FunctionStarts,
        DyldEnvironment,
        Main = (KindRequiredByDyld | 0x28ul),
        DataInCode = 0x29,
        SourceVersion,
        DylibCodeSignDRS,
        EncryptionInfo64,
        LinkerOption,
        LinkerOptimizationHint,
        VersionMinimumTvOS,
        VersionMinimumWatchOS,
        Note,
        BuildVersion,
        DyldExportsTrie = (KindRequiredByDyld | 0x33ul),
        DyldChainedFixups = (KindRequiredByDyld | 0x34ul)
    };

    extern const std::string_view EmptyStringValue;
    enum class SizeRangeError {
        None,
        Empty,
        Overflows,
        PastEnd,
    };

    constexpr static inline SizeRangeError
    CheckSizeRange(const ConstMemoryMap &Map,
                   uint32_t Offset,
                   uint32_t Size,
                   uint64_t *EndOut) noexcept
    {
        auto End = static_cast<uint64_t>(Offset) + Size;
        auto Range = LocationRange::CreateWithEnd(Offset, End);

        if (!Map.getRange().containsRange(Range)) {
            return SizeRangeError::PastEnd;
        }

        if (Size == 0) {
            return SizeRangeError::Empty;
        }

        if (EndOut != nullptr) {
            *EndOut = End;
        }

        return SizeRangeError::None;
    }
    
    enum class DyldInfoByteMasks : uint8_t {
        Opcode    = 0xF0,
        Immediate = 0x0F
    };

    enum class DyldInfoByteShifts : uint8_t {
        Opcode    = 4,
        Immediate = 0
    };

    using DyldInfoByteMasksHandler =
        BasicMasksHandler<DyldInfoByteMasks, uint8_t>;

    using DyldInfoByteMasksAndShiftsHandler =
        BasicMasksAndShiftsHandler<DyldInfoByteMasks,
                                    DyldInfoByteShifts,
                                    uint8_t>;

    struct NakedOpcodeList {
    protected:
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        explicit
        NakedOpcodeList(const uint8_t *Begin, const uint8_t *End) noexcept
        : Begin(Begin), End(End) {}

        explicit
        NakedOpcodeList(const uint8_t *Begin, uint32_t Size) noexcept
        : Begin(Begin), End(this->Begin + Size) {}

        struct EndIterator {};

        struct Iterator;
        struct IteratorInfo {
            friend Iterator;
        protected:
            const uint8_t *Iter;
            const uint8_t *End;
        public:
            explicit
            IteratorInfo(const uint8_t *Iter, const uint8_t *End) noexcept;

            [[nodiscard]] inline const uint8_t *getPtr() const noexcept {
                return Iter;
            }

            template <typename T>
            [[nodiscard]] inline const T *getAsByte() const noexcept {
                return reinterpret_cast<const T *>(Iter);
            }

            template <typename T = uint64_t>
            [[nodiscard]] std::optional<T> ReadUleb128() noexcept {
                auto Value = T();
                auto *Ptr = ::ReadUleb128(Iter, End, &Value);

                if (Ptr == nullptr) {
                    return std::nullopt;
                }

                Iter = Ptr;
                return Value;
            }

            template <typename T = int64_t>
            [[nodiscard]] std::optional<T> ReadSleb128() noexcept {
                auto Value = T();
                auto *Ptr = ::ReadSleb128(Iter, End, &Value);

                if (Ptr == nullptr) {
                    return std::nullopt;
                }

                Iter = Ptr;
                return Value;
            }

            [[nodiscard]]
            std::optional<std::string_view> ParseString() noexcept;

            [[nodiscard]] inline bool IsAtEnd() const noexcept {
                return (Iter >= End);
            }
        };

        struct Iterator {
        protected:
            IteratorInfo Info;
        public:
            explicit Iterator(const uint8_t *Iter, const uint8_t *End) noexcept;

            [[nodiscard]] inline const uint8_t *getPtr() const noexcept {
                return Info.getPtr();
            }

            [[nodiscard]] inline IteratorInfo &operator*() noexcept {
                return Info;
            }

            [[nodiscard]] inline IteratorInfo *operator->() noexcept {
                return &Info;
            }

            [[nodiscard]]
            inline const IteratorInfo &operator*() const noexcept {
                return Info;
            }

            [[nodiscard]]
            inline const IteratorInfo *operator->() const noexcept {
                return &Info;
            }

            [[nodiscard]] inline bool IsAtEnd() const noexcept {
                return Info.IsAtEnd();
            }

            [[nodiscard]]
            inline bool operator==(const EndIterator &) const noexcept {
                return IsAtEnd();
            }

            [[nodiscard]]
            inline bool operator!=(const EndIterator &) const noexcept {
                return !IsAtEnd();
            }

            inline Iterator &operator++() noexcept {
                Info.Iter++;
                return *this;
            }

            inline Iterator &operator++(int) noexcept {
                return ++(*this);
            }

            inline Iterator &operator+=(uint64_t Amt) noexcept {
                for (auto I = uint64_t(); I != Amt; I++) {
                    (*this)++;
                }

                return *this;
            }
        };

        [[nodiscard]] Iterator begin() const noexcept;
        [[nodiscard]] EndIterator end() const noexcept;
    };

    enum class MemoryProtectionsEnum : uint32_t {
        None,
        Read    = (1ull << 0),
        Write   = (1ull << 1),
        Execute = (1ull << 2)
    };

    template <MemoryProtectionsEnum>
    struct MemoryProtectionsEnumInfo {};

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::None> {
        constexpr static const auto Kind = MemoryProtectionsEnum::None;

        constexpr static const auto Name = std::string_view("VM_PROT_NONE");
        constexpr static const auto Decription = std::string_view("None");
    };

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Read> {
        constexpr static const auto Kind = MemoryProtectionsEnum::Read;

        constexpr static const auto Name = std::string_view("VM_PROT_READ");
        constexpr static const auto Decription = std::string_view("Write");
    };

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Write> {
        constexpr static const auto Kind = MemoryProtectionsEnum::Write;

        constexpr static const auto Name = std::string_view("VM_PROT_WRITE");
        constexpr static const auto Decription = std::string_view("Write");
    };

    template <>
    struct MemoryProtectionsEnumInfo<MemoryProtectionsEnum::Execute> {
        constexpr static const auto Kind = MemoryProtectionsEnum::Execute;

        constexpr static const auto Name = std::string_view("VM_PROT_EXECUTE");
        constexpr static const auto Decription = std::string_view("Execute");
    };

    struct MemoryProtections :
        public ::BasicFlags<MemoryProtectionsEnum> {
    public:
        using Masks = MemoryProtectionsEnum;
        using MaskIntegerType = std::underlying_type_t<MemoryProtectionsEnum>;

        constexpr MemoryProtections() noexcept = default;
        constexpr MemoryProtections(MaskIntegerType Integer) noexcept
        : ::BasicFlags<MemoryProtectionsEnum>(Integer) {}

        [[nodiscard]] constexpr inline bool IsReadable() const noexcept {
            return hasValueForMask(Masks::Read);
        }

        [[nodiscard]] constexpr inline bool IsWritable() const noexcept {
            return hasValueForMask(Masks::Write);
        }

        [[nodiscard]] constexpr inline bool IsExecutable() const noexcept {
            return hasValueForMask(Masks::Execute);
        }

        constexpr inline MemoryProtections &setReadable() noexcept {
            setValueForMask(Masks::Read, true);
            return *this;
        }

        constexpr inline MemoryProtections &setWritable() noexcept {
            setValueForMask(Masks::Write, true);
            return *this;
        }

        constexpr inline MemoryProtections &setExecutable() noexcept {
            setValueForMask(Masks::Execute, true);
            return *this;
        }
    };

    enum class SegmentFlagsEnum : uint32_t {
        HighVM             = 1 << 0,
        FixedVMLibary      = 1 << 1,
        NoRelocations      = 1 << 2,
        ProtectionVersion1 = 1 << 3,
        ReadOnlyAfterFixup = 1 << 4
    };

    template <SegmentFlagsEnum Type>
    struct SegmentFlagsEnumInfo {};

    template <>
    struct SegmentFlagsEnumInfo<SegmentFlagsEnum::HighVM> {
        constexpr static const auto Kind = SegmentFlagsEnum::HighVM;

        constexpr static const auto Name = std::string_view("SG_HIGHVM");
        constexpr static const auto Description = std::string_view("High Vm");
    };

    template <>
    struct SegmentFlagsEnumInfo<SegmentFlagsEnum::FixedVMLibary> {
        constexpr static const auto Kind = SegmentFlagsEnum::FixedVMLibary;

        constexpr static const auto Name = std::string_view("SG_FVMLIB");
        constexpr static const auto Description =
            std::string_view("Fixed Vm Library");
    };

    template <>
    struct SegmentFlagsEnumInfo<SegmentFlagsEnum::NoRelocations> {
        constexpr static const auto Kind = SegmentFlagsEnum::NoRelocations;

        constexpr static const auto Name = std::string_view("SG_NORELOC");
        constexpr static const auto Description =
            std::string_view("No Relocations");
    };

    template <>
    struct SegmentFlagsEnumInfo<SegmentFlagsEnum::ProtectionVersion1> {
        constexpr static const auto Kind = SegmentFlagsEnum::NoRelocations;

        constexpr static const auto Name =
            std::string_view("SG_PROTECTED_VERSION_1");
        constexpr static const auto Description =
            std::string_view("Protected (Version 1)");
    };

    template <>
    struct SegmentFlagsEnumInfo<SegmentFlagsEnum::ReadOnlyAfterFixup> {
        constexpr static const auto Kind = SegmentFlagsEnum::ReadOnlyAfterFixup;

        constexpr static const auto Name = std::string_view("SG_READ_ONLY");
        constexpr static const auto Description =
            std::string_view("Read-Only (After Fix-Ups)");
    };

    struct SegmentFlags : public ::BasicFlags<SegmentFlagsEnum> {
    private:
        using Base = ::BasicFlags<SegmentFlagsEnum>;
    public:
        using FlagType = SegmentFlagsEnum;
        using FlagIntegerType = std::underlying_type_t<FlagType>;

        constexpr SegmentFlags() noexcept = default;
        constexpr SegmentFlags(FlagIntegerType Integer) noexcept
        : Base(Integer) {}

        [[nodiscard]] constexpr inline bool IsHighVM() const noexcept {
            return hasValueForMask(FlagType::HighVM);
        }

        [[nodiscard]] constexpr inline bool IsFixedVMLibary() const noexcept {
            return hasValueForMask(FlagType::FixedVMLibary);
        }

        [[nodiscard]] constexpr inline bool IsNoRelocations() const noexcept {
            return hasValueForMask(FlagType::NoRelocations);
        }

        [[nodiscard]] constexpr inline bool IsProtected() const noexcept {
            return hasValueForMask(FlagType::ProtectionVersion1);
        }

        [[nodiscard]]
        constexpr inline bool IsReadOnlyAfterFixup() const noexcept {
            return hasValueForMask(FlagType::ReadOnlyAfterFixup);
        }

        constexpr inline SegmentFlags &setHighVM(bool Value) noexcept {
            setValueForMask(FlagType::HighVM, Value);
            return *this;
        }

        constexpr inline SegmentFlags &setFixedVMLibary(bool Value) noexcept {
            setValueForMask(FlagType::FixedVMLibary, Value);
            return *this;
        }

        constexpr inline SegmentFlags &setNoRelocations(bool Value) noexcept {
            setValueForMask(FlagType::NoRelocations, Value);
            return *this;
        }

        constexpr
        inline SegmentFlags &setProtectionVersion1(bool Value) noexcept {
            setValueForMask(FlagType::ProtectionVersion1, Value);
            return *this;
        }

        constexpr
        inline SegmentFlags &setReadOnlyAfterFixup(bool Value) noexcept {
            setValueForMask(FlagType::ReadOnlyAfterFixup, Value);
            return *this;
        }

    };

    enum class SegmentSectionKind {
        Regular,
        ZeroFillOnDemand,
        CStringLiterals,
        FourByteLiterals,
        EightByteLiterals,
        LiteralPointers,
        NonLazySymbolPointers,
        LazySymbolPointers,
        SymbolStubs,
        ModInitFunctionPointers,
        ModTermFunctionPointers,
        CoalescedSymbols,
        ZeroFillOnDemandGigaBytes,
        InterposingFunctions,
        SixteenByteLiterals,
        DTraceDOF,
        LazyDylibSymbolPointers,

        ThreadLocalRegular,
        ThreadLocalZeroFill,
        TheradLocalVariables,
        ThreadLocalVariablePointers,
        ThreadLocalInitFunctionPointers,
        InitFunction32BitOffsets
    };

    template <SegmentSectionKind>
    struct SegmentSectionKindInfo {};

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::Regular> {
        constexpr static const auto Kind = SegmentSectionKind::Regular;

        constexpr static const auto Name = std::string_view("S_REGULAR");
        constexpr static const auto Description = std::string_view("Regular");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::ZeroFillOnDemand> {
        constexpr static const auto Kind = SegmentSectionKind::ZeroFillOnDemand;

        constexpr static const auto Name = std::string_view("S_ZEROFILL");
        constexpr static const auto Description = std::string_view("Zerofill");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::CStringLiterals> {
        constexpr static const auto Kind = SegmentSectionKind::CStringLiterals;

        constexpr static const auto Name =
            std::string_view("S_CSTRING_LITERALS");
        constexpr static const auto Description =
            std::string_view("C-String Literals");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::FourByteLiterals> {
        constexpr static const auto Kind = SegmentSectionKind::FourByteLiterals;

        constexpr static const auto Name = std::string_view("S_4BYTE_LITERALS");
        constexpr static const auto Description =
            std::string_view("Four-Byte Literal");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::EightByteLiterals> {
        constexpr static const auto Kind =
            SegmentSectionKind::EightByteLiterals;

        constexpr static const auto Name = std::string_view("S_8BYTE_LITERALS");
        constexpr static const auto Description =
            std::string_view("Eight-Byte Literal");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::LiteralPointers> {
        constexpr static const auto Kind = SegmentSectionKind::CStringLiterals;

        constexpr static const auto Name =
            std::string_view("S_LITERAL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Literal Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::NonLazySymbolPointers> {
        constexpr static const auto Kind =
            SegmentSectionKind::NonLazySymbolPointers;

        constexpr static const auto Name =
            std::string_view("S_NON_LAZY_SYMBOL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Non-Lazy Symbol-Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::LazySymbolPointers> {
        constexpr static const auto Kind =
            SegmentSectionKind::LazySymbolPointers;

        constexpr static const auto Name =
            std::string_view("S_LAZY_SYMBOL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Lazy Symbol-Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::SymbolStubs> {
        constexpr static const auto Kind = SegmentSectionKind::CStringLiterals;

        constexpr static const auto Name = std::string_view("S_SYMBOL_STUBS");
        constexpr static const auto Description =
            std::string_view("Symbol Stubs");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::ModInitFunctionPointers> {
        constexpr static const auto Kind =
            SegmentSectionKind::ModInitFunctionPointers;

        constexpr static const auto Name =
            std::string_view("S_MOD_INIT_FUNC_POINTERS");
        constexpr static const auto Description =
            std::string_view("Mod-Init Function Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::ModTermFunctionPointers> {
        constexpr static const auto Kind =
            SegmentSectionKind::ModTermFunctionPointers;

        constexpr static const auto Name =
            std::string_view("S_MOD_TERM_FUNC_POINTERS");
        constexpr static const auto Description =
            std::string_view("Mod-Term Function Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::CoalescedSymbols> {
        constexpr static const auto Kind = SegmentSectionKind::CoalescedSymbols;

        constexpr static const auto Name = std::string_view("S_COALESCED");
        constexpr static const auto Description =
            std::string_view("Coalesced Symbols");
    };

    template <>
    struct SegmentSectionKindInfo<
        SegmentSectionKind::ZeroFillOnDemandGigaBytes>
    {
        constexpr static const auto Kind =
            SegmentSectionKind::ZeroFillOnDemandGigaBytes;

        constexpr static const auto Name = std::string_view("S_GB_ZEROFILL");
        constexpr static const auto Description =
            std::string_view("Zero-Fill (GigaBytes)");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::InterposingFunctions> {
        constexpr static const auto Kind =
            SegmentSectionKind::InterposingFunctions;

        constexpr static const auto Name = std::string_view("S_INTERPOSING");
        constexpr static const auto Description =
            std::string_view("Interposing Functions");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::SixteenByteLiterals> {
        constexpr static const auto Kind =
            SegmentSectionKind::SixteenByteLiterals;

        constexpr static const auto Name =
            std::string_view("S_16BYTE_LITERALS");
        constexpr static const auto Description =
            std::string_view("16-Byte Literals");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::DTraceDOF> {
        constexpr static const auto Kind = SegmentSectionKind::DTraceDOF;

        constexpr static const auto Name = std::string_view("S_DTRACE_DOF");
        constexpr static const auto Description =
            std::string_view("DTrace DOFs");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::LazyDylibSymbolPointers> {
        constexpr static const auto Kind = SegmentSectionKind::DTraceDOF;

        constexpr static const auto Name =
            std::string_view("S_LAZY_DYLIB_SYMBOL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Lazy Dylib Lazy-Symbol Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::ThreadLocalRegular> {
        constexpr static const auto Kind =
            SegmentSectionKind::ThreadLocalRegular;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_REGULAR");
        constexpr static const auto Description =
            std::string_view("Thread-Local Regular");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::ThreadLocalZeroFill> {
        constexpr static const auto Kind =
            SegmentSectionKind::ThreadLocalZeroFill;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_ZEROFILL");
        constexpr static const auto Description =
            std::string_view("Thread-Local Zero-Fill");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::TheradLocalVariables> {
        constexpr static const auto Kind =
            SegmentSectionKind::TheradLocalVariables;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_VARIABLES");
        constexpr static const auto Description =
            std::string_view("Thread-Local Variables");
    };

    template <>
    struct SegmentSectionKindInfo<
        SegmentSectionKind::ThreadLocalVariablePointers>
    {
        constexpr static const auto Kind =
            SegmentSectionKind::ThreadLocalVariablePointers;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_VARIABLE_POINTERS");
        constexpr static const auto Description =
            std::string_view("Thread-Local Variable-Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<
        SegmentSectionKind::ThreadLocalInitFunctionPointers>
    {
        constexpr static const auto Kind =
            SegmentSectionKind::ThreadLocalInitFunctionPointers;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_INIT_FUNCTION_POINTERS");
        constexpr static const auto Description =
            std::string_view("Thread-Local Init-Function Pointers");
    };

    template <>
    struct SegmentSectionKindInfo<SegmentSectionKind::InitFunction32BitOffsets>
    {
        constexpr static const auto Kind =
            SegmentSectionKind::InitFunction32BitOffsets;

        constexpr static const auto Name =
            std::string_view("S_INIT_FUNC_OFFSETS");
        constexpr static const auto Description =
            std::string_view("32-Bit Init Function Offsets");
    };

    constexpr std::string_view
    SegmentSectionKindGetName(SegmentSectionKind Type) noexcept {
        switch (Type) {
            case SegmentSectionKind::Regular:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::Regular>::Name;
            case SegmentSectionKind::ZeroFillOnDemand:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ZeroFillOnDemand>::Name;
            case SegmentSectionKind::CStringLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::CStringLiterals>::Name;
            case SegmentSectionKind::FourByteLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::FourByteLiterals>::Name;
            case SegmentSectionKind::EightByteLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::EightByteLiterals>::Name;
            case SegmentSectionKind::LiteralPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::LiteralPointers>::Name;
            case SegmentSectionKind::NonLazySymbolPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::NonLazySymbolPointers>::Name;
            case SegmentSectionKind::LazySymbolPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::LazySymbolPointers>::Name;
            case SegmentSectionKind::SymbolStubs:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::SymbolStubs>::Name;
            case SegmentSectionKind::ModInitFunctionPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ModInitFunctionPointers>::Name;
            case SegmentSectionKind::ModTermFunctionPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ModTermFunctionPointers>::Name;
            case SegmentSectionKind::CoalescedSymbols:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::CoalescedSymbols>::Name;
            case SegmentSectionKind::ZeroFillOnDemandGigaBytes:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ZeroFillOnDemandGigaBytes>::Name;
            case SegmentSectionKind::InterposingFunctions:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::InterposingFunctions>::Name;
            case SegmentSectionKind::SixteenByteLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::SixteenByteLiterals>::Name;
            case SegmentSectionKind::DTraceDOF:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::DTraceDOF>::Name;
            case SegmentSectionKind::LazyDylibSymbolPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::LazyDylibSymbolPointers>::Name;
            case SegmentSectionKind::ThreadLocalRegular:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalRegular>::Name;
            case SegmentSectionKind::ThreadLocalZeroFill:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalZeroFill>::Name;
            case SegmentSectionKind::TheradLocalVariables:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::TheradLocalVariables>::Name;
            case SegmentSectionKind::ThreadLocalVariablePointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalVariablePointers>::Name;
            case SegmentSectionKind::ThreadLocalInitFunctionPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalInitFunctionPointers>::Name;
            case SegmentSectionKind::InitFunction32BitOffsets:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::InitFunction32BitOffsets>::Name;
        }

        return std::string_view();
    }

    constexpr std::string_view
    SegmentSectionKindGetDescription(SegmentSectionKind Type) noexcept {
        switch (Type) {
            case SegmentSectionKind::Regular:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::Regular>::Description;
            case SegmentSectionKind::ZeroFillOnDemand:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ZeroFillOnDemand>::Description;
            case SegmentSectionKind::CStringLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::CStringLiterals>::Description;
            case SegmentSectionKind::FourByteLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::FourByteLiterals>::Description;
            case SegmentSectionKind::EightByteLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::EightByteLiterals>::Description;
            case SegmentSectionKind::LiteralPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::LiteralPointers>::Description;
            case SegmentSectionKind::NonLazySymbolPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::NonLazySymbolPointers>::Description;
            case SegmentSectionKind::LazySymbolPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::LazySymbolPointers>::Description;
            case SegmentSectionKind::SymbolStubs:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::SymbolStubs>::Description;
            case SegmentSectionKind::ModInitFunctionPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ModInitFunctionPointers>::Description;
            case SegmentSectionKind::ModTermFunctionPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ModTermFunctionPointers>::Description;
            case SegmentSectionKind::CoalescedSymbols:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::CoalescedSymbols>::Description;
            case SegmentSectionKind::ZeroFillOnDemandGigaBytes:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ZeroFillOnDemandGigaBytes>::Description;
            case SegmentSectionKind::InterposingFunctions:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::InterposingFunctions>::Description;
            case SegmentSectionKind::SixteenByteLiterals:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::SixteenByteLiterals>::Description;
            case SegmentSectionKind::DTraceDOF:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::DTraceDOF>::Description;
            case SegmentSectionKind::LazyDylibSymbolPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::LazyDylibSymbolPointers>::Description;
            case SegmentSectionKind::ThreadLocalRegular:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalRegular>::Description;
            case SegmentSectionKind::ThreadLocalZeroFill:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalZeroFill>::Description;
            case SegmentSectionKind::TheradLocalVariables:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::TheradLocalVariables>::Description;
            case SegmentSectionKind::ThreadLocalVariablePointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalVariablePointers>
                        ::Description;
            case SegmentSectionKind::ThreadLocalInitFunctionPointers:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::ThreadLocalInitFunctionPointers>
                        ::Description;
            case SegmentSectionKind::InitFunction32BitOffsets:
                return SegmentSectionKindInfo<
                    SegmentSectionKind::InitFunction32BitOffsets>::Description;
        }

        return std::string_view();
    }

    static const auto SegmentSectionUserSettableAttributesMask =
        static_cast<uint32_t>(0xff000000);

    static const auto SegmentSectionSystemSettableAttributesMask =
        static_cast<uint32_t>(0xff000000);

    enum class SegmentSectionAttributesMasks : uint32_t {
        IsAllInstructions = 0x80000000,
        NoRanlibTableOfContents = 0x40000000,
        StripStaticSymbols = 0x20000000,
        NoDeadStripping = 0x10000000,
        LiveSupport = 0x08000000,
        SelfModifyingCode = 0x04000000,

        IsDebugSection = 0x02000000,
        HasSomeInstructions = 0x00000400,
        HasExternalRelocEntries = 0x00000200,
        HasLocalRelocEntries = 0x00000100,
    };

    struct SegmentSectionAttributes :
        public ::BasicFlags<SegmentSectionAttributesMasks>
    {
    private:
        using Base = ::BasicFlags<SegmentSectionAttributesMasks>;
    public:
        using MaskType = SegmentSectionAttributesMasks;
        using MaskIntegerType = std::underlying_type_t<MaskType>;

        constexpr SegmentSectionAttributes() noexcept = default;
        constexpr SegmentSectionAttributes(MaskIntegerType Attributes)
        : Base(Attributes) {}

        [[nodiscard]]
        constexpr inline bool IsAllInstructions() const noexcept {
            return hasValueForMask(MaskType::IsAllInstructions);
        }

        [[nodiscard]]
        constexpr inline bool NoRanlibTableOfContents() const noexcept {
            const auto NoRanlibTableOfContents =
                hasValueForMask(MaskType::NoRanlibTableOfContents);

            return NoRanlibTableOfContents;
        }

        [[nodiscard]]
        constexpr inline bool ShouldStripStaticSymbols() const noexcept {
            return hasValueForMask(MaskType::StripStaticSymbols);
        }

        [[nodiscard]] constexpr inline bool NoDeadStripping() const noexcept {
            return hasValueForMask(MaskType::NoDeadStripping);
        }

        [[nodiscard]] constexpr inline bool HasLiveSupport() const noexcept {
            return hasValueForMask(MaskType::LiveSupport);
        }

        [[nodiscard]] constexpr inline bool SelfModifyingCode() const noexcept {
            return hasValueForMask(MaskType::SelfModifyingCode);
        }

        [[nodiscard]] constexpr inline bool IsDebugSection() const noexcept {
            return hasValueForMask(MaskType::IsDebugSection);
        }

        [[nodiscard]]
        constexpr inline bool HasSomeInstructions() const noexcept {
            return hasValueForMask(MaskType::HasSomeInstructions);
        }

        [[nodiscard]]
        constexpr inline bool HasExternalRelocEntries() const noexcept {
            return hasValueForMask(MaskType::HasExternalRelocEntries);
        }

        [[nodiscard]]
        constexpr inline bool HasLocalRelocEntries() const noexcept {
            return hasValueForMask(MaskType::HasLocalRelocEntries);
        }

        constexpr inline
        SegmentSectionAttributes &setIsAllInstructions(bool Value) noexcept {
            setValueForMask(MaskType::IsAllInstructions, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setNoRanlibTableOfContents(bool Value) noexcept {
            setValueForMask(MaskType::NoRanlibTableOfContents, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setStripStaticSymbols(bool Value) noexcept {
            setValueForMask(MaskType::StripStaticSymbols, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setNoDeadStripping(bool Value) noexcept {
            setValueForMask(MaskType::NoDeadStripping, Value);
            return *this;
        }

        constexpr
        inline SegmentSectionAttributes &setLiveSupport(bool Value) noexcept {
            setValueForMask(MaskType::LiveSupport, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setSelfModifyingCode(bool Value) noexcept {
            setValueForMask(MaskType::SelfModifyingCode, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setIsDebugSection(bool Value) noexcept {
            setValueForMask(MaskType::IsDebugSection, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setHasSomeInstructions(bool Value) noexcept {
            setValueForMask(MaskType::HasSomeInstructions, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setHasExternalRelocEntries(bool Value) noexcept {
            setValueForMask(MaskType::HasExternalRelocEntries, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setHasLocalRelocEntries(bool Value) noexcept {
            setValueForMask(MaskType::HasLocalRelocEntries, Value);
            return *this;
        }
    };

    enum class SegmentSectionMasks : uint32_t {
        Attributes = 0xffffff00,
        Kind = 0x000000ff,
    };

    struct SegmentSectionFlags :
        public  BasicMasksHandler<SegmentSectionMasks> {
    private:
        using Base = BasicMasksHandler<SegmentSectionMasks>;
    public:
        using MaskType = SegmentSectionMasks;
        using MaskIntegerType = std::underlying_type_t<MaskType>;

        using Kind = SegmentSectionKind;
        using Attributes = SegmentSectionAttributes;

        constexpr SegmentSectionFlags() noexcept = default;
        constexpr SegmentSectionFlags(MaskIntegerType Integer) noexcept
        : Base(Integer) {}

        [[nodiscard]]
        constexpr inline Attributes getAttributes() const noexcept {
            return Attributes(getValueForMask(MaskType::Attributes));
        }

        [[nodiscard]] constexpr inline Kind getKind() const noexcept {
            return Kind(getValueForMask(MaskType::Kind));
        }

        [[nodiscard]] constexpr inline
        SegmentSectionFlags &setAttributes(Attributes Attributes) noexcept {
            setValueForMask(MaskType::Attributes, Attributes.value());
            return *this;
        }

        [[nodiscard]]
        constexpr inline SegmentSectionFlags &setType(Kind Kind) noexcept {
            setValueForMask(MaskType::Kind, static_cast<MaskIntegerType>(Kind));
            return *this;
        }
    };
}
