//
//  include/ADT/Mach-O/DyldInfoCommon.h
//  ktool
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

#include "MemoryProtections.h"

namespace MachO {
    constexpr static inline auto KindRequiredByDyld = 0x80000000ul;
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
        DyldChainedFixups = (KindRequiredByDyld | 0x34ul),
        FileSetEntry = (KindRequiredByDyld | 0x35ul)
    };

    enum class SizeRangeError {
        None,
        Empty,
        Overflows,
        PastEnd,
    };

    [[nodiscard]] inline SizeRangeError
    CheckSizeRange(const ConstMemoryMap &Map,
                   uint32_t Offset,
                   uint32_t Size,
                   uint64_t *EndOut) noexcept
    {
        auto End = static_cast<uint64_t>(Offset) + Size;
        auto Range = LocationRange::CreateWithEnd(Offset, End);

        if (!Map.getRange().containsLocRange(Range)) {
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
            IteratorInfo(const uint8_t *Iter, const uint8_t *End) noexcept
            : Iter(Iter), End(End) {}

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

            [[nodiscard]] inline bool isAtEnd() const noexcept {
                return (Iter >= End);
            }
        };

        struct Iterator {
        protected:
            IteratorInfo Info;
        public:
            explicit Iterator(const uint8_t *Iter, const uint8_t *End) noexcept
            : Info(Iter, End) {}

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

            [[nodiscard]] inline bool isAtEnd() const noexcept {
                return Info.isAtEnd();
            }

            [[nodiscard]]
            inline bool operator==(const EndIterator &) const noexcept {
                return isAtEnd();
            }

            [[nodiscard]]
            inline bool operator!=(const EndIterator &) const noexcept {
                return !isAtEnd();
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

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]] inline EndIterator end() const noexcept {
            return EndIterator();
        }
    };

    enum class SegmentFlagsEnum : uint32_t {
        HighVM             = 1 << 0,
        FixedVMLibary      = 1 << 1,
        NoRelocations      = 1 << 2,
        ProtectionVersion1 = 1 << 3,
        ReadOnlyAfterFixup = 1 << 4
    };

    [[nodiscard]] constexpr
    std::string_view SegmentFlagsGetName(SegmentFlagsEnum Val) noexcept {
        switch (Val) {
            using Enum = SegmentFlagsEnum;
            case Enum::HighVM:
                return "SG_HIGHVM";
            case Enum::FixedVMLibary:
                return "SG_FVMLIB";
            case Enum::NoRelocations:
                return "SG_NORELOC";
            case Enum::ProtectionVersion1:
                return "SG_PROTECTED_VERSION_1";
            case Enum::ReadOnlyAfterFixup:
                return "SG_READ_ONLY";
        }
    }

    [[nodiscard]] constexpr
    std::string_view SegmentFlagsGetDesc(SegmentFlagsEnum Val) noexcept {
        switch (Val) {
            using Enum = SegmentFlagsEnum;
            case Enum::HighVM:
                return "High Vm";
            case Enum::FixedVMLibary:
                return "Fixed Vm Library";
            case Enum::NoRelocations:
                return "No Relocations";
            case Enum::ProtectionVersion1:
                return "Protected (Version 1)";
            case Enum::ReadOnlyAfterFixup:
                return "Read-Only (After Fix-Ups)";
        }
    }

    struct SegmentFlags : public ::BasicFlags<SegmentFlagsEnum> {
    private:
        using Base = ::BasicFlags<SegmentFlagsEnum>;
    public:
        using FlagEnum = SegmentFlagsEnum;
        using FlagIntegerType = std::underlying_type_t<FlagEnum>;

        constexpr SegmentFlags() noexcept = default;
        constexpr SegmentFlags(FlagIntegerType Integer) noexcept
        : Base(Integer) {}

        [[nodiscard]] constexpr inline bool isHighVM() const noexcept {
            return hasFlag(FlagEnum::HighVM);
        }

        [[nodiscard]] constexpr inline bool isFixedVMLibary() const noexcept {
            return hasFlag(FlagEnum::FixedVMLibary);
        }

        [[nodiscard]] constexpr inline bool isNoRelocations() const noexcept {
            return hasFlag(FlagEnum::NoRelocations);
        }

        [[nodiscard]] constexpr inline bool isProtected() const noexcept {
            return hasFlag(FlagEnum::ProtectionVersion1);
        }

        [[nodiscard]]
        constexpr inline bool isReadOnlyAfterFixup() const noexcept {
            return hasFlag(FlagEnum::ReadOnlyAfterFixup);
        }

        constexpr inline SegmentFlags &setHighVM(bool Value = true) noexcept {
            setValueForFlag(FlagEnum::HighVM, Value);
            return *this;
        }

        constexpr
        inline SegmentFlags &setFixedVMLibary(bool Value = true) noexcept {
            setValueForFlag(FlagEnum::FixedVMLibary, Value);
            return *this;
        }

        constexpr
        inline SegmentFlags &setNoRelocations(bool Value = true) noexcept {
            setValueForFlag(FlagEnum::NoRelocations, Value);
            return *this;
        }

        constexpr
        inline SegmentFlags &setIsProtected(bool Value = true) noexcept {
            setValueForFlag(FlagEnum::ProtectionVersion1, Value);
            return *this;
        }

        constexpr
        inline SegmentFlags &setReadOnlyAfterFixup(bool Value = true) noexcept {
            setValueForFlag(FlagEnum::ReadOnlyAfterFixup, Value);
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

    constexpr std::string_view
    SegmentSectionKindGetName(SegmentSectionKind Type) noexcept {
        using Enum = SegmentSectionKind;
        switch (Type) {
            case Enum::Regular:
                return "S_REGULAR";
            case Enum::ZeroFillOnDemand:
                return "S_ZEROFILL";
            case Enum::CStringLiterals:
                return "S_CSTRING_LITERALS";
            case Enum::FourByteLiterals:
                return "S_4BYTE_LITERALS";
            case Enum::EightByteLiterals:
                return "S_8BYTE_LITERALS";
            case Enum::LiteralPointers:
                return "S_LITERAL_POINTERS";
            case Enum::NonLazySymbolPointers:
                return "S_NON_LAZY_SYMBOL_POINTERS";
            case Enum::LazySymbolPointers:
                return "S_LAZY_SYMBOL_POINTERS";
            case Enum::SymbolStubs:
                return "S_SYMBOL_STUBS";
            case Enum::ModInitFunctionPointers:
                return "S_MOD_INIT_FUNC_POINTERS";
            case Enum::ModTermFunctionPointers:
                return "S_MOD_TERM_FUNC_POINTERS";
            case Enum::CoalescedSymbols:
                return "Coalesced Symbols";
            case Enum::ZeroFillOnDemandGigaBytes:
                return "Zero-Fill (GigaBytes)";
            case Enum::InterposingFunctions:
                return "Interposing Functions";
            case Enum::SixteenByteLiterals:
                return "S_16BYTE_LITERALS";
            case Enum::DTraceDOF:
                return "S_DTRACE_DOF";
            case Enum::LazyDylibSymbolPointers:
                return "S_THREAD_LOCAL_REGULAR";
            case Enum::ThreadLocalRegular:
                return "S_THREAD_LOCAL_REGULAR";
            case Enum::ThreadLocalZeroFill:
                return "S_THREAD_LOCAL_ZEROFILL";
            case Enum::TheradLocalVariables:
                return "S_THREAD_LOCAL_VARIABLES";
            case Enum::ThreadLocalVariablePointers:
                return "S_THREAD_LOCAL_VARIABLE_POINTERS";
            case Enum::ThreadLocalInitFunctionPointers:
                return "S_THREAD_LOCAL_INIT_FUNCTION_POINTERS";
            case Enum::InitFunction32BitOffsets:
                return "S_INIT_FUNC_OFFSETS";
        }

        return std::string_view();
    }

    constexpr std::string_view
    SegmentSectionKindGetDescription(SegmentSectionKind Type) noexcept {
        using Enum = SegmentSectionKind;
        switch (Type) {
            case Enum::Regular:
                return "Regular";
            case Enum::ZeroFillOnDemand:
                return "Zerofill";
            case Enum::CStringLiterals:
                return "C-String Literals";
            case Enum::FourByteLiterals:
                return "Four-Byte Literal";
            case Enum::EightByteLiterals:
                return "Eight-Byte Literal";
            case Enum::LiteralPointers:
                return "Literal Pointers";
            case Enum::NonLazySymbolPointers:
                return "Non-Lazy Symbol-Pointers";
            case Enum::LazySymbolPointers:
                return "Lazy Symbol-Pointers";
            case Enum::SymbolStubs:
                return "Symbol Stubs";
            case Enum::ModInitFunctionPointers:
                return "Mod-Init Function Pointers";
            case Enum::ModTermFunctionPointers:
                return "Mod-Term Function Pointers";
            case Enum::CoalescedSymbols:
                return "Coalesced Symbols";
            case Enum::ZeroFillOnDemandGigaBytes:
                return "Zero-Fill (GigaBytes)";
            case Enum::InterposingFunctions:
                return "Interposing Functions";
            case Enum::SixteenByteLiterals:
                return "16-Byte Literals";
            case Enum::DTraceDOF:
                return "DTrace DOFs";
            case Enum::LazyDylibSymbolPointers:
                return "Lazy Dylib Lazy-Symbol Pointers";
            case Enum::ThreadLocalRegular:
                return "Thread-Local Regular";
            case Enum::ThreadLocalZeroFill:
                return "Thread-Local Zero-Fill";
            case Enum::TheradLocalVariables:
                return "Thread-Local Variables";
            case Enum::ThreadLocalVariablePointers:
                return "Thread-Local Variable-Pointers";
            case Enum::ThreadLocalInitFunctionPointers:
                return "Thread-Local Init-Function Pointers";
            case Enum::InitFunction32BitOffsets:
                return "32-Bit Init Function Offsets";
        }

        return std::string_view();
    }

    constexpr static inline auto SegmentSectionUserSettableAttributesMask =
        static_cast<uint32_t>(0xff000000);

    constexpr static inline auto SegmentSectionSystemSettableAttributesMask =
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
        using MaskKind = SegmentSectionAttributesMasks;
        using MaskIntegerType = std::underlying_type_t<MaskKind>;

        using Base::Base;

        [[nodiscard]]
        constexpr inline bool isAllInstructions() const noexcept {
            return hasValueForMask(MaskKind::IsAllInstructions);
        }

        [[nodiscard]]
        constexpr inline bool noRanlibTableOfContents() const noexcept {
            const auto NoRanlibTableOfContents =
                hasValueForMask(MaskKind::NoRanlibTableOfContents);

            return NoRanlibTableOfContents;
        }

        [[nodiscard]]
        constexpr inline bool shouldStripStaticSymbols() const noexcept {
            return hasValueForMask(MaskKind::StripStaticSymbols);
        }

        [[nodiscard]] constexpr inline bool noDeadStripping() const noexcept {
            return hasValueForMask(MaskKind::NoDeadStripping);
        }

        [[nodiscard]] constexpr inline bool hasLiveSupport() const noexcept {
            return hasValueForMask(MaskKind::LiveSupport);
        }

        [[nodiscard]]
        constexpr inline bool hasSelfModifyingCode() const noexcept {
            return hasValueForMask(MaskKind::SelfModifyingCode);
        }

        [[nodiscard]] constexpr inline bool isDebugSection() const noexcept {
            return hasValueForMask(MaskKind::IsDebugSection);
        }

        [[nodiscard]]
        constexpr inline bool hasSomeInstructions() const noexcept {
            return hasValueForMask(MaskKind::HasSomeInstructions);
        }

        [[nodiscard]]
        constexpr inline bool hasExternalRelocEntries() const noexcept {
            return hasValueForMask(MaskKind::HasExternalRelocEntries);
        }

        [[nodiscard]]
        constexpr inline bool hasLocalRelocEntries() const noexcept {
            return hasValueForMask(MaskKind::HasLocalRelocEntries);
        }

        constexpr inline SegmentSectionAttributes &
        setIsAllInstructions(bool Value = true) noexcept {
            setValueForMask(MaskKind::IsAllInstructions, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setNoRanlibTableOfContents(bool Value) noexcept {
            setValueForMask(MaskKind::NoRanlibTableOfContents, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setStripStaticSymbols(bool Value = true) noexcept {
            setValueForMask(MaskKind::StripStaticSymbols, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setNoDeadStripping(bool Value = true) noexcept {
            setValueForMask(MaskKind::NoDeadStripping, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &setLiveSupport(bool Value = true) noexcept {
            setValueForMask(MaskKind::LiveSupport, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setSelfModifyingCode(bool Value = true) noexcept {
            setValueForMask(MaskKind::SelfModifyingCode, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setIsDebugSection(bool Value = true) noexcept {
            setValueForMask(MaskKind::IsDebugSection, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setHasSomeInstructions(bool Value = true) noexcept {
            setValueForMask(MaskKind::HasSomeInstructions, Value);
            return *this;
        }

        constexpr inline SegmentSectionAttributes &
        setHasExternalRelocEntries(bool Value = true) noexcept {
            setValueForMask(MaskKind::HasExternalRelocEntries, Value);
            return *this;
        }

        constexpr inline
        SegmentSectionAttributes &
        setHasLocalRelocEntries(bool Value = true) noexcept {
            setValueForMask(MaskKind::HasLocalRelocEntries, Value);
            return *this;
        }
    };

    enum class SegmentSectionMasks : uint32_t {
        Attributes = 0xffffff00,
        Kind = 0x000000ff,
    };

    struct SegmentSectionFlags :
        public BasicMasksHandler<SegmentSectionMasks> {
    private:
        using Base = BasicMasksHandler<SegmentSectionMasks>;
    public:
        using MaskType = SegmentSectionMasks;
        using MaskIntegerType = std::underlying_type_t<MaskType>;

        using Kind = SegmentSectionKind;
        using Attributes = SegmentSectionAttributes;

        using Base::Base;

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
